/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HistogramDataCOOL.cxx,v 1.9 2009-02-13 12:32:11 ponyisi Exp $
// Last Modified 07/18/2008 by M.D'Onofrio 
// **********************************************************************

#include "DataQualityUtils/HistogramDataCOOL.h"

//CORAL API include files
#include "CoralBase/Attribute.h"

//COOL API include files (CoolKernel)
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/ConstRecordAdapter.h"

#include <sstream>
#include <fstream>
#include <string>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TIterator.h>
#include <TKey.h>
#include <TLegend.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TMath.h>
#include <TColor.h>
#include <TTree.h>

ClassImp(dqutils::HistogramDataCOOL)

namespace dqutils{

cool::IDatabasePtr 
HistogramDataCOOL::
coolDbInstance(std::string dbStr, bool readOnly) {
    try {
        std::cout << "Opening database '" << dbStr << "'...";
        cool::IDatabaseSvc& dbSvc = this->databaseService();
        std::cout << "done." << std::endl;
        return dbSvc.openDatabase(dbStr.c_str(), readOnly);
    }
    catch (cool::DatabaseDoesNotExist&) {
        std::cout << "Error! Database does not exist!" << std::endl;
        throw;
    }
}

cool::IFolderPtr 
HistogramDataCOOL::
coolFolderInstance(std::string folderStr) {
    try {
        cool::IFolderPtr folder = m_coolDb->getFolder(folderStr.c_str());
        std::cout << "Browsing objects of '" << folderStr << "'" << std::endl;
        return folder;
    }
    catch (cool::FolderNotFound& ) {
        std::cout << "Error! Folder '" << folderStr << "' does not exist!" << std::endl;
        throw;
    }
}

void 
HistogramDataCOOL::
setSince(cool::Int64 run, cool::Int64 lumi) {
    m_since = ((run << 32) + lumi);
}

void 
HistogramDataCOOL::
setUntil(cool::Int64 run, cool::Int64 lumi) {
    m_until = ((run << 32) + lumi);
}

void
HistogramDataCOOL::
setIOV(cool::Int64 runS, cool::Int64 lumiS, cool::Int64 runU, cool::Int64 lumiU) {
    this->setSince(runS, lumiS);
    this->setUntil(runU, lumiU);
    this->printIOV();
}

void
HistogramDataCOOL::
setIOV(cool::Int64 run) {
    this->setSince(run, 0);
    this->setUntil(run, cool::UInt32Max);
    //this->printIOV();
}


void
HistogramDataCOOL::
printIOV(){
    cool::Int64 runS=m_since>>32;
    cool::Int64 lumiS=m_since-(runS<<32);
    cool::Int64 runU=m_until>>32;
    cool::Int64 lumiU=m_until-(runU<<32);
    std::cout << "Using IOVrange [(" << runS << "," << lumiS << "),("  << runU << "," << lumiU << ")] " << "[" << m_since << "," << m_until << "]" << std::endl;
}

HistogramDataCOOL::
HistogramDataCOOL (std::string dbStr, std::string folderStr, int runS, int lumiS, int runU, int lumiU) {
    m_coolDb = this->coolDbInstance(dbStr, false);
    m_coolFolder = this->coolFolderInstance(folderStr);
    m_coolFolderH = this->coolFolderInstance(folderStr);
    this->setIOV(runS, lumiS, runU, lumiU);
}

HistogramDataCOOL::
HistogramDataCOOL (int runS, int lumiS, int runU, int lumiU) {
    m_coolDb = this->coolDbInstance("COOLOFL_GLOBAL/OFLP200", false);
    m_coolFolder = this->coolFolderInstance("/GLOBAL/DETSTATUS/DQMFOFL");
    m_coolFolderH = this->coolFolderInstance("/GLOBAL/DETSTATUS/DQMFOFLH");
    this->setIOV(runS, lumiS, runU, lumiU);
}

HistogramDataCOOL::
HistogramDataCOOL() {
    m_coolDb = this->coolDbInstance("COOLOFL_GLOBAL/OFLP200", false);
    m_coolFolder = this->coolFolderInstance("/GLOBAL/DETSTATUS/DQMFOFL");
    m_coolFolderH = this->coolFolderInstance("/GLOBAL/DETSTATUS/DQMFOFLH");
    this->setIOV(0, 0, 0, 0);
}

HistogramDataCOOL::
~HistogramDataCOOL () {
    m_coolDb->closeDatabase();
    std::cout << "Cleared!" << std::endl;
}

cool::RecordSpecification 
HistogramDataCOOL::
createSpec() {
  //std::cout << "Preparing RecordSpecification" << std::endl;
    cool::RecordSpecification spec;
    spec.extend("Code",cool::StorageType::Int32);
    spec.extend("deadFrac",cool::StorageType::Float);
    spec.extend("Thrust",cool::StorageType::Float);
    if (!(spec==m_coolFolder->payloadSpecification())) {
        std::cout << "ERROR Source and destination folder specifications differ." << std::endl;
    }
    return spec;
}

coral::AttributeList 
HistogramDataCOOL::
createPayload(int colourCode, float dfrac, float thrust, const cool::RecordSpecification& spec) {
    coral::AttributeList payload = cool::Record( spec ).attributeList();
    payload["Code"].data<cool::Int32>() = colourCode;
    payload["deadFrac"].data<cool::Float>() = dfrac;
    payload["Thrust"].data<cool::Float>() = thrust;
    //std::cout << "Creating payload: ";
    //std::cout << "[Code : " << colourCode << "],";
    //std::cout << "[DeadFraction : " << dfrac << "],";
    //std::cout << "[Thrust : " << thrust << "]" << std::endl;
    return payload;
}

