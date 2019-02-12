/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     CaloCellContainerFromClusterTool
 PACKAGE:  offline/Calorimeter/CaloRec

 AUTHORS:  David Rousseau
 CREATED:  May 24,2005

 PURPOSE:  build a cell container from a calocluster collection

 ********************************************************************/

// Calo includes
#include "CaloCellContainerFromClusterTool.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerFromClusterTool::CaloCellContainerFromClusterTool(const std::string& type,
    const std::string& name, const IInterface* parent)
    : base_class(type, name, parent)
{
  declareProperty("CaloClustersName", m_caloClusterName = ""); //Backward compatibility
  declareProperty("CaloClusterNames", m_clusterKeys);
  declareProperty("CaloCellName", m_cellName = "AllCalo");
  declareProperty("AddSamplingCells", m_addSamplingCells = false);
  declareProperty("SamplingCellsName", m_samplingNames);
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerFromClusterTool::initialize() {

  //If old interface "CaloClusterName" is used, add it to the set:
  if (m_caloClusterName.size()) {
    m_clusterKeys.emplace_back (m_caloClusterName);
  }

  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Will add cells from the following cluster container(s): ";
    for (const SG::ReadHandleKey<xAOD::CaloClusterContainer>& clusterName : m_clusterKeys) 
      msg(MSG::INFO) << clusterName.key() << ", ";    

    msg(MSG::INFO) << endmsg;
  }

  // check sampling names set in properties
  if (m_addSamplingCells) {
    for (const std::string& samp : m_samplingNames) {
      if (samp == "PreSamplerB")
        m_validSamplings.insert(CaloCell_ID::PreSamplerB);
      else if (samp == "EMB1")
        m_validSamplings.insert(CaloCell_ID::EMB1);
      else if (samp == "EMB2")
        m_validSamplings.insert(CaloCell_ID::EMB2);
      else if (samp == "EMB3")
        m_validSamplings.insert(CaloCell_ID::EMB3);
      else if (samp == "PreSamplerE")
        m_validSamplings.insert(CaloCell_ID::PreSamplerE);
      else if (samp == "EME1")
        m_validSamplings.insert(CaloCell_ID::EME1);
      else if (samp == "EME2")
        m_validSamplings.insert(CaloCell_ID::EME2);
      else if (samp == "EME3")
        m_validSamplings.insert(CaloCell_ID::EME3);
      else if (samp == "HEC0")
        m_validSamplings.insert(CaloCell_ID::HEC0);
      else if (samp == "HEC1")
        m_validSamplings.insert(CaloCell_ID::HEC1);
      else if (samp == "HEC2")
        m_validSamplings.insert(CaloCell_ID::HEC2);
      else if (samp == "HEC3")
        m_validSamplings.insert(CaloCell_ID::HEC3);
      else if (samp == "TileBar0")
        m_validSamplings.insert(CaloCell_ID::TileBar0);
      else if (samp == "TileBar1")
        m_validSamplings.insert(CaloCell_ID::TileBar1);
      else if (samp == "TileBar2")
        m_validSamplings.insert(CaloCell_ID::TileBar2);
      else if (samp == "TileGap1")
        m_validSamplings.insert(CaloCell_ID::TileGap1);
      else if (samp == "TileGap2")
        m_validSamplings.insert(CaloCell_ID::TileGap2);
      else if (samp == "TileGap3")
        m_validSamplings.insert(CaloCell_ID::TileGap3);
      else if (samp == "TileExt0")
        m_validSamplings.insert(CaloCell_ID::TileExt0);
      else if (samp == "TileExt1")
        m_validSamplings.insert(CaloCell_ID::TileExt1);
      else if (samp == "TileExt2")
        m_validSamplings.insert(CaloCell_ID::TileExt2);
      else if (samp == "FCAL0")
        m_validSamplings.insert(CaloCell_ID::FCAL0);
      else if (samp == "FCAL1")
        m_validSamplings.insert(CaloCell_ID::FCAL1);
      else if (samp == "FCAL2")
        m_validSamplings.insert(CaloCell_ID::FCAL2);
      else
        ATH_MSG_ERROR( "Calorimeter sampling" << samp
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
      for (const std::string& samp : m_samplingNames) {
	msg(MSG::INFO) << " " << samp;
      }

      msg(MSG::INFO) << endmsg;
    }
  }

  ATH_CHECK( m_clusterKeys.initialize() );
  if (m_addSamplingCells) {
    ATH_CHECK( m_cellName.initialize() );
  }
  else {
    m_cellName = "";
  }

  return StatusCode::SUCCESS;
}

StatusCode
CaloCellContainerFromClusterTool::process (CaloConstCellContainer* theCont,
                                           const EventContext& ctx) const
{
  unsigned nCells=0;
  //Build bitmap to keep track which cells have been added to reducedCellContainer;
  std::bitset<200000> addedCellsMap;

  //Loop over list cluster container keys
  for (const SG::ReadHandleKey<xAOD::CaloClusterContainer>& clusKey :
         m_clusterKeys)
  {
    ATH_MSG_DEBUG( "Now working on CaloClusterContainer with key " << clusKey.key() );

    SG::ReadHandle<xAOD::CaloClusterContainer> clusterContainer (clusKey, ctx);

    //Got ClusterContainer, loop over clusters;
    for (const xAOD::CaloCluster* it_cluster : *clusterContainer) {
      const CaloClusterCellLink* cellLinks=it_cluster->getCellLinks();
      if (!cellLinks) {
        ATH_MSG_WARNING( "  Cluster without cell links found  in collection: " << clusKey.key()
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
        // if add additional cells, retrieve full cell container "AllCalo"
        SG::ReadHandle<CaloCellContainer> cellContainer (m_cellName, ctx);

        double eta = it_cluster->eta();
        double phi = it_cluster->phi();
        double deta = it_cluster->getClusterEtaSize() * 0.025;
        double dphi = it_cluster->getClusterPhiSize() * 0.025;
        // get cell lists for each sampling we want to add
        for (int isamp : m_validSamplings) {
          CaloCellList cell_list(cellContainer.cptr());
          cell_list.select(eta, phi, deta, dphi, isamp);

          ATH_MSG_DEBUG( "sampling " << isamp
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

