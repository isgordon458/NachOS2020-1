// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "translate.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------


class VictimSelector{
	int fifoCount=0;
	//char st;
public:	
	VictimSelector(){}//st = FIFO;}
  
	int getFIFO(){
		if (fifoCount == kernel->machine->last_use_frame) {
			fifoCount = (fifoCount + 1) % NumPhysPages;
		}
		int ret = fifoCount;
		fifoCount = (fifoCount + 1) % NumPhysPages;
		return ret;
	}
	
	int getLRU() {
		int min;
		int index;
		for (int i = 0; i < NumPhysPages; i++) {
			if (!AddrSpace::usedPhyPage[i]) {
				return i;
			}
			else if(!AddrSpace::invertedPageTable[i]->valid){
				return i;
			}
			else {
				if (i == 0) {
					min = AddrSpace::invertedPageTable[i]->last_use;
					index = 0;
				}
				else if(AddrSpace::invertedPageTable[i]->last_use < min){
					min = AddrSpace::invertedPageTable[i]->last_use;
					index = i;
				}
			}
		}

		return index;
	}

  int get(){

	  

    if(kernel->vicStatus == VicFIFO){
      return getFIFO();
	}
    else if(kernel->vicStatus == VicLRU){
    		cout << ".";
  return getLRU();
    }
  }
};

VictimSelector *vicSelctor = new VictimSelector;

void
ExceptionHandler(ExceptionType which)
{
	int	type = kernel->machine->ReadRegister(2);
	int	val;

    switch (which) {
	case SyscallException:
	    switch(type) {
		case SC_Halt:
		    DEBUG(dbgAddr, "Shutdown, initiated by user program.\n");
   		    kernel->interrupt->Halt();
		    break;
		case SC_PrintInt:
			val=kernel->machine->ReadRegister(4);
			cout << "Print integer:" <<val << endl;
			return;
/*		case SC_Exec:
			DEBUG(dbgAddr, "Exec\n");
			val = kernel->machine->ReadRegister(4);
			kernel->StringCopy(tmpStr, retVal, 1024);
			cout << "Exec: " << val << endl;
			val = kernel->Exec(val);
			kernel->machine->WriteRegister(2, val);
			return;
*/		case SC_Exit:
			DEBUG(dbgAddr, "Program exit\n");
			val=kernel->machine->ReadRegister(4);
			cout << "return value:" << val << endl;
			kernel->currentThread->Finish();
			break;
		default:
		    cerr << "Unexpected system call " << type << "\n";
 		    break;
	    }
	    break;
		

	case PageFaultException:
	{
		int victim = vicSelctor->get();
		unsigned int virPage = divRoundDown(kernel->machine->ReadRegister(BadVAddrReg), PageSize);
		//writ to disk

		if (AddrSpace::usedPhyPage[victim]) {

			cout << "Swap out" << endl;

			kernel->synchDisk->WriteSector(AddrSpace::invertedPageTable[victim]->diskSector, &(kernel->machine->mainMemory[victim * PageSize]));

		//memcpy( &(kernel->currentThread->space->virMem[virPage*PageSize]), &(kernel->machine->mainMemory[victim*PageSize]), PageSize);
		//adjust page table of victim
			AddrSpace::invertedPageTable[victim]->valid = false;
		//adjust page table of virPage
		}
		kernel->machine->pageTable[virPage].valid = true;
		kernel->machine->pageTable[virPage].physicalPage = victim;
		AddrSpace::invertedPageTable[victim] = &(kernel->machine->pageTable[virPage]);
		kernel->machine->last_use_frame = victim;
		kernel->machine->pageTable[virPage].last_use= TranslationEntry::time;
		TranslationEntry::time++;

		AddrSpace::usedPhyPage[victim] = true;

		//read from disk
		char *incomingPageData= new char[PageSize];
		//cout<<"read sector: "<<kernel->machine->pageTable[virPage].diskSector<<endl;

			cout << "Swap in" << endl;

		kernel->synchDisk->ReadSector(kernel->machine->pageTable[virPage].diskSector, incomingPageData);
		memcpy(&(kernel->machine->mainMemory[victim*PageSize]), incomingPageData, PageSize);
		
		//restart
		kernel->machine->WriteRegister(PCReg,kernel->machine->ReadRegister(PCReg) - 4);

		//kernel->currentThread->Sleep(false);
		/*    Page Fault Exception    */
		return;
	}	

	default:
	    cerr << "Unexpected user mode exception" << which << "\n";
		break;
    }
    ASSERTNOTREACHED();
}