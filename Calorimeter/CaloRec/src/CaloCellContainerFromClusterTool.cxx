/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     CaloCellContainerFromClusterTool
 PACKAGE:  offline/Calorimeter/CaloRec

 AUTHORS:  David Rousseau
 CREATED:  May 24,2005

 PURPOSE:  build a cell container from a calocluster correction

 ********************************************************************/

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloCellContainerFromClusterTool.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerFromClusterTool::CaloCellContainerFromClusterTool(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
  , m_firstEvent(true)
{
  declareInterface<ICaloConstCellMakerTool>(this);
  declareProperty("CaloClustersName", m_caloClusterName = ""); //Backward compatibility
  declareProperty("CaloClusterNames", m_caloClusterNames);
  declareProperty("CaloCellName", m_cellName = "AllCalo");
  declareProperty("AddSamplingCells", m_addSamplingCells = false);
  declareProperty("SamplingCellsName", m_samplingNames);
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerFromClusterTool::initialize() {

  //Create a set of cluster names (clear out duplicate names):
  m_caloClusterNameSet.insert(m_caloClusterNames.begin(),m_caloClusterNames.end());
  //If old interface "CaloClusterName" is used, add it to the set:
  if (m_caloClusterName.size()) m_caloClusterNameSet.insert(m_caloClusterName);

  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Will add cells from the following cluster container(s): ";
    for (const std::string& clusterName : m_caloClusterNameSet) 
      msg(MSG::INFO) << clusterName << ", ";    

    msg(MSG::INFO) << endmsg;
  }

  // check sampling names set in properties
  if (m_addSamplingCells) {
    std::vector<std::string>::iterator samplingIter = m_samplingNames.begin();
    std::vector<std::string>::iterator samplingIterEnd = m_samplingNames.end();
    for (; samplingIter != samplingIterEnd; samplingIter++) {
      if (*samplingIter == "PreSamplerB")
        m_validSamplings.insert(CaloCell_ID::PreSamplerB);
      else if (*samplingIter == "EMB1")
        m_validSamplings.insert(CaloCell_ID::EMB1);
      else if (*samplingIter == "EMB2")
        m_validSamplings.insert(CaloCell_ID::EMB2);
      else if (*samplingIter == "EMB3")
        m_validSamplings.insert(CaloCell_ID::EMB3);
      else if (*samplingIter == "PreSamplerE")
        m_validSamplings.insert(CaloCell_ID::PreSamplerE);
      else if (*samplingIter == "EME1")
        m_validSamplings.insert(CaloCell_ID::EME1);
      else if (*samplingIter == "EME2")
        m_validSamplings.insert(CaloCell_ID::EME2);
      else if (*samplingIter == "EME3")
        m_validSamplings.insert(CaloCell_ID::EME3);
      else if (*samplingIter == "HEC0")
        m_validSamplings.insert(CaloCell_ID::HEC0);
      else if (*samplingIter == "HEC1")
        m_validSamplings.insert(CaloCell_ID::HEC1);
      else if (*samplingIter == "HEC2")
        m_validSamplings.insert(CaloCell_ID::HEC2);
      else if (*samplingIter == "HEC3")
        m_validSamplings.insert(CaloCell_ID::HEC3);
      else if (*samplingIter == "TileBar0")
        m_validSamplings.insert(CaloCell_ID::TileBar0);
      else if (*samplingIter == "TileBar1")
        m_validSamplings.insert(CaloCell_ID::TileBar1);
      else if (*samplingIter == "TileBar2")
        m_validSamplings.insert(CaloCell_ID::TileBar2);
      else if (*samplingIter == "TileGap1")
        m_validSamplings.insert(CaloCell_ID::TileGap1);
      else if (*samplingIter == "TileGap2")
        m_validSamplings.insert(CaloCell_ID::TileGap2);
      else if (*samplingIter == "TileGap3")
        m_validSamplings.insert(CaloCell_ID::TileGap3);
      else if (*samplingIter == "TileExt0")
        m_validSamplings.insert(CaloCell_ID::TileExt0);
      else if (*samplingIter == "TileExt1")
        m_validSamplings.insert(CaloCell_ID::TileExt1);
      else if (*samplingIter == "TileExt2")
        m_validSamplings.insert(CaloCell_ID::TileExt2);
      else if (*samplingIter == "FCAL0")
        m_validSamplings.insert(CaloCell_ID::FCAL0);
      else if (*samplingIter == "FCAL1")
        m_validSamplings.insert(CaloCell_ID::FCAL1);
      else if (*samplingIter == "FCAL2")
        m_validSamplings.insert(CaloCell_ID::FCAL2);
      else
        ATH_MSG_ERROR( "Calorimeter sampling" << *samplingIter
                     << " is not a valid Calorimeter sampling name and will be ignored! "
                     << "Valid names are: "
                     << "PreSamplerB, EMB1, EMB2, EMB3, "
                     << "PreSamplerE, EME1, EME2, EME3, "
                     << "HEC0, HEC1, HEC2, HEC3, "
                     << "TileBar0, TileBar1, TileBar2, "
                     << "TileGap1, TileGap2, TileGap3, "
                     << "TileExt0, TileExt1, TileExt2, "
                     << "FCAL0, FCAL1, FCAL2." );
    }

    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << "Samplings to consider for AODcellContainer:";
      samplingIter = m_samplingNames.begin();
      for (; samplingIter != samplingIterEnd; samplingIter++)
	msg(MSG::INFO) << " " << *samplingIter;

      msg(MSG::INFO) << endmsg;
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode CaloCellContainerFromClusterTool::process(CaloConstCellContainer* theCont) {
  unsigned nCells=0;
  //Build bitmap to keep track which cells have been added to reducedCellContainer;
  std::bitset<200000> addedCellsMap;
  // if add additional cells, retrieve full cell container "AllCalo"
  const CaloCellContainer* cellContainer = NULL;
  if (m_addSamplingCells) {
    ATH_CHECK( evtStore()->retrieve(cellContainer, m_cellName) );
  }

  //On first event check if all containers exist. Remove missing ones
  if (m_firstEvent) {
    m_firstEvent = false;
    std::set<std::string>::iterator it1 = m_caloClusterNameSet.begin();
    while (it1 != m_caloClusterNameSet.end()) {
      if (evtStore()->contains < xAOD::CaloClusterContainer > (*it1)) {
        ++it1; //Container found, everything ok
      } else {
        ATH_MSG_WARNING( "Cluster container with key " << *it1
            << " not found on first event. Removing from list of clusters." );

        // This is c++11.  We're compiling with c++11; however, it's not
        // safe to use this if we're linking against code compiled with
        // c++98, as is ROOT currently.
        //it1 = m_caloClusterNameSet.erase(it1);
        auto it_erase = it1;
        ++it1;
        m_caloClusterNameSet.erase (it_erase);
      }
    } // end loop over cluster container names
  }

  //Loop over list cluster container keys
  for (const std::string& clusterName : m_caloClusterNameSet) {
    ATH_MSG_DEBUG( "Now working on CaloClusterContainer with key " << clusterName );

    const xAOD::CaloClusterContainer* clusterContainer = NULL;
    CHECK(evtStore()->retrieve(clusterContainer, clusterName));

    //Got ClusterContainer, loop over clusters;
    for (auto it_cluster : (*clusterContainer)) {
      const CaloClusterCellLink* cellLinks=it_cluster->getCellLinks();
      if (!cellLinks) {
        ATH_MSG_WARNING( "  Cluster without cell links found  in collection: " << clusterName
			 << "   ===> cells cannot be written in AOD as requested ! " );
        continue;
      }

      ATH_MSG_DEBUG( "Will loop over cells" );
      
      for (const CaloCell* cell : *cellLinks) {
        //Ask cell for it's hash
        const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
        //Check it this cell is already part of reducedCellContainer
        if (!addedCellsMap.test(cellHash)) {
          theCont->push_back(cell);
          addedCellsMap.set(cellHash);
	  ++nCells;
        } else {
          ATH_MSG_VERBOSE( "Cell with hash " << cellHash << " added more than once." );
        }
      } //End loop over cells

      // add additional layer cells
      if (m_addSamplingCells) {	
        double eta = it_cluster->eta();
        double phi = it_cluster->phi();
        double deta = it_cluster->getClusterEtaSize() * 0.025;
        double dphi = it_cluster->getClusterPhiSize() * 0.025;
        std::set<int>::const_iterator vSamplingIter = m_validSamplings.begin();
        std::set<int>::const_iterator vSamplingIterEnd = m_validSamplings.end();
        // get cell lists for each sampling we want to add
        for (; vSamplingIter != vSamplingIterEnd; vSamplingIter++) {
          CaloCellList cell_list(cellContainer);
          cell_list.select(eta, phi, deta, dphi, (*vSamplingIter));

          ATH_MSG_DEBUG( "sampling " << (*vSamplingIter)
                        << ", size of list = " << cell_list.ncells()
                        << ", eta = " << eta
                        << ", phi = " << phi
                        << ", deta = " << deta
                        << ", dphi = " << dphi );


          for (const CaloCell* cell : cell_list) {
            const IdentifierHash cellHash = cell->caloDDE()->calo_hash();
            if (!addedCellsMap.test(cellHash)) {
              theCont->push_back(cell);
              ATH_MSG_DEBUG( "adding gap/tile cell " << cellHash );
              addedCellsMap.set(cellHash);
	      ++nCells;
            } else {
              ATH_MSG_VERBOSE( "Cell with hash " << cellHash << " added more than once." );
	    }
          } // end of cell loop
        } // end of sampling loop
      } // endif m_addSamplingCells
    } // End loop over clusters in container
  } // End loop over cluster containers.
  ATH_MSG_DEBUG("Added " << nCells << " cells to new CaloCellContainer");
  return StatusCode::SUCCESS;
}

