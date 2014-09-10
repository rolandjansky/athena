/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackParametersAtCaloFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

using HepGeom::Point3D;


namespace D3PD {


TrkTrackParametersAtCaloFillerTool::TrkTrackParametersAtCaloFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<Trk::Track> (type, name, parent),
    m_trackToCaloTool("ExtrapolateToCaloTool/extrapolateToCaloTool"),
    m_calo_dd(0)
{
  declareProperty ("TrackToCaloTool", m_trackToCaloTool);

  declareProperty("SubCaloForExtrapol",   m_subCaloStr        = "LAREM");
  declareProperty("Sampling",             m_sampling       = 0);

  book().ignore(); // Avoid coverity warnings.
}

StatusCode TrkTrackParametersAtCaloFillerTool::initialize(){

  CHECK( D3PD::BlockFillerTool<Trk::Track>::initialize() );

  StatusCode sc = m_trackToCaloTool.retrieve();
  if(sc.isFailure()){
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToCaloTool";
    return StatusCode::FAILURE;
  }

  m_caloDepthTool = m_trackToCaloTool->getCaloDepth();
  sc = m_caloDepthTool.retrieve();
  if(sc.isFailure()){
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve CaloDepthTool";
    return StatusCode::FAILURE;
  }
  
  if ( m_subCaloStr == "LAREM" )        m_subCalo = CaloCell_ID::LAREM;
  else if ( m_subCaloStr == "LARHEC" )  m_subCalo = CaloCell_ID::LARHEC;
  else if ( m_subCaloStr == "LARFCAL" ) m_subCalo = CaloCell_ID::LARFCAL;
  else if ( m_subCaloStr == "TILE" )    m_subCalo = CaloCell_ID::TILE;
  else {
    ATH_MSG_DEBUG("Calorimeter " << m_subCalo << " is not a valid Calorimeter. Will use default = LAREM. "
		  << "Valid names are: LAREM, LARHEC, LARFCAL, and TILE.");
    m_subCalo = CaloCell_ID::LAREM;
  }

  m_calo_dd = CaloDetDescrManager::instance();

  return StatusCode::SUCCESS;
}

StatusCode TrkTrackParametersAtCaloFillerTool::book()
{
  // track parameters at perigee
  CHECK( addVariable ("eta_atCalo", m_eta_atCalo, "", -999.99) );
  CHECK( addVariable ("phi_atCalo", m_phi_atCalo, "", -999.99) );

  return StatusCode::SUCCESS;
}

StatusCode TrkTrackParametersAtCaloFillerTool::fill (const Trk::Track& track)
{

  if(extrapolateTrack(&track, m_eta_atCalo, m_phi_atCalo).isSuccess()){
    ATH_MSG_DEBUG("Variables filled correctly: eta = " << *m_eta_atCalo << " - phi = " << *m_phi_atCalo);
  }else{
    ATH_MSG_DEBUG("Default values filled: eta = " << *m_eta_atCalo << " - phi = " << *m_phi_atCalo);
  }

  return StatusCode::SUCCESS;
}

StatusCode TrkTrackParametersAtCaloFillerTool::extrapolateTrack(const Trk::Track * trk, float * eta, float * phi) 
{
  /// Track parameters of this track
  const Trk::Perigee *trackParams = trk->perigeeParameters();
  
  // Determine if it's in the barrel
  bool barrel = true;
  double trketa = trackParams->eta();
  if ( m_caloDepthTool->deta(CaloCell_ID::PreSamplerB,trketa) < 0) barrel = true;
  else barrel = false;
  
  CaloCell_ID::CaloSample sample;
  m_calo_dd->build_sample( m_subCalo, barrel ,m_sampling, sample);
  
  
  const Trk::TrackParameters *trackParameters =  m_trackToCaloTool->extrapolate(*trk,sample,0.0);
  if(trackParameters){
    ATH_MSG_DEBUG("Extrapolated the track in " << m_subCaloStr << " successfully ");
    // If it didn't work, try the opposite case
  } else  {
    m_calo_dd->build_sample( m_subCalo, !barrel ,m_sampling, sample);
    trackParameters =  m_trackToCaloTool->extrapolate(*trk,sample,0.0);
    ATH_MSG_DEBUG("Extrapolated the track in " << m_subCaloStr << " successfully, using the endcap ");
  }
  
  if (!trackParameters){
    ATH_MSG_DEBUG("Could not extraplate the track in " << m_subCaloStr << " successfully...giving up ");
    
    return StatusCode::FAILURE;
  }
  
  *eta = trackParameters->eta();
  *phi = (trackParameters->parameters())[Trk::phi];
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
