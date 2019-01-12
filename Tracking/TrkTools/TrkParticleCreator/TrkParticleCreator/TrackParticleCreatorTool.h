/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
TrackParticleCreatorTool.h  -  Description
-------------------
begin   : Autumn 2003
authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
changes : 11.02.04 added docu

***************************************************************************/
#ifndef TRKPARTICLECREATOR_PARTICLECREATORTOOL_H
#define TRKPARTICLECREATOR_PARTICLECREATORTOOL_H

#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParticleBase/TrackParticleBase.h" // for TrackParticleOrigin enum
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "AthContainers/AuxElement.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelGeoModel/IBLParameterSvc.h"

class AtlasDetectorID;

class IBeamCondSvc;
class IBLParameterSvc;

namespace Rec
{
  class TrackParticle;
}

namespace Reco 
{ 
  class ITrackToVertex; 
} 

namespace MagField 
{
  class IMagFieldSvc;
}


namespace Trk {

  class ITrackSummaryTool;
  class IExtrapolator;
  class Track;
  class VxCandidate;

  class TrackParticleCreatorTool : virtual public ITrackParticleCreatorTool, public AthAlgTool
  {
  public:

    TrackParticleCreatorTool(const std::string&,const std::string&,const IInterface*);

    virtual ~TrackParticleCreatorTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /** Method to construct a TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee
    i.e. the TrackParticle will not be complete
    @param track Pointer to a valid track (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
    @param vxCandidate Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
    @param prtOrigin 
    @warning In my opinion, the interface is not optimal - we're not taking ownership of the Trk::Track or Vx::Candidate, 
    so they should be passed by reference.
    */
    Rec::TrackParticle* createParticle( const Trk::Track* track,
    const Trk::VxCandidate* vxCandidate,
    Trk::TrackParticleOrigin prtOrigin);

    /** Method to construct a xAOD::TrackParticle from a Rec::TrackParticle.
    @param track particle 
    @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
    */
    xAOD::TrackParticle* createParticle( const Rec::TrackParticle& trackParticle,  xAOD::TrackParticleContainer* container ) const ;

    /** Method to construct a xAOD::TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee
    i.e. the TrackParticle will not be complete
    @param track Pointer to a valid track (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
    @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
    @param xAOD::Vertex Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
    @param prtOrigin Particle type
    @warning In my opinion, the interface is not optimal - we're not taking ownership of the Trk::Track or Vx::Candidate, 
    so they should be passed by reference.
    */
    xAOD::TrackParticle* createParticle( const Trk::Track& track,
                                         xAOD::TrackParticleContainer* container,
                                         const xAOD::Vertex* vxCandidate,
                                         xAOD::ParticleHypothesis prtOrigin) const;

    /** Method to construct a TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee
    i.e. the TrackParticle will not be complete
    @param track element link to a valid track (i.e. do not pass a zero!). 
    @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
    @param xAOD::Vertex Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
    @param prtOrigin 
    */
    xAOD::TrackParticle* createParticle( const ElementLink<TrackCollection>& trackLink,
                                         xAOD::TrackParticleContainer* container,
                                         const xAOD::Vertex* vxCandidate,
                                         xAOD::ParticleHypothesis prtOrigin) const;

    /** create a xAOD::TrackParticle out of constituents */
  xAOD::TrackParticle* createParticle( const Perigee* perigee, const FitQuality* fq, const TrackInfo* trackInfo, const TrackSummary* summary,
                                       const std::vector<const Trk::TrackParameters*>& parameters, 
                                       const std::vector< xAOD::ParameterPosition>& positions, 
                                       xAOD::ParticleHypothesis prtOrigin,
                                       xAOD::TrackParticleContainer* container ) const;

    /** Method to set FitQuality of a xAOD::TrackParticle */
  void setFitQuality( xAOD::TrackParticle& tp, const FitQuality& fq ) const;

    /** Method to set TrackInfo of a xAOD::TrackParticle */
  void setTrackInfo( xAOD::TrackParticle& tp, const TrackInfo& trackInfo, xAOD::ParticleHypothesis prtOrigin ) const;

    /** Method to set TrackSummary of a xAOD::TrackParticle */
  void setTrackSummary( xAOD::TrackParticle& tp, const TrackSummary& summary ) const;

    /** Method to set Defining parameters of a xAOD::TrackParticle */
  void setDefiningParameters( xAOD::TrackParticle& tp, const Perigee& perigee ) const;

    /** Method to set parameters of a xAOD::TrackParticle */
  void setParameters( xAOD::TrackParticle& tp, const std::vector<const Trk::TrackParameters*>& parameters, const std::vector< xAOD::ParameterPosition>& positions  ) const;

  void setTilt(xAOD::TrackParticle& tp, float tiltx, float tilty) const;

  void setHitPattern(xAOD::TrackParticle& tp, unsigned long hitpattern) const;

  void setNumberOfUsedHits(xAOD::TrackParticle& tp, int hits) const;

  void setNumberOfOverflowHits(xAOD::TrackParticle& tp, int overflows) const;
  
  /** Get the name used for the decoration of the track particle with the number of used hits for TRT dE/dx computation.*/
  static const std::string & trtdEdxUsedHitsAuxName() { return s_trtdEdxUsedHitsDecorationName; }

private:
  void compare( const Rec::TrackParticle& tp, const xAOD::TrackParticle& tpx ) const;      
  void compare( const TrackParameters& tp1, const TrackParameters& tp2 ) const;
  /**atlas id helper*/
  const AtlasDetectorID* m_detID;
  const PixelID* m_pixelID;
  
