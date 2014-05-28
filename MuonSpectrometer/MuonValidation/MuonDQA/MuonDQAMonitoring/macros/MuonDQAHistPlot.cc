/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//================================================================================================
// *** MuonDQAHistPlot.cc *******************************************************************     
// DESCRIPTION:                                                                                   
//   ROOT Macro for extracting to a single PDF all Monitoring Histos useful to Offline DQA.       
// Authors:  I. Nomidis (AUTh - Thessaloniki)  -  N. Benekos(Illinois)  Feb. 2008
//     ...                                                                                        
// TO EXECUTE, in ROOT command line enter:                                                        
//  .x MuonDQAHistPlot.cc("fileName",                                                              
//                         doRawDataMon,doTrackMon,doSegmMon,                                     
//                         doMBoyTrack,doMooreTrack,doMBoySegm,doMooreSegm,                       
//                         ExtendedMon)                                                           
// ...........................................................................................
// -> Do not omit quotes("").                                                                     
// -> doSomething are boolean flags, use true or false to enable/disable or just omit them and    
//    get default values (true for all!).
// -> doMBoyTrack/Segm & doMooreTrack/Segm enable display for each reconstruction algo.           
// -> If data for one of those algorithms do not exist, user should better disable its display.   
// -> ExtendedMon displays more histograms, useful to experts
//================================================================================================

int DrawObj(TIter dirOfHists,int npage,int nhist,TPDF *pdf,TPad *pad,string tech="",string folder="");
int DrawMDTSummary(TFile* file,TPDF* sumPDF,int numPdf,std::string runNumID="1",bool doKey=true);
int DrawMDTvsRPCSectors(TFile* file,TPDF* sumPDF,int numPdf,string runNumID,TPad *pad);
int DrawTGCProfile(TFile* file,TPDF* sumPDF,int numPdf,string runNumID,TPad *pad);
int DrawTGCLV1Correlation(TFile* file,TPDF* sumPDF,int numPdf,string runNumID,TPad *pad) ;

bool doSL=false;
bool doMDTch=false;
bool doRPCch=false;
bool doMDTvsRPCch=false;
string extendedMon[] = { "Expert", "Phi48", "Sector", "Chambers", "Summary", "Efficiency", "Profile", "Sectors", "SectorLogic", "Layer", "Timing", "Correlation","BA","BC","EA","EC","tmp" };

#include <string>

