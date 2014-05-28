/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RawMuonHistPlot.cc
// Authors:  J. Love (Boston University)  -  N. Benekos(Illinois) 
// Jan. 2008
//
// DESCRIPTION:
// ROOT script for automatic generation seperate pdf files for MDT,RPC,RPCLV1... so on MuonRawDataMonitoring plots
// The macro makes MDTxxxx.pdf where xxxx is runNumber.  
// HOWTO:  root -l 'RawMuonHistPlot.cc("filename","runNumber",true)'
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

int DrawObj(TIter dirOfHists,int npage,int nhist,TPDF *pdf,TPad *pad);

int DrawSummary(TFile* file,TPDF* sumPDF,int numPdf,std::string runNumID="1",bool doKey=true);

void RawMuonHistPlot(char * fName="",std::string runNumber="1",bool doChambers=false,bool doMDTSummaryLegend=true)
{

  TFile* f=new TFile(fName,"READ");
  const char* Overview="Overview";
  const char* Summary="Summary";
  const char* Chambers="Chambers";
  const char* Sectors="Sectors";
  std::string pdfTag=runNumber+".pdf";
  TString mDir = "run_"+runNumber+"/Muon/MuonRawDataMonitoring";
  TCanvas *canv=new TCanvas("canv","Spectrometer Hists",0,0,850,650);
  const int nPDFs=5;
  TPDF* pdf[nPDFs];
  int loop=0;  
  int npage;
 
  f->cd(mDir);
  //  std:: cout << "GDIRECTORY path: " << gDirectory->ls() << endl;
  TDirectory* currentDir = gDirectory;
  gDirectory=f->GetDirectory(mDir);
  TIter nextcd1(gDirectory->GetListOfKeys());
  TKey *key1;
    
  while(key1 = (TKey*)nextcd1()) {//While in MDT/RPC/MDTvsRPC/RPCLV1/TGC
    canv->Clear();
    TDirectory *dir1= (TDirectory*)key1->ReadObj();
    TIter nextcd2(dir1->GetListOfKeys());
    TKey *key2;

    //start new pdf
    stringstream pdfName;
    pdfName<<dir1->GetName()<<pdfTag;
    if (loop>nPDFs-1){
      std::cout<<"There are more than "<<nPDFs<<" Directories please increase the number of PDF's"<<endl;
      return 0;
    }
    pdf[loop]=new TPDF(pdfName.str().c_str());
    cout<<"_____________________________________"<<endl;
    cout<<"\n working in "<<dir1->GetName()<<endl;
    cout<<"_____________________________________"<<endl; 

    npage=0;
    
    while(key2 = (TKey*)nextcd2()) { //Chambers/Sectors/Overview/Summary
 
      TDirectory *dir2= (TDirectory*)key2->ReadObj();
      cout<<"\t@ "<<dir2->GetName()<<endl;
      TIter nextcd3(dir2->GetListOfKeys());
      int nhist=0;

      if(strncmp(dir2->GetName(),Overview,5)==0) {
	//count histograms...
	while(key = (TKey*)nextcd3()) {
	  TObject* obj=key->ReadObj();
	  if ( obj->IsA()->InheritsFrom( "TH1" ) ) nhist++;
	}
	nextcd3.Reset();
	
	npage=DrawObj(nextcd3,npage,nhist,pdf[loop],(TPad*)canv);
      }
     
      else if(strncmp(dir2->GetName(),Summary,5)==0) {
	npage=DrawSummary(f,pdf[loop],npage,runNumber,doMDTSummaryLegend,(TPad*) canv);
      }

      else if(strncmp(dir2->GetName(),Chambers,5)==0) {
	if(!doChambers){ cout<<"\t\t\tSkip!\n"; continue; }
	TKey *key3;
	while(key3 = (TKey*)nextcd3()){
	  TDirectory *dir3= (TDirectory*)key3->ReadObj();
	  TIter nextcd4(dir3->GetListOfKeys());
	  npage=DrawObj(nextcd4,npage,pdf[loop],(TPad*)canv);
	}
      }
      
      else if(strncmp(dir2->GetName(),Sectors,5)==0){
	TKey *key3;
	if (strncmp(dir1->GetName(),"RPC",5)==0) {
	  
	  while( key3 = (TKey*)nextcd3() ){ //while in SectorX,SectorY,...
	    TDirectory *dir3= (TDirectory*)key3->ReadObj();
	    TIter nextcd4(dir3->GetListOfKeys());
	    TKey *key4;
	    while(key4 = (TKey*)nextcd4()){ //in PhivsEta..
	      TDirectory *dir4= (TDirectory*)key4->ReadObj();
	      TIter nextcd5(dir4->GetListOfKeys());
	      TKey *key5;
	      nhist=0;
	      //count histograms...
	      while(key5 = (TKey*)nextcd5()) {
		TObject* obj=key5->ReadObj();
		if ( obj->IsA()->InheritsFrom( "TH1" ) ) nhist++; 
	      }
	      nextcd5.Reset();
	            
	      npage=DrawObj(nextcd5,npage,nhist,pdf[loop],(TPad*)canv);
	    }
	  }
	  
	}
	else {
	  while( key3 = (TKey*)nextcd3() ){ //while in SectorX,SectorY,...
	    TDirectory *dir3= (TDirectory*)key3->ReadObj();
	    TIter nextcd4(dir3->GetListOfKeys());
	    TKey *key4;
	    nhist=0;
	    //count histograms...
	    while(key4 = (TKey*)nextcd4()) {
	      TObject* obj=key4->ReadObj();
	      if ( obj->IsA()->InheritsFrom( "TH1" ) ) nhist++;
	    }
	    nextcd4.Reset();
	    
	    npage=DrawObj(nextcd4,npage,nhist,pdf[loop],(TPad*)canv);
	  }
	}
	
      }
      
      else {
	cout<<"\t\t\tSkip!\n";//<<dir1->GetName()<<"/"<<dir2->GetName()<<" structure not accounted for"<<endl;
      }
    }
    pdf[loop]->Close();
    loop++;
  }//While key1
  f->Close();
  return;    
}//RawMuonHistPlot()

