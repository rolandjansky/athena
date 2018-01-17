/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MDT_PrepDataToxAOD.cxx
//   Implementation file for class MDT_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#include "MDT_PrepDataToxAOD.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// Constructor with parameters:
MDT_PrepDataToxAOD::MDT_PrepDataToxAOD(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonPrepDataToxAOD(name,pSvcLocator,"MDT_DriftCircles","MDT_SDO"),
  m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator")
{

}

StatusCode MDT_PrepDataToxAOD::initialize() {
  ATH_CHECK(MuonPrepDataToxAOD::initialize());
  ATH_CHECK(m_mdtRotCreator.retrieve());
  return StatusCode::SUCCESS;
}
// Execute method:
StatusCode MDT_PrepDataToxAOD::execute()
{
  SG::ReadHandle<Muon::MdtPrepDataContainer> mdtPrds(m_inputContainerName);
  SG::ReadHandle<MuonSimDataCollection> mdtSdos(m_sdoContainerName);
  if(!mdtPrds.isPresent()){
    ATH_MSG_DEBUG("No "<<m_inputContainerName.key()<<" collection");
    return StatusCode::SUCCESS;
  }
  if(!mdtPrds.isValid()){
    ATH_MSG_WARNING(m_inputContainerName.key()<<" not valid");
    return StatusCode::FAILURE;
  }
  SG::WriteHandle<xAOD::TrackMeasurementValidationContainer> trackMeasValCont(m_trackMeasVal);
  ATH_CHECK(trackMeasValCont.record(std::make_unique<xAOD::TrackMeasurementValidationContainer>(),std::make_unique<xAOD::TrackMeasurementValidationAuxContainer>()));
  if( !buildCollections(mdtPrds.cptr(),mdtSdos.cptr(),trackMeasValCont.ptr()) ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

void MDT_PrepDataToxAOD::addPRD_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::MdtPrepData& prd ) const {
  xprd.auxdata<int>("tdc") = prd.tdc();
  xprd.auxdata<int>("adc") = prd.adc();
  xprd.auxdata<int>("status") = prd.status();
  xprd.auxdata<unsigned int>("collectionHash") = prd.collectionHash();
  xprd.auxdata<float>("driftRadius") = prd.localPosition()[Trk::locR];
  xprd.auxdata<float>("driftError") = prd.localCovariance()(0,0);  
}

void MDT_PrepDataToxAOD::addSDO_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::MdtPrepData& prd, const MuonSimData* sdo ) const {

  xprd.auxdata<float>("driftTime")   = invalid_value;
  xprd.auxdata<float>("driftRadius") = invalid_value;
  xprd.auxdata<float>("driftError")  = invalid_value;
  xprd.auxdata<int>("driftStatus")   = invalid_value;

  if( !sdo ) return;

  // calibrate MDT hit
  Amg::Vector3D gpos = sdo->globalPosition();
  bool recalculated = false;
  // recover global position from deposit
  if( gpos.mag() < 1000 ){
    float firstEntry  = invalid_value;
    float secondEntry = invalid_value;
    /// loop over hits, check for muons and extract additional info
    for( const auto& deposit : sdo->getdeposits() ){
      if( !deposit.first.cptr() || abs(deposit.first.cptr()->pdg_id()) != 13 ) continue;
      firstEntry = deposit.second.firstEntry();
      secondEntry = deposit.second.secondEntry();
    }
    if( firstEntry != invalid_value ){
      Amg::Vector2D lpos(firstEntry,secondEntry);
      prd.detectorElement()->surface(prd.identify()).localToGlobal(lpos,prd.globalPosition(),gpos);
    }
    recalculated = true;
  }

  if( gpos.mag() > 1000 ){
    const Muon::MdtDriftCircleOnTrack* rot = m_mdtRotCreator->createRIO_OnTrack(prd,gpos);
    if( rot ){
      xprd.auxdata<float>("driftTime")   = rot->driftTime();
      xprd.auxdata<float>("driftRadius") = rot->driftRadius();
      xprd.auxdata<float>("driftError")  = sqrt(rot->localCovariance()(0,0));
      xprd.auxdata<int>("driftStatus")   = rot->status();

      if( recalculated ){
        Amg::Vector3D lpos = prd.detectorElement()->transform(prd.identify()).inverse()*gpos;
        xprd.auxdata<float>("truth_gx")    = gpos.x();
        xprd.auxdata<float>("truth_gy")    = gpos.y();
        xprd.auxdata<float>("truth_gz")    = gpos.z();
        xprd.auxdata<float>("truth_lx")    = lpos.x();
        xprd.auxdata<float>("truth_ly")    = lpos.y();
        xprd.auxdata<float>("truth_lz")    = lpos.z();
      }
    }
  }
}
