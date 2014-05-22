# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
import sys, os

from ROOT import TTree, TFile
from array import array


#from HeapMon import MemoryScanner as memscanner
#from HeapMon import MemoryMarker as memmarker
#from Hephaestus import MemoryTracker as memtracker


class EventExecutionNotifier( PyAthena.Alg ):
    event_number = 0
    max_events = 0
    stop_scan  = 1
    start_scan = 0
    report_scan_number = 0
    file_name = "heapmon.freed.root"
    marker = 0xCD
    
    memmarker = 'HeapMon.MemoryMarker' in sys.modules and sys.modules[ 'HeapMon.MemoryMarker' ] or None 
    memscanner = 'HeapMon.MemoryScanner' in sys.modules and sys.modules[ 'HeapMon.MemoryScanner' ] or None
    memtracker = 'Hephaestus.MemoryTracker' in sys.modules and sys.modules[ 'Hephaestus.MemoryTracker' ] or None

    #memtracker = 'Hephaestus.MemoryTracker' in sys.modules and \
    #   sys.modules['Hephaestus.MemoryTracker' ] or None
    
    def __init__(self, name="EventExecutionNotifier"):
        ## init the base class
        super(EventExecutionNotifier,self).__init__()        
        return

    def initialize( self ):        
    # INITIALIZATION MARKING    
        print 'Notifier.initialize():'
        #memmarker = 'HeapMon.MemoryMarker' in sys.modules and \
        #sys.modules[ 'HeapMon.MemoryMarker' ] or None        
        #if memmarker:
        #    memmarker.start()        
        
        self.event_number = 0
        
        print "    file_name = ", self.file_name
        print "    self.max_events = ", self.max_events    
        print "    report_scan_number = ", self.report_scan_number 
        if self.memtracker:
            print('    starting memtracker - MemoryTracker ')
            self.memtracker.start()
        else:
            print('    memtracker - MemoryTracker not loaded!')

        if self.memscanner:
            #print'    memscanner.memscan()'
            self.memscanner.memscan(self.file_name, 2, self.event_number, self.marker); #SCAN_FLAG = 2: no holes statistics saved, just info
        else:
            print('    memscanner is not loaded!')   
            
    # Extract MallocInfo and put into root file    
        self.mallinfo()
        return PyAthena.StatusCode.Success
    
    def execute( self ):
        print "Notifier.execute()"        
        self.event_number +=1 
        print "Notifier.event_number = ", self.event_number
        
        if self.memscanner:
            print '    memscanner.memscan()'                
            if   (self.start_scan == 0):
                    self.memscanner.memscan(self.file_name, 2, self.event_number, self.marker); #SCAN_FLAG = 1:  holes statistics saved
            elif (self.start_scan == -1):
                    self.memscanner.memscan(self.file_name, 1, self.event_number, self.marker); #SCAN_FLAG = 2: no holes statistics saved, just info
            elif ((self.event_number >= self.start_scan) 
                    and (self.event_number <= self.stop_scan)):
                       self.memscanner.memscan(self.file_name, 1, self.event_number, self.marker); #SCAN_FLAG = 2: no holes statistics saved, just info
            else:
                    self.memscanner.memscan(self.file_name, 2, self.event_number, self.marker); #SCAN_FLAG = 1:  holes statistics saved
        else:
            print('    memscanner is not loaded!')
        
        
        self.mallinfo()
        
        if self.memtracker:
            if self.event_number <= self.stop_scan:
                print ('MemoryTracker is marking the memory')
            else:
                print("MemoryTracker configure() to stop the marking");
                print ("MemoryTracker.gFlags = ",
                    self.memtracker.configure(
                        self.memtracker.LEAK_CHECK | 
                        self.memtracker.QUICK )#| MemoryTracker.FILTER_STL)
                    )
        elif self.memmarker:
            if (self.start_scan == 0):
                print('EventNotifier.execute(): marker idling')      
            elif self.event_number <= self.stop_scan:
                    print 'EventNotifier.execute(): memmarker.start(), event_number=', self.event_number
                    self.memmarker.start() 
            else:
                print('Notifier.execute(): MemoryMarker.stop() , event_number=', self.event_number)
                self.memmarker.stop()
        else:
            print('Notifier.execute(): memmarker and memtracker are not loaded!')

        return PyAthena.StatusCode.Success
        
    def finalize( self ):
        self.msg.info( 'finalizing [%s]', self.name() )
        if self.memmarker:
            self.memmarker.stop()
        if self.memtracker:
            self.memtracker.stop()
        if self.memscanner:
            from HeapMon import HeapMonReport as memreport
            memreport.report()
        return PyAthena.StatusCode.Success


    def mallinfo(self):   
        if self.memmarker:
            print('Notifier.mallinfo(): malloc info') 
            (malloc_sbrk, malloc_mmap, malloc_inuse, malloc_free) = self.memmarker.get_malloc_info()
            sbrk =  array('I', [0]); sbrk[0] = malloc_sbrk;
            mmap =  array('I', [0]); mmap[0] = malloc_mmap;
            inuse = array('I', [0]); inuse[0] = malloc_inuse;
            free =  array('I', [0]); free[0] = malloc_free;
            print 'sbrk=', sbrk, ' mmap=', mmap, ' inuse=', inuse, 'free', free
            scan_number = array('L', [0]); scan_number[0] = self.event_number;
            memFile = TFile(self.file_name, "update")
            mallocTree = memFile.Get("mallocTree");
            mallocTree.SetBranchAddress("malloc_sbrk", sbrk);
            mallocTree.SetBranchAddress("malloc_mmap", mmap);
            mallocTree.SetBranchAddress("malloc_inuse",inuse);
            mallocTree.SetBranchAddress("malloc_free", free);
            mallocTree.SetBranchAddress("scan_number", scan_number);
            mallocTree.Fill()
            mallocTree.Write()
            mallocTree.Delete
            memFile.Write()
            memFile.Close()
            self.memmarker.show_info()
        else:
            print('Notifier.mallinfo(): memmarker is not loaded!')
