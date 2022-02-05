/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LArIdTranslatorHelper
// -- Class to access information about cells that may not be stored or quickly
//    accessible from the ntuples created in this package.
//
// -- Author(s): Olivier Simard (CEA-Saclay)
//
//
//

#include "LArSamplesMon/LArIdTranslatorHelper.h"

// ROOT includes
#include "TMath.h"
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
#include "TPad.h"
#include "TAxis.h"
#include "TKey.h"

LArIdTranslatorHelper::LArIdTranslatorHelper(const TString& file)
  : bec(0),side(0),ft(0),sl(0),ch(0),sa(0),part(0),
    eta(0),phi(0),x(-99999),y(-99999),z(-99999),
    onlid(0),offlid(0),ttid(0),
    hvid(nullptr),
    entry(0),
    binetadqm(0),binphidqm1(0),binphidqm2(0),
    m_kInitialized(false),
    m_tree(nullptr),
    m_file( nullptr),
    m_ntotal(0),m_extrabins(0),
    m_canvas_counts(0),
    m_clonemap_counts(0),
    m_nPartitionLayers(30),
    m_nHistCategories(5)
{
  // some hardcoded identifiers
  const Char_t* pl[30] = {
     "EMBPA","EMB1A","EMB2A","EMB3A","EMBPC","EMB1C","EMB2C","EMB3C",
     "EMECPA","EMEC1A","EMEC2A","EMEC3A","EMECPC","EMEC1C","EMEC2C","EMEC3C",
     "HEC0A","HEC1A","HEC2A","HEC3A","HEC0C","HEC1C","HEC2C","HEC3C",
     "FCAL1A","FCAL2A","FCAL3A","FCAL1C","FCAL2C","FCAL3C"
  };
  m_PartitionLayers.resize (m_nPartitionLayers);

  const Char_t* ch[5] = {
    "FT","SL","CH","ONLID","Entry"
  };
  m_HistCategories.resize (m_nHistCategories);

  m_HistCellmaps.resize (m_nPartitionLayers);
  for(int ipl=0;ipl<m_nPartitionLayers;ipl++){
    m_PartitionLayers[ipl] = pl[ipl];
    m_HistCellmaps[ipl].resize (m_nHistCategories);
  }
  for(int ic=0;ic<m_nHistCategories;ic++){
    m_HistCategories[ic] = ch[ic];
  }
  
  printf("Initialized LArIdTranslatorHelper object with %d Partition/Layers and %d histogram frame categories.\n",
    m_nPartitionLayers,m_nHistCategories);

  // load file
  bool loaded = LoadIdTranslator(file);
  if(loaded){
    printf("Loaded IdTranslator successfully with %d entries, and histograms with %d duplicate bins.\n",m_ntotal,m_extrabins);
    m_kInitialized = true;
  } else m_kInitialized = false;
}

LArIdTranslatorHelper::~LArIdTranslatorHelper()
{
  // class destructor - delete your created objects here

  if(m_kInitialized && m_file && m_file->IsOpen()){ m_file->Close(); delete m_file; }
}

