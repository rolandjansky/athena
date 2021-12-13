/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LArCellsEmptyMonitoring
// -- Basic class aimed at running simple LAr monitoring function on ntuples
//    produced from that very same package.
//
// -- Author(s): Emma Kuwertz (KTH), Maud Schwoerer (LAPP), Olivier Simard (CEA-Saclay)
//
//
//

// Base includes
#include <map>
#include <utility>

#include <vector>
#include <fstream>
#include <iostream>
// Package includes
#include "LArSamplesMon/LArCellsEmptyMonitoring.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/HistoryIterator.h"
#include "LArSamplesMon/History.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/CaloId.h"
// ROOT includes
#include "TObject.h"
#include "TObjString.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TKey.h"

// forward declarations
using namespace std;
using namespace LArSamples;
/*namespace LArSamples { 
  struct Definitions;
  class Data;
  class Interface;
  class HistoryIterator;
  class History;
  class CellInfo;
  class ShapeInfo;
  class EventData;
  enum CaloId;
}*/

LArCellsEmptyMonitoring::LArCellsEmptyMonitoring()
  : m_LarIdTranslator (std::make_unique<LArIdTranslatorHelper>("LarIdTree.root"))
{
  // class constructor - set your class variables to default values here

  m_nexpected = 195072; // entries expected from Calo container

  
  m_SaveRootFile = false;
  m_SaveTextFile = true;
  m_Algo = 2;
  m_SetLumiblockRange = false;
  m_input_lbmin = 0;
  m_input_lbmax = 0;
  m_RemoveBadLB = true; 
  m_Qthreshold = 4000;
  m_nsigma = 4.;
  m_Ethreshold = 1.;
  m_UpperCountThreshold = 250;
  m_LowerCountThreshold = 25;
  m_LowerCellEnergyThreshold = 1.5;
  m_UpperCellEnergyThreshold = 30.;
  m_SelectRecurringBadCells = false;
  m_nsigmaHits = 10.;
  m_SaveRootTree = false;
  m_SetPartition = false;
}

LArCellsEmptyMonitoring::~LArCellsEmptyMonitoring()
{
}
//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::TestRun(const TString& inputfile)
{

  TString htitle,hname,textfilename,rootfilename;
  //define cumulative energy cuts here (in GeV)
  double Ecum_cut_PS = 20.;
  double Ecum_cut_calo = 10.;

  // define other constants and counters
  const double GeV = 1/1000.;
  int index = 0;
  double nsigmaHits = 10.;
  const int threshold = 1.;
  const int qthreshold = m_Qthreshold;
  int nskipped=0;
  int nBadCells=0;
  int noEvdata=0;
  unsigned int onlid = 0;
  int ne=0;
  int emin=0;
  int emax=0;
  int nq=0;
  int qmin=0;
  int qmax=0;
  int nlb=0;
  int lbmin=0;
  int lbmax=0;
  int runNumber=0;
  double MeanEventEnergy_max = -999;
  double MeanEventEnergy_min = 999;

  int CellCounter=0;


  // Deal with bad LBs
  // @ TODO: Do we still need this option? Not if we are always reading in the defect list at merging...?
  vector<int, std::allocator<int> > DQLBList;    
  // placeholder
  vector<int, std::allocator<int> > BadLBList;
  
  // find minimum and maximum LB, cell energy, q factor...  
  printf("Getting histogram limits... ");
  GetLimits_EqLB(inputfile,lbmin,lbmax,emin,emax,qmin,qmax,runNumber,BadLBList);
  if (m_SetLumiblockRange){
    lbmin = m_input_lbmin;
    lbmax = m_input_lbmax;
  }
  nlb=lbmax-lbmin;
  ne=emax-emin;
  nq=qmax-qmin;
  printf("Done.\n LBmin: %d  LBmax: %d  Emin: %d  Emax: %d  Qmin: %d  Qmax: %d\n",lbmin,lbmax,emin,emax,qmin,qmax);


  // Declare and open output textfile
  FILE * pFile=nullptr;
  if (m_SaveTextFile){
    if (!m_SetPartition){
      textfilename.Form("Output/BadCellList_run%d.txt",runNumber);
      pFile = fopen (textfilename , "w");
      if (!pFile){
	printf("Cannot open output text file\n");
	exit(0);}
    } else {
      // placeholder - in case someone wants to look at individual partitions only!
      if (m_SetPartition){
	textfilename.Form("Output/BadCellList_%s_run%d.txt",m_LarIdTranslator->GetPartitonLayerName(m_PartitionIndex),runNumber);
	pFile = fopen (textfilename , "w");
	if (!pFile){
	  printf("Cannot open output text file\n");
	  exit(0);}
      }
    }
  }
  // Open output text file to write column titles
  if (m_SaveTextFile){
    fprintf(pFile, "onlid // part // FT // SL // CH // # Events E>%d sigma // Mean Event Energy [GeV] (min // max) // # LBs // # LBs affected // fraction events Q>%d \n",(int)nsigmaHits,qthreshold);
  }
  
  int EventCount=0., qcount=0.;

 // Declare root file (LAr investigations - not intended to be produced by default)
  if (m_SetPartition){
    rootfilename.Form("Output/BadCells_%s_run%d.root",m_LarIdTranslator->GetPartitonLayerName(m_PartitionIndex),runNumber);  
  } else {
    rootfilename.Form("Output/BadCells_run%d.root",runNumber);  
  }

  TFile* fout = new TFile(rootfilename,"RECREATE");
  Float_t noise = 0;
  Float_t fr_q4k = 0;
  TH1F* h1_lb = nullptr;
  TH1F* h1_elb = nullptr;
  TH1F* h1_qlb = nullptr;
  TH1F* h1_e = nullptr;
  TH1F* h1_q = nullptr;
  TH2F* h2_elb = nullptr;


  // -------------------------------------------------------------------------
  // create individual cell histos
  // -------------------------------------------------------------------------
  printf("Creating histograms (nEbins = %d, nQbins = %d, nLBbins = %d)... ",ne,nq,nlb);
  TH1F* hene   = new TH1F("hEne","",ne,emin,emax); hene->SetXTitle("Energy [GeV]"); hene->SetYTitle("Events");
  TH1F* hqua   = new TH1F("hQua","",nq/100.,qmin,qmax); hqua->SetXTitle("Quality"); hqua->SetYTitle("Events");
  TH1F* hlb    = new TH1F("hLB","",nlb,lbmin,lbmax); hlb->SetXTitle("LB"); hlb->SetYTitle("Events");
  TH2F* henelb = new TH2F("hEnelb","",nlb,lbmin,lbmax,ne,emin,emax); henelb->SetXTitle("LB"); henelb->SetYTitle("Energy [GeV]");
  // -------------------------------------------------------------------------
  printf("Done.\n");

  // -------------------------------------------------------------------------
  // New test histos:
  // -------------------------------------------------------------------------
  printf("Creating cut test histograms...");
  int npl=4;
  TH2F* tempHist = new TH2F("tempNEvLB","",1000,0,1000.,1000,0.,1000.);
  TH1F** hNoise = new TH1F*[npl];
  TH1F** hCellsPerLB = new TH1F*[npl];
  TH2F** hNEvVsEMean = new TH2F*[npl];
  TH2F** hNEvVsECum = new TH2F*[npl];
  TH2F** hECumVsEMean = new TH2F*[npl];
  
  for (int i=0;i<npl;i++){
    hname.Form("hNEvVsEMean_Layer%i",i);
    hNEvVsEMean[i] = (TH2F*)tempHist->Clone(hname);
    hNEvVsEMean[i]->GetXaxis()->SetTitle("Number of Events (E > 10 #sigma) / LB");    hNEvVsEMean[i]->GetYaxis()->SetTitle("E_{mean} per Event / LB [GeV]");
    
    hname.Form("hNEvVsECum_Layer%i",i);
    hNEvVsECum[i] = (TH2F*)tempHist->Clone(hname);
    hNEvVsECum[i]->GetXaxis()->SetTitle("Number of Events (E > 10 #sigma) / LB");     hNEvVsECum[i]->GetYaxis()->SetTitle("E_{cumulative} per Cell / LB [GeV]");
    
    hname.Form("hECumVsEMean_Layer%i",i);
    hECumVsEMean[i] = (TH2F*)tempHist->Clone(hname);
    hECumVsEMean[i]->GetXaxis()->SetTitle("E_{cumulative} per Cell / LB [GeV]");     hECumVsEMean[i]->GetYaxis()->SetTitle("E_{mean} per Event / LB [GeV]");
    
    hname.Form("hNoise_Layer%i",i);
    hNoise[i] = new TH1F(hname,"",60,0.,30.);    
    hNoise[i]->GetXaxis()->SetTitle("Noise [GeV]");     hNoise[i]->GetYaxis()->SetTitle("Number Of Cells");
    
    hname.Form("hCellsPerLB_Layer%i",i);
    hCellsPerLB[i] = (TH1F*)hlb->Clone(hname);
    hCellsPerLB[i]->GetXaxis()->SetTitle("LB");     hNoise[i]->GetYaxis()->SetTitle("Number Of Cells");
  }
  printf("Done\n");
  // -------------------------------------------------------------------------

 
  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  printf("Number of events: %u %u\n",tuple->nEvents(),tuple->nChannels()); 
  unsigned int nchannels = tuple->nChannels();

  printf("Begin looping over cells...\n");
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){

    // pass empty cells
    if(tuple->historySize(ichan)==0){ nskipped++; continue; }
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();

    // index for partition
    index = m_LarIdTranslator->GetPartitionLayerIndex(cellInfo->calo(),cellInfo->layer());
    onlid = cellInfo->onlid();

    // if cell contains less hits than the threshold requirement, skip it
    unsigned int ndigits = hist->nData();
    if(ndigits<(unsigned int)threshold){ nskipped++; continue; }

    if(m_SaveTextFile || m_SaveRootFile){
      // set individual cell histos
      hname.Form("h0x%x_Energy",onlid);
      h1_e = (TH1F*)hene->Clone(hname);
      hname.Form("h0x%x_Quality",onlid);
      h1_q = (TH1F*)hqua->Clone(hname);
      hname.Form("h0x%x_LB",onlid);
      h1_lb = (TH1F*)hlb->Clone(hname);
      hname.Form("h0x%x_hNEvVsEMean",onlid);
      h1_elb = (TH1F*)hlb->Clone(hname);
      hname.Form("h0x%x_Energy_LB",onlid);
      h2_elb = (TH2F*)henelb->Clone(hname);
      hname.Form("h0x%x_Quality_LB",onlid);
      h1_qlb = (TH1F*)hlb->Clone(hname);
    }
    
    // loop on the events for each cell
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);
      double noise = data->noise()*GeV;
      const LArSamples::EventData* Evdata = data->eventData();
      if(!Evdata){ noEvdata++; continue; }// avoid crash
      int lumiBlock = Evdata->lumiBlock();
      double energy = data->energy()*GeV; 

      if (energy > nsigmaHits*noise){ // E>10sigma

	// Fill individual cell histos	
	if(m_SaveTextFile || m_SaveRootFile){
	  h1_e->Fill(energy);
	  h1_q->Fill(data->quality());
	  h1_lb->Fill(lumiBlock);
	  h1_elb->Fill(lumiBlock, energy);
	  h2_elb->Fill(lumiBlock, energy);
	  h1_qlb->Fill(lumiBlock, data->quality());
	}
	
	// count the number of processed events per cell with a bad q-factor
	if (data->quality() > qthreshold){
	  qcount = qcount+1;
	}
	
	// count the number of events processed per cell
	EventCount++;

      } // end of E>10sigma

    } // end of event loop
    if(m_SaveTextFile || m_SaveRootFile){
      hNoise[cellInfo->layer()]->Fill(noise);
    }
    CellCounter++;
    if ( CellCounter%1000 == 0 ) {
      printf("Scanning Cell No. = %d\n",CellCounter);
    }
    MeanEventEnergy_max = -999;
    MeanEventEnergy_min = 999;
    bool FlagCell=false;    
    int FlagCount=0;
    int NFullLB = 0;

    if(m_SaveTextFile || m_SaveRootFile){
      for (int ilb=1;ilb<=nlb;ilb++){
	if (h1_lb->GetBinContent(ilb) > 0.){
	  double EtotInLB = h1_elb->GetBinContent(ilb);
	  double NEventsInLB = h1_lb->GetBinContent(ilb);
	  //double MeanQ = h1_qlb->GetBinContent(ilb) / NEventsInLB; // change to fraction..
	  int LB_number = h1_lb->GetBinLowEdge(ilb);
	  NFullLB += 1;
	  double MeanEventEnergy = EtotInLB / NEventsInLB;
	  
	  
	  if (cellInfo->layer()==0){
	    if (EtotInLB>Ecum_cut_PS){
	      hNEvVsEMean[cellInfo->layer()]->Fill(NEventsInLB,MeanEventEnergy);
	      hNEvVsECum[cellInfo->layer()]->Fill(NEventsInLB,EtotInLB);
	      hECumVsEMean[cellInfo->layer()]->Fill(EtotInLB,MeanEventEnergy);
	      hCellsPerLB[cellInfo->layer()]->Fill(LB_number);
	    }
	  }
	  else if (EtotInLB>Ecum_cut_calo){
	    hNEvVsEMean[cellInfo->layer()]->Fill(NEventsInLB,MeanEventEnergy);
	    hNEvVsECum[cellInfo->layer()]->Fill(NEventsInLB,EtotInLB);
	    hECumVsEMean[cellInfo->layer()]->Fill(EtotInLB,MeanEventEnergy);
	    hCellsPerLB[cellInfo->layer()]->Fill(LB_number);
	  }
	  
	  if (EtotInLB>Ecum_cut_PS && cellInfo->layer()==0){
	    FlagCell=true; FlagCount++; 
	    if (MeanEventEnergy_max<MeanEventEnergy){ MeanEventEnergy_max = MeanEventEnergy; }
	    if (MeanEventEnergy_min>MeanEventEnergy){ MeanEventEnergy_min = MeanEventEnergy; }
	  }
	  else if (EtotInLB>Ecum_cut_calo && cellInfo->layer()==1){ 
	    FlagCell=true; FlagCount++; 
	    if (MeanEventEnergy_max<MeanEventEnergy){ MeanEventEnergy_max = MeanEventEnergy; }
	    if (MeanEventEnergy_min>MeanEventEnergy){ MeanEventEnergy_min = MeanEventEnergy; }
	  }
	  else if (EtotInLB>Ecum_cut_calo && cellInfo->layer()==2){  
	    FlagCell=true; FlagCount++; 
	    if (MeanEventEnergy_max<MeanEventEnergy){ MeanEventEnergy_max = MeanEventEnergy; }
	    if (MeanEventEnergy_min>MeanEventEnergy){ MeanEventEnergy_min = MeanEventEnergy; }
	  }
	  else if (EtotInLB>Ecum_cut_calo && cellInfo->layer()==3){  
	    FlagCell=true; FlagCount++; 
	    if (MeanEventEnergy_max<MeanEventEnergy){ MeanEventEnergy_max = MeanEventEnergy; }
	    if (MeanEventEnergy_min>MeanEventEnergy){ MeanEventEnergy_min = MeanEventEnergy; }
	  }
	}
      }
      
      fr_q4k = (double)qcount/(double)EventCount;
    }
    // Write bad channel quantities to text file
    if (m_SaveTextFile){
      if (FlagCell){
	nBadCells++;
	fprintf(pFile, "0x%x \t %7s \t %d \t %d \t %d \t %d \t %4.3f \t %4.3f \t %d \t %d \t %4.3f \n",onlid,m_LarIdTranslator->GetPartitonLayerName(index),cellInfo->feedThrough(),cellInfo->slot(),cellInfo->channel(),EventCount,MeanEventEnergy_min,MeanEventEnergy_max,NFullLB,FlagCount,fr_q4k);

      }
    }      
    
    if(h1_e)        delete h1_e;
    if(h1_q)        delete h1_q;
    if(h1_lb)       delete h1_lb;
    if(h1_elb)      delete h1_elb;
    if(h2_elb)      delete h2_elb;
    if(h1_qlb)      delete h1_qlb;
    qcount = 0.;
    EventCount = 0.;
  } // end of cell loop
  
  int nbad = m_nexpected-nskipped;
  printf("Skipped %d cells. Cells scanned = %d\n",nskipped,nbad);
  printf("Cells put in Bad Cell List = %d\n",nBadCells);
  if (noEvdata>0)  printf("No Evdata pointer: %d\n",noEvdata);

  // Write test histos to output rootfile
  if(m_SaveRootFile){
    fout->cd();
    for(int i=0;i<npl;i++){
      hNEvVsEMean[i]->Write();
      hNEvVsECum[i]->Write();
      hECumVsEMean[i]->Write();
      hNoise[i]->Write();
      hCellsPerLB[i]->Write();
    }
  }

    fout->Close(); delete fout;
    return;
  
}



