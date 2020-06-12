/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaShowerShape.h"
#include "egammaInterfaces/IegammaPreSamplerShape.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "egammaInterfaces/IegammaMiddleShape.h"
#include "egammaInterfaces/IegammaBackShape.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <cmath>

egammaShowerShape::egammaShowerShape(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : AthAlgTool(type, name, parent){ 
        // declare Interface
        declareInterface<IegammaShowerShape>(this);
    }

egammaShowerShape::~egammaShowerShape(){ 
}

StatusCode egammaShowerShape::initialize(){
    ATH_MSG_DEBUG(" Initializing egammaShowerShape");

    if(m_egammaPreSamplerShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaPreSamplerShape);
        return StatusCode::FAILURE;
    } 
    ATH_MSG_DEBUG("Tool " << m_egammaPreSamplerShape << " retrieved"); 

    if(m_egammaStripsShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaStripsShape);
        return StatusCode::FAILURE;
    } 
    ATH_MSG_DEBUG("Tool " << m_egammaStripsShape << " retrieved"); 

    if(m_egammaMiddleShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaMiddleShape);
        return StatusCode::FAILURE;
    } 

    if(m_egammaBackShape.retrieve().isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve "<<m_egammaBackShape);
        return StatusCode::FAILURE;
    } 
    ATH_MSG_DEBUG("Tool " << m_egammaBackShape << " retrieved"); 

    return StatusCode::SUCCESS;
}

