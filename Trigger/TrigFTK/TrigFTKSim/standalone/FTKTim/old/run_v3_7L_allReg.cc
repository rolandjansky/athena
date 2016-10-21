/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
using namespace std;
#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TClassTable.h"


//#include "NtupleHeader/ftk_track_ntuple_v2.h"
#include "NtupleHeader/ftk_track_merge.h" // for merged tracks
#include "NtupleHeader/ftk_road.h"
//#include "NtupleHeader/ftk_wrap2root_ntuple.h"

//#include "FTKTimeSim_Mod/include/FTKTimeSim.hh"
//#include "FTKTimeSim_Mod/include/IBoardBase.hh"
//#include "FTKTimeSim_Mod/FTKTimeSim.cc"

//int debug = 1;
//#define debug 1
#define FTKTimeSim_Debug 3                // duplicate with STKTimeSim  3 summary 4 full
//#define NASSOCIATIVEMEMORY 12             // duplicate with STKTimeSim
#define NASSOCIATIVEMEMORY 4       // new board            // duplicate with STKTimeSim
#define NREGION 8                         // duplicate with STKTimeSim
#define NLAYER 11 // pixel 0-2 sct 3-10   // duplicate with STKTimeSim
#define NLAYERA 7 // pixel 0-2 sct 3-10   // duplicate with STKTimeSim

#define L1rate 100   //kHz (100 kHz) LVL1 rate
#define SOVERLAP 1 // 1 : consider events overlap
//#define time_kill_TF_proc 10000000 // 10 msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
//#define time_kill_TF_proc 2000000 // 2msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
//#define time_kill_TF_proc 500000 // 0.5msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
#define time_kill_TF_proc 1000000 // 1msec // if 0 : no kill. nsec for kill signal for TF, if tf proc time over this time, process will killed
//#define time_kill_TF_proc 0 // nokill



