/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================//
// *** MuonDQAShiftHistPlot.cc **************************************************************         //
// DESCRIPTION:                                                                                   //
//   ROOT Macro for extracting to a single PDF all Shift Monitoring Histos useful to Offline DQA. //
// Authors:  I. Nomidis (AUTH- Thessaloniki)  -  N. Benekos(Illinois)  ,  Feb.2008                //
//                                                                                                //
// ---->>>>For help, enter in linux console: cat README.txt                                       //
//================================================================================================//

void MuonDQAShiftHistPlot(char * fName="",std::TString runNumber="1",vector<TString> Sectors,bool doRawDataMon=true,bool doTrackMon=true,bool doSegmMon=true,bool doMBoyTrack=true,bool doMooreTrack=true,bool doMBoySegm=true,bool doMoMuSegm=true)
{
  const char* Overview="Overview";
  //const char* Summary="Summary";
  // const char* Chambers="Chambers";
  //  const char* Sectors="Sectors";

  TFile* f=new TFile(fName,"READ");
  TPDF* pdf;
  std::string pdfTag=runNumber+".pdf";
  stringstream pdfName;
  pdfName<<"MuonDQAShiftHistPlot"<<pdfTag;
  pdf=new TPDF(pdfName.str().c_str());

  TString mDir;
  TCanvas *canv=new TCanvas("canv","Spectrometer Hists",0,0,850,650);
  TPad *pad=(TPad*) canv;
  int npage=0;      
  TH1F *obj;
 
  if (doRawDataMon) {
    cout<<"\n>>>>>>>>>> Doing RawDataMonitoring <<<<<<<<<<"<<endl;
    mDir = "run_"+runNumber+"/Muon/MuonRawDataMonitoring";
    f->cd(mDir);
    std:: cout << "GDIRECTORY path: " << gDirectory->ls() << endl;
    TDirectory* currentDir = gDirectory;
    gDirectory=f->GetDirectory(mDir);
    TIter nextcd1(gDirectory->GetListOfKeys());
    TKey *key1;
    int nhist=0;
    while(key1 = (TKey*)nextcd1()) {//While in MDT/RPC/MDTvsRPC/RPCLvl1
      //     key1->Dump();     
      TDirectory *dir1= (TDirectory*)key1->ReadObj();
      TIter nextcd2(dir1->GetListOfKeys());
      TKey *key2;
      TString dirname=dir1->GetName();
      std::cout<<"Working in "<<dirname<<endl;

      if (dirname=="MDTvsRPC"){    //special treatment, extra folder Sectors/Sector# exists!
	TString SectorNum;
	for (int i=0; i<Sectors.size(); i++) {
	  SectorNum=Sectors[i];
	  TString mDirSec=mDir+"/"+dirname+"/Sectors/Sector"+SectorNum;
	  cout<<"@@@@@@@@@@@@"<< mDirSec<<endl;
	  gDirectory=f->GetDirectory(mDirSec);
	  TIter nextcd(gDirectory->GetListOfKeys());
	  npage=DrawObj(dirname,nextcd,npage,nhist,pdf,(TPad*)canv,SectorNum);
	} //end loop over sectors	  
      }
      else if (dirname=="TGC") {cout<<"\tempty...\n"; continue;} //drop tgc histos, they're empty

      else if (dirname=="MDT" || dirname=="RPC" || dirname=="RPVLV1") {
	while(key2 = (TKey*)nextcd2()) { //Chambers/Overview/Summary
	  TDirectory *dir2= (TDirectory*)key2->ReadObj();
	  TIter nextcd3(dir2->GetListOfKeys());
	  
	  
	  if(strncmp(dir2->GetName(),Overview,5)==0) {
	    int nhist=dir2->GetListOfKeys()->GetSize();
	    if (dirname=="MDT") {
	      npage=DrawObj("MDT_Hits",nextcd3,npage,nhist,pdf,(TPad*) canv);//@@@
	      npage=DrawObj("MDT_Barrel_Endcap_Hits",nextcd3,npage,nhist,pdf,(TPad*) canv);//@@@
	    }
	    	    
	    else if (dirname=="RPC" || dirname=="RPCLV1")
	      npage=DrawObj(dirname,nextcd3,npage,nhist,pdf,(TPad*) canv);//@@@
	    	    
	    else cout<<"warning...(???)\n";
	    
	  }
	} //end - while key2
      }
	
    }//end - while key1 
    
  } //close - if rawDataMon........................................

  if (doTrackMon) {
    cout<<"\n>>>>>>>>>> Doing TrackMonitoring <<<<<<<<<<"<<endl;
    mDir = "run_"+runNumber+"/Muon/MuonTrackMonitoring/MuonGenericTracksMon";
    f->cd(mDir);
    std:: cout << "GDIRECTORY path: " << gDirectory->ls() << endl;
    TDirectory* currentDir = gDirectory;
    gDirectory=f->GetDirectory(mDir);
    TIter nextcd1(gDirectory->GetListOfKeys());
    TKey *key1a,*key1b;
    key1a = (TKey*)nextcd1(); //1: Get MBoy - MOORE directories and contents
    key1b = (TKey*)nextcd1();
        
    cout<<"...Drawing: \n";
    TDirectory *dir1a= (TDirectory*)key1a->ReadObj();
    TString RecAlgoA=dir1a->GetName();
    TDirectory *dir1b= (TDirectory*)key1b->ReadObj();
    TString RecAlgoB=dir1b->GetName(); 
    
    TIter nextcd2a(dir1a->GetListOfKeys()); //2: Get histos from each folder
    TIter nextcd2b(dir1b->GetListOfKeys());
    
    if ( (doMooreTrack) && (!doMBoyTrack) ) {
      if (RecAlgoA=="ConvertedMBoyTracks") {RecAlgoA=RecAlgoB; nextcd2a=nextcd2b;}
      RecAlgoB=""; nextcd2b=0;
      cout<<"\t@ "<<RecAlgoA<<"   (green)"<< endl;
    }
    else if ( (doMBoyTrack) && (!doMooreTrack) ) {
      if (RecAlgoA=="MooreTracks") {RecAlgoA=RecAlgoB; nextcd2a=nextcd2b;}
      RecAlgoB=""; nextcd2b=0;
      cout<<"\t@ "<<RecAlgoA<<"   (green)"<< endl;
    }
    else {
      cout<<"\t@ "<<RecAlgoA<<"   (green)"<< endl;
      cout<<"\t@ "<<RecAlgoB<<"   (blue)"<< endl;
    }
        
    int nhist=dir1a->GetListOfKeys()->GetSize();
    if (nhist!=dir1b->GetListOfKeys()->GetSize()) 
      cout<<"WARNING! not equal number of histos...propably some are missing.\n";
         
    npage = DrawObjSupImposed("ResidualPull",RecAlgoA,nextcd2a,RecAlgoB,nextcd2b,nhist,npage,pdf,pad);
    npage = DrawObjSupImposed("TrackPars",RecAlgoA,nextcd2a,RecAlgoB,nextcd2b,nhist,npage,pdf,pad);
    npage = DrawObjSupImposed("Hits",RecAlgoA,nextcd2a,RecAlgoB,nextcd2b,nhist,npage,pdf,pad);
 
  }// close-if trackMon....................................................

  if (doSegmMon) {
    cout<<"\n>>>>>>>>>> Doing SegmentMonitoring <<<<<<<<<<"<<endl;
    mDir = "run_"+runNumber+"/Muon/MuonSegmentMonitoring/MuonGenericSegmsMon";
    f->cd(mDir);
    std:: cout << "GDIRECTORY path: " << gDirectory->ls() << endl;
    TDirectory* currentDir = gDirectory;
    gDirectory=f->GetDirectory(mDir);
    TIter nextcd1(gDirectory->GetListOfKeys());
    TKey *key1a,*key1b;
    key1a = (TKey*)nextcd1(); //1: Get MBoy - MOORE directories and contents
    key1b = (TKey*)nextcd1();
        
    cout<<"...Drawing: \n";
    TDirectory *dir1a= (TDirectory*)key1a->ReadObj();
    TString RecAlgoA=dir1a->GetName();
    TDirectory *dir1b= (TDirectory*)key1b->ReadObj();
    TString RecAlgoB=dir1b->GetName();
    TIter nextcd2a(dir1a->GetListOfKeys()); //2: Get histos from each folder
    TIter nextcd2b(dir1b->GetListOfKeys()); 

    if ( (doMoMuSegm) && (!doMBoySegm) ) {
      if (RecAlgoA=="ConvertedMBoySegments") {RecAlgoA=RecAlgoB; nextcd2a=nextcd2b;}
      RecAlgoB=""; nextcd2b=0;
      cout<<"\t@ "<<RecAlgoA<<"   (green)"<< endl;
    }
    else if ( (doMBoySegm) && (!doMoMuSegm) ) {
      if (RecAlgoA=="MuonSegments_MoMu") {RecAlgoA=RecAlgoB; nextcd2a=nextcd2b;}
      RecAlgoB=""; nextcd2b=0;
      cout<<"\t@ "<<RecAlgoA<<"   (green)"<< endl;
    }
    else {
      cout<<"\t@ "<<RecAlgoA<<"   (green)"<< endl;
      cout<<"\t@ "<<RecAlgoB<<"   (blue)"<< endl;
    }
      
    int nhist=dir1a->GetListOfKeys()->GetSize();
    if (nhist!=dir1b->GetListOfKeys()->GetSize()) 
      cout<<"WARNING! not equal number of histos...propably some are missing.\n";
      
    npage = DrawObjSupImposed("SegmAll",RecAlgoA,nextcd2a,RecAlgoB,nextcd2b,nhist,npage,pdf,pad);
  
  }// close-if segmMon....................................................
      
  pdf->Close();
  return;

} //***********************************************************************

