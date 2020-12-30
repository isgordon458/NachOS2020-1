// synchdisk.h 
// 	Data structures to export a synchronous interface to the raw 
//	disk device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYNCHDISK_H
#define SYNCHDISK_H

#include "copyright.h"
#include "disk.h"
#include "synch.h"
#include "callback.h"

// The following class defines a "synchronous" disk abstraction.
// As with other I/O devices, the raw physical disk is an asynchronous device --
// requests to read or write portions of the disk return immediately,
// and an interrupt occurs later to signal that the operation completed.
// (Also, the physical characteristics of the disk device assume that
// only one operation can be requested at a time).
//
// This class provides the abstraction that for any individual thread
// making a request, it waits around until the operation finishes before
// returning.

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

class Semaphore;
class Lock;
class SynchDisk : public CallBackObj {
  public:
    SynchDisk(char* name);    		// Initialize a synchronous disk,
					// by initializing the raw Disk.
    ~SynchDisk();			// De-allocate the synch disk data
    
    void ReadSector(int sectorNumber, char* data);
    					// Read/write a disk sector, returning
    					// only once the data is actually read 
					// or written.  These call
    					// Disk::ReadRequest/WriteRequest and
					// then wait until the request is done.
    void WriteSector(int sectorNumber, char* data);
    
    void CallBack();			// Called by the disk device interrupt
					// handler, to signal that the
					// current disk operation is complete.



    //  similar to WriteSector, but won't suspend any thread
    //  used at the load time(addrSpace::Load())
    //  Note: it can be used when the user thread is loaded ONLY.
    void IntializeSector(int sectorNumber, char *data);
    void ReadIntializeSector(int sectorNumber, char *data);

  private:
    Disk *disk;		  		// Raw disk device
    Semaphore *semaphore; 		// To synchronize requesting thread 
					// with the interrupt handler
    Lock *lock;		  		// Only one read/write request
					// can be sent to the disk at a time

    bool initialize = false;

};

#endif // SYNCHDISK_H