//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::Run(const TString& inputfile)
{

  TString htitle,hname,textfilename,rootfilename;

  // partition/layer indexing entrirely defined by LarIdTranslator
  int npl = m_LarIdTranslator->GetNpl();


  // ----------------------------------------------------
  // Setter error messages
  // ----------------------------------------------------


  if (m_SelectRecurringBadCells && m_Algo != 2){
    printf("WARNING: Received instruction to select recurring bad cells only in SetSelectRecurringBadCells(bool flag)\n");
    printf("-------> Only one algorithm selected for cell selection criteria in SetAlgo(int algoindex)\n");
    printf("-------> Recurring cell selection overides algorithm specification\n");
    exit(0);
  }

  if (m_SetPartition && m_PartitionIndex == -1){
    printf("ERROR: Partition not specified in SetPartition(bool set_part,  std::string partname)\n");
    exit(0);
  } else if (m_SetPartition){
    printf("Running cell selection on %s only.\n",m_LarIdTranslator->GetPartitonLayerName(m_PartitionIndex));
  }
  // ----------------------------------------------------
  //
  int selectionFlag = -1;
  const double GeV = 1/1000.;
  const double ecut = m_Ethreshold; // GeV
  int index = 0;
  double nsigma = m_nsigma;
  double nsigmaHits = m_nsigmaHits;
  const int threshold = 10.;
  const int qthreshold = m_Qthreshold;
  int nskipped=0;
  int nBadCells=0;
  int noEvdata=0;
  unsigned int onlid = 0;
  int ne=0;
  int emin=0;
  int emax=0;
  int nq=0;
  int qmin=0;
  int qmax=0;
  int nlb=0;
  int nlb_corr=0; // count only non-empty LBs
  int lbmin=0;
  int lbmax=0;
  int runNumber=0;
  int nEvents_E_gt_ecut=0;
  double EventEnergySum=0.;
  double EventEnergyMean=0.;

  int nBadLBs=0;
  double  MeanHits=0, rmsHits=0;
  int iMeanCellHitsSelection=0,iEnergyThresholdSelection=0,iOR_selection=0,iAND_selection=0;
  int CellCounter=0;


  // Deal with bad LBs
  // @ TODO: Do we still need this option? Not if we are always reading in the defect list at merging...?
  vector<int, std::allocator<int> > DQLBList;    
  vector<int, std::allocator<int> > BadLBList;
  if(m_ReadDefectLBList){
    printf("Reading in DQ Defect LB List...\n");
    DQLBList=ReadBadLBList(m_LBfile);
    printf("...Done\n");
  }
    if (m_RemoveBadLB){
    printf("Getting bad LB list...\n");
    BadLBList=GetBadLBList(inputfile,lbmin,lbmax,nsigma,nlb,DQLBList);
    printf("...Done.\n");
    nBadLBs = BadLBList.size();
  }
  else if (m_ReadDefectLBList){
    BadLBList = DQLBList;
    nBadLBs = BadLBList.size();
  }
  
  // find minimum and maximum LB, cell energy, q factor...  
  printf("Getting histogram limits... ");
  GetLimits_EqLB(inputfile,lbmin,lbmax,emin,emax,qmin,qmax,runNumber,BadLBList);
  if (m_SetLumiblockRange){
    lbmin = m_input_lbmin;
    lbmax = m_input_lbmax;
  }
  

  nlb=lbmax-lbmin;
  ne=emax-emin;
  nq=qmax-qmin;
  printf("Done.\n LBmin: %d  LBmax: %d  Emin: %d  Emax: %d  Qmin: %d  Qmax: %d\n",lbmin,lbmax,emin,emax,qmin,qmax);

  // Declare and open summary textfile
  FILE * outputFile=nullptr;
  if (!m_SetPartition){
  textfilename.Form("Output/BadCellSelection_run%d.txt",runNumber);
  outputFile = fopen (textfilename , "w");
  if (!outputFile){
    std::cout<<("Cannot open output selection summary file " + textfilename)<< "\n";
    exit(0);}
  } else {
    textfilename.Form("BadCellSelection_%s_run%d.txt",m_LarIdTranslator->GetPartitonLayerName(m_PartitionIndex),runNumber);
    outputFile = fopen (textfilename , "w");
    if (!outputFile){
      std::cout<<("Cannot open output selection summary file " + textfilename)<< "\n";
      exit(0);}
  }
  

  // Declare and open output textfile
  FILE * pFile=nullptr;
  if (m_SaveTextFile){
    if (!m_SetPartition){
      textfilename.Form("Output/BadCellList_run%d.txt",runNumber);
      pFile = fopen (textfilename , "w");
      if (!pFile){
	printf("Cannot open output text file\n");
	exit(0);}
    } else {
      if (m_SetPartition){
	textfilename.Form("BadCellList_%s_run%d.txt",m_LarIdTranslator->GetPartitonLayerName(m_PartitionIndex),runNumber);
	pFile = fopen (textfilename , "w");
	if (!pFile){
	  printf("Cannot open output text file\n");
	  exit(0);}
      }
    }
  }



  
printf("Determining mean cell hits...\n ");
 GetMeanCellHits(inputfile, nlb, lbmin, lbmax, nsigmaHits, BadLBList, MeanHits, rmsHits, nlb_corr);
printf("Done.\n");
printf("Set threshold at %4.3f counts per cell for LB range. \n",(MeanHits+(nsigmaHits*rmsHits))*((double)nlb_corr-(double)nBadLBs));

 
  if (m_SaveTextFile){
    fprintf(pFile, "onlid // part // FT // SL // CH // Events (E>%d sigma) // Events E>1GeV // Mean Energy [GeV] // # LBs // fraction events Q>%d // Flagged in LB // selectionFlag\n",(int)nsigmaHits,qthreshold);
  }
  
  int EventCount=0., qcount=0.;
  // ----------------------------------------------------
  // Prepare output tree listing bad channels 
  // ----------------------------------------------------
  if (m_SetPartition){
    rootfilename.Form("Output/BadCells_%s_run%d.root",m_LarIdTranslator->GetPartitonLayerName(m_PartitionIndex),runNumber);  
  } else {
    rootfilename.Form("Output/BadCells_run%d.root",runNumber);  
  }

  TFile* fout = new TFile(rootfilename,"RECREATE");
  Int_t larid = 0;
  Int_t t_run =0;
  Int_t n_ensig = 0;
  Int_t n_ecut = 0;
  Float_t fr_LB = 0;
  Float_t nbrLB = 0;
  Float_t noise = 0;
  Float_t meanECell = 0;
  Float_t fr_q4k = 0;
  TH1F* h1_lb = nullptr;
  TH1F* h1_e = nullptr;
  TH1F* h1_q = nullptr;
  TH2F* h2_elb = nullptr;
  TH2D* h2_pulse = nullptr;
  TH2D* h2_t_LB = nullptr;
  TProfile* TProf_pulse = nullptr;
  TH1F* h1_ADCmax = nullptr;
  TTree* tree_cells = nullptr;
  if(m_SaveRootFile){
    tree_cells = new TTree("BadTree","LAr Tree ordered by OnlID");
    tree_cells->Branch("Energy",&h1_e);
    tree_cells->Branch("Quality",&h1_q);
    tree_cells->Branch("LB",&h1_lb);
    tree_cells->Branch("larid",&larid);
    tree_cells->Branch("onlid",&onlid);
    tree_cells->Branch("Run",&t_run);
    tree_cells->Branch("selectionFlag",&selectionFlag);
    tree_cells->Branch("nhits_ensig",&n_ensig);
    tree_cells->Branch("nhits_e1GeV",&n_ecut);
    tree_cells->Branch("Quality_4k",&fr_q4k);
    tree_cells->Branch("LBfraction",&fr_LB);
    tree_cells->Branch("EnergyVsLB",&h2_elb);
    tree_cells->Branch("PulseShape",&h2_pulse);
    tree_cells->Branch("PulsePeakTimeVsLB",&h2_t_LB);
    tree_cells->Branch("noise",&noise);
    tree_cells->Branch("MeanCellEnergy",&meanECell);
    tree_cells->Branch("PulseShapeTProf",&TProf_pulse);
    tree_cells->Branch("ADCmax",&h1_ADCmax);
  }

  Double_t xmin=-4.9,xmax=4.9;
  Double_t xw=0.1;
  TH2D* NormPulse = new TH2D("Normalised_pulse","",100,-200,200,400,-10,10); NormPulse->SetXTitle("Time [ns]"); NormPulse->SetYTitle("Value [ADC counts] / ADCmax");
  TH2D** Pulsemaps = new TH2D*[npl];
  TH2F** Cellmaps = new TH2F*[npl];
  TH2F** E_LBmaps = new TH2F*[npl];
  TH2D** t_LBmaps = new TH2D*[npl];
  TH1F** CellsFlagged_LB_part = new TH1F*[npl];
  TH1F** Emean_NbrEvents_part = new TH1F*[npl];
  TH2F* E_LB = new TH2F("E_LB","",nlb,lbmin,lbmax,ne,emin,emax); E_LB->SetXTitle("LB"); E_LB->SetYTitle("Energy [GeV]");    
  TH2D* t_LB=new TH2D("t_LB","",nlb,lbmin,lbmax,400,-200,200); t_LB->GetXaxis()->SetTitle("LB"); t_LB->GetYaxis()->SetTitle("Time(maxSample) + ofcTime [ns]");  
  TH1F* CF_LB = new TH1F("CF_LB","",nlb,lbmin,lbmax); CF_LB->GetXaxis()->SetTitle("LB"); CF_LB->GetYaxis()->SetTitle("Number of cells flagged"); 
  TH1F* ME_EV = new TH1F("ME_EV","",1000,0,1000); ME_EV->GetXaxis()->SetTitle("Nbr events >1 GeV"); ME_EV->GetYaxis()->SetTitle("Mean Energy [GeV]"); 



  if(m_SaveRootFile){
    for (int ii=0;ii<npl;ii++){
      Cellmaps[ii] = (TH2F*)m_LarIdTranslator->GetCaloPartitionLayerMap(ii);
      Cellmaps[ii]->GetXaxis()->SetTitle("#eta"); Cellmaps[ii]->GetYaxis()->SetTitle("#Phi"); 
      hname.Form("%s_PulseShape_%dsigma",m_LarIdTranslator->GetPartitonLayerName(ii),(int)nsigmaHits);
      Pulsemaps[ii] = (TH2D*)NormPulse->Clone(hname);
      hname.Form("%s_EvergyVsLB",m_LarIdTranslator->GetPartitonLayerName(ii));
      E_LBmaps[ii] = (TH2F*)E_LB->Clone(hname);
      hname.Form("%s_PulseTimeVsLB",m_LarIdTranslator->GetPartitonLayerName(ii));
      t_LBmaps[ii] = (TH2D*)t_LB->Clone(hname);
      hname.Form("%s_CellsFlaggedVsLB",m_LarIdTranslator->GetPartitonLayerName(ii));
      CellsFlagged_LB_part[ii] = (TH1F*)CF_LB->Clone(hname);
      // en plus
      hname.Form("%s_EmeanVsNbrEvents",m_LarIdTranslator->GetPartitonLayerName(ii));
      Emean_NbrEvents_part[ii] = (TH1F*)ME_EV->Clone(hname);
      //
    }
  }
  TH2F* Eeta = new TH2F("Eeta","",(int)floor((xmax-xmin)/xw),xmin,xmax,ne,emin,emax);
  Eeta->GetXaxis()->SetTitle("#eta");  Eeta->GetYaxis()->SetTitle("Energy [GeV]");
  TH1F* CellEnergy = new TH1F("CellEnergy","",ne,emin,emax);
  CellEnergy->GetXaxis()->SetTitle("Cell Mean Energy [GeV]"); CellEnergy->GetYaxis()->SetTitle("Cells");
  TH1F* LBfrac = new TH1F("LBfrac","",100,0.,1.);
  LBfrac->GetXaxis()->SetTitle("LB fraction"); LBfrac->GetYaxis()->SetTitle("Cells");
  TH1F* Qfrac = new TH1F("Qfrac","",100.,0.,1.);
  Qfrac->GetXaxis()->SetTitle("Fraction of Events Q>4000"); Qfrac->GetYaxis()->SetTitle("Cells");
  TH1F* CellsFlagged_LB = new TH1F("CellsFlagged_LB","",nlb,lbmin,lbmax);
  CellsFlagged_LB->GetXaxis()->SetTitle("LB"); CellsFlagged_LB->GetYaxis()->SetTitle("Number of cells flagged"); 
  TH1F* Emean_NbrEvents = new TH1F("Emean_NEvents_1GeV","",1000,0,1000);
  Emean_NbrEvents->GetXaxis()->SetTitle("N events > 1 GeV"); Emean_NbrEvents->GetYaxis()->SetTitle("Mean Energy [GeV]");
  //
  // -------------------------------------------------------------------------
  // create individual cell histos
  // -------------------------------------------------------------------------
  printf("Creating histograms (nEbins = %d, nQbins = %d, nLBbins = %d)... ",ne,nq,nlb);
  TH1F* hene   = new TH1F("hEne","",ne,emin,emax); hene->SetXTitle("Energy [GeV]"); hene->SetYTitle("Events");
  TH1F* hqua   = new TH1F("hQua","",nq/100.,qmin,qmax); hqua->SetXTitle("Quality"); hqua->SetYTitle("Events");
  TH1F* hlb    = new TH1F("hLB","",nlb,lbmin,lbmax); hlb->SetXTitle("LB"); hlb->SetYTitle("Events");
  TH2F* henelb = new TH2F("hEnelb","",nlb,lbmin,lbmax,ne,emin,emax); henelb->SetXTitle("LB"); henelb->SetYTitle("Energy [GeV]");
  TH2D* hpulse = new TH2D("hPulse","",100,-200,200,400, -10,10); hpulse->SetXTitle("Time [ns]"); hpulse->SetYTitle("Value [ADC counts] / ADCmax");
  TProfile* TProfpulse = new TProfile("", "",5, 0, 5, "s"); TProfpulse->SetXTitle("Sample Number"); TProfpulse->SetYTitle("Value [ADC counts]");
  TH2D* ht_LB=new TH2D("ht_LB","",nlb,lbmin,lbmax,400,-200,200); ht_LB->GetXaxis()->SetTitle("LB"); ht_LB->GetYaxis()->SetTitle("Time(maxSample) + ofcTime [ns]");  
  TH1F* hADCmax = new TH1F("","",110,-200,2000); hADCmax->SetXTitle("ADCmax [ADC counts]"); hADCmax->SetYTitle("Events");
  // -------------------------------------------------------------------------
  printf("Done.\n");
 
  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  printf("Number of events: %u %u\n",tuple->nEvents(),tuple->nChannels()); //tuple->ShowEvents("energy>0.");
  unsigned int nchannels = tuple->nChannels();

  fprintf(outputFile,"Onlid \t MeanCellHitBased \t EnergyThresholdBased \t OR \t AND\n");
  // -------------------------------------------------------------------------

  printf("Begin looping over cells...\n");
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ nskipped++; continue; }
    
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();
    double eta = cellInfo->eta();
    double phi = cellInfo->phi();


    // index for partition
    index = m_LarIdTranslator->GetPartitionLayerIndex(cellInfo->calo(),cellInfo->layer());
    onlid = cellInfo->onlid();
        
    // if only checking a specific partition, skip any cells not associated with that partition
    if(m_SetPartition){
      if (!CheckPartition(index)){ nskipped++; continue; }
    }

    // if cell contains less hits that the threshold requirement, skip it
    unsigned int ndigits = hist->nData();
    if(ndigits<(unsigned int)threshold){ nskipped++; continue; }


    // initialise individual cell histograms
    if(m_SaveTextFile || m_SaveRootFile){
      // set individual cell histos
      hname.Form("h0x%x_Energy",onlid);
      h1_e = (TH1F*)hene->Clone(hname);
      hname.Form("h0x%x_Quality",onlid);
      h1_q = (TH1F*)hqua->Clone(hname);
      hname.Form("h0x%x_LB",onlid);
      h1_lb = (TH1F*)hlb->Clone(hname);
      hname.Form("h0x%x_Energy_LB",onlid);
      h2_elb = (TH2F*)henelb->Clone(hname);
      hname.Form("h0x%x_Pulse",onlid);
      h2_pulse = (TH2D*)hpulse->Clone(hname);
      hname.Form("TProf0x%x_Pulse",onlid);
      TProf_pulse = (TProfile*)TProfpulse->Clone(hname);
      hname.Form("h0x%x_ADCmax",onlid);
      h1_ADCmax = (TH1F*)hADCmax->Clone(hname);
      hname.Form("h0x%x_t_LB",onlid);
      h2_t_LB = (TH2D*)ht_LB->Clone(hname);
    }
    
    bool CellInList = false;    
    int LBFlaggedIn = -1;
    