int DrawObj(TString ColVars,TIter dirOfHists,int npage,int nhist,TPDF* pdf,TPad *pad,TString SectorNum="") // draws RawData Histos...
{
  TKey *key;
  int jPad=0;
  vector<TString> hists; //table of histo names
  pad->cd(0);  
  int nhists;
  if (npage>0) pad->Clear();
  pad->SetFillColor(10);

  if (ColVars=="MDT_Hits") { 
    hists.push_back("Number_of_MDT_hits_per_chamber");
    hists.push_back("TopTen_Number_of_MDT_hits_per_chamber");
    hists.push_back("Number_of_MDTs_with_hits_per_event");
    hists.push_back("TotalNumber_of_MDT_hits_per_event");
    hists.push_back("TotalNumber_of_MDT_hits_per_event_ADCcut");
  }
  else if (ColVars=="MDT_Barrel_Endcap_Hits") { 
    hists.push_back("Number_of_EndCapMDTHits_inRZView_Global");
    hists.push_back("Number_of_EndCapMDTHits_inYXView_Global");
    hists.push_back("Number_of_BarrelMDTHits_inRZView_Global");
    hists.push_back("Number_of_BarrelMDTHits_inYXView_Global");
    hists.push_back("Number_of_OverlapMDTHits_inRZView_Global");
    hists.push_back("Number_of_OverlapMDTHits_inYXView_Global");
  }
  else if (ColVars=="MDTvsRPC") {
    hists.push_back("Sector"+SectorNum+"_HighPt_MDTtube_vs_RPCstrip");
    hists.push_back("Sector"+SectorNum+"_LowPt_MDTtube_vs_RPCstrip");
    hists.push_back("Sector"+SectorNum+"_Pivot_MDTtube_vs_RPCstrip");
  }
  else if (ColVars=="RPC") {
    hists.push_back("RPC_Chamber_Stat");
    hists.push_back("Number_of_RPC_hits_per_event"); 
    hists.push_back("RPC_Chamber_TopTen_Stat"); 
    hists.push_back("RPC_hits_per_sector_logic");
  }
  else if (ColVars=="RPCLV1") {
    hists.push_back("CMAchannel_per_SectorLogic_Stat");
    hists.push_back("CMAchannel_per_towervslogic_Stat");
    hists.push_back("Trigger_Hits_HighPt_Stat");
    hists.push_back("Trigger_Hits_LowPt_Stat");
  }
  else if (ColVars=="TGC")
    return npage;
  else {
    cout<<"Warning???"<<endl;
    return npage;
  }

  nhists=hists.size();
  if      (nhists<2)  pad->cd(0);
  else if (nhists<3)  pad->Divide(1,2);
  else if (nhists<5)  pad->Divide(2,2);
  else if (nhists<7)  pad->Divide(2,3);
  else if (nhists<9)  pad->Divide(2,4);
  else if (nhists<10) pad->Divide(3,3);
  else if (nhists<13) pad->Divide(3,4);
  else if (nhists<17) pad->Divide(4,4);
  else {cout<<"### too many histos of "<<ColVars<<"... skipping!\n"<<endl; return npage;}

  while( key = (TKey*)dirOfHists() ) {
    TObject* obj1=key->ReadObj();
    TString hname=obj1->GetName();    
    if ( obj1->IsA()->InheritsFrom( "TH1" ) && isPartOf(hname,hists,nhists) ) {
      pad->cd(++jPad);
      obj1->Draw();
      pad->Update();  
    }
    //else cout<<"...neglecting "<<hname<<endl; 
  }
  hists.clear();
  return ++npage;
}

