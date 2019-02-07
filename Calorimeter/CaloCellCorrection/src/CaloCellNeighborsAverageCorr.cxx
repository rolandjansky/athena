/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************************
//
// Assign as energy the average energy of the surrounding cells (i.e. the neibhgors).
//
// Georgios Choudalakis & Guillaume Unal,   January 2009.  
//
// ****************************************************************************************

#include "CaloCellNeighborsAverageCorr.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <map> //useful in testMode

// ======================================================
// Constructor

CaloCellNeighborsAverageCorr::CaloCellNeighborsAverageCorr(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class(type, name, parent),
   m_calo_dd_man(nullptr),
   m_calo_id(nullptr),
   m_tile_id(nullptr)
{
  declareProperty("testMode",m_testMode=false,"test mode");
  declareProperty("skipDeadFeb",    m_skipDeadFeb=true,     "Skip dead LAr Febs (default = true)");
  declareProperty("skipDeadLAr",    m_skipDeadLAr=false,    "Skip all dead LAr cells (default = false)");
  declareProperty("skipDeadDrawer", m_skipDeadDrawer=false, "Skip dead Tile drawers (default = false)");
  declareProperty("skipDeadTile",   m_skipDeadTile=true,    "Skip all dead Tile cells (default = true)");
}

//========================================================
// Initialize

StatusCode CaloCellNeighborsAverageCorr::initialize()
{
  ATH_MSG_INFO ( " in CaloCellNeighborsAverageCorr::initialize() " );

  if (m_testMode)
    ATH_MSG_DEBUG ( " Running in Test Mode. " );

  // don't need to check status of the drawer if all dead cells are ignored
  if (m_skipDeadTile && m_skipDeadDrawer) {
    ATH_MSG_WARNING ( "Please, don't set skipDeadDrawer and skipDeadTile to True at the same time" );
    ATH_MSG_WARNING ( "Setting skipDeadDrawer=False" );
    m_skipDeadDrawer=false; 
  }
  
  ATH_MSG_INFO ( "Skip Dead Feb    = " << ((m_skipDeadFeb)?"true":"false")    );
  ATH_MSG_INFO ( "Skip Dead LAr    = " << ((m_skipDeadLAr)?"true":"false")    );
  ATH_MSG_INFO ( "Skip Dead Drawer = " << ((m_skipDeadDrawer)?"true":"false") );
  ATH_MSG_INFO ( "Skip Dead Tile   = " << ((m_skipDeadTile)?"true":"false")   );

  
  m_calo_dd_man  = CaloDetDescrManager::instance();
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();  
  m_tile_id   = m_calo_id->tile_idHelper();

  ATH_MSG_INFO ( "CaloCellNeighborsAverageCorr initialize() end" );
  
  return StatusCode::SUCCESS;
}

// ============================================================================

StatusCode
CaloCellNeighborsAverageCorr::process (CaloCellContainer* theCont,
                                       const EventContext& /*ctx*/) const
{
  ATH_MSG_VERBOSE ( " in process " );

  std::vector<IdentifierHash> theNeighbors;
  theNeighbors.reserve(22);
  
  std::map<const CaloCell*,float> cannedUncorrectedEnergies;
  if (m_testMode) { //fill in the map of uncorrected energies, to preserve it for later use.
    CaloCellContainer::const_iterator itrCell_test=theCont->begin();
    for (; itrCell_test!=theCont->end();++itrCell_test){
      const CaloCell* pointerToCell=*itrCell_test; //yes, this is an iterator that when dereferenced returns a pointer, not a CaloCell directly.
      cannedUncorrectedEnergies[pointerToCell] = pointerToCell->energy();
      //std::cout << "GEORGIOS DEBUGGING For CaloCell* = " << pointerToCell << " we have energy = " << cannedUncorrectedEnergies[pointerToCell] << std::endl;
    }
  }

  int nGoodCellsPerDrawer[4][64];

  if (m_skipDeadDrawer) {
    memset(nGoodCellsPerDrawer,0,sizeof(nGoodCellsPerDrawer));
      
    size_t cellItr = theCont->indexFirstCellCalo(CaloCell_ID::TILE);
    size_t lastCell = theCont->indexLastCellCalo(CaloCell_ID::TILE);

    for (; cellItr <= lastCell; ++cellItr) {
      const CaloCell* CCell = (*theCont)[cellItr];
      if (CCell && ! CCell->badcell() ) {
        Identifier id = CCell->ID();
        int part = std::min(m_tile_id->section(id),2)+m_tile_id->side(id);
        int module = m_tile_id->module(id);
        ++nGoodCellsPerDrawer[part][module];
      }
    }
    if (msgLvl(MSG::VERBOSE)) {
      //                       1-1   2-1   1+1   2+1
      const char * name[4] = {"LBC","EBC","LBA","EBA"};
      for (int part=0; part<4; ++part) {
        for (int module=0; module<64; ++module) {
          //log << MSG::VERBOSE << name[part] << module+1 << " nCells " 
          //    << nGoodCellsPerDrawer[part][module]
          //    << ( ( nGoodCellsPerDrawer[part][module] < 2 ) ? " bad drawer" : "")
          //    << endmsg;
          if ( nGoodCellsPerDrawer[part][module] < 2 ) {
            ATH_MSG_VERBOSE ( "Bad drawer " << name[part] << module+1  );
          }
        }
      }
    }
  }
  
  CaloCellContainer::iterator itrCell=theCont->begin();
  for (; itrCell!=theCont->end();++itrCell){
    CaloCell * aCell=*itrCell;
    
    if (aCell->badcell() || m_testMode) {

      const CaloDetDescrElement* caloDDE = aCell->caloDDE();
      if (!caloDDE) continue;
      int theCellSubCalo = caloDDE->getSubCalo();

      bool isTile = (theCellSubCalo == CaloCell_ID::TILE);

      if (isTile) {

        if (m_skipDeadTile) {
          ATH_MSG_VERBOSE ( " skipping Tile hash " << m_calo_id->calo_cell_hash(aCell->ID()) );
          continue;
        } else if (m_skipDeadDrawer) {

          Identifier id = aCell->ID();
          int part = std::min(m_tile_id->section(id),2)+m_tile_id->side(id);
          int module = m_tile_id->module(id);
          if ( nGoodCellsPerDrawer[part][module] < 2 ) {
            ATH_MSG_VERBOSE ( " skipping Tile drawer hash " << m_calo_id->calo_cell_hash(aCell->ID()) );
            continue;
          }
        }
        
      } else {

        if (m_skipDeadLAr) {
          ATH_MSG_VERBOSE ( " skipping LAr hash " << m_calo_id->calo_cell_hash(aCell->ID()) );
          continue;
        } else if (m_skipDeadFeb && ((aCell->provenance() & 0x0200) == 0x0200) ) {
          ATH_MSG_VERBOSE ( " skipping LAr Feb hash " << m_calo_id->calo_cell_hash(aCell->ID()) );
          continue;
        }
      }
      
      //inspired from http://alxr.usatlas.bnl.gov/lxr-stb3/source/atlas/Calorimeter/CaloRec/src/CaloTopoClusterMaker.cxx#585
      //We need a IdentifierHash to pass as input to the get_neighbors().
      //   IdentifierHash theCellHashID = theCell->getID(); //this doesn't work (any more?)
      Identifier theCellID = aCell->ID();
      const float oldE=aCell->energy();
      IdentifierHash theCellHashID = m_calo_id->calo_cell_hash(theCellID); 
      
      //Find now the neighbors around theCell, and store them in theNeighbors vector.
      m_calo_id->get_neighbours(theCellHashID,LArNeighbours::all2D,theNeighbors);
      
      //std::cout << "Found " << theNeighbors.size() << " neighbors." << std::endl;
      
      //first let's find the volume of theCell we are correcting.
      float volumeOfTheCell=0;
      if (caloDDE) volumeOfTheCell = caloDDE->volume();
      //     std::cout << " volumeOfTheCell " << volumeOfTheCell << std::endl;
      if (volumeOfTheCell==0) continue;
      //int theCellSampling = caloDDE->getSampling();
      //      if (theCellSubCalo == CaloCell_ID::TILE) {
      //      std::cout << "theCell = " << theCellSubCalo << "  " << theCellSampling  << " is at eta=" << aCell->eta() << " phi=" << aCell->phi() << " E=" << aCell->energy() << " V=" << volumeOfTheCell*1e-6 << "D=" << aCell->energy() / volumeOfTheCell * 1e6 << std::endl;
      //      }
	

      const Identifier theCellRegion=m_calo_id->region_id(theCellID);
      //loop through neighbors, and calculate average energy density of guys who have a legitimate volume (namely >0).
      //float totalEnergyDensity=0;
      //float legitimateNeighbors=0;

      float goodNeighborEnergyDensitySum=0;
      unsigned goodNeighbors=0;

      float betterNeighborEnergyDensitySum=0;
      unsigned betterNeighbors=0;

      ATH_MSG_VERBOSE("Cell " << theCellID.get_identifier32().get_compact() << " has " << theNeighbors.size() << " neighbors");
      for (unsigned int iN=0;iN<theNeighbors.size();iN++) {
	const CaloCell* thisNeighbor = theCont->findCell(theNeighbors[iN]);
	//if there is a hardware problem in real data, then the above pointer may be NULL.  In that case the geometric neighbor is absent from the container, so we should move on to the next geometric neighbor.
	if (!thisNeighbor) continue;
	const CaloDetDescrElement* thisNeighborDDE = (thisNeighbor)->caloDDE();     
	float thisEnergy = thisNeighbor->energy();
	if (m_testMode) {
	  thisEnergy = cannedUncorrectedEnergies[thisNeighbor];
	  //	  std::cout << "GEORGIOS DEBUGGING Retrieving cannedUncorrectedEnergies[" << thisNeighbor << "] = " << cannedUncorrectedEnergies[thisNeighbor] << std::endl;
	}
	if (thisNeighbor->badcell()) {
	  ATH_MSG_VERBOSE("Ignoring neighbor " << thisNeighbor->ID().get_identifier32().get_compact() << " because of it's bad cell status");
	  continue;
	}
	//int thisNeighborSubCalo = thisNeighborDDE->getSubCalo();
	//int thisNeighborSampling = thisNeighborDDE->getSampling();
	//if (thisNeighborSubCalo != theCellSubCalo) continue;
	//if (thisNeighborSampling != theCellSampling) continue; //if the quality of the cell is very different, it's a wrong idea that dE/dV would be similar.


	float thisVolume = thisNeighborDDE->volume();
	if (thisVolume <= 0) continue;

	//A good neightbor if we arrive at this point
	goodNeighbors++;
	float thisEnergyDensity= thisEnergy / thisVolume;
	goodNeighborEnergyDensitySum += thisEnergyDensity;


	const Identifier thisNeighborRegion=m_calo_id->region_id(thisNeighbor->ID());
	//Better neighbors are in the same region
	if (thisNeighborRegion==theCellRegion) {
	  ATH_MSG_VERBOSE("Neighbor is in different region " << thisNeighborRegion.get_identifier32().get_compact() << "/" << theCellRegion.get_identifier32().get_compact());
	  betterNeighbors++;
	  betterNeighborEnergyDensitySum+=thisEnergyDensity;
	}
       
	//	if (theCellSubCalo == CaloCell_ID::TILE && theCellSampling == CaloCell_ID::TileBar0)
	//	std::cout << "Neighbor " << iN << " : " << thisNeighborSubCalo << " , " << thisNeighborSampling << " : " << thisNeighbor->eta() << " , " << thisNeighbor->phi() << " E=" << thisNeighbor->energy() << " V=" << thisVolume*1e-6 << " D=" << thisEnergyDensity*1e6 << std::endl;

      } //end loop over neighbors


      unsigned nNeighbors=0;
      float neighborEnergyDensitySum=0;

      if (betterNeighbors>=2) {//Have at least 2 good neighbors in the same region
	nNeighbors=betterNeighbors;
	neighborEnergyDensitySum=betterNeighborEnergyDensitySum;
      }
      else {//No good neighbors in the same region
	ATH_MSG_DEBUG("Cell " <<  theCellID.get_identifier32().get_compact() << ": Did not find enough good neighbors in the same region. Will use neighbors from different regions");
	nNeighbors=goodNeighbors;
	neighborEnergyDensitySum=goodNeighborEnergyDensitySum;
      }

      if(nNeighbors <= 0) {
	ATH_MSG_DEBUG("Did not get any suitable neighbor for cell " << theCellID.get_identifier32().get_compact() << ". Not corrected.");
	continue;
      }

      const float averageEnergyDensity = neighborEnergyDensitySum/nNeighbors;
      
      //now use the average energy density to make a prediction for the energy of theCell
      const float predictedEnergy = averageEnergyDensity * volumeOfTheCell;
      aCell->setEnergy(predictedEnergy);
      ATH_MSG_VERBOSE ( " correcting " << ((isTile)?"Tile":"LAr") <<  " id " << theCellID.get_identifier32().get_compact() << " Eold=" 
			<<oldE << "Enew=" << predictedEnergy << ", used " << nNeighbors <<  " neighbors" );
    }  // end of if(badcell)
  }  // end loop over cells
  return StatusCode::SUCCESS;
}

//memo:
    /** From CaloCell_ID.h:    
    //enumeration of sub calorimeters
    enum SUBCALO { LAREM = 0, LARHEC = 1, LARFCAL = 2, TILE = 3, NSUBCALO = 4, NOT_VALID=999999 };

    //enumeration of samplings (i.e.layers) separately for various sub calorimeters
    enum CaloSample {
      PreSamplerB=0, EMB1, EMB2, EMB3,       // LAr barrel
      PreSamplerE, EME1, EME2, EME3,         // LAr EM endcap
      HEC0, HEC1, HEC2, HEC3,                // Hadronic end cap cal.
      TileBar0, TileBar1, TileBar2,      // Tile barrel
      TileGap1, TileGap2, TileGap3,      // Tile gap (ITC & scint)
      TileExt0, TileExt1, TileExt2,      // Tile extended barrel
      FCAL0, FCAL1, FCAL2,                   // Forward EM endcap
    Unknown
    };
    */
