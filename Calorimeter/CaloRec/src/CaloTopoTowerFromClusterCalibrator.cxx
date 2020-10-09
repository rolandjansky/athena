/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#include "CaloTopoTowerFromClusterCalibrator.h"
#include "CaloTopoClusterFromTowerHelpers.h"

#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloGeoHelpers/CaloSampling.h"

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

CaloTopoTowerFromClusterCalibrator::CaloTopoTowerFromClusterCalibrator(const std::string& type,const std::string& name,const IInterface* pParent)
  : AthAlgTool(type,name,pParent)
{ 
  declareInterface<CaloTowerCollectionProcessor>(this);
  declareProperty("OrderClusterByPt",    m_orderByPt,           "Order clusters by calibrated Pt (input)");
}

StatusCode CaloTopoTowerFromClusterCalibrator::execute(const EventContext& /*ctx*/,
						       xAOD::CaloClusterContainer* pClusCont, 
						       CaloCellClusterWeights* cellWeights) const
{

  // protection needed?
  if ( cellWeights == nullptr ) { 
    ATH_MSG_ERROR("Tower calibrator needs valid pointer to LCW weight lookup");
    return StatusCode::FAILURE;
  }

  /////////////////////////
  // Calibrated clusters //
  /////////////////////////

  // loop towers                                                                   
  for ( auto pClus : *pClusCont ) {                                                
    // loop cells in towers                                                        
    for ( auto fCell(pClus->cell_begin()); fCell != pClus->cell_end(); ++fCell ) {
      // pick up the LCW weights (one if cell is only in one topo-cluster, two if cell is split  between two) -> w_cell 
      const CaloCellClusterWeights::weight_t& wght(cellWeights->at(*fCell));
      // pick up geometrical weight of cells in tower       
      double weight(fCell.weight());
      // combine the weights -> w_geo * w_cell                                               
      weight *= accumulateWeight(wght);       
      if ( weight == 0. ) { 
	// weight = 0 should actualy never happen!!!!                                     
	ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("[NO_LCW_REWEIGHT] Tower (%6.3f,%6.3f) cell [%6zu] weight = %6.3f [# LCW weights %zu geo %6.3f LCW %6.3f] SamplingID %2u Name \042%s\042",
						pClus->eta(),pClus->phi(),(size_t)fCell->caloDDE()->calo_hash(),weight,wght.size(),fCell.weight(),weight/std::max(fCell.weight(),1e-08),
						(unsigned int)fCell->caloDDE()->getSampling(),CaloSampling::getSamplingName(fCell->caloDDE()->getSampling()).c_str()) );
      } else {
	// valid weight, apply to cell in tower
	ATH_MSG_DEBUG(   CaloRec::Helpers::fmtMsg("[DO_LCW_REWEIGHT] Tower (%6.3f,%6.3f) cell [%6zu] weight = %6.3f [# LCW weights %zu geo %6.3f LCW %6.3f]",
						  pClus->eta(),pClus->phi(),(size_t)fCell->caloDDE()->calo_hash(),weight,wght.size(),fCell.weight(),weight/fCell.weight()) );
	pClus->reweightCell(fCell,weight);                                         // Set new weight.                                        //
      }
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

