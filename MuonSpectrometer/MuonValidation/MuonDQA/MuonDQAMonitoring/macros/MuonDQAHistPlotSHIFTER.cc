/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================//
// *** MuonDQAHistPlotSHIFTER.cc **************************************************************** //
// DESCRIPTION:                                                                                   //
//  ROOT Macro for extracting to a single PDF all Offline Muon Monitoring Histos useful to shifter//
//  Author:  Ioannis Nomidis (AUTh - Thessaloniki, Sep. 2008)                                          //
//     ...                                                                                        //
// TO EXECUTE, in ROOT command line enter:                                                        //
//  .x MuonDQAHistPlotSHIFTER.cc("fileName")                                                      // 
//================================================================================================//

#include <string>
#include <vector>

void MuonDQAHistPlotSHIFTER( string fName="" )
{ 
  const char* MBoy  = "ConvertedMBoy";
  const char* Moore = "Moore";
  
  TFile* f=new TFile(fName.c_str(),"READ");

  //Get the run number - it's the top-folder's name in the root file!
  TIter nextcd0(gDirectory->GetListOfKeys());
  TKey *key0;
  key0 = (TKey*)nextcd0();
  TDirectory *dir0= (TDirectory*)key0->ReadObj();
  string runNumber=dir0->GetName(); //runNumber.substr(4) returns only the number
  std::string pdfTag=runNumber+".pdf";
  TDirectory *dir;
  // string dirname;
  // string histname;
  TCanvas *canv=new TCanvas("canv","Spectrometer Hists",0,0,850,650);
  TPad *pad=(TPad*) canv;
  //  canv->Clear();
  TPDF* pdf;
  stringstream pdfName;
  pdfName<<"MuonDqaShiftHist_"<<pdfTag;
  pdf=new TPDF(pdfName.str().c_str());

  int npage=0;      
  int ipad=0;
 
  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing RawDataMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  //MDT
  cout<<"((( MDT )))\n";
  vector<string> dirname;
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/Overview/"); // 0
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTBA/Overview/"); // 1 
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTBC/Overview/"); // 2
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTEA/Overview/"); // 3
  dirname.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDT/MDTEC/Overview/"); // 4   
  std::vector<std::vector<std::string> > histname;
  std::vector<std::string> histname_tmp;
  for (int i=0; i<dirname.size(); i++) histname.push_back(histname_tmp);
  histname[0].push_back("Number_of_Chambers_with_hits_per_event_ADCCut");
  histname[0].push_back("TotalNumber_of_MDT_hits_per_event_ADCCut"); 
  histname[0].push_back("Overall_TDC_ADCCut_spectrum");
  histname[0].push_back("Overall_TDCADC_spectrum");
  histname[0].push_back("NumberOfHitsInMDTInner_ADCCut");
  histname[0].push_back("NumberOfHitsInMDTMiddle_ADCCut");
  histname[0].push_back("NumberOfHitsInMDTOuter_ADCCut");
  histname[0].push_back("EffsInBarrelPerChamber_ADCCut");
  histname[0].push_back("EffsInEndCapPerChamber_ADCCut");
  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++)     nhist++;
 
  if (nhist<2)  pad->cd(0);
  else if (nhist<3)  canv->Divide(1,2);
  else if (nhist<5)  canv->Divide(2,2);
  else if (nhist<7)  canv->Divide(2,3);
  else if (nhist<9)  canv->Divide(2,4);
  else if (nhist<10) canv->Divide(3,3);
  else if (nhist<13) canv->Divide(3,4);
  else if (nhist<17) canv->Divide(4,4);

  for (int idir=0; idir<dirname.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname[idir],histname[idir][ihist],f,"Raw");
      canv->Update();
    }
  }
  npage++;

