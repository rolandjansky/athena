/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// NAME:     TileCalCellMonTool.cxx
#include "TileCalCellMonTool.h"


#include "CaloIdentifier/TileID.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "AthenaKernel/Units.h"

#include "TProfile2D.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TH1F.h"

#include <cassert>

using Athena::Units::GeV;

////////////////////////////////////////////
TileCalCellMonTool::TileCalCellMonTool(const std::string& type, const std::string& name,const IInterface* parent) 
  :CaloMonToolBase(type, name, parent),
   m_useElectronicNoiseOnly(false),
   m_useTwoGaus(true),
   m_noiseTool("CaloNoiseTool"),
   m_tile_id(nullptr)
{
  declareInterface<IMonitorToolBase>(this);

  // tools 
  declareProperty("useElectronicNoiseOnly",m_useElectronicNoiseOnly=false,"Consider only electronic noise and ignore pile-up contributiuon)");
  declareProperty("useTwoGaus", m_useTwoGaus=true,"Use double-gaussian noise description for Tile-cells");
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise Tool");

  // min energy for tilecal cells 
  declareProperty("energyThreshold",m_tileThreshold=300.); //Threshold in GeV

}



TileCalCellMonTool::~TileCalCellMonTool() {

}

////////////////////////////////////////////
StatusCode TileCalCellMonTool::initialize() {

  ATH_MSG_DEBUG("TileCalCellMonTool::initialize() start");

  ATH_CHECK( detStore()->retrieve(m_tile_id) );

  //Calonoise tool
  ATH_CHECK(m_noiseTool.retrieve());


  //Choose noise type
  if (m_useElectronicNoiseOnly) 
    m_noiseType = ICalorimeterNoiseTool::ELECTRONICNOISE;
  else
    m_noiseType=ICalorimeterNoiseTool::TOTALNOISE; 

  initParam();

  //Initialize read handle key
  ATH_CHECK( m_cellContainerName.initialize() );

  ATH_CHECK( ManagedMonitorToolBase::initialize() );
  ATH_CHECK( CaloMonToolBase::initialize() );

  ATH_MSG_DEBUG("TileCalCellMonTool::initialize() is done!");
  return StatusCode::SUCCESS;
}


void TileCalCellMonTool::initParam(){
 
  m_b_Ncells.reserve(3);
  m_b_Ncells[0]=250;
  m_b_Ncells[1]=0;
  m_b_Ncells[2]=500;
  
  m_b_CellsE.reserve(3);
  m_b_CellsE[0]=50;
  m_b_CellsE[1]=0.;
  m_b_CellsE[2]=20.;
    
  m_b_CellsEta.reserve(3);
  m_b_CellsEta[0]=100;
  m_b_CellsEta[1]=-5.0;
  m_b_CellsEta[2]=5.0;
  
  m_b_CellsEtaTile.reserve(3);
  m_b_CellsEtaTile[0]=17;
  m_b_CellsEtaTile[1]=-1.7;
  m_b_CellsEtaTile[2]=1.7;
 
  m_b_CellsPhi.reserve(3);
  m_b_CellsPhi[0]=64;
  m_b_CellsPhi[1]=-M_PI;
  m_b_CellsPhi[2]=M_PI;
  
    
  m_b_CellsEtaPhi.reserve(6);
  m_b_CellsEtaPhi[0]=17;
  m_b_CellsEtaPhi[1]=-1.7;
  m_b_CellsEtaPhi[2]=1.7;
  m_b_CellsEtaPhi[3]=64;
  m_b_CellsEtaPhi[4]=-M_PI;
  m_b_CellsEtaPhi[5]=M_PI;
    
    
  m_b_CellsModule.reserve(3);
  m_b_CellsModule[0]=70;
  m_b_CellsModule[1]=0.0;
  m_b_CellsModule[2]=70;
    
  m_b_CellsTower.reserve(3);
  m_b_CellsTower[0]=18;
  m_b_CellsTower[1]=0.0;
  m_b_CellsTower[2]=18;
  
}

