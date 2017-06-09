#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// updated by Leigh Schaefer <leigh.schaefer@cern.ch> April 2017


#include "TRT_StrawMap.h"

int deadStraws[2][32][5482];

void initializeDeadStrawsList(); // intiaalize deadStraws, read in permanently dead list
void simpleAnalysis(std::string filename); // identify problematic straws
void printAthena( int run ); // print athena format	
void printAthenaBoardsOnly( int run ); // print athena format but only for entire boards 
void reportResults(std::string filename, int run ); // print for root plots	
									

bool m_checkNoisy_HT = false;
bool m_checkLoEff_HT = false;
bool m_checkNoisy_LT = false;
bool m_checkLoEff_LT = true; // check these by default
bool m_checkDead_LT  = true; // check these by default
bool m_onlyMaskBoards = false; // only mask entire dead boards if updating StatusPermanent

int main(int argc, char** argv){	

  strawMap straw_global(1, 1, 1);
  initializeDeadStrawsList();
  
  int run = atoi(argv[1]);
  
  //FILE *f = fopen("run.txt", "r"); assert(f); fscanf(f, "%d\n", &run); fclose(f);
  //printf("TRT_StrawStatusReport for run %d\n", run);
  std::string filename = "straws."+std::to_string(run)+".txt";
  std::cout<<filename<<std::endl;
  
  // options for which straws to mask
  for (int i=2; i<argc; ++i){
    if      (std::string(argv[i]) == "checkNoisy_HT") m_checkNoisy_HT=true;
    else if (std::string(argv[i]) == "checkLoEff_HT") m_checkLoEff_HT=true;
    else if (std::string(argv[i]) == "checkNoisy_LT") m_checkNoisy_LT=true;
    else if (std::string(argv[i]) == "checkLoEff_LT") m_checkLoEff_LT=true;
    else if (std::string(argv[i]) == "checkDead_LT" ) m_checkDead_LT= true;
    
    
    else if (std::string(argv[i]) == "ignoreNoisy_HT") m_checkNoisy_HT=false;
    else if (std::string(argv[i]) == "ignoreLoEff_HT") m_checkLoEff_HT=false;
    else if (std::string(argv[i]) == "ignoreNoisy_LT") m_checkNoisy_LT=false;
    else if (std::string(argv[i]) == "ignoreLoEff_LT") m_checkLoEff_LT=false;
    else if (std::string(argv[i]) == "ignoreDead_LT" ) m_checkDead_LT= false;
    
    else if (std::string(argv[i]) == "StatusPermanent") m_onlyMaskBoards=true;
    
    else {
      std::cout<<"\n Please use one or several of the following options: \n"
	       << "\n\t - StatusPermanent: to only mask entire dead boards -- for use if updating StatusPermanent "
	       << "\n\t\t - if this option is used, all others will be ignored. \n"
	       << "\n\t - checkNoisy_HT: to mask straws with HT fraction > 0.5 "
	       << "\n\t - checkNoisy_LT: to mask straws with LT fraction > 0.99 "
	       << "\n\t - checkLoEff_HT: to mask straws with HT fraction < 0.0001 "
	       << "\n\t - checkLoEff_LT: to mask straws with LT fraction < 0.7 "
	       << "\n\t - checkDead_LT : to mask straws with LT fraction = 0.0 "
	       << "\n\t Replace \"check\" with \"ignore\" to ignore this category. \n "
	       << std::endl;
      return 1;
    }
  }

  std::cout<< " checkNoisy_HT = "<< m_checkNoisy_HT
	   << "\n checkNoisy_LT = " << m_checkNoisy_LT
	   << "\n checkLoEff_HT = " << m_checkLoEff_HT
	   << "\n checkLoEff_LT = " << m_checkLoEff_LT
	   << "\n checkDead_LT  = " << m_checkDead_LT
	   << "\n StatusPermanent  = " << m_onlyMaskBoards
	   <<std::endl;
  
  simpleAnalysis(filename);	
  if (m_onlyMaskBoards) {
    printAthenaBoardsOnly(run);
  }
  else {
    printAthena(run);
    reportResults(filename, run);
  }
  
  return 1;	
}