bool isPartOf(TString hname,vector<TString> &hists,int nhists) 
{
  int flag=0;
  int n=0;
  while (flag==0) {
    if (n==nhists)
      return false;
    if (hname==hists[n])
      flag=1;
    else if (n<nhists) 
      n++;
  }
  return true;
}

int DrawObjSupImposed(TString ColVars,TString dir1,TIter dirOfHists1,TString dir2="",TIter dirOfHists2,int nhist, int npage,TPDF* pdf,TPad *pad)  //draws Track&Segm Histos, with ability to superimpose results from 2 different reco algorithms
{ 
  int ipad=0;
  int npars;
  vector<TString> pars; //table of histo names
  TString hname;
  if (npage>0) pad->Clear();
  pad->SetFillColor(10);

  if (ColVars=="ResidualPull") {
    pars.push_back("All_Residuals");
    pars.push_back("MDT_Residuals");
    pars.push_back("RPC_Residuals");
    pars.push_back("All_Pull");
    pars.push_back("MDT_Pull");
    pars.push_back("RPC_Pull");
  }
  if (ColVars=="TrackPars") {
    pars.push_back("m_muon_Rec_chi2");
    pars.push_back("m_muon_Rec_d0");
    // pars.push_back("m_muon_Rec_z0");
    pars.push_back("m_muon_Rec_eta");
    pars.push_back("m_muon_Rec_phi0");
    // pars.push_back("m_recomuon_1overpT");
  }
  if (ColVars=="Hits") {
    pars.push_back("m_num_hits_per_track");
    pars.push_back("m_num_tracks");
    pars.push_back("m_muon_receta_recphi");
  }  
  if (ColVars=="SegmAll") {
    pars.push_back("allRes");
    pars.push_back("allPull");
    pars.push_back("mdtRes");
    pars.push_back("mdtPull");   
    pars.push_back("chi2");
    //pars.push_back("rpcRes");
    //pars.push_back("tgcRes");
    //pars.push_back("rpcPull");
    //pars.push_back("tgcPull");
  }

  npars=pars.size();
  if      (npars<2)  pad->cd(0);
  else if (npars<3)  pad->Divide(1,2);
  else if (npars<5)  pad->Divide(2,2);
  else if (npars<7)  pad->Divide(2,3);
  else if (npars<9)  pad->Divide(2,4);
  else if (npars<10) pad->Divide(3,3);
  else if (npars<13) pad->Divide(3,4);
  else if (npars<17) pad->Divide(4,4);
  else {cout<<"### got too many histos of "<<ColVars<<"... skipping!\n"<<endl; return npage;}

  TKey *key3;
  TH1F* obj1;
  TH1F* obj2;
  //minimum and maximum entry numbers of histo
  double min;  double max; 
  double min2; double max2;
   
  for (int j=0; j<nhist; j++) { //loop for all histos
    key3 = (TKey*)dirOfHists1();
    obj1=(TH1F*)key3->ReadObj();
    key3 = (TKey*)dirOfHists2();
    obj2=(TH1F*)key3->ReadObj();

    hname=obj1->GetName();
    if ( obj1->IsA()->InheritsFrom("TH1") && isPartOf(hname,pars,npars) ) {
      TLegend *leg = new TLegend(0.55,0.75,1,0.85);
      obj1->SetLineColor(kGreen);
      pad->cd(++ipad);
      leg->AddEntry(obj1,dir1,"l");
              
      max=obj1->GetMaximum();
      min=obj1->GetMinimum();
    
      hname=obj2->GetName();
      if (dir2!="") {  //plot 2 histos with proper range in y-axis
	max2=obj2->GetMaximum();
	min2=obj2->GetMinimum();
	if ( obj2->IsA()->InheritsFrom("TH1") && isPartOf(hname,pars,npars) ) {
	  if ( max< max2 )  max=max2;
	  if ( min< min2 )  min=min2;
	  if (min>0) min=0;
	  if (!obj2->IsA()->InheritsFrom("TH2"))  //don't mess up with the range of a 2D histo!!!!
	    obj1->GetYaxis()->SetRangeUser(1.2*min,1.1*max);
	  obj2->SetLineColor(kBlue);
	  obj1->Draw();
	  obj2->Draw("same");
	  leg->AddEntry(obj2,dir2,"l");  
	  leg->Draw();
	  pad->Update();
	}
      }
      else { //only 1 histo to plot
	obj1->Draw();
	leg->Draw();
	pad->Update();
      }	
    }

  } //end loop for all histos in both algos (key3)...
  
  return ++npage;   

}//end...DrawObjSupImposed()...........................................

 
