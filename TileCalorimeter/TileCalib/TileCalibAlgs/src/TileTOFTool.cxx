/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGate.h"

#include "TileCalibAlgs/TileTOFTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "CaloIdentifier/TileID.h"

#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"

//////////////////////////////////////////////////////////////////////////////////////////////

TileTOFTool::TileTOFTool(const std::string& type, const std::string& name, const IInterface* pParent ): AlgTool( type, name, pParent ) {
	   declareInterface<ITileCalibTool>( this );
           
  /// Declare the properties
}

TileTOFTool::~TileTOFTool() {}

///////////////////////////////////////////////////////////

StatusCode TileTOFTool::initialize()
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize()" << endreq;

  if(service("DetectorStore", m_detStore).isFailure()){log << MSG::ERROR << "Unable to get pointer to DetectorStore Service" << endreq; return StatusCode::FAILURE;}
  if(service("StoreGateSvc", m_gatStore).isFailure()){log << MSG::ERROR << "Unable to get pointer to DetectorStore Service" << endreq; return StatusCode::FAILURE;}

  memset( tcor, '\0', sizeof(tcor[0][0][0])*4*32*32 );
  memset( Npair, '\0', sizeof(Npair[0][0][0])*4*32*32 );
  memset( LBA_EBA, '\0', sizeof(LBA_EBA[0])*4);
  memset( LBA_LBC, '\0', sizeof(LBA_LBC[0])*4);
  memset( LBC_EBC, '\0', sizeof(LBC_EBC[0])*4);
  memset( Neba, '\0', sizeof(Neba[0])*4);
  memset( Nlbc, '\0', sizeof(Nlbc[0])*4);
  memset( Nebc, '\0', sizeof(Nebc[0])*4);

  return StatusCode::SUCCESS;  
} 

StatusCode TileTOFTool::initNtuple(int runNumber, int runType, TFile * rootFile )
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initializeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;
  return StatusCode::SUCCESS;  
}

StatusCode TileTOFTool::execute()
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "execute()" << endreq;

  const CaloCellContainer* cellCONT;
  StatusCode  sc = m_gatStore->retrieve( cellCONT, "AllCalo");

  if( sc.isFailure()  ||  !cellCONT )
    {
      log << MSG::FATAL << "No cell container found" << endreq;
      return StatusCode::SUCCESS;
    }

  else{log << MSG::DEBUG << "Cell container found" << endreq;}

  CaloCellContainer::const_iterator iCell  = cellCONT->begin();
  CaloCellContainer::const_iterator lastCell = cellCONT->end();

  sc = m_detStore->retrieve(m_tileID);

  if (sc.isFailure())
    {
      log << MSG::ERROR << "Unable to retrieve TileID helper from DetectorStore" << endreq;
      return sc;
    }

  n=0;
  memset( x, '\0', sizeof(x[0])*9 );
  memset( y, '\0', sizeof(y[0])*9 );
  memset( z, '\0', sizeof(z[0])*9 );
  memset( t, '\0', sizeof(t[0])*9 );
  memset( mod, '\0', sizeof(mod[0])*9 );
  memset( part, '\0', sizeof(part[0])*9 );

  int section;
  int side;

  for( ; iCell != lastCell; ++iCell)  // loop over Calo cells
    {
      const CaloCell* cell_ptr = *iCell;     // pointer to cell object

      if ( m_tileID->is_tile(cell_ptr->ID()) )
        {
          const TileCell* tile_cell = dynamic_cast<const TileCell*> (cell_ptr);
	  
          if ((tile_cell!=0)&&((tile_cell->ene1())>150.)&&((tile_cell->ene2())>150.)&&((tile_cell->energy())<5000.)&&(fabs(tile_cell->timeDiff())<3.5)&&(fabs(tile_cell->time())>0.00000001)) // cut to select TileCal cells crossed by muons
            {
              Identifier id = tile_cell->ID();

	      x[n] = tile_cell->x();
	      y[n] = tile_cell->y();
	      z[n] = tile_cell->z();
	      t[n] = tile_cell->time();
	      mod[n] = m_tileID->module(id);

	      section = m_tileID->section(id);
	      side = m_tileID->side(id);
	      
	      if((section == 2 || section ==3) && side == 1){part[n] = 0;} 		//EBA 
	      else if(section == 1 && side == 1){part[n] = 1;} 	           	        //LBA
	      else if(section == 1 && side == -1){part[n] = 2;} 			//LBC
	      else if((section == 2 || section ==3) && side == -1){part[n] = 3;}	//EBC   
	      n++;
	      if(n==9){break;}		// events with air showers not considered
            }
	}
    }
  
