/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================//
// *** MuonDQAPlotsForRTT.cc ****************************************************************     //
// DESCRIPTION:                                                                                   //
//  ROOT Macro for extracting to a single ps all Offline Muon Monitoring Histos useful to RTT     //
//  Authors:  Nektarios Chr. Benekos (Illinois) Ioannis Nomidis (AUTh - Thessaloniki, March 2009) //
//     ...                                                                                        //
// TO EXECUTE, in ROOT command line enter:                                                        //
//  .x MuonDQAPlotsForRTT.cc("fileName")                                                          // 
//================================================================================================//

#include <string>
#include <vector>

void MuonDQAPlotsForRTT( string fName="" )
{ 
  //gROOT->Macro("atlas_style.cc");
  const char* MBoy  = "ConvertedMBoy";
  const char* Moore = "Moore";
  
  TFile* f=new TFile(fName.c_str(),"READ");

  //Get the run number - it's the top-folder's name in the root file!
  TIter nextcd0(gDirectory->GetListOfKeys());
  TKey *key0;
  key0 = (TKey*)nextcd0();
  TDirectory *dir0= (TDirectory*)key0->ReadObj();
  string runNumber=dir0->GetName(); //runNumber.substr(4) returns only the number
  //std::string pdfTag=runNumber+".ps";
  std::string psTag=runNumber+".ps";
  TDirectory *dir;
  TCanvas *canv=new TCanvas("canv","MuonSpectrometerDQA Hists",0,0,850,650);
  TPad *pad=(TPad*) canv;
  canv->Clear();
  //TPDF* pdf;
  TPostScript* ps;
  //stringstream pdfName;
  stringstream psName;
  //pdfName<<"MuonDqaPlotsforRTT_"<<pdfTag;
  //pdf=new TPDF(pdfName.str().c_str());
  psName<<"MuonDqaPlotsforRTT_"<<psTag;
  ps=new TPostScript(psName.str().c_str());
  int npage=0;      
  int ipad=0;
 
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing RawDataMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  //MDT
  cout<<"((( MDT )))\n";
  canv->Clear();  ipad=0;
  vector<string> dirname;
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/Overview/");
  std::vector<std::vector<std::string> > histname;
  histname.clear();

  canv->SetFillColor(10);
  canv->SetFillColor(10);

  int doStats = 1;


  for (int i=0; i<dirname.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("Number_of_BarrelMDTHits_inRZView_Global_ADCCut");
  histname[0].push_back("Number_of_EndCapMDTHits_inRZView_Global_ADCCut");
  histname[0].push_back("Number_of_OverlapMDTHits_inRZView_Global_ADCCut");
  histname[0].push_back("Number_of_Chambers_with_hits_per_event_ADCCut");
  histname[0].push_back("TotalNumber_of_MDT_hits_per_event_ADCCut"); 
  histname[0].push_back("TotalNumber_of_MDT_hits_per_event_RPCtrig_ADCCut");
  histname[0].push_back("TotalNumber_of_MDT_hits_per_event_TGCtrig_ADCCut");
  histname[0].push_back("Overall_TDC_ADCCut_spectrum");
  histname[0].push_back("Overall_TDCADC_spectrum");
  histname[0].push_back("NumberOfHitsInMDTInner_ADCCut");
  histname[0].push_back("NumberOfHitsInMDTMiddle_ADCCut");
  histname[0].push_back("NumberOfHitsInMDTOuter_ADCCut");  
  histname[0].push_back("NumberOfHitsInBarrelPerChamber_ADCCut");
  histname[0].push_back("NumberOfHitsInEndCapPerChamber_ADCCut");
  histname[0].push_back("EffsInBarrelPerChamber_ADCCut");
  histname[0].push_back("EffsInEndCapPerChamber_ADCCut");  
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) { nhist++; }    
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<24) canv->Divide(5,5);

  for (int idir=0; idir<dirname.size(); idir++) 
    {
      for (int ihist=0; ihist<histname[idir].size(); ihist++) 
	{
	  canv->cd(++ipad);
	  drawSupImp(dirname[idir],histname[idir][ihist],f,"Raw",doStats);
	canv->Update();	  
	}

    }
   
	   npage++; 
  
  canv->Clear(); ipad=0;
  cout<<"((( MDT Occupancy Plots )))\n";
  string sector[16]={"Sector1","Sector2","Sector3","Sector4","Sector5","Sector6","Sector7",
  "Sector8","Sector9","Sector10","Sector11","Sector12","Sector13","Sector14","Sector15","Sector16"}
  canv->Divide(4,4);
  dirname.clear();
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/Overview/HitOccupancies_Sectors"); //0
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname.size(); i++) histname.push_back(vector<string>());
  for (int ireg=0; ireg<16; ireg++) histname[0].push_back("MDTHitsOccup_ADCCut_"+sector[ireg]);
     for (int ihist=0; ihist<histname[0].size(); ihist++) {
       canv->cd(++ipad);
       drawSupImp(dirname[0],histname[0][ihist],f,"Raw",doStats);
       canv->Update();
      }    

  npage++; 


  //MDTBA
  cout<<"((( MDT BarrelA)))\n";
  canv->Clear();  ipad=0;
  vector<string> dirname;
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTBA/Overview/");
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("NumberOfHitsInBAInnerPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInBAMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInBAOuterPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInBAExtraPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBAInnerPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBAMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBAOuterPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBAExtraPerMultiLayer_ADCCut");
  histname[0].push_back("MDT_tdrift_BA");
  histname[0].push_back("MDT_t0_BA");
  histname[0].push_back("MDT_tmax_BA");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) { nhist++; }    
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<24) canv->Divide(5,5);

  for (int idir=0; idir<dirname.size(); idir++) 
    {
      for (int ihist=0; ihist<histname[idir].size(); ihist++) 
	{
	  canv->cd(++ipad);
	  drawSupImp(dirname[idir],histname[idir][ihist],f,"Raw",doStats);
	canv->Update();	  
	}

    }
  
	   npage++; 

  //MDTBC
  cout<<"((( MDT BarrelC)))\n";
  canv->Clear();  ipad=0;
  vector<string> dirname;
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTBC/Overview/");
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("NumberOfHitsInBCInnerPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInBCMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInBCOuterPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInBCExtraPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBCInnerPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBCMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBCOuterPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInBCExtraPerMultiLayer_ADCCut");
  histname[0].push_back("MDT_tdrift_BC");
  histname[0].push_back("MDT_t0_BC");
  histname[0].push_back("MDT_tmax_BC");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) { nhist++; }    
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<24) canv->Divide(5,5);

  for (int idir=0; idir<dirname.size(); idir++) 
    {
      for (int ihist=0; ihist<histname[idir].size(); ihist++) 
	{
	  canv->cd(++ipad);
	  drawSupImp(dirname[idir],histname[idir][ihist],f,"Raw",doStats);
	canv->Update();	  
	}

    }
  
	   npage++; 

  //MDTEA
  cout<<"((( MDT EndCapA)))\n";
  canv->Clear();  ipad=0;
  vector<string> dirname;
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTEA/Overview/");
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("NumberOfHitsInEAInnerPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInEAMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInEAOuterPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInEAExtraPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInEAInnerPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInEAMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("EffsInEAOuterPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInEAExtraPerMultiLayer_ADCCut");
  histname[0].push_back("MDT_tdrift_EA");
  histname[0].push_back("MDT_t0_EA");
  histname[0].push_back("MDT_tmax_EA");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) { nhist++; }    
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<24) canv->Divide(5,5);

  for (int idir=0; idir<dirname.size(); idir++) 
    {
      for (int ihist=0; ihist<histname[idir].size(); ihist++) 
	{
	  canv->cd(++ipad);
	  drawSupImp(dirname[idir],histname[idir][ihist],f,"Raw",doStats);
	canv->Update();	  
	}

    }
  
	   npage++; 


  //MDTEC
  cout<<"((( MDT EndCapC)))\n";
  canv->Clear();  ipad=0;
  vector<string> dirname;
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTEC/Overview/");
  std::vector<std::vector<std::string> > histname;
  histname.clear();
  for (int i=0; i<dirname.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("NumberOfHitsInECInnerPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInECMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInECOuterPerMultiLayer_ADCCut");
  histname[0].push_back("NumberOfHitsInECExtraPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInECInnerPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInECMiddlePerMultiLayer_ADCCut");
  histname[0].push_back("EffsInECOuterPerMultiLayer_ADCCut");
  histname[0].push_back("EffsInECExtraPerMultiLayer_ADCCut");
  histname[0].push_back("MDT_tdrift_EC");
  histname[0].push_back("MDT_t0_EC");
  histname[0].push_back("MDT_tmax_EC");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) { nhist++; }    
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<24) canv->Divide(5,5);

  for (int idir=0; idir<dirname.size(); idir++) 
    {
      for (int ihist=0; ihist<histname[idir].size(); ihist++) 
	{
	  canv->cd(++ipad);
	  drawSupImp(dirname[idir],histname[idir][ihist],f,"Raw",doStats);
	canv->Update();	  
	}

    }
  
	   npage++; 


  //RPC
  cout<<"((( RPC )))\n";
  canv->Clear(); ipad=0;
  vector<string> dirname_v;
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/RPC/Overview/"); //0
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/RPCLV1/GLOBAL/");  //1
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/RPCLV1/Overview/");  //2
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDTvsRPC/Dqmf/");//3
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("Time_Distribution");
  histname[0].push_back("Phi_Time_Distribution"); 
  histname[0].push_back("Eta_Time_Distribution");   
  histname[0].push_back("Number_of_RPC_clusters_per_event");
  histname[0].push_back("Eta_ClusterSize_Distribution");   
  histname[0].push_back("Phi_ClusterSize_Distribution");
  histname[0].push_back("Number_of_RPC_hits_per_event"); 
  histname[0].push_back("rpc2DEtaStationTriggerHits");
  histname[0].push_back("rpc2DEtaStationGap1");
  histname[0].push_back("rpc2DEtaStationGap2");
  histname[0].push_back("AtlasLowPt0");
  histname[0].push_back("AtlasPivot0");
  histname[0].push_back("AtlasHighPt0");
  histname[0].push_back("AtlasLowPt1");
  histname[0].push_back("AtlasPivot1");
  histname[0].push_back("AtlasHighPt1");   
  histname[1].push_back("TriggerCondition_vs_SectorLogic");
  histname[1].push_back("rpclv1_BCid_vs_SectorLogic");
  histname[1].push_back("rpclv1_BCid_per_TriggerType");
  histname[2].push_back("rpclv1_hitperEvent");
  histname[3].push_back("MdtRpcZdifference");
  histname[3].push_back("MdtNHitsvsRpcNHits");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++)      nhist++;

  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<26) canv->Divide(5,5);
  
  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw",doStats);
      canv->Update();
    }
  }
  npage++;

 
 npage = DrawMDTvsRPCSectors(f,runNumber,(TPad*) canv,npage);


  //TGC
  cout<<"((( TGC )))\n";
  canv->Clear(); ipad=0;
  vector<string> dirname_v;
  dirname_v.clear(); 
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/Global/");          //0
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEA/");               //1
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEC/");               //2  
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEA/Efficiency/");    //3
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEC/Efficiency/");    //4 
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("Number_Of_TGC_Hits_Per_Event");
  histname[0].push_back("Number_Of_TGC_Strip_Hits_Per_Event");
  histname[0].push_back("Number_Of_TGC_Wire_Hits_Per_Event");
  histname[1].push_back("XY_View_A");
  histname[1].push_back("Number_Of_TGC_Hits_Per_Event_A");
  histname[2].push_back("XY_View_C");
  histname[2].push_back("Number_Of_TGC_Hits_Per_Event_C");
  histname[3].push_back("Efficiency_A");
  histname[3].push_back("Strip_Efficiency_A");
  histname[3].push_back("Wire_Efficiency_A");
  histname[4].push_back("Efficiency_C");
  histname[4].push_back("Strip_Efficiency_C");
  histname[4].push_back("Wire_Efficiency_C");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++)      nhist++;

  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw",doStats);
      canv->Update();
    }
  }
  npage++;


  //TGCLV1
  cout<<"((( TGCLV1 )))\n";
  canv->Clear(); ipad=0;
  dirname_v.clear();    
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/Global/");       //0
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/TGCEA/");        //1
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/TGCEC/");        //2     
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/Global/Correlation/");     //3
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("Number_Of_SL_Triggers_Previous");
  histname[0].push_back("Number_Of_SL_Triggers_Current");
  histname[0].push_back("Number_Of_SL_Triggers_Next");
  histname[0].push_back("Number_Of_SL_Triggers_Total");
  histname[1].push_back("SL_Vs_LowPt_Timing_More_Than_PT2_A");
  histname[1].push_back("RoI_Eta_Vs_Phi_A");
  histname[1].push_back("SL_Sector_A"); 
  histname[1].push_back("SL_Timing_A");  
  histname[2].push_back("SL_Vs_LowPt_Timing_More_Than_PT2_C");
  histname[2].push_back("RoI_Eta_Vs_Phi_C");
  histname[2].push_back("SL_Sector_C"); 
  histname[2].push_back("SL_Timing_C");
  histname[3].push_back("RoI_Eta_Vs_Wire_Hit_Eta");
  histname[3].push_back("RoI_Phi_Vs_Strip_Hit_Phi");   
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++)      nhist++;

  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw",doStats);
      canv->Update();
    }
  }
  npage++;


  //MDTvsTGC
  cout<<"((( MDTvsTGC )))\n";
  canv->Clear(); ipad=0;
  dirname_v.clear();    
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDTvsTGC/TGCEA/");       //0
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDTvsTGC/TGCEC/");       //1 
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("MDT_Eta_Minus_TGC_RoI_Eta_A");
  histname[0].push_back("MDT_Eta_Vs_TGC_RoI_Eta_A");
  histname[0].push_back("MDT_Phi_Vs_TGC_RoI_Phi_A");
  histname[0].push_back("MDT_Sector_Vs_TGC_RoI_48_Sector_A"); 
  histname[1].push_back("MDT_Eta_Minus_TGC_RoI_Eta_C");
  histname[1].push_back("MDT_Eta_Vs_TGC_RoI_Eta_C");
  histname[1].push_back("MDT_Phi_Vs_TGC_RoI_Phi_C");
  histname[1].push_back("MDT_Sector_Vs_TGC_RoI_48_Sector_C"); 
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++)      nhist++;

  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw",doStats);
      canv->Update();
    }
  }
  npage++;


  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Trk Parameters <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonGenericTracksMon/"); //0 
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("m_muon_Rec_chi2");
  histname[0].push_back("m_muon_Rec_P");
  histname[0].push_back("m_muon_Rec_d0");
  histname[0].push_back("m_muon_Rec_z0");
  histname[0].push_back("m_muon_Rec_phi0");
  histname[0].push_back("m_muon_Rec_eta");
  histname[0].push_back("m_muon_receta_recphi");
  histname[0].push_back("m_muontrackndof");
  histname[0].push_back("m_recomuon_pT");
  histname[0].push_back("m_recomuon_POS_pT");
  histname[0].push_back("m_recomuon_NEG_pT");
  histname[0].push_back("m_recomuon_HighpT");
  histname[0].push_back("m_recomuon_POS_HighpT");
  histname[0].push_back("m_recomuon_NEG_HighpT");  
  histname[0].push_back("m_charge_muontracks");
  histname[0].push_back("m_recomuon_ChargeRatio");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks",doStats);
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Trk Parameters <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonTrkParameters/");           //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());   
  histname[0].push_back("muon_trk_pT_MDThits"); 
  histname[0].push_back("muon_trk_chi2oDoF");
  histname[0].push_back("muon_trk_chi2Prob");
  histname[0].push_back("muon_trk_chi2oDoF_MDThits");
  histname[0].push_back("muon_trk_chi2oDoF_pT");
  histname[0].push_back("muon_HitOnTrk_ToF");
  histname[0].push_back("muon_HitOnTrk_X0"); 
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks",doStats);
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Trk Parameters Barrel <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonTrkParameters/");    //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());   
  histname[0].push_back("muon_trk_d0c_barrel");
  histname[0].push_back("muon_trk_z0_barrel"); 
  histname[0].push_back("muon_trk_d0_vs_z0_barrel");
  histname[0].push_back("muon_trk_d0_vs_phi_barrel");
  histname[0].push_back("muon_trk_phi0_pos_barrel");
  histname[0].push_back("muon_trk_phi0_neg_barrel");  
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImpMuonTrkParamBarrel(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Trk Parameters EndCap <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonTrkParameters/");    //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());   
  histname[0].push_back("muon_trk_d0c_eca");
  histname[0].push_back("muon_trk_d0c_ecc");
  histname[0].push_back("muon_trk_d0_vs_phi_eca");
  histname[0].push_back("muon_trk_d0_vs_phi_ecc");
  histname[0].push_back("muon_trk_z0_eca");
  histname[0].push_back("muon_trk_z0_ecc"); 
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImpMuonTrkParamEndCap(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;

 
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Residuals and Pull <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonGenericTracksMon/"); //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  //residuals-pulls
  histname[0].push_back("All_Residuals");
  histname[0].push_back("All_Pull");
  histname[0].push_back("MDT_Residuals");
  histname[0].push_back("MDT_Pull");
  histname[0].push_back("RPC_Residuals");
  histname[0].push_back("RPC_Pull");
  histname[0].push_back("m_rpc_res_eta");
  histname[0].push_back("m_rpc_pull_eta"); 
  histname[0].push_back("m_rpc_res_phi");
  histname[0].push_back("m_rpc_pull_phi");
  histname[0].push_back("TGC_Residuals");
  histname[0].push_back("TGC_Pull");
  histname[0].push_back("m_tgc_res_eta");
  histname[0].push_back("m_tgc_pull_eta"); 
  histname[0].push_back("m_tgc_res_phi");
  histname[0].push_back("m_tgc_pull_phi");
  histname[0].push_back("CSC_Residuals");
  histname[0].push_back("CSC_Pull");
  histname[0].push_back("m_csc_res_eta");
  histname[0].push_back("m_csc_pull_eta"); 
  histname[0].push_back("m_csc_res_phi");
  histname[0].push_back("m_csc_pull_phi");  
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks",doStats);
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring HitsOnTrack <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonMonTrkSummary/");    //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("muon_ntracks");
  histname[0].push_back("muon_Nhits_per_track");
  histname[0].push_back("muon_noutliers_per_track");
  histname[0].push_back("num_muonholes_per_Trk");
  histname[0].push_back("num_muonHits_per_Trk");
  histname[0].push_back("num_muonEtaHits_per_Trk");
  histname[0].push_back("num_muonPhiHits_per_Trk");
  histname[0].push_back("num_muonTrigHits_per_Trk");
  histname[0].push_back("num_MDThits_per_Trk_vs_Eta");
  histname[0].push_back("num_MDThits_per_Trk_vs_Phi");
//  histname[0].push_back("num_hits_per_Trk_vs_Eta");
//  histname[0].push_back("num_hits_per_Trk_vs_Phi");
  
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks",doStats);
      canv->Update();
    }
  }
  npage++;


 
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing HitsPerTrack Detailed TrackMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/HitsPerTrk/"); //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("muon_nmdthits_per_track");
  histname[0].push_back("muon_nrpchits_per_track");
  histname[0].push_back("muon_nrpcetahits_per_track");
  histname[0].push_back("muon_nrpcphihits_per_track");
  histname[0].push_back("muon_ntgchits_per_track");
  histname[0].push_back("muon_ntgcetahits_per_track");
  histname[0].push_back("muon_ntgcphihits_per_track");
  histname[0].push_back("muon_ncschits_per_track");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks",doStats);
      canv->Update();
    }
  }
  npage++;


  /////////////////
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Residual Barrel Plots<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonGenericTracksMon/"); //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi1");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi2");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi3");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi4");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi5");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi6");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi7");
  histname[0].push_back("m_mdt_resid_barrel_SectorPhi8");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImpResidualsBarrel(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;


  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Residual EndCap-A Plots<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonGenericTracksMon/"); //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("m_mdt_resid_eca_SectorPhi1");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi2");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi3");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi4");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi5");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi6");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi7");
  histname[0].push_back("m_mdt_resid_eca_SectorPhi8");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImpResidualsEndCapA(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring Residual EndCap-C Plots<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonGenericTracksMon/"); //0
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi1");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi2");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi3");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi4");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi5");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi6");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi7");
  histname[0].push_back("m_mdt_resid_ecc_SectorPhi8");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImpResidualsEndCapC(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;


///////////////////////////////

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing RPCStandAlone TrackMonitoring Plots <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon/Overview/"); //0
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon/GLOBAL/"); //1
     
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>()); 
  histname[1].push_back("rpcNtracks"); 
  histname[0].push_back("TimeTrackRMS");
  histname[0].push_back("rpcEtaResidual");
  histname[0].push_back("rpcPhiResidual");
  histname[0].push_back("rpcTrackType");
  histname[0].push_back("rpcPointPerTracks"); 
  histname[0].push_back("rpcchi2dof");
  histname[0].push_back("rpctrack_phivseta");
  histname[0].push_back("rpczSurfaceView");
  histname[1].push_back("rpczxSurfaceView"); 
  histname[1].push_back("TimeTrackResidual");
  histname[1].push_back("Layer_Efficiency");  
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      	  drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw",doStats);
      //drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;




