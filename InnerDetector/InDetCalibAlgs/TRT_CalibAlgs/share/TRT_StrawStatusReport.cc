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



#include "TRT_StrawMap.h"

int deadStraws[2][32][5482];

void initializeDeadStrawsList(); // intiaalize deadStraws, read in permanently dead list
void simpleAnalysis(char *filename); // identify problematic straws
void printAthena( int run ); // print athena format	
void reportResults( char *filename, int run ); // print for root plots	
									

int main(int argc, char** argv){	
	
	strawMap straw_global(1, 1, 1);
	initializeDeadStrawsList();
	
	int run = 0;
	run = atoi(argv[1]);
	
	//FILE *f = fopen("run.txt", "r"); assert(f); fscanf(f, "%d\n", &run); fclose(f);
	//printf("TRT_StrawStatusReport for run %d\n", run);
	char filename[300]; sprintf( filename, "straws.%d.txt", run );
	
	simpleAnalysis(filename);	
	printAthena(run);
	reportResults(filename, run);
	
	return 1;	
}



void initializeDeadStrawsList(){
	
	for (int i=0; i<2; i++) for (int j=0; j<32; j++) for (int k=0; k<5482; k++) deadStraws[i][j][k] = 0;
	
	int tmp[3];
	int count = 0;
	
	char filename[300]; sprintf(filename, "TRT_StrawStatus_PermanentlyExcludedStraws.txt");
	FILE *f = fopen(filename, "r"); 
	if (!f) {
		printf("TRT_StrawStatusReport::initializePermanentlyDead() ERROR: failed to open the input file %s \n", filename);
		printf("TRT_StrawStatusReport::initializePermanentlyDead() ERROR: you need to fix the file name / link, WILL CRASH THE CODE NOW\n");
		assert(0);					
	}
	while (fscanf(f, "%d %d %d\n", tmp, tmp+1, tmp+2)==3) {		
		deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]] = 1;
		count++;
	} 
	printf("read %d permanently excluded straws from file %s\n", count, filename);
	fclose(f);		
	
	return;
}

void simpleAnalysis(char *filename) {
	
	printf("simpleAnalysis: reading file %s\n", filename);
	FILE *f = fopen( filename, "r" ); assert(f);
	int count(0), nevents(0), tmp[9];	
	
	while (fscanf(f, "%d %d %d %d %d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5, tmp+6, tmp+7, tmp+8)==9) { 
		
		if (tmp[0]==0&&tmp[1]==0&&tmp[2]==0&&tmp[3]==0&&tmp[4]==0&&tmp[5]==0&&tmp[6]==0&&tmp[7]==0) { nevents = tmp[8]; continue; }
		count++;
		
		double occupancy = 1. * tmp[3] / nevents;
		double HToccupancy = 1. * tmp[5] / nevents;
		double efficiency = (tmp[4]+tmp[7]>0) ? (1. * tmp[4] / (tmp[4]+tmp[7])) : 0.;
		
		int skip = 0;
		if (HToccupancy>0.5) skip = 51; // high HT fraction straws
		if (HToccupancy<0.0001) skip = 52; // low HT fraction straws
		if (efficiency<0.7) skip = 42;
		if (occupancy>0.99) skip = 11; // 100% occupancy straws
		if (tmp[3]==0) skip = 12; // 0 hits		
		
		if ( tmp[0]==1 && (tmp[1]==28||tmp[1]==29) ) {
			strawMap straw_tmp(tmp[0], tmp[1], tmp[2]);
			if (straw_tmp.DTMROC()==7 ) skip = 12; // but not on the existing athena permanent list in 2011!!!
		}
		
		if (skip && !deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]]) deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]] = skip;		
	} 
	
	printf("read %d lines from file %s, N events: %d \n", count, filename, nevents);
	fclose(f);
	
	return;
}