int run_v3_7L_allReg(){
  cout<<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"<<endl;
  cout<<"H      FTK timing study.         H"<<endl;
  cout<<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"<<endl;
  TStopwatch timer;
  timer.Start();
  //  int nrbin=100;
  //  int nrbin=5; // for v14 first
  //  int nrbin=4; // 3E34?
  //  int nrbin=10; // normal
  //  int nrbin=4; // sub
  int nrbin=1; // sub new 8l 32x19

  int switchEvent=3;  // 0:VBFtautaulh 3E 1: VBF 1E 2:DiJet 3E 3: DiJet 1E
  
  


  int nloop = 10;  // # events for loop (0 is full)
  int debug = 0;
  int s_plot = 1; // 1 : plot,  0 : no plot
  int dump_event = 1; // event # for dump information (0 non)
  //  int n_dump=7;
  //  int array_dump_event[]={0,1,64,65,15,155};
  int n_dump=2;
  int array_dump_event[]={1,46};
  int dump; 
  double sf_hit_overlapregion = 1.43; // for use hit/sf, to minimum overlap region study for DO hits. // 1 is no sf // 1.43 from Anton
  int s_split = 0;
  double sec_sf = 1; // if 2 : secstage road and fit is 1/2 of first stage //
  int nloopsloop = 1; // def=1; if nloop!=0, it is not work //for loop of event loop for check overlap 
  int sub =1; // use subregion reduction? 
  int first_stage_layer = 7;  // artitecture

   // normal region
   double sf_nregion = 1;
   double sf_nregion_hit = 1;
   double hit_div[NLAYER] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

  // 8 sub region
   if(sub){
     //     sf_nregion = 1./8.; // road and fit doesnot have overlap
     //     sf_nregion = 1./16.; // road and fit doesnot have overlap //temp!!!! for 8LTSP
     //     if(first_stage_layer==7) sf_nregion = 1./16.; // one subregion have 2 AM board

     sf_nregion = 1./(6.68*2.); // road and fit doesnot have overlap  // From Anton for 8LTSP
     //     sf_nregion = 1./6.68; // road and fit doesnot have overlap  // From Anton

     if(first_stage_layer==7) sf_nregion = 1./(6.68*2.); // one subregion have 2 AM board. hit factor.
     //     if(first_stage_layer==7) sf_nregion = 1./(6.68*4.); // for 11x8x36

     //     sf_nregion_hit = ((360./16.+10.) / (45.+10.)); // 360/X's X is number of region
     sf_nregion_hit = 1.;
     //     hit_div[0] = {2.0, 2.3, 2.5, 3.1, 3.1, 3.4, 3.4, 3.7, 3.7, 4.0, 4.0};
//      hit_div[0] = 2.0;
//      hit_div[1] = 2.3;
//      hit_div[2] = 2.5;
//      hit_div[3] = 3.1;
//      hit_div[4] = 3.1;
//      hit_div[5] = 3.4;
//      hit_div[6] = 3.4;
//      hit_div[7] = 3.7;
//      hit_div[8] = 3.7;
//      hit_div[9] = 4.0;
//      hit_div[10] = 4.0;
     hit_div[0] = 2.26;   //hit factor for 11Layer
     hit_div[1] = 2.94;
     hit_div[2] = 3.6;
     hit_div[3] = 4.68;
     hit_div[4] = 4.68;
     hit_div[5] = 4.65;
     hit_div[6] = 4.65;
     hit_div[7] = 5.28;
     hit_div[8] = 5.28;
     hit_div[9] = 5.76;
     hit_div[10] = 5.76;
 }
  //  gSystem->Load("/scratch/kimura/FTK/FTK_Sim1/FTKSim_6_NaokiN/standalone/libftk_classes.so");
  //  gSystem->Load("/scratch/kimura/FTK/FTK_Sim1/FTKSim_5_SepaTSP/standalone/libftk_classes.so");
  //  For Warning in <TClass::TClass>: no dictionary for class FTKTrackStream is available  etc
  
   
   if (!TClassTable::GetDict("FTKTimeSim")) {
     gSystem->Load("FTKTimeSim_Mod/share/libFTKTimeSim.so");
   }
  

  // ***** file read start *****//  ------------------------------------------------------------------------------
  // *** parameters ***//
  //-------------------//

  //output directre name
  char description2[64];
  if(switchEvent==3)sprintf(description2,"test");
  
  
  // path name
  char description[64];
  char raw11[128];
  int  rootarray=0;
  

  
  //-------------------//

  int divi=0;
  int temp_denom=1000;
  //  int temp_denom=1;
  // ================================= after this line, no setting parameter ================================//
  char temp_outdir[256];
  sprintf(temp_outdir,"mkdir -p output/%s",description2); //output directry
  system(temp_outdir);



  TChain *chain_road[NREGION];
  char file_road[NREGION][999];
  ftk_road *road[NREGION];

  TChain *chain_trk[NREGION];
  char file_trk[NREGION][999];
  ftk_track_merge *trk[NREGION]; 

  TChain *chain_road2[NREGION];
  char file_road2[NREGION][999];
  ftk_road *road2[NREGION];

  TChain *chain_trk2[NREGION];
  char file_trk2[NREGION][999];
  ftk_track_merge *trk2[NREGION];


  if(sf_hit_overlapregion!=1) cout<<"ooooooo # hits is corrected by "<<sf_hit_overlapregion<<endl;
  // ** chain for all sub region ** //

  // Chain Root File //



  

    sprintf(description,"merge.v2.120101032729");
    sprintf(raw11,"user10.YangyangCheng.v14jetjetJ3.set01_RECO1E34.rset13.20091114");
       
    

    for(int i=0 ; i<NREGION ; i++){
      chain_road[i]  = new TChain("ftkdata");
      chain_trk[i]   = new TChain("ftkdata");
      chain_trk2[i]  = new TChain("ftkdata");
      chain_road2[i] = new TChain("ftkdata");
    }
    
    for(int i=0 ; i<NREGION ; i++){
      
      //      sprintf(file_road[i], "./FTKSimOutput/user.chiba.raw_7L_24x20x36_%s.%s/raw_7L_24x20x36_%s_np4000000/merged/%d/ftkroads_%s.EXT0.merge.root",raw11,description, raw11,i,raw11);     
      sprintf(file_road[i], "/afs/cern.ch/user/k/kimura/work/ftk_data/data/output/raw_8Lc_24x20x36_16m_np40000000/merged/%d/ftkroads_user.tohya.000705.EXT0._00001.satoshi_20000ev_BeamCondSvc.x2.y2.z0.00001_ftksim_wrap_raw.root",i);
      chain_road[i]->Add(file_road[i]);
      
      //      sprintf(file_trk[i],  "./FTKSimOutput/user.chiba.raw_7L_24x20x36_%s.%s/raw_7L_24x20x36_%s_np4000000/tracks_merge/%d/ftktracks_%s.EXT0.merge.root",raw11,description, raw11,i,raw11);
      sprintf(file_trk[i],  "/afs/cern.ch/user/k/kimura/work/ftk_data/data/output/raw_8Lc_24x20x36_16m_np40000000/tracks_merge/8/ftktracks_user.tohya.000705.EXT0._00001.satoshi_20000ev_BeamCondSvc.x2.y2.z0.00001_ftksim_wrap_raw.root",i);
      chain_trk[i]->Add(file_trk[i]);     
      
      sprintf(file_road2[i], "/afs/cern.ch/user/k/kimura/work/ftk_data/data/output/raw_8Lc_24x20x36_16m_np40000000/merged/%d/ftkroads_user.tohya.000705.EXT0._00001.satoshi_20000ev_BeamCondSvc.x2.y2.z0.00001_ftksim_wrap_raw.root",i);
      chain_road2[i]->Add(file_road2[i]);
      
      sprintf(file_trk2[i],  "/afs/cern.ch/user/k/kimura/work/ftk_data/data/output/raw_8Lc_24x20x36_16m_np40000000/tracks_merge/8/ftktracks_user.tohya.000705.EXT0._00001.satoshi_20000ev_BeamCondSvc.x2.y2.z0.00001_ftksim_wrap_raw.root",i);
      chain_trk2[i]->Add(file_trk2[i]);      
    }

    
    for(int i=0 ; i<NREGION ; i++){
      road[i]  = new ftk_road(chain_road[i]);
      trk[i]   = new ftk_track_merge(chain_trk[i]);
      road2[i] = new ftk_road(chain_road2[i]);  
      trk2[i]  = new ftk_track_merge(chain_trk2[i]);
    }











  if(chain_trk[0]->GetEntries()!=chain_trk[1]->GetEntries() || 
     chain_trk[0]->GetEntries()!=chain_road[0]->GetEntries() ){
    cout<<"ERROR : file read error. Entries is not much between region files!"<<endl;
    return -1;
  }  
  // **** file read end -------------------------------------------------------------------------

  // ** set for loop ** //
  int nentries = chain_trk[0]->GetEntries();
  int primary_nentries = nentries;
  nentries=nentries*nloopsloop;
  if(nloop) nentries = nloop;
  int preb_combs[NREGION]={0};

 

  FTKTimeSim* sim = new FTKTimeSim();
  

  // ** temp parameter for buffer study **//
  int nroad_rw_integ_temp[NREGION]={0}, nroad_norw_integ_temp[NREGION]={0};// ..cofusing norw is after rw..... // no use nroad_rw....
  int nroad_norw_AM_integ_temp[NREGION][NASSOCIATIVEMEMORY]={{0}};
  int nroad_rw_AM_integ_temp[NREGION][NASSOCIATIVEMEMORY]={{0}};
  int ncomb_integ_temp[NREGION]={0};
  int ntrack_integ_temp[NREGION]={0};
  int ntrack_hw_integ_temp[NREGION]={0};
  int nfit_integ_temp[NREGION]={0};
  int nhit_raw_integ_temp[NREGION][NLAYER]={{0}};
  int nhit_hw_integ_temp[NREGION][NLAYER]={{0}}, nhit_nohw_integ_temp[NREGION][NLAYER]={{0}};  // ..cofusing nohw is after hw.....
  int nhit_raw_tot_integ_temp[NREGION]={0};  // Creep! =[0]+....[NLAYER]
  int nhit_hw_tot_integ_temp[NREGION]={0}, nhit_nohw_tot_integ_temp[NREGION]={0}; // Creep!

  int nss_integ_temp[NREGION][NLAYER]={{0}};  // ..cofusing nohw is after hw.....
  int nss_tot_integ_temp[NREGION]={0}; // Creep!

  int ntrack_tot=0, ntrack_ok=0;

  InputValues inv[NREGION];
  for(int i=0; i<NREGION ; i++){ // for first events
    for(int j=0; j<NLAYER ; j++)inv[i].nhit_nohw_integ_history[j].push_back(0);  
    inv[i].TF_ew_out_history.push_back(0);
  }

  /// ***** event loop ***** // ------------------- event loop start -----------------------------------------------  
  // include all root files in turn Ev1 -> Ev100 //
  for(int jentry=0 ; jentry<nentries; jentry++){
    dump=0;
    if(dump_event == jentry)dump=1;
    for(int d=0 ; d<n_dump ; d++) if(array_dump_event[d] == jentry ) dump=1;
    // *** Get events ***//
    if(FTKTimeSim_Debug>3)cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
    if(debug>0 || FTKTimeSim_Debug>1)cout<<"LLLLLLLLLLLLLLLLLLLLLLLLL event number = "
		   <<jentry<<"  LLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
    if(FTKTimeSim_Debug>3)cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
 


   // ** region get ** //-------------------------------------------------
    int n_overloop = jentry/primary_nentries;
    for(int i=0 ; i<NREGION ; i++){
      //cout<<jentry-primary_nentries*n_overloop<<endl;
      trk[i]->GetEntry(jentry-primary_nentries*n_overloop);
      road[i]->GetEntry(jentry-primary_nentries*n_overloop);
      trk2[i]->GetEntry(jentry-primary_nentries*n_overloop);
      road2[i]->GetEntry(jentry-primary_nentries*n_overloop);
    }


    // ** set the input value ** //    --------- set start -----------
    for(int i=0; i<NREGION ; i++) { // for .....
      inv[i].nentry=jentry;
      inv[i].LVL1Gap= (1000000/L1rate); // rate to gap
      inv[i].s_overlap = SOVERLAP;
      inv[i].t_kill_TF = time_kill_TF_proc;//time_kill_TF_proc;
      inv[i].tt = 0;
      inv[i].split=s_split;
      inv[i].FirstStage=first_stage_layer;
    }


    // * # of track after hw * //
    double temp_track_hw;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[0]->m_ntracks ; kk++)if(!trk[0]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[0].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[0]+=temp_track_hw; inv[0].ntrack_hw_integ=ntrack_hw_integ_temp[0]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[1]->m_ntracks ; kk++)if(!trk[1]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[1].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[1]+=temp_track_hw; inv[1].ntrack_hw_integ=ntrack_hw_integ_temp[1]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[2]->m_ntracks ; kk++)if(!trk[2]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[2].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[2]+=temp_track_hw; inv[2].ntrack_hw_integ=ntrack_hw_integ_temp[2]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[3]->m_ntracks ; kk++)if(!trk[3]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[3].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[3]+=temp_track_hw; inv[3].ntrack_hw_integ=ntrack_hw_integ_temp[3]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[4]->m_ntracks ; kk++)if(!trk[4]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[4].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[4]+=temp_track_hw; inv[4].ntrack_hw_integ=ntrack_hw_integ_temp[4]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[5]->m_ntracks ; kk++)if(!trk[5]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[5].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[5]+=temp_track_hw; inv[5].ntrack_hw_integ=ntrack_hw_integ_temp[5]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[6]->m_ntracks ; kk++)if(!trk[6]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[6].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[6]+=temp_track_hw; inv[6].ntrack_hw_integ=ntrack_hw_integ_temp[6]*sf_nregion;
    temp_track_hw=0;
    for(int kk=0 ; kk<trk[7]->m_ntracks ; kk++)if(!trk[7]->m_tracks_m_HW_rejected[kk])temp_track_hw++;
    inv[7].ntrack_hw=temp_track_hw*sf_nregion; ntrack_hw_integ_temp[7]+=temp_track_hw; inv[7].ntrack_hw_integ=ntrack_hw_integ_temp[7]*sf_nregion;


    // * # of track * //
    inv[0].ntrack=trk[0]->m_ntracks*sf_nregion; ntrack_integ_temp[0]+=trk[0]->m_ntracks; inv[0].ntrack_integ=ntrack_integ_temp[0]*sf_nregion;
    inv[1].ntrack=trk[1]->m_ntracks*sf_nregion; ntrack_integ_temp[1]+=trk[1]->m_ntracks; inv[1].ntrack_integ=ntrack_integ_temp[1]*sf_nregion;
    inv[2].ntrack=trk[2]->m_ntracks*sf_nregion; ntrack_integ_temp[2]+=trk[2]->m_ntracks; inv[2].ntrack_integ=ntrack_integ_temp[2]*sf_nregion;
    inv[3].ntrack=trk[3]->m_ntracks*sf_nregion; ntrack_integ_temp[3]+=trk[3]->m_ntracks; inv[3].ntrack_integ=ntrack_integ_temp[3]*sf_nregion;
    inv[4].ntrack=trk[4]->m_ntracks*sf_nregion; ntrack_integ_temp[4]+=trk[4]->m_ntracks; inv[4].ntrack_integ=ntrack_integ_temp[4]*sf_nregion;
    inv[5].ntrack=trk[5]->m_ntracks*sf_nregion; ntrack_integ_temp[5]+=trk[5]->m_ntracks; inv[5].ntrack_integ=ntrack_integ_temp[5]*sf_nregion;
    inv[6].ntrack=trk[6]->m_ntracks*sf_nregion; ntrack_integ_temp[6]+=trk[6]->m_ntracks; inv[6].ntrack_integ=ntrack_integ_temp[6]*sf_nregion;
    inv[7].ntrack=trk[7]->m_ntracks*sf_nregion; ntrack_integ_temp[7]+=trk[7]->m_ntracks; inv[7].ntrack_integ=ntrack_integ_temp[7]*sf_nregion;
    // second stage
    inv[0].SecStage_ntrack=trk2[0]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[0]+=trk2[0]->m_ntracks; inv[0].SecStage_ntrack_integ=ntrack_integ_temp[0]*sf_nregion/sec_sf;
    inv[1].SecStage_ntrack=trk2[1]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[1]+=trk2[1]->m_ntracks; inv[1].SecStage_ntrack_integ=ntrack_integ_temp[1]*sf_nregion/sec_sf;
    inv[2].SecStage_ntrack=trk2[2]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[2]+=trk2[2]->m_ntracks; inv[2].SecStage_ntrack_integ=ntrack_integ_temp[2]*sf_nregion/sec_sf;
    inv[3].SecStage_ntrack=trk2[3]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[3]+=trk2[3]->m_ntracks; inv[3].SecStage_ntrack_integ=ntrack_integ_temp[3]*sf_nregion/sec_sf;
    inv[4].SecStage_ntrack=trk2[4]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[4]+=trk2[4]->m_ntracks; inv[4].SecStage_ntrack_integ=ntrack_integ_temp[4]*sf_nregion/sec_sf;
    inv[5].SecStage_ntrack=trk2[5]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[5]+=trk2[5]->m_ntracks; inv[5].SecStage_ntrack_integ=ntrack_integ_temp[5]*sf_nregion/sec_sf;
    inv[6].SecStage_ntrack=trk2[6]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[6]+=trk2[6]->m_ntracks; inv[6].SecStage_ntrack_integ=ntrack_integ_temp[6]*sf_nregion/sec_sf;
    inv[7].SecStage_ntrack=trk2[7]->m_ntracks*sf_nregion/sec_sf; ntrack_integ_temp[7]+=trk2[7]->m_ntracks; inv[7].SecStage_ntrack_integ=ntrack_integ_temp[7]*sf_nregion/sec_sf;
    //* # fit * //
    inv[0].nfit=(trk[0]->m_nfits-trk[0]->m_nfits_rec)*sf_nregion; nfit_integ_temp[0]+=(trk[0]->m_nfits-trk[0]->m_nfits_rec); inv[0].nfit_integ=nfit_integ_temp[0]*sf_nregion;
    inv[1].nfit=(trk[1]->m_nfits-trk[1]->m_nfits_rec)*sf_nregion; nfit_integ_temp[1]+=(trk[1]->m_nfits-trk[1]->m_nfits_rec); inv[1].nfit_integ=nfit_integ_temp[1]*sf_nregion;
    inv[2].nfit=(trk[2]->m_nfits-trk[2]->m_nfits_rec)*sf_nregion; nfit_integ_temp[2]+=(trk[2]->m_nfits-trk[2]->m_nfits_rec); inv[2].nfit_integ=nfit_integ_temp[2]*sf_nregion;
    inv[3].nfit=(trk[3]->m_nfits-trk[3]->m_nfits_rec)*sf_nregion; nfit_integ_temp[3]+=(trk[3]->m_nfits-trk[3]->m_nfits_rec); inv[3].nfit_integ=nfit_integ_temp[3]*sf_nregion;
    inv[4].nfit=(trk[4]->m_nfits-trk[4]->m_nfits_rec)*sf_nregion; nfit_integ_temp[4]+=(trk[4]->m_nfits-trk[4]->m_nfits_rec); inv[4].nfit_integ=nfit_integ_temp[4]*sf_nregion;
    inv[5].nfit=(trk[5]->m_nfits-trk[5]->m_nfits_rec)*sf_nregion; nfit_integ_temp[5]+=(trk[5]->m_nfits-trk[5]->m_nfits_rec); inv[5].nfit_integ=nfit_integ_temp[5]*sf_nregion;
    inv[6].nfit=(trk[6]->m_nfits-trk[6]->m_nfits_rec)*sf_nregion; nfit_integ_temp[6]+=(trk[6]->m_nfits-trk[6]->m_nfits_rec); inv[6].nfit_integ=nfit_integ_temp[6]*sf_nregion;
    inv[7].nfit=(trk[7]->m_nfits-trk[7]->m_nfits_rec)*sf_nregion; nfit_integ_temp[7]+=(trk[7]->m_nfits-trk[7]->m_nfits_rec); inv[7].nfit_integ=nfit_integ_temp[7]*sf_nregion;
    // second stage
    inv[0].SecStage_nfit=(trk2[0]->m_nfits-trk2[0]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[0]+=(trk2[0]->m_nfits-trk2[0]->m_nfits_rec); inv[0].SecStage_nfit_integ=nfit_integ_temp[0]*sf_nregion/sec_sf;
    inv[1].SecStage_nfit=(trk2[1]->m_nfits-trk2[1]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[1]+=(trk2[1]->m_nfits-trk2[1]->m_nfits_rec); inv[1].SecStage_nfit_integ=nfit_integ_temp[1]*sf_nregion/sec_sf;
    inv[2].SecStage_nfit=(trk2[2]->m_nfits-trk2[2]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[2]+=(trk2[2]->m_nfits-trk2[2]->m_nfits_rec); inv[2].SecStage_nfit_integ=nfit_integ_temp[2]*sf_nregion/sec_sf;
    inv[3].SecStage_nfit=(trk2[3]->m_nfits-trk2[3]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[3]+=(trk2[3]->m_nfits-trk2[3]->m_nfits_rec); inv[3].SecStage_nfit_integ=nfit_integ_temp[3]*sf_nregion/sec_sf;
    inv[4].SecStage_nfit=(trk2[4]->m_nfits-trk2[4]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[4]+=(trk2[4]->m_nfits-trk2[4]->m_nfits_rec); inv[4].SecStage_nfit_integ=nfit_integ_temp[4]*sf_nregion/sec_sf;
    inv[5].SecStage_nfit=(trk2[5]->m_nfits-trk2[5]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[5]+=(trk2[5]->m_nfits-trk2[5]->m_nfits_rec); inv[5].SecStage_nfit_integ=nfit_integ_temp[5]*sf_nregion/sec_sf;
    inv[6].SecStage_nfit=(trk2[6]->m_nfits-trk2[6]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[6]+=(trk2[6]->m_nfits-trk2[6]->m_nfits_rec); inv[6].SecStage_nfit_integ=nfit_integ_temp[6]*sf_nregion/sec_sf;
    inv[7].SecStage_nfit=(trk2[7]->m_nfits-trk2[7]->m_nfits_rec)*sf_nregion/sec_sf; nfit_integ_temp[7]+=(trk2[7]->m_nfits-trk2[7]->m_nfits_rec); inv[7].SecStage_nfit_integ=nfit_integ_temp[7]*sf_nregion/sec_sf;
    // * # road norw * //
    inv[0].nroad_norw=road[0]->FTKBank0_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[0]+=road[0]->FTKBank0_m_nao_nroads_am; inv[0].nroad_norw_integ=nroad_norw_integ_temp[0]*sf_nregion;
    inv[1].nroad_norw=road[1]->FTKBank1_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[1]+=road[1]->FTKBank1_m_nao_nroads_am; inv[1].nroad_norw_integ=nroad_norw_integ_temp[1]*sf_nregion;
    inv[2].nroad_norw=road[2]->FTKBank2_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[2]+=road[2]->FTKBank2_m_nao_nroads_am; inv[2].nroad_norw_integ=nroad_norw_integ_temp[2]*sf_nregion;
    inv[3].nroad_norw=road[3]->FTKBank3_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[3]+=road[3]->FTKBank3_m_nao_nroads_am; inv[3].nroad_norw_integ=nroad_norw_integ_temp[3]*sf_nregion;
    inv[4].nroad_norw=road[4]->FTKBank4_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[4]+=road[4]->FTKBank4_m_nao_nroads_am; inv[4].nroad_norw_integ=nroad_norw_integ_temp[4]*sf_nregion;
    inv[5].nroad_norw=road[5]->FTKBank5_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[5]+=road[5]->FTKBank5_m_nao_nroads_am; inv[5].nroad_norw_integ=nroad_norw_integ_temp[5]*sf_nregion;
    inv[6].nroad_norw=road[6]->FTKBank6_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[6]+=road[6]->FTKBank6_m_nao_nroads_am; inv[6].nroad_norw_integ=nroad_norw_integ_temp[6]*sf_nregion;
    inv[7].nroad_norw=road[7]->FTKBank7_m_nao_nroads_am*sf_nregion; nroad_norw_integ_temp[7]+=road[7]->FTKBank7_m_nao_nroads_am; inv[7].nroad_norw_integ=nroad_norw_integ_temp[7]*sf_nregion;

    // secound stage
    inv[0].SecStage_nroad_norw=road[0]->FTKBank0_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[0]+=road[0]->FTKBank0_m_nao_nroads_am; inv[0].SecStage_nroad_norw_integ=nroad_norw_integ_temp[0]*sf_nregion/sec_sf;
    inv[1].SecStage_nroad_norw=road[1]->FTKBank1_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[1]+=road[1]->FTKBank1_m_nao_nroads_am; inv[1].SecStage_nroad_norw_integ=nroad_norw_integ_temp[1]*sf_nregion/sec_sf;
    inv[2].SecStage_nroad_norw=road[2]->FTKBank2_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[2]+=road[2]->FTKBank2_m_nao_nroads_am; inv[2].SecStage_nroad_norw_integ=nroad_norw_integ_temp[2]*sf_nregion/sec_sf;
    inv[3].SecStage_nroad_norw=road[3]->FTKBank3_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[3]+=road[3]->FTKBank3_m_nao_nroads_am; inv[3].SecStage_nroad_norw_integ=nroad_norw_integ_temp[3]*sf_nregion/sec_sf;
    inv[4].SecStage_nroad_norw=road[4]->FTKBank4_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[4]+=road[4]->FTKBank4_m_nao_nroads_am; inv[4].SecStage_nroad_norw_integ=nroad_norw_integ_temp[4]*sf_nregion/sec_sf;
    inv[5].SecStage_nroad_norw=road[5]->FTKBank5_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[5]+=road[5]->FTKBank5_m_nao_nroads_am; inv[5].SecStage_nroad_norw_integ=nroad_norw_integ_temp[5]*sf_nregion/sec_sf;
    inv[6].SecStage_nroad_norw=road[6]->FTKBank6_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[6]+=road[6]->FTKBank6_m_nao_nroads_am; inv[6].SecStage_nroad_norw_integ=nroad_norw_integ_temp[6]*sf_nregion/sec_sf;
    inv[7].SecStage_nroad_norw=road[7]->FTKBank7_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[7]+=road[7]->FTKBank7_m_nao_nroads_am; inv[7].SecStage_nroad_norw_integ=nroad_norw_integ_temp[7]*sf_nregion/sec_sf;

    // * # road rw * //
    inv[0].nroad_rw=road[0]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[0]+=road[0]->FTKBank0_m_nao_nroads_rw; inv[0].nroad_rw_integ=nroad_rw_integ_temp[0]*sf_nregion;
    inv[1].nroad_rw=road[1]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[1]+=road[1]->FTKBank1_m_nao_nroads_rw; inv[1].nroad_rw_integ=nroad_rw_integ_temp[1]*sf_nregion;
    inv[2].nroad_rw=road[2]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[2]+=road[2]->FTKBank2_m_nao_nroads_rw; inv[2].nroad_rw_integ=nroad_rw_integ_temp[2]*sf_nregion;
    inv[3].nroad_rw=road[3]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[3]+=road[3]->FTKBank3_m_nao_nroads_rw; inv[3].nroad_rw_integ=nroad_rw_integ_temp[3]*sf_nregion;
    inv[4].nroad_rw=road[4]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[4]+=road[4]->FTKBank4_m_nao_nroads_rw; inv[4].nroad_rw_integ=nroad_rw_integ_temp[4]*sf_nregion;
    inv[5].nroad_rw=road[5]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[5]+=road[5]->FTKBank5_m_nao_nroads_rw; inv[5].nroad_rw_integ=nroad_rw_integ_temp[5]*sf_nregion;
    inv[6].nroad_rw=road[6]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[6]+=road[6]->FTKBank6_m_nao_nroads_rw; inv[6].nroad_rw_integ=nroad_rw_integ_temp[6]*sf_nregion;
    inv[7].nroad_rw=road[7]->FTKBank0_m_nao_nroads_rw*sf_nregion; nroad_rw_integ_temp[7]+=road[7]->FTKBank7_m_nao_nroads_rw; inv[7].nroad_rw_integ=nroad_rw_integ_temp[7]*sf_nregion;


    // secound stage
    inv[0].SecStage_nroad_norw=road[0]->FTKBank0_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[0]+=road[0]->FTKBank0_m_nao_nroads_am; inv[0].SecStage_nroad_norw_integ=nroad_norw_integ_temp[0]*sf_nregion/sec_sf;
    inv[1].SecStage_nroad_norw=road[1]->FTKBank1_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[1]+=road[1]->FTKBank1_m_nao_nroads_am; inv[1].SecStage_nroad_norw_integ=nroad_norw_integ_temp[1]*sf_nregion/sec_sf;
    inv[2].SecStage_nroad_norw=road[2]->FTKBank2_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[2]+=road[2]->FTKBank2_m_nao_nroads_am; inv[2].SecStage_nroad_norw_integ=nroad_norw_integ_temp[2]*sf_nregion/sec_sf;
    inv[3].SecStage_nroad_norw=road[3]->FTKBank3_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[3]+=road[3]->FTKBank3_m_nao_nroads_am; inv[3].SecStage_nroad_norw_integ=nroad_norw_integ_temp[3]*sf_nregion/sec_sf;
    inv[4].SecStage_nroad_norw=road[4]->FTKBank4_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[4]+=road[4]->FTKBank4_m_nao_nroads_am; inv[4].SecStage_nroad_norw_integ=nroad_norw_integ_temp[4]*sf_nregion/sec_sf;
    inv[5].SecStage_nroad_norw=road[5]->FTKBank5_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[5]+=road[5]->FTKBank5_m_nao_nroads_am; inv[5].SecStage_nroad_norw_integ=nroad_norw_integ_temp[5]*sf_nregion/sec_sf;
    inv[6].SecStage_nroad_norw=road[6]->FTKBank6_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[6]+=road[6]->FTKBank6_m_nao_nroads_am; inv[6].SecStage_nroad_norw_integ=nroad_norw_integ_temp[6]*sf_nregion/sec_sf;
    inv[7].SecStage_nroad_norw=road[7]->FTKBank7_m_nao_nroads_am*sf_nregion/sec_sf; nroad_norw_integ_temp[7]+=road[7]->FTKBank7_m_nao_nroads_am; inv[7].SecStage_nroad_norw_integ=nroad_norw_integ_temp[7]*sf_nregion/sec_sf;




    // #road for each AM //
   int temp_nroad[NREGION][NASSOCIATIVEMEMORY] = {0};
    for(int i=0 ; i<road[0]->FTKBank0_m_nroads ; i++) temp_nroad[0][road[0]->FTKBank0_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[1]->FTKBank1_m_nroads ; i++) temp_nroad[1][road[1]->FTKBank1_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[2]->FTKBank2_m_nroads ; i++) temp_nroad[2][road[2]->FTKBank2_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[3]->FTKBank3_m_nroads ; i++) temp_nroad[3][road[3]->FTKBank3_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[4]->FTKBank4_m_nroads ; i++) temp_nroad[4][road[4]->FTKBank4_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[5]->FTKBank5_m_nroads ; i++) temp_nroad[5][road[5]->FTKBank5_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[6]->FTKBank6_m_nroads ; i++) temp_nroad[6][road[6]->FTKBank6_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road[7]->FTKBank7_m_nroads ; i++) temp_nroad[7][road[7]->FTKBank7_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<NREGION; i++){
      for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){
	inv[i].nroad_rw_AM[j]=temp_nroad[i][j]*sf_nregion; nroad_rw_AM_integ_temp[i][j]+=temp_nroad[i][j];
	inv[i].nroad_rw_AM_integ[j]=nroad_rw_AM_integ_temp[i][j]*sf_nregion;
      }
    }

    
    // sec stage
    int temp_nroad2[NREGION][NASSOCIATIVEMEMORY] = {0};
    for(int i=0 ; i<road2[0]->FTKBank0_m_nroads ; i++) temp_nroad2[0][road2[0]->FTKBank0_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[1]->FTKBank1_m_nroads ; i++) temp_nroad2[1][road2[1]->FTKBank1_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[2]->FTKBank2_m_nroads ; i++) temp_nroad2[2][road2[2]->FTKBank2_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[3]->FTKBank3_m_nroads ; i++) temp_nroad2[3][road2[3]->FTKBank3_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[4]->FTKBank4_m_nroads ; i++) temp_nroad2[4][road2[4]->FTKBank4_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[5]->FTKBank5_m_nroads ; i++) temp_nroad2[5][road2[5]->FTKBank5_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[6]->FTKBank6_m_nroads ; i++) temp_nroad2[6][road2[6]->FTKBank6_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<road2[7]->FTKBank7_m_nroads ; i++) temp_nroad2[7][road2[7]->FTKBank7_m_roads_m_patternID[i]%NASSOCIATIVEMEMORY]++;
    for(int i=0 ; i<NREGION; i++){
      for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++) inv[i].SecStage_nroad_rw_AM[j]=temp_nroad2[i][j]*sf_nregion/sec_sf; nroad_rw_AM_integ_temp[i][j]+=temp_nroad2[i][j]; inv[i].SecStage_nroad_rw_AM_integ[j]=nroad_rw_AM_integ_temp[i][j]*sf_nregion/sec_sf;
    }
      
    // #road for each AM //
    for(int i=0 ; i<NREGION; i++){
      for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){
	inv[i].nroad_norw_AM[j]=inv[i].nroad_norw/NASSOCIATIVEMEMORY;
	inv[i].nroad_norw_AM_integ[j]=inv[i].nroad_norw_integ/NASSOCIATIVEMEMORY;
      }
    }    
    // sec stage
    for(int i=0 ; i<NREGION; i++){
      for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++){
	inv[i].SecStage_nroad_norw_AM[j]=inv[i].SecStage_nroad_norw/NASSOCIATIVEMEMORY;
	inv[i].SecStage_nroad_norw_AM_integ[j]=inv[i].SecStage_nroad_norw_integ/NASSOCIATIVEMEMORY;
      }
    }    


    
    // * # hit Input * //
    int temp_hit[NREGION]={0};
    int temp_ss[NREGION]={0};
    int temp_hit_plane[NREGION][NLAYER]={0};
    int temp_ss_plane[NREGION][NLAYER]={0};


    temp_hit[0]=road[0]->FTKBank0_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[0][i]=road[0]->FTKBank0_m_nao_nclus[i];

    temp_hit[1]=road[1]->FTKBank1_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[1][i]=road[1]->FTKBank1_m_nao_nclus[i];

    temp_hit[2]=road[2]->FTKBank2_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[2][i]=road[2]->FTKBank2_m_nao_nclus[i];

    temp_hit[3]=road[3]->FTKBank3_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[3][i]=road[3]->FTKBank3_m_nao_nclus[i];

    temp_hit[4]=road[4]->FTKBank4_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[4][i]=road[4]->FTKBank4_m_nao_nclus[i];

    temp_hit[5]=road[5]->FTKBank5_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[5][i]=road[5]->FTKBank5_m_nao_nclus[i];

    temp_hit[6]=road[6]->FTKBank6_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[6][i]=road[6]->FTKBank6_m_nao_nclus[i];

    temp_hit[7]=road[7]->FTKBank7_m_nao_nclus_tot;
    for(int i=0 ;i<NLAYERA; i++)temp_hit_plane[7][i]=road[7]->FTKBank7_m_nao_nclus[i];
    

    
    
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[0][i]=road[0]->FTKBank0_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[1][i]=road[1]->FTKBank1_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[2][i]=road[2]->FTKBank2_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[3][i]=road[3]->FTKBank3_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[4][i]=road[4]->FTKBank4_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[5][i]=road[5]->FTKBank5_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[6][i]=road[6]->FTKBank6_m_nao_nss[i];
    for(int i=0 ;i<NLAYERA; i++)temp_ss_plane[7][i]=road[7]->FTKBank7_m_nao_nss[i];



    //    cout<<temp_hit[0]<<" = "<<temp_hit_plane[0][0]<<" + "<<temp_hit_plane[0][1]<<" + "<<temp_hit_plane[0][2]<<" + "<<temp_hit_plane[0][3]<<" + "<<temp_hit_plane[0][4]<<" + "<<temp_hit_plane[0][5]<<" + "<<temp_hit_plane[0][6]<<" + "<<temp_hit_plane[0][7]<<" + "<<temp_hit_plane[0][8]<<" + "<<temp_hit_plane[0][9]<<" + "<<temp_hit_plane[0][10]<<" + "<<temp_hit_plane[0][11]
    //	<<"  check2"<<endl;    


    for(int i=0 ; i<NREGION ; i++){
      for(int j=0 ; j<8 ; j++){ 
	temp_ss[i]+=temp_ss_plane[i][j];
	//	cout<<"check calc  " <<temp_ss[i]<<"+="<<temp_ss_plane[i][j]<<endl;
      }
    }




    // Hit Info Fill to inv[i] for main func // 
    for(int i=0 ; i<NREGION ; i++){
      inv[i].nhit_nohw_tot=temp_hit[i]*sf_nregion_hit/sf_hit_overlapregion; nhit_nohw_tot_integ_temp[i]+=temp_hit[i]/sf_hit_overlapregion; inv[i].nhit_nohw_tot_integ=nhit_nohw_tot_integ_temp[i]*sf_nregion_hit;
      inv[i].nss_tot=temp_ss[i]*sf_nregion_hit/sf_hit_overlapregion; nss_tot_integ_temp[i]+=temp_ss[i]/sf_hit_overlapregion; inv[i].nss_tot_integ=nss_tot_integ_temp[i]*sf_nregion_hit;
      for(int j=0 ; j<NLAYER ; j++){ // layer, plane, pixel3 sct8  
	inv[i].nhit_nohw[j]=temp_hit_plane[i][j]*sf_nregion_hit/(sf_hit_overlapregion*hit_div[j]); 
	nhit_nohw_integ_temp[i][j]+=temp_hit_plane[i][j]/(sf_hit_overlapregion*hit_div[j]); 
	inv[i].nhit_nohw_integ[j]=nhit_nohw_integ_temp[i][j]*sf_nregion_hit;

	inv[i].nss[j]=temp_hit_plane[i][j]*sf_nregion_hit/(sf_hit_overlapregion*hit_div[j]); nss_integ_temp[i][j]+=temp_hit_plane[i][j]/(sf_hit_overlapregion*hit_div[j]); inv[i].nss_integ[j]=nss_integ_temp[i][j]*sf_nregion_hit;
      }
      //      cout<<"check  : "<<inv[0].nss_tot<<" ss     "<<inv[0].nhit_nohw_tot<<" hits"<<endl;

      // ************* set all Value for  region *************** //    --------- set end  ---------
      sim->SetInputValueForRegion(i,inv[i]);
    }



    //*********************************************************************************************************************//
    //*******************************************   Set End   *************************************************************//
    //*********************************************************************************************************************//



    // **** Fill of some paremeter  **** //   //It is in FTKTimeSim Func//
    sim->FillInputValue(); 

    // **** Calculation **** //   // FTKTimeSim -> Region -> DO,AM,SecDO,TF's Calc() //
    sim->Calc();   



    // **** Counter of Killed track **** //

    for(int i=0 ; i<NREGION ; i++){
      if(sim->GetNkill(i)){
	cout<<"region "<<i<<" is killed "<<sim->GetNkill(i)<<endl;
      }else{
	ntrack_ok+=inv[i].ntrack;
      }
      ntrack_tot+=inv[i].ntrack;
      
    }
    //    if(debug>3){
    //      cout<<"***TrackEff is "<<(double)ntrack_ok/(double)ntrack_tot<<"     "<<(double)ntrack_ok<<" / "<<(double)ntrack_tot<<endl;
    //
    //    }
    
    // **** Fill the input value for next envets **** //
    for(int i=0 ; i<NREGION ; i++){
      for(int j=0; j<NLAYER ; j++)inv[i].nhit_nohw_integ_history[j].push_back(inv[i].nhit_nohw_integ[j]);  
      inv[i].TF_ew_out_history.push_back(sim->GetTF_EwOut(i));
    }
    // **** Fill results **** //
    sim->FillResults(dump, jentry, description2, temp_denom, s_split);


    if(debug>5){  // ***** debug ***** //  -------------- debug start -----------
      cout<<" **** HW RW FW? **** "<<endl;
      cout<<"Road : m_nroads = "<<road[0]->FTKBank0_m_nroads
	  <<"     : m_nroads_tot = "<<road[0]->FTKBank0_m_nroads_tot
	  <<"     : m_nroads_good = "<<road[0]->FTKBank0_m_nroads_good
	  <<"     : m_nroads_maj = "<<road[0]->FTKBank0_m_nroads_maj
	  <<"     : m_nroads_maj_good = "<<road[0]->FTKBank0_m_nroads_maj_good
	  <<endl;
    }                                    //   -------------- debug end -----------

    

  }  // ---------------------------- event loop end--------------------------------------



  // **** Print some input parameter **** //
  cout<<"*** PlotInputValues ***"<<endl;
  if(s_plot)sim->PlotInputValues(description2, s_split,nrbin); //FTKTimeSim.cc func.
  cout<<"*** PlotResults ***"<<endl;
  if(s_plot)sim->PlotResults(description2, temp_denom, s_split,nrbin); //FTKTimeSim.cc func.



  sim->PrintResults(sf_hit_overlapregion, nloop, nloopsloop, sf_nregion, description2, ntrack_ok , ntrack_tot, s_split); //FTKTimeSim.cc func.


  //  Stop timer and print results
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "************** summary table *********************" << endl;   // This time is compiling time.
  printf("RealTime=%.3f seconds, CpuTime=%.3f seconds\n",rtime,ctime);
  printf(" %.3f NEV/Realtime seconds\n",(Double_t)nentries/rtime);
  printf(" %.3f NEV/Cputime seconds\n",(Double_t)nentries/ctime);
  
  return 0;
}