// loop on the events for each cell
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);
      noise = data->noise()*GeV;
      const LArSamples::EventData* Evdata = data->eventData();
      if(!Evdata){ noEvdata++; continue; }// avoid crash
      int lumiBlock = Evdata->lumiBlock();
      double energy = data->energy()*GeV; 

      // checks whether or not an event is in a bad LB when the bad LB list is read in manually
      // @ TODO:  Do we still need this functionality? Aren't we always going to read in the list at the merging stage now?
      bool isBadLB = CheckBadLBList(lumiBlock,BadLBList);

      // skip bad LBs
      if (isBadLB) continue; 

      // check the event satisfies selection criteria
      bool ProcessEvent = CheckEventSelectionCriteria(lumiBlock, nsigmaHits, energy, noise, lbmin, lbmax);
      if (ProcessEvent){
	
	if(m_SaveTextFile || m_SaveRootFile){
	  h1_e->Fill(data->energy()*GeV);
	  h1_q->Fill(data->quality());
	  h1_lb->Fill(lumiBlock);
	  h2_elb->Fill(lumiBlock, data->energy()*GeV);
	  h1_ADCmax->Fill(data->adcMax());
	  double time = data->maxPosition()*25.0+data->ofcTime(); 
	  h2_t_LB->Fill(lumiBlock,time);
	  for(unsigned int isample=0;isample<data->nSamples();isample++) {
	    TProf_pulse->Fill(isample, data->value(isample));
	    h2_pulse->Fill(data->time(isample)-time, data->value(isample)/data->adcMax());
	  }
	}
	
	// count the number of processed events per cell with a bad q-factor
	if (data->quality() > qthreshold){
	  qcount = qcount+1;
	}
	
	// count the number of events processed per cell
	EventCount++;
	
	// count number of processed events with energy > ecut and calculate cumulative energy
	if (data->energy()*GeV > ecut) {
	  nEvents_E_gt_ecut++;
	  EventEnergySum+=(data->energy()*GeV);
	}
	
	// if the cell isn't already in the list check whether or not it should be
	if(!CellInList){
	  selectionFlag=CheckCellSelectionCriteria(EventCount,nsigmaHits,MeanHits,rmsHits,nEvents_E_gt_ecut,EventEnergySum,nBadLBs,nlb_corr);
	  if(selectionFlag==2){
	    LBFlaggedIn = lumiBlock;
	    CellInList = true;
	  }
	  else if(selectionFlag==1){
	    if(m_Algo==2){
	      LBFlaggedIn = lumiBlock;
	      CellInList = true;
	    }
	    else if(m_Algo==0){
	      LBFlaggedIn = lumiBlock;
	      CellInList = true;
	    }
	  }
	  else if(selectionFlag==0){
	    if(m_Algo==2){
	      LBFlaggedIn = lumiBlock;
	      CellInList = true;
	    }
	    else if(m_Algo==1){
	      LBFlaggedIn = lumiBlock;
	      CellInList = true;
	    }
	  }
	}
	
      }
      
    } // end of event loop
    
    CellCounter++;
    if ( CellCounter%1000 == 0 ) {
      printf("Scanning Cell No. = %d\n",CellCounter);
    }
    selectionFlag=CheckCellSelectionCriteria(EventCount,nsigmaHits,MeanHits,rmsHits,nEvents_E_gt_ecut,EventEnergySum,nBadLBs,nlb_corr);
    
    // check whether or not cell needs to be put in list
    if (selectionFlag >= 0){
      bool ListOutput = true;
      int MeanCellHitsSelection = 0;
      int EnergyThresholdSelection = 0;
      int OR_selection = 0;
      int AND_selection = 0;
      
      if (selectionFlag == 2){
	MeanCellHitsSelection = 1;
	EnergyThresholdSelection = 1;
	OR_selection = 1;
	AND_selection = 1;
      }
      
      
      else if (selectionFlag == 1){
	if (m_Algo == 1) ListOutput = false;
	MeanCellHitsSelection = 1;
	EnergyThresholdSelection = 0;
	OR_selection = 1;
	AND_selection = 0;
      }
      
      else if (selectionFlag == 0){
	if (m_Algo == 0) ListOutput = false;
	MeanCellHitsSelection = 0;
	EnergyThresholdSelection = 1;
	OR_selection = 1;
	AND_selection = 0;
      }

      // record number of cells selected by each algorithm      
      iMeanCellHitsSelection += MeanCellHitsSelection;
      iEnergyThresholdSelection += EnergyThresholdSelection;
      iOR_selection += OR_selection;
      iAND_selection += AND_selection;
      
      // does the cell occur using both algorithms?
      if (m_SelectRecurringBadCells){ if(AND_selection == 0) ListOutput = false; }
      if (m_SelectRecurringBadCells){ if(AND_selection == 1) ListOutput = true; }
      
      if (ListOutput){
	fprintf(outputFile,"0x%x \t %d \t \t \t %d \t \t %d \t %d\n",onlid,MeanCellHitsSelection,EnergyThresholdSelection,OR_selection,AND_selection);
	
	
	
	nBadCells++;
	if(m_SaveTextFile){
	  int nLB = 0;
	  for (int ilb=1;ilb<=nlb;ilb++){
	    if (h1_lb->GetBinContent(ilb) > 0.) nLB++;

	  }
	  fr_LB = (double)nLB/(double)(nlb_corr-nBadLBs);
	  nbrLB = (double)nLB;
	  meanECell = h1_e->GetMean();
	  n_ensig = EventCount;
	  fr_q4k = (double)qcount/(double)EventCount;
	  n_ecut = nEvents_E_gt_ecut;
	}
	
	if (m_SaveRootFile){
	  int nLB = 0;
	  for (int ilb=1;ilb<=nlb;ilb++){
	    if (h1_lb->GetBinContent(ilb) > 0.) nLB++;
	  }
	  fr_LB = (double)nLB/(double)(nlb_corr-nBadLBs);
	  nbrLB = (double)nLB;
	  meanECell = h1_e->GetMean();
	  n_ensig = EventCount;
	  fr_q4k = (double)qcount/(double)EventCount;
	  n_ecut = nEvents_E_gt_ecut;
	  Cellmaps[index]->Fill(eta,phi,meanECell);
	  Eeta->Fill(eta,meanECell);
	  CellEnergy->Fill(meanECell);
	  LBfrac->Fill(fr_LB);
	  if (n_ecut>0) {
            Emean_NbrEvents->Fill(n_ecut,EventEnergySum/n_ecut);
	    Emean_NbrEvents_part[index]->Fill(n_ecut,EventEnergySum/n_ecut);
          }

	  Qfrac->Fill(fr_q4k);
	  Pulsemaps[index]->Add(Pulsemaps[index],h2_pulse);
	  E_LBmaps[index]->Add(E_LBmaps[index],h2_elb);
	  t_LBmaps[index]->Add(t_LBmaps[index],h2_t_LB);
	  CellsFlagged_LB_part[index]->Fill(LBFlaggedIn);
	  CellsFlagged_LB->Fill(LBFlaggedIn);

	}

	if (m_SaveRootTree){ tree_cells->Fill(); }
	
	
	
	if (m_SaveTextFile){
	  if (nEvents_E_gt_ecut>0) { EventEnergyMean = EventEnergySum/nEvents_E_gt_ecut; }
	  fprintf(pFile, "0x%x \t %7s \t %d \t %d \t %d \t %d \t %d \t %f \t %4.0f \t %4.3f \t %d \t %d\n",onlid,m_LarIdTranslator->GetPartitonLayerName(index),cellInfo->feedThrough(),cellInfo->slot(),cellInfo->channel(),EventCount,nEvents_E_gt_ecut,EventEnergyMean,nbrLB,fr_q4k,LBFlaggedIn,selectionFlag);

	}
	
      }
    }
    
    if(h1_e)        delete h1_e;
    if(h1_q)        delete h1_q;
    if(h1_lb)       delete h1_lb;
    if(h2_elb)      delete h2_elb;
    if(h2_pulse)    delete h2_pulse;
    if(TProf_pulse) delete TProf_pulse;
    if(h1_ADCmax)   delete h1_ADCmax;    
    if(h2_t_LB)     delete h2_t_LB;
    qcount = 0.;
    EventCount = 0.;
    nEvents_E_gt_ecut = 0.;
    EventEnergySum = 0.;
        EventEnergyMean = 0.; 
  } // end of cell loop
  printf("Cells selected: MeanCellHitsCut \t EnergyThreshodCut \t Either List \t Both Lists\n");
      printf("\t \t %d \t \t \t \t %d \t \t %d \t %d\n",iMeanCellHitsSelection,iEnergyThresholdSelection,iOR_selection,iAND_selection);
  if (m_SaveTextFile){
    fclose (pFile);
  }
  fclose (outputFile);

  int nbad = m_nexpected-nskipped;
  printf("Skipped %d cells. Cells scanned = %d\n",nskipped,nbad);
  printf("Cells put in Bad Cell List = %d\n",nBadCells);
  if (noEvdata>0)  printf("No Evdata pointer: %d\n",noEvdata);
  
  if(m_SaveRootFile){
    fout->cd();
              
    for(int i=0;i<npl;i++){
      if(Cellmaps[i]->GetEntries()>0) Cellmaps[i]->Write();
      if(E_LBmaps[i]->GetEntries()>0) E_LBmaps[i]->Write();
      if(t_LBmaps[i]->GetEntries()>0) t_LBmaps[i]->Write();
      if(Pulsemaps[i]->GetEntries()>0) Pulsemaps[i]->Write();
      if(CellsFlagged_LB_part[i]->GetEntries()>0) CellsFlagged_LB_part[i]->Write();
      if( Emean_NbrEvents_part[i]->GetEntries()>0) Emean_NbrEvents_part[i]->Write();
    }
    CellsFlagged_LB->Write();
    Eeta->Write();
    CellEnergy->Write();
    Qfrac->Write();
    Emean_NbrEvents->Write();
    LBfrac->Write();
  }
  
    fout->Close(); delete fout;
    return;
  
}

