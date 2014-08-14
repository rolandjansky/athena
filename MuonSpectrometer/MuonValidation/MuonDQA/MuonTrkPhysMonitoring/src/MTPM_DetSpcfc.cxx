/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
   MTPM_DetSpcfc.cxx
   ------------------------------
   AUTHORS:     Nektarios Chr.Benekos, Austin Basye, Christina G. Skarpathiotaki (UoP)
   created:     September 2013
   description: Implementation code for the MTPM_DetSpcfc
   ============================================================================== */

#include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

using namespace std;

/** bookHistograms */
//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::bookHistograms_DetSpcfc(std::string sPath)
{
  ATH_MSG_DEBUG("Booking Histograms for bookHistograms_DetSpcfc");
  InitializePlots(m_oDetSpcfcPlots, MonGroup(this, sPath, run, ATTRIB_UNMANAGED));
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::procHistograms_DetSpcfc()
{
  ATH_MSG_DEBUG("Processing Histograms for procHistograms_DetSpcfc");
  m_oDetSpcfcPlots.finalize();        
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillHistograms_DetSpcfc( const xAOD::Muon* muon ){
  // Get pointer to track state on surfaces -   // Retrieve the states on surfaces from the track
  const xAOD::TrackParticle* MSTrackParticle = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if (!MSTrackParticle) return;
  const Trk::Track* MuTrk = MSTrackParticle->track();
  if (!MuTrk) return;
  if(!MuTrk->trackStateOnSurfaces()) return;

  const  Trk::Perigee *measPerigee = MuTrk->perigeeParameters();
  if (!measPerigee) return; 

  const DataVector< const Trk::TrackStateOnSurface>* trackSoS = MuTrk->trackStateOnSurfaces();
  ATH_MSG_DEBUG("Begin hitLoop over " << trackSoS << " TrkSurfaces");                                
  for (DataVector<const Trk::TrackStateOnSurface>::const_iterator stateOnSurface = trackSoS->begin(); stateOnSurface != trackSoS->end(); ++stateOnSurface ){                                
   CalculateResidualFromTrackStateOnSurface(*stateOnSurface, measPerigee);
  }
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::CalculateResidualFromTrackStateOnSurface(const Trk::TrackStateOnSurface* stateOnSurface, const Trk::Perigee *measPerigee){
  if( !stateOnSurface || stateOnSurface->type(Trk::TrackStateOnSurface::Outlier)) return;
  
  const Trk::TrackParameters* trackParameters = stateOnSurface->trackParameters();
  const Trk::MeasurementBase* meas = stateOnSurface->measurementOnTrack();
  if (!trackParameters || !meas) return;
  
  // skip pseudo measurements and ID hits
  Identifier id = m_helperTool->getIdentifier(*meas);
  if(!id.is_valid() || !m_idHelperTool->isMuon(id) ) return;
  
  //Redidual Calculations:
  std::unique_ptr<const Trk::ResidualPull> resPull(m_pullCalculator->residualPull(meas, trackParameters, Trk::ResidualPull::Unbiased));
  if( !resPull ) return;
                                     
  // Calculate Residual for hit: res = (vec(x_track) - vec(x_hit)) * vec(n_perpendicular)
  double residual = resPull->residual().front();
  double pull = fabs(resPull->pull().front());
  double error = (pull != 0.) ? residual/pull : 1.; //Amg::error(meas->localCovariance(),Trk::locX);
                                     
  fillResidualPlots(residual,id,error,measPerigee);                                    
  PrintResidualDebugInfo(residual,id);
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillResidualPlots(double residual, const Identifier &id, double error, const Trk::Perigee *measPerigee)
{
   bool isMDT = m_idHelperTool->isMdt(id);
   bool isCSC = m_idHelperTool->isCsc(id);
   int imdtstationName = (isMDT) ? int(m_idHelperTool->mdtIdHelper().stationName(id)) : 0;
   bool Fill_HR_Histograms = (isMDT) ? (error < m_GlobalResidualError || TString(m_trackCollectionName).CompareTo("StacoMuonCollection") ) : false; 
     
   m_oDetSpcfcPlots.fill(measPerigee, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::PrintResidualDebugInfo(double residual, const Identifier &id){
  if (!m_idHelperTool->isMdt(id)) return;      
  ATH_MSG_DEBUG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
  ATH_MSG_DEBUG("&     ---  Measurement Summary ---    "     );
  ATH_MSG_DEBUG("&  Residual: "           << residual        );
  ATH_MSG_DEBUG("&  Mdt Station Phi: "    << int(m_idHelperTool->mdtIdHelper().stationPhi(id))  );
  ATH_MSG_DEBUG("&  Mdt Station Eta: "    << int(m_idHelperTool->mdtIdHelper().stationEta(id))  );
  ATH_MSG_DEBUG("&  Mdt Station Name: "   << int(m_idHelperTool->mdtIdHelper().stationName(id)) );
  ATH_MSG_DEBUG("&  Mdt TubeLayer: "      << int(m_idHelperTool->mdtIdHelper().tubeLayer(id))   );
  ATH_MSG_DEBUG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
}