//____________________________________________________________________________________________________
bool LArIdTranslatorHelper::LoadIdTranslator(const TString& file)
{
  // Load Translator root file containing channel info and mapping.

  // info from single cells
  m_file = new TFile(file);
  if(!m_file || !m_file->IsOpen()){
    printf("LArIdTranslatorHelper::LoadIdTranslator : File %s could not be found (see above TFile message).\n",file.Data());
    return false;
  }

  m_tree = (TTree*)m_file->Get("LarId");
  m_tree->SetBranchAddress("BEC",&bec);
  m_tree->SetBranchAddress("Region",&side);
  m_tree->SetBranchAddress("FT",&ft);
  m_tree->SetBranchAddress("SL",&sl);
  m_tree->SetBranchAddress("CH",&ch);
  m_tree->SetBranchAddress("Sampling",&sa);
  m_tree->SetBranchAddress("Partition",&part);
  m_tree->SetBranchAddress("F_Eta",&eta);
  m_tree->SetBranchAddress("F_Phi",&phi);
  m_tree->SetBranchAddress("X",&x);
  m_tree->SetBranchAddress("Y",&y);
  m_tree->SetBranchAddress("Z",&z);
  m_tree->SetBranchAddress("onlid",&onlid);
  m_tree->SetBranchAddress("offlid",&offlid);
  m_tree->SetBranchAddress("ttid",&ttid);
  m_tree->SetBranchAddress("hvid",&hvid);
  m_tree->SetBranchAddress("binetadqm",&binetadqm);
  m_tree->SetBranchAddress("binphidqm1",&binphidqm1);
  m_tree->SetBranchAddress("binphidqm2",&binphidqm2);
  m_ntotal = (int)m_tree->GetEntries();

  // load mapping
  Int_t i,j,ix,iy;
  Int_t nbins = 0;
  Char_t name[128];
  for(i=0;i<m_nPartitionLayers;i++){
    for(j=0;j<m_nHistCategories;j++){
      sprintf(name,"%s_%s",m_PartitionLayers[i].c_str(),m_HistCategories[j].c_str());
      m_HistCellmaps[i][j] = std::unique_ptr<TH2I> ((TH2I*)m_file->Get(name));
    }
    
    nbins += (m_HistCellmaps[i][0]->GetXaxis()->GetNbins())*(m_HistCellmaps[i][0]->GetYaxis()->GetNbins());
    for(ix=1;ix<=m_HistCellmaps[i][0]->GetXaxis()->GetNbins();ix++){
      for(iy=1;iy<=m_HistCellmaps[i][0]->GetYaxis()->GetNbins();iy++){
        if(m_HistCellmaps[i][0]->GetBinContent(ix,iy)==-1) nbins--;
      }
    }
  }
  // duplicate bins
  m_extrabins = nbins-m_ntotal;

  return true;
}

//____________________________________________________________________________________________________
int LArIdTranslatorHelper::GetPartitionLayerIndex(const int partition, const int layer) const
{
  // Mapping from the partition/layer to partitionlayers.
  // This must agree with what is initialized in the contructor!
  //
  
  // Convention for 'partition' from LArSamplesMon ntuples:
  // -----------------------------------------
  // FCAL_A  	 +5
  // HEC_A 	+4
  // EMEC_OUTER_A 	+3
  // EMEC_INNER_A 	+2
  // EMB_A 	+1
  // EMB_C 	-1
  // EMEC_INNER_C 	-2
  // EMEC_OUTER_C 	-3
  // HEC_C 	-4
  // FCAL_C 	-5
  // -----------------------------------------
  //
  // const Char_t* partitionlayers[npl] = {
  //   "EMBPA","EMB1A","EMB2A","EMB3A","EMBPC","EMB1C","EMB2C","EMB3C",
  //   "EMECPA","EMEC1A","EMEC2A","EMEC3A","EMECPC","EMEC1C","EMEC2C","EMEC3C",
  //   "HEC0A","HEC1A","HEC2A","HEC3A","HEC0C","HEC1C","HEC2C","HEC3C",
  //   "FCAL1A","FCAL2A","FCAL3A","FCAL1C","FCAL2C","FCAL3C"
  // };
  
  if(partition==1){ // EMBA
    return layer;
  } else if(partition == -1){ // EMBC
    return layer+4;
  } else if(partition == 2 || partition == 3){ // EMECA
    return layer+8;
  } else if(partition == -2 || partition == -3){ // EMECC
    return layer+12;
  } else if(partition == 4){ // HECA
    return layer+16;
  } else if(partition == -4){ // HECC
    return layer+20;
  } else if(partition == 5){ // FCALA
    return layer+23;
  } else if(partition == -5){ // FCALC
    return layer+26;
  }
  return -1; // error value
}

//____________________________________________________________________________________________________
bool LArIdTranslatorHelper::FindChannel(const int index, float eta, float phi)
{ 
  // Selects the right entry from the Tree, given the partition index
  // and (eta,phi) coordinates.
  //
  // Status bit returned: 1 is successful, 0 is not
  //
  
  if(index < 0 || index >= m_nPartitionLayers){
    printf("LArIdTranslatorHelper::FindChannel : Supplied index out of range: %d, [%d,%d].\n",index,0,m_nPartitionLayers-1);
    entry = 0; // return to tree top
    return false; // bad status
  }

  // watch the hardcoded index for "Entry" in array 'm_HistCategories'
  entry = (Long_t)m_HistCellmaps[index][4]->GetBinContent(
    m_HistCellmaps[index][4]->GetXaxis()->FindBin(eta),m_HistCellmaps[index][4]->GetYaxis()->FindBin(phi));
  m_tree->GetEntry(entry); // automatically positions the tree at this entry
  return true; // found it
}
//____________________________________________________________________________________________________
bool LArIdTranslatorHelper::IsHVLine(const int hvline) const
{
  // Loops over the current entry hvline vector and determines if this corresponds
  // to the input.
  // The current hvid is set by a call to FindChannel(..) or similar.

  for(unsigned int i=0;i<hvid->size();i++) if(hvid->at(i)==hvline) return true;
  return false;
}