//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::GetLimits_EqLB(const char* inputfile, int& lbmin, int& lbmax, int& emin, int& emax, int& qmin, int& qmax, int& runNumber, const std::vector<int, std::allocator<int> >& BadLBList){
  int lb=0.;

  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  
  unsigned int nchannels = tuple->nChannels();
  
  // -------------------------------------------------------------------------
  qmin = emin = lbmin = 99999;
  qmax = emax = lbmax = -1;
  runNumber = 0.;
  
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ continue; }
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);

    unsigned int ndigits = hist->nData();
    if(ndigits==0){ continue; }    
    
    // loop on the events for each cells
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);
      const LArSamples::EventData* Evdata = data->eventData();
      if(!Evdata) continue;
      int lumiBlock = Evdata->lumiBlock();

      
      if (data->energy() != 0. && data->noise() != 0.){    // record only events with real energy/noise values 
	bool isBadLB = CheckBadLBList(lumiBlock,BadLBList);
	//skip bad LBs
	if (isBadLB) continue; 


	lb = (int)lumiBlock;
	if(lb>lbmax) lbmax = lb;
	if(lb<lbmin) lbmin = lb;
	double energyGeV = data->energy()/1000.;
	if(energyGeV > emax) emax = energyGeV;
	if(energyGeV < emin) emin = energyGeV;
	if(data->quality() > qmax) qmax = data->quality();
	if(data->quality() < qmin) qmin = data->quality();
	
	runNumber = data->run();
      }      
    }
  }
}

