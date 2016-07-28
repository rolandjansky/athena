/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TLine.h"
#include "TLatex.h"
#include "TLegend.h"
//#include "TFile.h"

#include "FTKTimeSim.hh"

FTKTimeSim::FTKTimeSim()   //Instance set initialize
{
  tf = new TFile("OutputFigure.root","RECREATE");

  for(int idx=0;idx<NPU;idx++) {
     Region sR(idx);
     region.push_back(sR);
   }
   make_heap(region.begin(), region.end(), CompEwOut());

   // **** plot def **** //
   char temp_name[256];

   // HH full range full region HH //
   for(int reg=0 ; reg<NPU; reg++){
     sprintf(temp_name, "hreg_hit_pix0_%d",reg);
     hreg_hit_pix0[reg] = new TH1D(temp_name, temp_name, 10000, 0, 10000);
     sprintf(temp_name, "hreg_road_%d",reg);
     hreg_road[reg] = new TH1D(temp_name, temp_name, 1000000, 0, 1000000);
     sprintf(temp_name, "hreg_fit_%d",reg);
     hreg_fit[reg] = new TH1D(temp_name, temp_name, 10000000, 0, 10000000);
   }
   

   // * hit distribution * //
   int n_hitmax=80000;
   h_ftkhit = new TH1D("h_ftkhit", "h_ftkhit", n_hitmax/4, 0, n_hitmax);
   h_ftkss = new TH1D("h_ftkss", "h_ftkss", n_hitmax/4, 0, n_hitmax);
   h_maxftkhit = new TH1D("h_maxftkhit", "h_max_ftkhit", n_hitmax/3, 0, n_hitmax);
   for(int i=0 ; i<NLAYER ; i++){
     sprintf(temp_name, "h_ftkhit_layer_%d", i);
     h_ftkhit_layer[i] = new TH1D(temp_name, temp_name, n_hitmax/3, 0, n_hitmax);
     sprintf(temp_name, "h_ftkss_layer_%d", i);
     h_ftkss_layer[i] = new TH1D(temp_name, temp_name, n_hitmax/3, 0, n_hitmax);
   }
   // * road distribution * //
   int n_roadmax=3000000;
   h_road = new TH1D("h_road", "h_road", n_roadmax/60, 0, n_roadmax);
   h_road_rw = new TH1D("h_road_rw", "h_road_rw", n_roadmax/60, 0, n_roadmax);
   h_maxroad = new TH1D("h_maxroad", "h_maxroad", n_roadmax/60, 0, n_roadmax);
   h_SecStage_road = new TH1D("h_SecStage_road", "h_SecStage_road", n_roadmax/60, 0, n_roadmax);
   h_SecStage_maxroad = new TH1D("h_SecStage_maxroad", "h_SecStage_maxroad", n_roadmax/60, 0, n_roadmax);
   for(int i=0 ; i<NASSOCIATIVEMEMORY ; i++){
     sprintf(temp_name, "h_road_AM_%d", i);
     h_road_AM[i] = new TH1D(temp_name, temp_name, n_roadmax/60, 0, n_roadmax);
     sprintf(temp_name, "h_road_rw_AM_%d", i);
     h_road_rw_AM[i] = new TH1D(temp_name, temp_name, n_roadmax/60, 0, n_roadmax);
     sprintf(temp_name, "h_SecStage road_AM_%d", i);
     h_SecStage_road_AM[i] = new TH1D(temp_name, temp_name, n_roadmax/60, 0, n_roadmax);
   }
   // * fit distribution * //
   //   int n_fitmax=20000000;
   int n_fitmax=200000000;
   h_fit = new TH1D("h_fit", "h_fit", n_fitmax/20, 0, n_fitmax);
   h_maxfit = new TH1D("h_maxfit", "h_max_fit", n_fitmax/20, 0, n_fitmax);
   h_SecStage_fit = new TH1D("h_SecStage_fit", "h_fit", n_fitmax/20, 0, n_fitmax);
   h_SecStage_maxfit = new TH1D("h_SecStage_maxfit", "h_max_fit", n_fitmax/200, 0, n_fitmax);
   // * vs plot * //
   h_ftkhit_vs_road = new TH2D("h_ftkhit_vs_road", "hit_vs_road", 1000, 0, n_hitmax, 1000, 0, n_roadmax);
   h_ftkhit_vs_fit = new TH2D("h_ftkhit_vs_fit", "hit_vs_fit", 1000, 0, n_hitmax, 1000, 0, n_fitmax);
   h_road_vs_fit = new TH2D("h_road_vs_fit", "hit_vs_fit", 1000, 0, n_hitmax, 1000, 0, n_fitmax);

   // ** results plot ** //
   h_timeee = new TH1D("h_timeee","", 1000000, 0, 1000000);
   h_timeee_full = new TH1D("h_timeee_full","", 3000000, 0, 3000000);
   h_SecStage_timeee = new TH1D("h_SecStage_timeee","FTK Procces Time", 1000000, 0, 1000000);
   h_DF_EwOut = new TH1D("h_DF_EwOut","DF EwOut", 1000000, 0, 1000000);
   h_DO_EwOut = new TH1D("h_DO_EwOut","DO EwOut", 1000000, 0, 1000000);
   h_AM_EwOut = new TH1D("h_AM_EwOut","AM EwOut", 1000000, 0, 1000000);
   h_SecDO_EwOut = new TH1D("h_SecDO_EwOut","SecDO EwOut", 1000000, 0, 1000000);
   h_TF_EwOut = new TH1D("h_TF_EwOut","TF EwOut", 1000000, 0, 1000000);
   h_SecStageSecDO_EwOut = new TH1D("h_SecStageSecDO_EwOut","SecStageSecDO EwOut", 1000000, 0, 1000000);
   h_SecStageTF_EwOut = new TH1D("h_SecStageTF_EwOut","SecStageTF EwOut", 1000000, 0, 1000000);
   
   h_event_detail = new TH2D("h_event_detail", "EventDetail", 1000000, 0, 1000000, 10, -0.5, 9.5);
   h_SecStage_event_detail = new TH2D("h_SecStage_event_detail", "EventDetail", 1000000, 0, 1000000, 10, -0.5, 9.5);

   
}

FTKTimeSim::~FTKTimeSim()
{
  //  tf->Write();
  tf->Delete();
}

void
FTKTimeSim::SetInputValueForRegion(int index, InputValues iv)
{
  region[index].SetInputValues(iv);
}

void
FTKTimeSim::Calc()
{
   for(int idx=0;idx<NPU;idx++) {
     if(FTKTimeSim_Debug==4) bool is_region_dump=true;

     if(idx==dump_pu){ 
       FTKTimeSim_Debug=4;
     }else{
       FTKTimeSim_Debug=3;
     }
     //     cout<<"CCCCCCCCCCCC  check reg="<<idx<<"   debug = "<<FTKTimeSim_Debug<<endl;

     
     if(FTKTimeSim_Debug>3)
       cout<<"=======================================  Calc Region is "
	   <<idx<<"======================================= data exist ="<<region[idx].iv.data_exist<<"    nentry = "<<region[idx].iv.nentry<<endl;
     if(region[idx].iv.data_exist){
       region[idx].Calc();
     }else{
       region[idx].SetFwOut(0);
       region[idx].SetEwOut(0);
       region[idx].SetFwIn(0);
       region[idx].SetEwIn(0);  
       region[idx].SetPriFwOut(0);	
       region[idx].SetPriEwOut(0);	
       region[idx].SetPriFwIn(0);	
       region[idx].SetPriEwIn(0);  
     }				     
   }
   // Sort Region objects by ew_out
   //   sort_heap(region.begin(), region.end(), CompEwOut());

  
}

void 
FTKTimeSim::FillInputValue()
{
  for(int idx=0 ; idx<NPU ; idx++){
    if(!region[idx].iv.data_exist) continue;
    // full range full region
    hreg_hit_pix0[idx] ->Fill(region[idx].iv.nhit_nohw[0]);
    hreg_road[idx] ->Fill(region[idx].iv.nroad_norw);
    hreg_fit[idx] ->Fill(region[idx].iv.nfit);
    RoadsX5_vec[idx].push_back((region[idx].iv.nroad_norw)*5./8000.);
    FitsX1_vec[idx].push_back(region[idx].iv.nfit/8000.);
    if((region[idx].iv.nroad_norw)*5./8000. > region[idx].iv.nfit/8000.){
      maxFitRoad_vec[idx].push_back((region[idx].iv.nroad_norw)*5./8000.);
    }else{
      maxFitRoad_vec[idx].push_back(region[idx].iv.nfit/8000.);
    }
    //    if(idx==dump_pu)cout<<"sec fit = "<<(region[idx].iv.nfit/8000.)<<"   road = "<<((region[idx].iv.nroad_norw)*5./8000.)<<endl;
    //    if(idx==dump_pu)cout<<"n   fit = "<<(region[idx].iv.nfit)<<"   road = "<<(region[idx].iv.nroad_norw)<<endl;
    //
    h_ftkhit->Fill(region[idx].iv.nhit_nohw_tot);
    h_ftkss->Fill(region[idx].iv.nss_tot);
    h_road->Fill(region[idx].iv.nroad_norw);
    h_road_rw->Fill(region[idx].iv.nroad_rw);
    h_fit->Fill(region[idx].iv.nfit);
    //    h_ftkhit_vs_road->Fill(region[idx].iv.nhit_nohw_tot, region[idx].iv.nroad_norw);
    //    h_ftkhit_vs_fit->Fill(region[idx].iv.nhit_nohw_tot, region[idx].iv.nfit);
    //    h_road_vs_fit->Fill(region[idx].iv.nroad_norw, region[idx].iv.nfit);
    // SecStage
    h_SecStage_road->Fill(region[idx].iv.SecStage_nroad_norw);
    h_SecStage_fit->Fill(region[idx].iv.SecStage_nfit);
    //    cout<<region[idx].iv.SecStage_nfit<<"    for the just check of the SecStage inv"<<endl;
    for(int j=0 ; j<NLAYER ; j++)   h_ftkhit_layer[j]->Fill(region[idx].iv.nhit_nohw[j]);
    for(int j=0 ; j<NLAYER ; j++)   h_ftkss_layer[j]->Fill(region[idx].iv.nss[j]);
    for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++)  h_road_AM[j]->Fill(region[idx].iv.nroad_norw_AM[j]);
    for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++)  h_road_rw_AM[j]->Fill(region[idx].iv.nroad_rw_AM[j]);
    for(int j=0 ; j<NASSOCIATIVEMEMORY ; j++)  h_SecStage_road_AM[j]->Fill(region[idx].iv.SecStage_nroad_norw_AM[j]);
  }
}


double
FTKTimeSim::GetTF_EwOut(int index){
  gStyle->SetPadLeftMargin(0.17);           
  // ****** to input value for next events to estimate DO buffer ***** //
  double temp_ewout = 0;
  for(int i=0 ; i<NTRACKFITTER ; i++){
    if(region[index].GetTrackFitter(i).Get(i).GetEwOut()>temp_ewout) temp_ewout =region[index].GetTrackFitter(i).Get(i).GetEwOut();
  }
  return temp_ewout;
}

double
FTKTimeSim::GetNkill(int index){
  // ****** to input value for next events to estimate DO buffer ***** //
  double temp_kill = 0;
  for(int i=0 ; i<NTRACKFITTER ; i++){
    temp_kill+=region[index].GetTrackFitter(i).Get(i).GetNKillTF();
    //    cout<<temp_kill<<" check!! "<< i<<endl;
  }
  return temp_kill;
}