//   string region[4]={"BA", "BC",  "EA",  "EC",};
//   std::string layer[4]={"Inner","Middle","Outer", "Extra"};
//   int iec;
//   canv->Clear(); ipad=0; canv->Divide(3,3);
//   for (int idir=0; idir<dirname.size(); idir++) {
//   for (int ireg=0; ireg<4; ireg++) {
//     for (int ilayer=0; ilayer<4; ilayer++) {
//       string hName="NumberOfHitsIn"+region[ireg]+layer[ilayer]+"PerMultiLayer_ADCCut";
//       //      if (ireg<2) iec=0; else iec=1;
//       canv->cd(++ipad);
//       drawSupImp(dirname[idir],hName,f,"Raw");
//       canv->Update();
//     }
//   }
// }  

  std::string layer[4]={"Inner","Middle","Outer", "Extra"};
  int iec;
  canv->Clear(); ipad=0; canv->Divide(2,2);
    for (int ilayer=0; ilayer<4; ilayer++) {
      string hName="NumberOfHitsInBA"+layer[ilayer]+"PerMultiLayer_ADCCut";
       canv->cd(++ipad);
      drawSupImp(dirname[1],hName,f,"Raw");
      canv->Update();
    }
  
npage++;

  canv->Clear(); ipad=0; canv->Divide(2,2);
    for (int ilayer=0; ilayer<4; ilayer++) {
      string hName="NumberOfHitsInBC"+layer[ilayer]+"PerMultiLayer_ADCCut";
       canv->cd(++ipad);
      drawSupImp(dirname[2],hName,f,"Raw");
      canv->Update();
    }
  
npage++;

  canv->Clear(); ipad=0; canv->Divide(2,2);
    for (int ilayer=0; ilayer<4; ilayer++) {
      string hName="NumberOfHitsInEA"+layer[ilayer]+"PerMultiLayer_ADCCut";
       canv->cd(++ipad);
      drawSupImp(dirname[3],hName,f,"Raw");
      canv->Update();
    }
  
npage++;

  canv->Clear(); ipad=0; canv->Divide(2,2);
    for (int ilayer=0; ilayer<4; ilayer++) {
      string hName="NumberOfHitsInEC"+layer[ilayer]+"PerMultiLayer_ADCCut";
       canv->cd(++ipad);
      drawSupImp(dirname[4],hName,f,"Raw");
      canv->Update();
    }
  
npage++;
   