int DrawObj(TIter dirOfHists,int npage,int nhist,TPDF* pdf,TPad *pad){

  TKey *key;
  int jPad=0;
  pad->cd(0); 
 
  if (npage>0) pad->Clear();
  pad->SetFillColor(10);
   
  if      (nhist<2)  pad->cd(0);
  else if (nhist<3)  pad->Divide(1,2);
  else if (nhist<5)  pad->Divide(2,2);
  else if (nhist<7)  pad->Divide(2,3);
  else if (nhist<9)  pad->Divide(2,4);
  else if (nhist<10) pad->Divide(3,3);
  else if (nhist<13) pad->Divide(3,4);
  else if (nhist<17) pad->Divide(4,4);
  else {   cout<<"IN DRAWOBJ(): too many hists "<<nhist<<" skipping"<<endl;  return npage;  }
  
  while(key = (TKey*)dirOfHists()) {
    TObject* obj1=key->ReadObj();
    if ( obj1->IsA()->InheritsFrom( "TH1" ) ) {
      cout<<"----------------------"<<obj1->GetName()<<endl;
      pad->cd(++jPad);
      obj1->Draw();
      pad->Update();  
    }
  }
  npage++;
  return npage;
}

int DrawSummary(TFile* file,TPDF* sumPDF,int numPdf,std::string runNumID,bool doKey,TPad *pad) {

  TDirectory* dir[2];
  string dirEndCap = "run_"+runNumID+"/Muon/MuonRawDataMonitoring/MDT/Summary/EndCap";
  string dirBarrel = "run_"+runNumID+"/Muon/MuonRawDataMonitoring/MDT/Summary/Barrel";
  dir[0]=file->GetDirectory(dirEndCap.c_str());
  dir[1]=file->GetDirectory(dirBarrel.c_str());
  TPaveLabel* title;
 
  std::string ecap[2]={"EndCap","Barrel"};    
  std::string layer[3]={"Inner","Middle","Outer"};
  std::string phiLabel[16]={"#Phi=1","#Phi=2","#Phi=3","#Phi=4",
			    "#Phi=5","#Phi=6","#Phi=7","#Phi=8",
			    "#Phi=9","#Phi=10","#Phi=11","#Phi=12",
			    "#Phi=13","#Phi=14","#Phi=15","#Phi=16"};
  std::string FullLayer[3]={"Inner","Middle","Outer"};
  
  for (int iecap=0;iecap<2;iecap++) {
    for(int iPad=0;iPad<2;iPad++) {
      std::stringstream inttostring1;
      std::stringstream inttostring2;
      std::stringstream inttostring3;
      std::stringstream inttostring4;
      std::string intisstring1;
      std::string intisstring2;
      std::string canName;
      std::string strngTitle;
      std::string pTitle;
      
      inttostring1<<iecap+iPad+1+((iecap==1)?1:0);
      inttostring1>>intisstring1;
      canName= "c"+intisstring1;
      
    }
  }  
  /*  
  //Draw a key to the histograms.
  if(doKey){
    TPaveLabel* keyLabel=new TPaveLabel(0,0.95,1,1,"Run Number MDT Location & Phi Range");
    TPaveLabel* keyDesLayer[3];
    TPaveLabel* keyDesX=new TPaveLabel(0,0,1,0.05,"#Phi = 1...8 or 9...16");
    TPaveLabel* keyDesPlot=new TPaveLabel(0.11,0.06,1,0.94,
					  "Number of Hits vs. Chamber #eta");
    TText* keyText=new TText(0.251773,0.419614,
			     "With A-Side Positive and C-Side Negative");
    keyDesLayer[0]=new TPaveLabel(0,0.65,0.1,0.94,"Inner");
    keyDesLayer[1]=new TPaveLabel(0,0.35,0.1,0.64,"Middle");
    keyDesLayer[2]=new TPaveLabel(0,0.06,0.1,0.34,"Outer");
      key->cd(0);
    keyLabel->SetFillColor(10);
    keyDesX->SetFillColor(10);
    keyDesPlot->SetFillColor(10);
    keyLabel->Draw();
    keyDesX->Draw();
    keyDesPlot->Draw();
    keyText->Draw();
    
    for(int l=0;l<3;l++){
      keyDesLayer[l]->SetFillColor(10);
      keyDesLayer[l]->Draw();
    }
    key->Update();
  }
  else
    key->Close();
  */
  for (int iecap=0;iecap<2;iecap++) {
    for(int iPad=0;iPad<2;iPad++) {
            
      std::stringstream inttostring1;
      std::stringstream inttostring2;
      std::stringstream inttostring3;
      std::stringstream inttostring4;
      std::string intisstring1;
      std::string intisstring2;
      std::string canName;
      std::string strngTitle;
      std::string pTitle;
      
      inttostring1<<iecap+iPad+1+((iecap==1)?1:0);
      inttostring1>>intisstring1;
      canName= "c"+intisstring1;
      
      inttostring2<<((iPad==1)?16:8);
      inttostring2>>intisstring2;
      inttostring3<<((iPad==1)?9:1);
      inttostring3>>intisstring1;
      strngTitle="Run "+runNumID+" MDT "+ecap[iecap]+" Chambers Phi="+intisstring1+"-"+intisstring2;
               
      title = new TPaveLabel(0,0.95,1,1,strngTitle.c_str());
      title->SetFillColor(10);
      title->Draw();
      pad->Update();
      
      inttostring4<<iecap+iPad+1+((iecap==1)?1:0);
      inttostring4>>intisstring1;
      pTitle="pad"+intisstring1;
      
      pad->cd(0);
      if (numPdf>0) pad->Clear();
      pad->SetFillColor(10);
      pad->Divide(8,3);
      
      for (int ilayer=0;ilayer<3;ilayer++) {
	for(int iPhi=0;iPhi<8;iPhi++){
	  std::stringstream Phi_inttostring;
	  std::string Phi;
	  std::string m_title;
	  int padNumber=1+iPhi+ilayer*8;
	  
	  Phi_inttostring<<iPhi+1+iPad*8;
	  Phi_inttostring>>Phi;
	  m_title="MDTHits_"+ecap[iecap]+"_"+layer[ilayer]+"_StPhi"+Phi;
	  TH1F *hist=(TH1F*)dir[iecap]->Get(m_title.c_str());
	  pad->cd(padNumber);
	  hist->Draw();

	  hist->GetXaxis()->SetLabelSize(.105);
	  
	  if(iPhi==0){
	    hist->GetYaxis()->SetTitleOffset(0.42);
	    hist->GetYaxis()->SetTitleSize(0.12);
	    hist->GetYaxis()->SetTitle(FullLayer[ilayer].c_str());
	  }
	  if(ilayer==2){
	    hist->GetXaxis()->SetTitleOffset(0.75);
	    hist->GetXaxis()->SetTitleSize(0.105);
	    hist->GetXaxis()->SetTitle(phiLabel[iPhi+iPad*8].c_str());
	  }
	  pad->Update();
	}
      }
     
    }
  }

  numPdf++;
  return numPdf;
}