///////////////////////////////

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TGCStandAlone TrackMonitoring Plots <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/TGCStandAloneTrackMon/TGCEA/"); //0
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/TGCStandAloneTrackMon/TGCEA/Track/"); //1
  
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/TGCStandAloneTrackMon/TGCEC/"); //2
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/TGCStandAloneTrackMon/TGCEC/Track/"); //3
  
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>()); 
  histname[0].push_back("MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_A");
  histname[1].push_back("TGC_Track_Chi2_Over_NDF_A");
  histname[1].push_back("TGC_Track_Chi2_Over_NDF_With_Pt_Cut_A");  
  histname[1].push_back("TGC_Track_Finding_Efficiency_A");
  histname[1].push_back("TGC_Track_Pull_A");
  histname[1].push_back("TGC_Track_Pull_With_Pt_Cut_A");
  histname[1].push_back("TGC_Track_Pull_3Station_Track_A");
  histname[1].push_back("TGC_Track_Pull_3Station_Track_With_Pt_Cut_A");
  histname[2].push_back("MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_C"); 
  histname[3].push_back("TGC_Track_Chi2_Over_NDF_C");
  histname[3].push_back("TGC_Track_Chi2_Over_NDF_With_Pt_Cut_C");  
  histname[3].push_back("TGC_Track_Finding_Efficiency_C");
  histname[3].push_back("TGC_Track_Pull_C");
  histname[3].push_back("TGC_Track_Pull_With_Pt_Cut_C");
  histname[3].push_back("TGC_Track_Pull_3Station_Track_C");
  histname[3].push_back("TGC_Track_Pull_3Station_Track_With_Pt_Cut_C");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++) {
      nhist++;
      //cout<<dirname_v[i]<<histname[i][j]<<endl;
    }
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);
  else if (nhist<30) canv->Divide(5,5);

  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      	  drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw",doStats);
      //drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;


