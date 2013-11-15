#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <unistd.h>

using namespace std;


int main(int argc, char *argv[]) {

  int nfiles=argc-2;
  
  int m_accumulateHits[2][32][5482][6]; // same as in the athena package TRT_StrawStatus.cxx
  int nevents = 0;
  for (int i=0; i<2; i++) for (int j=0; j<32; j++) for (int k=0; k<5482; k++) for (int m=0; m<6; m++) m_accumulateHits[i][j][k][m] = 0;
		
  for (int irun = 0; irun<nfiles; irun++) {

    char filename[1000];
    sprintf(filename, "%s", argv[irun+2]); 
			
    printf("reading file %s \n", filename); // debug printout
    FILE *f = fopen( filename, "r");
    if (f==0) {  printf("file %s missing\n", filename); continue; }
			
    int tmp[10];
    int count(0);
    int nEventsInOneFile = 0;
    while (fscanf(f, "%d %d %d %d %d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5, tmp+6, tmp+7, tmp+8)==9) { 

//	printf("LINE \n");				
      if (tmp[0]==0&&tmp[1]==0&&tmp[2]==0&&tmp[3]==0&&tmp[4]==0&&tmp[5]==0&&tmp[6]==0&&tmp[7]==0) { nEventsInOneFile = tmp[8]; nevents += tmp[8]; continue; }
      count++;
				
      for (int k=0; k<6; k++) m_accumulateHits[(tmp[0]>0)?0:1][tmp[1]][tmp[2]][k] += tmp[3+k];
      if (tmp[3]>nEventsInOneFile) printf("warning: problem with N events? %d %d\n", tmp[5], nEventsInOneFile);
    } 
    fclose(f);
    printf("read %7d lines from file %s (N events: %d) \n", count, argv[irun+2], nEventsInOneFile);
  } // end loop over input files

  printf("read %d events from %d files\n", nevents, nfiles);
		
  FILE *f = fopen(argv[1], "w"); // now print out merged file
  if (f==0) { printf("could not open file %s for writing, EXIT", argv[1]); return 0; }
  printf("now writing merged file: %s\n", argv[1]);
  for (int i=0; i<8; i++) fprintf(f, "%d ", 0); fprintf(f, "%d\n", nevents); 
  for (int i=0; i<2; i++) for (int j=0; j<32; j++) for (int k=0; k<5482; k++) {
    int side = i ? -1 : 1;
    if (k>=1642) side *= 2;
    fprintf(f, "%d %d %d", side, j, k);
    for (int m=0; m<6; m++) fprintf(f, " %d", m_accumulateHits[i][j][k][m] ); fprintf(f, "\n");
  }
  fclose(f);
	
  return 1;
}

