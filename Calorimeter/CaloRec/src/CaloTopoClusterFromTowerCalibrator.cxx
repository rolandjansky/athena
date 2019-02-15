/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#include "CaloRec/CaloTopoClusterFromTowerCalibrator.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"

#include "CaloEvent/CaloCellClusterWeights.h"

#include <numeric>

namespace {                                                                             /////////////////////////////////////////////////////////////
  double accumulateWeight(const CaloCellClusterWeights::weight_t& wght)                 // A cell can be split across clusters (usually only two). //
  { return wght.empty()                                                                 // It thus receives two different calibration weights,     //
      ? 0.                                                                              // including geometrical weight introduced by splitting.   //
      : wght.size() == 1                                                                // The overall weight is the sum of the two.               //
      ? wght.front()                                                                    /////////////////////////////////////////////////////////////
      : std::accumulate(wght.begin(),wght.end(),0.);
  }
}

CaloTopoClusterFromTowerCalibrator::CaloTopoClusterFromTowerCalibrator(const std::string& type,const std::string& name,const IInterface* pParent)
  : AthAlgTool(type,name,pParent)
  , m_cellClusterWeightKey("CaloTopoClusterCellWeights")
  , m_orderByPt(false)
{ 
  declareInterface<CaloClusterCollectionProcessor>(this);
  declareProperty("CellClusterWeightKey",m_cellClusterWeightKey,"SG Key for CellClusterWeights (input)");
  declareProperty("OrderClusterByPt",    m_orderByPt,           "Order clusters by calibrated Pt (input)");
}

StatusCode CaloTopoClusterFromTowerCalibrator::initialize()
{ 
  ATH_CHECK(m_cellClusterWeightKey.initialize()); 
  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterFromTowerCalibrator::execute(xAOD::CaloClusterContainer* pClusCont)
{
  // retrieve weights
  SG::ReadHandle<CaloCellClusterWeights> pCellWeights(m_cellClusterWeightKey);
  if ( !pCellWeights.isValid() ) { 
    ATH_MSG_ERROR( "Cannot allocate CaloCellClusterWeights with key <" << m_cellClusterWeightKey.key() << ">" );
    return StatusCode::FAILURE;
  }

  /////////////////////////
  // Calibrated clusters //
  /////////////////////////

  // loop clusters                                                                 ////////////////////////////////////////////////////////////
  for ( auto pClus : *pClusCont ) {                                                // This code does not change the cluster kinematics.      //
    // loop cells in clusters                                                      // It only modifies the weight of the cell signals.       //
    for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) { ////////////////////////////////////////////////////////////
      const CaloCellClusterWeights::weight_t& wght(pCellWeights->at(*fCell));      // Retrieve list of weights associated with this cluster. //
      double weight(fCell.weight());                                               // Get cell-in-tower weight.                              //
      weight *= accumulateWeight(wght);                                            // Combine with calibration weights.                      //
      pClus->reweightCell(fCell,weight);                                           // Set new weight.                                        //
    }                                                                              ////////////////////////////////////////////////////////////
    // preserve raw (EM) kinematics
    double rawE(pClus->e());                       ////////////////////////////////////////////////////////////////////////////////////////////
    double rawEta(pClus->eta());                   // Clusters are instantiated with signal state CALIBRATED. The kinematics prior to the    //
    double rawPhi(pClus->phi());                   // application of LCW is EM. This code saves the EM scale kinematics.                     //
    double rawM(pClus->m());                       ////////////////////////////////////////////////////////////////////////////////////////////
    // update cluster kinematics
    CaloRec::Helpers::calculateKine(pClus,true);   ////////////////////////////////////////////////////////////////////////////////////////////
    pClus->setRawE(rawE);                          // The calculateKine function recombines cell signals with the updated weights (combined  //
    pClus->setRawEta(rawEta);                      // LCW and tower geometry weights). After this function, the CALIBRATED state reflects    //
    pClus->setRawPhi(rawPhi);                      // these combined weights. The EM scale kinematics is restored as raw (UNCALIBRATED).     //
    pClus->setRawM(rawM);                          ////////////////////////////////////////////////////////////////////////////////////////////
  } // cluster loop

  // order clusters by Pt if requested
  if ( m_orderByPt ) { 
    std::sort(pClusCont->begin(),pClusCont->end(),[](xAOD::CaloCluster* pc1,xAOD::CaloCluster* pc2) {
        volatile double pt1(pc1->pt());
        volatile double pt2(pc2->pt());
   	return ( pt1 > pt2 );  
      }
      );
  }

  return StatusCode::SUCCESS;
}