void printAthena( int run ) { // print athena format
	
	int countDeadStraws[] = {0, 0};
	int breakDownOfDeadStraws[100][2]; for (int i=0; i<100; i++) for (int j=0; j<2; j++) breakDownOfDeadStraws[i][j] = 0; // barrel / end-caps	
	
	char filename[1000]; sprintf(filename, "output/athenaFormat_runDependentInactiveStraws_run%07d.txt", run );
	std::cout << filename << endl;
	FILE *fout = fopen(filename, "w"); assert(fout);  
	int count(0);
	
	for (int i=0; i<2; i++) for (int j=0; j<32; j++) for (int k=0; k<5482; k++) {
		
		breakDownOfDeadStraws[deadStraws[i][j][k]][(k<1642)?0:1]++;		
		
		if (!deadStraws[i][j][k]) continue;
		if (deadStraws[i][j][k]==1) continue; // do not write out straws that are already on the dead straw list
		
		int side = i ? -1 : 1;
		if (k>=1642) side *= 2;
		
		strawMap map(side, j, k);
		
		count++;				
		countDeadStraws[(fabs(side)==1)?0:1]++;
		
		fprintf(fout, "%2d %2d %2d %2d %2d %2d\n", side, j, map.strawWithinLayer(), map.strawLayer(), map.layer(), deadStraws[i][j][k]);
		// 
		// http://alxr.usatlas.bnl.gov/lxr-stb4/source/atlas/InnerDetector/InDetConditions/TRT_ConditionsServices/src/TRT_StrawStatusSummarySvc.cxx#497
		//		502       while( ifs >> bec >> sector>> straw >> strawlayer >> layer >> status ) {
	}
	printf("printed %d lines to %s\n", count, filename);
	printf("N dead straws in TRT barrel: %4d or %4.1lf%%\n", countDeadStraws[0], (100.*countDeadStraws[0])/(64.*1642));
	printf("N dead straws in TRT end-caps: %4d or %4.1lf%%\n", countDeadStraws[1], (100.*countDeadStraws[1])/(64.*20*192));
	printf("overall: %4.2lf%%\n", 100.*count/350848.);
	fclose(fout);
	
	printf("criteria barrel end-cap\n");
	for (int i=2; i<100; i++) if (breakDownOfDeadStraws[i][0]+breakDownOfDeadStraws[i][1]) printf("%2d %4d %4d\n", i, breakDownOfDeadStraws[i][0], breakDownOfDeadStraws[i][1]);
	
	return;
}

void reportResults( char *filename, int run ) {

	FILE *f = fopen( filename, "r" ); assert(f);
	int count(0), nevents(0), tmp[9];	
	FILE *fout = fopen("TRT_StrawStatusReport.txt", "w");
	fprintf(fout, "%d %d %d %d %d %lf %lf %lf\n", 0, 0, 0, 0, run, 0., 0., 0.);		

	while (fscanf(f, "%d %d %d %d %d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4, tmp+5, tmp+6, tmp+7, tmp+8)==9) { 
		
		if (tmp[0]==0&&tmp[1]==0&&tmp[2]==0&&tmp[3]==0&&tmp[4]==0&&tmp[5]==0&&tmp[6]==0&&tmp[7]==0) { nevents = tmp[8]; continue; }
		count++;
		
		double occupancy = 1. * tmp[3] / nevents;
		double HToccupancy = 1. * tmp[5] / nevents;
		double efficiency = (tmp[4]+tmp[7]>0) ? (1. * tmp[4] / (tmp[4]+tmp[7])) : 0.;
		
		fprintf(fout, "%d %d %d %d %d %lf %lf %lf\n", tmp[0], tmp[1], tmp[2], deadStraws[(tmp[0]>0)?0:1][tmp[1]][tmp[2]], tmp[4], occupancy, HToccupancy, efficiency);		
	} 
	
	printf("reportResults: read %d lines from file %s, wrote to TRT_StrawStatusReport.txt\n", count, filename );
	fclose(f);
	fclose(fout);
	
	return;
}
