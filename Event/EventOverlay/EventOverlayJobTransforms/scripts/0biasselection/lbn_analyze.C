/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include "TRandom3.h"

class lbn_info{
public:
 lbn_info(){nevt=0; intlumi=0; nevtwanted=0;}
 int nevt;
 float intlumi;
 int nevtwanted;
};

class run_info{
public:
 run_info(){nevt=0; intlumi=0; nevtwanted=0;}
 int nevt;
 float intlumi;
 std::map<int,lbn_info> lbnmap;
 int nevtwanted;
};

FILE *outfile=fopen("lbn_anal_map.txt","w");
int debug=1;

std::map< int, int > eventcounts;
void readeventcounts(int run){
  eventcounts.clear();
  char buf[50];
  sprintf(buf,"lbnevents_%d.txt",run);
  if (debug) printf("Opening %s, eventcounts size is %lu\n",buf,eventcounts.size());
  FILE *fp = fopen(buf,"r");
  if (!fp) printf("Failed to open %s!!!\n",buf);
  char *line=new char[500];
  while (fgets(line,480,fp)) {
     int lbn,ne,nf,runn;
     float mb;
     int s=sscanf(&line[0]," ... Run %d, LB %d has %d events, %d RAW files and %f MB",&runn,&lbn,&ne,&nf,&mb);
     if (s>4){
       assert(run==runn);
       if (debug) printf("run %d lbn %d has %d events\n",run,lbn,ne);
       eventcounts[lbn]=ne;
     }
     else {
       printf("s=%d, bad read?\n",s);
       assert(false);
     }
  }
  if (debug) printf("Closing %s, eventcounts size is %lu\n",buf,eventcounts.size());
  fclose(fp); 
}