//   string region[3]={"EndCapA","EndCapC","Barrel"};
//   //  std::string ecap[2]={"EndCap","Barrel"};    
//   std::string layer[3]={"Inner","Middle","Outer"};
//   int iec;
//   canv->Clear(); ipad=0; canv->Divide(3,3);
//   for (int idir=0; idir<dirname.size(); idir++) {
//   for (int ireg=0; ireg<3; ireg++) {
//     for (int ilayer=0; ilayer<3; ilayer++) {
//       string hName="NumerOfHitsIn"+region[ireg]+layer[ilayer]+"PerMultiLayer_ADCcut";
//       //      if (ireg<2) iec=0; else iec=1;
//       canv->cd(++ipad);
//       drawSupImp(dirname[idir],hName,f,"Raw");
//       canv->Update();
//     }
//   }
//   }
//   npage++;
//   
  
 
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
  histname[3].push_back("MdtNHitsvsRPCNHits");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw");
      canv->Update();
    }
  }
  npage++;

  npage = DrawMDTvsRPCSectors(f,runNumber,(TPad*) canv,npage);

  //TGC
  cout<<"((( TGC )))\n";
  canv->Clear(); ipad=0;
  dirname_v.clear(); 
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/Global/");        //0
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEA/");        //1
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEA/Efficiency/");        //2 
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEC/");        //3
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGC/TGCEC/Efficiency/");        //4
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("Number_Of_TGC_Hits_Per_Event");
  histname[0].push_back("Number_Of_TGC_Strip_Hits_Per_Event");
  histname[0].push_back("Number_Of_TGC_Wire_Hits_Per_Event");
  histname[1].push_back("Strip_Hit_Profile_A");
  histname[1].push_back("Wire_Hit_Profile_A");
  histname[1].push_back("XY_View_A");
  histname[2].push_back("Efficiency_A");
  histname[2].push_back("Strip_Efficiency_A");
  histname[2].push_back("Wire_Efficiency_A");
  histname[3].push_back("Strip_Hit_Profile_C");
  histname[3].push_back("Wire_Hit_Profile_C");
  histname[3].push_back("XY_View_C");
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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw");
      canv->Update();
    }
  }
  npage++;
 

  //TGC
  cout<<"((( TGCLV1 )))\n";
  canv->Clear(); ipad=0;
  dirname_v.clear(); 
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/Global/");     //0
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/Global/Correlation/");     //1

  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/TGCEA/");     //2  
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/TGCLV1/TGCEC/");     //3  
   
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDTvsTGC/TGCEA/");//4
  dirname_v.push_back(runNumber+"/Muon/MuonRawDataMonitoring/MDTvsTGC/TGCEC/");//5
  
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  
  histname[0].push_back("Number_Of_SL_Triggers_Total");
  histname[1].push_back("RoI_Eta_Vs_Wire_Hit_Eta");
  histname[1].push_back("RoI_Phi_Vs_Strip_Hit_Phi"); 
  histname[2].push_back("LowPt_Timing_A");
  histname[2].push_back("Pt_Threshold_A");
  histname[2].push_back("SL_Sector_A");
  histname[2].push_back("SL_Timing_A");
  histname[2].push_back("RoI_Eta_Vs_Phi_A"); 
  histname[3].push_back("LowPt_Timing_C");
  histname[3].push_back("Pt_Threshold_C");
  histname[3].push_back("SL_Sector_C");
  histname[3].push_back("SL_Timing_C");
  histname[4].push_back("MDT_Eta_Minus_TGC_RoI_Eta_A");
  histname[4].push_back("MDT_Eta_Vs_TGC_RoI_Eta_A");
  histname[4].push_back("MDT_Phi_Vs_TGC_RoI_Phi_A");
  histname[5].push_back("MDT_Eta_Minus_TGC_RoI_Eta_C");
  histname[5].push_back("MDT_Eta_Vs_TGC_RoI_Eta_C");
  histname[5].push_back("MDT_Phi_Vs_TGC_RoI_Phi_C");
   
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
  else if (nhist<21) canv->Divide(5,4);
  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Raw");
      canv->Update();
    }
  }
  npage++;



  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonGenericTracksMon/"); //0
  dirname_v.push_back(runNumber+"/Muon/MuonTrackMonitoring/NoTrigger/MuonMonTrkSummary/");    //1
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("m_muon_receta_recphi");
  histname[0].push_back("m_recomuon_ChargeAsymmetry");
  histname[1].push_back("num_hits_per_Trk_vs_Eta");
  histname[1].push_back("num_hits_per_Trk_vs_Phi");

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


  for (int idir=0; idir<dirname_v.size(); idir++) {
    for (int ihist=0; ihist<histname[idir].size(); ihist++) {
      canv->cd(++ipad);
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Tracks");
      canv->Update();
    }
  }
  npage++;
 

  cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing SegmentMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
  canv->Clear();  ipad=0;
  dirname_v.clear();
  dirname_v.push_back(runNumber+"/Muon/MuonSegmentMonitoring/MuonGenericSegmsMon/");
  histname.clear();
  for (int i=0; i<dirname_v.size(); i++) histname.push_back(vector<string>());
  histname[0].push_back("m_num_hits_per_segm_vs_station");
  histname[0].push_back("m_segm_eta_vs_phi");
  histname[0].push_back("m_num_segms");

  int nhist=0;
  for (int i=0; i<histname.size(); i++)
    for (int j=0; j<histname[i].size(); j++)    nhist++;

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
      drawSupImp(dirname_v[idir],histname[idir][ihist],f,"Segments");
      canv->Update();
    }
  }
  npage++;

  pdf->Close();
  return;
} //=====================================================================================================

void drawSupImp(string dirname, string histname,TFile *f,string TrkOrSegm)
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