//____________________________________________________________________________________________________
const Char_t* LArIdTranslatorHelper::GetPartitonLayerName(const int index)
{
  if(index < 0 || index >= m_nPartitionLayers){
    printf("LArIdTranslatorHelper::GetPartitonLayerName : Supplied index out of range: %d, [%d,%d].\n",index,0,m_nPartitionLayers-1);
    return nullptr;
  }

  return m_PartitionLayers[index].c_str();
}

//____________________________________________________________________________________________________
TH2* LArIdTranslatorHelper::GetCaloPartitionLayerMap(const int index,bool kProfile)
{
  // Returns a clone of the m_HistCellmaps histograms.
  // The type is TH2F for monitoring purposes.
  //
  // This function returns a new histogram hence the user must be careful at cleaning up after usage.

  if(index < 0 || index >= m_nPartitionLayers){
    printf("LArIdTranslatorHelper::GetCaloPartitionLayerMap : Supplied index out of range: %d, [%d,%d].\n",index,0,m_nPartitionLayers-1);
    return nullptr;
  }

  TH2* th2 = nullptr;
  if(!m_HistCellmaps[index][0]){
    printf("LArIdTranslatorHelper::GetCaloPartitionLayerMap : Could not clone map for index %d. Check that maps were loaded from input rootfile.\n",index);
    return nullptr;
  } else {
    sprintf(m_namebuf,"%s_Cloned_%d",m_PartitionLayers[index].c_str(),m_clonemap_counts);
    if(kProfile){
      th2 = new TProfile2D(m_namebuf,"",m_HistCellmaps[index][0]->GetXaxis()->GetNbins(),(m_HistCellmaps[index][0]->GetXaxis()->GetXbins())->GetArray(),m_HistCellmaps[index][0]->GetYaxis()->GetNbins(),(m_HistCellmaps[index][0]->GetYaxis()->GetXbins())->GetArray());
    } else {
      th2 = (TH2F*)(m_HistCellmaps[index][0]->Clone(m_namebuf));
      th2->Reset("M"); // reset content and statistics
    }
  }

  m_clonemap_counts++;

  return th2;
}
//____________________________________________________________________________________________________
TCanvas* LArIdTranslatorHelper::CaloPartitionLayerDisplay(TH1** h,const Char_t* title,bool kLogz)
{
  // Produces a TCanvas with the partitionlayers arranged.
  // This assumes some hardcoded settings:
  // - m_nPartitionLayers = 30
  //
  // Once the IdTranslator histograms are loaded, one can reuse the frames and fill them with
  // a different variable, and then pass them to this function with **h2 to make that display.
  //

  if(m_nPartitionLayers!=30){
    printf("Error in LArIdTranslatorHelper::CaloPartitionLayerDisplay: dimension must be 30.\n");
    return nullptr;
  }

  // create new name according to scheme
  sprintf(m_namebuf,"calosnapshot_%d",m_canvas_counts);
  TCanvas* c = new TCanvas(m_namebuf,title,1000,600);
  c->Divide(8,4); // 32 subpads (2 unused)

  // ordering:
  // bottom-top is by sampling ([0,3] except for FCAL)
  // left-right is by partition (following sign of eta, C/A)
  int i,j;
  const int n = 30;
  Int_t cmapping[n] = {
    1,2,3,4,5,6,7,8,
    9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,
    26,27,28,29,30,31
  };
  Int_t larmapping[n] = {
    29,23,15,7,3,11,19,26,
    28,22,14,6,2,10,18,25,
    27,21,13,5,1,9,17,24,
    20,12,4,0,8,16
  };

  // get min,max z-values
  Double_t zmin=1e6,zmax=-1e6;
  for(i=0;i<m_nPartitionLayers;i++){
    if(h[i]->GetMinimum()<zmin) zmin = h[i]->GetMinimum();
    if(h[i]->GetMaximum()>zmax) zmax = h[i]->GetMaximum();
  }

  // shift min for log-scale
  if(kLogz && zmin<=0.) zmin = 0.1;
  //printf("Object %s: Extrema: [%.1e,%.1e]\n",c->GetName(),zmin,zmax);

  TPad* subpad = nullptr;
  for(i=0;i<n;i++){
    c->cd(cmapping[i]);
    j = larmapping[i];
    h[j]->SetTitle(m_PartitionLayers[j].c_str());
    h[j]->SetStats(0);
    subpad = (TPad*)c->GetPad(cmapping[i]);
    // choose between dimensions
    if(h[j]->GetDimension()==2){ // 2D objects (eta,phi)
      h[j]->GetZaxis()->SetRangeUser(zmin,zmax); // apply scale to all partitions
      h[j]->Draw("colz");
      subpad->SetLogz(kLogz);
    } else { // 1D objects (z becomes y)
      h[j]->GetYaxis()->SetRangeUser(zmin,zmax); // apply scale to all partitions
      h[j]->Draw();
      subpad->SetLogy(kLogz);
    }
  }

  m_canvas_counts += 1;
  return c;
}