///////////////////////////////



// 
//   /////////////////
// 
//   cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing SegmentMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
//   canv->Clear();  ipad=0;
//   dirname_v.clear();
//   dirname_v.push_back(runNumber+"/Muon/MuonSegmentMonitoring/Global/"); // 0
//   dirname_v.push_back(runNumber+"/Muon/MuonSegmentMonitoring/BarrelA/Overview/");// 1
//   dirname_v.push_back(runNumber+"/Muon/MuonSegmentMonitoring/BarrelC/Overview/");// 2
//   dirname_v.push_back(runNumber+"/Muon/MuonSegmentMonitoring/EndCapA/Overview/");// 3
//   dirname_v.push_back(runNumber+"/Muon/MuonSegmentMonitoring/EndCapC/Overview/");// 4

//   histname.clear();
//   for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
//   histname[0].push_back("n_hits_per_segm_vs_station");
//   histname[0].push_back("segm_station_eta_vs_phi");
//   histname[0].push_back("n_segms");
//   histname[0].push_back("n_total_hits");
//   histname[0].push_back("n_precision_hits");
//   histname[0].push_back("n_phi_hits");
//   histname[0].push_back("chi2_per_ndof");
//   histname[0].push_back("mdt_Res");
//   histname[0].push_back("mdt_hitSignedRadius");
// 
//   int nhist=0;
//   for (int i=0; i<histname.size(); i++)
//     for (int j=0; j<histname[i].size(); j++)    nhist++;
// 
//   if (nhist<2)  pad->cd(0);
//   else if (nhist<3)  canv->Divide(1,2);
//   else if (nhist<5)  canv->Divide(2,2);
//   else if (nhist<7)  canv->Divide(2,3);
//   else if (nhist<9)  canv->Divide(2,4);
//   else if (nhist<10) canv->Divide(3,3);
//   else if (nhist<13) canv->Divide(3,4);
//   else if (nhist<17) canv->Divide(4,4);
// 
//   for (int idir=0; idir<dirname_v.size(); idir++) {
//     for (int ihist=0; ihist<histname[idir].size(); ihist++) {
//       canv->cd(++ipad);
//       drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Segments");
//       canv->Update();
//     }
//   }
//   npage++;

  //pdf->Close();
  ps->Close();
  return;
} //=====================================================================================================

