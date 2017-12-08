/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RPC_PrepDataToxAOD.cxx
//   Implementation file for class RPC_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#include "RPC_PrepDataToxAOD.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "TrkEventPrimitives/ResidualPull.h"

// Constructor with parameters:
RPC_PrepDataToxAOD::RPC_PrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonPrepDataToxAOD(name,pSvcLocator,"RPC_Measurements","RPC_SDO"),
  m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
  m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
  m_inversePropagationSpeed(0.0048)
{  
    
}

StatusCode RPC_PrepDataToxAOD::initialize() 
{
  ATH_CHECK(MuonPrepDataToxAOD::initialize());
  ATH_CHECK(m_clusterCreator.retrieve());
  ATH_CHECK(m_pullCalculator.retrieve());
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode RPC_PrepDataToxAOD::execute() 
{
  if( !buildCollections() ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

void RPC_PrepDataToxAOD::addPRD_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::RpcPrepData& prd ) const {
  xprd.auxdata<float>("time") = prd.time();
  xprd.auxdata<int>("triggerInfo") = prd.triggerInfo();
  xprd.auxdata<int>("ambiguityFlag") = prd.ambiguityFlag();
  xprd.auxdata<unsigned int>("collectionHash") = prd.collectionHash();
  xprd.auxdata<uint16_t>("measPhi") = m_idHelper->measuresPhi(prd.identify());
  xprd.auxdata<uint16_t>("muonClusterSize") = (uint16_t)prd.rdoList().size();
}

void RPC_PrepDataToxAOD::addSDO_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::RpcPrepData& prd, const MuonSimData* sdo ) const {

  float& propagationTime = xprd.auxdata<float>("propagationTime");
  propagationTime = invalid_value;
  if( !sdo ) return;

  Amg::Vector3D gpos = sdo->globalPosition();
  if( gpos.mag() > 1000 ){
    double distToReadout = 0.;
    if( m_idHelper->measuresPhi(prd.identify()) ) {
      distToReadout = prd.detectorElement()->distanceToPhiReadout(gpos);
    }else{
      distToReadout = prd.detectorElement()->distanceToEtaReadout(gpos);
    }
    if( distToReadout > -998 ) propagationTime  =  distToReadout*m_inversePropagationSpeed;    
  }

  float& residual = xprd.auxdata<float>("residual");
  float& pull = xprd.auxdata<float>("pull");
  residual = invalid_value;
  pull = invalid_value;
  if( gpos.mag() > 1000 ){
    Amg::Transform3D gToL = prd.detectorElement()->transform(prd.identify()).inverse();
    Amg::Vector3D lpos = gToL*gpos;
    Amg::Vector3D ldir = gToL.linear()*gpos.unit();
    Amg::Vector3D lposCor = lpos - lpos.z()/ldir.z()*ldir;
    ATH_MSG_DEBUG("lpos  " << lpos.z() << " cor " << lposCor.z() );
    Amg::Vector3D gposCor = gToL.inverse()*lposCor;
    const Muon::MuonClusterOnTrack* clus = m_clusterCreator->createRIO_OnTrack(prd,gposCor);
    if( clus ) {
      const Trk::TrackParameters* pars = clus->associatedSurface().createTrackParameters(gposCor,gposCor.unit(),1.,NULL);
      if( pars ){
        const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( clus, pars, Trk::ResidualPull::HitOnly );
        if( resPull && !resPull->residual().empty() ) {
          residual = resPull->residual().front();
          pull     = resPull->pull().front();
        }else ATH_MSG_DEBUG("Failed to calculate residual");
        delete resPull;
      }else ATH_MSG_DEBUG("Failed to create track parameters");
      delete pars;
    }else ATH_MSG_DEBUG("Failed to create cluster on track");
    delete clus;
  }
  ATH_MSG_DEBUG("time " << prd.time() << " prop time " << propagationTime );
}