StatusCode TileCalCellMonTool::bookHistograms() {

  MonGroup tileGroup(this,m_tilePath,run);

  m_h_Ncells    = new TH1F("CellsXN","Number of tile cells",(int)m_b_Ncells[0],m_b_Ncells[1], m_b_Ncells[2]);
  m_h_Ncells->GetXaxis()->SetTitle("Number" );
  m_h_Ncells->GetYaxis()->SetTitle("Events");
  tileGroup.regHist(  m_h_Ncells  ).ignore();

  m_h_CellsE    = new TH1F("CellsXE","Energy of tile cells",(int)m_b_CellsE[0],m_b_CellsE[1], m_b_CellsE[2]);
  m_h_CellsE->GetXaxis()->SetTitle("Cell Energy [GeV]");
  m_h_CellsE->GetYaxis()->SetTitle("Events");
  tileGroup.regHist(  m_h_CellsE  ).ignore();

  m_h_CellsNoiseE    = new TH1F("CellsNoiseTile","Energy/Noise(DB) of TileCal",200, -10., 10.);
  m_h_CellsNoiseE->GetXaxis()->SetTitle("Cell Energy / sigma (DB)");
  m_h_CellsNoiseE->GetYaxis()->SetTitle("Events");
  tileGroup.regHist(  m_h_CellsNoiseE  ).ignore();
 
  m_h_CellsEta    = new TProfile("CellsXEta","Tile cell energy vs Eta",(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2]);
  m_h_CellsEta->GetXaxis()->SetTitle("Eta");
  m_h_CellsEta->GetYaxis()->SetTitle("Mean energy [GeV]");
  tileGroup.regHist(  m_h_CellsEta  ).ignore();
  

  m_h_CellsEtaPhi    = new TH2F("CellsXEtaVSPhi","Nr TileCal cells above threshold",(int)m_b_CellsEtaPhi[0],m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
  m_h_CellsEtaPhi->GetXaxis()->SetTitle("Eta");
  m_h_CellsEtaPhi->GetYaxis()->SetTitle("Phi [rad]");
  m_h_CellsEtaPhi->GetXaxis()->SetNdivisions(530); 
  m_h_CellsEtaPhi->GetYaxis()->SetNdivisions(530);
  tileGroup.regHist(  m_h_CellsEtaPhi   ).ignore();


  m_h_CellsPhi    = new TProfile("CellsXPhi","Tile cell energy vs Phi",(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2]);
  m_h_CellsPhi->GetXaxis()->SetTitle("Phi");
  m_h_CellsPhi->GetYaxis()->SetTitle("Mean energy [GeV]");
  tileGroup.regHist(  m_h_CellsPhi   ).ignore();

  m_h_CellsModuleS1    = new TProfile("CellsXModuleS1","Sampling Nr 1",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
  m_h_CellsModuleS1->GetXaxis()->SetTitle("Module");
  m_h_CellsModuleS1->GetYaxis()->SetTitle("Mean cell energy [GeV]");
  tileGroup.regHist(  m_h_CellsModuleS1   ).ignore();

  m_h_CellsModuleS2    = new TProfile("CellsXModuleS2","Sampling Nr 2",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
  m_h_CellsModuleS2->GetXaxis()->SetTitle("Module");
  m_h_CellsModuleS2->GetYaxis()->SetTitle("Mean cell energy [GeV]");
  tileGroup.regHist(  m_h_CellsModuleS2   ).ignore();

  m_h_CellsModuleS3    = new TProfile("CellsXModuleS3","Sampling Nr 3",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
  m_h_CellsModuleS3->GetXaxis()->SetTitle("Module");
  m_h_CellsModuleS3->GetYaxis()->SetTitle("Mean cell energy [GeV]");
  tileGroup.regHist(  m_h_CellsModuleS3   ).ignore();


  m_h_CellsModuleS4    = new TProfile("CellsXModuleS4","Sampling Nr 4",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
  m_h_CellsModuleS4->GetXaxis()->SetTitle("Module");
  m_h_CellsModuleS4->GetYaxis()->SetTitle("Mean cell energy [GeV]");
  tileGroup.regHist(  m_h_CellsModuleS4   ).ignore();

  m_h_CellsTower    = new TProfile("CellsXTower","Towers",(int)m_b_CellsTower[0],m_b_CellsTower[1], m_b_CellsTower[2]);
  m_h_CellsTower->GetXaxis()->SetTitle("Towers");
  m_h_CellsTower->GetYaxis()->SetTitle("Mean cell energy [GeV]");
  tileGroup.regHist(  m_h_CellsTower   ).ignore();


  // tilecal
  m_h_CellsNoiseEta    = new TProfile("CellsNoiseXEta","Tile cell sigma(DB) vs Eta",(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2]);
  m_h_CellsNoiseEta->GetXaxis()->SetTitle("Eta");
  m_h_CellsNoiseEta->GetYaxis()->SetTitle("Mean cell noise (DB) [MeV]");
  tileGroup.regHist(  m_h_CellsNoiseEta  ).ignore();

  m_h_CellsNoisePhi    = new TProfile("CellsNoiseXPhi","Tile cell sigma(DB) vs Phi",(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2]);
  m_h_CellsNoisePhi->GetXaxis()->SetTitle("Phi");
  m_h_CellsNoisePhi->GetYaxis()->SetTitle("Mean cell noise (DB) [MeV]");
  tileGroup.regHist(  m_h_CellsNoisePhi   ).ignore();

  m_h_CellsNoiseEtaPhi    = new TH2F("CellsNoiseXEtaVSPhi","Nr of TileCells with E.gt.4 sigma(DB)",(int)m_b_CellsEtaPhi[0],m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
  m_h_CellsNoiseEtaPhi->GetXaxis()->SetTitle("Eta");
  m_h_CellsNoiseEtaPhi->GetYaxis()->SetTitle("Phi [rad]");
  m_h_CellsNoiseEtaPhi->GetXaxis()->SetNdivisions(530);
  m_h_CellsNoiseEtaPhi->GetYaxis()->SetNdivisions(530);
  tileGroup.regHist(  m_h_CellsNoiseEtaPhi   ).ignore();


  m_h_CellsNoiseEtaPhiA    = new TH2F("CellsNoiseXEtaVSPhiAcells","Nr of Tile cells-A with E.gt.4 sigma(DB)",(int)(2*m_b_CellsEtaPhi[0]),m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
				      (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
  m_h_CellsNoiseEtaPhiA->GetXaxis()->SetTitle("Eta");
  m_h_CellsNoiseEtaPhiA->GetYaxis()->SetTitle("Phi [rad]");
  m_h_CellsNoiseEtaPhiA->GetXaxis()->SetNdivisions(530);
  m_h_CellsNoiseEtaPhiA->GetYaxis()->SetNdivisions(530);
  tileGroup.regHist(  m_h_CellsNoiseEtaPhiA   ).ignore();


  m_h_CellsNoiseEtaPhiBC    = new TH2F("CellsNoiseXEtaVSPhiBCcells","Nr of TileCells-BC with E.gt.4 sigma(DB)",(int)(2*m_b_CellsEtaPhi[0]),m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
  m_h_CellsNoiseEtaPhiBC->GetXaxis()->SetTitle("Eta");
  m_h_CellsNoiseEtaPhiBC->GetYaxis()->SetTitle("Phi [rad]");
  m_h_CellsNoiseEtaPhiBC->GetXaxis()->SetNdivisions(530);
  m_h_CellsNoiseEtaPhiBC->GetYaxis()->SetNdivisions(530);
  tileGroup.regHist(  m_h_CellsNoiseEtaPhiBC   ).ignore();


  m_h_CellsNoiseEtaPhiD    = new TH2F("CellsNoiseXEtaVSPhiDcells","Nr of TileCells-D with E.gt.4 sigma(DB)",(int)m_b_CellsEtaPhi[0],m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
  m_h_CellsNoiseEtaPhiD->GetXaxis()->SetTitle("Eta");
  m_h_CellsNoiseEtaPhiD->GetYaxis()->SetTitle("Phi [rad]");
  m_h_CellsNoiseEtaPhiD->GetXaxis()->SetNdivisions(530);
  m_h_CellsNoiseEtaPhiD->GetYaxis()->SetNdivisions(530);
  tileGroup.regHist(  m_h_CellsNoiseEtaPhiD   ).ignore();

  // cell E vs Eta and Phi. Errors show RMS (not error on the mean) 
  m_h_CellsRMSEta    = new TProfile("CellsRMSXEta","Tile cell energy with RMS errors for Eta",(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2],"s");
  m_h_CellsRMSEta->GetXaxis()->SetTitle("Eta");
  m_h_CellsRMSEta->GetYaxis()->SetTitle("Mean cell energy+RMS [GeV]");
  tileGroup.regHist(  m_h_CellsRMSEta  ).ignore();

  m_h_CellsRMSPhi    = new TProfile("CellsRMSXPhi","Tile cell energy with RMS errors vs Phi",(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2],"s");
  m_h_CellsRMSPhi->GetXaxis()->SetTitle("Phi");
  m_h_CellsRMSPhi->GetYaxis()->SetTitle("Mean cell energy+RMS [GeV]");
  tileGroup.regHist(  m_h_CellsRMSPhi   ).ignore();


  char name[256];
  char title[256];
  sprintf(name,"%s_%s",(char*)m_h_CellsRMSEta->GetName(),"divNoiseDB");
  sprintf(title,"%s - %s",(char*)m_h_CellsRMSEta->GetTitle(),"divNoiseDB");
  m_h_CellsRMSdivDBnoiseEta = new TH1F(name,title,(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2]);
  m_h_CellsRMSdivDBnoiseEta->SetNameTitle(name,title);
  m_h_CellsRMSdivDBnoiseEta ->GetXaxis()->SetTitle("Eta");
  m_h_CellsRMSdivDBnoiseEta ->GetYaxis()->SetTitle("Energy cell RMS / Cell noise (DB)");
  tileGroup.regHist(  m_h_CellsRMSdivDBnoiseEta  ).ignore();

  sprintf(name,"%s_%s",(char*)m_h_CellsRMSPhi->GetName(),"divNoiseDB");
  sprintf(title,"%s - %s",(char*)m_h_CellsRMSPhi->GetTitle(),"divNoiseDB");
  m_h_CellsRMSdivDBnoisePhi = new TH1F(name,title,(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2]);
  m_h_CellsRMSdivDBnoisePhi->SetNameTitle(name,title);
  m_h_CellsRMSdivDBnoisePhi ->GetXaxis()->SetTitle("Phi");
  m_h_CellsRMSdivDBnoisePhi ->GetYaxis()->SetTitle("Energy cell RMS / Cell noise (DB)");
  tileGroup.regHist(  m_h_CellsRMSdivDBnoisePhi  ).ignore();

  m_h_CellsNoiseHash    = new TH1F("CellsXNoiseXHash","Number of 4 sigma seeds per hash",5184,-0.5, 5183.5);
  m_h_CellsNoiseHash->GetXaxis()->SetTitle("HashId" );
  m_h_CellsNoiseHash->GetYaxis()->SetTitle("Events");
  tileGroup.regHist(  m_h_CellsNoiseHash  ).ignore();

  //    ATH_MSG_INFO("end of bookTileHists()");


  MonGroup summaryGroup(this,m_tilePath+"/Summary",run);
  ATH_CHECK(bookBaseHists(&summaryGroup)); //from base class
  return StatusCode::SUCCESS;
}
 
////////////////////////////////////////////////////////////////////////////
StatusCode TileCalCellMonTool::fillHistograms(){  

//  ATH_MSG_INFO("TileCalCellMonTool::fillHistograms() starts");
  StatusCode sc = StatusCode::SUCCESS;

  bool ifPass = 1;
  sc = checkFilters(ifPass);
  if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;


  //=====================
  // CaloCell info
  //============================

  SG::ReadHandle<CaloCellContainer> cellContHandle{m_cellContainerName};
  if (! cellContHandle.isValid()) { ATH_MSG_WARNING("No CaloCell container found in TDS"); return StatusCode::FAILURE; }
  const CaloCellContainer* cellCont = cellContHandle.get();


  CaloCellContainer::const_iterator it = cellCont->beginConstCalo(CaloCell_ID::TILE);
  CaloCellContainer::const_iterator it_e = cellCont->endConstCalo(CaloCell_ID::TILE);
  // loop over cells -------------
  unsigned ncells=0;
  for ( ; it!=it_e;++it) { 
    // cell info
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    IdentifierHash hash = m_tile_id->cell_hash(id);
    long sample=m_tile_id->sample(id);
    long module=m_tile_id->module(id);
    long tower=m_tile_id->tower(id);
    int side=m_tile_id->side(id); 
    float cellen = cell->energy();
    float cellenGeV=cellen/GeV;
    double eta = cell->eta();
    double phi = cell->phi();
    bool   badc=cell->badcell();
    float cellnoisedb = 0.;


    if (m_useTwoGaus==false)
      cellnoisedb = m_noiseTool->getNoise(cell,m_noiseType);
    else
      cellnoisedb = m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,m_noiseType);

    double rs=999;
    if (std::isfinite(cellnoisedb) && cellnoisedb > 0 &&  cellen != 0 && badc == false) rs= cellen / cellnoisedb;

    if (sample < 3) { // Sample E not relevant for topoclusters
      if (badc == false && rs != 999 ) m_h_CellsNoiseE->Fill( rs );
      if (fabs(rs)>4.0 && badc == false && rs != 999 ) {
	m_h_CellsNoiseEtaPhi->Fill( eta, phi);
	m_h_CellsNoiseHash->Fill(hash);
	if (sample ==0 ) m_h_CellsNoiseEtaPhiA->Fill(  eta, phi  );
	else if (sample ==1 ) m_h_CellsNoiseEtaPhiBC->Fill(  eta, phi );
	else  m_h_CellsNoiseEtaPhiD->Fill(  eta, phi  );

	ATH_MSG_DEBUG( "TileCalCellMonTool: Cell rs=" << rs << " e=" << cellen << " eta=" << eta << " phi="<< phi  );
	ATH_MSG_DEBUG( "TileCalCellMonTool: hash=" << hash << " module= " << module+1 << " sample=" << sample
		       << " tower=" << side*tower );
      }// end of if (fabs(rs)>4.0 && badc == false && rs != 999 )

      if ( badc == false) {
	m_h_CellsNoiseEta->Fill(  eta, cellnoisedb, 1.0  );
	m_h_CellsNoisePhi->Fill(  phi, cellnoisedb, 1.0  );
	m_h_CellsRMSEta->Fill(  eta, cellenGeV, 1.0  );
	m_h_CellsRMSPhi->Fill(  phi, cellenGeV, 1.0  );
      }
    } // end of if (sample < 3)

    if (cellen > m_tileThreshold) {
      ncells++;
      m_h_CellsE->Fill(   cellenGeV  );
      m_h_CellsEta->Fill(  eta, cellenGeV, 1.0  );
      m_h_CellsPhi->Fill(  phi, cellenGeV, 1.0  );
      if (sample ==0 ) m_h_CellsModuleS1->Fill(  module+1, cellenGeV  );
      if (sample ==1 ) m_h_CellsModuleS2->Fill(  module+1, cellenGeV  );
      if (sample ==2 ) m_h_CellsModuleS3->Fill(  module+1, cellenGeV  );
      if (sample ==3 ) m_h_CellsModuleS4->Fill(  module+1, cellenGeV  );
      m_h_CellsTower->Fill(  tower+1, cellenGeV  );
      m_h_CellsEtaPhi->Fill( eta, phi );
    } //end if cell energy above threshold  
  } // end of it loop 
  m_h_Ncells->Fill( (float) ncells ); // count TileCal cells above threshold
  
  return StatusCode::SUCCESS;
}

StatusCode TileCalCellMonTool::procHistograms() {
  getRMSdevNoise(m_h_CellsRMSEta,m_h_CellsNoiseEta,m_h_CellsRMSdivDBnoiseEta);
  getRMSdevNoise(m_h_CellsRMSPhi,m_h_CellsNoisePhi,m_h_CellsRMSdivDBnoisePhi);
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////
// helpers for process histograms 


/* The aim of this function is calculate RMS/NoiseDB histogram from 2 profile
 *  histograms. 
 */ 
void TileCalCellMonTool::getRMSdevNoise(TProfile *t1, TProfile *t2, TH1F *t3) { 
  // t1 histogram with RMS as errors
  // t2 histogram with noise DB
  // resulting histogram 

  //Check if the histograms are actually defined, if not, return:
  if( (!t1) || (!t2) || (!t3) ) {
    return;
  }

  int nb=t1->GetNbinsX();
  for(int ix=1;ix<nb+1;ix++){
    Double_t ierr1=(t1->GetBinError(ix))*1000; // convert to MeV 
    Double_t ent = t1->GetBinEntries(ix);
    Double_t ierr2=t2->GetBinError(ix);
    Double_t ienn2=t2->GetBinContent(ix);
    double Rat=0.0;
    if (ienn2>0) Rat=ierr1/ienn2; // ratio: RMS/noiseDB  
    double er1=0;
    if (ent>0) er1=ierr1/sqrt(ent); // error on RMS 
    double er2=0;
    if (ienn2>0) er2=ierr2/ienn2;
    double Err=Rat*sqrt( (er1*er1) + (er2*er2) );
    t3->SetBinContent(ix,Rat);
    t3->SetBinError(ix,Err);
  }
  t3->SetEntries( nb );
  t3->SetStats(0);
}
 
