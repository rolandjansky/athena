/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
MEMORY SCANNER
*/

// standard includes
#include <iostream>
using namespace std;

// C includes
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


//system includes
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>

// STL includes
#include <map>
#include <vector>
#include <iostream>
#include <iterator>

// ROOT includes
#include <TTree.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#ifdef __APPLE__
#define PTRACE_ATTACH PT_ATTACH
#define PTRACE_DETACH PT_DETACH
#endif

int usage( char* name ){
   cout << "usage: " << name <<" <pid> <scan_flag=0|1|2|3|4> <scan_number> <output_file_name> <marker-type(not-used-yet)>\n";
   return 2;
}


int main( int argc, char *argv[] ){
	typedef unsigned int ms_uint64;
	typedef unsigned int ms_uint32;

	int result = -1;
	FILE *maps = 0;
	FILE *statusf = 0;
	int mem = 0;
	pid_t pid = 0;

	unsigned short int SCAN_FLAG = 0;
	unsigned short int scan_number = 0;

	char mapfile[128], memfile[128], statusfile[128];
	char *buffer = NULL;

	ms_uint32 marker 	= 	0xCDCDCDCD; // new_marker = 	0xBCBCBCBC;
	ms_uint32 size_mark 	= 	0xFBFBFBFB;
	ms_uint64  addr_start = 0, addr_end = 0;

	siginfo_t *infop = NULL;
	char perm[3];


	// Parsing of arguments
	if ( argc != 6 )
		return usage( argv[0] );

#ifndef __APPLE__

	pid = atoi( argv[1] );
	SCAN_FLAG = atoi( argv[2]);
	scan_number = atoi (argv[3]);
	char* file_name  = argv[4];
	unsigned char marker_char = atoi(argv[5]); // "the color" of memory painting

	marker = (marker_char << 24) +
				(marker_char << 16) +
					(marker_char << 8) +
						marker_char; // 0xCD converted -> 0xCDCDCDCD
	cout << "  MemoryScanner: marker_char = " << marker_char << " marker = "
			<< marker << endl;
	cout << "  MemoryScanner: scan_flag = " << SCAN_FLAG << endl;
	switch ( SCAN_FLAG ) {
		   case 0 :
			   cout << "MemoryScanner: std output, no file output produced" ;
	       break;

	       case 1 :
			   cout << "MemoryScanner: full output of hole statistics and malloc/process memory info into " << file_name << "\n";
	       break;

	       case 2 :
	    	   cout << "MemoryScanner: malloc/process memory info output into " << file_name << "\n";
	       break;

	       default :
	       // Process for all other cases.
	    	cout << "MemoryScanner: ERROR - Wrong scan_flag=" << SCAN_FLAG <<" provided.\n";
	   		return usage( argv[0] );

	}

	cout <<"\n MemoryScanner: scan_number=" << scan_number <<" \n";


	//auxiliary buffer for reading bits from proc/mem file
	   const ms_uint64 aux_buffer_size = 1024; // 4096bytes >> 2 = 1024 size_t elements!
	   cout << "\n buffer_page_size =" <<  aux_buffer_size <<" bytes "<< endl;
	   size_t aux_buffer[aux_buffer_size];
	   //size_t mem_size = addr_end - addr_start;

	   //Declaration of counters
	   ms_uint64 counter = 0; //  marker pieces ('CDCDCDCD') counter
	   ms_uint64 block_counter = 0; //  marked blockes ('CDCD...CDCD') counter
	   ms_uint64 maprange_counter = 0;

	   //Declaration of size variables

	   ms_uint64  maps_total_size = 0; // Total maps in /proc/PID/maps
	   ms_uint64  mem_total_size = 0; //  Total scanned maps (excluding the ranges with no 'wr' permission)
	   ms_uint64  block_total_size = 0;// Total holes size
	   ms_uint64 mapsSize = 0, holesSize = 0; //memSize = 0, ;

	   ms_uint64 malloc_total = 0;

	   //Declaration of Marked block related variables
	   ms_uint64 block_flag = 0;
	   ms_uint64 block_size = 0;
	   ms_uint64  block_start = 0; // auxiliary to remember block start location
	   size_t* block_ptr = 0;
	   size_t* block_end_ptr = 0;
	   size_t* addr = 0;

	   //variables related to the TTree building
	   ms_uint64  hole_addr = (ms_uint64) (size_t) block_ptr;
	   ms_uint64  hole_size = (ms_uint64) (size_t) block_size;



	   //--------------------------------------------------//
	   // ATTACHING To the pid Process
	   // the %pid-Process restores operation after the MemoryScanner finishes it's job
	   //--------------------------------------------------//
	   cout << "using marker: " << marker << endl;
	   cout << "halting process: " <<  pid << endl;
	   result = ptrace( PTRACE_ATTACH, pid, NULL, NULL );
	   if ( result == -1 ) {
		  cerr << "	MemoryScanner: ERROR - cannot attach to the " << pid << " process \n";
	      exit( -1 );
	   }
	   waitid( P_PID, pid, infop, WSTOPPED );

	   //------------------------------------------------------//
	   // READING /proc/%PID/status file
	   //------------------------------------------------------//
	   char line [128]; /* or other suitable maximum line size */
	   char head_buffer[6];//char* head_buffer;
	   char tail_buffer[10];//char* tail_buffer;
	   snprintf( statusfile, 128, "/proc/%u/status", pid );
	   statusf = fopen ( statusfile, "r" );
	   cout << "opened /proc/" << pid << "/status file \n";

	   ms_uint32 tag_value;
	   ms_uint32 VmSize=0, VmLck=0, VmRSS=0, VmData=0, VmStk=0, VmExe=0, VmLib=0;

	   if ( statusf != NULL ){
		   while ( fgets ( line, sizeof line, statusf ) != NULL) /* read a line */{
			   sscanf( line, "%5s %80u %9s", head_buffer, &tag_value, tail_buffer);
			   //cout <<"h_buffer=" << head_buffer << " tag_value=" << tag_value << " t_buffer=" << tail_buffer << "\n";
			   if (strncmp(head_buffer, 		"VmSize",5) == 0){
				   VmSize = tag_value;
			   } else if (strncmp(head_buffer, 	"VmLck", 5) == 0){
				   VmLck = tag_value;
		   	   } else if (strncmp(head_buffer, 	"VmRSS", 5) == 0){
				   VmRSS = tag_value;
		   	   } else if (strncmp(head_buffer, 	"VmData",5) == 0){
				   VmData = tag_value;
		   	   } else if (strncmp(head_buffer, 	"VmStk", 5) == 0){
				   VmStk = tag_value;
		   	   } else if (strncmp(head_buffer, 	"VmExe", 5) == 0){
				   VmExe = tag_value;
		   	   } else if (strncmp(head_buffer, 	"VmLib", 5) == 0){
				   VmLib = tag_value;
		   	   }
		   }
		   fclose ( statusf );
	   }
	   else{
			perror ( statusfile); /* why didn't the file open? */
	   }
	   cout << "VmSize=" << VmSize << " VmRSS=" << VmRSS << " VmData=" << VmData
		   << "VmStk=" << VmStk << "VmExe=" << VmExe << "VmLib=" << VmLib << endl;



	   //---------------------------------------------------------//
	   // READING /proc/%PID/maps file
	   //---------------------------------------------------------//
	   snprintf( mapfile, 128, "/proc/%u/maps", pid );
	   maps = fopen( mapfile, "r" );
	   if( ! maps ) {
		   cerr << " MemoryScanner: ERROR - accessing /proc/" << pid <<"/maps file \n";
		   exit( -1 );
	   }
	   cout << "opened mapfile:" << mapfile << "\n";

	   //-------------------------------------------------------//
	   // Handle to /proc/%PID/mem file
	   //-------------------------------------------------------//
	   snprintf( memfile, 128, "/proc/%u/mem", pid );
	   mem = open( memfile, O_RDONLY, NULL );
	   if ( mem == -1 ) {
		   cerr << " MemoryScanner: ERROR - accessing /proc/" << pid << "/mem file \n";
		   exit( -1 );
	   }
	   cout << "opened memfile: " << memfile << endl;


	   // ROOT Declarations
	   TFile *memFile ;
	   TTree *memTree;
	   TTree *infoTree;

	   if (scan_number == 0){
	     cout << " Prepare the memFile = " << file_name << "\n";
		   TTree *mallocTree;

		   memFile = new TFile(file_name, "recreate");

		   // create hole statistics tree and it's branches:
		   // start_addr,end_addr, scan_number, marker_char
		   memTree = new TTree("holeTree","memory_holes_data");
		   memTree->Branch("hole_addr", &hole_addr,"hole_addr/i");
		   memTree->Branch("hole_size", &hole_size, "hole_size/i");
		   memTree->Branch("scan_number",&scan_number, "scan_number/s");
		   memTree->Branch("hole_marker", &marker, "marker/i");


		   // create process memory usage + hole info tree and it's branches:
		   infoTree = new TTree("infoTree","memory_usage_statistics");
		   infoTree->Branch("total_hole_memory", &holesSize,"hole_mem/i");
		   infoTree->Branch("total_hole_number", &block_counter,"hole_number/i");
		   infoTree->Branch("hole_marker", &marker, "hole_marker/i");
		   infoTree->Branch("total_maps_memory", &mapsSize, "maps_memory/i");
		   infoTree->Branch("VmSize", 	&VmSize,"vmsize/i");
		   infoTree->Branch("VmLck", 	&VmLck, "vmlck/i");
		   infoTree->Branch("VmRSS", 	&VmRSS,	"vmrss/i");
		   infoTree->Branch("VmData", 	&VmData,"vmdata/i");
		   infoTree->Branch("VmStk", 	&VmStk,	"vmstk/i");
		   infoTree->Branch("VmExe", 	&VmExe, "vmexe/i");
		   infoTree->Branch("VmLib", 	&VmLib, "vmlib/i");
		   infoTree->Branch("scan_number", &scan_number, "scan_number/s");

		   // create malloc info tree and it's branches:
		   mallocTree = new TTree("mallocTree","malloc_statistics");
		   mallocTree->Branch("malloc_sbrk",  &malloc_total, "sbrk/i");
		   mallocTree->Branch("malloc_mmap",  &malloc_total, "mmap/i");
		   mallocTree->Branch("malloc_inuse", &malloc_total, "inuse/i");
		   mallocTree->Branch("malloc_free",  &malloc_total, "free/i");
		   mallocTree->Branch("scan_number",  &scan_number,  "scan_number/s");

		   mallocTree->Fill();
		   mallocTree->Print();
		   mallocTree->Write();
	   } else {
	     cout << " Update the memFile = " << file_name << "\n";
		   memFile = TFile::Open(file_name, "update");
		   memTree = (TTree*)memFile->Get("holeTree");
		   infoTree = (TTree*)memFile->Get("infoTree");

		   //bind variables to the memTree  for further update/filling
		   memTree->SetBranchAddress("hole_addr", &hole_addr);
		   memTree->SetBranchAddress("hole_size",  &hole_size);
		   memTree->SetBranchAddress("scan_number",&scan_number);
		   memTree->SetBranchAddress("hole_marker", &marker);

		   //bind variables to the infoTree for further update/filling
		   infoTree->SetBranchAddress("total_hole_memory", &holesSize);
		   infoTree->SetBranchAddress("total_hole_number", &block_counter);
		   infoTree->SetBranchAddress("total_maps_memory", &mapsSize);
		   infoTree->SetBranchAddress("hole_marker", &marker);
		   infoTree->SetBranchAddress("VmSize", &VmSize);
		   infoTree->SetBranchAddress("VmLck", 	&VmLck);
		   infoTree->SetBranchAddress("VmRSS", 	&VmRSS);
		   infoTree->SetBranchAddress("VmData", &VmData);
		   infoTree->SetBranchAddress("VmStk", 	&VmStk);
		   infoTree->SetBranchAddress("VmExe", 	&VmExe);
		   infoTree->SetBranchAddress("VmLib", 	&VmLib);
		   infoTree->SetBranchAddress("scan_number", &scan_number);
	   }




	   //----------------------------------------------------//
	   //Main Part: The actual memory scanning, first read /proc/%pid/maps file
	   //Then scan /proc/%pid/mem for marked holes
	   //----------------------------------------------------//
	   size_t len = 0;
	   while (getline(&buffer, &len, maps) > 0) {
	      sscanf( buffer, "%80x-%80x %2s", &addr_start, &addr_end, perm );
	      maps_total_size += (addr_end - addr_start);

	      if ( strcmp( perm, "rw" ) != 0 ){
	         continue;
	      }
	      //cout << "addr_start=" << hex << addr_start << " addr_end=" << hex << addr_end << " prm=" << perm << "\n";

	      maprange_counter++;
	      mem_total_size += (addr_end - addr_start);
	      //mem_size = addr_end - addr_start;

	      addr = (size_t*) (unsigned long) addr_start;

		   switch ( SCAN_FLAG ) {
			   case 0 :
		    	 cout <<"Maps: " << hex << addr_start
					  << "-"	 << hex << addr_end
					  << " size=" << addr_end -addr_start << "/n";
		       break;

		       case 1 :
			    /*cout <<"Maps: " << hex << addr_start
						  << "-"	 << hex << addr_end
						  << " size=" << addr_end -addr_start << "/n";*/;
		       break;

		       case 2 :
		    	// cout << "Maps: " << hex << addr_start
		       break;

		       default :
		       ;// Process for all other cases.
		   }
	      //these are auxiliary flags and variables to define the-size of CDCDCDCD and PINNED blocks
	      //CD blocks parameters
	      block_flag = 0;
	      block_size = 0;
	      block_ptr = 0;
	      block_end_ptr = 0;
	      result = 0;

	    /* scan /proc/%pid/mem for marked holes */
	      while ( (ms_uint64) (size_t) addr < addr_end  &&  result != -1) {

				/*for (i=0;  i<aux_buffer_size; i++)
					aux_buffer[i] = 0;*/

				result = pread( mem, &aux_buffer, sizeof(aux_buffer), (ms_uint64) (size_t) addr );
				if (result == -1){
					//cerr << " MemoryScanner: ERROR - reading mem at address" << addr << "\t" <<  "error=" << errno << " " << strerror(errno) << "\n";
					continue ;
				} else if (result == 0){
					cerr << " MemoryScanner: WARNING - zero bytes read from mem at address" << addr <<"\n";
					continue ;
				}

				int  i;

				for (i=0; i<result; i++){
						if ( aux_buffer[i] == marker ) {
							//Here block_flag==0 defines the beginning of the 'CDCDCDCD' memory block
						   if (block_flag==0) {//cout << "found CDCDCDCD block: " << endl;
							   block_start = counter;
							   block_ptr = addr + i;
							   block_counter++; // 'CDCDCDCD' block counter
							   block_flag=1; // forthcoming consecutive 'CDCDCDCD' are not the beginning
						   }
						   counter ++; //counter of 'CDCDCDCD' pieces
						}else { //Here block_flag==1 defines the end of the 'CDCDCDCD' memory block

							//this condition for Tcmalloc marking of freed memmory
							if (block_flag == 1) {
								if (aux_buffer[i] == size_mark){
									if ((aux_buffer[i+2] == size_mark) && (aux_buffer[i+1]!=size_mark && aux_buffer[i+1] > 0)){
										if (aux_buffer[i+1] < 65000000)
											counter += aux_buffer[i+1]/4 - 2;
									}
								}
							   block_flag = 0;
							   block_size = counter - block_start;
							   block_end_ptr = block_ptr + block_size;
							   //block_size=block_size*sizeof(size_t);
							   block_total_size +=  block_size;
							   hole_addr = (ms_uint64 ) (size_t) block_ptr;
							   //hole_end_addr =   (ms_uint64 ) block_end_ptr;
							   //hole_size = hole_end_addr - hole_addr;
							   hole_size = block_size*sizeof(size_t);

							   switch ( SCAN_FLAG ) {
							     case 0 :
									   cout <<"C:\t" << hex << block_ptr <<"-" << hex << block_end_ptr ;
									   cout << " size=" << block_size;
									   cout << " counter="<< counter << "\n";
							     break;

							     case 1 :
									   memTree -> Fill();
							     break;
							     case 2 :
									  /*if(scan_number < report_scan_number)
										  memTree -> Fill()*/;
								 break;

							     default :
							       ;// Process for all other cases.
							   }

						   }
						}//if
					}//for
					addr += aux_buffer_size;
			  }//inner-while-end

	     //cout << "hs=" << dec << 4*block_total_size << " c=" << 4*counter << " map_range=" << hex << (size_t*) addr_start << "-" << (size_t*)addr_end << " map_size=" << dec << addr_end - addr_start << " len= "<< len<<"\n";
	     len = 1;
	   }//outer-while-end


//FINISHED SCANNING of the MEMORY
	   close( mem );
	   fclose( maps );
	   cout << "resuming process:" << pid << endl ;

	   result = ptrace( PTRACE_DETACH, pid, NULL, NULL );
	   if ( result == -1 ) {
		  cerr << "MemoryScanner: ERROR " << errno << " " << strerror(errno) << "\n";
	      exit( -1 );
	   }


	   switch ( SCAN_FLAG ) {
	     case 0 :
	       // Process for scan_flag = 0
			   cout <<"TOTAL FOUND for scan_number: " 			<< scan_number << "\n";
			   cout << maprange_counter << " MAP_RANGES scanned"<< "\n";
			   cout << block_counter 	<< " MARKED-Blocks" 	<< "\n";
			   cout << counter			<< " MARKED-pieces"		<< "\n";
			   cout << maps_total_size 	<< "(" << maps_total_size*1.0/(1048576.0)	<< " Mb)- TOTAL MAPS MEMORY SIZE   \n";
			   cout << mem_total_size 	<< "(" << mem_total_size*1.0/(1048576.0) 	<< " Mb)- TOTAL SCANNED MEMORY SIZE\n";
			   cout <<4*block_total_size<< "(" << 4.0*block_total_size/(1048576.0)	<< " Mb)- MARKED blocks MEMORY SIZE\n";
			   cout <<4*counter			<< "(" 	<< 4.0*counter/(1048576.0) 	 		<< " Mb)- MARKED pieces MEMORY SIZE\n";

	     break;

	     case 1: case 2 :
	       // Process for scan_flag = 1
			   cout <<"TOTAL FOUND for scan_number: " 			<< scan_number << "\n";
			   cout << maprange_counter << " MAP_RANGES scanned"<< "\n";
			   cout << block_counter 	<< " MARKED-Blocks" 	<< "\n";
			   cout << counter			<< " MARKED-pieces"		<< "\n";
			   cout << maps_total_size 	<< "(" << maps_total_size*1.0/(1048576.0)	<< " Mb)- TOTAL MAPS MEMORY SIZE   \n";
			   cout << mem_total_size 	<< "(" << mem_total_size*1.0/(1048576.0) 	<< " Mb)- TOTAL SCANNED MEMORY SIZE\n";
			   cout <<4*block_total_size<< "(" << 4.0*block_total_size/(1048576.0)	<< " Mb)- MARKED blocks MEMORY SIZE\n";
			   cout <<4*counter			<< "(" 	<< 4.0*counter/(1048576.0) 	 		<< " Mb)- MARKED pieces MEMORY SIZE\n";

			   memFile->cd();
			   memTree->Write();
			   mapsSize = maps_total_size/1024;
			   holesSize = 4*block_total_size/1024;
			   infoTree->Fill();
			   infoTree->Write();
			   memTree->Delete();
			   infoTree->Delete();
			   memFile->Close();
	      break;

	     default :
	       // Process for all other cases.
	    	 memTree->Delete();
	    	 infoTree->Delete();
	    	 memFile->Close();
	   }
#endif

	   return 0;
}