  // Monica 4/6/2008, modified 24/6/2008 to allocate more quantities 
cool::RecordSpecification
HistogramDataCOOL::
createSpecH() {
    cool::RecordSpecification specH;
    specH.extend("Code",cool::StorageType::Int32);
    specH.extend("Algo",cool::StorageType::String255);
    specH.extend("Entries",cool::StorageType::Int32);
    specH.extend("Par1",cool::StorageType::Float);
    specH.extend("Par2",cool::StorageType::Float);
    specH.extend("Par3",cool::StorageType::Float);
    specH.extend("Par4",cool::StorageType::Float);
    specH.extend("Par5",cool::StorageType::Float);

    if (!(specH==m_coolFolderH->payloadSpecification())) {
      std::cout << "ERROR Source and destination folder specifications differ. histos" << std::endl;
    }
    return specH;
}

coral::AttributeList 
HistogramDataCOOL::
createPayloadH(int colourCode, std::string algo, int entries, float par1, float par2, float par3, float par4, float par5, const cool::RecordSpecification& specH) {
    coral::AttributeList payloadH = cool::Record( specH ).attributeList();
    payloadH["Code"].data<cool::Int32>() = colourCode;
    payloadH["Algo"].data<cool::String255>() = algo;
    payloadH["Entries"].data<cool::Int32>() = entries;
    payloadH["Par1"].data<cool::Float>() = par1;
    payloadH["Par2"].data<cool::Float>() = par2;
    payloadH["Par3"].data<cool::Float>() = par3;
    payloadH["Par4"].data<cool::Float>() = par4;
    payloadH["Par5"].data<cool::Float>() = par5;
    std::cout << "Creating payload histos: ";
    std::cout << "[Code : " << colourCode << "],";
    std::cout << "[Algo : " << algo << "],";
    std::cout << "[Entries : " << entries << "],";
    std::cout << "[Par1 : " << par1 << "],";
    std::cout << "[Par2 : " << par2 << "],";
    std::cout << "[Par3 : " << par3 << "],";
    std::cout << "[Par4 : " << par4 << "],";
    std::cout << "[Par5 : " << par5 << "]" << std::endl;
    return payloadH;
}
  // end of modification in create new folder and payload (Monica 4/6/2008)

void 
HistogramDataCOOL::
dump(cool::ChannelSelection selection, std::string tag_name) {      
    try {
      //        cool::IObjectIteratorPtr objects = m_coolFolder->browseObjects(m_since, m_until, selection,"DetStatusDQMFOFLH-FDR2-01");
        cool::IObjectIteratorPtr objects = m_coolFolder->browseObjects(m_since, m_until, selection,tag_name);
        while (objects->goToNext()) {
            const cool::IObject& element = objects->currentRef();
            std::cout << element << std::endl;
        }
    }
    catch (cool::Exception& e) {
        std::cout << "Unknown exception caught!" << e.what() << std::endl;
    }
}

int 
HistogramDataCOOL::
dumpCode(std::string channelName, std::string tag_name) {
    std::string result = this->dumpHisto(this->getCoolFolderH()->channelId(channelName.c_str()), "Code", tag_name);
    return atoi(result.c_str());
}

void 
HistogramDataCOOL::
dumpall(std::string tag_name) {      
  this->dump(cool::ChannelSelection::all(), tag_name);
}