// doStats   < 0 = false;  0 = use first bin for stats;  > 0 = use full histo for stats
void drawSupImp(string dirname, string histname,TFile *f,string TrkOrSegm, int doStats)
{
  if (TrkOrSegm=="Raw") { //No superimposing for RawData plots
    TDirectory *dir=f->GetDirectory(dirname.c_str());
    if( dir->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist=(TH1F*)dir->Get(histname.c_str());
    cout<<"---"<<histname<<endl;
    hist->Draw();
    return;
  }
  
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string dirname1=dirname+"ConvertedMBoy"+TrkOrSegm;
  string dirname2=dirname+"Moore"+TrkOrSegm;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
///
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);

  hist1->SetFillColor(0);
  hist2->SetFillColor(0);


  TLegend *leg = new TLegend(0.58,0.9,1,1); 

  float norm1 = hist1->GetEntries();
  float norm2 = hist2->GetEntries();
  float mean1 = 0.;
  float mean2 = 0.;
  float rms1  = 0.;
  float rms2  = 0.;


  // Special case:  Use first bin to normalize (e.g. for muon author)
  if ( doStats == 0 ) {   
    norm1 = hist1->GetBinContent(1);
    norm2 = hist2->GetBinContent(1);
  }

  if ( doStats > -1 ) {
    if ( !hist1->IsA()->InheritsFrom("TH2") && !hist1->IsA()->InheritsFrom("TProfile")) {
      if (norm1 > 0. ) hist1->Scale( 1./ norm1 );
      if (norm2 > 0. ) hist2->Scale( 1./ norm2 );
      mean1 = hist1->GetMean();
      mean2 = hist2->GetMean();
      rms1  = hist1->GetRMS();
      rms2  = hist1->GetRMS();
    }
  }
  double max =hist1->GetMaximum();
  double min =hist1->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();

  bool plotFirst = true;
  if ( max < max2 ){ 
    max = max2;
    plotFirst = false;
  }
  if ( min > min2 ){ min = min2;}
  if (min > 0) min = 0.;

  if ( !hist1->IsA()->InheritsFrom("TH2") && !hist1->IsA()->InheritsFrom("TProfile")) { 
    //    hist2->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
    if ( plotFirst ) {
      if ( doStats != 0) hist1->GetYaxis()->SetRangeUser(1.2*min,1.2*max);
      hist1->Draw();
      hist2->Draw("same");
    } else {
      if ( doStats != 0) hist2->GetYaxis()->SetRangeUser(1.2*min,1.2*max);
      hist2->Draw();
      hist1->Draw("same");
    }
  } 
  else if ( hist1->IsA()->InheritsFrom("TH2") ) {
    hist1->Draw("BOX");
    hist2->Draw("sameBOX");
  }
  else {
    hist1->Draw();
    hist2->Draw("same");
  }

  char t1[100];
  char t2[100];
  if ( doStats > 0 && (!hist1->IsA()->InheritsFrom("TH2") || !hist1->IsA()->InheritsFrom("TProfile")) ){
    sprintf(t1,"Muonboy entries= %d;  mean= %6.3f;  rms= %6.3f",hist1->GetEntries(),hist1->GetMean(1),hist1->GetRMS(1));
    sprintf(t2,"MOORE  entries= %d;  mean= %6.3f;  rms= %6.3f",hist2->GetEntries(),hist2->GetMean(1),hist2->GetRMS(1));
  }    
  else if ( doStats == 0 && (!hist1->IsA()->InheritsFrom("TH2") || !hist1->IsA()->InheritsFrom("TProfile")) ){
    sprintf(t1,"Muonboy: number of muons= %d",norm1);
    sprintf(t2,"MOORE:  number of muons= %d",norm2);

  } 
  else {
    sprintf(t1,"Muonboy entries: %d",hist1->GetEntries());
    sprintf(t2,"MOORE  entries: %d",hist2->GetEntries());
  }


  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  
  cout<<"---"<<histname<<endl;
  return;
}