void 
FTKTimeSim::FillResults(int dump, int jentry, char *description, int temp_denom, int s_split)  //Fill Section
{
  // ** temp  output file name ** //
  char temp_outdir[256];
  sprintf(temp_outdir,"mkdir -p output/%s",description);
  system(temp_outdir);
  sprintf(temp_outdir,"mkdir -p output/%s/C",description);
  system(temp_outdir);

  if(FTKTimeSim_Debug>2)cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
  if(FTKTimeSim_Debug>2)cout<<"XX Event Summary (PU version) : This is Fill results ()    event = "<<jentry<<endl;
  if(FTKTimeSim_Debug>2)cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
  //  int temp_denom=1;
  Region regtemp(-99);
  vector<double> temp;
  temp.clear();
  //  cout<<region[0].GetDataOrganizer(0).Get(0).GetBuff()<<"  check for region0 DO0 buff"<<endl;


  for(int idx=0 ; idx<NPU ; idx++){
    if(region[idx].GetEwOut()!=0){
      if(FTKTimeSim_Debug>2)    cout<<"region"<<idx<<"  "<<region[idx].GetEwOut()<<endl;
    }
    //    if(FTKTimeSim_Debug>2)    cout<<"region"<<idx<<"  "<<region[idx].GetEwOut()<<endl;
    temp.push_back(region[idx].GetEwOut());
  }  
  h_timeee->Fill(regtemp.GetMaxVec(temp)/temp_denom); // temp_denom is for just unit of micro and nano
  h_timeee_full->Fill(regtemp.GetMaxVec(temp)/temp_denom); // temp_denom is for just unit of micro and nano
  EwOut_vec.push_back(regtemp.GetMaxVec(temp)/temp_denom);
  xAxis_vec.push_back((double)jentry);
  int max_region = regtemp.GetMaxRegion(temp);
  if(FTKTimeSim_Debug>2){
    cout<<"max region is = "<<max_region<<endl;
    cout<<regtemp.GetMaxVec(temp)/temp_denom
	<<" Final timeeee of this event!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }

  // *** for make max region's each board's time! *** //
  vector<IOTimeBase> iot;

  //  DF_FwIn=-1; DF_FwOut=-1; DF_EwIn=-1; DF_EwOut=-1;  
//   for(int i=0 ; i<NTRACKFITTER; i++) iot.push_back(region[max_region].cDF[i].iotime[i]);
//   DF_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;

//   DF_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
//   DF_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  DF_FwOut=1.;
  DF_EwOut=1.;
  DF_FwIn=0.;
  DF_EwIn=0.;
  //  if(FTKTimeSim_Debug>2) cout<<"DF_EwOut = "<<DF_EwOut<<endl;
  iot.clear();

  DO_FwIn=-1; DO_FwOut=-1; DO_EwIn=-1; DO_EwOut=-1;  
  for(int i=0 ; i<NDATAORGANIZER; i++) iot.push_back(region[max_region].GetDataOrganizer(i).Get(i));
  DO_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;
  DO_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  DO_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
  DO_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  if(FTKTimeSim_Debug>2) cout<<"DO_FwIn = "<<DO_FwIn<<",   DO_FwOut = "<<DO_FwOut<<",   DO_EwIn = "<<DO_EwIn<<",   DO_EwOut = "<<DO_EwOut<<endl;
  h_DO_EwOut->Fill(DO_EwOut); 
  iot.clear();
  for(int i=0 ; i<NDATAORGANIZER; i++) iot.push_back(region[max_region].GetDataOrganizer(i).GetPre(i));
  pre_DO_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  iot.clear();
  
  AM_FwIn=-1; AM_FwOut=-1; AM_EwIn=-1; AM_EwOut=-1;  
  for(int i=0 ; i<NASSOCIATIVEMEMORY; i++) iot.push_back(region[max_region].GetAssociativeMemory(i).Get(i));
  AM_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;
  AM_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  AM_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
  AM_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  if(FTKTimeSim_Debug>2) cout<<"AM_FwIn = "<<AM_FwIn<<",   AM_FwOut = "<<AM_FwOut<<",   AM_EwIn = "<<AM_EwIn<<",   AM_EwOut = "<<AM_EwOut<<endl;
  h_AM_EwOut->Fill(AM_EwOut);
  iot.clear();
  for(int i=0 ; i<NASSOCIATIVEMEMORY; i++) iot.push_back(region[max_region].GetAssociativeMemory(i).GetPre(i));
  pre_AM_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  iot.clear();

  SecDO_FwIn=-1; SecDO_FwOut=-1; SecDO_EwIn=-1; SecDO_EwOut=-1;  
  for(int i=0 ; i<NDATAORGANIZER; i++) iot.push_back(region[max_region].GetSecDataOrganizer(i).Get(i));
  SecDO_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;
  SecDO_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  SecDO_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
  SecDO_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  if(FTKTimeSim_Debug>2) cout<<"SecDO_FwIn = "<<SecDO_FwIn<<",   SecDO_FwOut = "<<SecDO_FwOut<<",  SecDO_EwIn = "<<SecDO_EwIn<<",   SecDO_EwOut = "<<SecDO_EwOut<<endl;
  h_SecDO_EwOut->Fill(SecDO_EwOut);
  iot.clear();
  for(int i=0 ; i<NDATAORGANIZER; i++) iot.push_back(region[max_region].GetSecDataOrganizer(i).GetPre(i));
  pre_SecDO_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  iot.clear();

  TF_FwIn=-1; TF_FwOut=-1; TF_EwIn=-1; TF_EwOut=-1;  
  for(int i=0 ; i<NTRACKFITTER; i++) iot.push_back(region[max_region].GetTrackFitter(i).Get(i));
  TF_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;
  TF_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  TF_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
  TF_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  if(FTKTimeSim_Debug>2) cout<<"TF_FwIn = "<<TF_FwIn<<",   TF_FwOut = "<<TF_FwOut<<",   TF_EwIn = "<<TF_EwIn<<",   TF_EwOut = "<<TF_EwOut<<endl;
  h_TF_EwOut->Fill(TF_EwOut);
  iot.clear();
  for(int i=0 ; i<NTRACKFITTER; i++) iot.push_back(region[max_region].GetTrackFitter(i).GetPre(i));
  pre_TF_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  iot.clear();  

  // SecStage
  SecStageSecDO_FwIn=-1; SecStageSecDO_FwOut=-1; SecStageSecDO_EwIn=-1; SecStageSecDO_EwOut=-1;  
  for(int i=0 ; i<NSECSTAGEDATAORGANIZER; i++) iot.push_back(region[max_region].GetSecStageSecDataOrganizer(i).Get(i));
  SecStageSecDO_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;
  SecStageSecDO_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  SecStageSecDO_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
  SecStageSecDO_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  if(FTKTimeSim_Debug>2) cout<<"SecStageSecDO_FwIn = "<<SecStageSecDO_FwIn<<",   SecStageSecDO_FwOut = "<<SecStageSecDO_FwOut<<",  SecStageSecDO_EwIn = "<<SecStageSecDO_EwIn<<",   SecStageSecDO_EwOut = "<<SecStageSecDO_EwOut<<endl;
  h_SecStageSecDO_EwOut->Fill(SecStageSecDO_EwOut);
  iot.clear();
  for(int i=0 ; i<NSECSTAGEDATAORGANIZER; i++) iot.push_back(region[max_region].GetSecStageSecDataOrganizer(i).GetPre(i));
  pre_SecStageSecDO_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  iot.clear();
      
  SecStageTF_FwIn=-1; SecStageTF_FwOut=-1; SecStageTF_EwIn=-1; SecStageTF_EwOut=-1;  
  for(int i=0 ; i<NSECSTAGETRACKFITTER; i++) iot.push_back(region[max_region].GetSecStageTrackFitter(i).Get(i));
  SecStageTF_FwOut=regtemp.GetMinFwOut(iot)/temp_denom;
  SecStageTF_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  SecStageTF_FwIn=regtemp.GetMinFwIn(iot)/temp_denom;
  SecStageTF_EwIn=regtemp.GetMaxEwIn(iot)/temp_denom;
  if(FTKTimeSim_Debug>2) cout<<"SecStageTF_FwIn = "<<SecStageTF_FwIn<<",   SecStageTF_FwOut = "<<SecStageTF_FwOut<<",   SecStageTF_EwIn = "<<SecStageTF_EwIn<<",   SecStageTF_EwOut = "<<SecStageTF_EwOut<<endl;
  h_SecStageTF_EwOut->Fill(SecStageTF_EwOut);
  iot.clear();
  for(int i=0 ; i<NSECSTAGETRACKFITTER; i++) iot.push_back(region[max_region].GetSecStageTrackFitter(i).GetPre(i));
  pre_SecStageTF_EwOut=regtemp.GetMaxEwOut(iot)/temp_denom;
  iot.clear();
  cout<<"check"<<region[max_region].iv.LVL1Gap<<endl;
  double l1gap =region[max_region].iv.LVL1Gap/temp_denom;

  // ** history plot ** //
  for(int i=0 ; i<NPU ; i++){
    // DO //
    for(int j=0 ; j<NDATAORGANIZER; j++) iot.push_back(region[i].GetDataOrganizer(j).Get(j));
    DO_EwOut_vec[i].push_back(regtemp.GetMaxEwOut(iot)/temp_denom);
    iot.clear();
    // AM //
    for(int j=0 ; j<NASSOCIATIVEMEMORY; j++) iot.push_back(region[i].GetAssociativeMemory(j).Get(j));
    AM_EwOut_vec[i].push_back(regtemp.GetMaxEwOut(iot)/temp_denom);
    iot.clear();
    // TF //
    for(int j=0 ; j<NDATAORGANIZER; j++) iot.push_back(region[i].GetSecDataOrganizer(j).Get(j));
    SecDO_EwOut_vec[i].push_back(regtemp.GetMaxEwOut(iot)/temp_denom);
    iot.clear();
    // SecDO //
    for(int j=0 ; j<NTRACKFITTER; j++) iot.push_back(region[i].GetTrackFitter(j).Get(j));
    TF_EwOut_vec[i].push_back(regtemp.GetMaxEwOut(iot)/temp_denom);
    iot.clear();
    // SecStage SecDO //
    for(int j=0 ; j<NSECSTAGEDATAORGANIZER; j++) iot.push_back(region[i].GetSecStageSecDataOrganizer(j).Get(j));
    SecStageSecDO_EwOut_vec[i].push_back(regtemp.GetMaxEwOut(iot)/temp_denom);
    iot.clear();
    // SecStage TF //
    for(int j=0 ; j<NSECSTAGETRACKFITTER; j++) iot.push_back(region[i].GetSecStageTrackFitter(j).Get(j));
    SecStageTF_EwOut_vec[i].push_back(regtemp.GetMaxEwOut(iot)/temp_denom);
    iot.clear();
  }

  // ** maxregion ** //
  h_maxftkhit->Fill(region[max_region].iv.nhit_nohw_tot);
  h_maxroad->Fill(region[max_region].iv.nroad_norw);
  h_maxfit->Fill(region[max_region].iv.nfit);
  h_SecStage_maxroad->Fill(region[max_region].iv.SecStage_nroad_norw);
  h_SecStage_maxfit->Fill(region[max_region].iv.SecStage_nfit);
  if(FTKTimeSim_Debug>2){
    cout<<"# of hit of MaxRegion = "<<region[max_region].iv.nhit_nohw_tot<<endl;
    cout<<"# of road of MaxRegion = "<<region[max_region].iv.nroad_norw<<endl;
    cout<<"# of fit of MaxRegion = "<<region[max_region].iv.nfit<<endl;
    if(s_split)cout<<"# of road of MaxRegion = "<<region[max_region].iv.nroad_norw<<endl;
    if(s_split)cout<<"# of fit of MaxRegion = "<<region[max_region].iv.nfit<<endl;
  }
  // ***splot for buffer size study *** //
  //  cout<<region[0].GetDataOrganizer(0).Get(0).GetBuff()<<"  check for region0 DO0 buff"<<endl;
  for(int i=0 ; i<NPU; i++){
    //    for(int i=0 ; i<NDATAORGANIZER; i++){
    //    }
    for(int j=0 ; j<NDATAORGANIZER; j++){
      DO_buff_vec[i][j].push_back(region[i].GetDataOrganizer(j).Get(j).GetBuff());
      SecDO_buff_vec[i][j].push_back(region[i].GetSecDataOrganizer(j).Get(j).GetBuff());
      SecDO_buff2_vec[i][j].push_back(region[i].GetSecDataOrganizer(j).Get(j).GetBuff2());
    }
    for(int j=0 ; j<NASSOCIATIVEMEMORY; j++){
      AM_buff_vec[i][j].push_back(region[i].GetAssociativeMemory(j).Get(j).GetBuff());
    }
    for(int j=0 ; j<NTRACKFITTER; j++){
      TF_buff_vec[i][j].push_back(region[i].GetTrackFitter(j).Get(j).GetBuff());
    }
  }

  
  // *** special plot for detail each event times *** //
  if(dump && !s_split){
  //  if(dump){
    cout<<"GGGG this is dumped event GGGG"<<endl;

    gStyle->SetPadGridX(1);
    gStyle->SetOptStat(0);
    TH2D *h_t = new TH2D("h_time","procesing time (latest region)",100,0,TF_EwOut*1.2, 10, -0.5, 9.5);
    if(temp_denom==1)h_t->GetXaxis()->SetNdivisions(054);
    h_t->SetTitle("");
    TCanvas *c_t = new TCanvas("c_t","c_t",1000,1000);
    c_t->cd(1);
    h_t->GetYaxis()->SetBinLabel(10,"DF");
    //    h_t->GetYaxis()->SetBinLabel(10,"");
    h_t->GetYaxis()->SetBinLabel(9,"DO write");
    h_t->GetYaxis()->SetBinLabel(8,"AM");
    h_t->GetYaxis()->SetBinLabel(7,"DO read");
    h_t->GetYaxis()->SetBinLabel(6,"TF");
    h_t->GetYaxis()->SetBinLabel(5,"2ndStage");
    h_t->GetYaxis()->SetBinLabel(4,"");
    h_t->GetYaxis()->SetBinLabel(3,"128 PUs");
    h_t->GetYaxis()->SetBinLabel(2," ");
    h_t->GetYaxis()->SetBinLabel(1,"total time"); //2013
    if(temp_denom==1000)h_t->SetXTitle("Latency (#mu sec)");
    if(temp_denom==1)h_t->SetXTitle("n sec");
    gStyle->SetOptStat(0);
    h_t->Draw();
    gStyle->SetOptStat(0);
    // line previous event
    TLine l_pre;
    l_pre.SetLineColor(48);
    l_pre.SetLineWidth(50);
    l_pre.SetLineStyle(3);
    //    if(pre_DF+_EwOut-l1gap>0) l_pre.DrawLine(0,9,pre_DF_EwOut-l1gap,9);
    if(pre_DO_EwOut-l1gap>0) l_pre.DrawLine(0,8,pre_DO_EwOut-l1gap,8);
    if(pre_AM_EwOut-l1gap>0) l_pre.DrawLine(0,7,pre_AM_EwOut-l1gap,7);
    if(pre_SecDO_EwOut-l1gap>0) l_pre.DrawLine(0,6,pre_SecDO_EwOut-l1gap,6);
    if(pre_TF_EwOut-l1gap>0) l_pre.DrawLine(0,5,pre_TF_EwOut-l1gap,5);
    if(pre_SecStageTF_EwOut-l1gap>0) l_pre.DrawLine(0,4,pre_SecStageTF_EwOut-l1gap,4);
    
    
    // line tot
    TLine l_tot;
    l_tot.SetLineColor(13);
    l_tot.SetLineWidth(40);
    l_tot.DrawLine(DF_FwIn,9,DF_EwOut,9);
    l_tot.DrawLine(DO_FwIn,8,DO_EwOut,8);
    l_tot.DrawLine(AM_FwIn,7,AM_EwOut,7);
    l_tot.DrawLine(SecDO_FwIn,6,SecDO_EwOut,6);
    l_tot.DrawLine(TF_FwIn,5,TF_EwOut,5);
    l_tot.DrawLine(SecStageTF_FwIn,4,SecStageTF_EwOut,4);
    // lien fw
    TLine l_fw;
    l_fw.SetLineColor(2);
    l_fw.SetLineWidth(30);
    l_fw.DrawLine(DF_FwIn,9,DF_FwOut,9);
    l_fw.DrawLine(DO_FwIn,8,DO_FwOut,8);
    l_fw.DrawLine(AM_FwIn,7,AM_FwOut,7);
    l_fw.DrawLine(SecDO_FwIn,6,SecDO_FwOut,6);
    l_fw.DrawLine(TF_FwIn,5,TF_FwOut,5);
    l_fw.DrawLine(SecStageTF_FwIn,4,SecStageTF_FwOut,4);
    if(DF_FwIn==DF_FwOut)    l_fw.DrawLine(DF_FwIn,9,DF_FwOut+1,9);
    if(DO_FwIn==DO_FwOut)    l_fw.DrawLine(DO_FwIn, 8,DO_FwOut+1, 8);
    if(AM_FwIn==AM_FwOut)    l_fw.DrawLine(AM_FwIn,7,AM_FwOut+1,7);
    if(SecDO_FwIn==SecDO_FwOut)    l_fw.DrawLine(SecDO_FwIn,6,SecDO_FwOut+1,6);
    if(TF_FwIn==TF_FwOut)    l_fw.DrawLine(TF_FwIn,5,TF_FwOut+1,5);
    if(SecStageTF_FwIn==SecStageTF_FwOut)    l_fw.DrawLine(SecStageTF_FwIn,4,SecStageTF_FwOut+1,4);    
    
    // line ew
    TLine l_ew;
    l_ew.SetLineColor(4);
    //    l_ew.SetLineWidth(15);
    l_ew.SetLineWidth(15);
    l_ew.DrawLine(DF_EwIn,9,DF_EwOut,9);
    l_ew.DrawLine(DO_EwIn,8,DO_EwOut,8);
    l_ew.DrawLine(AM_EwIn,7,AM_EwOut,7);
    l_ew.DrawLine(SecDO_EwIn,6,SecDO_EwOut,6);
    l_ew.DrawLine(TF_EwIn,5,TF_EwOut,5);
    l_ew.DrawLine(SecStageTF_EwIn,4,SecStageTF_EwOut,4);

    
    TLine l_wedge;
    l_wedge.SetLineColor(6);
    //    l_wedge.SetLineWidth(3); //2013
    l_wedge.SetLineWidth(1);
    for(int i=0 ; i<NPU ; i++){
      //      l_wedge.DrawLine(0, 2.0+(double)i/10, region[i].GetEwOut()/temp_denom, 2.0+(double)i/10); //2013
      //      l_wedge.DrawLine(0, 2.0+(double)i/30, region[i].GetEwOut()/temp_denom, 2.0+(double)i/30);
      //      l_wedge.DrawLine(0, 1.0+(double)i/30, region[i].GetEwOut()/temp_denom, 1.0+(double)i/30);
      //      l_wedge.DrawLine(0, 0.7+(double)i/30, region[i].GetEwOut()/temp_denom, 0.7+(double)i/30);
      l_wedge.DrawLine(0, 0.5+(double)i/45, region[i].GetEwOut()/temp_denom, 0.5+(double)i/45);
    }

    TLine l_exp;
    l_exp.SetLineColor(4);
    l_exp.SetLineWidth(30);
    //    l_exp.DrawLine(0, 1, region[max_region].GetEwOut()/temp_denom, 1);
    l_exp.DrawLine(0, 0, region[max_region].GetEwOut()/temp_denom, 0);

    // for legend 
    TH1D *h_1 = new TH1D("h_1","h_1",10,0,10);
    h_1->SetLineColor(2);
    h_1->SetLineWidth(10);
    TH1D *h_2 = new TH1D("h_2","h_2",10,0,10);
    h_2->SetLineColor(4);
    h_2->SetLineWidth(10);
    TLegend *leg = new TLegend(0.7,0.7, 0.9, 0.9);
    leg->AddEntry(h_1, "First Word");
    leg->AddEntry(h_2, "End   Word");
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->Draw();

    char par[256];
    sprintf(par,"loop# = %d", jentry);
    TLatex *tex = new TLatex(0, -1.5, par);
    tex->SetTextSize(0.025);
    tex->Draw("same");

    char par2[256];
    sprintf(par2,"hits = %d,  roads = %d, fits = %d,  tower = %d",
	    (int)region[max_region].iv.nhit_nohw_tot,
	    (int)region[max_region].iv.nroad_norw,
	    (int)region[max_region].iv.nfit,
	    max_region);
    TLatex *tex2 = new TLatex(0,10.0,par2);
    tex2->SetTextSize(0.03);
    tex2->Draw("same");

    //    char par[256];
    TLatex *stex = new TLatex(0, -2.0, description);
    stex->SetTextSize(0.025);
    if(0) stex->Draw("same");    

    //    char temp_outdir[256];
    sprintf(temp_outdir,"output/%s/c_time_detail_%03d.pdf",description,jentry);
    c_t->SaveAs(temp_outdir);
    //    sprintf(temp_outdir,"output/%s/C/c_time_detail_%03d.C",description,jentry);
    sprintf(temp_outdir,"output/%s/c_time_detail_%03d.C",description,jentry);
    if(dump_Cfile)c_t->SaveAs(temp_outdir);
    //    c_t->SaveAs("test.root");

  }// normal dump ----------------------------------
  // for Split
  if(dump && s_split){  // different name
    //  if(dump){

    gStyle->SetOptStat(0);
    cout<<"GGGG this is dumped event (split)GGGG"<<endl;
    gStyle->SetPadGridX(1);
    gStyle->SetOptStat(0);
    TH2D *h_t = new TH2D("h_time","procesing time (latest region)",100,0,SecStageTF_EwOut*1.2, 12, -0.5, 11.5);
    if(temp_denom==1)h_t->GetXaxis()->SetNdivisions(054);
    h_t->SetTitle("");
    TCanvas *c_t = new TCanvas("c_t","c_t",1000,1000);
    c_t->cd(1);
    h_t->GetYaxis()->SetBinLabel(12,"DF");
    h_t->GetYaxis()->SetBinLabel(11,"8LDO");
    h_t->GetYaxis()->SetBinLabel(10,"8LAM");
    h_t->GetYaxis()->SetBinLabel(9,"8LSecDO");
    h_t->GetYaxis()->SetBinLabel(8,"8LTF");
    h_t->GetYaxis()->SetBinLabel(7,"4LSecDO");
    h_t->GetYaxis()->SetBinLabel(6,"4LTF");
    h_t->GetYaxis()->SetBinLabel(5,"");
    h_t->GetYaxis()->SetBinLabel(4,"Other");
    h_t->GetYaxis()->SetBinLabel(3,"Region");
    h_t->GetYaxis()->SetBinLabel(2,"Expect");
    h_t->GetYaxis()->SetBinLabel(1," ");
    if(temp_denom==1000)h_t->SetXTitle("#mu sec");
    if(temp_denom==1)h_t->SetXTitle("n sec");
    h_t->Draw();
    gStyle->SetOptStat(0);
    // line previous event
    TLine l_pre;
    l_pre.SetLineColor(48);
    l_pre.SetLineWidth(30);
    l_pre.SetLineStyle(3);
    //    if(pre_DF_EwOut-l1gap>0)l_pre.DrawLine(0,11,pre_DF_EwOut-l1gap,11);
    if(pre_DO_EwOut-l1gap>0)l_pre.DrawLine(0,10,pre_DO_EwOut-l1gap,10);
    if(pre_AM_EwOut-l1gap>0)l_pre.DrawLine(0,9,pre_AM_EwOut-l1gap,9);
    if(pre_SecDO_EwOut-l1gap>0)l_pre.DrawLine(0,8,pre_SecDO_EwOut-l1gap,8);
    if(pre_TF_EwOut-l1gap>0)l_pre.DrawLine(0,7,pre_TF_EwOut-l1gap,7);
    if(pre_SecStageSecDO_EwOut-l1gap>0)l_pre.DrawLine(0,6,pre_SecStageSecDO_EwOut-l1gap,6);
    if(pre_SecStageTF_EwOut-l1gap>0)l_pre.DrawLine(0,5,pre_SecStageTF_EwOut-l1gap,5);

    
    // line tot
    TLine l_tot;
    l_tot.SetLineColor(13);
    l_tot.SetLineWidth(20);
    l_tot.DrawLine(DF_FwIn,11,DF_EwOut,11);
    l_tot.DrawLine(DO_FwIn,10,DO_EwOut,10);
    l_tot.DrawLine(AM_FwIn,9,AM_EwOut,9);
    l_tot.DrawLine(SecDO_FwIn,8,SecDO_EwOut,8);
    l_tot.DrawLine(TF_FwIn,7,TF_EwOut,7);
    l_tot.DrawLine(SecStageSecDO_FwIn,6,SecStageSecDO_EwOut,6);
    l_tot.DrawLine(SecStageTF_FwIn,5,SecStageTF_EwOut,5);
    // lien fw
    TLine l_fw;
    l_fw.SetLineColor(2);
    l_fw.SetLineWidth(10);
    l_fw.DrawLine(DF_FwIn,11,DF_FwOut,11);
    l_fw.DrawLine(DO_FwIn,10,DO_FwOut,10);
    l_fw.DrawLine(AM_FwIn,9,AM_FwOut,9);
    l_fw.DrawLine(SecDO_FwIn,8,SecDO_FwOut,8);
    l_fw.DrawLine(TF_FwIn,7,TF_FwOut,7);
    l_fw.DrawLine(SecStageSecDO_FwIn,6,SecStageSecDO_FwOut,6);
    l_fw.DrawLine(SecStageTF_FwIn,5,SecStageTF_FwOut,5);
    if(DF_FwIn==DF_FwOut)    l_fw.DrawLine(DF_FwIn,11,DF_FwOut+1,11);
    if(DO_FwIn==DO_FwOut)    l_fw.DrawLine(DO_FwIn, 10,DO_FwOut+1, 10);
    if(AM_FwIn==AM_FwOut)    l_fw.DrawLine(AM_FwIn,9,AM_FwOut+1,9);
    if(SecDO_FwIn==SecDO_FwOut)    l_fw.DrawLine(SecDO_FwIn,8,SecDO_FwOut+1,8);
    if(TF_FwIn==TF_FwOut)    l_fw.DrawLine(TF_FwIn,7,TF_FwOut+1,7);
    if(SecStageSecDO_FwIn==SecStageSecDO_FwOut)    l_fw.DrawLine(SecStageSecDO_FwIn,6,SecStageSecDO_FwOut+1,6);
    if(SecStageTF_FwIn==SecStageTF_FwOut)    l_fw.DrawLine(SecStageTF_FwIn,5,SecStageTF_FwOut+1,5);
    
    // line ew
    TLine l_ew;
    l_ew.SetLineColor(4);
    l_ew.SetLineWidth(6);
    l_ew.DrawLine(DF_EwIn,11,DF_EwOut,11);
    l_ew.DrawLine(DO_EwIn,10,DO_EwOut,10);
    l_ew.DrawLine(AM_EwIn,9,AM_EwOut,9);
    l_ew.DrawLine(SecDO_EwIn,8,SecDO_EwOut,8);
    l_ew.DrawLine(TF_EwIn,7,TF_EwOut,7);
    l_ew.DrawLine(SecStageSecDO_EwIn,6,SecStageSecDO_EwOut,6);
    l_ew.DrawLine(SecStageTF_EwIn,5,SecStageTF_EwOut,5);

    
//     cout<<TF_EwIn<<" "<<TF_EwOut<<endl;
//     cout<<SecStageSecDO_EwIn<<" "<<SecStageSecDO_EwOut<<endl;
//     cout<<SecStageTF_EwIn<<" "<<SecStageTF_EwOut<<endl;
    
    TLine l_wedge;
    l_wedge.SetLineColor(6);
    l_wedge.SetLineWidth(3);
    for(int i=0 ; i<NPU ; i++){
      l_wedge.DrawLine(0, 2.0+(double)i/10, region[i].GetEwOut()/temp_denom, 2.0+(double)i/10);
    }


    //    TLine l_exp;
    //    l_exp.SetLineColor(4);
    //    l_exp.SetLineWidth(20);
    //    l_exp.DrawLine(0, 1, region[max_region].GetEwOut()/temp_denom, 1);

    // for legend 
    TH1D *h_1 = new TH1D("h_1","h_1",10,0,10);
    h_1->SetLineColor(2);
    h_1->SetLineWidth(10);
    TH1D *h_2 = new TH1D("h_2","h_2",10,0,10);
    h_2->SetLineColor(4);
    h_2->SetLineWidth(10);
    TLegend *leg = new TLegend(0.7,0.75, 0.9, 0.9);
    leg->AddEntry(h_1, "First Word");
    leg->AddEntry(h_2, "End   Word");
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->Draw();

    char par[256];
    sprintf(par,"loop# = %d", jentry);
    TLatex *tex = new TLatex(0, -1.5, par);
    tex->SetTextSize(0.025);
    tex->Draw("same");

    char par2[256];
    sprintf(par2,"hit = %d,  road = %d, combi = %d,  region = %d",
	    (int)region[max_region].iv.nhit_nohw_tot,
	    (int)region[max_region].iv.nroad_norw,
	    (int)region[max_region].iv.nfit,
	    max_region);
    TLatex *tex2 = new TLatex(0,12.0,par2);
    tex2->SetTextSize(0.03);
    tex2->Draw("same");

    TLatex *stex = new TLatex(0, -2.0, description);
    stex->SetTextSize(0.025);
    if(0)stex->Draw("same");    

    sprintf(temp_outdir,"output/%s/c_time_split_detail_%d.pdf",description,jentry);
    c_t->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_time_split_detail_%d.C",description,jentry);
    if(dump_Cfile)c_t->SaveAs(temp_outdir);

    gStyle->SetOptStat(1001110);
  } // *** end of special plot for detail each event times *** //

} 

void
FTKTimeSim::PrintResults(double sf_hit_overlapregion, int nloop, int nloopsloop, double sf_nregion, char *description2, int ntrack_ok, int ntrack_tot, int s_split) 
{
  char temp_outdir[256];
  sprintf(temp_outdir,"output/%s/summary_table.txt",description2);
  ofstream fout(temp_outdir);
  double temp_ntimeregion = 0;
  cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
  cout<<"**** FTKTimeSim::PrintResults ****"<<endl;
  cout<<"*** "<<description2<<" ***"<<endl;
  cout<<"*** Setting ***"<<endl;
  cout<<"Split                      : "<<s_split<<endl;
  //  cout<<"sf for minimum overlap     : "<<sf_hit_overlapregion<<endl;
  //  cout<<"sf_more_region (x/8region) : "<<sf_nregion<<"  ("<<(360./(sf_nregion*55. -10.))<<" Region)"<<endl;
  cout<<"sf_more_region (x/8region) : "<<sf_nregion<<endl;
  cout<<"# of loop (0 is full)      : "<<nloop<<endl;
  cout<<"# of loop of event loop    : "<<nloopsloop<<endl;
  //cout<<region[0].GetDataOrganizer(0).Get(0).GetBuff()<<"  check for region0 DO0 buff"<<endl;  
  cout<<"LVL1Gap              (nsec): "<<region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap
      <<"   ("<<1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap<<" kHz)"<<endl;
  cout<<"TF process dead line (nsec): "<<region[0].GetDataOrganizer(0).GetInputValue().t_kill_TF<<endl;
  cout<<"*** board info ***"<<endl;
  cout<<"# of Region                : "<<NREGION<<endl;
  cout<<"# of processor unit        : "<<NPU<<endl;
  cout<<"# of NLAYER                : "<<NLAYER<<endl;
  cout<<"-------------------------------------------"<<endl;
  cout<<"# of DF                    : "<<setw(4)<<NDATAFORMATTER<<"   with each "
      <<endl;
  //      <<1000000000./(double)region[0].GetDataFormatter(0).Get(0).Gettemp_proc()<<" MHz"<<endl;
  cout<<"# of DO                    : "<<setw(4)<<NDATAORGANIZER<<"   with each "
    //      <<endl;
    //    <<1000000./region[0].GetDataOrganizer(3).Get(3).Gettemp_proc()<<endl;
      <<1000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc()<<" MHz"<<endl;
  cout<<"# of AM                    : "<<setw(4)<<NASSOCIATIVEMEMORY<<"   with each "
      <<1000./(double)region[0].GetAssociativeMemory(0).Get(0).Gettemp_proc()<<" MHz"<<endl;
  cout<<"# of TF (1 boad hvae "<<NGF<<" GF) : "<<setw(4)<<NTRACKFITTER/NGF<<" with each "
      <<1000./(double)region[0].GetTrackFitter(0).Get(0).Gettemp_proc()*NGF<<" MHz"<<endl;
  cout<<"-------------------------------------------"<<endl;
  temp_ntimeregion = (360./(sf_nregion*55. -10.))/8.;
  //  cout<<"(360./("<<sf_nregion<<"*55. -10.))/8.;"<<endl;
  cout<<"# of total boad            : "<<//temp_ntimeregion<<"     "<<
    //    (NDATAFORMATTER+NDATAORGANIZER+NASSOCIATIVEMEMORY+NTRACKFITTER/NGF)*NPU*temp_ntimeregion<<endl;
    "-- "<<endl;
  cout<<"*** hits roads combi fits ***"<<endl;
////  cout<<" mean # of hits(11L)/subregion : "<<setw(12)<<h_ftkhit->GetMean()
////      <<"   (acceptable for DO     "
  
//       <<(1000000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc())*(double)NDATAORGANIZER<<endl;
//   cout<<(1000000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc())<<endl;
//   cout<<(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc()<<endl;
////       <<((1000000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc())*NDATAORGANIZER) /
////     (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  cout<<" mean # of hit(l0) / tower     : "<<setw(12)<<h_ftkhit_layer[0]->GetMean()<<"    (acceptable 1k hits @ 100MHz 10msec)"<<endl;
  cout<<" mean # of ss(8L)/ subregion   : "<<setw(12)<<h_ftkss->GetMean()<<endl;
  cout<<" mean # of roads / subregion   : "<<setw(12)<<h_road->GetMean()
        <<"   (acceptable for AM etc "
      <<((1000000./(double)region[0].GetAssociativeMemory(0).Get(0).Gettemp_proc())*NASSOCIATIVEMEMORY) /
    (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  cout<<" mean # of roads after RW      : "<<setw(12)<<h_road_rw->GetMean()<<endl;
  cout<<" mean # of combi / subregion   : "<<setw(12)<<h_fit->GetMean()
      <<"   (acceptable for TF     "
      <<((1000000./(double)region[0].GetTrackFitter(0).Get(0).Gettemp_proc())*NTRACKFITTER) /
    (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  if(s_split){
  cout<<" mean # of roads (4L)/subregion: "<<setw(12)<<h_SecStage_road->GetMean()       <<endl;
//       <<"   (acceptable for AM etc "
//       <<((1000000./(double)region[0].GetAssociativeMemory(0).Get(0).Gettemp_proc())*NASSOCIATIVEMEMORY) /
//     (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  cout<<" mean # of combi (4L)/subregion: "<<setw(12)<<h_SecStage_fit->GetMean()      <<endl;



//       <<"   (acceptable for TF     "
//       <<((1000000./(double)region[0].GetTrackFitter(0).Get(0).Gettemp_proc())*NTRACKFITTER) /
//     (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  }

  cout<<"-------------------------------------------"<<endl;  
  cout<<"***TrackEff is "<<(double)ntrack_ok/(double)ntrack_tot<<"     "<<(double)ntrack_ok<<" / "<<(double)ntrack_tot<<endl;  
  cout<<"-------------------------------------------"<<endl;
  cout<<"*** Total Pcessing time of Last Event of each Region (micro sec) ***"<<endl;
  cout<<"DF Ew Out    = "<<DF_EwOut<<endl;
  cout<<"DO Ew Out    = "<<DO_EwOut<<endl;
  cout<<"AM Ew Out    = "<<AM_EwOut<<endl;
  cout<<"SecDO Ew Out = "<<SecDO_EwOut<<endl;
  cout<<"TF Ew Out    = "<<TF_EwOut<<endl;
  cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl; 
  // ============================================================================================================
  // ============================================================================================================
  // ============================================================================================================
  // ============================================================================================================
  


  fout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl;
  fout<<"**** FTKTimeSim::PrintResults ****"<<endl;
  fout<<"*** "<<description2<<" ***"<<endl;
  fout<<"*** Setting ***"<<endl;
  fout<<"Split                      : "<<s_split<<endl;
  fout<<"sf for minimum overlap     : "<<sf_hit_overlapregion<<endl;
  fout<<"sf_more_region (x/8region) : "<<sf_nregion<<"  ("<<(360./(sf_nregion*55. -10.))<<" Region)"<<endl;
  fout<<"# of loop (0 is full)      : "<<nloop<<endl;
  fout<<"# of loop of event loop    : "<<nloopsloop<<endl;
  //fout<<region[0].GetDataOrganizer(0).Get(0).GetBuff()<<"  check for region0 DO0 buff"<<endl;  
  fout<<"LVL1Gap              (nsec): "<<region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap
      <<"   ("<<1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap<<" kHz)"<<endl;
  fout<<"TF process dead line (nsec): "<<region[0].GetDataOrganizer(0).GetInputValue().t_kill_TF<<endl;
  fout<<"*** board info ***"<<endl;
  fout<<"# of Region                : "<<NREGION<<endl;
  fout<<"# of processor unit        : "<<NPU<<endl;
  fout<<"-------------------------------------------"<<endl;
  fout<<"# of DF                    : "<<setw(4)<<NDATAFORMATTER<<"   with each "
      <<endl;
  //      <<1000000000./(double)region[0].GetDataFormatter(0).Get(0).Gettemp_proc()<<" MHz"<<endl;
  fout<<"# of DO                    : "<<setw(4)<<NDATAORGANIZER<<"   with each "
    //      <<endl;
    //    <<1000000./region[0].GetDataOrganizer(3).Get(3).Gettemp_proc()<<endl;
      <<1000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc()<<" MHz"<<endl;
  fout<<"# of AM                    : "<<setw(4)<<NASSOCIATIVEMEMORY<<"   with each "
      <<1000./(double)region[0].GetAssociativeMemory(0).Get(0).Gettemp_proc()<<" MHz"<<endl;
  fout<<"# of TF (1 boad hvae "<<NGF<<" GF) : "<<setw(4)<<NTRACKFITTER/NGF<<" with each "
      <<1000./(double)region[0].GetTrackFitter(0).Get(0).Gettemp_proc()*NGF<<" MHz"<<endl;
  fout<<"-------------------------------------------"<<endl;
  temp_ntimeregion = (360./(sf_nregion*55. -10.))/8.;
  //  fout<<"(360./("<<sf_nregion<<"*55. -10.))/8.;"<<endl;
  fout<<"# of total boad            : "<<//temp_ntimeregion<<"     "<<
    //    (NDATAFORMATTER+NDATAORGANIZER+NASSOCIATIVEMEMORY+NTRACKFITTER/NGF)*NPU*temp_ntimeregion<<endl;
    "-- "<<endl;
  fout<<"*** hits roads combi fits ***"<<endl;
////  fout<<" mean # of hits(11L)/subregion : "<<setw(12)<<h_ftkhit->GetMean()
////      <<"   (acceptable for DO     "
  
//       <<(1000000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc())*(double)NDATAORGANIZER<<endl;
//   fout<<(1000000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc())<<endl;
//   fout<<(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc()<<endl;
////       <<((1000000./(double)region[0].GetDataOrganizer(0).Get(0).Gettemp_proc())*NDATAORGANIZER) /
////     (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  fout<<" mean # of hit(l0) / tower     : "<<setw(12)<<h_ftkhit_layer[0]->GetMean()<<"    (acceptable 1k hits @ 100MHz 10msec)"<<endl;
  fout<<" mean # of ss(8L)/ subregion   : "<<setw(12)<<h_ftkss->GetMean()<<endl;
  fout<<" mean # of roads / subregion   : "<<setw(12)<<h_road->GetMean()
        <<"   (acceptable for AM etc "
      <<((1000000./(double)region[0].GetAssociativeMemory(0).Get(0).Gettemp_proc())*NASSOCIATIVEMEMORY) /
    (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  fout<<" mean # of roads after RW      : "<<setw(12)<<h_road_rw->GetMean()<<endl;
  fout<<" mean # of combi / subregion   : "<<setw(12)<<h_fit->GetMean()
      <<"   (acceptable for TF     "
      <<((1000000./(double)region[0].GetTrackFitter(0).Get(0).Gettemp_proc())*NTRACKFITTER) /
    (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  if(s_split){
  fout<<" mean # of roads (4L)/subregion: "<<setw(12)<<h_SecStage_road->GetMean()       <<endl;
//       <<"   (acceptable for AM etc "
//       <<((1000000./(double)region[0].GetAssociativeMemory(0).Get(0).Gettemp_proc())*NASSOCIATIVEMEMORY) /
//     (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  fout<<" mean # of combi (4L)/subregion: "<<setw(12)<<h_SecStage_fit->GetMean()      <<endl;



//       <<"   (acceptable for TF     "
//       <<((1000000./(double)region[0].GetTrackFitter(0).Get(0).Gettemp_proc())*NTRACKFITTER) /
//     (1000000./region[0].GetDataOrganizer(0).GetInputValue().LVL1Gap)<<")"<<endl;;
  }

  fout<<"-------------------------------------------"<<endl;  
  fout<<"***TrackEff is "<<(double)ntrack_ok/(double)ntrack_tot<<"     "<<(double)ntrack_ok<<" / "<<(double)ntrack_tot<<endl;  
  fout<<"-------------------------------------------"<<endl;
  fout<<"*** Total Pcessing time of Last Event of each Region (micro sec) ***"<<endl;
  fout<<"DF Ew Out    = "<<DF_EwOut<<endl;
  fout<<"DO Ew Out    = "<<DO_EwOut<<endl;
  fout<<"AM Ew Out    = "<<AM_EwOut<<endl;
  fout<<"SecDO Ew Out = "<<SecDO_EwOut<<endl;
  fout<<"TF Ew Out    = "<<TF_EwOut<<endl;
  fout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL"<<endl; 
  
  
}

//Input from root file simulated by FTKSim//
void
FTKTimeSim::PlotInputValues(char *description, int s_split, int nrbin)  //For Input Values 
{
  //  need change some parameter by hand... // 
  gStyle->SetOptStat(1001110);
  // *** switch of plot *** //
  bool plot_nftkhit_layer = true;
  bool plot_nroad = true;
  bool plot_nroad_rw = true;
  bool plot_nfit = true;
  bool plot_nfit_2nd = true;
  bool plot_nss = true;
  bool plot_vs = true;
  // ** temp  output file name ** //
  //  char temp_outdir[256];
  char temp_outdir[512];
  sprintf(temp_outdir,"mkdir -p output/%s",description);
  system(temp_outdir);
  sprintf(temp_outdir,"mkdir -p output/%s/C",description);
  system(temp_outdir);
  cout<<"----- ok 0"<<endl;
  //  *** plot start **/
  if(plot_nss){
    TCanvas *c_nftkss = new TCanvas("c_nftkss","c_nftkss",800,800);
    c_nftkss->cd(1);
    h_ftkss->SetXTitle("number of ss");
    h_ftkss->SetYTitle("events * regions");
    cout<<"----- ok 1"<<endl;
    h_ftkss->Rebin(nrbin*10);
    cout<<"----- ok 2"<<endl;
    //    h_ftkss->SetAxisRange(0,(int)(h_ftkss->GetMean()*2));
    //    h_ftkss->GetXaxis()->SetNdivisions(4);
    h_ftkss->Fit("gaus");
    h_ftkss->Draw();
    cout<<"----- ok 3"<<endl;
    sprintf(temp_outdir,"output/%s/c_nftkss.pdf",description);
    c_nftkss->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_nftkss.C",description);
    if(dump_Cfile)    c_nftkss->SaveAs(temp_outdir);

    // each layer //
    TCanvas *c_nftkss_layer = new TCanvas("c_nftkss_layer","c_nftkss_layer",1600,1200);
    c_nftkss_layer->Divide(4,3);
    for(int i=0 ; i<NLAYER ; i++){
      c_nftkss_layer->cd(i+1);
      h_ftkss_layer[i]->SetXTitle("number of ss");
      h_ftkss_layer[i]->SetYTitle("events * regions");
      h_ftkss_layer[i]->Rebin(nrbin);
      h_ftkss_layer[i]->SetAxisRange(0,(int)h_ftkss_layer[i]->GetMean()*2);
      h_ftkss_layer[i]->GetXaxis()->SetNdivisions(111);
      h_ftkss_layer[i]->Fit("gaus");
      h_ftkss_layer[i]->Draw();
    }
    sprintf(temp_outdir,"output/%s/c_nftkss_layer.pdf",description);
    c_nftkss_layer->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_nftkss_layer.C",description);
    if(dump_Cfile)    c_nftkss_layer->SaveAs(temp_outdir);


  }
  // ** plot for number of hit in each layer  ** //
  if(plot_nftkhit_layer){
    //  tot //
    TCanvas *c_nftkhit = new TCanvas("c_nftkhit","c_nftkhit",800,800);
    c_nftkhit->cd(1);
    h_ftkhit->SetXTitle("number of hit");
    h_ftkhit->SetYTitle("events * regions");
    h_ftkhit->Rebin(nrbin*10);
    h_ftkhit->SetAxisRange(0,(int)(h_ftkhit->GetMean()*2));
    h_ftkhit->GetXaxis()->SetNdivisions(4);
    h_ftkhit->Fit("gaus");
    h_ftkhit->Draw();
    sprintf(temp_outdir,"output/%s/c_nftkhit.pdf",description);
    c_nftkhit->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_nftkhit.C",description);
    if(dump_Cfile)    c_nftkhit->SaveAs(temp_outdir);
    // each layer //
    TCanvas *c_nftkhit_layer = new TCanvas("c_nftkhit_layer","c_nftkhit_layer",1600,1200);
    c_nftkhit_layer->Divide(4,3);
    for(int i=0 ; i<NLAYER ; i++){
      c_nftkhit_layer->cd(i+1);
      h_ftkhit_layer[i]->SetXTitle("number of hit");
      h_ftkhit_layer[i]->SetYTitle("events * regions");
      h_ftkhit_layer[i]->Rebin(nrbin);
      h_ftkhit_layer[i]->SetAxisRange(0,(int)h_ftkhit_layer[i]->GetMean()*2);
      h_ftkhit_layer[i]->GetXaxis()->SetNdivisions(111);
      h_ftkhit_layer[i]->Fit("gaus");
      h_ftkhit_layer[i]->Draw();
    }
    sprintf(temp_outdir,"output/%s/c_nftkhit_layer.pdf",description);
    c_nftkhit_layer->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_nftkhit_layer.C",description);
    if(dump_Cfile)    c_nftkhit_layer->SaveAs(temp_outdir);
  }

  // ** plot for number of road in each AM ** //
  if(plot_nroad){
    // tot // 
    TCanvas *c_road = new TCanvas("c_road","c_road",1600,1200);
    c_road->cd(1);
    h_road->SetXTitle("number of road");
    h_road->SetYTitle("events * regions");
    h_road->Rebin(nrbin*5);
    //    h_road->SetAxisRange(0,(int)(h_road->GetMean()*2));
    h_road->SetAxisRange(0,(int)(h_road->GetMean()*3));
    h_road->GetXaxis()->SetNdivisions(017);
    //    h_road->Fit("gaus");
    h_road->Draw();
    sprintf(temp_outdir,"output/%s/c_road.pdf",description);
    c_road->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_road.C",description);
    if(dump_Cfile)    c_road->SaveAs(temp_outdir);
    
    // for each AM //
    TCanvas *c_road_AM = new TCanvas("c_road_AM","c_road_AM",1600,1200);
    c_road_AM->Divide(4,3);    
    for(int i=0 ; i<NASSOCIATIVEMEMORY ; i++){
      c_road_AM->cd(i+1);
      h_road_AM[i]->SetXTitle("number of road");
      h_road_AM[i]->SetYTitle("events * regions");
      h_road_AM[i]->Rebin((int)(nrbin/2));
      //      h_road_AM[i]->SetAxisRange(0,(int)h_road_AM[i]->GetMean()*2);
      h_road_AM[i]->SetAxisRange(0,(int)h_road_AM[i]->GetMean()*3);
      h_road_AM[i]->GetXaxis()->SetNdivisions(111);
      //      h_road_AM[i]->Fit("gaus");
      h_road_AM[i]->Draw();
    }
    sprintf(temp_outdir,"output/%s/c_road_AM.pdf",description);
    c_road_AM->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_road_AM.C",description);
    if(dump_Cfile)    c_road_AM->SaveAs(temp_outdir);
   }

    // ** plot for number of road_rw in each AM ** //
  if(plot_nroad_rw){
    // tot // 
    TCanvas *c_road_rw = new TCanvas("c_road_rw","c_road_rw",1600,1200);
    c_road_rw->cd(1);
    h_road_rw->SetXTitle("number of road_rw");
    h_road_rw->SetYTitle("events * regions");
    h_road_rw->Rebin(nrbin*5);
    //    h_road_rw->SetAxisRange(0,(int)(h_road_rw->GetMean()*2));
    h_road_rw->SetAxisRange(0,(int)(h_road_rw->GetMean()*3));
    h_road_rw->GetXaxis()->SetNdivisions(4);
    //    h_road_rw->Fit("gaus");
    h_road_rw->Draw();
    sprintf(temp_outdir,"output/%s/c_road_rw.pdf",description);
    c_road_rw->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_road_rw.C",description);
    if(dump_Cfile)    c_road_rw->SaveAs(temp_outdir);
    
    // for each AM //
    TCanvas *c_road_rw_AM = new TCanvas("c_road_rw_AM","c_road_rw_AM",1600,1200);
    c_road_rw_AM->Divide(4,3);    
    for(int i=0 ; i<NASSOCIATIVEMEMORY ; i++){
      c_road_rw_AM->cd(i+1);
      h_road_rw_AM[i]->SetXTitle("number of road_rw");
      h_road_rw_AM[i]->SetYTitle("events * regions");
      h_road_rw_AM[i]->Rebin((int)(nrbin/2));
      h_road_rw_AM[i]->SetAxisRange(0,(int)h_road_rw_AM[i]->GetMean()*2);
      h_road_rw_AM[i]->GetXaxis()->SetNdivisions(014);
      //      h_road_rw_AM[i]->Fit("gaus");
      h_road_rw_AM[i]->Draw();
    }
    sprintf(temp_outdir,"output/%s/c_road_rw_AM.pdf",description);
    c_road_rw_AM->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_road_rw_AM.C",description);
    if(dump_Cfile)    c_road_rw_AM->SaveAs(temp_outdir);
   }
  cout<<"check!"<<endl;
  
  // ** plot for number of fit  ** //
  if(plot_nfit){
    TCanvas *c_fit = new TCanvas("c_fit","c_fit",1600,1200);
    c_fit->cd(1); 
    h_fit->SetXTitle("# of fit");
    h_fit->SetYTitle("events");
    h_fit->Rebin(200*nrbin);
    //    h_fit->Rebin((int)2);
    h_fit->SetAxisRange(0,(int)(h_fit->GetMean()*3));
    //    h_fit->SetAxisRange(0,(int)(h_fit->GetMean()*4));
    h_fit->GetXaxis()->SetNdivisions(4);
    //    h_fit->Fit("gaus");
    h_fit->Draw();
    cout<<"check6!"<<endl;   
    sprintf(temp_outdir,"output/%s/c_fit.pdf",description);
    c_fit->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_fit.C",description);
    if(dump_Cfile)    c_fit->SaveAs(temp_outdir);
  }

  // ** Vs plot ** //
  if(plot_vs){
    TCanvas *c_vs = new TCanvas("c_vs","c_vs",1600,1200);
    c_vs->Divide(2,2);
    c_vs->cd(1);
    h_ftkhit_vs_road->SetXTitle("# of hit");
    h_ftkhit_vs_road->SetYTitle("# of road");
    h_ftkhit_vs_road->SetAxisRange(0,(int)(h_ftkhit->GetMean()*2));
    h_ftkhit_vs_road->Draw();
    c_vs->cd(2);
    h_ftkhit_vs_fit->SetXTitle("# of hit");
    h_ftkhit_vs_fit->SetYTitle("# of road");
    h_ftkhit_vs_fit->SetAxisRange(0,(int)(h_ftkhit->GetMean()*2));
    h_ftkhit_vs_fit->Draw();
    c_vs->cd(3);
    h_road_vs_fit->SetXTitle("# of hit");
    h_road_vs_fit->SetYTitle("# of road");
    h_road_vs_fit->SetAxisRange(0,(int)(h_road->GetMean()*2));
    h_road_vs_fit->Draw();
    c_vs->cd(4);
		
    
    

    sprintf(temp_outdir,"output/%s/c_vs.pdf",description);
    c_vs->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_vs.C",description);
    if(dump_Cfile)    c_vs->SaveAs(temp_outdir);
  }

  
  if(plot_nfit_2nd){
    TCanvas *c_SecStage_fit = new TCanvas("c_SecStage_fit","c_SecStage_fit",1600,1200);
    c_SecStage_fit->cd(1);
    //      h_SecStage_fit->Rebin(200*nrbin);
    h_SecStage_fit->SetAxisRange(0,(int)(h_SecStage_fit->GetMean()*2));
    h_SecStage_fit->GetXaxis()->SetNdivisions(4);
    //    h_SecStage_fit->Fit("gaus");
    h_SecStage_fit->Draw();
    sprintf(temp_outdir,"output/%s/c_SecStage_fit.pdf",description);
    c_SecStage_fit->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_SecStage_fit.C",description);
    if(dump_Cfile)      c_SecStage_fit->SaveAs(temp_outdir);
  }
  



  // *** Special plot for split aertitecture *** //
  if(s_split){
    if(plot_nroad){
      // tot // 
      TCanvas *c_SecStage_road = new TCanvas("c_SecStage_road","c_SecStage_road",1600,1200);
      c_SecStage_road->cd(1);
      h_SecStage_road->SetXTitle("number of road");
      h_SecStage_road->SetYTitle("events * regions");
      h_SecStage_road->Rebin(5*nrbin);
      h_SecStage_road->SetAxisRange(0,(int)(h_SecStage_road->GetMean()*2));
      h_SecStage_road->GetXaxis()->SetNdivisions(4);
      //    h_SecStage_road->Fit("gaus");
      h_SecStage_road->Draw();
      sprintf(temp_outdir,"output/%s/c_SecStage_road.pdf",description);
      c_SecStage_road->SaveAs(temp_outdir);
      sprintf(temp_outdir,"output/%s/C/c_SecStage_road.C",description);
      if(dump_Cfile)      c_SecStage_road->SaveAs(temp_outdir);
      
      // for each AM //
      TCanvas *c_SecStage_road_AM = new TCanvas("c_SecStage_road_AM","c_SecStage_road_AM",1600,1200);
      c_SecStage_road_AM->Divide(4,3);    
      for(int i=0 ; i<NASSOCIATIVEMEMORY ; i++){
	c_SecStage_road_AM->cd(i+1);
	h_SecStage_road_AM[i]->SetXTitle("number of road");
	h_SecStage_road_AM[i]->SetYTitle("events * regions");
	h_SecStage_road_AM[i]->Rebin((int)(nrbin/2));
	h_SecStage_road_AM[i]->SetAxisRange(0,(int)h_SecStage_road_AM[i]->GetMean()*2);
	h_SecStage_road_AM[i]->GetXaxis()->SetNdivisions(111);
	//      h_SecStage_road_AM[i]->Fit("gaus");
	h_SecStage_road_AM[i]->Draw();
      }
      sprintf(temp_outdir,"output/%s/c_SecStage_road_AM.pdf",description);
      c_SecStage_road_AM->SaveAs(temp_outdir);
      sprintf(temp_outdir,"output/%s/C/c_SecStage_road_AM.C",description);
      if(dump_Cfile)      c_SecStage_road_AM->SaveAs(temp_outdir);
    }
    // ** plot for number of fit  ** //
    if(plot_nfit){
      TCanvas *c_SecStage_fit = new TCanvas("c_SecStage_fit","c_SecStage_fit",1600,1200);
      c_SecStage_fit->cd(1);
      //      h_SecStage_fit->Rebin(200*nrbin);
      h_SecStage_fit->SetAxisRange(0,(int)(h_SecStage_fit->GetMean()*2));
      h_SecStage_fit->GetXaxis()->SetNdivisions(4);
      //    h_SecStage_fit->Fit("gaus");
      h_SecStage_fit->Draw();
      sprintf(temp_outdir,"output/%s/c_SecStage_fit.pdf",description);
      c_SecStage_fit->SaveAs(temp_outdir);
      sprintf(temp_outdir,"output/%s/C/c_SecStage_fit.C",description);
      if(dump_Cfile)c_SecStage_fit->SaveAs(temp_outdir);
    }
  }

}


void
FTKTimeSim::PlotResults(char *description, int temp_denom, int s_split, int nrbin)  // For Results
{
  // ** switch of plot **/
  bool plot_timeee = true;
  bool plot_each_ewout = true;
  bool plot_maxregion = true;
  bool plot_history = true;

  char temp_outdir[256];
  sprintf(temp_outdir,"mkdir -p output/%s",description);
  system(temp_outdir);
  sprintf(temp_outdir,"mkdir -p output/%s/C",description);
  //  system(temp_outdir);
  // ** description ** //
  //  TLatex *tex = new TLatex(0, -2, description);
  TLatex *tex = new TLatex(0, -2, ""); // remove tex
  tex->SetTextSize(0.025);



  //  int rebin=40; // for single mu
  int rebin=nrbin; // for WHbb
  //  int rebin=40000; // for WHbb nano
  //  *** plot start **/
  if(plot_timeee){
    cout<<"Special comment Mean timeeee = "<<h_timeee->GetMean()<<endl;
    //    gStyle->SetOptStat(0111001);
    gStyle->SetOptStat(1001110);
    //    gStyle->SetOptStat(1111111);
    TCanvas *c_ntimeee = new TCanvas("c_ntimeee","c_ntimeee",800,800);
    c_ntimeee->cd(1);
    if(temp_denom==1000)h_timeee->SetXTitle("Latency (#mu sec)");
    if(temp_denom==1)h_timeee->SetXTitle("Latency time (#mu sec");
    h_timeee->SetYTitle("events");
    //h_timeee->Rebin(rebin+1);
    h_timeee->Rebin(rebin);
    h_timeee->SetAxisRange(0,(int)(h_timeee->GetMean()*2.5));
    //    h_timeee->GetXaxis()->SetNdivisions(4);
    //    h_timeee->Fit("gaus");
    h_timeee->Draw();
    tex->Draw("same");
    sprintf(temp_outdir,"output/%s/c_ntimeee.pdf",description);
    c_ntimeee->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_ntimeee.C",description);
    if(dump_Cfile)    c_ntimeee->SaveAs(temp_outdir);
  }
  if(plot_timeee){
    //    gStyle->SetOptStat(0111001);
    gStyle->SetOptStat(1001110);
    TCanvas *c_ntimeee_full = new TCanvas("c_ntimeee_full","c_ntimeee_full",800,800);
    c_ntimeee_full->cd(1);
    if(temp_denom==1000)h_timeee->SetXTitle("Latency (#mu sec)");
    if(temp_denom==1)h_timeee->SetXTitle("Latency (#mu sec)");
    h_timeee->SetYTitle("events");
    //    h_timeee->Rebin(rebin);
    //    h_timeee->SetAxisRange(0,25000); //default
    //    h_timeee->SetAxisRange(0,40);
    //h_timeee->SetAxisRange(0,200);

    //    h_timeee->GetXaxis()->SetNdivisions(4);
    //    h_timeee->Fit("gaus");
    h_timeee->Draw();

    tex->Draw("same");
    sprintf(temp_outdir,"output/%s/c_ntimeee_full.pdf",description);
    c_ntimeee_full->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_ntimeee_full.C",description);
    if(dump_Cfile)    c_ntimeee_full->SaveAs(temp_outdir);
  }

  if(plot_each_ewout){
    gStyle->SetOptStat(0);
    TCanvas *c_event_detail = new TCanvas("c_event_detail","c_event_detail",800,800);
    c_event_detail->cd(1);
    if(temp_denom==1000)h_TF_EwOut->SetXTitle("#mu sec");
    if(temp_denom==1)h_TF_EwOut->SetXTitle("n sec");
    h_TF_EwOut->SetYTitle("events");

    h_DF_EwOut->Rebin(rebin);
    h_DO_EwOut->Rebin(rebin);
    h_AM_EwOut->Rebin(rebin);
    h_SecDO_EwOut->Rebin(rebin);
    h_TF_EwOut->Rebin(rebin);
		    
    h_TF_EwOut->SetAxisRange(0,(int)(h_TF_EwOut->GetMean()*1.5));
    h_TF_EwOut->GetXaxis()->SetNdivisions(054);
    //    h_TF_EwOut->Fit("gaus");
    h_TF_EwOut->SetTitle("EndWord Out Timing");
    if(s_split){
      h_SecStageTF_EwOut->SetLineWidth(2);
      h_SecStageTF_EwOut->SetMaximum(h_SecStageTF_EwOut->GetMaximum()*1.3);
      h_SecStageTF_EwOut->SetLineColor(2);
      h_SecStageTF_EwOut->Draw();
      h_DF_EwOut->SetLineColor(17);
      h_DF_EwOut->Draw("same");
      h_DO_EwOut->SetLineColor(6);
      h_DO_EwOut->SetLineWidth(8);
      h_DO_EwOut->Draw("same");
      h_AM_EwOut->SetLineColor(4);
      h_AM_EwOut->SetLineWidth(6);
      h_AM_EwOut->Draw("same");
      h_SecDO_EwOut->SetLineColor(30);
      h_SecDO_EwOut->SetLineWidth(2);
      h_SecDO_EwOut->Draw("same");
      h_TF_EwOut->SetLineColor(46);
      h_TF_EwOut->SetLineWidth(1);
      h_TF_EwOut->Draw("same");
      h_SecStageSecDO_EwOut->SetLineColor(3);
      h_SecStageSecDO_EwOut->SetLineWidth(2);
      h_SecStageSecDO_EwOut->Draw("same");
      h_SecStageTF_EwOut->SetLineColor(1);
      h_SecStageTF_EwOut->SetLineWidth(2);
      h_SecStageTF_EwOut->Draw("same");

    }else{
      h_TF_EwOut->SetLineWidth(2);
      h_TF_EwOut->SetMaximum(h_TF_EwOut->GetMaximum()*1.3);
      h_TF_EwOut->SetLineColor(2);
      h_TF_EwOut->Draw();
      h_DF_EwOut->SetLineColor(17);
      h_DF_EwOut->Draw("same");
      h_DO_EwOut->SetLineColor(6);
      h_DO_EwOut->SetLineWidth(8);
      h_DO_EwOut->Draw("same");
      h_AM_EwOut->SetLineColor(4);
      h_AM_EwOut->SetLineWidth(6);
      h_AM_EwOut->Draw("same");
      h_SecDO_EwOut->SetLineColor(3);
      h_SecDO_EwOut->SetLineWidth(2);
      h_SecDO_EwOut->Draw("same");
      h_TF_EwOut->Draw("same");
    }
    TLegend *leg = new TLegend(0.7,0.65, 0.88, 0.88);
    leg->AddEntry(h_DF_EwOut,   "DF");
    leg->AddEntry(h_DO_EwOut,   "DO");
    leg->AddEntry(h_AM_EwOut,   "AM");
    leg->AddEntry(h_SecDO_EwOut, "SecDO");
    leg->AddEntry(h_TF_EwOut,   "TF");
    if(s_split){
      leg->AddEntry(h_SecStageSecDO_EwOut, "4LSecDO");
      leg->AddEntry(h_SecStageTF_EwOut,   "4LTF");
    }
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->Draw();
    tex->Draw("same");

    sprintf(temp_outdir,"output/%s/event_detail.pdf",description);
    c_event_detail->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/event_detail.C",description);
    if(dump_Cfile)    c_event_detail->SaveAs(temp_outdir);

  }

  if(plot_maxregion){ // plot maxregion

    TCanvas *c_maxregion = new TCanvas("c_maxregion","c_maxregion",800,800);
    c_maxregion->Divide(2,2);
    gStyle->SetOptStat(1001110);
    // hit //
    c_maxregion->cd(1);
    h_maxftkhit->SetXTitle("number of hit in max region");
    h_maxftkhit->SetYTitle("events");
    h_maxftkhit->Rebin(nrbin*10);
    h_maxftkhit->SetAxisRange(0,(int)(h_maxftkhit->GetMean()*2));
    h_maxftkhit->GetXaxis()->SetNdivisions(4);
    h_maxftkhit->Draw();

    // road //
    c_maxregion->cd(2);
    h_maxroad->SetXTitle("number of road in max region");
    h_maxroad->SetYTitle("events");
    h_maxroad->Rebin(nrbin*5);
    h_maxroad->SetAxisRange(0,(int)(h_maxroad->GetMean()*3));
    h_maxroad->GetXaxis()->SetNdivisions(4);
    h_maxroad->Draw();

    // fit //
    c_maxregion->cd(3);
    h_maxfit->SetXTitle("number of fit in max region");
    h_maxfit->SetYTitle("events");
    h_maxfit->Rebin(200*nrbin);
    //    h_maxfit->SetAxisRange(0,(int)(h_maxfit->GetMean()*10));
    h_maxfit->SetMinimum(0);
    h_maxfit->SetAxisRange(0,1000000);
    h_maxfit->GetXaxis()->SetNdivisions(4);
    h_maxfit->Draw();

    tex->Draw("same");

    sprintf(temp_outdir,"output/%s/c_maxregion.pdf",description);
    c_maxregion->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/c_maxregion.C",description);
    if(dump_Cfile)    c_maxregion->SaveAs(temp_outdir);
  }

  if(plot_history){ // histroy plot
    cout<<"HHH  plots : history    HHH"<<endl;
    cout<<xAxis_vec.size()<<"   number of evrents"<<endl;

    double DO_EwOut_array[NPU][xAxis_vec.size()];
    double AM_EwOut_array[NPU][xAxis_vec.size()];
    double SecDO_EwOut_array[NPU][xAxis_vec.size()];
    double TF_EwOut_array[NPU][xAxis_vec.size()];
    double SecStageSecDO_EwOut_array[NPU][xAxis_vec.size()];
    double SecStageTF_EwOut_array[NPU][xAxis_vec.size()];
    double EwOut_array[xAxis_vec.size()];
    double xAxis_array[xAxis_vec.size()];
    double RoadsX5_array[NPU][xAxis_vec.size()];
    double FitsX1_array[NPU][xAxis_vec.size()];
    double maxFitRoad_array[NPU][xAxis_vec.size()];
  
    cout<<"HHH  plots : history Fill start   HHH"<<endl;
    for(int j=0 ; j<(int)xAxis_vec.size() ; j++){ // creep ..... vec to array
      for(int i=0 ; i<NPU ; i++){
	DO_EwOut_array[i][j]  =               DO_EwOut_vec[i][j];	      
	AM_EwOut_array[i][j]  =		      AM_EwOut_vec[i][j];	      
	SecDO_EwOut_array[i][j]  =	      SecDO_EwOut_vec[i][j];	      
	TF_EwOut_array[i][j]  =		      TF_EwOut_vec[i][j];	      
	SecStageSecDO_EwOut_array[i][j]  =    SecStageSecDO_EwOut_vec[i][j];
	SecStageTF_EwOut_array[i][j]  =	      SecStageTF_EwOut_vec[i][j];
	//	cout<<SecStageTF_EwOut_vec[i][j]<<"   TFEwOut "<<i<<" "<<j<<endl;
	RoadsX5_array[i][j]  =               RoadsX5_vec[i][j];	      
	FitsX1_array[i][j]  =               FitsX1_vec[i][j];	      
	maxFitRoad_array[i][j]  =               maxFitRoad_vec[i][j];	      
      }
      EwOut_array[j]=EwOut_vec[j];
      xAxis_array[j]=xAxis_vec[j];
    }
    cout<<"HHH Fill Vec done HHH"<<endl;
    // fill to tgraph
    for(int i=0 ; i<NPU ; i++){
      g_DO_EwOut[i] = new TGraph(xAxis_vec.size(), xAxis_array, DO_EwOut_array[i]);
      g_AM_EwOut[i] = new TGraph(xAxis_vec.size(), xAxis_array, AM_EwOut_array[i]);
      g_SecDO_EwOut[i] = new TGraph(xAxis_vec.size(), xAxis_array, SecDO_EwOut_array[i]);
      g_TF_EwOut[i] = new TGraph(xAxis_vec.size(), xAxis_array, TF_EwOut_array[i]);
      g_SecStageSecDO_EwOut[i] = new TGraph(xAxis_vec.size(), xAxis_array, SecStageSecDO_EwOut_array[i]);
      g_SecStageTF_EwOut[i] = new TGraph(xAxis_vec.size(), xAxis_array, SecStageTF_EwOut_array[i]);

      g_RoadsX5[i] = new TGraph(xAxis_vec.size(), xAxis_array, RoadsX5_array[i]);
      g_FitsX1[i] = new TGraph(xAxis_vec.size(), xAxis_array, FitsX1_array[i]);
      g_maxFitRoad[i] = new TGraph(xAxis_vec.size(), xAxis_array, maxFitRoad_array[i]);
    }
    for(int i=0 ; i<NPU ; i++){
      g_DO_EwOut[i]->SetMinimum(0);
      g_AM_EwOut[i]->SetMinimum(0);
      g_SecDO_EwOut[i]->SetMinimum(0);
      g_TF_EwOut[i]->SetMinimum(0);
      g_SecStageSecDO_EwOut[i]->SetMinimum(0);
      g_SecStageTF_EwOut[i]->SetMinimum(0);
      g_RoadsX5[i]->SetMinimum(0);
      g_FitsX1[i]->SetMinimum(0);
      g_maxFitRoad[i]->SetMinimum(0);

    }

    g_EwOut = new TGraph(xAxis_vec.size(), xAxis_array, EwOut_array);
    g_EwOut->SetMinimum(0);
    //   g_EwOut->SetTitle("End Word Out Time History");
    g_EwOut->SetTitle("");
    g_EwOut->GetXaxis()->SetTitle("event number");
    if(temp_denom==1000)g_EwOut->GetYaxis()->SetTitle("time (#mu sec)");
    if(temp_denom==1)g_EwOut->GetYaxis()->SetTitle("time (n sec)");
    //g_EwOut->GetYaxis()->SetTitleOffset(1.0);
    for(int i=0 ; i<NPU ; i++){
      g_DO_EwOut[i]->SetLineWidth(1);
      g_AM_EwOut[i]->SetLineWidth(1);
      g_SecDO_EwOut[i]->SetLineWidth(1);
      g_TF_EwOut[i]->SetLineWidth(1);
      g_SecStageSecDO_EwOut[i]->SetLineWidth(1);
      g_SecStageTF_EwOut[i]->SetLineWidth(1);
    }
    g_EwOut->SetLineWidth(2);
    cout<<"HHH  history fill end    HHH"<<endl;
    // HH Special history road*5 fit*1 max HH //
    TCanvas *c_maxRoadFit = new TCanvas("c_maxRoadFit","c_maxRoadFit",800,800);
    gStyle->SetOptStat(0);
    //    g_EwOut->Draw("AL");
    //    g_FitsX1[2]->Draw("L same");
    g_maxFitRoad[2]->SetTitle("");
    g_maxFitRoad[2]->GetXaxis()->SetTitle("event number");
    g_maxFitRoad[2]->GetYaxis()->SetTitle("time (roads x 5n or fits x 1n sec");

    g_maxFitRoad[2]->Draw("AL");
    g_RoadsX5[2]->Draw("L same");
    g_FitsX1[2]->Draw("L same");
    if(0){
    cout<<"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"<<endl;
    for(int i=0 ; i<NPU ; i++){
      cout<<"meanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmeanmean"<<endl;
      cout<<"Time Average for Fits Roads ++ tower = "<<i<<" = "<<endl;
      cout<<"Fits = "<<g_FitsX1[i]->GetMean(2)<<setw(8)<<"("<<g_FitsX1[i]->GetMean(2)*8000.<<")"<<endl;
      cout<<"Roads = "<<g_RoadsX5[i]->GetMean(2)<<setw(8)<<"("<<g_RoadsX5[i]->GetMean(2)*8000./5.<<")"<<endl;
      cout<<"maxFitRoad = "<<g_maxFitRoad[i]->GetMean(2)<<endl;

      double x,y;
      for(int j=0; j<xAxis_vec.size() ; j++){
	g_RoadsX5[i]->GetPoint(j,x,y);
	cout<<"Loading time = "<<y<<"     nroads = "<<y*8000./5.<<endl;
	g_FitsX1[i]->GetPoint(j,x,y);
	cout<<"Fitting time = "<<y<<"     nfits = "<<y*8000.<<endl;
	g_maxFitRoad[i]->GetPoint(j,x,y);
	cout<<"processing time = "<<y<<endl;
      }
    }
    cout<<"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"<<endl;
    }
    cout<<"HHH  Special for road and fit  HHH"<<endl;
    g_RoadsX5[2]->SetLineColor(4);
    g_FitsX1[2]->SetLineColor(2);
    //    g_maxFitRoad[2]->

    sprintf(temp_outdir,"output/%s/maxRoadFit.pdf",description);
    c_maxRoadFit->SaveAs(temp_outdir);


    // *** TF EwOut history *** //
    TCanvas *c_historyplot = new TCanvas("c_historyplot","c_historyplot",800,800);
    c_historyplot->Divide(1,1);
    //    c_historyplot->Divide(3,3);
    gStyle->SetOptStat(0);
    g_EwOut->Draw("AL");
    for(int i=0 ; i<NPU ; i++){
      if(s_split){
	g_SecStageTF_EwOut[i]->SetLineColor(i+1);
	c_historyplot->cd(i+1);
	//	g_EwOut->Draw("AL");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }else{
	g_TF_EwOut[i]->SetLineColor(i+1);
	c_historyplot->cd(i+1);
	//	g_EwOut->Draw("AL");
	g_TF_EwOut[i]->Draw("L same");
      }
    }
    TLegend *leg_a = new TLegend(0.7,0.35, 0.88, 0.88);
    leg_a->AddEntry(g_TF_EwOut[0],   "Reg0","L");
    leg_a->AddEntry(g_TF_EwOut[1],   "Reg1","L");
    leg_a->AddEntry(g_TF_EwOut[2],   "Reg2","L");
    leg_a->AddEntry(g_TF_EwOut[3],   "Reg3","L");
    leg_a->AddEntry(g_TF_EwOut[4],   "Reg4","L");
    leg_a->AddEntry(g_TF_EwOut[5],   "Reg5","L");
    leg_a->AddEntry(g_TF_EwOut[6],   "Reg6","L");
    leg_a->AddEntry(g_TF_EwOut[7],   "Reg7","L");
    leg_a->AddEntry(g_EwOut, "total proc","L");
    leg_a->SetBorderSize(0);
    //    leg_a->SetFillColor(0);
    leg_a->SetFillColor(0);
    leg_a->SetFillStyle(0);
    leg_a->Draw();
    tex->Draw("same");

    sprintf(temp_outdir,"output/%s/TFEwOutHistory.pdf",description);
    //    sprintf(temp_outdir,"output/%s/TFEwOutDivHistory.pdf",description);
    c_historyplot->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/TFEwOutHistory.C",description);
    if(dump_Cfile)c_historyplot->SaveAs(temp_outdir);


    // *** ALL EwOut history *** //
    for(int i=0 ; i<NPU ; i++){
      g_DO_EwOut[i]->SetLineColor(6);
      g_AM_EwOut[i]->SetLineColor(4);
      g_SecDO_EwOut[i]->SetLineColor(30);
      g_TF_EwOut[i]->SetLineColor(46);
      g_SecStageSecDO_EwOut[i]->SetLineColor(3);
      g_SecStageTF_EwOut[i]->SetLineColor(2);
      //      g_DO_EwOut[i]->SetLineStyle(i+2);
      //      g_AM_EwOut[i]->SetLineStyle(i+2);
      //      g_SecDO_EwOut[i]->SetLineStyle(i+2);
      //      g_TF_EwOut[i]->SetLineStyle(i+2);
      //      g_SecStageSecDO_EwOut[i]->SetLineStyle(i+2);
      //      g_SecStageTF_EwOut[i]->SetLineStyle(i+2);
    }
    
    TCanvas *c_historyplot_all = new TCanvas("c_historyplot_all","c_historyplot_all",800,800);
    c_historyplot_all->Divide(1,1);
    gStyle->SetOptStat(0);
    g_EwOut->Draw("AL");
    
    for(int i=0 ; i<NPU ; i++){
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
    }
    //TLegend *leg = new TLegend(0.6,0.45, 0.78, 0.88);
    TLegend *leg = new TLegend(0.7,0.7, 0.88, 0.88);
    //    leg->AddEntry(g_DF_EwOut[1],   "DF");
    
    leg->AddEntry(g_DO_EwOut[1],   "DO","L");
    leg->AddEntry(g_AM_EwOut[1],   "AM","L");
    leg->AddEntry(g_SecDO_EwOut[1], "SecDO","L");
    leg->AddEntry(g_TF_EwOut[1],   "TF","L");
    if(s_split){
      leg->AddEntry(g_SecStageSecDO_EwOut[1], "4LSecDO","L");
      leg->AddEntry(g_SecStageTF_EwOut[1],   "4LTF","L");
    }
    leg->AddEntry(g_EwOut, "total proc","L");
    leg->SetBorderSize(0);
//    leg->SetFillColor(0);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->Draw();
    tex->Draw("same");

    sprintf(temp_outdir,"output/%s/AllWeOutHistory.pdf",description);
    c_historyplot_all->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/AllWeOutHistory.C",description);
    if(dump_Cfile) c_historyplot_all->SaveAs(temp_outdir);

    
    
    
    TCanvas *c_historyplot_reg1 = new TCanvas("c_historyplot_reg1","c_historyplot_reg1",800,800);
    c_historyplot_reg1->Divide(1,1);
    gStyle->SetOptStat(0);
    g_EwOut->Draw("AL");
    //    for(int i=0 ; i<NPU ; i++){
    //    for(int i=0 ; i<1 ; i++){
    for(int i=special_region ; i<special_region+1 ; i++){
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
    }
    TLegend*leg_reg1 = new TLegend(0.7,0.7, 0.88, 0.88);
    //    leg_reg1->AddEntry(g_DF_EwOut[1],   "DF");
    leg_reg1->AddEntry(g_DO_EwOut[1],   "DO","L");
    leg_reg1->AddEntry(g_AM_EwOut[1],   "AM","L");
    leg_reg1->AddEntry(g_SecDO_EwOut[1], "SecDO","L");
    leg_reg1->AddEntry(g_TF_EwOut[1],   "TF","L");
    if(s_split){
      leg_reg1->AddEntry(g_SecStageSecDO_EwOut[1], "4LSecDO","L");
      leg_reg1->AddEntry(g_SecStageTF_EwOut[1],   "4LTF","L");
    }
    leg_reg1->AddEntry(g_EwOut, "total proc","L");
    leg_reg1->SetBorderSize(0);
//    leg_reg1->SetFillColor(0);
    leg_reg1->SetFillColor(0);
    leg_reg1->SetFillStyle(0);
    leg_reg1->Draw();
    tex->Draw("same");

    sprintf(temp_outdir,"output/%s/Reg1EwOutHistory.pdf",description);
    c_historyplot_reg1->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/Reg1EwOutHistory.C",description);
    if(dump_Cfile)c_historyplot_reg1->SaveAs(temp_outdir);



    TLegend*leg_regsep = new TLegend(0.7,0.7, 0.88, 0.88);
    //    leg_regsep->AddEntry(g_DF_EwOut[1],   "DF");
    leg_regsep->AddEntry(g_DO_EwOut[1],   "DO","L");
    leg_regsep->AddEntry(g_AM_EwOut[1],   "AM","L");
    leg_regsep->AddEntry(g_SecDO_EwOut[1], "SecDO","L");
    leg_regsep->AddEntry(g_TF_EwOut[1],   "TF","L");
    if(s_split){
      leg_regsep->AddEntry(g_SecStageSecDO_EwOut[1], "4LSecDO","L");
      leg_regsep->AddEntry(g_SecStageTF_EwOut[1],   "4LTF","L");
    }
    leg_regsep->AddEntry(g_EwOut, "total proc","L");
    leg_regsep->SetBorderSize(0);
    leg_regsep->SetFillColor(0);
    leg_regsep->SetFillStyle(0);
    //    leg_regsep->Draw();
    gStyle->SetOptStat(0);
    // history all ----------------------------- 0 -----------------------------------------------------
    special_region=0;
    TCanvas *c_historyplot_regsep0 = new TCanvas("c_historyplot_regsep0","c_historyplot_regsep0",1600,1600);
    c_historyplot_regsep0->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep0->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep0EwOutHistory.pdf",description);
    c_historyplot_regsep0->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/Regsep0EwOutHistory.C",description);
    if(dump_Cfile)c_historyplot_regsep0->SaveAs(temp_outdir);
    // history all ----------------------------- 16 -----------------------------------------------------
    special_region=16;
    TCanvas *c_historyplot_regsep16 = new TCanvas("c_historyplot_regsep16","c_historyplot_regsep16",161616,161616);
    c_historyplot_regsep16->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep16->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep16EwOutHistory.pdf",description);
    c_historyplot_regsep16->SaveAs(temp_outdir);
    sprintf(temp_outdir,"output/%s/C/Regsep16EwOutHistory.C",description);
    if(dump_Cfile)c_historyplot_regsep16->SaveAs(temp_outdir);
    // history all ----------------------------- 32 -----------------------------------------------------
    special_region=32;
    TCanvas *c_historyplot_regsep32 = new TCanvas("c_historyplot_regsep32","c_historyplot_regsep32",163232,163232);
    c_historyplot_regsep32->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep32->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep32EwOutHistory.pdf",description);
    c_historyplot_regsep32->SaveAs(temp_outdir);
    // history all ----------------------------- 48 -----------------------------------------------------
    special_region=48;
    TCanvas *c_historyplot_regsep48 = new TCanvas("c_historyplot_regsep48","c_historyplot_regsep48",164848,164848);
    c_historyplot_regsep48->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep48->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep48EwOutHistory.pdf",description);
    c_historyplot_regsep48->SaveAs(temp_outdir);
    // history all ----------------------------- 64 -----------------------------------------------------
    special_region=64;
    TCanvas *c_historyplot_regsep64 = new TCanvas("c_historyplot_regsep64","c_historyplot_regsep64",166464,166464);
    c_historyplot_regsep64->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep64->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep64EwOutHistory.pdf",description);
    c_historyplot_regsep64->SaveAs(temp_outdir);
    // history all ----------------------------- 80 -----------------------------------------------------
    special_region=80;
    TCanvas *c_historyplot_regsep80 = new TCanvas("c_historyplot_regsep80","c_historyplot_regsep80",168080,168080);
    c_historyplot_regsep80->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep80->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep80EwOutHistory.pdf",description);
    c_historyplot_regsep80->SaveAs(temp_outdir);
    // history all ----------------------------- 96 -----------------------------------------------------
    special_region=96;
    TCanvas *c_historyplot_regsep96 = new TCanvas("c_historyplot_regsep96","c_historyplot_regsep96",169696,169696);
    c_historyplot_regsep96->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep96->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep96EwOutHistory.pdf",description);
    c_historyplot_regsep96->SaveAs(temp_outdir);
    // history all ----------------------------- 112 -----------------------------------------------------
    special_region=112;
    TCanvas *c_historyplot_regsep112 = new TCanvas("c_historyplot_regsep112","c_historyplot_regsep112",16112112,16112112);
    c_historyplot_regsep112->Divide(4,4);

    for(int i=special_region ; i<special_region+16 ; i++){
      c_historyplot_regsep112->cd(i-special_region+1);
      g_EwOut->Draw("AL");
      g_DO_EwOut[i]->Draw("L same");
      g_AM_EwOut[i]->Draw("L same");
      g_SecDO_EwOut[i]->Draw("L same");
      g_TF_EwOut[i]->Draw("L same");
      if(s_split){
	g_SecStageSecDO_EwOut[i]->Draw("L same");
	g_SecStageTF_EwOut[i]->Draw("L same");
      }
      tex->Draw("same");
      leg_regsep->Draw();
    }
    sprintf(temp_outdir,"output/%s/Regsep112EwOutHistory.pdf",description);
    c_historyplot_regsep112->SaveAs(temp_outdir);



  } // end of hist plot_history
  // end of PlotResults

  h_road->Write();
  tf->Write();

}