// calculate a matrix for each partition with the time offset of each module at the bottom side of TileCal wrt each module at the bottom

  if(n<9){
    for(i=0; i<n; i++)
     {
      for(j=0; j<n; j++)
       {
	for(int s=0; s<4; s++){for(int k=0; k<32; k++){for(int l=0; l<32; l++){
	 if((timeCor(k,l+32,s,s))<1000000.){Npair[s][k][l]++; tcor[s][k][l] = tcor[s][k][l] + timeCor(k,l+32,s,s);}}}}

         for(int s=0; s<4; s++){
          if((timeCor(15,s+46,1,0))<1000000.){
	    LBA_EBA[s] = LBA_EBA[s] + timeCor(15,s+46,1,0); Neba[s]++;
	  }
          if((timeCor(15,s+46,1,2))<1000000.){
	    LBA_LBC[s] = LBA_LBC[s] + timeCor(15,s+46,1,2); Nlbc[s]++; 
	  }
          if((timeCor(15,s+46,2,3))<1000000.){LBC_EBC[s] = LBC_EBC[s] + timeCor(15,s+46,2,3); Nebc[s]++;}}
       }
     }}     
    
  return StatusCode::SUCCESS;
}

float TileTOFTool::timeCor(int mod_ref1, int mod_ref2, int part_ref1, int part_ref2)
{
  //function to calculate difference between measured time and expected TOF of the muon

  float tc = 1000001.; float TOF;

  if((mod[i]==mod_ref1)&&(mod[j]==mod_ref2)&&(part[i]==part_ref1)&&(part[j]==part_ref2)){
   TOF = sqrt(((x[j]-x[i])*(x[j]-x[i]))+((y[j]-y[i])*(y[j]-y[i]))+((z[j]-z[i])*(z[j]-z[i])))/299.792;
   tc = t[j]-t[i]-TOF;}

  return(tc);
}

StatusCode TileTOFTool::finalizeCalculations()
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalizeCalculations()" << endreq;

// finalize calculation of the matrix

  for(int s=0; s<4; s++){for(int k=0; k<32; k++){for(int l=0; l<32; l++){
   if(Npair[s][k][l]!=0){tcor[s][k][l] = tcor[s][k][l]/Npair[s][k][l];}}}} 

// Offsets between different partitions

  int p0 = 0, p2 = 0, p3 = 0; LA_EA = 0.; LA_LC = 0.; LA_EC = 0.;

  for(int s=0; s<4; s++){
   if(Neba[s]!=0){LBA_EBA[s] = LBA_EBA[s]/Neba[s]; if(Npair[0][15][s+14]>5){LA_EA = LA_EA + tcor[0][15][s+14] - LBA_EBA[s]; p0++;}}

   if(Nlbc[s]!=0){LBA_LBC[s] = LBA_LBC[s]/Nlbc[s]; if(Npair[2][15][s+14]>5){LA_LC = LA_LC + tcor[2][15][s+14] - LBA_LBC[s]; p2++;}}

   if(Nebc[s]!=0){LBC_EBC[s] = LBC_EBC[s]/Nlbc[s]; if(Npair[2][15][s+14]>5){LA_EC = LA_EC + LA_LC + tcor[3][15][s+14] - LBC_EBC[s]; p3++;}}}

   if(p0 != 0){LA_EA = LA_EA/p0; }
   if(p2 != 0){LA_LC = LA_LC/p2; } 
   if(p3 != 0){LA_EC = LA_EC/p3; }