void lbn_analyze(int stream, int nwanted)
{
   FILE *fp = fopen("lbn","r");
   if (!fp) printf("Failed to open lbn input file!!!\n");
   char *line=new char[500];
   int run,lbn,L1Acc,beforeps,afterps,valid; //L1p,L2p,L3p
   int liveL1,livebp,liveap;
   float instlumi,dt,avevtperbx,live,L1p,HLTp,LARp;
   //float intlumi,intlumitrig;
   int grun=0;
   float gtotaltotallumi=0,gtotaltotallumiprescaled=0;
   float gtotallumi=0, gtotallumiprescaled=0;

   TRandom myrand;
   myrand.SetSeed(0);
   printf("- myrand seed is %u\n",myrand.GetSeed());
   myrand.SetSeed(myrand.Integer(10000000)+stream);
   printf("- myrand seed is now %u\n",myrand.GetSeed());

   std::map<int,run_info> runmap;

   while (fgets(line,480,fp)) {
     if (line[0]!='-') continue;
     
//   int s=sscanf(&line[0],"--- LumiCalculator      : %d[%d]: L1Acc: %d, Livetime trigger L1Acc: %d, InstLumi: %f, deltaT: %f, AvEvtsPerBX: %f, BeforePrescale: %d, AfterPrescale: %d, Livetime trigger BeforePrescale: %d Livetime trigger AfterPrescale: %d, Livefrac: %f, L1Presc: %d, L2Presc: %d, L3Presc: %d, Valid: %d", &run,&lbn,&L1Acc,&liveL1,&instlumi,&dt,&avevtperbx,&beforeps,&afterps,&livebp,&liveap,&live,&L1p,&L2p,&L3p,&valid);
     int s=sscanf(&line[0],"--- LumiCalculator      : %d[%d]: L1Acc: %d, Livetime trigger L1Acc: %d, InstLumi: %f, deltaT: %f, AvEvtsPerBX: %f, BeforePrescale: %d, AfterPrescale: %d, Livetime trigger BeforePrescale: %d Livetime trigger AfterPrescale: %d, Livefrac: %f, L1Presc: %f, HLTPresc: %f, Valid: %d, LAr ready fraction: %f", &run,&lbn,&L1Acc,&liveL1,&instlumi,&dt,&avevtperbx,&beforeps,&afterps,&livebp,&liveap,&live,&L1p,&HLTp,&valid,&LARp);

     if (s>8){
       if (debug) printf("Got %d values : run=%d, lbn=%d, L1Acc=%d, instlumi=%f, L1p=%f, dt=%f, live=%f, afterps=%d \n",s,run,lbn,L1Acc,instlumi,L1p,dt,live,afterps);
       
       if (run!=grun){
	 if (grun>0){//change of run
	   runmap[grun].intlumi=gtotallumi; printf("Setting lumi for run %d to %f\n",grun,gtotallumi);
	   if (debug) printf("Adding to totaltotal lumis\n");
	   gtotaltotallumi+=gtotallumi; gtotaltotallumiprescaled+=gtotallumiprescaled;
	   gtotallumi=0;gtotallumiprescaled=0;//reset the int lumi calculation for this run
	   //runmap[grun].nevt=0;
	 }
	 grun=run;//save the run number
	 printf("Setting grun to %d\n",run);

	 //read in event counts from runquery file for run
	 readeventcounts(run);
 
       }//new run
       
       if (L1p<0) {
	 runmap[run].lbnmap[lbn].nevt=0;
	 if (eventcounts[lbn]!=0) printf("For lbn %d, L1p<0 but eventcounts is %d\n",lbn,eventcounts[lbn]);
       }
       else{
	 int ne = L1Acc;//afterps
	 if (ne!=eventcounts[lbn]){
	   if (0==stream) printf("For lbn %d, ne from lumicalc is %d but we will trust runquery value of %d\n",lbn,ne,eventcounts[lbn]);
	   ne=eventcounts[lbn];
	 }
	 if (0==ne){dt=0; printf("For lbn %d, setting lumi to 0 since 0 events were recorded\n",lbn);}
	 runmap[run].lbnmap[lbn].nevt=ne; 
	 runmap[run].lbnmap[lbn].intlumi=instlumi*dt*live;
	 runmap[run].nevt+=ne;
	 gtotallumiprescaled+= instlumi*dt*live/L1p;
	 gtotallumi+= instlumi*dt*live;
       }
       if (debug) printf("grun=%d, gtotallumi=%f, gtotallumiprescaled=%f\n",grun,gtotallumi,gtotallumiprescaled);
     }//good line
   }//loop over lines in file
   
   //after last run
   runmap[grun].intlumi=gtotallumi; printf("Setting lumi for run %d to %f\n",grun,gtotallumi);
   if (debug) printf("Adding to totaltotal lumis for last run\n");
   gtotaltotallumi+=gtotallumi; gtotaltotallumiprescaled+=gtotallumiprescaled;

   fclose(fp);
   printf("- %lu runs, gtotaltotallumi=%f, gtotaltotallumiprescaled=%f\n",runmap.size(),gtotaltotallumi,gtotaltotallumiprescaled);

   if (runmap.size()<1) {printf("- runmap size is %lu, quitting!\n",runmap.size()); return;}
   //return;

   //check the total lumi...
   double tempr=0;
   for (std::map<int,run_info>::const_iterator r=runmap.begin(); r!=runmap.end(); ++r){
     tempr += r->second.intlumi;
   }
   if (fabs(tempr-gtotaltotallumi)/tempr>0.001){
     printf("tempr=%f and gtotaltotallumi=%f\n",tempr,gtotaltotallumi);
     assert(false);
   }
   
   for (int e=0;e<nwanted;++e){     
     
     //pick a random run, proportional to intlumi
     double rnd = myrand.Uniform(tempr);
     run=-1;
     for (std::map<int,run_info>::const_iterator r=runmap.begin(); r!=runmap.end(); ++r){
       if (rnd<r->second.intlumi) {run=r->first; break;}
       rnd -= r->second.intlumi;
     }
     assert(run>=0);

     //check the total run lumi...
     double tempt=0;
     for (std::map<int,lbn_info>::const_iterator l=runmap[run].lbnmap.begin(); l!=runmap[run].lbnmap.end(); ++l){
       tempt += l->second.intlumi;
     }
     if (fabs(tempt-runmap[run].intlumi)/tempt>0.001){
       printf("tempt=%f and runmap[%d].intlumi=%f\n",tempt,run,runmap[run].intlumi);
       assert(false);
     }

     //pick a random lbn, proportional to intlumi
     rnd = myrand.Uniform(tempt);
     lbn=-1;
     for (std::map<int,lbn_info>::const_iterator l=runmap[run].lbnmap.begin(); l!=runmap[run].lbnmap.end(); ++l){
       if (rnd<l->second.intlumi) {lbn=l->first; break;}
       rnd -= l->second.intlumi;
     }
     assert(lbn>=0);
     
     runmap[run].nevtwanted++; runmap[run].lbnmap[lbn].nevtwanted++;
     if (debug) printf("- stream %d, run %d, lbn %d, choose %d of out %d\n",stream,run,lbn,runmap[run].lbnmap[lbn].nevtwanted,runmap[run].lbnmap[lbn].nevt);

   }//loop over nwanted

   for (std::map<int,run_info>::const_iterator r=runmap.begin(); r!=runmap.end(); ++r){
     int totnevt=0; float totintlumi=0;
     printf("stream %d, run %d, has %d events and %f/ub, %f intlumi of total, and %d wanted\n", stream, 
        r->first,r->second.nevt,r->second.intlumi,r->second.intlumi/gtotaltotallumi,r->second.nevtwanted);
     for (std::map<int,lbn_info>::const_iterator l=r->second.lbnmap.begin(); l!=r->second.lbnmap.end(); ++l){
       fprintf(outfile,"stream %d, run %d, lbn %d, has %d events and %f/ub, %f intlumi of run, and %d wanted", stream, r->first, 
        l->first,l->second.nevt,l->second.intlumi,l->second.intlumi/r->second.intlumi,l->second.nevtwanted);
       if (l->second.nevtwanted>l->second.nevt) fprintf(outfile," : WARNING, more than available, will be duplicates!\n"); else fprintf(outfile,"\n");
       totnevt+=l->second.nevt; totintlumi+=l->second.intlumi;
     }
     if (totnevt!=r->second.nevt) printf("  XXX events do not agree !!! \n");
     if (totintlumi>0.0 && fabs(totintlumi - r->second.intlumi)/totintlumi>.001) printf("  XXX intlumi does not agree !!! %f %f \n",totintlumi,r->second.intlumi);

   } 
   printf("--\n\n");
}

