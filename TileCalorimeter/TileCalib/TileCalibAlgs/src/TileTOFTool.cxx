/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////

#include "StoreGate/StoreGate.h"
#include "StoreGate/ReadHandle.h"

#include "TileCalibAlgs/TileTOFTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "CaloIdentifier/TileID.h"

#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"

//////////////////////////////////////////////////////////////////////////////////////////////

TileTOFTool::TileTOFTool(const std::string& type, const std::string& name, const IInterface* pParent )
  : AthAlgTool( type, name, pParent ),
    m_tileID(nullptr),
    m_TimeCor(),
    m_LBA_LBC(),
    m_LBA_EBA(),
    m_LBC_EBC(),
    m_Nlbc(),
    m_Neba(),
    m_Nebc(),
    m_LA_EA(0),
    m_LA_LC(0),
    m_LA_EC(0),
    m_tcor(),
    m_Npair()
{
  declareInterface<ITileCalibTool>( this );
}

TileTOFTool::~TileTOFTool() {}

///////////////////////////////////////////////////////////

StatusCode TileTOFTool::initialize()
{
  ATH_MSG_INFO ( "initialize()" );

  ATH_CHECK( m_CaloCellContainerKey.initialize() );

  return StatusCode::SUCCESS;  
} 

StatusCode TileTOFTool::initNtuple(int runNumber, int runType, TFile * rootFile )
{
  ATH_MSG_INFO ( "initializeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" );
  return StatusCode::SUCCESS;  
}

StatusCode TileTOFTool::execute()
{
  ATH_MSG_INFO ( "execute()" );

  SG::ReadHandle<CaloCellContainer> cellCONT(m_CaloCellContainerKey);
  ATH_CHECK( cellCONT.isValid() );
  ATH_MSG_DEBUG ( "Cell container found" );

  CaloCellContainer::const_iterator iCell  = cellCONT->begin();
  CaloCellContainer::const_iterator lastCell = cellCONT->end();

  ATH_CHECK( detStore()->retrieve(m_tileID) );

  int n = 0;
  double x[20] = {0};
  double y[20] = {0};
  double z[20] = {0};
  double t[20] = {0};
  int part[20] = {0};
  int mod[20] = {0};

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

//function to calculate difference between measured time and expected TOF of the muon
#define TIMECOR(MOD_REF1, MOD_REF2, PART_REF1, PART_REF2, COUNT, TCOR)   \
  do {                                                                  \
    if (mod[i]==MOD_REF1   && mod[j]==MOD_REF2 &&                       \
        part[i]==PART_REF1 && part[j]==PART_REF2) {                     \
      float TOF = sqrt(((x[j]-x[i])*(x[j]-x[i]))+((y[j]-y[i])*(y[j]-y[i]))+((z[j]-z[i])*(z[j]-z[i])))/299.792; \
      float tc = t[j]-t[i]-TOF;                                         \
      ++COUNT;                                                          \
      TCOR += tc;                                                       \
    }                                                                   \
  } while(0)


  if(n<9){
    for(int i=0; i<n; i++) {
      for(int j=0; j<n; j++) {
	for(int s=0; s<4; s++) {
          for(int k=0; k<32; k++) {
            for(int l=0; l<32; l++) {
              TIMECOR(k,l+32,s,s, m_Npair[s][k][l], m_tcor[s][k][l]);
            }
          }
        }

        for(int s=0; s<4; s++){
          TIMECOR(15,s+46,1,0, m_Neba[s], m_LBA_EBA[s]);
          TIMECOR(15,s+46,1,2, m_Nlbc[s], m_LBA_LBC[s]);
          TIMECOR(15,s+46,2,3, m_Nebc[s], m_LBC_EBC[s]);
        }
      }
    }
  }     
    
  return StatusCode::SUCCESS;
}