void initializeDeadStrawsList(){
  // compare to input (defined below)
  // assume that input has same format as output:
  // bec sector straw strawlayer layer status
  
  for (int i=0; i<2; i++) 
    for (int j=0; j<32; j++) 
      for (int k=0; k<5482; k++) 
	deadStraws[i][j][k] = 0;
  
  int tmp[6]; //old length: 3
  int count = 0;
  
  std::string filename = "/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/tmp/Tier0/StrawStatusCode/TRT_StrawStatus_ExcludedStraws_2017.txt";
  
  FILE *f = fopen(filename.c_str(), "r"); 
  if (!f) {
    std::cout<<"TRT_StrawStatusReport::initializePermanentlyDead() ERROR: failed to open the input file "<< filename << std::endl;
    std::cout<<"TRT_StrawStatusReport::initializePermanentlyDead() ERROR: you need to fix the file name / link, WILL CRASH THE CODE NOW"<<std::endl;
    assert(0);					
  }
  while (fscanf(f, "%d %d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5)==6) {
    //  bec sector strawWithinLayer strawlayer layer status
    strawMap map(tmp[0], tmp[1], tmp[4], tmp[3], tmp[2]);
    if (tmp[5]>0) {
      deadStraws[(tmp[0]>0)?0:1][tmp[1]][map.straw()] = tmp[5];
      count++;
    } 
  }
  std::cout<<"read "<<count<<" permanently excluded straws from file "<< filename<<std::endl;
  fclose(f);		
  
  return;
}

void simpleAnalysis(std::string filename) {
  // input format of straws.RUNNUMBER.txt: 	
  // bec, phi, strawID, # hits, # track hits, # HT hits, # HT track hits, # holes, # holes with hit (hit not assigned to track)
  std::cout<<"simpleAnalysis: reading file "<< filename<<std::endl;
  FILE *f = fopen( filename.c_str(), "r" ); assert(f);
  int countlines(0), nevents(0), tmp[9];	
  
  while (fscanf(f, "%d %d %d %d %d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5, tmp+6, tmp+7, tmp+8)==9) { 
    
    if (tmp[0]==0&&tmp[1]==0&&tmp[2]==0&&tmp[3]==0&&tmp[4]==0&&tmp[5]==0&&tmp[6]==0&&tmp[7]==0) { nevents = tmp[8]; continue; }
    countlines++;
    
    double occupancy = 1. * tmp[3] / nevents;
    double HToccupancy = 1. * tmp[5] / nevents;
    double efficiency = (tmp[4]+tmp[7]>0) ? (1. * tmp[4] / (tmp[4]+tmp[7])) : 0.;
    
    int skip = 0;
    
    if (m_onlyMaskBoards && tmp[3]==0) skip=12;
    else {
      // ignore all other possible types of bad straw if we're masking dead boards
      
      if (HToccupancy>0.5    ) skip	 = 51; // high HT fraction straws
      if (HToccupancy<0.0001 ) skip	 = 52; // low HT fraction straws
      if (efficiency<0.7     ) skip	 = 42; // low efficiency LT straws
      if (occupancy>0.99     ) skip	 = 11; // 100% occupancy straws
      
      if (tmp[3]==0          )  skip = 12; // 0 hits
    }
    
    if (deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]] == 1 ){
      if (!m_onlyMaskBoards){
	// this assumes the same qualities were checked in the reference. 
	// if that is not true, you'll get a lot of print out. 
	// that's why we don't do this check for the board-level masking.
	if ( (tmp[3]==0 && m_checkDead_LT)
	     || (occupancy>0.99 && m_checkNoisy_LT)
	     || (efficiency<0.7 && m_checkLoEff_LT)
	     || (HToccupancy<0.0001 && m_checkLoEff_HT)
	     || (HToccupancy>0.5 && m_checkNoisy_HT   )
	     )
	  continue;
	std::cout<<"read ONE straw permanently masked is ALIVE!!! "<<tmp[0]<<", "<<tmp[1]<<", "<<tmp[2]<<", hits: "<<tmp[3]<<", occ: "<<occupancy<<", eff: "<<efficiency<<std::endl;
      }
    }
    
    if (skip && !deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]])  deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]] = skip;		
  }
  
  std::cout<<"read "<<countlines<<" lines from file "<<filename<<", N events: "<<nevents<<std::endl;
  fclose(f);
  
  return;
}