//____________________________________________________________________________________________________
void LArIdTranslatorHelper::MakeTranslatorMapping(const char* inputtreefile,const char* inputhistfile,const int run)
{
  // Merges histogramming info and mapping histograms to existing LArId TTree.
  // This needs a regular LArId TTree and one sample monitoring root file.

  Int_t i,j,k;
  int t_barrel_ec,t_pos_neg,t_emhad;
  int t_FT,t_SL,t_CH,t_sampling,t_caloindex;
  int t_Etai,t_Phii;
  float t_Etaf,t_Phif,t_Eta_Rawf,t_Phi_Rawf,t_x,t_y,t_z;
  ULong64_t t_onlid,t_offlid;
  ULong64_t t_ttid;
  std::vector<int>* t_hvid = nullptr;
  TFile* ifile = new TFile(inputtreefile);
  TTree* idtree = (TTree*)ifile->Get("LarId");
  idtree->SetBranchAddress("BEC",&t_barrel_ec);
  idtree->SetBranchAddress("Region",&t_pos_neg);
  idtree->SetBranchAddress("EmHad",&t_emhad);
  idtree->SetBranchAddress("FT",&t_FT);
  idtree->SetBranchAddress("SL",&t_SL);
  idtree->SetBranchAddress("CH",&t_CH);
  idtree->SetBranchAddress("Sampling",&t_sampling);
  idtree->SetBranchAddress("Partition",&t_caloindex);
  idtree->SetBranchAddress("I_Eta",&t_Etai);
  idtree->SetBranchAddress("I_Phi",&t_Phii);
  idtree->SetBranchAddress("F_Eta",&t_Etaf);
  idtree->SetBranchAddress("F_Phi",&t_Phif);
  idtree->SetBranchAddress("R_Eta",&t_Eta_Rawf);
  idtree->SetBranchAddress("R_Phi",&t_Phi_Rawf);
  idtree->SetBranchAddress("X",&t_x);
  idtree->SetBranchAddress("Y",&t_y);
  idtree->SetBranchAddress("Z",&t_z);
  idtree->SetBranchAddress("onlid",&t_onlid);
  idtree->SetBranchAddress("offlid",&t_offlid);
  idtree->SetBranchAddress("ttid",&t_ttid);
  idtree->SetBranchAddress("hvid",&t_hvid);

  // ------------------------------------------
  // load DQM histogram style (any file)
  TFile* fdqm = new TFile(inputhistfile);
  const Char_t* Pattern = "run_%d/CaloMonitoring/LArCellMon_NoTrigSel/2d_Occupancy/CellOccupancyVsEtaPhi_%s_4Sigma";
  Char_t name[1024];
  TH2F** h2 = new TH2F*[m_nPartitionLayers];
  for(i=0;i<m_nPartitionLayers;i++){
    sprintf(name,Pattern,run,m_PartitionLayers[i].c_str());
    h2[i] = (TH2F*)fdqm->Get(name);
    if(!h2[i]){ printf("LArIdTranslatorHelper::MakeTranslatorMapping : Could not read input histograms.\n"); exit(-1); }
  }
  // ------------------------------------------
  
  Int_t ix,iy,nbinsx,nbinsy;
  Double_t* xbins=nullptr,*ybins=nullptr;
  TH2I*** h2map = new TH2I**[m_nPartitionLayers];
  TH2I*** h2count = new TH2I**[m_nPartitionLayers];
  for(i=0;i<m_nPartitionLayers;i++){
    nbinsx = h2[i]->GetXaxis()->GetNbins();
    nbinsy = h2[i]->GetYaxis()->GetNbins();
    xbins = new Double_t[nbinsx+1];
    for(ix=0;ix<nbinsx;ix++){
      xbins[ix] = h2[i]->GetXaxis()->GetBinLowEdge(ix+1);
    }
    xbins[nbinsx] = h2[i]->GetXaxis()->GetBinUpEdge(nbinsx);
    ybins = new Double_t[nbinsy+1];
    for(iy=0;iy<nbinsy;iy++){
      ybins[iy] = h2[i]->GetYaxis()->GetBinLowEdge(iy+1);
    }
    ybins[nbinsy] = h2[i]->GetYaxis()->GetBinUpEdge(nbinsy);
    
    h2map[i] = new TH2I*[m_nHistCategories];
    h2count[i] = new TH2I*[m_nHistCategories];
    for(j=0;j<m_nHistCategories;j++){
      sprintf(name,"%s_%s",m_PartitionLayers[i].c_str(),m_HistCategories[j].c_str());
      h2map[i][j] = new TH2I(name,"",nbinsx,xbins,nbinsy,ybins);
      sprintf(name,"%s_%s_counts",m_PartitionLayers[i].c_str(),m_HistCategories[j].c_str());
      h2count[i][j] = (TH2I*)h2map[i][j]->Clone(name);

      // initialize to -1
      for(ix=0;ix<nbinsx;ix++){
        for(iy=0;iy<nbinsy;iy++) h2map[i][j]->SetBinContent(ix+1,iy+1,-1);
      }
    }
    delete [] xbins;
    delete [] ybins;
  }

  Int_t barrel_ec,region,emhad;
  Int_t FT,SL,CH,Sampling,Partition;
  Int_t iEta,iPhi;
  Float_t Eta,Phi,Eta_Raw,Phi_Raw,x,y,z;
  ULong64_t onlid,offlid,ttid;
  std::vector<int>* hvid = nullptr;
  Int_t binetadqm,binphidqm1,binphidqm2;

  // new file and TTree with appendices
  TFile* file = new TFile("LarIdTree_new.root","RECREATE"); // default, different name
  TTree* tree = new TTree("LarId","Channel IDs");
  tree->Branch("BEC",&barrel_ec);
  tree->Branch("Region",&region);
  tree->Branch("EmHad",&emhad);
  tree->Branch("FT",&FT);
  tree->Branch("SL",&SL);
  tree->Branch("CH",&CH);
  tree->Branch("Sampling",&Sampling);
  tree->Branch("Partition",&Partition);
  tree->Branch("I_Eta",&iEta);
  tree->Branch("I_Phi",&iPhi);
  tree->Branch("F_Eta",&Eta);
  tree->Branch("F_Phi",&Phi);
  tree->Branch("R_Eta",&Eta_Raw);
  tree->Branch("R_Phi",&Phi_Raw);
  tree->Branch("X",&x);
  tree->Branch("Y",&y);
  tree->Branch("Z",&z);
  tree->Branch("onlid",&onlid);
  tree->Branch("offlid",&offlid);
  tree->Branch("ttid",&ttid);
  tree->Branch("hvid",&hvid);
  tree->Branch("binetadqm",&binetadqm);
  tree->Branch("binphidqm1",&binphidqm1);
  tree->Branch("binphidqm2",&binphidqm2);

  // counters
  Int_t nchannels = 0;
  Double_t emb1PhiGran = TMath::Pi()/(Double_t)32;
  Int_t dupl = 0,dupl2 = 0;
  Int_t empt = 0;
  Int_t ntotal = 0;
  Int_t phishifts = 0;
  
  // loop over input entries
  for(Int_t iline = 0 ; iline < idtree->GetEntries() ; iline++){
    idtree->GetEntry(iline);

    // copy these variables
    barrel_ec = t_barrel_ec;
    region = t_pos_neg;
    emhad = t_emhad;
    FT = t_FT;
    SL = t_SL;
    CH = t_CH;
    Sampling = t_sampling;
    Partition = t_caloindex;
    iEta = t_Etai;
    iPhi = t_Phii;
    Eta = t_Etaf;
    Phi = t_Phif;
    x = t_x;
    y = t_y;
    z = t_z;
    Eta_Raw = t_Eta_Rawf;
    Phi_Raw = t_Phi_Rawf;
    onlid = t_onlid;
    offlid = t_offlid;
    ttid = t_ttid;
    hvid = t_hvid;
    
    // store these new variables
    binetadqm = binphidqm1 = binphidqm2 = -1;
    // hacks to make sensible 2D histos
    Double_t celleta = Eta_Raw;
    Double_t cellphi = Phi_Raw;
    Double_t phiMod = 0;

    if(Partition < 0 || Partition >= m_nPartitionLayers){
      printf("LArIdTranslatorHelper::MakeTranslatorMapping : Supplied index out of range: %d, [%d,%d]. Skipped entry %d.\n",Partition,0,m_nPartitionLayers-1,iline);
      continue;
    }
       
    // taken from CaloMonitoring
    //
    // do a little shift in eta for EMB sampling 1 to facilitate filling cheat in eta and eta phi plot
    if(Partition==1 || Partition==5){
      if( ((Eta_Raw > 1.4) && (Eta_Raw < 1.475)) || ((Eta_Raw < -1.4) && (Eta_Raw > -1.475))) {    
          //this is the eta region where we mess around:                                            
          //take phi modulus the phi granularity we have in our histogram, then shift eta accordingly:
          phiMod = fmod( fabs(Phi_Raw),emb1PhiGran);
          celleta = Eta_Raw + (phiMod * 0.2551) - 0.0125;
      }
    }
    
    // find bin indices in dqm histograms
    j = Partition;
    binetadqm = h2[j]->GetXaxis()->FindBin(celleta);
    binphidqm1 = h2[j]->GetYaxis()->FindBin(cellphi);

    // fill current bin and check for duplicates
    Bool_t success = 0;
    Double_t lodiff,updiff;

    if(h2count[j][0]->GetBinContent(binetadqm,binphidqm1)==0){ // virgin bin
      for(k=0;k<m_nHistCategories;k++) h2count[j][k]->SetBinContent(binetadqm,binphidqm1,1);
      h2map[j][0]->SetBinContent(binetadqm,binphidqm1,FT);
      h2map[j][1]->SetBinContent(binetadqm,binphidqm1,SL);
      h2map[j][2]->SetBinContent(binetadqm,binphidqm1,CH);
      h2map[j][3]->SetBinContent(binetadqm,binphidqm1,onlid);
      h2map[j][4]->SetBinContent(binetadqm,binphidqm1,iline);
      success=1;
    }

    // then add duplicates for weird phi binning in HEC-FCAL
    if(emhad>=1){
      lodiff = cellphi-h2map[j][0]->GetYaxis()->GetBinLowEdge(binphidqm1);
      updiff = cellphi-h2map[j][0]->GetYaxis()->GetBinUpEdge(binphidqm1);
      Bool_t loedge = (fabs(lodiff) < 1e-5) ? 1 : 0;
      Bool_t upedge = (fabs(updiff) < 1e-5) ? 1 : 0;

      if(loedge || upedge){ // bin edges are close enough
        if(h2count[j][0]->GetBinContent(binetadqm,binphidqm1)==1){ // nominal bin
          phishifts++;

          // try upper edge
          if(upedge){
            if(h2count[j][0]->GetBinContent(binetadqm,binphidqm1+1)==0){
              binphidqm2 = binphidqm1+1;
              for(k=0;k<m_nHistCategories;k++) h2count[j][k]->SetBinContent(binetadqm,binphidqm2,1);
              h2map[j][0]->SetBinContent(binetadqm,binphidqm2,FT);
              h2map[j][1]->SetBinContent(binetadqm,binphidqm2,SL);
              h2map[j][2]->SetBinContent(binetadqm,binphidqm2,CH);
              h2map[j][3]->SetBinContent(binetadqm,binphidqm2,onlid);
              h2map[j][4]->SetBinContent(binetadqm,binphidqm2,iline);
              success=1;
            } else success = 0;
          } else if(loedge){
            if(h2count[j][0]->GetBinContent(binetadqm,binphidqm1-1)==0){
              binphidqm2 = binphidqm1-1;
              for(k=0;k<m_nHistCategories;k++) h2count[j][k]->SetBinContent(binetadqm,binphidqm2,1);
              h2map[j][0]->SetBinContent(binetadqm,binphidqm2,FT);
              h2map[j][1]->SetBinContent(binetadqm,binphidqm2,SL);
              h2map[j][2]->SetBinContent(binetadqm,binphidqm2,CH);
              h2map[j][3]->SetBinContent(binetadqm,binphidqm2,onlid);
              h2map[j][4]->SetBinContent(binetadqm,binphidqm2,iline);
              success=1;
            } else success = 0;
          } else success = 0;
        }
        if(success) dupl+=1;
      }
    }

    // check for bins out of range
    if(binetadqm<=0 || binphidqm1<=0 || binetadqm > h2map[j][0]->GetXaxis()->GetNbins() || binphidqm1 > h2map[j][0]->GetYaxis()->GetNbins()) printf("Warning at entry %s, %d, %+.4f %+.4f, %d %d\n",m_PartitionLayers[j].c_str(),Sampling,celleta,cellphi,binetadqm,binphidqm1);

    if(!success){
      printf("LArIdTranslatorHelper::MakeTranslatorMapping : Orphan at %s %d %d (ft/sl/ch: %d %d %d) : eta,phi = %+.2f,%+.2f with bins [%+.2f,%+.2f]  [%+.2f,%+.2f]. Prevented overwrite on ft/ch/sl %d %d %d\n",
             m_PartitionLayers[j].c_str(),binetadqm,binphidqm1,FT,SL,CH,celleta,cellphi,
             h2map[j][0]->GetXaxis()->GetBinLowEdge(binetadqm),h2map[j][0]->GetXaxis()->GetBinUpEdge(binetadqm),
             h2map[j][0]->GetYaxis()->GetBinLowEdge(binphidqm1),h2map[j][0]->GetYaxis()->GetBinUpEdge(binphidqm1),
             (int)h2map[j][0]->GetBinContent(binetadqm,binphidqm1),
             (int)h2map[j][1]->GetBinContent(binetadqm,binphidqm1),
             (int)h2map[j][2]->GetBinContent(binetadqm,binphidqm1)
             );
    }

    // fill entries
    tree->Fill();
    nchannels+=1;
  }


  // out-of-loop checks
  for(i=0;i<m_nPartitionLayers;i++){
    nbinsx = h2map[i][0]->GetXaxis()->GetNbins();
    nbinsy = h2map[i][0]->GetYaxis()->GetNbins();
    ntotal += nbinsx*nbinsy;
    for(ix=0;ix<nbinsx;ix++){
      for(iy=0;iy<nbinsy;iy++){
          
        if(h2count[i][0]->GetBinContent(ix+1,iy+1)>1){
          printf("LArIdTranslatorHelper::MakeTranslatorMapping : Duplicate at %s %d %d (last ft/sl/ch: %d %d %d) : [%+.4f,%+.4f]  [%+.4f,%+.4f]\n",m_PartitionLayers[i].c_str(),ix+1,iy+1,(int)h2map[i][0]->GetBinContent(ix+1,iy+1),(int)h2map[i][1]->GetBinContent(ix+1,iy+1),(int)h2map[i][2]->GetBinContent(ix+1,iy+1),h2map[i][0]->GetXaxis()->GetBinLowEdge(ix+1),h2map[i][0]->GetXaxis()->GetBinUpEdge(ix+1),h2map[i][0]->GetYaxis()->GetBinLowEdge(iy+1),h2map[i][0]->GetYaxis()->GetBinUpEdge(iy+1));
          dupl2+=1;
        }
          
        if(h2count[i][0]->GetBinContent(ix+1,iy+1)==0) empt+=1;
      }
    }
  }
  
  // write out
  file->cd();
  tree->Write();
  for(i=0;i<m_nPartitionLayers;i++) for(j=0;j<m_nHistCategories;j++) h2map[i][j]->Write();
  file->Close(); delete file; 

  printf("-------------------------------------------------------------------\n");
  printf("LArIdTranslatorHelper::MakeTranslatorMapping: Summary:\n");
  printf("Info for %d channels (%d entries read).\n",nchannels,(int)idtree->GetEntries());
  printf("There were %d duplicates, %d empty, %d (%d) phi-shifted (HEC-FCAL), %d physical.\n",dupl2,empt,dupl,phishifts,ntotal-empt-dupl);
  printf("-------------------------------------------------------------------\n");

  ifile->Close(); delete ifile;

  return;
}

