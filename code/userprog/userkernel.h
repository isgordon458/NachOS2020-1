// userkernel.h
//	Global variables for the Nachos kernel, for the assignment
//	supporting running user programs.
//
//	The kernel supporting user programs is a version of the 
//	basic multithreaded kernel.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef USERKERNEL_H  
#define USERKERNEL_H

#include "kernel.h"
#include "filesys.h"
#include "machine.h"
#include "synchdisk.h"

class SynchDiskManager{
  bool bitMap[NumSectors];
  int nextEmptySector = 0;
  int emptySector = NumSectors;

public:
  SynchDiskManager(){
    for(int i=0; i<NumSectors; i++) bitMap[i] = false;
  }

  int numEmptySec() {return emptySector;}

  int getEmptySector() {

    emptySector--;
    ASSERT(emptySector >= 0);

    int searchEnd = nextEmptySector;
    do{
      if(bitMap[nextEmptySector]==false) {
        bitMap[nextEmptySector]=true;
        return nextEmptySector;
      }else
        nextEmptySector = (nextEmptySector+1)%NumSectors;
      
    }while(nextEmptySector!=searchEnd);

    cout << "Error:" << endl
         << "Disk is full" << endl;
    
    ASSERTNOTREACHED();
  }
  void setEmptySector(int sector) {
    ASSERT(0<=sector && sector < NumSectors);
    bitMap[sector] = false;
  }
};

enum {VicFIFO, VicLRU};

class SynchDisk;
class UserProgKernel : public ThreadedKernel {
  public:
    UserProgKernel(int argc, char **argv);
				// Interpret command line arguments
    ~UserProgKernel();		// deallocate the kernel

    void Initialize();		// initialize the kernel 

    void Run();			// do kernel stuff 

    void SelfTest();		// test whether kernel is working

// These are public for notational convenience.
    Machine *machine;
    FileSystem *fileSystem;

    SynchDisk *synchDisk;
    SynchDiskManager *synchDiskMnager;

  private:
    bool debugUserProg;		// single step user program
	Thread* t[10];
	char*	execfile[10];
	int	execfileNum;

  public:
  int vicStatus;

  public:
    int getExecFileNum() {return execfileNum;}
};

#endif //USERKERNEL_H