void printAthena( int run ) { // print athena format
  // dumps output/athenaFormat_runDependentInactiveStraws_runNUMBER.txt
  // this file is used to update conditions tags 
  // lists all problematic straws -- without comparing to reference 
  // types of problematic straws to mask can be set on the command line (see options in main)

  int count(0);
  int countDeadStraws[] = {0, 0};
  int breakDownOfDeadStraws[100][2]; 
  for (int i=0; i<100; i++) // different types of dead straw
    for (int j=0; j<2; j++) // barrel / end-caps	
      breakDownOfDeadStraws[i][j] = 0; 


  std::string filename = "output/athenaFormat_runDependentInactiveStraws_run"+std::to_string(run)+".txt";
  std::cout << filename << endl;
  FILE *fout = fopen(filename.c_str(), "w"); assert(fout);  
  
  for (int i=0; i<2; i++) {
    for (int j=0; j<32; j++) {
      for (int k=0; k<5482; k++) {
	if (!deadStraws[i][j][k]) continue; // ignore good straws for now
	
	int side = (i!=0 ? -1 : 1);
	if (k>=1642) side *= 2;
	
	strawMap map(side, j, k);
	
	if (deadStraws[i][j][k]>0) {
	  // for compatibility with upload (via txt2db_template.py)
	  // ONLY SET STRAWS DEAD THAT SHOULD BE CHECKED
	  // THIS SETS ALL TYPES OF DEAD THAT ARE CHECKED TO 1
	  if (deadStraws[i][j][k]==1)
	    fprintf(fout,  "%2d %2d %2d %2d %2d %2d\n", side, j, map.strawWithinLayer(), map.strawLayer(), map.layer(), 1);
	  if ((deadStraws[i][j][k]==11 && m_checkNoisy_LT)
	      || (deadStraws[i][j][k]==12 && m_checkDead_LT)
	      || (deadStraws[i][j][k]==42 && m_checkLoEff_LT)
	      || (deadStraws[i][j][k]==51 && m_checkNoisy_HT)
	      || (deadStraws[i][j][k]==52 && m_checkLoEff_HT)
	      )
	    fprintf(fout,  "%2d %2d %2d %2d %2d %2d\n", side, j, map.strawWithinLayer(), map.strawLayer(), map.layer(), 1);
	  
	  // do not dump to command-line straws that are already on the dead straw list (which will all be marked 1 due to previous line)
	  // but dump them to command line even if m_checkBLAH is false
	  if (deadStraws[i][j][k]>1) {
	    breakDownOfDeadStraws[deadStraws[i][j][k]][(k<1642)?0:1]++;		
	    count++;				
	    countDeadStraws[(fabs(side)==1)?0:1]++;
	  }
	}
      }
    }
  }
  

  printf("found %d newly dead straws compared to %s \n", count, filename.c_str());
  printf("N dead straws in TRT barrel: %4d or %4.1lf%%\n", countDeadStraws[0], (100.*countDeadStraws[0])/(64.*1642));
  printf("N dead straws in TRT end-caps: %4d or %4.1lf%%\n", countDeadStraws[1], (100.*countDeadStraws[1])/(64.*20*192));
  printf("overall: %4.2lf%%\n", 100.*count/350848.);

  // std::cout<<"found "<<count<<" newly dead straws compared to "<<filename<<std::endl; 
  // std::cout<<"N dead straws in TRT barrel: "<<countDeadStraws[0]<<" or "<< (100.*countDeadStraws[0])/(64.*1642) << "%% "<<std::endl; //%4.1lf%%
  // std::cout<<"N dead straws in TRT end-caps: "<<countDeadStraws[1]<<" or "<< (100.*countDeadStraws[1])/(64.*20*192) << "%%"<<std::endl; // %4.1lf%%
  // std::cout<<"overall: "<< 100.*count/350848. << "%%"<< std::endl; //%4.2lf%%
  fclose(fout);
  
  std::cout<<"criteria barrel end-cap"<<std::endl;
  for (int i=2; i<100; i++) 
    if (breakDownOfDeadStraws[i][0]+breakDownOfDeadStraws[i][1])
      printf("%2d %4d %4d\n", i, breakDownOfDeadStraws[i][0], breakDownOfDeadStraws[i][1]);
  
  return;
  
}


