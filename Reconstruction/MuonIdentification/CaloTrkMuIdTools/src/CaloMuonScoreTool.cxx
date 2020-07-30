/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/CaloMuonScoreTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "PathResolver/PathResolver.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
#include "TrkParameters/TrackParameters.h"

#include "TFile.h"
#include "TH1F.h"

#include <string>
#include <iostream>
#include <cmath>
#include <map>

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool constructor
///////////////////////////////////////////////////////////////////////////////
CaloMuonScoreTool::CaloMuonScoreTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  AthAlgTool(type,name,parent),
  m_modelFileNames{"CaloMuonCNN_0.onnx"};
{
  declareInterface<ICaloMuonScoreTool>(this);  
  declareProperty("ModelFileNames", m_modelFileNames);
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonScoreTool::initialize() {
  ATH_MSG_INFO("Initializing " << name());

  ATH_CHECK(m_caloExtensionTool.retrieve());
  
  if (m_modelFileNames.size()<1) {
    ATH_MSG_FATAL("Number of input ONNX model files should be at least 1");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// CaloMuonScoreTool::getMuonScore
///////////////////////////////////////////////////////////////////////////////
double CaloMuonScoreTool::getLHR( const xAOD::TrackParticle* trk, const xAOD::CaloClusterContainer* ClusContainer, const double dR_CUT ) const {  
  ATH_MSG_DEBUG("in CaloMuonScoreTool::getLHR()");

  if(trk && ClusContainer){
    double eta_trk = trk->eta();
    double p_trk = 0;
    double qOverP = trk->qOverP();
    if (qOverP!=0) p_trk = std::abs(1/qOverP);

    std::unique_ptr<Trk::CaloExtension> caloExt=m_caloExtensionTool->caloExtension(*trk);

    if(!caloExt) return 0;
    const Trk::TrackParameters* caloEntryInterSec = caloExt->caloEntryLayerIntersection();
    if(!caloEntryInterSec) {
      ATH_MSG_WARNING("getLHR() - caloEntryLayerIntersection is nullptr");
      return 0;
    }
    double eta_trkAtCalo = caloEntryInterSec->eta();
    double phi_trkAtCalo = caloEntryInterSec->parameters()[Trk::phi];

    double LR = getLHR( ClusContainer, eta_trk, p_trk, eta_trkAtCalo, phi_trkAtCalo, dR_CUT);
    return LR;
  }
    
  return 0;
}