// Calculation of time offsets wrt module 16 of each partition

  int n2[4][64];
  memset( n2, '\0', sizeof(n2[0][0])*4*64 );
  memset( TimeCor, '\0', sizeof(TimeCor[0][0])*4*64 );

  for(int s=0; s<4; s++){

  TimeCor[s][15] = 0.; n2[s][15] = 1;

  for(int k=12; k<20; k++){if(Npair[s][15][k]>5){TimeCor[s][k+32] = -tcor[s][15][k]; n2[s][k+32]=1;}}

// Path to calculate all time offsets for modules in region x > 0

  for(int k=12; k<15; k++){for(int l=16; l<20; l++){ 
   if(Npair[s][k][l]>5){TimeCor[s][k] = TimeCor[s][k] + TimeCor[s][l+32] + tcor[s][k][l]; n2[s][k]++;}}}
  for(int k=12; k<15; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=12; k<16; k++){for(int l=20; l<24; l++){
   if(Npair[s][k][l]>5){TimeCor[s][l+32] = TimeCor[s][l+32] + TimeCor[s][k] - tcor[s][k][l]; n2[s][l+32]++;}}}
  for(int k=52; k<56; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=8; k<12; k++){for(int l=20; l<24; l++){
   if(Npair[s][k][l]>5){TimeCor[s][k] = TimeCor[s][k] + TimeCor[s][l+32] + tcor[s][k][l]; n2[s][k]++;}}}
  for(int k=8; k<12; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=8; k<12; k++){for(int l=24; l<28; l++){
   if(Npair[s][k][l]>5){TimeCor[s][l+32] = TimeCor[s][l+32] + TimeCor[s][k] - tcor[s][k][l]; n2[s][l+32]++;}}}
  for(int k=56; k<60; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=0; k<8; k++){for(int l=24; l<28; l++){
   if(Npair[s][k][l]>5){TimeCor[s][k] = TimeCor[s][k] + TimeCor[s][l+32] + tcor[s][k][l]; n2[s][k]++;}}}
  for(int k=0; k<8; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=4; k<8; k++){for(int l=28; l<32; l++){
   if(Npair[s][k][l]>5){TimeCor[s][l+32] = TimeCor[s][l+32] + TimeCor[s][k] - tcor[s][k][l]; n2[s][l+32]++;}}}
  for(int k=60; k<64; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

// Path to calculate all time offsets for modules in region x < 0

  for(int k=16; k<20; k++){for(int l=12; l<16; l++){
   if(Npair[s][k][l]>5){TimeCor[s][k] = TimeCor[s][k] + TimeCor[s][l+32] + tcor[s][k][l]; n2[s][k]++;}}}
  for(int k=16; k<20; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=16; k<20; k++){for(int l=8; l<12; l++){
   if(Npair[s][k][l]>5){TimeCor[s][l+32] = TimeCor[s][l+32] + TimeCor[s][k] - tcor[s][k][l]; n2[s][l+32]++;}}}
  for(int k=40; k<44; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=20; k<24; k++){for(int l=8; l<12; l++){
   if(Npair[s][k][l]>5){TimeCor[s][k] = TimeCor[s][k] + TimeCor[s][l+32] + tcor[s][k][l]; n2[s][k]++;}}}
  for(int k=20; k<24; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=20; k<24; k++){for(int l=4; l<8; l++){
   if(Npair[s][k][l]>5){TimeCor[s][l+32] = TimeCor[s][l+32] + TimeCor[s][k] - tcor[s][k][l]; n2[s][l+32]++;}}}
  for(int k=36; k<40; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=24; k<32; k++){for(int l=4; l<8; l++){
   if(Npair[s][k][l]>5){TimeCor[s][k] = TimeCor[s][k] + TimeCor[s][l+32] + tcor[s][k][l]; n2[s][k]++;}}}
  for(int k=24; k<32; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int k=24; k<28; k++){for(int l=0; l<4; l++){
   if(Npair[s][k][l]>5){TimeCor[s][l+32] = TimeCor[s][l+32] + TimeCor[s][k] - tcor[s][k][l]; n2[s][l+32]++;}}}
  for(int k=32; k<36; k++){if(n2[s][k]>0){TimeCor[s][k] = TimeCor[s][k]/n2[s][k];}}

  for(int l=0; l<64; l++)
    {
      log << MSG::INFO << "Partition: " << s << " Module: " << l+1 << " TimeCor: " << TimeCor[s][l] << " n: " << n2[s][l] << endreq;
    }	
  } //end of loop over different partitions

  return StatusCode::SUCCESS;  
} 

StatusCode TileTOFTool::writeNtuple(int runNumber, int runType, TFile * rootFile)
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;

  TTree *t = new TTree("TOF", "TOF");
  t->Branch("Time_Offset", TimeCor, "Time_Offset[4][64]/F");
  t->Branch("eba16_lba16", &LA_EA, "eba16_lba16/F");
  t->Branch("lbc16_lba16", &LA_LC, "lbc16_lba16/F");
  t->Branch("ebc16_lba16", &LA_EC, "ebc16_lba16/F");
  t->Fill();
  t->Write();

  return StatusCode::SUCCESS;  
}

StatusCode TileTOFTool::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;
  return StatusCode::SUCCESS;
}
