// synchdisk.cc 
//	Routines to synchronously access the disk.  The physical disk 
//	is an asynchronous device (disk requests return immediately, and
//	an interrupt happens later on).  This is a layer on top of
//	the disk providing a synchronous interface (requests wait until
//	the request completes).
//
//	Use a semaphore to synchronize the interrupt handlers with the
//	pending requests.  And, because the physical disk can only
//	handle one operation at a time, use a lock to enforce mutual
//	exclusion.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchdisk.h"


//----------------------------------------------------------------------
// SynchDisk::SynchDisk
// 	Initialize the synchronous interface to the physical disk, in turn
//	initializing the physical disk.
//
//	"name" -- UNIX file name to be used as storage for the disk data
//	   (usually, "DISK")
//----------------------------------------------------------------------

SynchDisk::SynchDisk(char* name)
{
    semaphore = new Semaphore("synch disk", 0);
    lock = new Lock("synch disk lock");
    disk = new Disk(name, this);
}

//----------------------------------------------------------------------
// SynchDisk::~SynchDisk
// 	De-allocate data structures needed for the synchronous disk
//	abstraction.
//----------------------------------------------------------------------

SynchDisk::~SynchDisk()
{
    delete disk;
    delete lock;
    delete semaphore;
}

//----------------------------------------------------------------------
// SynchDisk::ReadSector
// 	Read the contents of a disk sector into a buffer.  Return only
//	after the data has been read.
//
//	"sectorNumber" -- the disk sector to read
//	"data" -- the buffer to hold the contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::ReadSector(int sectorNumber, char* data)
{
    //cout << "  Want read" << kernel->currentThread->getName() <<endl;
    lock->Acquire();			// only one disk I/O at a time
    disk->ReadRequest(sectorNumber, data);

    //cout << "  Read Done, waiting for interrupt." << kernel->currentThread->getName() <<endl;

    semaphore->P();			// wait for interrupt
    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::WriteSector
// 	Write the contents of a buffer into a disk sector.  Return only
//	after the data has been written.
//
//	"sectorNumber" -- the disk sector to be written
//	"data" -- the new contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::WriteSector(int sectorNumber, char* data)
{
    //cout << "  Want write" << kernel->currentThread->getName() <<endl;

    lock->Acquire();			// only one disk I/O at a time
    disk->WriteRequest(sectorNumber, data);

    //cout << "  Write Done, waiting for interrupt." << kernel->currentThread->getName() <<endl;

    semaphore->P();			// wait for interrupt
    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::CallBack
// 	Disk interrupt handler.  Wake up any thread waiting for the disk
//	request to finish.
//----------------------------------------------------------------------

void
SynchDisk::CallBack()
{ 
    if(!initialize){
        //cout<<"w/r ready\n";
        semaphore->V();
    }
    else
        initialize = false;
    //semaphore->V();
}

void SynchDisk::IntializeSector(int sectorNumber, char *data) 
{
    initialize = true;
    disk->WriteRequest(sectorNumber, data);
}

void SynchDisk::ReadIntializeSector(int sectorNumber, char *data) 
{
    initialize = true;
    disk->ReadRequest(sectorNumber, data);
}