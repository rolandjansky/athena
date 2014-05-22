# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys;

memscan_output = 'heapmon.holes.root'
freescan_output = 'heapmon.freed.root'
newscan_output = 'heapmon.newed.root'

pid = os.getpid();
free_marker = 0xCD; 
new_marker = 0xBC;
scan_number = 0;
scan_flag = 1;
free_scan_number = 0;
new_scan_number = 0;

memmarker = 'HeapMon.MemoryMarker' in sys.modules and \
sys.modules[ 'HeapMon.MemoryMarker' ] or None

#meminfo_file_name = 'mem'
#print('    RunScanner is loaded: file name for memscan=', memscan_output)

def nmemscan(file_name, scan_flag, n, id, mrkr = 0xCD):
    global pid;
    
    try:
        os.kill(id, 0)
    except OSError, err:
        print("pid ", id , " doesn't exist, so pid stays os.getpid()=", pid)
        id = pid;
    
    print(' output_file ', file_name)
    print ' scan_flag = ', scan_flag
    print ' scan_number = ', n
    
    print('MemoryScanner.exe %d %d %d %s %d %s' %(id, scan_flag, n, file_name,  mrkr) );
    os.system( 'MemoryScanner.exe %d %d %d %s %d %s' %(id, scan_flag, n, file_name,  mrkr)  );
    
def imemscan():
    global pid, free_marker, scan_number, scan_flag, memscan_output  
    print('MemoryScanner.exe %d %d %d %s %d' %(pid, scan_flag, scan_number, memscan_output,  free_marker) );
    os.system( 'MemoryScanner.exe %d %d %d %s %d' %(pid, scan_flag, scan_number, memscan_output,  free_marker)  );
    scan_number+=1
    
def memscan(file_name, scan_f, scan_n, marker):
    global pid 
    print('MemoryScanner.exe %d %d %d %s %d' %(pid, scan_f, scan_n, file_name,  marker) );
    os.system( 'MemoryScanner.exe %d %d %d %s %d' %(pid, scan_f, scan_n, file_name,  marker)  );
    
def freescan(scan_flag):
    global free_scan_number, free_marker
    global freescan_output
    print ('MemoryScanner.freescan(', scan_flag, ')')
    print ' scan_flag = ', scan_flag
    print ' scan_number = ', free_scan_number
    #print ' RunScanner.memstat()'
    print('MemoryScanner.exe %d %d %d %s %d' %(pid, scan_flag, free_scan_number, freescan_output,  free_marker) );
    os.system( 'MemoryScanner.exe %d %d %d %s %d' %(pid, scan_flag, free_scan_number, freescan_output,  free_marker)  );

    free_scan_number+=1

def newscan(scan_flag):
    global new_scan_number, new_marker
    global newscan_output
    print ('MemoryScanner.newscan(', scan_flag, ')')
    print ' scan_flag = ', scan_flag
    print ' scan_number = ', new_scan_number
    #print ' RunScanner.memstat()'
    print('MemoryScanner.exe %d %d %d %s %d' %(pid, scan_flag, new_scan_number, newscan_output,  new_marker) );
    os.system( 'MemoryScanner.exe %d %d %d %s %d' %(pid, scan_flag, new_scan_number, newscan_output,  new_marker)  );
    new_scan_number+=1