//////////////
void drawSupImpResidualsBarrel(string dirname, string histname,TFile *f,string TrkOrSegm)
{
  if (TrkOrSegm=="Raw") { //No superimposing for RawData plots
    TDirectory *dir=f->GetDirectory(dirname.c_str());
    if( dir->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist=(TH1F*)dir->Get(histname.c_str());
    cout<<"---"<<histname<<endl;
    hist->Draw();
    return;
  }
  
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string resbarreldirname="/ResidualsBarrel";
  string dirname1=dirname+"ConvertedMBoy"+TrkOrSegm+resbarreldirname;
  string dirname2=dirname+"Moore"+TrkOrSegm+resbarreldirname;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  TLegend *leg = new TLegend(0.58,0.9,1,1); 
  double max=hist1->GetMaximum();
  double min=hist2->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();
  if ( max< max2 )  max=max2;
  if ( min< min2 )  min=min2;
  if (min>0) min=0;
  if (!hist1->IsA()->InheritsFrom("TH2")) { 
    hist1->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
  }
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);
  hist1->Draw();
  hist2->Draw("same");
  char t1[100];
  char t2[100];
  sprintf(t1,"ConvertedMBoy entries: %d",hist1->GetEntries());
  sprintf(t2,"Moore entries: %d",hist2->GetEntries());
  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  cout<<"---"<<histname<<endl;
  return;
}