  // Monica 4/6/2208
std::string 
HistogramDataCOOL::
dumpHisto(cool::ChannelId channelId, std::string field, std::string tag_name) {      
    std::string result ="";
    try {
        cool::ChannelSelection selection = cool::ChannelSelection(channelId);
	//        cool::IObjectIteratorPtr objects = m_coolFolderH->browseObjects(m_since, m_until, selection,"DetStatusDQMFOFLH-FDR2-01");
        cool::IObjectIteratorPtr objects = m_coolFolderH->browseObjects(m_since, m_until, selection,tag_name);
        while (objects->goToNext()) {
            const cool::IObject& element = objects->currentRef();
            result = element.payloadValue(field);
        }
    }
    catch (cool::Exception& e) {
        std::cout << "Unknown exception caught!" << e.what() << std::endl;
    }
    return result;
}

void 
HistogramDataCOOL::
insertH(cool::ChannelId channelId, int code, std::string algo, int entries, float par1, float par2, float par3, float par4, float par5, std::string tag_name) {
    try {
      cool::RecordSpecification specH = this->createSpecH();
      coral::AttributeList payloadH = this->createPayloadH(code, algo, entries, par1, par2, par3, par4, par5, specH);	
      std::cout << "Trying to store payload histos [channel " << this->getCoolFolderH()->channelName(channelId)  <<" ("<< channelId  <<")]...";
      //      m_coolFolderH->storeObject(m_since, m_until, payloadH,  channelId, "DetStatusDQMFOFLH-FDR2-01", true);
      m_coolFolderH->storeObject(m_since, m_until, cool::Record(m_coolFolder->payloadSpecification(), payloadH),  channelId, tag_name, true);
      std::cout << "stored!" << std::endl;       	
    }
    catch (cool::Exception& e) {
        std::cout << "Unknown exception caught!" << e.what() << std::endl;
    }
}

void 
HistogramDataCOOL::
insertH(std::string channelName, int code, std::string algo, int entries, float par1, float par2, float par3, float par4, float par5, std::string tag_name) {
    try {
      this->insertH(this->getCoolFolderH()->channelId(channelName), code, algo, entries, par1, par2, par3, par4, par5, tag_name);
    }
    catch (cool::Exception& e) {
        std::cout << "Unknown exception caught!" << e.what() << std::endl;
    }
}

cool::IFolderPtr 
HistogramDataCOOL::
getCoolFolderH() {
    return this->m_coolFolderH;
}
  
void
HistogramDataCOOL::
ntupleDB( int HistoId, std::string nameHisto, std::string configuration, int Irun, int Frun)
{

  gROOT->Reset();
  gROOT->SetBatch();
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(1);
  gStyle->SetStatColor(0);
  gStyle->SetFillColor(1);
  gStyle->SetPalette(1,0);
  gStyle->SetTitleFontSize(0.06);
  gStyle->SetTitleH(0.06);

  // Declaration of leave types
  UInt_t          RunSince;
  UInt_t          RunUntil;
  UInt_t          LBSince;
  UInt_t          LBUntil;
  UInt_t           ChannelID;
  Char_t          TagID[26];
  Int_t           Code;
  Char_t          Algo[100];
  Int_t           Entries;
  Float_t         Par1;
  Float_t         Par2;
  Float_t         Par3;
  Float_t         Par4;
  Float_t         Par5;

  // List of branches
  TBranch        *b_RunSince;   
  TBranch        *b_RunUntil;   
  TBranch        *b_LBSince;   
  TBranch        *b_LBUntil;   
  TBranch        *b_ChannelID;   
  TBranch        *b_TagID;   
  TBranch        *b_Code;   
  TBranch        *b_Algo;   
  TBranch        *b_Entries;   
  TBranch        *b_Par1;   
  TBranch        *b_Par2;   
  TBranch        *b_Par3;   
  TBranch        *b_Par4;   
  TBranch        *b_Par5;   

  unsigned int i = HistoId;
  unsigned int init_run = Irun;
  unsigned int last_run = Frun;
  std::string nameh = nameHisto;
  std::string config = configuration;

  char f_name[200];
  char dir_name[200];
  sprintf(f_name,"dqmfoflh_%s.root",config.c_str()); 
  sprintf(dir_name,"dqmfoflh_%s.root:/COOL/GLOBAL/DETSTATUS",config.c_str()); 
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(f_name);
  if (!f) {
    f = new TFile(f_name);
    f->cd(dir_name);
  }
  TTree *tree = (TTree*)gDirectory->Get("DQMFOFLH");

  if (!tree) return;

  tree->SetBranchAddress("RunSince", &RunSince, &b_RunSince);
  tree->SetBranchAddress("RunUntil", &RunUntil, &b_RunUntil);
  tree->SetBranchAddress("LBSince", &LBSince, &b_LBSince);
  tree->SetBranchAddress("LBUntil", &LBUntil, &b_LBUntil);
  tree->SetBranchAddress("ChannelID", &ChannelID, &b_ChannelID);
  tree->SetBranchAddress("TagID", TagID, &b_TagID);
  tree->SetBranchAddress("Code", &Code, &b_Code);
  tree->SetBranchAddress("Algo", Algo, &b_Algo);
  tree->SetBranchAddress("Entries", &Entries, &b_Entries);
  tree->SetBranchAddress("Par1", &Par1, &b_Par1);
  tree->SetBranchAddress("Par2", &Par2, &b_Par2);
  tree->SetBranchAddress("Par3", &Par3, &b_Par3);
  tree->SetBranchAddress("Par4", &Par4, &b_Par4);
  tree->SetBranchAddress("Par5", &Par5, &b_Par5);

  TGraphErrors *entr_g = new TGraphErrors();
  TGraphErrors *par1_g[4];
  TGraphErrors *par2_g[4];
  TGraphErrors *par3_g[4];
  TGraphErrors *par4_g[4];
  TGraphErrors *par5_g[4];

  for (int k=0;k<4;k++) {
   par1_g[k] = new TGraphErrors();
   par2_g[k] = new TGraphErrors();
   par3_g[k] = new TGraphErrors();
   par4_g[k] = new TGraphErrors();
   par5_g[k] = new TGraphErrors();
  }

  std::string Algorithm;

  Int_t nentries = tree->GetEntries();

  if (nentries<1) {
    std::cout << " The TTree is empty! Check if the right Folder DB Tag is used" << std::endl;
    return;
  }

  for (Int_t jj=0; jj<nentries; jj++) {
    tree->GetEntry(jj);

    if (i==ChannelID && ((RunSince>=init_run) && (RunSince<=last_run))) { 
      Algorithm=Algo;
      if (Algorithm=="BinContentComp" && Par5!=0.) {
	int npoints = 0;

	// N Entries
	npoints=entr_g->GetN();
	npoints++;
	entr_g->Set(npoints);
	entr_g->SetPoint(npoints-1,RunSince,Entries);
	entr_g->SetPointError(npoints-1,0.,sqrt(Entries));
	
	// Par 1 
	int npoints1=0; int npoints2=0;  int npoints3=0; int npoints4=0; int npoints5=0;
	npoints1=0; 	npoints2=0; 	npoints3=0; 	npoints4=0; 	npoints5=0;
	for (int k=0;k<4;k++) npoints1=par1_g[k]->GetN();
	for (int k=0;k<4;k++) npoints2=par2_g[k]->GetN();
	for (int k=0;k<4;k++) npoints3=par3_g[k]->GetN();
	for (int k=0;k<4;k++) npoints4=par4_g[k]->GetN();
	for (int k=0;k<4;k++) npoints5=par5_g[k]->GetN();
	npoints1++;
	npoints2++;
	npoints3++;
	npoints4++;
	npoints5++;
	for (int k=0;k<4;k++) {
	  par1_g[k]->Set(npoints1);
	  par2_g[k]->Set(npoints2);
	  par3_g[k]->Set(npoints3);
	  par4_g[k]->Set(npoints4);
	  par5_g[k]->Set(npoints5);
	}
	par1_g[0]->SetPoint(npoints1-1,RunSince,Par1);
	par2_g[0]->SetPoint(npoints2-1,RunSince,Par2);
	par3_g[0]->SetPoint(npoints3-1,RunSince,Par3);
	par4_g[0]->SetPoint(npoints4-1,RunSince,Par4);
	par5_g[0]->SetPoint(npoints5-1,RunSince,Par5);
	if (Code==1) {
	  par1_g[1]->SetPoint(npoints1-1,RunSince,Par1);
	  par2_g[1]->SetPoint(npoints2-1,RunSince,Par2);
	  par3_g[1]->SetPoint(npoints3-1,RunSince,Par3);
	  par4_g[1]->SetPoint(npoints4-1,RunSince,Par4);
	  par5_g[1]->SetPoint(npoints5-1,RunSince,Par5);
	}
	if (Code==2) {
	  par1_g[2]->SetPoint(npoints1-1,RunSince,Par1);
	  par2_g[2]->SetPoint(npoints2-1,RunSince,Par2);
	  par3_g[2]->SetPoint(npoints3-1,RunSince,Par3);
	  par4_g[2]->SetPoint(npoints4-1,RunSince,Par4);
	  par5_g[2]->SetPoint(npoints5-1,RunSince,Par5);
	}
	if (Code==3) {
	  par1_g[3]->SetPoint(npoints1-1,RunSince,Par1);
	  par2_g[3]->SetPoint(npoints2-1,RunSince,Par2);
	  par3_g[3]->SetPoint(npoints3-1,RunSince,Par3);
	  par4_g[3]->SetPoint(npoints4-1,RunSince,Par4);
	  par5_g[3]->SetPoint(npoints5-1,RunSince,Par5);
	}
	
	par1_g[0]->SetPointError(npoints1-1,0.,0.);
	par2_g[0]->SetPointError(npoints2-1,0.,Par2/sqrt(Entries));
	par3_g[0]->SetPointError(npoints3-1,0.,Par3/sqrt(Entries));
	par4_g[0]->SetPointError(npoints4-1,0.,Par4/sqrt(Entries));
	par5_g[0]->SetPointError(npoints5-1,0.,Par5/sqrt(Entries));
      }
    }

    if (i==ChannelID && ((RunSince>=init_run) && (RunSince<=last_run))) { 
      Algorithm=Algo;
      int npoints=0;
      // N Entries
      npoints=entr_g->GetN();
      npoints++;
      entr_g->Set(npoints);
      entr_g->SetPoint(npoints-1,RunSince,Entries);
      entr_g->SetPointError(npoints-1,0.,sqrt(Entries));

      // Par 1 
      npoints=0;
      for (int k=0;k<4;k++) npoints=par1_g[k]->GetN();
      npoints++;
      for (int k=0;k<4;k++) par1_g[k]->Set(npoints);
      par1_g[0]->SetPoint(npoints-1,RunSince,Par1);
      if (Code==1) par1_g[1]->SetPoint(npoints-1,RunSince,Par1);
      if (Code==2) par1_g[2]->SetPoint(npoints-1,RunSince,Par1);
      if (Code==3) par1_g[3]->SetPoint(npoints-1,RunSince,Par1);
      
      
      std::string::size_type loc = Algorithm.find( "CheckHisto_Mean&GatherData", 0 );
      if( loc != std::string::npos ) {
	Algorithm="CheckHisto_Mean&GatherData";
      } 

      if (Algorithm=="GatherData" || Algorithm=="Histogram_Not_Empty&GatherData" || 
	  Algorithm=="CheckHisto_Mean&GatherData" || Algorithm=="BinContentComp&GatherData") {  
	// in this case, Par1=Mean, error depends on the algorithm  
	if (Par3> 0 || Par4>0) {
	  // this is the new GatherData: return Mean, EMean, RMS, ERMS or 
	  // Mean, EMean, RMS, ERMS + XMean or Mean, EMean, NBins, RMS, ERMS 
	  par1_g[0]->SetPointError(npoints-1,0.,Par2);
	} else {
	  // this is the old GatherData: return Mean and RMS only
	  if (Entries>0.) {
	    par1_g[0]->SetPointError(npoints-1,0.,Par2/sqrt(Entries)); 	
	  } else {
	    par1_g[0]->SetPointError(npoints-1,0.,0.);	    
	  }
	}
      } else {
	if (Entries>0.) {
	  if (Algorithm=="Simple_gaus_Fit") {
	    par1_g[0]->SetPointError(npoints-1,0.,Par2/sqrt(Entries));
	  } else {
	    par1_g[0]->SetPointError(npoints-1,0.,Par1/sqrt(Entries)); 		
	  }
	} else {
	  par1_g[0]->SetPointError(npoints-1,0.,0.); 		
	}
      }

      // Par2 
      npoints=0;
      for (int k=0;k<4;k++) npoints=par2_g[k]->GetN();
      npoints++;
      for (int k=0;k<4;k++) par2_g[k]->Set(npoints);
      par2_g[0]->SetPoint(npoints-1,RunSince,Par2);
      if (Code==1) par2_g[1]->SetPoint(npoints-1,RunSince,Par2);
      if (Code==2) par2_g[2]->SetPoint(npoints-1,RunSince,Par2);
      if (Code==3) par2_g[3]->SetPoint(npoints-1,RunSince,Par2);
      if (Entries>0.) {
	par2_g[0]->SetPointError(npoints-1,0.,Par2/sqrt(Entries)); 
      } else {
	par2_g[0]->SetPointError(npoints-1,0.,0.); 
      }

      if ((Algorithm=="GatherData" || Algorithm=="Histogram_Not_Empty&GatherData") && 
	  (Par3> 0 || Par4>0) ) {
	// this is the new GatherData: return Mean, EMean, RMS, ERMS
	par2_g[0]->SetPoint(npoints-1,RunSince,Par3);
	if (Code==1) par2_g[1]->SetPoint(npoints-1,RunSince,Par3);
	if (Code==2) par2_g[2]->SetPoint(npoints-1,RunSince,Par3);
	if (Code==3) par2_g[3]->SetPoint(npoints-1,RunSince,Par3);
	par2_g[0]->SetPointError(npoints-1,0.,Par4);
      } 

      if ((Algorithm=="CheckHisto_Mean&GatherData" || Algorithm=="BinContentComp&GatherData" 
	   ) && (Par5!= 0)) {	  
	// this is the new GatherData: return Mean, EMean, RMS, ERMS + XMean or Mean, EMean, NBins, RMS, ERMS 
	if (Algorithm=="BinContentComp&GatherData") {
	  par2_g[0]->SetPoint(npoints-1,RunSince,Par4);
	  if (Code==1) par2_g[1]->SetPoint(npoints-1,RunSince,Par4);
	  if (Code==2) par2_g[2]->SetPoint(npoints-1,RunSince,Par4);
	  if (Code==3) par2_g[3]->SetPoint(npoints-1,RunSince,Par4);
	  par2_g[0]->SetPointError(npoints-1,0.,Par5);
	}
	if (Algorithm=="CheckHisto_Mean&GatherData") {
	  par2_g[0]->SetPoint(npoints-1,RunSince,Par3);
	  if (Code==1) par2_g[1]->SetPoint(npoints-1,RunSince,Par3);
	  if (Code==2) par2_g[2]->SetPoint(npoints-1,RunSince,Par3);
	  if (Code==3) par2_g[3]->SetPoint(npoints-1,RunSince,Par3);
	  par2_g[0]->SetPointError(npoints-1,0.,Par4);
	} 
      } 

      //Par3 
      npoints=0;
      if (Algorithm=="Simple_gaus_Fit" || Algorithm=="BinContentComp&GatherData"
	  || Algorithm=="CheckHisto_Mean&GatherData") { //currently the only algorithms with third parameter filled 
	for (int k=0;k<4;k++) npoints=par3_g[k]->GetN();
	npoints++;
	for (int k=0;k<4;k++) par3_g[k]->Set(npoints);
	par3_g[0]->SetPoint(npoints-1,RunSince,Par3);
	if (Code==1) par3_g[1]->SetPoint(npoints-1,RunSince,Par3);
	if (Code==2) par3_g[2]->SetPoint(npoints-1,RunSince,Par3);
	if (Code==3) par3_g[3]->SetPoint(npoints-1,RunSince,Par3);
	if (Entries>0.) {
	  par3_g[0]->SetPointError(npoints-1,0.,Par3/sqrt(Entries));
	} else {
	  par3_g[0]->SetPointError(npoints-1,0.,0.);
	}
	if (Algorithm=="CheckHisto_Mean&GatherData") { 
	  if (Par5!=0) {
	    par3_g[0]->SetPoint(npoints-1,RunSince,Par5);
	    if (Code==1) par3_g[1]->SetPoint(npoints-1,RunSince,Par5);
	    if (Code==2) par3_g[2]->SetPoint(npoints-1,RunSince,Par5);
	    if (Code==3) par3_g[3]->SetPoint(npoints-1,RunSince,Par5);
	    par3_g[0]->SetPointError(npoints-1,0.,Par4);
	  } else {
	    par3_g[0]->SetPoint(npoints-1,RunSince,Par3);
	    if (Code==1) par3_g[1]->SetPoint(npoints-1,RunSince,Par3);
	    if (Code==2) par3_g[2]->SetPoint(npoints-1,RunSince,Par3);
	    if (Code==3) par3_g[3]->SetPoint(npoints-1,RunSince,Par3);
	    if (Entries>0.) par3_g[0]->SetPointError(npoints-1,0.,Par2/sqrt(Entries));
	  } 
	}
      }
    } // end channel selection and run range selection
  }

  TFile *hfile = (TFile*)gROOT->GetListOfFiles()->FindObject("c1.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("c1.root","RECREATE","ROOT file with graphs");

  std::string par1_name= defParName(Algorithm, nameh, 1);
  std::string par2_name= defParName(Algorithm, nameh, 2);
  std::string par3_name= defParName(Algorithm, nameh, 3);
  std::string par4_name= defParName(Algorithm, nameh, 4);
  std::string par5_name= defParName(Algorithm, nameh, 5);

  std::string status_code[4];
  status_code[0]="";
  status_code[1]="Red";
  status_code[2]="Yellow";
  status_code[3]="Green";

  std::string par1_g_name[4];   std::string par2_g_name[4]; std::string par3_g_name[4]; std::string par4_g_name[4]; std::string par5_g_name[4];
  for (int k=0;k<4;k++) {
    par1_g_name[k] = "Par1, Algo:"+Algorithm+" "+status_code[k];  par2_g_name[k] = "Par2, Algo:"+Algorithm+" "+status_code[k];
    par3_g_name[k] = "Par3, Algo:"+Algorithm+" "+status_code[k];  par4_g_name[k] = "Par4, Algo:"+Algorithm+" "+status_code[k];
    par5_g_name[k] = "Par5, Algo:"+Algorithm+" "+status_code[k];
  }

  TCanvas *c1 = new TCanvas("c1"," History ");
  if (Algorithm=="Simple_gaus_Fit" || Algorithm=="BinContentComp&GatherData" || Algorithm=="CheckHisto_Mean&GatherData") { //currently the only algorithms with third parameter filled
    c1->SetCanvasSize(900,900); c1->Divide(2,2);
  } else {
    if (Algorithm=="BinContentComp") {
      c1->SetCanvasSize(900,900); c1->Divide(2,3);
    } else {
      c1->SetCanvasSize(1000,450); c1->Divide(3,1);
    }
  }
  c1->Update();
  int green = TColor::GetColor("#009900");
  int yellow = TColor::GetColor("#ff9933");

  c1->cd(1);
  gPad->SetGridx(); gPad->SetGridy();
  formatGraph( c1,entr_g);
  entr_g->SetTitle("N Entries");
  entr_g->GetYaxis()->SetTitle("N entries");
  entr_g->SetMarkerStyle(20);

  bool non_zero=false;

  if (entr_g->GetMaxSize()!=0) {
    entr_g->Draw("ap");
    non_zero=true;
  }

  c1->cd(2);
  gPad->SetGridx(); gPad->SetGridy();
  formatGraph( c1,par1_g[0]);
  for (int k=0;k<4;k++) {
    par1_g[k]->SetTitle(par1_g_name[k].c_str());
    par1_g[k]->SetMarkerStyle(20);
  }
  par1_g[0]->GetYaxis()->SetTitle(par1_name.c_str());
  par1_g[1]->SetMarkerColor(2);
  par1_g[2]->SetMarkerColor(yellow);
  par1_g[3]->SetMarkerColor(green);
  if (non_zero) {
    par1_g[0]->Draw("ap");
    par1_g[1]->Draw("p");
    par1_g[2]->Draw("p");
    par1_g[3]->Draw("p");
  }
  c1->cd(3);
  gPad->SetGridx(); gPad->SetGridy();
  formatGraph( c1,par2_g[0]);
  par2_g[0]->GetYaxis()->SetTitle(par2_name.c_str());
  for (int k=0;k<4;k++) {
    par2_g[k]->SetTitle(par2_g_name[k].c_str());
    par2_g[k]->SetMarkerStyle(20);  
  }
  par2_g[1]->SetMarkerColor(2);
  par2_g[2]->SetMarkerColor(yellow);
  par2_g[3]->SetMarkerColor(green);
  if (non_zero) {
    par2_g[0]->Draw("ap");
    par2_g[1]->Draw("p");
    par2_g[2]->Draw("p");
    par2_g[3]->Draw("p");
  }
  if (Algorithm=="Simple_gaus_Fit" || Algorithm=="BinContentComp&GatherData" || (Algorithm=="BinContentComp") || 
      Algorithm=="CheckHisto_Mean&GatherData") { //currently the only algorithm with third parameter filled  
    c1->cd(4);
    gPad->SetGridx(); gPad->SetGridy();
    formatGraph( c1,par3_g[0]);
    par3_g[0]->GetYaxis()->SetTitle(par3_name.c_str());
    for (int k=0;k<4;k++) {
      par3_g[k]->SetMarkerStyle(20);
      par3_g[k]->SetTitle(par3_g_name[k].c_str());
    }
    par3_g[1]->SetMarkerColor(2);
    par3_g[2]->SetMarkerColor(yellow);
    par3_g[3]->SetMarkerColor(green);
    if (non_zero) {
      par3_g[0]->Draw("ap");
      par3_g[1]->Draw("p");
      par3_g[2]->Draw("p");
      par3_g[3]->Draw("p");
    }
  }

  if (Algorithm=="BinContentComp") {
    c1->cd(5);
    gPad->SetGridx(); gPad->SetGridy();
    formatGraph( c1,par4_g[0]);
    par4_g[0]->GetYaxis()->SetTitle(par4_name.c_str());
    for (int k=0;k<4;k++) {
      par4_g[k]->SetMarkerStyle(20);
      par4_g[k]->SetTitle(par4_g_name[k].c_str());
    }
    par4_g[1]->SetMarkerColor(2);
    par4_g[2]->SetMarkerColor(yellow);
    par4_g[3]->SetMarkerColor(green);
    if (non_zero) {
      par4_g[0]->Draw("ap");
      par4_g[1]->Draw("p");
      par4_g[2]->Draw("p");
      par4_g[3]->Draw("p");
    }

    c1->cd(6);
    gPad->SetGridx(); gPad->SetGridy();
    formatGraph( c1,par5_g[0]);
    par5_g[0]->GetYaxis()->SetTitle(par5_name.c_str());
    for (int k=0;k<4;k++) {
      par5_g[k]->SetMarkerStyle(20);
      par5_g[k]->SetTitle(par5_g_name[k].c_str());
    }
    par5_g[1]->SetMarkerColor(2);
    par5_g[2]->SetMarkerColor(yellow);
    par5_g[3]->SetMarkerColor(green);
    if (non_zero) {
      par5_g[0]->Draw("ap");
      par5_g[1]->Draw("p");
      par5_g[2]->Draw("p");
      par5_g[3]->Draw("p");
    }

  }

  entr_g->Write("Nentr");
  for (int k=0;k<4;k++) {
    if (par1_g[k]) par1_g[k]->Write(par1_g_name[k].c_str());
    if (par2_g[k]) par2_g[k]->Write(par2_g_name[k].c_str());
    if (par3_g[k]) par3_g[k]->Write(par3_g_name[k].c_str());
    if (par4_g[k]) par4_g[k]->Write(par4_g_name[k].c_str());    
    if (par5_g[k]) par5_g[k]->Write(par5_g_name[k].c_str());    
  }

  hfile->Close();
  if (non_zero) {
    c1->Print("c1.gif");
  }
  delete entr_g;
  for (int k=0;k<4;k++) {
    if (par1_g[k]) delete par1_g[k];
    if (par2_g[k]) delete par2_g[k];
    if (par3_g[k]) delete par3_g[k];
    if (par4_g[k]) delete par4_g[k];
    if (par5_g[k]) delete par5_g[k];
  }
  f->Close();

}
  
void
HistogramDataCOOL::
historyDB(  int HistoId, std::string nameHisto, std::string tag_name  )
{
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(1);
  gStyle->SetStatColor(0);
  gStyle->SetFillColor(1);
  gStyle->SetPalette(1,0);
  gStyle->SetTitleFontSize(0.06);
  gStyle->SetTitleH(0.06);

  int run[10000];
  float StatusH[1000];
  float Entries[1000];
  float Par1[1000];
  float Par2[1000];
  float Par3[1000];
  float Par4[1000];
  float Par5[1000];
  std::string Algorithm;
  std::string Algorithm1;
  
  gROOT->SetBatch();

  for (int j=0;j<1000;j++) {
    StatusH[j]=-2.;  
    Par1[j]=0.;  
    Par2[j]=0.; 
    Par3[j]=0.; 
    Par4[j]=0.; 
    Par5[j]=0.; 
  }

  std::ifstream tmp_run("runxml");
  if (!tmp_run.is_open())
    {
      std::cout << " Could not open the run list file " << std::endl;
      return;
    }

  Int_t dummy=0;
  while(tmp_run.good()) {
    tmp_run >> run[dummy];
    dummy++;
  }
  int ALL = dummy-1;
  int i = HistoId;
  std::string nameh = nameHisto;
  std::string tagDB = tag_name;

  for (int j=0;j<ALL;j++) {
    setIOV(run[j]);
    StatusH[j]=dumpCode(nameh, tagDB);
    std::string entr=dumpHisto(i,"Entries", tagDB);
    std::string alg=dumpHisto(i,"Algo", tagDB);
    std::string aa=dumpHisto(i,"Par1", tagDB);
    std::string cc=dumpHisto(i,"Par2", tagDB);
    std::string ee=dumpHisto(i,"Par3", tagDB);
    std::string gg=dumpHisto(i,"Par4", tagDB);
    std::string ll=dumpHisto(i,"Par5", tagDB);

    std::istringstream entri(entr); 
    std::istringstream algo(alg); 
    std::istringstream bb(aa); 
    std::istringstream dd(cc); 
    std::istringstream ff(ee);
    std::istringstream hh(gg);
    std::istringstream mm(ll);   

    entri >> Entries[j];
    algo >> Algorithm1;
    bb >> Par1[j];
    dd >> Par2[j];
    ff >> Par3[j];
    hh >> Par4[j];
    mm >> Par5[j];
    if (alg.length()>0) Algorithm = Algorithm1;
  }

  TFile *hfile = (TFile*)gROOT->GetListOfFiles()->FindObject("c1.root");
  if (hfile) hfile->Close();
  hfile = new TFile("c1.root","RECREATE","ROOT file with graphs");

  TGraphErrors *entr_g = new TGraphErrors();
  TGraphErrors *par1_g[4];
  TGraphErrors *par2_g[4];
  TGraphErrors *par3_g[4];
  TGraphErrors *par4_g[4];
  // TGraphErrors *par5_g[4];

  for (int k=0;k<4;k++) {
    par1_g[k] = new TGraphErrors();
    par2_g[k] = new TGraphErrors();
    par3_g[k] = new TGraphErrors();
    par4_g[k] = new TGraphErrors();
    // par5_g[k] = new TGraphErrors();
  }

  
  std::string par1_name= defParName(Algorithm, nameh, 1);
  std::string par2_name= defParName(Algorithm, nameh, 2);
  std::string par3_name= defParName(Algorithm, nameh, 3);
  std::string par4_name= defParName(Algorithm, nameh, 4);
  std::string par5_name= defParName(Algorithm, nameh, 5);

  std::string status_code[4];
  status_code[0]="";
  status_code[1]="Red";
  status_code[2]="Yellow";
  status_code[3]="Green";

  std::string par1_g_name[4];   std::string par2_g_name[4]; std::string par3_g_name[4]; std::string par4_g_name[4]; std::string par5_g_name[4];
  for (int k=0;k<4;k++) {
    par1_g_name[k] = "Par1, Algo:"+Algorithm+" "+status_code[k];  par2_g_name[k] = "Par2, Algo:"+Algorithm+" "+status_code[k];
    par3_g_name[k] = "Par3, Algo:"+Algorithm+" "+status_code[k];  par4_g_name[k] = "Par4, Algo:"+Algorithm+" "+status_code[k];
    par5_g_name[k] = "Par5, Algo:"+Algorithm+" "+status_code[k];
  }

  for (int j=0;j<(ALL);j++) {
    int npoints=entr_g->GetN();
    npoints++;
    entr_g->Set(npoints);
    entr_g->SetPoint(npoints-1,run[j],Entries[j]);
    entr_g->SetPointError(npoints-1,0.,sqrt(Entries[j]));
  }

  std::string::size_type loc = Algorithm.find( "CheckHisto_Mean&GatherData", 0 );
  if( loc != std::string::npos ) {
    Algorithm="CheckHisto_Mean&GatherData";
  } 

  //Par1 
  for (int j=0;j<(ALL);j++) {
    int npoints;
    for (int k=0;k<4;k++) npoints=par1_g[k]->GetN();
    npoints++;
    for (int k=0;k<4;k++) par1_g[k]->Set(npoints);
    par1_g[0]->SetPoint(npoints-1,run[j],Par1[j]);
    if (StatusH[j]==1) par1_g[1]->SetPoint(npoints-1,run[j],Par1[j]);
    if (StatusH[j]==2) par1_g[2]->SetPoint(npoints-1,run[j],Par1[j]);
    if (StatusH[j]==3) par1_g[3]->SetPoint(npoints-1,run[j],Par1[j]);

    if (Algorithm=="GatherData" || Algorithm=="Histogram_Not_Empty&GatherData" || 
	Algorithm=="CheckHisto_Mean&GatherData" || Algorithm=="BinContentComp&GatherData") {  
      // in this case, Par1=Mean, error depends on the algorithm  
      if (Par3[j]> 0 || Par4[j]>0) {
	// this is the new GatherData: return Mean, EMean, RMS, ERMS or 
	// Mean, EMean, RMS, ERMS + XMean or Mean, EMean, NBins, RMS, ERMS 
	par1_g[0]->SetPointError(npoints-1,0.,Par2[j]);
      } else {
	// this is the old GatherData: return Mean and RMS only
	if (Entries[j]>0.) {
	  par1_g[0]->SetPointError(npoints-1,0.,Par2[j]/sqrt(Entries[j])); 	
	} else {
	  par1_g[0]->SetPointError(npoints-1,0.,0.);	    
	}
      }
    } else {
      if (Entries[j]>0.) {
	if (Algorithm=="Simple_gaus_Fit") {
	  par1_g[0]->SetPointError(npoints-1,0.,Par2[j]/sqrt(Entries[j]));
	} else {
	  par1_g[0]->SetPointError(npoints-1,0.,Par1[j]/sqrt(Entries[j])); 		
	}
      } else {
	par1_g[0]->SetPointError(npoints-1,0.,0.); 		
      }
    }
  }

  //Par2  
  for (int j=0;j<(ALL);j++) {
    int npoints;
    for (int k=0;k<4;k++) npoints=par2_g[k]->GetN();
    npoints++;
    for (int k=0;k<4;k++) par2_g[k]->Set(npoints);
    par2_g[0]->SetPoint(npoints-1,run[j],Par2[j]);
    if (StatusH[j]==1) par2_g[1]->SetPoint(npoints-1,run[j],Par2[j]);
    if (StatusH[j]==2) par2_g[2]->SetPoint(npoints-1,run[j],Par2[j]);
    if (StatusH[j]==3) par2_g[3]->SetPoint(npoints-1,run[j],Par2[j]);
    if (Entries[j]>0.) {
      par2_g[0]->SetPointError(npoints-1,0.,Par2[j]/sqrt(Entries[j])); 
    } else {
      par2_g[0]->SetPointError(npoints-1,0.,0.); 
    }

    if ((Algorithm=="GatherData" || Algorithm=="Histogram_Not_Empty&GatherData") && 
	(Par3[j]> 0 || Par4[j]>0) ) {
      // this is the new GatherData: return Mean, EMean, RMS, ERMS
      par2_g[0]->SetPoint(npoints-1,run[j],Par3[j]);
      if (StatusH[j]==1) par2_g[1]->SetPoint(npoints-1,run[j],Par3[j]);
      if (StatusH[j]==2) par2_g[2]->SetPoint(npoints-1,run[j],Par3[j]);
      if (StatusH[j]==3) par2_g[3]->SetPoint(npoints-1,run[j],Par3[j]);
      par2_g[0]->SetPointError(npoints-1,0.,Par4[j]);
    } 

    if ((Algorithm=="CheckHisto_Mean&GatherData" || Algorithm=="BinContentComp&GatherData" 
	 ) && (Par5[j]!= 0)) {	  
      // this is the new GatherData: return Mean, EMean, RMS, ERMS + XMean or Mean, EMean, NBins, RMS, ERMS 
      if (Algorithm=="BinContentComp&GatherData") {
	par2_g[0]->SetPoint(npoints-1,run[j],Par4[j]);
	if (StatusH[j]==1) par2_g[1]->SetPoint(npoints-1,run[j],Par4[j]);
	if (StatusH[j]==2) par2_g[2]->SetPoint(npoints-1,run[j],Par4[j]);
	if (StatusH[j]==3) par2_g[3]->SetPoint(npoints-1,run[j],Par4[j]);
	par2_g[0]->SetPointError(npoints-1,0.,Par5[j]);
      }
      if (Algorithm=="CheckHisto_Mean&GatherData") {
	par2_g[0]->SetPoint(npoints-1,run[j],Par3[j]);
	if (StatusH[j]==1) par2_g[1]->SetPoint(npoints-1,run[j],Par3[j]);
	if (StatusH[j]==2) par2_g[2]->SetPoint(npoints-1,run[j],Par3[j]);
	if (StatusH[j]==3) par2_g[3]->SetPoint(npoints-1,run[j],Par3[j]);
	par2_g[0]->SetPointError(npoints-1,0.,Par4[j]);
      } 
    } 
  }

  //Par3  
  if (Algorithm=="Simple_gaus_Fit" || Algorithm=="BinContentComp&GatherData" 
      || Algorithm=="CheckHisto_Mean&GatherData") { //currently the only algorithms with third parameter filled 
    for (int j=0;j<(ALL);j++) {
      int npoints;
      for (int k=0;k<4;k++) npoints=par3_g[k]->GetN();
      npoints++;
      for (int k=0;k<4;k++) par3_g[k]->Set(npoints);
      par3_g[0]->SetPoint(npoints-1,run[j],Par3[j]);
      if (StatusH[j]==1) par3_g[1]->SetPoint(npoints-1,run[j],Par3[j]);
      if (StatusH[j]==2) par3_g[2]->SetPoint(npoints-1,run[j],Par3[j]);
      if (StatusH[j]==3) par3_g[3]->SetPoint(npoints-1,run[j],Par3[j]);
      if (Entries[j]>0.) {
	par3_g[0]->SetPointError(npoints-1,0.,Par3[j]/sqrt(Entries[j]));
      } else {
	par3_g[0]->SetPointError(npoints-1,0.,0.);
      }
      if (Algorithm=="CheckHisto_Mean&GatherData") { 
	if (Par5[j]!=0) {
	  par3_g[0]->SetPoint(npoints-1,run[j],Par5[j]);
	  if (StatusH[j]==1) par3_g[1]->SetPoint(npoints-1,run[j],Par5[j]);
	  if (StatusH[j]==2) par3_g[2]->SetPoint(npoints-1,run[j],Par5[j]);
	  if (StatusH[j]==3) par3_g[3]->SetPoint(npoints-1,run[j],Par5[j]);
	  par3_g[0]->SetPointError(npoints-1,0.,Par4[j]);
	} else {
	  par3_g[0]->SetPoint(npoints-1,run[j],Par3[j]);
	  if (StatusH[j]==1) par3_g[1]->SetPoint(npoints-1,run[j],Par3[j]);
	  if (StatusH[j]==2) par3_g[2]->SetPoint(npoints-1,run[j],Par3[j]);
	  if (StatusH[j]==3) par3_g[3]->SetPoint(npoints-1,run[j],Par3[j]);
	  if (Entries[j]>0.) par3_g[0]->SetPointError(npoints-1,0.,Par2[j]/sqrt(Entries[j]));
	} 
      }
    }
  }

  TCanvas *c1 = new TCanvas("c1"," History ");
  if (Algorithm=="Simple_gaus_Fit" || Algorithm=="BinContentComp&GatherData"
      || (Algorithm=="CheckHisto_Mean&GatherData")) { //currently the only algorithms with third parameter filled
    c1->SetCanvasSize(900,900); c1->Divide(2,2);
  } else {
    c1->SetCanvasSize(1000,450); c1->Divide(3,1);
  }

  c1->Update();
  int green = TColor::GetColor("#009900");
  int yellow = TColor::GetColor("#ff9933");

  c1->cd(1);
  gPad->SetGridx(); gPad->SetGridy();
  formatGraph( c1,entr_g);
  entr_g->SetTitle("N Entries");
  entr_g->GetXaxis()->SetRange(run[0],run[ALL-1]);
  entr_g->GetYaxis()->SetTitle("N entries");
  entr_g->SetMarkerStyle(20);
  entr_g->Draw("ap");

  c1->cd(2);
  gPad->SetGridx(); gPad->SetGridy();
  formatGraph( c1,par1_g[0]);
  for (int k=0;k<4;k++) {
    par1_g[k]->SetTitle(par1_g_name[k].c_str());
    par1_g[k]->SetMarkerStyle(20);
  }
  par1_g[0]->GetYaxis()->SetTitle(par1_name.c_str());
  par1_g[1]->SetMarkerColor(2);
  par1_g[2]->SetMarkerColor(yellow);
  par1_g[3]->SetMarkerColor(green);
  par1_g[0]->Draw("ap");
  par1_g[1]->Draw("p");
  par1_g[2]->Draw("p");
  par1_g[3]->Draw("p");

  c1->cd(3);
  gPad->SetGridx(); gPad->SetGridy();
  formatGraph( c1,par2_g[0]);
  par2_g[0]->GetYaxis()->SetTitle(par2_name.c_str());
  for (int k=0;k<4;k++) {
    par2_g[k]->SetTitle(par2_g_name[k].c_str());
    par2_g[k]->SetMarkerStyle(20);  
  }
  par2_g[1]->SetMarkerColor(2);
  par2_g[2]->SetMarkerColor(yellow);
  par2_g[3]->SetMarkerColor(green);
  par2_g[0]->Draw("ap");
  par2_g[1]->Draw("p");
  par2_g[2]->Draw("p");
  par2_g[3]->Draw("p");


  if (Algorithm=="Simple_gaus_Fit" || Algorithm=="BinContentComp&GatherData" ||
      Algorithm=="CheckHisto_Mean&GatherData") { //currently the only algorithm with third parameter filled
    c1->cd(4);
    gPad->SetGridx(); gPad->SetGridy();
    formatGraph( c1,par3_g[0]);
    par3_g[0]->GetYaxis()->SetTitle(par3_name.c_str());
    for (int k=0;k<4;k++) {
      par3_g[k]->SetMarkerStyle(20);
      par3_g[k]->SetTitle(par3_g_name[k].c_str());
    }
    par3_g[1]->SetMarkerColor(2);
    par3_g[2]->SetMarkerColor(yellow);
    par3_g[3]->SetMarkerColor(green);
    par3_g[0]->Draw("ap");
    par3_g[1]->Draw("p");
    par3_g[2]->Draw("p");
    par3_g[3]->Draw("p");

  }


  entr_g->Write("Nentr");
  for (int k=0;k<4;k++) {
    if (par1_g[k]) par1_g[k]->Write(par1_g_name[k].c_str());
    if (par2_g[k]) par2_g[k]->Write(par2_g_name[k].c_str());
    if (par3_g[k]) par3_g[k]->Write(par3_g_name[k].c_str());
    if (par4_g[k]) par4_g[k]->Write(par4_g_name[k].c_str());
  }

  hfile->Close();
  c1->Print("c1.gif");

  delete entr_g;
  for (int k=0;k<4;k++) {
    if (par1_g[k]) delete par1_g[k];
    if (par2_g[k]) delete par2_g[k];
    if (par3_g[k]) delete par3_g[k];
    if (par4_g[k]) delete par4_g[k];
  }

  return;
}

std::string 
HistogramDataCOOL::
defParName(std::string algo, std::string nameHisto, int i)
{
  std::string Algorithm = algo;
  std::string nameh = nameHisto;
  std::string par1_name="Par1";   std::string par2_name="Par2"; std::string par3_name="Par3"; 
  std::string par4_name="Par4"; std::string par5_name="Par5";


  if (Algorithm=="GatherData" || Algorithm=="Histogram_Not_Empty&GatherData") {
    par1_name = "Mean";
    par2_name = "RMS";
  }
  if (Algorithm=="CheckHisto_Mean&GatherData") {
    par1_name = "Mean";
    par2_name = "RMS";
    par3_name = "XMean";
    par4_name = "YMean";
  }
  if (Algorithm=="BinContentComp&GatherData") {
    par1_name = "Mean";
    par2_name = "RMS";
    par3_name = "NBins";
  }
  if (Algorithm=="Bins_Diff_FromAvg") {
    par1_name = "Average";
    par2_name = "NBins_above_Av";
  } 
  if (Algorithm=="Simple_pol1_Fit") {
    par1_name = "par[0]";
    par2_name = "par[1]";
  }
  if (Algorithm=="Simple_gaus_Fit") {
    par1_name = "Mean gaus";
    par2_name = "RMS gaus";
    par3_name = "Constant gaus";
  }
  if (Algorithm=="SideBand_Absolute" ||
      Algorithm=="SideBand_Relative") {
    par1_name = "SideBands";
    par2_name = "Total Integral";
  }
  if (Algorithm=="CheckHisto_Mean") {
    par1_name = "XMean";
    par2_name = "YMean";
  }
  if (Algorithm=="CheckHisto_RMS") {
    par1_name = "XRMS";
    par2_name = "YRMS";
  }
  if (Algorithm=="Histogram_Not_Empty" || 
      Algorithm=="Histogram_Empty" || 
      Algorithm=="No_OverFlows" || 
      Algorithm=="No_UnderFlows" || 
      Algorithm=="Histogram_Effective_Empty") {

    par1_name = "NULL par";
    par2_name = "NULL par";
  }

  if (Algorithm=="Bins_LessThan_Threshold" ||
      Algorithm=="Bins_LessThanEqual_Threshold" ||
      Algorithm=="Bins_GreaterThan_Threshold" ||
      Algorithm=="Bins_GreaterThanEqual_Threshold" ||
      Algorithm=="Bins_Equal_Threshold" ||
      Algorithm=="Bins_NotEqual_Threshold" ) {

    par1_name = "NBins Less/Equal/NotEqual/Above Threshold";
    par2_name = "NULL par";
  }

   if (Algorithm=="BinContentComp") {
     par1_name = "NBins";
     par2_name = "Pix L0";
     par3_name = "Pix L1";
     par4_name = "Pix L2";
     par5_name = "SCT L0 S0";
   }

  if (i==1) {
    return par1_name;
  } else {
    if (i==2) {
      return par2_name;
    } else {
      if (i==3) {
	return par3_name;
      } else {
	if (i==4) {
	  return par4_name;
	} else {
	  if (i==5) {
	    return par5_name;
	  } else
	    return "";
	}
      }
    }
  }

}

void
HistogramDataCOOL::
formatGraph( TCanvas* c, TGraphErrors* gr ) const
{
  if( c == 0 || gr == 0 )
    return;

  c->SetLeftMargin(0.15);
  c->SetRightMargin(0.13);
  c->SetBottomMargin(0.15);
  c->SetTopMargin(0.12);
  
  gr->GetXaxis()->SetLabelSize(0.04);
  gr->GetYaxis()->SetLabelSize(0.04);
  gr->GetXaxis()->SetLabelFont(62);
  gr->GetYaxis()->SetLabelFont(62);

  gr->GetXaxis()->SetTitleSize(0.04);
  gr->GetYaxis()->SetTitleSize(0.04);
  gr->GetXaxis()->SetTitleFont(62);
  gr->GetYaxis()->SetTitleFont(62);
  gr->GetXaxis()->SetTitle("Runs");

  gr->GetYaxis()->SetTitleOffset(1.5);
  gr->GetXaxis()->SetTitleOffset(0.9);

  gr->GetXaxis()->SetNdivisions(504);

}

  // end of modification in insert functions (Monica 4/6/2008)

cool::IDatabasePtr 
HistogramDataCOOL::
getCoolDb() {
    return this->m_coolDb;
}



} //namespace dqutils