//____________________________________________________________________________________________________ USING ALL LBS
std::vector<int, std::allocator<int> >  LArCellsEmptyMonitoring::GetBadLBList(const char* inputfile, int lbmin, int lbmax, double nsigma, int nlb, std::vector<int, std::allocator<int> > DQLBList){
  std::map< std::pair<unsigned int, unsigned int>, unsigned int > eventCells_tot;
  std::map< std::pair<unsigned int, unsigned int>, double > eventEnergy_tot;
  std::map< std::pair<unsigned int, unsigned int>, unsigned int > eventLumiBlock;
  std::map< std::pair<std::string, unsigned int>, unsigned int > eventCells;
  int lb=0.;
  double NSIG = nsigma;

  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  unsigned int nchannels = tuple->nChannels();

  // -------------------------------------------------------------------------
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ continue; }
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();
    int index = m_LarIdTranslator->GetPartitionLayerIndex(cellInfo->calo(),cellInfo->layer());
    int calo = cellInfo->calo();

      if (m_MaskPresampler){
      if (CheckForPresamplerCells(index)){ continue; }
    }
    if (m_MaskCalo){
      if (!CheckForPresamplerCells(index)){ continue; }
    }

    unsigned int ndigits = hist->nData();
    if(ndigits==0){ continue; }
    
    // loop on the events for each cell
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);

      const LArSamples::EventData* Evdata = data->eventData();
      if(!Evdata) continue;
      int lumiBlock = Evdata->lumiBlock();
      //int lumiBlock = data->lumiBlock();
      
      lb = (int)lumiBlock;
      double energyGeV = data->energy()/1000.;


      if (lumiBlock <= lbmax && lumiBlock >= lbmin ) { // use only LBs in range
	if (data->energy() > sqrt((NSIG*data->noise())*(NSIG*data->noise())) && data->energy() != 0. && data->noise() != 0.){    // record only events with E>NSIG and real energy/noise values 
	  // cell-event mapping
	  std::pair<unsigned int, unsigned int> ev(data->run(), data->event());
	  std::pair<std::string, unsigned int> ev_cryo(GetCryostat(calo), data->event());
	  eventCells_tot[ev]++;
	  eventEnergy_tot[ev] += energyGeV;
	  eventLumiBlock[ev] = lb;
	  eventCells[ev_cryo]++;
	}
      }
    }
  }
  // -------------------------------------------------------------------------
  // Find Bad LBs -> make list (BadLB vector output)
  // -------------------------------------------------------------------------
  TProfile* hCells_Ev_LB = new TProfile("","",nlb,lbmin,lbmax,-100,100000);//averaged number of cells per event per LB
   std::map< std::string, TProfile*> hCellsEvLB;
   std::map< std::string, TH1F*> hp_cryo;
  // -------------------------------------------------------------------------
  // loop over events (based on Interface::ShowEvents).
  // -------------------------------------------------------------------------
   int nCryo = 8;   
   std::string Cryo[8] = {"EMBA","EMBC","EMECA","EMECC","HECA", "HECC","FCALA","FCALC"};
   for(int k = 0; k< nCryo; k++){
     hCellsEvLB[Cryo[k]] = (TProfile*)hCells_Ev_LB->Clone(); 
    }
    for(unsigned int ievent = 0; ievent < tuple->nEvents(); ievent++) {
     const LArSamples::EventData* evtData = tuple->eventData(ievent);
     std::pair<unsigned int, unsigned int> id(evtData->run(), evtData->event());
     if(eventCells_tot[id]==0) continue;

     for(int j=0; j<nCryo ;j++){
       std::pair<std::string, unsigned int> id_cryo(Cryo[j], evtData->event());
       hCellsEvLB[Cryo[j]]->Fill(eventLumiBlock[id], eventCells[id_cryo]);
     }
          
     hCells_Ev_LB->Fill(eventLumiBlock[id], eventCells_tot[id]);
  }

   TH1F* hp = new TH1F("","",(hCells_Ev_LB->GetBinContent(hCells_Ev_LB->GetMaximumBin())),0,hCells_Ev_LB->GetBinContent(hCells_Ev_LB->GetMaximumBin()));// histo to find average number of cells firing per event per LB
   for(int k = 0; k< nCryo; k++){
    TH1F* hp_temp = new TH1F("","",(hCellsEvLB[Cryo[k]]->GetBinContent(hCellsEvLB[Cryo[k]]->GetMaximumBin())*10),0,hCellsEvLB[Cryo[k]]->GetBinContent(hCellsEvLB[Cryo[k]]->GetMaximumBin()));// histo to find average number of cells firing per event per LB
     std::string name1 = "hp_" + Cryo[k];
     hp_cryo[Cryo[k]] = (TH1F*)hp_temp->Clone(name1.c_str());
    }
   
   for (int p=1;p<=nlb;p++){
    for(int j=0; j<nCryo; j++){
     double bincont = hCellsEvLB[Cryo[j]]->GetBinContent(p);
     hp_cryo[Cryo[j]]->Fill(bincont);
    }
    double bincont1 = hCells_Ev_LB->GetBinContent(p);
    hp->Fill(bincont1);
    }
   
  std::map< std::string, double> Mean;
  std::map< std::string, double> RMS;
  std::map< int, vector<std::string> > BadLB_cryo;

  for(int k=0; k<nCryo; k++)
   {
    Mean[Cryo[k]] = hp_cryo[Cryo[k]]->GetMean();
    RMS[Cryo[k]] = hp_cryo[Cryo[k]]->GetRMS();
   }
    
   double MEAN2 = hp->GetMean();
   double RMS2 = hp->GetRMS();
   m_Mean_checkBadLBs = MEAN2;
   m_RMS_checkBadLBs = RMS2;
   vector<int> BadLB;
   // vector<int> BadLB_tot;
   vector<int> BadLB_tot=std::move(DQLBList);

   double value1 = (MEAN2+(RMS2*3.));
   int numberFlagged=0;
   for (int l=1;l<=nlb;l++){
     for(int j=0; j<nCryo; j++){
     double value = MEAN2 + (RMS[Cryo[j]]*3);
     if(hCellsEvLB[Cryo[j]]->GetBinContent(l) > value){
       int badLB = hCellsEvLB[Cryo[j]]->GetBinLowEdge(l);
        BadLB_cryo[badLB].push_back(Cryo[j]);
        bool isadd = true;
        for(unsigned int p=0; p< BadLB_tot.size(); p++) {
         if(badLB == BadLB_tot[p]) isadd = false;
         }
        if(isadd){
	  BadLB_tot.push_back(badLB);
	  numberFlagged++;
	  printf("Removing bad LB %d in %s\n",badLB,Cryo[j].c_str());
	}
       }
     }

    if (hCells_Ev_LB->GetBinContent(l) > value1){
      BadLB.push_back(hCells_Ev_LB->GetBinLowEdge(l));
    }
  }
  printf("\n");
  printf("Number of Bad LBs found: %d\n",numberFlagged);
  printf("Number of LBs to be removed: %zu", BadLB_tot.size());
  printf("\n");
  delete tuple;

  return BadLB_tot;
}