void drawSupImpResidualsEndCapA(string dirname, string histname,TFile *f,string TrkOrSegm)
{
  if (TrkOrSegm=="Raw") { //No superimposing for RawData plots
    TDirectory *dir=f->GetDirectory(dirname.c_str());
    if( dir->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist=(TH1F*)dir->Get(histname.c_str());
    cout<<"---"<<histname<<endl;
    hist->Draw();
    return;
  }
  
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string resecadirname="/ResidualsEndCapA";
  string dirname1=dirname+"ConvertedMBoy"+TrkOrSegm+resecadirname;
  string dirname2=dirname+"Moore"+TrkOrSegm+resecadirname;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  TLegend *leg = new TLegend(0.58,0.9,1,1); 
  double max=hist1->GetMaximum();
  double min=hist2->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();
  if ( max< max2 )  max=max2;
  if ( min< min2 )  min=min2;
  if (min>0) min=0;
  if (!hist1->IsA()->InheritsFrom("TH2")) { 
    hist1->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
  }
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);
  hist1->Draw();
  hist2->Draw("same");
  char t1[100];
  char t2[100];
  sprintf(t1,"ConvertedMBoy entries: %d",hist1->GetEntries());
  sprintf(t2,"Moore entries: %d",hist2->GetEntries());
  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  cout<<"---"<<histname<<endl;
  return;
}