StatusCode egammaShowerShape::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode egammaShowerShape::execute(const xAOD::CaloCluster& cluster,
                                      const CaloDetDescrManager& cmgr,
                                      const CaloCellContainer& cell_container,
                                      Info& info) const {

  // check if cluster is in barrel or in the end-cap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    ATH_MSG_WARNING(" egammaShowerShape: Cluster is neither in Barrel nor in "
                    "Endcap, cannot calculate ShowerShape ");
  }

  IegammaPreSamplerShape::Info egammaPreSamplerShapeInfo;
  IegammaStripsShape::Info egammaStripsShapeInfo;
  IegammaMiddleShape::Info egammaMiddleShapeInfo;
  IegammaBackShape::Info egammaBackShapeInfo;

  // shower shapes in presampler
  if (m_ExecAllVariables || m_ExecPreSampler) {
    // call execute method
    StatusCode sc = m_egammaPreSamplerShape->execute(cluster, cmgr,cell_container,
                                                     egammaPreSamplerShapeInfo);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Presampler shape returned failure ");
    }
  }

  // shower shapes in 1st compartment
  if (m_ExecAllVariables || m_ExecEMFirst) {
    // call the execute method
    StatusCode sc =
        m_egammaStripsShape->execute(cluster, cmgr, egammaStripsShapeInfo);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Strip shape returned failure ");
    }
  }

  // shower shapes in 2nd compartment
  if (m_ExecAllVariables || m_ExecEMSecond) {
    // call the execute method
    StatusCode sc = m_egammaMiddleShape->execute(cluster, cmgr, cell_container,
                                                 egammaMiddleShapeInfo);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Middle shape returned failure ");
    }
  }

  // shower shapes in 3rd compartment
  if (m_ExecAllVariables || m_ExecEMThird) {
    // call execute method
    StatusCode sc = m_egammaBackShape->execute(cluster, cmgr, cell_container,
                                               egammaBackShapeInfo);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Back shape returned failure ");
    }
  }
  /*
   * Fill egammaShowerShape object
   * with values from other tools
   */

  // presampler
  info.e011 = egammaPreSamplerShapeInfo.e011;
  info.e033 = egammaPreSamplerShapeInfo.e033;

  // strips
  info.etot = egammaStripsShapeInfo.etot;
  info.e1152 = egammaStripsShapeInfo.e1152;
  info.e132 = egammaStripsShapeInfo.e132;
  info.emaxs1 = egammaStripsShapeInfo.emaxs1;
  info.emins1 = egammaStripsShapeInfo.emins1;
  info.esec = egammaStripsShapeInfo.esec;
  info.esec1 = egammaStripsShapeInfo.esec1;
  info.f1 = egammaStripsShapeInfo.f1;
  info.f1core = egammaStripsShapeInfo.f1core;
  info.f2 = egammaStripsShapeInfo.f2;
  info.ncetamax = egammaStripsShapeInfo.ncetamax;
  info.ncetaseed = egammaStripsShapeInfo.ncetaseed;
  info.etas3 = egammaStripsShapeInfo.etas3;
  info.poscs1 = egammaStripsShapeInfo.poscs1;
  info.deltaEtaTrackShower = egammaStripsShapeInfo.deltaEtaTrackShower;
  info.deltaEtaTrackShower7 = egammaStripsShapeInfo.deltaEtaTrackShower7;
  info.wstot = egammaStripsShapeInfo.wstot;
  info.ws3 = egammaStripsShapeInfo.ws3;
  info.ws3c = egammaStripsShapeInfo.ws3c;
  info.widths5 = egammaStripsShapeInfo.widths5;
  info.asymmetrys3 = egammaStripsShapeInfo.asymmetrys3;
  info.val = egammaStripsShapeInfo.val;
  info.fside = egammaStripsShapeInfo.fside;
  info.success = egammaStripsShapeInfo.success;

  // middle
  info.e233 = egammaMiddleShapeInfo.e233;
  info.e235 = egammaMiddleShapeInfo.e235;
  info.e255 = egammaMiddleShapeInfo.e255;
  info.e237 = egammaMiddleShapeInfo.e237;
  info.e277 = egammaMiddleShapeInfo.e277;
  info.etaw = egammaMiddleShapeInfo.etaw;
  info.phiw = egammaMiddleShapeInfo.phiw;
  info.poscs2 = egammaMiddleShapeInfo.poscs2;

  // back
  info.e333 = egammaBackShapeInfo.e333;
  info.e335 = egammaBackShapeInfo.e335;
  info.e355 = egammaBackShapeInfo.e355;
  info.e337 = egammaBackShapeInfo.e337;
  info.e377 = egammaBackShapeInfo.e377;
  info.f3 = egammaBackShapeInfo.f3;
  info.f3core = egammaBackShapeInfo.f3core;

  // shower shapes combined in different samplings
  if (m_ExecAllVariables || m_ExecEMCombined) {
    /* Shower shapes combining different samplings
     * all energy values have to be initialised to zero
     * which is equivalent to have nothing (if it does not exists)
     * or 0 if there is no deposit
     */

    // energy in 3 strips in the 1st sampling
    double e132 = (info.e132 > -999.) ? info.e132 : 0.;

    // energy in 1X1 in the presampler
    double e011 = (info.e011 > -999.) ? info.e011 : 0.;
    // energy in 3X3 in the presampler
    double e033 = (info.e033 > -999.) ? info.e033 : 0.;
    // energy in 15 strips in the 1st sampling
    double e1152 = (info.e1152 > -999.) ? info.e1152 : 0.;
    // energy in 3X3 in the 2nd sampling
    double e233 = (info.e233 > -999.) ? info.e233 : 0.;
    // energy in 5X5 in the 2nd sampling
    double e255 = (info.e255 > -999.) ? info.e255 : 0.;
    // energy in 3X7 in the 2nd sampling
    double e237 = (info.e237 > -999.) ? info.e237 : 0.;
    // energy in 3X3 in the 3rd sampling
    double e333 = (info.e333 > -999.) ? info.e333 : 0.;
    // energy in 3X5 in the 3rd sampling
    double e335 = (info.e335 > -999.) ? info.e335 : 0.;
    // energy in 3X7 in the 3rd sampling
    double e337 = (info.e337 > -999.) ? info.e337 : 0.;

    /*energy in core of the shower
     * 3X3 in the presampler (info.e033)
     * 15X2 in the strips     (info.e1152)
     * 5X5 in the middle     (info.e255)
     * 3X5 in the back       (info.e335)
     */
    info.ecore = e033 + e1152 + e255 + e335;

    /* e 3X3 / e 3X7, the ratio of energies deposited
     * in a 3X3 and 3X7 clusters. More precisely,
     * for the 3X3 cluster, sampling by sampling, the cell energies
     * are summed on the following eta by phi windows
     * 1X1 in the presampler (info.e011)
     * 3X2 in the strips     (info.e132)
     * 3X3 in the middle     (info.e233)
     * 3X3 in the back       (info.e333)
     * for the 7X3 cluster, sampling by sampling, the cell energies
     * are summed on the following eta by phi windows
     * 3X3 in the presampler (info.e033)
     * 15X2 in the strips     (info.e1152)
     * 3X7 in the middle     (info.e237)
     * 3X7 in the back       (info.e337)
     */
    double e33 = e011 + e132 + e233 + e333;
    double e37 = e033 + e1152 + e237 + e337;
    info.reta3337_allcalo = fabs(e37) > 0. ? 1. - e33 / e37 : 1.;
  }
  return StatusCode::SUCCESS;
}