//____________________________________________________________________________________________________
bool LArCellsEmptyMonitoring::CheckBadLBList(int lumiBlock, std::vector<int, std::allocator<int> > BadLBList){
	for (unsigned int yy=0;yy<BadLBList.size();yy++){
	  if (lumiBlock == BadLBList[yy]){
	    return true;
	    }
	}
	return false;
}

//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::GetMeanCellHits(const char* inputfile, int nlb, int lbmin, int lbmax, int nsigma, const std::vector<int, std::allocator<int> >& BadLBList, double& MeanHits, double& rmsHits, int& nlb_corr)
{


  int  nHits = 0., lumiBlock = 0.,nCells = 0.;
  double energy = 0, noise = 0;
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  unsigned int nchannels = tuple->nChannels();
  
  //TH1F* h1_hits = new TH1F("","",nlb,lbmin,lbmax); // temp histo filled with every event in cell with E>4sig
  double TotalRecordedHits=0;
  std::vector<int, std::allocator<int> > HitsPerLB;
  double var=0;
  TH1F* th1_Hits = new TH1F("","",nlb,lbmin,lbmax); // number of events (E>nsig) in each LB for each cell
  TH1F* hNLB = new TH1F("","",nlb,lbmin,lbmax); // number of lumiblocks

  // -------------------------------------------------------------------------
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ continue; }


    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);

    unsigned int ndigits = hist->nData();
    
    // loop on the events for each cells
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);
      const LArSamples::EventData* Evdata = data->eventData();
      if(!Evdata) continue;
      lumiBlock = Evdata->lumiBlock();

      energy = data->energy();
      noise = data->noise();
      bool isBadLB = CheckBadLBList(lumiBlock,BadLBList);
      if (isBadLB) continue; //skip bad LBs
      hNLB->Fill(lumiBlock);      

      bool ProcessEvent = CheckEventSelectionCriteria(lumiBlock, nsigma, energy, noise, lbmin, lbmax);
      if (ProcessEvent){
	

	th1_Hits->Fill(lumiBlock);
	nHits++;
      } 
    }
    if (nHits > 0.) nCells++;

    nHits = 0.;

  }

  
  // -------------------------------------------------------------------------
  // determine average #hits per cell per LB
  // -------------------------------------------------------------------------
  


  TH1F* tp_ev = new TH1F("","",th1_Hits->GetBinContent(th1_Hits->GetMaximumBin())*100,0,th1_Hits->GetBinContent(th1_Hits->GetMaximumBin()));

   for (int i=1;i<=nlb;i++){
     if(hNLB->GetBinContent(i)>0){
       nlb_corr++;
       tp_ev->Fill(th1_Hits->GetBinContent(i));
       TotalRecordedHits+=th1_Hits->GetBinContent(i);
       HitsPerLB.push_back(th1_Hits->GetBinContent(i));
     }
   }

   MeanHits = ((double)TotalRecordedHits/(double)nlb_corr)/(double)nCells;
   // MeanHits = tp_ev->GetMean()/(double)nCells;
   // rmsHits = tp_ev->GetRMS()/(double)nCells;

   for (unsigned int j=0;j<HitsPerLB.size();j++){
     var += (((double)HitsPerLB[j]/(double)nCells) - MeanHits)*(((double)HitsPerLB[j]/(double)nCells) - MeanHits);
   }

   rmsHits = var/nlb_corr;
   
   printf("Mean number of hits/cell for 1 LB = %4.3f, RMS = %4.3f\n",MeanHits,rmsHits);
   printf("Number of cells firing at E > %d sigma = %d\n",nsigma,nCells);
   printf("Total number of LBs included = %d\n",nlb_corr);


   if (th1_Hits) delete th1_Hits;     

}

//____________________________________________________________________________________________________
bool LArCellsEmptyMonitoring::CheckEventSelectionCriteria(int lumiBlock, int nsigma, double energy, Float_t noise, int lbmin, int lbmax){


  if (energy == 0 || noise == 0){
    return false;
  } else if (lumiBlock <= lbmax && lumiBlock >= lbmin && energy > sqrt((nsigma*noise)*(nsigma*noise)) ) { // use only LBs in range
    return true;
  }
  return false;
      
}


//____________________________________________________________________________________________________
int LArCellsEmptyMonitoring::CheckCellSelectionCriteria(int EventCount, int nsigmaHits, double MeanHits, double rmsHits, int nEvents_E_gt_ecut, double EventEnergySum, int nBadLBs, int nlb) const{
  //FLAG KEY:
  //selectionFlag == 0 -> Used EventEnergyCut
  //selectionFlag == 1 -> Used MeanCellHitCut
  //selectionFlag == 2 -> Used EventEnergyCut && MeanCellHitCut

 
  bool a=false;
  bool b=false;
  bool c=false;

  //pas bon  if ((EventCount > (MeanHits[GetSectionIndex(index)]+(nsigmaHits*rmsHits[GetSectionIndex(index)]))*(nlb-nBadLBs[GetCryoIndex(index)]))) a=true;

  //original
  if ((EventCount > (MeanHits+(nsigmaHits*rmsHits))*((double)nlb-(double)nBadLBs))) a=true;
  // reverse cut
  //  if ((EventCount < (MeanHits+(nsigmaHits*rmsHits))*((double)nlb-(double)nBadLBs))) a=true;

  //original
  if (nEvents_E_gt_ecut > m_UpperCountThreshold && (EventEnergySum/(double)nEvents_E_gt_ecut) > m_LowerCellEnergyThreshold) b=true;
  // reverse cut
  //  if (nEvents_E_gt_ecut < m_UpperCountThreshold && (EventEnergySum/(double)nEvents_E_gt_ecut) < m_LowerCellEnergyThreshold) b=true;

  //original
  if (nEvents_E_gt_ecut > m_LowerCountThreshold && (EventEnergySum/(double)nEvents_E_gt_ecut) > m_UpperCellEnergyThreshold) c=true;
  // reverse cut
  //  if (nEvents_E_gt_ecut < m_LowerCountThreshold && (EventEnergySum/(double)nEvents_E_gt_ecut) < m_UpperCellEnergyThreshold) c=true;


  if ((a && b) || (a && c)){
    return 2;
  } else if (a && !b && !c){
    return 1;
  } else if ((!a && b) || (!a && c)){
    return 0;
  }

  return -1;
}

//____________________________________________________________________________________________________
bool LArCellsEmptyMonitoring::CheckForPresamplerCells(int index){

  return index == 0 || index == 4;
}

//____________________________________________________________________________________________________
bool LArCellsEmptyMonitoring::CheckPartition(int index) const{
  return index == m_PartitionIndex;
}

//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::SetPartition(bool setpart, const std::string& partname)
{
  const int npl = m_LarIdTranslator->GetNpl();
  
  m_SetPartition = setpart;
  
  if(!setpart)
    {
      m_PartitionName = "";
      m_PartitionIndex = -1;
    }   
  else if(setpart)
    {
      bool isFoundPartition = false;
      for(int i=0; i< npl; i++){
	if(partname == m_LarIdTranslator->GetPartitonLayerName(i)){
	  m_PartitionName = "_" +partname;
	  m_PartitionIndex = i;
	  isFoundPartition = true; 
	}
      }
      if(!isFoundPartition){
	printf("ERROR: Partition %s does not exist! Invalid input name.\n",partname.c_str());
	printf("Possible candidates are:\n");
	for (int j=0;j<npl;j++){ printf("%s\n",m_LarIdTranslator->GetPartitonLayerName(j)); }
	exit(0);
      }
    }
}
//____________________________________________________________________________________________________
std::string LArCellsEmptyMonitoring::GetCryostat(int calo)
{
 std::string cryostat = "NotGiven";
 if(calo == 1) cryostat = "EMBA";
 else if(calo == -1) cryostat = "EMBC";
 else if(calo == 5) cryostat = "FCALA";
 else if(calo == -5) cryostat = "FCALC";
 else if(calo == 4) cryostat = "HECA";
 else if(calo == -4) cryostat = "HECC";
 else if(calo == 2 || calo == 3) cryostat = "EMECA";
 else if(calo == -2 || calo == -3) cryostat = "EMECC";
 return cryostat;
}