void drawSupImpResidualsEndCapC(string dirname, string histname,TFile *f,string TrkOrSegm)
{
  if (TrkOrSegm=="Raw") { //No superimposing for RawData plots
    TDirectory *dir=f->GetDirectory(dirname.c_str());
    if( dir->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist=(TH1F*)dir->Get(histname.c_str());
    cout<<"---"<<histname<<endl;
    hist->Draw();
    return;
  }
  
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string reseccdirname="/ResidualsEndCapC";
  string dirname1=dirname+"ConvertedMBoy"+TrkOrSegm+reseccdirname;
  string dirname2=dirname+"Moore"+TrkOrSegm+reseccdirname;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  TLegend *leg = new TLegend(0.58,0.9,1,1); 
  double max=hist1->GetMaximum();
  double min=hist2->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();
  if ( max< max2 )  max=max2;
  if ( min< min2 )  min=min2;
  if (min>0) min=0;
  if (!hist1->IsA()->InheritsFrom("TH2")) { 
    hist1->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
  }
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);
  hist1->Draw();
  hist2->Draw("same");
  char t1[100];
  char t2[100];
  sprintf(t1,"ConvertedMBoy entries: %d",hist1->GetEntries());
  sprintf(t2,"Moore entries: %d",hist2->GetEntries());
  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  cout<<"---"<<histname<<endl;
  return;
}

//////////////
void drawSupImpMuonTrkParamBarrel(string dirname, string histname,TFile *f,string TrkOrSegm)
{
  if (TrkOrSegm=="Raw") { //No superimposing for RawData plots
    TDirectory *dir=f->GetDirectory(dirname.c_str());
    if( dir->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist=(TH1F*)dir->Get(histname.c_str());
    cout<<"---"<<histname<<endl;
    hist->Draw();
    return;
  }
  
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string resbarreldirname="/Barrel";
  string dirname1=dirname+"ConvertedMBoy"+TrkOrSegm+resbarreldirname;
  string dirname2=dirname+"Moore"+TrkOrSegm+resbarreldirname;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  TLegend *leg = new TLegend(0.58,0.9,1,1); 
  double max=hist1->GetMaximum();
  double min=hist2->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();
  if ( max< max2 )  max=max2;
  if ( min< min2 )  min=min2;
  if (min>0) min=0;
  if (!hist1->IsA()->InheritsFrom("TH2")) { 
    hist1->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
  }
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);
  hist1->Draw();
  hist2->Draw("same");
  char t1[100];
  char t2[100];
  sprintf(t1,"ConvertedMBoy entries: %d",hist1->GetEntries());
  sprintf(t2,"Moore entries: %d",hist2->GetEntries());
  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  cout<<"---"<<histname<<endl;
  return;
}