void printAthenaBoardsOnly( int run ) { // print athena format
  // dumps output/athenaFormat_runDependentInactiveStraws_runNUMBER.txt
  // ONLY FOR ENTIRE DEAD BOARDS
  // this file is used to update conditions tags 
  // lists all problematic straws -- without comparing to reference 
  // types of problematic straws to mask can be set on the command line (see options in main)

  int count(0);
  int countDeadStraws[] = {0, 0};

  std::string filename = "output/athenaFormat_DeadBoards_run"+std::to_string(run)+".txt";
  std::cout << filename << endl;
  FILE *fout = fopen(filename.c_str(), "w"); assert(fout);  
  
  for (int i=0; i<2; i++) {
    for (int j=0; j<32; j++) {
      std::vector<int> allStrawsOnBoard[29]; // (9  boards in barrel, 20 in endcap)
      std::vector<int> deadStrawsOnBoard[29]; 
      std::vector<int> newDeadStrawsOnBoard[29]; 
      
      for (int k=0; k<5482; k++) {
	int side = (i!=0 ? -1 : 1);
	
	if (k>=1642) side *= 2;
	strawMap map(side, j, k);
	int board = map.TTCgroup();
	
	// make a vector of all straws on a given board
	// and make a vector of dead straws on a given board
	// if the vector lengths match then the board is dead
	// and we can use the straw list in the vector to mask them
	
	allStrawsOnBoard[board].push_back(k);
	if (deadStraws[i][j][k]>0) deadStrawsOnBoard[board].push_back(k);
	if (deadStraws[i][j][k]>1) newDeadStrawsOnBoard[board].push_back(k);
      }
      
      for (int board=0; board<29; ++board){
	if (allStrawsOnBoard[board].size()==deadStrawsOnBoard[board].size()) {
	  // then the board is dead and we should mask all straws
	  
	  for (int it=0; it<deadStrawsOnBoard[board].size(); it++) {
	    int k = deadStrawsOnBoard[board].at(it);
	    int side = (i!=0 ? -1 : 1);
	    if (k>=1642) side *= 2;
	    strawMap map(side, j, k);
	    fprintf(fout,  "%2d %2d %2d %2d %2d %2d\n", side, j, map.strawWithinLayer(), map.strawLayer(), map.layer(), 1);
	    
	  }
	}
	
	if (allStrawsOnBoard[board].size()==newDeadStrawsOnBoard[board].size()) {
	  // then we found a new dead board
	  int side = (i !=0 ? -1 : 1);
	  std::cout<<"found a new dead board!!! "<<std::endl;
	  std::cout<<"See TRT_StrawMap.h for board ID info "<<std::endl;
	  std::cout<<"side: "<<side<<", phi: "<<j<<", board ID: "<<board<<std::endl<<std::endl;
	}
      }
    }
  }
  fclose(fout);
  return;
  
}

void reportResults(std::string filename, int run ) {
  // creates TRT_StrawStatusReport.txt
  // this file used to make histograms via TRT_StrawStatusReport.C

  FILE *f = fopen( filename.c_str(), "r" ); assert(f);
  int count(0), nevents(0), tmp[9];	
  FILE *fout = fopen("TRT_StrawStatusReport.txt", "w");
  fprintf(fout, "%d %d %d %d %d %lf %lf %lf %2d\n", 0, 0, 0, 0, run, 0., 0., 0., 0);		
  
  while (fscanf(f, "%d %d %d %d %d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5, tmp+6, tmp+7, tmp+8)==9) { 
    
    if (tmp[0]==0&&tmp[1]==0&&tmp[2]==0&&tmp[3]==0&&tmp[4]==0&&tmp[5]==0&&tmp[6]==0&&tmp[7]==0) { nevents = tmp[8]; continue; }
    count++;
    
    double occupancy = 1. * tmp[3] / nevents;
    double HToccupancy = 1. * tmp[5] / nevents;
    double efficiency = (tmp[4]+tmp[7]>0) ? (1. * tmp[4] / (tmp[4]+tmp[7])) : 0.;
    
    int side 	= int(tmp[0]); 
    int j	= int(tmp[1]); 
    int k	= int(tmp[2]); 
    strawMap map(side, j, k);
    fprintf(fout, "%d %d %d %d %d %lf %lf %lf %2d\n", tmp[0], tmp[1], tmp[2], deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]], tmp[4], occupancy, HToccupancy, efficiency,  map.layer() );		
  } 
  
  std::cout<<"reportResults: read "<<count<<" lines from file "<< filename <<", wrote to TRT_StrawStatusReport.txt"<<std::endl;
  fclose(f);
  fclose(fout);
  
  return;
}