StatusCode TileTOFTool::finalizeCalculations()
{
  ATH_MSG_INFO ( "finalizeCalculations()" );

// finalize calculation of the matrix

  for(int s=0; s<4; s++) {
    for(int k=0; k<32; k++) {
      for(int l=0; l<32; l++) {
        if(m_Npair[s][k][l]!=0) {
          m_tcor[s][k][l] = m_tcor[s][k][l]/m_Npair[s][k][l];
        }
      }
    }}
  

// Offsets between different partitions

  int p0 = 0, p2 = 0, p3 = 0;
  m_LA_EA = 0.;
  m_LA_LC = 0.;
  m_LA_EC = 0.;

  for(int s=0; s<4; s++){
    if(m_Neba[s]!=0) {
      m_LBA_EBA[s] = m_LBA_EBA[s]/m_Neba[s];
      if(m_Npair[0][15][s+14]>5) {
        m_LA_EA = m_LA_EA + m_tcor[0][15][s+14] - m_LBA_EBA[s];
        p0++;
      }
    }

    if(m_Nlbc[s]!=0) {
      m_LBA_LBC[s] = m_LBA_LBC[s]/m_Nlbc[s];
      if(m_Npair[2][15][s+14]>5) {
        m_LA_LC = m_LA_LC + m_tcor[2][15][s+14] - m_LBA_LBC[s];
        p2++;
      }
    }

    if(m_Nebc[s]!=0) {
      m_LBC_EBC[s] = m_LBC_EBC[s]/m_Nlbc[s];
      if(m_Npair[2][15][s+14]>5) {
        m_LA_EC = m_LA_EC + m_LA_LC + m_tcor[3][15][s+14] - m_LBC_EBC[s];
        p3++;
      }
    }
  }

   if(p0 != 0){m_LA_EA = m_LA_EA/p0; }
   if(p2 != 0){m_LA_LC = m_LA_LC/p2; } 
   if(p3 != 0){m_LA_EC = m_LA_EC/p3; }

// Calculation of time offsets wrt module 16 of each partition

   int n2[4][64] = {{0}};
   memset( m_TimeCor, 0, sizeof(m_TimeCor) );

  for(int s=0; s<4; s++){

    m_TimeCor[s][15] = 0.; n2[s][15] = 1;

    for(int k=12; k<20; k++) {
      if(m_Npair[s][15][k]>5) {
        m_TimeCor[s][k+32] = -m_tcor[s][15][k];
        n2[s][k+32]=1;
      }
    }

// Path to calculate all time offsets for modules in region x > 0

    for(int k=12; k<15; k++) {
      for(int l=16; l<20; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][k] += m_TimeCor[s][l+32] + m_tcor[s][k][l];
          n2[s][k]++;
        }
      }
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=12; k<16; k++) {
      for(int l=20; l<24; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][l+32] += m_TimeCor[s][k] - m_tcor[s][k][l];
          n2[s][l+32]++;
        }
      }
    }
    for(int k=52; k<56; k++) {
      if(n2[s][k]>0) {
        m_TimeCor[s][k] /= n2[s][k];
      }
    }

    for(int k=8; k<12; k++) { 
      for(int l=20; l<24; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][k] += m_TimeCor[s][l+32] + m_tcor[s][k][l];
          n2[s][k]++;
        }
      }
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=8; k<12; k++) {
      for(int l=24; l<28; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][l+32] += m_TimeCor[s][k] - m_tcor[s][k][l];
          n2[s][l+32]++;
        }
      }
    }
    for(int k=56; k<60; k++) {
      if(n2[s][k]>0) {
        m_TimeCor[s][k] /=n2[s][k];
      }
    }

    for(int k=0; k<8; k++) {
      for(int l=24; l<28; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][k] += m_TimeCor[s][l+32] + m_tcor[s][k][l];
          n2[s][k]++;
        }
      }
      if (n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=4; k<8; k++) {
      for(int l=28; l<32; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][l+32] += m_TimeCor[s][k] - m_tcor[s][k][l];
          n2[s][l+32]++;
        }
      }
    }
    for(int k=60; k<64; k++) {
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

// Path to calculate all time offsets for modules in region x < 0

    for(int k=16; k<20; k++) {
      for(int l=12; l<16; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][k] += m_TimeCor[s][l+32] + m_tcor[s][k][l];
          n2[s][k]++;
        }
      }
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=16; k<20; k++) {
      for(int l=8; l<12; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][l+32] += m_TimeCor[s][k] - m_tcor[s][k][l];
          n2[s][l+32]++;
        }
      }
    }
    for(int k=40; k<44; k++) {
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=20; k<24; k++){
      for(int l=8; l<12; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][k] += m_TimeCor[s][l+32] + m_tcor[s][k][l];
          n2[s][k]++;
        }
      }
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=20; k<24; k++) {
      for(int l=4; l<8; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][l+32] += m_TimeCor[s][k] - m_tcor[s][k][l];
          n2[s][l+32]++;
        }
      }
    }
    for(int k=36; k<40; k++) {
      if(n2[s][k]>0) {
        m_TimeCor[s][k] /= n2[s][k];
      }
    }
    
    for(int k=24; k<32; k++) {
      for(int l=4; l<8; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][k] += m_TimeCor[s][l+32] + m_tcor[s][k][l];
          n2[s][k]++;
        }
      }
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }

    for(int k=24; k<28; k++) {
      for(int l=0; l<4; l++) {
        if(m_Npair[s][k][l]>5) {
          m_TimeCor[s][l+32] += m_TimeCor[s][k] - m_tcor[s][k][l];
          n2[s][l+32]++;
        }
      }
    }
    for(int k=32; k<36; k++) {
      if(n2[s][k]>0)
        m_TimeCor[s][k] /= n2[s][k];
    }
    
    for(int l=0; l<64; l++) {
      ATH_MSG_INFO ( "Partition: " << s << " Module: " << l+1 << " TimeCor: " << m_TimeCor[s][l] << " n: " << n2[s][l] );
    }
  } //end of loop over different partitions

  return StatusCode::SUCCESS;  
} 

StatusCode TileTOFTool::writeNtuple(int runNumber, int runType, TFile * rootFile)
{
  ATH_MSG_INFO ( "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" );

  TTree *t = new TTree("TOF", "TOF");
  t->Branch("Time_Offset", m_TimeCor, "Time_Offset[4][64]/F");
  t->Branch("eba16_lba16", &m_LA_EA, "eba16_lba16/F");
  t->Branch("lbc16_lba16", &m_LA_LC, "lbc16_lba16/F");
  t->Branch("ebc16_lba16", &m_LA_EC, "ebc16_lba16/F");
  t->Fill();
  t->Write();

  return StatusCode::SUCCESS;  
}

StatusCode TileTOFTool::finalize() 
{
  ATH_MSG_INFO ( "finalize()" );
  return StatusCode::SUCCESS;
}