void MuonDQAHistPlot( string fName="", 
		      bool doRawDataMon=true,bool doTrackMon=true,bool doSegmMon=true, 
		      bool doMBoyTrack=true,bool doMooreTrack=true, bool doMBoySegm=true,bool doMooreSegm=true,
		      bool ExtendedMon=true,bool doChambers=false )
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
  string mDir;
  TCanvas *canv=new TCanvas("canv","Spectrometer Hists",0,0,850,650);
  TPad *pad=(TPad*) canv;

  if (doRawDataMon) { 
    cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing RawDataMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl; 
    mDir = runNumber+"/Muon/MuonRawDataMonitoring";
    f->cd( mDir.c_str() );
    TDirectory* currentDir=gDirectory;
    int nPDFs=currentDir->GetNkeys();  //new pdf for each technology
    const int q=nPDFs;
    TPDF* Pdf[q];

    int loop=0;  
    int npage;
    int nhist;
    
    TIter nextcd1(currentDir->GetListOfKeys());
    TKey *key1;
    
    while(key1 = (TKey*)nextcd1()) {
      canv->Clear();     npage=0;
      TDirectory *dir1= (TDirectory*)key1->ReadObj(); //MDT,RPC,MDTvsRPC,TGC,CSC,RPCLV1,TGCLV1
      TIter nextcd2(dir1->GetListOfKeys());
      TKey *key2;
      //start new pdf
      stringstream pdfName;
      pdfName<<dir1->GetName()<<pdfTag;
      Pdf[loop]=new TPDF(pdfName.str().c_str());
      
      cout<<"_____________________________________"<<endl;
      cout<<"\n|     "<<dir1->GetName()<<endl;
      cout<<"_____________________________________"<<endl; 
      
      while(key2 = (TKey*)nextcd2()) { 
	TDirectory *dir2= (TDirectory*)key2->ReadObj(); //1st level in each technology folder
	cout<<"\t@ "<<dir2->GetName()<<endl;
	if ( isExtendedMon(dir2->GetName()) ) {
	  if (!ExtendedMon)  {  cout<<">> Skipping!\n";  continue;  }
	  else if (ExtendedMon) {  doExtendedMon(dir1->GetName(),dir2->GetName(), f, Pdf[loop], npage,runNumber,true,(TPad*)canv); continue;  }
	} 
	
	TIter nextcd3(dir2->GetListOfKeys());
	TKey *key3;
	//count histograms...
	nhist=0;
	while(key3 = (TKey*)nextcd3()) {
	  TObject* obj=key3->ReadObj();
	  if ( obj->IsA()->InheritsFrom("TH1") ) nhist++;
	}
	nextcd3.Reset();
	if (nhist>0)  npage=DrawObj(nextcd3,npage,nhist,Pdf[loop],(TPad*)canv, dir1->GetName(), dir2->GetName());      
	
	while(key3 = (TKey*)nextcd3()) { 
	  TObject* dirOrHist=key3->ReadObj();
	  if ( !dirOrHist->IsA()->InheritsFrom("TDirectoryFile") ) break; //protection against metadata TTree
	  TDirectory *dir3 = (TDirectory*)key3->ReadObj(); //2nd level
	  cout<<"\t\t"<<dir3->GetName()<<endl;
	  if ( isExtendedMon(dir3->GetName()) ) {
	    if (!ExtendedMon)  {  cout<<">> Skipping!\n";  continue;  }
	    else if (ExtendedMon) {  doExtendedMon(dir1->GetName(),dir3->GetName(), f, Pdf[loop], npage,runNumber,true,(TPad*)canv); continue;  }
	  }
	  
	  TIter nextcd4(dir3->GetListOfKeys());
	  TKey *key4;
	  //count histos
	  nhist=0;
	  while(key4 = (TKey*)nextcd4()) {
	    TObject* obj=key4->ReadObj();
	    if ( obj->IsA()->InheritsFrom("TH1") ) nhist++;
	  }
	  nextcd4.Reset();
	  if (nhist>0)  npage=DrawObj(nextcd4,npage,nhist,Pdf[loop],(TPad*)canv, dir1->GetName(), dir3->GetName());

	  while(key4 = (TKey*)nextcd4()) { 
	    TObject* dirOrHist=key4->ReadObj();
	    if ( !dirOrHist->IsA()->InheritsFrom("TDirectoryFile") ) break; //protection against metadata TTree 
	    TDirectory *dir4 = (TDirectory*)key4->ReadObj(); //3rd level
	    cout<<"\t\t@@ "<<dir4->GetName()<<endl;
	    if ( isExtendedMon(dir4->GetName()) ) {
	      if (!ExtendedMon)  {  cout<<">> Skipping!\n";  continue;  }
	      else if (ExtendedMon) {  doExtendedMon(dir1->GetName(),dir4->GetName(), f, Pdf[loop], npage,runNumber,true,(TPad*)canv ); continue;  }
	    }
	    TIter nextcd5(dir4->GetListOfKeys());
	    TKey *key5;
	    //count histos
	    nhist=0;
	    while(key5 = (TKey*)nextcd5()) {
	      TObject* obj=key5->ReadObj();
	      if ( obj->IsA()->InheritsFrom("TH1") ) nhist++;
	    }	  
	    nextcd5.Reset();
	    if (nhist>0)  npage=DrawObj(nextcd5,npage,nhist,Pdf[loop],(TPad*)canv, dir1->GetName(), dir4->GetName());
	  }
	}
      }
      Pdf[loop]->Close();
      loop++;
    }//While key1

    pdfName.clear();
  } //close - if rawDataMon...............................................................................

  TPDF* pdf;
  int npage=0;      

  if (doTrackMon) {   
    cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing TrackMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    canv->Clear();
    stringstream pdfName;
    pdfName<<"MuonDqaTrackHist_"<<pdfTag;
    pdf=new TPDF(pdfName.str().c_str());
    int numTriggerItems=5;
    mDir = runNumber+"/Muon/MuonTrackMonitoring/";
    f->cd(mDir.c_str());

    TDirectory* currentDir = gDirectory;
    TIter nextcd(currentDir->GetListOfKeys());
    TKey *key;

    while (key = (TKey*)nextcd()) {
      currentDir = (TDirectory*)key->ReadObj();  // NoTrigger, TriggerAware
      string dirname=currentDir->GetName();
      cout<<"("<< dirname <<")\n";
      TIter nextcd1(currentDir->GetListOfKeys());
      TKey *key1;

      //get hist: "NumberOfEventsThatPassedTriggers"
      //count histos
      int nhist=0;
      while(key1 = (TKey*)nextcd1()) {
	TObject* obj=key1->ReadObj();
	if ( obj->IsA()->InheritsFrom("TH1") ) nhist++;
      }	  
      nextcd1.Reset();
      if (nhist>0)  { cout<<"FOUND "<<nhist<<"!!!!!!"<<endl; npage=DrawObj(nextcd1,npage,nhist,pdf,pad); }
 
      while (key1 = (TKey*)nextcd1()) { 
	currentDir = (TDirectory*)key1->ReadObj();  // GenericTraks, HitsPerTrk, TrkHoles, TrkParameters, TrkSummary
	string dirname1=currentDir->GetName();
	if (dirname1.find("metadata")<dirname1.length() || currentDir->IsA()->InheritsFrom( "TH1" ) ) { continue; } ////protection against metadata TTree and hists

	cout<<"\n************************************************************ " << dirname1 << endl;
	TIter nextcd2(currentDir->GetListOfKeys());
	TKey *key2a,*key2b;
	int items = (dirname=="NoTrigger") ? 1 : numTriggerItems;

	for(int i=0; i<items; i++) {
	  key2a = (TKey*)nextcd2();
	  key2b = (TKey*)nextcd2();  
	  cout<<"...Drawing: \n";
	  TDirectory *dir2a = (TDirectory*)key2a->ReadObj(); // Get MBoy - MOORE directories
	  TDirectory *dir2b = (TDirectory*)key2b->ReadObj();
	  string itemA=dir2a->GetName();
	  string itemB=dir2b->GetName();  
	  TIter nextcd3a(dir2a->GetListOfKeys()); // Get histos from each folder
	  TIter nextcd3b(dir2b->GetListOfKeys());
	  
	  if ( (doMooreTrack) && (!doMBoyTrack) ) {
	    if (strncmp(dir2a->GetName(),MBoy,13)==0) {itemA=itemB; nextcd3a=nextcd3b;}
	    itemB=""; nextcd3b=0;
	    cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	  }
	  else if ( (doMBoyTrack) && (!doMooreTrack) ) {
	    if (strncmp(dir2a->GetName(),Moore,5)==0) {itemA=itemB; nextcd3a=nextcd3b;}
	    itemB=""; nextcd3b=0;
	    cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	  }
	  else {
	    cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	    cout<<"\t@ "<<itemB<<"   (blue)"<< endl;
	  }
	  //count hists:
	  int nhist=0;
	  TKey *key3a; TKey *key3b;
	  while( (key3a = (TKey*)nextcd3a()) && (key3b = (TKey*)nextcd3b()) ) {
	    TObject* obja=key3a->ReadObj();
	    TObject* objb=key3b->ReadObj();
	    if ( obja->IsA()->InheritsFrom( "TH1" ) ) nhist++;
	    else if (obja->IsA()->InheritsFrom( "TDirectory" ) && (ExtendedMon) ) {
	      string dirnameExt=obja->GetName();
	      cout<<"\t\t\n\n  ("<<dirnameExt<<")\n\n\n";
	      if( dirnameExt.find("Residuals") < dirname.length() || dirnameExt.find("MDTHitsPerChamber") < dirname.length() ) { cout<<">> Skipping!\n"; continue; }
	      TDirectory *dir3a = (TDirectory*)key3a->ReadObj();
	      TDirectory *dir3b = (TDirectory*)key3b->ReadObj();
	      TIter nextcd4a(dir3a->GetListOfKeys()); // Get histos from each folder
	      TIter nextcd4b(dir3b->GetListOfKeys());
	      TKey *key4a; TKey *key4b;
	      int nhist2=0;
	      while ( key4a=(TKey*) nextcd4a() ) {
		TObject* obja2=key4a->ReadObj();
		if ( obja2->IsA()->InheritsFrom( "TH1" ) ) nhist2++;
	      }
	      nextcd4a.Reset();
	      nextcd4b.Reset();
	      npage = DrawObjSupImposed("ALLHISTOS",itemA,nextcd4a,itemB,nextcd4b,nhist2,npage,pdf,pad); 	      
	    }
	  }
	  nextcd3a.Reset();	  
	  nextcd3b.Reset();	  
	  
	  if( dirname1.find("Generic") < dirname1.length() ) {
	    //Since there are too many hists in Generic folder, they are organised in collections & r drawn in 1 page each
	    npage=DrawObjSupImposed("TrackResiduals"     ,itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	    npage=DrawObjSupImposed("TrackPulls"         ,itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	    npage=DrawObjSupImposed("TrackDir"           ,itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	    npage=DrawObjSupImposed("TrackPos"           ,itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	    npage=DrawObjSupImposed("TrackMomentumCharge",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	    npage=DrawObjSupImposed("TrackErrors"        ,itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  }
	  else {
	    npage = DrawObjSupImposed("ALLHISTOS",itemA,nextcd3a,itemB,nextcd3b,nhist,npage,pdf,pad);
	  }
	}
      }
    }
    
    pdfName.clear();
    pdf->Close();
  }// close-if trackMon-----------------------------------------------------------------------

  if (doSegmMon) {
    cout<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>> Doing SegmentMonitoring <<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    canv->Clear();
    stringstream pdfName;
    pdfName<<"MuonDqaSegmentHist"<<pdfTag;
    pdf=new TPDF(pdfName.str().c_str());
    mDir = runNumber+"/Muon/MuonSegmentMonitoring";
    f->cd(mDir.c_str());
    TDirectory* currentDir = gDirectory;
    
    //////////>
    TIter nextcd1(currentDir->GetListOfKeys());
    TKey *key1;

    /*    while (key = (TKey*)nextcd()) { 
	  currentDir = (TDirectory*)key->ReadObj();  // NoTrigger, TriggerAware
	  string dirname=currentDir->GetName();
	  cout<<"("<< dirname <<")\n";
	  TIter nextcd1(currentDir->GetListOfKeys());
	  TKey *key1;*/

    while (key1 = (TKey*)nextcd1()) { 
      currentDir = (TDirectory*)key1->ReadObj();  // Generic-Selected Segm Mon
      string dirname1=currentDir->GetName();
      cout<<"\n************************************************************ " << dirname1 << endl;
      TIter nextcd2(currentDir->GetListOfKeys());
      int nFolders=0;
      if ( dirname1.find("Selected") < dirname1.length() ) { 
	if (ExtendedMon) {
	  cout<<">> Skipping!\n"; continue;
	  /*
	    TKey *key20;
	    while(key20=(TKey*)nextcd2()) {
	    TDirectory *dir20 = (TDirectory*)key20->ReadObj(); //extra level: Barrel-EndCap
	    cout<<"\n\n\t\t   ("<<dir20->GetName()<<")\n\n\n";
	    TIter nextcd20(dir20->GetListOfKeys());	  
	    TKey *key2a,*key2b;	
	    //	int items = (dirname=="NoTrigger") ? 1 : numTriggerItems;
	    //	for(int i=0; i<items; i++) {
	    key2a = (TKey*)nextcd20(); 
	    key2b = (TKey*)nextcd20();	  
	    cout<<"...Drawing: \n";
	    TDirectory *dir2a = (TDirectory*)key2a->ReadObj(); // Get MBoy - MOORE directories
	    TDirectory *dir2b = (TDirectory*)key2b->ReadObj();
	    string itemA=dir2a->GetName();
	    string itemB=dir2b->GetName();	  
	    TIter nextcd3a(dir2a->GetListOfKeys()); // Get histos from each folder
	    TIter nextcd3b(dir2b->GetListOfKeys());
	      
	    if ( (doMooreSegm) && (!doMBoySegm) ) {
	    if (strncmp(dir2a->GetName(),MBoy,13)==0) {itemA=itemB; nextcd3a=nextcd3b;}
	    itemB=""; nextcd3b=0;
	    cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	    }
	    else if ( (doMBoySegm) && (!doMooreSegm) ) {
	    if (strncmp(dir2a->GetName(),Moore,5)==0) {itemA=itemB; nextcd3a=nextcd3b;}
	    itemB=""; nextcd3b=0;
	    cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	    }
	    else {
	    cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	    cout<<"\t@ "<<itemB<<"   (blue)"<< endl;
	    }
	    //count hists:
	    int nhist=0;
	    TKey *key3;
	    while(key3 = (TKey*)nextcd3a()) {
	    TObject* obj=key3->ReadObj();
	    if ( obj->IsA()->InheritsFrom( "TH1" ) ) nhist++;
	    }
	    nextcd3a.Reset();	  
	    npage = DrawObjSupImposed("ALLHISTOS",itemA,nextcd3a,itemB,nextcd3b,nhist,npage,pdf,pad);
	    }
	  */
	}
      }
      else {
	TKey *key2a,*key2b;	
	//	int items = (dirname=="NoTrigger") ? 1 : numTriggerItems;
	//	for(int i=0; i<items; i++) {
	key2a = (TKey*)nextcd2(); 
	key2b = (TKey*)nextcd2();	  
	cout<<"...Drawing: \n";
	TDirectory *dir2a = (TDirectory*)key2a->ReadObj(); // Get MBoy - MOORE directories
	TDirectory *dir2b = (TDirectory*)key2b->ReadObj();
	string itemA=dir2a->GetName();
	string itemB=dir2b->GetName();	  
	TIter nextcd3a(dir2a->GetListOfKeys()); // Get histos from each folder
	TIter nextcd3b(dir2b->GetListOfKeys());
	  
	if ( (doMooreSegm) && (!doMBoySegm) ) {
	  if (strncmp(dir2a->GetName(),MBoy,13)==0) {itemA=itemB; nextcd3a=nextcd3b;}
	  itemB=""; nextcd3b=0;
	  cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	}
	else if ( (doMBoySegm) && (!doMooreSegm) ) {
	  if (strncmp(dir2a->GetName(),Moore,5)==0) {itemA=itemB; nextcd3a=nextcd3b;}
	  itemB=""; nextcd3b=0;
	  cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	}
	else {
	  cout<<"\t@ "<<itemA<<"   (green)"<< endl;
	  cout<<"\t@ "<<itemB<<"   (blue)"<< endl;
	}
	//count hists:
	int nhist=0;
	TKey *key3;
	while(key3 = (TKey*)nextcd3a()) {
	  TObject* obj=key3->ReadObj();
	  if ( obj->IsA()->InheritsFrom( "TH1" ) ) nhist++;
	}
	nextcd3a.Reset();	  
	  
	if( dirname1.find("Generic") < dirname1.length() ) {
	  //Since there are too many hists in Generic folder, they are organised in collections & r drawn in 1 page each
	  npage = DrawObjSupImposed("SegmentResiduals",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentPulls",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentPars",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentHits",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentChamberHits",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentChambers",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentRelPos",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	  npage = DrawObjSupImposed("SegmentHitErrors",itemA,nextcd3a,itemB,nextcd3b,0,npage,pdf,pad);
	}
	else { cout<<">> Skipping!\n";  continue; }
      }
    }
    //    }

    pdfName.clear();
    pdf->Close();

  }// close-if segmMon-----------------------------------------------------------------------------.
      
  return;
} //=============================================================================================================

int DrawObjSupImposed(string ColVars,string dir1,TIter dirOfHists1,string dir2="",TIter dirOfHists2,int nhist, int npage,TPDF* pdf,TPad *pad)  //draws Track&Segm Histos, with ability to superimpose results from 2 different reco algorithms
{ 
  bool manyHists = false; //hack! when there are more than 16 hists in a folder they are displayed in more than 1 page...
  int ipad=0;
  int npars=0;
  vector<string> pars; //table of histo names
  pars.clear();
  string hname;
  if (npage>0) pad->Clear();
  pad->SetFillColor(10);

  if (nhist>16) {
    manyHists=true;  cout<<"Found "<<nhist<<" hists (>16).";
    if (nhist==245) {  //segm-selected (barrel/ec)
      nhist = 5;  cout<<"Taking "<<nhist<<" per page...\n";
    }      
    if (nhist==49) {  //trk-hitsPerTrk (Mdt Hits)
      nhist = 9;  cout<<"Taking "<<nhist<<" per page...\n";
    }      
  }
  //TRACKS...
  if (ColVars=="TrackResiduals") {
    pars.push_back("MDT_Residuals");
    pars.push_back("RPC_Residuals");
    pars.push_back("TGC_Residuals");
    pars.push_back("CSC_Residuals");
    pars.push_back("m_rpc_res_eta");
    pars.push_back("m_rpc_res_phi");
    pars.push_back("m_tgc_res_eta");
    pars.push_back("m_tgc_res_phi");
    pars.push_back("m_csc_res_eta");
    pars.push_back("m_csc_res_phi");
    pars.push_back("m_muon_Rec_chi2");
    pars.push_back("m_muontrackndof");
  }
  else if (ColVars=="TrackPulls") {
    pars.push_back("MDT_Pull");
    pars.push_back("RPC_Pull");
    pars.push_back("TGC_Pull");
    pars.push_back("CSC_Pull");
    pars.push_back("m_rpc_pull_eta");
    pars.push_back("m_rpc_pull_phi");
    pars.push_back("m_tgc_pull_eta");
    pars.push_back("m_tgc_pull_phi");
    pars.push_back("m_csc_pull_eta");
    pars.push_back("m_csc_pull_phi");
  }
  else if (ColVars=="TrackDir") {
    pars.push_back("m_muon_Rec_d0");
    pars.push_back("m_muon_Rec_z0");
    pars.push_back("m_muon_Rec_eta");
    pars.push_back("m_muon_Rec_theta");
    pars.push_back("m_muon_Rec_phi0");
    pars.push_back("m_muon_receta_recphi");
  }
  else if (ColVars=="TrackPos") {
    pars.push_back("trk_rec_locR");
    pars.push_back("trk_rec_locX");
    pars.push_back("trk_rec_locY");
    pars.push_back("trk_rec_posR");
    pars.push_back("trk_rec_posX");
    pars.push_back("trk_rec_posY");
    pars.push_back("trk_rec_posZ");
    pars.push_back("trk_rec_phi");
    pars.push_back("trk_rec_theta");   
    pars.push_back("trk_rec_weights");
  }
  else if (ColVars=="TrackErrors") {
    pars.push_back("trk_rec_error_locR");
    pars.push_back("trk_rec_error_locX");
    pars.push_back("trk_rec_error_locY");
    pars.push_back("trk_rec_error_phi");
    pars.push_back("trk_rec_error_theta");
    pars.push_back("trk_rec_error_qOverP");
    pars.push_back("m_muon_error_d0");
    pars.push_back("m_muon_error_z0");
    pars.push_back("m_muon_error_phi0");
    pars.push_back("m_muon_error_theta0");
    pars.push_back("m_muon_error_qoverp");
  }
  else if (ColVars=="TrackMomentumCharge") {
    pars.push_back("m_muon_Rec_P");
    pars.push_back("m_charge_muontracks");
    pars.push_back("m_muon_Rec_qOverP");
    pars.push_back("m_muon_Rec_pT_vrs_eta");
    pars.push_back("m_muon_Rec_pT_vrs_phi");
    pars.push_back("m_recomuon_1overpT");
    pars.push_back("m_recomuon_pT");
    pars.push_back("m_recomuon_NEG_pT");
    pars.push_back("m_recomuon_POS_pT");
    pars.push_back("m_recomuon_HighpT");
    pars.push_back("m_recomuon_NEG_HighpT");
    pars.push_back("m_recomuon_POS_HighpT");
  }

  //SEGMENTS...
  else if (ColVars=="SegmentResiduals") {
    pars.push_back("all_technologies_Res");
    pars.push_back("mdt_Res");
    pars.push_back("rpcEta_Res");
    pars.push_back("rpcPhi_Res");
    pars.push_back("tgcEta_Res");
    pars.push_back("tgcPhi_Res");
    pars.push_back("cscEta_Res");
    pars.push_back("cscPhi_Res");
  }
  else if (ColVars=="SegmentPulls") {
    pars.push_back("all_technologies_Pull");
    pars.push_back("mdt_Pull");
    pars.push_back("rpcEta_Pull");
    pars.push_back("rpcPhi_Pull");
    pars.push_back("tgcEta_Pull");
    pars.push_back("tgcPhi_Pull");
    pars.push_back("cscEta_Pull");
    pars.push_back("cscPhi_Pull");
  }
  else if (ColVars=="SegmentPars") {
    pars.push_back("segm_position_eta");
    pars.push_back("segm_position_phi");
    pars.push_back("segm_position_z");
    pars.push_back("segm_position_r");
    pars.push_back("segm_direction_eta");
    pars.push_back("segm_direction_phi");
    pars.push_back("m_segm_etavsphi");
    pars.push_back("chi2_per_ndof");
    pars.push_back("chi2_probability");
    pars.push_back("chi2_vs_station");
    pars.push_back("ndof");
    pars.push_back("mdt_hitSignedRadius");
  }
  else if (ColVars=="SegmentChamberHits") {
    pars.push_back("n_total_hits");
    pars.push_back("n_precision_hits");
    pars.push_back("n_phi_hits");
    pars.push_back("n_eta_trig_hits");
    pars.push_back("n_mdt_hits");
    pars.push_back("n_rpcEta_hits");
    pars.push_back("n_rpcPhi_hits");
    pars.push_back("n_tgcEta_hits");
    pars.push_back("n_tgcPhi_hits");
    pars.push_back("n_cscEta_hits");
    pars.push_back("n_cscPhi_hits");
  }
  else if (ColVars=="SegmentChambers") {
    pars.push_back("n_total_chambers");
    pars.push_back("n_precision_chambers");
    pars.push_back("n_phi_chambers");
    pars.push_back("n_eta_trig_chambers");
    pars.push_back("n_mdt_chambers");
    pars.push_back("n_rpcEta_chambers");
    pars.push_back("n_rpcPhi_chambers");
    pars.push_back("n_tgcEta_chambers");
    pars.push_back("n_tgcPhi_chambers");
    pars.push_back("n_cscEta_chambers");
    pars.push_back("n_cscPhi_chambers");
  }
  else if (ColVars=="SegmentHits") {
    pars.push_back("m_num_hits_per_segm_vs_station");
    pars.push_back("m_num_hits_per_segmvsphi");
    pars.push_back("m_num_layers_per_segm");
    pars.push_back("m_num_mlayers_per_segm");
    pars.push_back("m_num_segms");
  }
  else if (ColVars=="SegmentRelPos") {
    pars.push_back("mdt_hitRelPosY");
    pars.push_back("rpcEta_hitRelPosX");
    pars.push_back("rpcEta_hitRelPosY");
    pars.push_back("rpcPhi_hitRelPosX");
    pars.push_back("rpcPhi_hitRelPosY");
    pars.push_back("tgcEta_hitRelPosX");
    pars.push_back("tgcEta_hitRelPosY");
    pars.push_back("tgcPhi_hitRelPosX");
    pars.push_back("tgcPhi_hitRelPosY");
    pars.push_back("cscEta_hitRelPosX");
    pars.push_back("cscEta_hitRelPosY");
    pars.push_back("cscPhi_hitRelPosX");
    pars.push_back("cscPhi_hitRelPosY");
  }
  else if (ColVars=="SegmentHitErrors") {
    pars.push_back("mdt_hitError");
    pars.push_back("rpcEta_hitError");
    pars.push_back("rpcPhi_hitError");
    pars.push_back("tgcEta_hitError");
    pars.push_back("tgcPhi_hitError");
    pars.push_back("cscEta_hitError");
    pars.push_back("cscPhi_hitError");
  }
  else if (ColVars=="ALLHISTOS") {
    int i=1;
    do pars.push_back("JOKER"); while (++i<=nhist);
  }
  else cout<<"Error in DrawObjSupImposed(): Wrong name of histogram collection...\n";
 
  if (pars.size()!=nhist) {
    if (ColVars!="ALLHISTOS") nhist=pars.size();
    else cout<<"Error! Wrong number of hists...\n";
  }
  if      (nhist<2)  pad->cd(0);
  else if (nhist<3)  pad->Divide(1,2);
  else if (nhist<5)  pad->Divide(2,2);
  else if (nhist<7)  pad->Divide(2,3);
  //  else if (nhist<9)  pad->Divide(2,4);
  else if (nhist<10) pad->Divide(3,3);
  else if (nhist<13) pad->Divide(3,4);
  else if (nhist<17) pad->Divide(4,4);
  else {cout<<"### got too many histos of "<<ColVars<<"... skipping!\n"<<endl; return npage;}

  TKey *key4a, *key4b;
  TH1F *obj1, *obj2;
  //minimum and maximum entry numbers of histo
  double min;  double max; 
  double min2; double max2;

  int j=0;
  while ( (key4a=(TKey*)dirOfHists1()) && (key4b=(TKey*)dirOfHists2()) ) { 
    obj1=(TH1F*)key4a->ReadObj();
    obj2=(TH1F*)key4b->ReadObj();
    hname=obj1->GetName();

    if( obj1->IsA()->InheritsFrom("TH1") && (isPartOf(hname,pars,nhist) || pars[0]=="JOKER") ) {      
      if (ipad==nhist && manyHists==true) {
	ipad=0;
	pad->cd(0); pad->Clear(); pad->SetFillColor(10);
	if (nhist<7) pad->Divide(2,3);
	else if(nhist<10) pad->Divide(3,3);
	else if (nhist<13) pad->Divide(3,4);
	else if (nhist<17) pad->Divide(4,4);
	npage++; 
      }
      cout<<"----------------------"<<hname<<endl;
      TLegend *leg = new TLegend(0.45,0.85,1,1); 
      char t1[100]; char t2[100];
      obj1->SetLineColor(kGreen); obj1->SetMarkerColor(kGreen);
      pad->cd(++ipad);
      sprintf(t1,"%s entries: %d",dir1.c_str(),obj1->GetEntries());
      leg->AddEntry(obj1,t1,"lp");

      max=obj1->GetMaximum();
      min=obj1->GetMinimum();
    
      hname=obj2->GetName();

      if(dir2!="") {  //plot 2 histos with proper range in y-axis
	max2=obj2->GetMaximum();
	min2=obj2->GetMinimum();
	if ( obj2->IsA()->InheritsFrom("TH1") && (isPartOf(hname,pars,nhist) || pars[0]=="JOKER") ) {
	  if ( max< max2 )  max=max2;
	  if ( min< min2 )  min=min2;
	  if (min>0) min=0;
	  if (!obj2->IsA()->InheritsFrom("TH2"))  //don't mess up the range of a 2D histo!!!!
	    obj1->GetYaxis()->SetRangeUser(1.2*min,1.1*max);
	  obj2->SetLineColor(kBlue); obj2->SetMarkerColor(kBlue);
	  obj1->Draw();
	  obj2->Draw("same");
	  sprintf(t2,"%s entries: %d",dir2.c_str(),obj2->GetEntries());
	  leg->AddEntry(obj2,t2,"lp");  
	  leg->Draw();
	  pad->Update();
	}
      }
      else { //only 1 hist to plot
	obj1->Draw();
	leg->Draw();
	pad->Update();
      }
    }
  } //end loop for all histos in both algos (key3)...
  
  return ++npage;   
}//end...DrawObjSupImposed()====================================================

bool isPartOf(string hname,vector<string> &hists,int nhists) 
{
  int flag=0;
  int n=0;
  while (flag==0) {
    if (n==nhists) return false;
    if (hname==hists[n]) flag=1;
    else if (n<nhists)  n++;
  }
  return true;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


int doExtendedMon(string tech, string folder, TFile* file, TPDF* pdf, int npage, string runNumber,bool doKey,TPad *pad)
{
  if (tech=="MDT") {
    if (folder=="Summary")
      npage=DrawMDTSummary(file,pdf,npage,runNumber,doKey,pad);
    else if (folder=="BA" || folder=="BC" || folder=="EA" || folder=="EC")
      cout<<">> Skipping!\n";
    else if (folder=="tmp")
      cout<<">> Skipping!\n";
  }
  if (tech=="RPC") {
    if (folder=="Chambers")
      if (doRPCch)
	;
      else cout<<">> Skipping!\n";
  }
  if (tech=="TGC") {
    if (folder=="Profile")
      npage=DrawTGCProfile(file,pdf,npage,runNumber,pad);
    else if (folder=="Efficiency")
      cout<<">> Skipping!\n";
  }
  if (tech=="CSC")
    cout<<">> Skipping!\n";

  if (tech=="MDTvsRPC") {
    if (folder=="Sectors")
      npage=DrawMDTvsRPCSectors(file,pdf,npage,runNumber,pad);
    else if (folder=="Chambers") {
      if (doMDTvsRPCch)
	;
      else cout<<">> Skipping!\n";
    }
  }
  if (tech=="RPCLV1") {
    if (folder=="SectorLogic")
      if (doSL)
	;
      else cout<<">> Skipping!\n";
  }
  if (tech=="TGCLV1") {
    if (folder=="Correlation")
      npage=DrawTGCLV1Correlation(file,pdf,npage,runNumber,pad);         
  }

  return npage;
}

////////////-------------------------------------------------------------/////////////
int DrawObj(TIter dirOfHists,int npage,int nhist,TPDF* pdf,TPad *pad,string tech, string folder)
{
  gStyle->SetOptStat(1111);//enable Statistics Info legend
  bool manyHists = false; //hack! when there are more than 16 hists in a folder they are displayed in more than 1 page...
  TKey *key;
  int jPad=0;  pad->cd(0);   
  if (npage>0) pad->Clear();
  pad->SetFillColor(10);
  
  if (nhist>16) {
    cout<<"Found "<<nhist<<" hists (>16).";
    manyHists=true;
    if (nhist<33) {
      nhist = ceil(nhist/2.);
      cout<<"Taking "<<nhist<<" per page...\n";
    }
    else {
      nhist = 16;
      cout<<"Taking "<<nhist<<" per page...\n";
    }
  }

  //divide canvas according to the number of hists per page:
  if (nhist<2)  pad->Divide(1,1);
  else if (nhist<3)  pad->Divide(1,2);
  else if (nhist<5)  pad->Divide(2,2);
  else if (nhist<7)  pad->Divide(2,3);
  else if (nhist<9)  pad->Divide(2,4);
  else if (nhist<10) pad->Divide(3,3);
  else if (nhist<13) pad->Divide(3,4);
  else if (nhist<17) pad->Divide(4,4);

  while(key = (TKey*)dirOfHists()) {
    TObject* obj1=key->ReadObj();
    if ( obj1->IsA()->InheritsFrom("TH1") ) {            
      cout<<"----------------------"<<obj1->GetName()<<endl;
      if (jPad==nhist && manyHists==true) {
	jPad=0;
	pad->cd(0); pad->Clear(); pad->SetFillColor(10);
	if (nhist<10) pad->Divide(3,3);
	else if (nhist<13) pad->Divide(3,4);
	else if (nhist<17) pad->Divide(4,4);
	npage++; 
      }
      pad->cd(++jPad);
      obj1->Draw();
      pad->Update();  
    }
  }
  npage++;
  return npage;
}
///////////////////////==================================================//////////////////////
int DrawMDTSummary(TFile* file,TPDF* sumPDF,int numPdf,std::string runNumber,bool doKey,TPad *pad) 
{
  // cout<<"in DrawMDTSummary()...\n";
  gStyle->SetOptStat(0000);//disable Statistics Info legend
  string dirEndCap = runNumber+"/Muon/MuonRawDataMonitoring/MDT/Summary/EndCap";
  string dirBarrel = runNumber+"/Muon/MuonRawDataMonitoring/MDT/Summary/Barrel";
  string dirHitOcc = runNumber+"/Muon/MuonRawDataMonitoring/MDT/Summary/HitOccupancies";
  TDirectory* dir[3];
  dir[0]=file->GetDirectory(dirEndCap.c_str());
  dir[1]=file->GetDirectory(dirBarrel.c_str());
  dir[2]=file->GetDirectory(dirHitOcc.c_str());
  
  std::string ecap[2]={"EndCap","Barrel"};    
  std::string layer[3]={"Inner","Middle","Outer"};
  
  for (int iecap=0;iecap<2;iecap++) {
    for(int iPage=0;iPage<2;iPage++) {                  
      pad->cd(0);
      if (numPdf>0) { pad->Update(); pad->Clear();}
      pad->SetFillColor(10);
      pad->Divide(8,3);

      cout<<"\t\t\t("<<ecap[iecap]<<")..."<<endl;
      
      for (int ilayer=0;ilayer<3;ilayer++) {  //inner,middle,outer
	for(int iPhi=1;iPhi<=8;iPhi++){        //sector phi
	  std::stringstream Phi_inttostring;
	  std::string Phi;
	  std::string hName;
	  int ipad=ilayer*8 + iPhi;	  
	  Phi_inttostring<<iPhi+iPage*8;
	  Phi_inttostring>>Phi;

	  hName="MDTHits_"+ecap[iecap]+"_"+layer[ilayer]+"_StPhi"+Phi;
	  TH1F *hist=(TH1F*)dir[iecap]->Get(hName.c_str());
	  pad->cd(ipad);
	  cout<<"\t\t\t--------"<<hist->GetName()<<endl;
	  hist->SetTitle();
	  hist->GetXaxis()->SetTitle(); 
	  hist->GetYaxis()->SetTitle();
	  hist->GetXaxis()->SetLabelSize(.11);
	  hist->Draw();
	  //	  pad->Update();
	}
      }
      pad->cd(0);
      string title = (iPage==0) ? ecap[iecap]+": MDTHits vs chamber #eta per layer // Sectors 1-8" : ecap[iecap]+": MDTHits vs chamber #eta per layer // Sectors 9-16";
      TPaveLabel* pageTitle=new TPaveLabel(0,0.96,1,1,title.c_str());   
      pageTitle->SetBorderSize(0);

      TPaveLabel* layerTitle[3];
      layerTitle[0]=new TPaveLabel( 0.98, 0.7, 1, 1., "Inner" );
      layerTitle[1]=new TPaveLabel( 0.98, 0.35, 1, 0.7, "Middle" );
      layerTitle[2]=new TPaveLabel( 0.98, 0, 1, 0.35, "Outer" );
      
      pageTitle->Draw(); 
      for (int i=0; i<3; i++) { layerTitle[i]->SetBorderSize(0); layerTitle[i]->SetTextAngle(90); layerTitle[i]->SetTextSize(0.12); layerTitle[i]->Draw(); }

      pad->Update();
      numPdf++;     
    }
  }
  /*  MOVED TO OVERVIWEW
  //getting number of hits per multilayer:
  string region[3]={"EndCapA","EndCapC","Barrel"};    
  int iec;
  pad->cd(0);
  pad->Clear();
  pad->SetFillColor(10);
  pad->Divide(3,3);

  int ipad=0;
  for (int ireg=0; ireg<3; ireg++) {
  for (int ilayer=0; ilayer<3; ilayer++) {
  string hName="NumerOfHitsIn"+region[ireg]+layer[ilayer]+"PerMultiLayer";
  if (ireg<2) iec=0; else iec=1;
  TH1F *hist=(TH1F*)dir[iec]->Get(hName.c_str());
  cout<<"\t\t\t--------"<<hist->GetName()<<endl;
  pad->cd(++ipad);
  hist -> Draw(); 
  pad->Update();
  }
  }
  numPdf++;
  */

  //getting hit occupancies
  cout<<"\t\t    @"<<dir[2]->GetName()<<endl;
  pad->cd(0);
  pad->Clear();
  pad->SetFillColor(10);
  pad->Divide(4,4);
  int ipad=0;
  gStyle->SetOptStat(1111);//enable Statistics Info legend

  for (int isec=1; isec<=16; isec++) {
    stringstream int2str;
    string secNum;
    int2str<<isec;
    int2str>>secNum;
    string hName="MDTHitsOccup_Sector"+secNum;
    TH1F *hist=(TH1F*)dir[2]->Get(hName.c_str());
    cout<<"\t\t\t--------"<<hist->GetName()<<endl;
    pad->cd(++ipad);
    hist -> Draw(); 
    pad->Update();
  }
  numPdf++;

  return numPdf;
}
/* ///////////////________________________________________________________\\\\\\\\\\\\\
   \\\\\\\\\\\\\\\						                                                    /////////////   */
int DrawMDTvsRPCSectors(TFile* file,TPDF* sumPDF,int numPdf,string runNumber,TPad *pad) 
{
  gStyle->SetOptStat(0000);//disable Statistics Info legend
  string item[3]={"HighPt", "LowPt", "Pivot"};

  for (int page=0; page<2; page++) { //drawing 8 sectors in each page...
    pad->cd(0); pad->Clear(); pad->SetFillColor(10); pad->Divide(8,3);
    
    for (int i=1; i<=8; i++) {
      int isec=i+page*8;
      stringstream int2str;  
      string sectorID;
      int2str<<isec;
      int2str>>sectorID;
      string dname=runNumber+"/Muon/MuonRawDataMonitoring/MDTvsRPC/Sectors/Sector"+sectorID;
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
    numPdf++;
  }

  return numPdf;
}

int DrawTGCProfile(TFile* file,TPDF* sumPDF,int numPdf,string runNumID,TPad *pad) 
{
  gStyle->SetOptStat(0000);//disable Statistics Info legend
  string dname=runNumID+"/Muon/MuonRawDataMonitoring/TGC/Profile";
  TDirectory *dir;
  dir=file->GetDirectory(dname.c_str());
  
  string side[2]={"A","C"};
  string element[2]={"Strip","Wire"};

  for (int iside=0; iside<2; iside++) {
    for (int ielement=0; ielement<2; ielement++) {
      pad->cd(0); 
      if (numPdf>0) pad->Clear(); 
      pad->SetFillColor(10);
      pad->Divide(7,2);
       
      for (int ilayer=1; ilayer<=7; ilayer++) {
	//	if (ilayer==2) continue;
	int jPad=ilayer;
	std::stringstream int2str;  
	std::string layer;
	int2str<<ilayer;
	int2str>>layer;	
	string hname=element[ielement]+"_Hit_Profile_"+side[iside]+"_Layer"+layer;
	if (! dir->FindKey(hname.c_str()) == 0 ) {
	  TH1F *hist=(TH1F*)dir->Get(hname.c_str());
	  cout<<"----------------------"<<hist->GetName()<<endl; 
	  hist->SetTitle();
	  hist->GetXaxis()->SetTitle(); 
	  hist->GetYaxis()->SetTitle();	
	  pad->cd(jPad);       hist->Draw(); //  pad->Update();
	}
	else cout<<"**NOT FOUND:  "<<hname<<"\n";

	string hname2=element[ielement]+"_Occupancy_"+side[iside]+"_Layer"+layer;
	if (! dir->FindKey(hname2.c_str()) == 0 ) {
	  TH1F *hist2=(TH1F*)dir->Get(hname2.c_str());
	  cout<<"----------------------"<<hist2->GetName()<<endl; 
	  hist2->SetTitle();
	  hist2->GetXaxis()->SetTitle(); 
	  hist2->GetYaxis()->SetTitle();	
	  pad->cd(jPad+7);     hist2->Draw(); // pad->Update();
	}
	else cout<<"**NOT FOUND:  "<<hname2<<"\n";
	
      }// looop layers

      pad->cd(0);
      string title = element[ielement]+"s Side "+side[iside]+" // Layers 1-7";
      TPaveLabel* pageTitle=new TPaveLabel(0,0.96,1,1,title.c_str());   
      pageTitle->SetBorderSize(0);
      
      TPaveLabel* itemTitle[2];
      itemTitle[0]=new TPaveLabel( 0.98, 0.5, 1, 1., "Hit Profile" );
      itemTitle[1]=new TPaveLabel( 0.98, 0., 1, 0.5, "Occupancy" );
      
      pageTitle->Draw(); 
      for (int i=0; i<2; i++) { itemTitle[i]->SetBorderSize(0); itemTitle[i]->SetTextAngle(90); itemTitle[i]->SetTextSize(0.09); itemTitle[i]->Draw(); }

      pad->Update();
      numPdf++;
    } //loop elements
  } //loop side

  pad->cd(0); pad->Clear(); pad->SetFillColor(10);
  pad->Divide(2,2);     
  gStyle->SetOptStat(1111);//enable Statistics Info legend    

  int jPad=0;

  for (int ielement=0; ielement<2; ielement++) {       
    for (int iside=0; iside<2; iside++) {
      string hname=element[ielement]+"_Profile_Per_Chamber_"+side[iside];
      cout<<"----------------------"<<hist->GetName()<<endl; 
      TH1F *hist=(TH1F*)dir->Get(hname.c_str());
      pad->cd(++jPad);       hist->Draw();   pad->Update();
    }    
  }  
  numPdf++;
  return numPdf;
}

int DrawTGCLV1Correlation(TFile* file,TPDF* sumPDF,int numPdf,string runNumID,TPad *pad) 
{
  gStyle->SetOptStat(0000);//disable Statistics Info legend
  string dname=runNumID+"/Muon/MuonRawDataMonitoring/TGCLV1/Correlation";
  TDirectory *dir;
  dir=file->GetDirectory(dname.c_str());

  string side[2]={"A","C"};
  string element[2]={"Wire","Strip"};
  string measurement[2]={"Eta","Phi"};

  for (int iside=0; iside<2; iside++) {
    for (int isector=1; isector<=12; isector++) {

      std::stringstream int2str;  
      std::string sector;
      if (isector<10) int2str<<"0"<<isector;	
      else int2str<<isector;
      int2str>>sector;

      pad->cd(0); 
      if (numPdf>0) pad->Clear(); 
      pad->SetFillColor(10);
      pad->Divide(7,2);    
      
      int jPad=0;	

      for (int ilayer=1; ilayer<=7; ilayer++) {      
	std::stringstream int2str;  
	std::string layer;
	int2str<<ilayer;
	int2str>>layer;

	for (int ielement=0; ielement<2; ielement++) {
	  string hname="RoI_"+measurement[ielement]+"_Vs_"+element[ielement]+"_Hit_"+measurement[ielement]+"_"+side[iside]+"_Side"+sector+"_Layer"+layer;
	  if (! dir->FindKey(hname.c_str()) == 0 ) {
	    TH1F *hist=(TH1F*)dir->Get(hname.c_str());
	    hist->SetTitle();
	    hist->GetXaxis()->SetTitle(); 
	    hist->GetYaxis()->SetTitle();	
	    jPad=ielement*7+ilayer;
	    pad->cd(jPad);  hist->Draw();  // pad->Update();
	    cout<<"----------------------"<<hist->GetName()<<endl; 
	  } 
	  else cout<<"---------------------- "<<hist->GetName()<<" *MISSING!*\n";	  
	}// loop layers	
      } //loop elements

      pad->cd(0);
      string title = "TGCLV1 Correlation: Sector "+sector+", Side "+side[iside]+" // Layers 1-7";
      TPaveLabel* pageTitle=new TPaveLabel(0,0.96,1,1,title.c_str());   
      pageTitle->SetBorderSize(0);
      
      TPaveLabel* itemTitle[2];
      itemTitle[0]=new TPaveLabel( 0.98, 0.5, 1, 1., "RoI #eta vs Wire hit #eta" );
      itemTitle[1]=new TPaveLabel( 0.98, 0., 1, 0.5, "RoI #phi vs Strip hit #phi" );
      
      pageTitle->Draw(); 
      for (int i=0; i<2; i++) { itemTitle[i]->SetBorderSize(0); itemTitle[i]->SetTextAngle(90); itemTitle[i]->SetTextSize(0.09); itemTitle[i]->Draw(); }

      pad->Update();
      numPdf++;
    } //loop sector
  } //loop side
  
  pad->cd(0); pad->Clear(); pad->SetFillColor(10);
  pad->Divide(1,2);     
  gStyle->SetOptStat(1111);//enable Statistics Info legend
  int jPad=0;
  
  for (int ielement=0; ielement<2; ielement++) {       

    string hname="RoI_"+measurement[ielement]+"_Vs_"+element[ielement]+"_Hit_"+measurement[ielement];
    TH1F *hist=(TH1F*)dir->Get(hname.c_str());
    cout<<"----------------------"<<hist->GetName()<<endl; 
    pad->cd(++jPad);       hist->Draw();   pad->Update();
  }    
  
  numPdf++;
  return numPdf;
}


bool isExtendedMon(string dirname) 
{
  int n=0;
  while ( n < sizeof(extendedMon)/sizeof(extendedMon[0]) ) {
    if (extendedMon[n]==dirname)
      return true;
    else n++;
  }
  return false;
}