//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::ReadDefectLBList(bool ReadList, const TString& LBfile)
{


  if (!ReadList){
    m_LBfile="";
    m_ReadDefectLBList=false;
  }
  else{
    m_LBfile=LBfile;
    m_ReadDefectLBList=true;
  }
}

//____________________________________________________________________________________________________
std::vector<int> LArCellsEmptyMonitoring::ReadBadLBList(const TString& LBfile)
{
  std::vector<int> LBList;

  std::ifstream infile(LBfile.Data());
  std::string line;

  // assume single-line format with coma-separated LBs (from python)                                                                                                                
  std::getline(infile,line,'\n');
  TString filter(line.c_str());
  TObjArray* list = filter.Tokenize(", "); // coma\space delimiters                                                                                                                 
  if(list->GetEntries() == 0){
    printf("No LB filtering specified, or bad format. Exiting.\n");
    delete list;
    LBList.push_back(0);
    return LBList;
  }

  for(int k = 0; k < list->GetEntries(); k++){
    TObjString* tobs = (TObjString*)(list->At(k));
    LBList.push_back((int)(tobs->String()).Atoi());
  }
  delete list;
  printf("LB List: %d\n",(int)LBList.size());

  return LBList;
}
/*

std::vector<int, std::allocator<int> >  LArCellsEmptyMonitoring::ReadBadLBList(const TString LBfile)
{
  // Reads in defect LB list if necessary  
  std::vector<int, std::allocator<int> > tlist;
  std::ifstream infile(LBfile);
  std::string argStr;
  std::getline(infile,argStr);
  for (size_t i = 0,n; i <= argStr.length(); i = n+1)
    {
      n = argStr.find_first_of(',',i);
      if (n == std::string::npos) n = argStr.length();
      std::string tmp = argStr.substr(i,n-i);
      char ctmp[6];
      for (int ic=0;ic<6;ic++){
        ctmp[ic]=tmp[ic];
      }
      int temp = atoi (ctmp);
      //      printf("temp: %d\n",temp);
      tlist.push_back(temp);
    }
  return tlist;


  
}

*/
//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::ScanOnlids(const TString& inputfile)
{
  // Scans all channel entries in the ntuple and looks for duplicates onlids.

  int index = -1;
  ULong64_t onlid = 0;
  std::map<ULong64_t,unsigned int>* idmap = new std::map<ULong64_t,unsigned int>;
  std::map<ULong64_t,unsigned int>::iterator idmap_itr;
  int nskipped=0,nrepeated=0;

  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  printf("Number of events: %u %u\n",tuple->nEvents(),tuple->nChannels()); //tuple->ShowEvents("energy>0.");
  unsigned int nchannels = tuple->nChannels();

  // -------------------------------------------------------------------------
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ nskipped++; continue; }
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();

    // index for partition
    index = m_LarIdTranslator->GetPartitionLayerIndex(cellInfo->calo(),cellInfo->layer());
    onlid = cellInfo->onlid();
    /*
    if(m_LarIdTranslator->FindChannel(index,cellInfo->eta(),cellInfo->phi())){
      onlid = m_LarIdTranslator->onlid;
    } else onlid = 0;
    */
    if(onlid<=0) printf("%u: Bad Cell Onlid = 0x%x (%+.2f,%+.2f)\n",ichan,(unsigned int)onlid,cellInfo->eta(),cellInfo->phi());

    idmap_itr = idmap->find(onlid);
    
    // new onlid
    if(idmap_itr == idmap->end()){
      (*idmap)[onlid] = ichan;
    } else {
      nrepeated+=1;
      printf("Onlid 0x%x (%d,%s,%+.2f,%+.2f)\n",(unsigned int)onlid,index,m_LarIdTranslator->GetPartitonLayerName(index),
        m_LarIdTranslator->eta,m_LarIdTranslator->phi);
    }
  }

  printf("Skipped %d cells.\n",nskipped);
  printf("Number of onlids: Unique=%d, Repeated=%d\n",(int)idmap->size(),nrepeated); 

  return; 
}

//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::DoEtaPhiMonitoring(const char* inputfile,const char* optionplot,const char* optionsave)
{
  // Main monitoring function with primary goal to produce simple DQM-equivalent (eta,phi) maps.
  // Depending on the argument, the function will put different quantities in the histograms.
  // Currently supported options:
  //
  // - optionplot = "NAbove4Sigma","PercentageAbove4Sigma",
  //                "NAbove5Sigma","PercentageAbove5Sigma",
  //                "NQualityAbove4000","PercentageQualityAbove4000"
  // - optionsave = "root","png"
  //
  //

  printf("LArCellsEmptyMonitoring::DoEtaPhiMonitoring: Reading options: %s %s.\n",optionplot,optionsave);

  bool knormplot = false;
  if(strstr(optionplot,"Precentage")) knormplot = true; // needs normalization plots

  int kcuttype = 0; // no cut
  double nsigmacut = 0.;
  double qualitycut = 0.;
  if(strstr(optionplot,"Sigma")){
    kcuttype = 1; // energy
    if(strstr(optionplot,"4Sigma")) nsigmacut = 4.;
    if(strstr(optionplot,"5Sigma")) nsigmacut = 5.;
  } else if(strstr(optionplot,"Quality")){
    kcuttype = 2; // quality
    qualitycut = 4000.; // not much control for now
  }

  Char_t hname[512];
  int index = 0;
  int nskipped=0;
  ULong64_t onlid = 0;

  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  printf("Number of events: %u %u\n",tuple->nEvents(),tuple->nChannels());
  unsigned int nchannels = tuple->nChannels();

  // eta-phi maps
  const int nhists = m_LarIdTranslator->GetNpl();
  TH2F** hmap_counts_all = new TH2F*[nhists];
  TH2F** hmap_energy_cut = new TH2F*[nhists];
  TH2F** hmap_quality_cut = new TH2F*[nhists];
  for(int j=0;j<nhists;j++){
    hmap_counts_all[j] = (TH2F*)m_LarIdTranslator->GetCaloPartitionLayerMap(j);
    sprintf(hname,"counst_all_%s_%d",m_LarIdTranslator->GetPartitonLayerName(j),j);
    hmap_counts_all[j]->SetName(hname);
    //
    hmap_energy_cut[j] = (TH2F*)m_LarIdTranslator->GetCaloPartitionLayerMap(j);
    sprintf(hname,"energy_cut_%s_%d",m_LarIdTranslator->GetPartitonLayerName(j),j);
    hmap_energy_cut[j]->SetName(hname);
    //
    hmap_quality_cut[j] = (TH2F*)m_LarIdTranslator->GetCaloPartitionLayerMap(j);
    sprintf(hname,"quality_cut_%s_%d",m_LarIdTranslator->GetPartitonLayerName(j),j);
    hmap_quality_cut[j]->SetName(hname);
  }

  // -------------------------------------------------------------------------
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ nskipped++; continue; }
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();
    unsigned int ndigits = hist->nData();
    if(ndigits==0){ nskipped++; continue; }

    // index for partition
    index = m_LarIdTranslator->GetPartitionLayerIndex(cellInfo->calo(),cellInfo->layer());
    // onlid = m_LarIdTranslator->onlid;
    onlid = cellInfo->onlid();
    if(onlid<=0) printf("%u: Bad Cell Onlid = 0x%x (%+.2f,%+.2f)\n",ichan,(unsigned int)onlid,cellInfo->eta(),cellInfo->phi());

    // loop on the events for each cells
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);
      //const LArSamples::EventData* Evdata = data->eventData();
      
      // all stats
      hmap_counts_all[index]->Fill(m_LarIdTranslator->eta,m_LarIdTranslator->phi);

      // energy-like
      if(kcuttype==1 && data->noise()>0.){
        if((data->energy()/data->noise())>nsigmacut) hmap_energy_cut[index]->Fill(m_LarIdTranslator->eta,m_LarIdTranslator->phi);
      }
      if(kcuttype==2){
        if(data->quality()>qualitycut) hmap_quality_cut[index]->Fill(m_LarIdTranslator->eta,m_LarIdTranslator->phi);
      }
    }
  }

  int nbad = m_nexpected-nskipped;
  printf("Skipped %d cells. Bad Cells = %d\n",nskipped,nbad);

  // normalize, if needed
  if(knormplot){
    for(int j=0;j<nhists;j++){
      hmap_energy_cut[j]->Divide(hmap_counts_all[j]);
      hmap_quality_cut[j]->Divide(hmap_counts_all[j]);
    }
  }

  TFile* tout = nullptr;
  TCanvas* c0 = nullptr, *c1 = nullptr;

  if(!strcmp(optionsave,"root")){
    tout = new TFile("EtaPhiMonitoring.root","recreate");
    c0 = m_LarIdTranslator->CaloPartitionLayerDisplay((TH1**)hmap_counts_all,"Counts",1);
    c0->SetName("Normalization");
    c0->Write();
    for(int j=0;j<nhists;j++) hmap_counts_all[j]->Write();
    if(kcuttype==1){
      c0 = m_LarIdTranslator->CaloPartitionLayerDisplay((TH1**)hmap_energy_cut,"EnergyCut",1);
      c0->SetName("EnergyCut");
      c0->Write();
      for(int j=0;j<nhists;j++) hmap_energy_cut[j]->Write();
    }
    if(kcuttype==2){
      c0 = m_LarIdTranslator->CaloPartitionLayerDisplay((TH1**)hmap_quality_cut,"QualityCut",1);
      c0->SetName("QualityCut");
      c0->Write();
      for(int j=0;j<nhists;j++) hmap_quality_cut[j]->Write();
    }
    tout->Close(); delete tout;
  } else {
    c0 = m_LarIdTranslator->CaloPartitionLayerDisplay((TH1**)hmap_counts_all,"Counts",1);
    c0->SaveAs("Normalization.png");
    c1 = new TCanvas("c1","");
    for(int j=0;j<nhists;j++){ hmap_counts_all[j]->Draw("colz"); sprintf(hname,"%s.png",hmap_counts_all[j]->GetName()); c1->SaveAs(hname); }
    if(kcuttype==1){
      c0 = m_LarIdTranslator->CaloPartitionLayerDisplay((TH1**)hmap_energy_cut,"EnergyCut",1);
      c0->SaveAs("EnergyCut.png");
      c1->cd();
      for(int j=0;j<nhists;j++){ hmap_energy_cut[j]->Draw("colz"); sprintf(hname,"%s.png",hmap_energy_cut[j]->GetName()); c1->SaveAs(hname); }
    }
    if(kcuttype==2){
      c0 = m_LarIdTranslator->CaloPartitionLayerDisplay((TH1**)hmap_quality_cut,"QualityCut",1);
      c0->SaveAs("QualityCut.png");
      c1->cd();
      for(int j=0;j<nhists;j++){ hmap_quality_cut[j]->Draw("colz"); sprintf(hname,"%s.png",hmap_quality_cut[j]->GetName()); c1->SaveAs(hname); }
    }
  }

  return;
}

