/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
   MTPM_BFMon.cxx
   ------------------------------
   AUTHORS:     Nektarios Chr.Benekos, Austin Basye, Christina G.Skarpathiotaki (UoP)
   created:     September 2013
   description: Implementation code for the MTPM_BFMon
   ============================================================================== */

#include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

using namespace std;

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::bookHistograms_BFMon(std::string sPath)
{
  ATH_MSG_DEBUG("MuonTrkPhysMonitoring : Booking: B-Field Mapping");
  InitializePlots(m_oBFMonPlots, MonGroup(this, sPath, run, ATTRIB_UNMANAGED));
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::procHistograms_BFMon()
{
  ATH_MSG_DEBUG("MuonTrkPhysMonitoring : Filling: B-Field Mapping");
  m_oBFMonPlots.finalize();
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillHistograms_BFMon(const xAOD::Muon *muon){                                
  ATH_MSG_DEBUG("MuonTrkPhysMonitoring : Filling: B-Field Mapping");
  const xAOD::TrackParticle* MSTrackParticle = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if (!MSTrackParticle) return;
  const Trk::Track* MuTrk = MSTrackParticle->track();
  if (!MuTrk) return;

  if (!MuTrk->trackStateOnSurfaces()) return;  
  const DataVector< const Trk::TrackStateOnSurface>* trackSoS = MuTrk->trackStateOnSurfaces();
  // For Sagitta Measurement:
  Amg::Vector3D     innerGP(0,0,0),middleGP(0,0,0),outerGP(0,0,0),eeGP(0,0,0),beeGP(0,0,0),cscGP(0,0,0);
  int               nInner(0),nMiddle(0),nOuter(0),nEE(0),nBEE(0),nCSC(0);

  /** Loop over all the state on surface objects in the track */

  ATH_MSG_DEBUG("Begin hitLoop over " << trackSoS << " TrkSurfaces for Sagitta Calculation");                                
  for (DataVector<const Trk::TrackStateOnSurface>::const_iterator stateOnSurfaceIt = trackSoS->begin() ; stateOnSurfaceIt != trackSoS->end(); ++stateOnSurfaceIt ){
    const Trk::TrackStateOnSurface* stateOnSurface = *stateOnSurfaceIt;
    if( !stateOnSurface || !stateOnSurface->trackParameters() || !stateOnSurface->measurementOnTrack() ||
          stateOnSurface->type(Trk::TrackStateOnSurface::Outlier) ) continue;
  
    const Trk::TrackParameters* trackParameters = stateOnSurface->trackParameters();
    const Trk::MeasurementBase* meas = stateOnSurface->measurementOnTrack();
  
    // skip pseudo measurements and ID hits
    Identifier id = m_helperTool->getIdentifier(*meas);
    if(!(id.is_valid() && m_idHelperTool->isMuon(id))) continue;
  
    Muon::MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(id);
    if (m_idHelperTool->isCsc(id)) AddSagittaCalculationInfo(cscGP, nCSC, trackParameters);
    else if ( stIndex == Muon::MuonStationIndex::EI || stIndex == Muon::MuonStationIndex::BI ) AddSagittaCalculationInfo(innerGP, nInner, trackParameters);
    else if ( stIndex == Muon::MuonStationIndex::EM || stIndex == Muon::MuonStationIndex::BM ) AddSagittaCalculationInfo(middleGP, nMiddle, trackParameters);
    else if ( stIndex == Muon::MuonStationIndex::EO || stIndex == Muon::MuonStationIndex::BO ) AddSagittaCalculationInfo(outerGP, nOuter, trackParameters);
    else if ( stIndex == Muon::MuonStationIndex::EE ) AddSagittaCalculationInfo(eeGP, nEE, trackParameters);
    else if ( stIndex == Muon::MuonStationIndex::BE ) AddSagittaCalculationInfo(beeGP, nBEE, trackParameters);
  } // end loop over all track state on surface on track
  
  //Begin Sagitta Measurement:                                
  if (nInner && nMiddle && nOuter)    FillSagittaPlots(innerGP/(double)nInner,  middleGP/(double)nMiddle,    outerGP/(double)nOuter, muon);
  else if (nInner && nEE && nMiddle)  FillSagittaPlots(innerGP/(double)nInner,  eeGP/(double)nEE,            middleGP/(double)nMiddle, muon);                                    
  else if (nInner && nBEE && nMiddle) FillSagittaPlots(innerGP/(double)nInner,  beeGP/(double)nBEE,          middleGP/(double)nMiddle, muon);
  else if (nCSC && nMiddle && nOuter) FillSagittaPlots(cscGP/(double)nCSC,      middleGP/(double)nMiddle,    outerGP/(double)nOuter, muon);
  else ATH_MSG_WARNING("Failed to compute Sagitta");
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::AddSagittaCalculationInfo(Amg::Vector3D& vec, int& hitInfo, const Trk::TrackParameters* trackParameters){
  vec += trackParameters->position();
  hitInfo++;
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::FillSagittaPlots(const Amg::Vector3D& oInnerPoint,const Amg::Vector3D& oMiddlePoint,const Amg::Vector3D& oOuterPoint, const xAOD::Muon* Muon){                                  
  TVector3 Center(0.0,0.0,0.0);
  TVector3 MiddlePoint(oMiddlePoint.x(),oMiddlePoint.y(),oMiddlePoint.z());

  Center = BarycentricCenter(oInnerPoint,oMiddlePoint,oOuterPoint);
  if ((MiddlePoint - Center).Mag() == 0) return;

  float fSagitta = Muon->pt()/(MiddlePoint-Center).Mag();                            
  m_oBFMonPlots.fill(Muon, fSagitta);	    
  ATH_MSG_DEBUG("End Residual Monitoring...");
}

//---------------------------------------------------------------------------------------
TVector3 MuonTrkPhysMonitoring::BarycentricCenter(const Amg::Vector3D& InnerPoint,const Amg::Vector3D& MiddlePoint,const Amg::Vector3D& OuterPoint){
    
  //Conversion needed...
  TVector3 A(InnerPoint.x(),InnerPoint.y(),InnerPoint.z());
  TVector3 B(MiddlePoint.x(),MiddlePoint.y(),MiddlePoint.z());
  TVector3 C(OuterPoint.x(),OuterPoint.y(),OuterPoint.z());  
  TVector3 Center(0.,0.,0.);
    
  if ((A).Mag2() == 0 || (B).Mag2() == 0 || (C).Mag2() == 0) return Center; // Origin not topologically consistent
  if ((A-B).Mag2() == 0 || (B-C).Mag2() == 0 || (A-C).Mag2() == 0) return Center; // Less than 3 independent points
  else if ((A-B).Cross(B-C).Mag2() == 0) return Center; //Co-linear Case
  
    
  double Alpha = (B-C).Mag2()*(A-B).Dot(A-C)/( (A-B).Cross(B-C).Mag2() *2 );
  double Beta  = (A-C).Mag2()*(B-A).Dot(B-C)/( (A-B).Cross(B-C).Mag2() *2 );
  double Gamma = (A-B).Mag2()*(C-A).Dot(C-B)/( (A-B).Cross(B-C).Mag2() *2 );
        
  Center = Alpha*A + Beta*B + Gamma*C;
    
  double RA = (A-Center).Mag();
  double RB = (B-Center).Mag();
  double RC = (C-Center).Mag();
    
  if (fabs(RA-RB)/RC > .001 || fabs(RA-RC)/RC > .001 || fabs(RC-RB)/RC > .001 ) {
    ATH_MSG_WARNING("WARNING: Error Calculating BarycentricCenter.");
    ATH_MSG_WARNING("[" << Alpha << "," << Beta << "," << Gamma << "]");
    ATH_MSG_WARNING("[" << Center.X() << "," << Center.Y() << "," << Center.Z() << "]");
    ATH_MSG_WARNING("[" << (A-Center).Mag() << "," << (B-Center).Mag() << "," << (C-Center).Mag() << "]");
  }    
  return Center;
}