void drawSupImpMuonTrkParamEndCap(string dirname, string histname,TFile *f,string TrkOrSegm)
{
  if (TrkOrSegm=="Raw") { //No superimposing for RawData plots
    TDirectory *dir=f->GetDirectory(dirname.c_str());
    if( dir->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
    TH1F *hist=(TH1F*)dir->Get(histname.c_str());
    cout<<"---"<<histname<<endl;
    hist->Draw();
    return;
  }
  
  gStyle->SetOptStat(0000); //disable Statistics Info legend
  string resbarreldirname="/EndCap";
  string dirname1=dirname+"ConvertedMBoy"+TrkOrSegm+resbarreldirname;
  string dirname2=dirname+"Moore"+TrkOrSegm+resbarreldirname;
  TDirectory *dir1=f->GetDirectory(dirname1.c_str());
  TDirectory *dir2=f->GetDirectory(dirname2.c_str());
  
  if( dir1->FindKey(histname.c_str()) == 0 && dir2->FindKey(histname.c_str()) == 0 ) { cout<<histname<<"  not found!!!\n"; return; }
  TH1F *hist1=(TH1F*)dir1->Get(histname.c_str());
  TH1F *hist2=(TH1F*)dir2->Get(histname.c_str());
  TLegend *leg = new TLegend(0.58,0.9,1,1); 
  double max=hist1->GetMaximum();
  double min=hist2->GetMinimum();
  double max2=hist2->GetMaximum();
  double min2=hist2->GetMinimum();
  if ( max< max2 )  max=max2;
  if ( min< min2 )  min=min2;
  if (min>0) min=0;
  if (!hist1->IsA()->InheritsFrom("TH2")) { 
    hist1->GetYaxis()->SetRangeUser(1.2*min,1.1*max); //don't mess up the range of a 2D histo!!!!
  }
  hist1->SetLineColor(kBlue);
  hist2->SetLineColor(kRed);
  hist1->SetMarkerColor(kBlue);
  hist2->SetMarkerColor(kRed);
  hist1->Draw();
  hist2->Draw("same");
  char t1[100];
  char t2[100];
  sprintf(t1,"ConvertedMBoy entries: %d",hist1->GetEntries());
  sprintf(t2,"Moore entries: %d",hist2->GetEntries());
  leg->AddEntry(hist1,t1,"lp");
  leg->AddEntry(hist2,t2,"lp");
  leg->Draw();
  cout<<"---"<<histname<<endl;
  return;
}

//////////////

int DrawMDTvsRPCSectors(TFile* file,string runNumber,TPad *pad,int npage) 
{
  //  cout<<"in DrawMDTvsRPCSectors()...\n";
  gStyle->SetOptStat(0000);//disable Statistics Info legend
  string item[3]={"HighPt", "LowPt", "Pivot"};

  for (int page=0; page<2; page++) { //drawing 8 sectors in each page...
    pad->cd(0); 
    pad->Update(); pad->Clear();
    pad->SetFillColor(10);
    pad->Divide(8,3);
    
    for (int i=1; i<=8; i++) {
      int isec=i+page*8;
      stringstream int2str;  
      string sectorID;
      int2str<<isec;
      int2str>>sectorID;
      if (sectorID.length()==1) {
      sectorID="0"+sectorID;     
      string dname=runNumber+"/Muon/MuonRawDataMonitoring/MDTvsRPC/Sectors/Sector"+sectorID;
      } else if (sectorID.length()==2){ string dname=runNumber+"/Muon/MuonRawDataMonitoring/MDTvsRPC/Sectors/Sector"+sectorID;}
      if (file->GetDirectory(dname.c_str())==0) {
	cout<<dname<<" not found\n";
	continue;
      }
      TDirectory* dir;
      dir=file->GetDirectory(dname.c_str());
      for (int iItem=0; iItem<3; iItem++) {
	string hName="Sector"+sectorID+"_"+item[iItem]+"_MDTtube_vs_RPCstrip";
	TH1F *hist=(TH1F*)dir->Get(hName.c_str());
	cout<<"---"<<hist->GetName()<<endl;
	int iPad=iItem*8+i;
	pad->cd(iPad);
	hist->SetTitle();
	hist->GetXaxis()->SetTitle(); 
	hist->GetYaxis()->SetTitle();	
	hist->Draw(); 
      }      
    } 
     
    pad->cd(0);
    string title = (page==0) ? "MDT tube vs RPC strip // Sectors 1-8" : "MDT tube vs RPC strip // Sectors 9-16";
    TPaveLabel* pageTitle=new TPaveLabel(0,0.96,1,1,title.c_str());   
    pageTitle->SetBorderSize(0);
    
    TPaveLabel* itemTitle[3];
    itemTitle[0]=new TPaveLabel( 0.98, 0.7, 1, 1., "HighPt" );
    itemTitle[1]=new TPaveLabel( 0.98, 0.35, 1, 0.7, "LowPt" );
    itemTitle[2]=new TPaveLabel( 0.98, 0, 1, 0.35, "Pivot" );
    
    pageTitle->Draw();
    for (int i=0; i<3; i++) { itemTitle[i]->SetBorderSize(0); itemTitle[i]->SetTextAngle(90); itemTitle[i]->SetTextSize(0.12); itemTitle[i]->Draw(); }
    pad->Update();
    npage++;
  }

  return npage;
}