//____________________________________________________________________________________________________
void LArCellsEmptyMonitoring::TriggerEfficiency(const char* inputfile,float fractionInPS)
{
  // Function aimed at extracting the trigger performance of a specific chain
  // by emulating its online selection using the calibration stream data.
  //
  // This is currently used to monitor the rejection performance of the
  // PreSampler noise in the physics_CosmicCalo stream (L2_PreS* chains)
  // which runs on each family of triggers (EM,TAU,J).
  // One needs to set the fraction of energy deposited in the PreSampler with
  // fractionInPS.
  //


  std::map< std::pair<unsigned int, unsigned int>, unsigned int > eventCells;
  std::map< std::pair<unsigned int, unsigned int>, double > eventEnergy_PS;
  std::map< std::pair<unsigned int, unsigned int>, double > eventEnergy_LAr;
  std::map< std::pair<unsigned int, unsigned int>, unsigned int > eventCells_PS;
  std::map< std::pair<unsigned int, unsigned int>, unsigned int > eventLayer;
  int run=0;
  // Opening file:
  LArSamples::Interface* tuple = (LArSamples::Interface*)Interface::open(inputfile);
  unsigned int nchannels = tuple->nChannels();

  // -------------------------------------------------------------------------
  for(unsigned int ichan = 0; ichan < nchannels; ichan++){
    // pass empty cells
    if(tuple->historySize(ichan)==0){ continue; }
    
    // process non empty cells
    const LArSamples::History* hist = tuple->cellHistory(ichan);
    const LArSamples::CellInfo* cellInfo = hist->cellInfo();
    unsigned int layer = cellInfo->layer();
    
    unsigned int ndigits = hist->nData();
    if(ndigits==0){ continue; }
    
    // loop on the events for each cells
    for(unsigned int idigit = 0; idigit < ndigits; idigit++){
      const LArSamples::Data* data = hist->data(idigit);
      run = data->run();
      
      // convert to GeV
      double energyGeV = data->energy()/1000.;

      // cell-event mapping
      std::pair<unsigned int, unsigned int> ev(data->run(), data->event());
      eventCells[ev]++;
      eventEnergy_LAr[ev] += energyGeV;
      eventLayer[ev] = layer;
      if(layer == 0){
        eventEnergy_PS[ev] += energyGeV;
        eventCells_PS[ev]++;
      }
    }
  }

  // non-constant binning for a wider spetrum
   //double binE[48]={0,2,4,6,8,10,13,16,19,22,26,30,34,38,42,47,52,57,62,68,75,82,90,100,110,120,130,140,150,160,170,180,195,210,225,240,255,270,285,300,320,340,360,380,400,430,460,500};

   const int nbins = 42;
   double* binE = new double[nbins+1];
   int i=0,j=0;
   int stop=0;
   double de = 0.,ene = 0.;
   // steps of 2GeV for the first 10 bins [0-20]
   de = 2.;
   stop = (int)(20./de);
   for(i=0;i<stop;i++,j++){
     ene = de*(double)i;
     binE[j] = ene;
   }
   // steps of 5GeV for [20-100]
   de = 5.;
   stop = (int)((100.-20.)/de);
   for(i=0;i<stop;i++,j++){
     ene += de;
     binE[j] = ene;
   }
   // steps of 25GeV for [100-500]
   de = 25.;
   stop = (int)((500.-100.)/de);
   for(i=0;i<stop;i++,j++){
     ene += de;
     binE[j] = ene;
   }
   // add extra bin to cover the rest
   binE[j] = 3500.;
   

   TH1F* E_LAr_pass = new TH1F("E_LAr_pass","",nbins,binE);
   TH1F* E_LAr_tot = new TH1F("E_LAr_tot","",nbins,binE);
   TH1F* E_LAr_EM5_pass = new TH1F("E_LAr_EM5_pass","",nbins,binE);
   TH1F* E_LAr_EM5_tot = new TH1F("E_LAr_EM5_tot","",nbins,binE);
   TH1F* E_LAr_TAU8_pass = new TH1F("E_LAr_TAU8_pass","",nbins,binE);
   TH1F* E_LAr_TAU8_tot = new TH1F("E_LAr_TAU8_tot","",nbins,binE);
   TH1F* E_LAr_J10_pass = new TH1F("E_LAr_J10_pass","",nbins,binE);
   TH1F* E_LAr_J10_tot = new TH1F("E_LAr_J10_tot","",nbins,binE);
   TH1F* nCellsPS_pass = new TH1F("nCellsPS_pass","",100, 0, 100);
   TH1F* nCellsPS_tot = new TH1F("nCellsPS_tot","",100, 0, 100);
   TH1F* nCellsPS_EM5_pass = new TH1F("nCellsPS_EM5_pass","",100, 0, 100);
   TH1F* nCellsPS_EM5_tot = new TH1F("nCellsPS_EM5_tot","",100, 0, 100);
   TH1F* nCellsPS_TAU8_pass = new TH1F("nCellsPS_TAU8_pass","",100, 0, 100);
   TH1F* nCellsPS_TAU8_tot = new TH1F("nCellsPS_TAU8_tot","",100, 0, 100);
   TH1F* nCellsPS_J10_tot = new TH1F("nCellsPS_J10_tot","",100, 0, 100);
   TH1F* nCellsPS_J10_pass = new TH1F("nCellsPS_J10_pass","",100, 0, 100);
    
  // -------------------------------------------------------------------------
  // loop over events (based on Interface::ShowEvents).
  // -------------------------------------------------------------------------
  for(unsigned int ievent = 0; ievent < tuple->nEvents(); ievent++) {
     const LArSamples::EventData* evtData = tuple->eventData(ievent);
     std::pair<unsigned int, unsigned int> id(evtData->run(), evtData->event());
     if(eventCells[id]==0) continue;
     if(eventEnergy_LAr[id]<=0) continue;
     bool isEM5 = evtData->isPassed("L1_EM5_EMPTY");
     bool isTAU8 = evtData->isPassed("L1_TAU8_EMPTY");
     bool isJ10 = evtData->isPassed("L1_J10_EMPTY");

     E_LAr_tot->Fill(eventEnergy_LAr[id]);
     nCellsPS_tot->Fill(eventCells_PS[id]);
     if(isEM5){
        E_LAr_EM5_tot->Fill(eventEnergy_LAr[id]);
        nCellsPS_EM5_tot->Fill(eventCells_PS[id]);
     }
     if(isTAU8){
        E_LAr_TAU8_tot->Fill(eventEnergy_LAr[id]);
        nCellsPS_TAU8_tot->Fill(eventCells_PS[id]);
      }
     if(isJ10){
        E_LAr_J10_tot->Fill(eventEnergy_LAr[id]);
        nCellsPS_J10_tot->Fill(eventCells_PS[id]);
     }

     double ratio = 0;
     if((eventEnergy_LAr[id])!=0) 
        ratio = eventEnergy_PS[id]/(eventEnergy_LAr[id]);
     bool isL2_PreS = false;
     if(ratio > fractionInPS) isL2_PreS = true;

     if(isL2_PreS){
      E_LAr_pass->Fill(eventEnergy_LAr[id]);
      nCellsPS_pass->Fill(eventCells_PS[id]);
      if(isEM5){
        E_LAr_EM5_pass->Fill(eventEnergy_LAr[id]);
        nCellsPS_EM5_pass->Fill(eventCells_PS[id]);
     }
     if(isTAU8){
        E_LAr_TAU8_pass->Fill(eventEnergy_LAr[id]);
        nCellsPS_TAU8_pass->Fill(eventCells_PS[id]);
     }
     if(isJ10){
        E_LAr_J10_pass->Fill(eventEnergy_LAr[id]);
        nCellsPS_J10_pass->Fill(eventCells_PS[id]);
     }
     }
  }

  char fname[50];
  sprintf(fname,"TriggerEfficiency_%d.root",run);
  TFile* mfile = new TFile(fname,"recreate");
  E_LAr_pass->Write();
  E_LAr_tot->Write();
  E_LAr_EM5_pass->Write();
  E_LAr_EM5_tot->Write();
  E_LAr_TAU8_pass->Write();
  E_LAr_TAU8_tot->Write();
  E_LAr_J10_pass->Write();
  E_LAr_J10_tot->Write();
  nCellsPS_pass->Write();
  nCellsPS_tot->Write();
  nCellsPS_EM5_pass->Write();
  nCellsPS_EM5_tot->Write();
  nCellsPS_TAU8_pass->Write();
  nCellsPS_TAU8_tot->Write();
  nCellsPS_J10_pass->Write();
  nCellsPS_J10_tot->Write();

  mfile->Close(); delete mfile;
  delete [] binE;

  delete tuple;

}