  ToolHandle< ITrackSummaryTool > m_trackSummaryTool;
  ToolHandle< IExtrapolator >  m_extrapolator;
  ToolHandle< Reco::ITrackToVertex > m_trackToVertex;
  ToolHandle<Muon::IMuonHitSummaryTool> m_hitSummaryTool;

  /** to query magnetic field configuration */
  ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc;
  ServiceHandle<IBeamCondSvc> m_beamConditionsService;
  ServiceHandle <IBLParameterSvc> m_IBLParameterSvc;


  /** Configurable to set the eProbabilities and extra track summary types which are to be copied from the track summary.*/
  std::vector<std::string>                                   m_copyExtraSummaryName;

  /** Enums of an eProbability which are set in the xAOD::TrackSummary.*/
  std::vector<Trk::eProbabilityType>                         m_copyEProbabilities;     

  /** The pairs if enums  of an eProbability which is added as a decoration to the track particle and the name of the decoration.*/
  std::vector<std::pair<SG::AuxElement::Decorator<float>,Trk::eProbabilityType> > m_decorateEProbabilities; 
  std::vector<std::pair<SG::AuxElement::Decorator<uint8_t>,Trk::SummaryType> >    m_decorateSummaryTypes; 

  /** Minimum number of TRT hits for which the TRT dE/dx is computed.*/
  int m_minTRThitsForTRTdEdx;
    
  /** Name used for the decoration of the track particle with TRT dE/dx .*/
  static const std::string s_trtdEdxUsedHitsDecorationName;
  static const SG::AuxElement::Decorator<uint8_t> s_trtdEdxUsedHitsDecoration;


  bool m_doIBL;
  bool m_useTrackSummaryTool;
  bool m_useMuonSummaryTool;
  bool m_doITk;
  bool m_forceTrackSummaryUpdate; /** use to force an update of the track summary
  rather than using the cached summary */
    /** the following keep options are mutually exclusive **/
  bool m_keepParameters;  /** keep all TrackParameters */
  bool m_keepFirstParameters; ///< keep the first parameters when creating track particles.
  bool m_keepAllPerigee;  /** keep all MeasuredPerigee parameters
  (e.g. adding those that may exist at Volume boundaries) */
  bool m_expressPerigeeToBeamSpot;
  int m_badclusterID;
  
  std::string m_perigeeExpression;
  std::vector<std::string> m_perigeeOptions{"BeamLine", "BeamSpot", "Vertex", "Origin"}; 

  bool m_checkConversion;
  int m_minSiHits;
  double m_minPt;
  
  bool castPerigeeAndCheck(const Trk::Track* track, const Trk::Perigee* &aPer) const;
};

inline void TrackParticleCreatorTool::setFitQuality( xAOD::TrackParticle& tp, const FitQuality& fq ) const {
  tp.setFitQuality(fq.chiSquared(), static_cast<float>(fq.doubleNumberDoF ()));
}

inline void TrackParticleCreatorTool::setTrackInfo( xAOD::TrackParticle& tp, const TrackInfo& trackInfo, xAOD::ParticleHypothesis prtOrigin ) const {
  unsigned int properties=0;
  for (unsigned int i = 0; i< Trk::TrackInfo::NumberOfTrackProperties ; i++){
    if (trackInfo.trackProperties(static_cast<Trk::TrackInfo::TrackProperties>(i))) properties |= (1<<i);
  }
  tp.setTrackProperties(static_cast<xAOD::TrackProperties>(properties));
  uint64_t patternReco=0;
  const uint64_t c1=1;

  for (unsigned int i = 0; i< xAOD::NumberOfTrackRecoInfo ; i++){         
    if (trackInfo.patternRecoInfo(static_cast<Trk::TrackInfo::TrackPatternRecoInfo>(i)))  patternReco |= c1 << i;
  }

  tp.setPatternRecognitionInfo(patternReco);

  if( prtOrigin == xAOD::noHypothesis ) tp.setParticleHypothesis(static_cast<xAOD::ParticleHypothesis>(trackInfo.particleHypothesis () ) );
  else                                  tp.setParticleHypothesis(prtOrigin);
  tp.setTrackFitter(static_cast<xAOD::TrackFitter>(trackInfo.trackFitter () ) );
}


inline void TrackParticleCreatorTool::setDefiningParameters( xAOD::TrackParticle& tp, const Perigee& perigee ) const {
  tp.setDefiningParameters(perigee.parameters()[Trk::d0],
    perigee.parameters()[Trk::z0],
    perigee.parameters()[Trk::phi0],
    perigee.parameters()[Trk::theta],
    perigee.parameters()[Trk::qOverP]);
  const AmgSymMatrix(5)* covMatrix = perigee.covariance();
  // see https://its.cern.ch/jira/browse/ATLASRECTS-645 for justification to comment out the following line 
  // assert(covMatrix && covMatrix->rows()==5&& covMatrix->cols()==5); 
  std::vector<float> covMatrixVec;
  if( !covMatrix ) ATH_MSG_WARNING("Setting Defining parameters without error matrix");
  else Amg::compress(*covMatrix,covMatrixVec);
  tp.setDefiningParametersCovMatrixVec(covMatrixVec);
  const Amg::Vector3D& surfaceCenter = perigee.associatedSurface().center(); 
  tp.setParametersOrigin(surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z() );
}

} // end of namespace Trk

#endif
