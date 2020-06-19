/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwGap_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  L. Carminati
CREATED:  Feb 2003

PURPOSE:  Correction for the barrel-endcap cracks
          Tuned using 100 GeV photons

UPDATED:  10/04/03   MW
          bug fix to take only the cells in near cluster for gap
          correction

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/

#include "CaloSwGap_g3.h"
                   
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "GaudiKernel/StatusCode.h"
 

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <iostream>
#include <math.h>

using xAOD::CaloCluster;
using CLHEP::pi;
using CLHEP::twopi;

namespace {

const double deta = 0.2;
const double dphi = twopi / 64. ;

}

/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloSwGap_g3::initialize()
{
  ATH_CHECK( CaloClusterCorrection::initialize() );
  ATH_CHECK( m_cells_name.initialize() );
  return StatusCode::SUCCESS;
}


void CaloSwGap_g3::makeCorrection (const Context& myctx,
                                   CaloCluster* cluster) const
{
  float eta = cluster->eta();
  float phi = cluster->phi();

  float aeta = fabs(eta);

  const float etamin_crack = m_etamin_crack (myctx);
  const float etamax_crack = m_etamax_crack (myctx);
  if (aeta < etamin_crack || aeta > etamax_crack) 
    return; // no correction required

  const CxxUtils::Array<1> scint_weight = m_scint_weight (myctx);
  const CxxUtils::Array<1> correction = m_correction (myctx);
  assert (scint_weight.size() == correction.size());
  if (correction.size() == 0) return;

  SG::ReadHandle<CaloCellContainer> cc (m_cells_name, myctx.ctx());

  double eh_scint = 0;
  if(cc.isValid())
  {
    CaloCellContainer::const_iterator f_cell =
      cc->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator l_cell =
      cc->endConstCalo(CaloCell_ID::TILE);

    for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ; 

      int inum = cell->caloDDE()->getSampling();

      if (CaloSampling::TileGap3 == inum  ) {
        double phic = cell->phi();
        double etac = cell->eta(); 
        if (phic-phi < -pi) phic = phic + twopi;
        if (phic-phi > pi)  phic = phic - twopi;    
        if (etac >= eta-deta && etac <= eta+deta) {
          if (phic >= phi-dphi && phic <= phi+dphi) {
            eh_scint += cell->e();
          }
        }
      }
    }
  }

  // make the correction
  double granularity = (etamax_crack - etamin_crack)/correction.size();  
  int ind = static_cast<int> ((aeta - etamin_crack)/granularity);

// Scintillator energy calibration
  eh_scint = eh_scint * scint_weight[ind];
 
//Correct for normalization factor total cluster energy and samplings energies 
  double energy = (cluster->e())*correction[ind];
  setenergy(cluster,energy);

// Add the scintillator energy to the cluster and correct the total energy
  energy += eh_scint * correction[ind]; 
  cluster->setE(energy);  
}


/**
 * @brief Change the name of the CaloCellContainer used by this tool.
 * @param name The new container name.
 */
StatusCode
CaloSwGap_g3::setCaloCellContainerName
  (const std::string& name)
{
  return this->setProperty (StringProperty ("cells_name", name));
}
