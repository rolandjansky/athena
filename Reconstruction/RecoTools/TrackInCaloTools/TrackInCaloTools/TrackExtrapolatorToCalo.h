/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKEXTRAPOLATORTOCALO_H
#define TRACKEXTRAPOLATORTOCALO_H
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventKernel/INavigable4Momentum.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloPhiRange.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
#include "TrackInCaloTools/TICTListener.h"

#define NLAYERS 11
/////////////////////////////////////////////////////////
class StoreGateSvc;

class ITrackDirectionTool;
class ICaloSurfaceBuilder;
class CaloDepthTool;
class CaloDetDescrManager;
class CaloCellDetPos;
//

/////////////////////////////////////////////////////////
/** @class TrackExtrapolatorToCalo

    Tool used to extrapolate a track from the perigee to each layer of the calorimeters

    @author Bruno.Lenzi@cern.ch
**/
class TrackExtrapolatorToCalo: public AthAlgTool, virtual public ITrackExtrapolatorToCalo, public TICTListener
{
  public:
    TrackExtrapolatorToCalo (const std::string& type,const std::string& name,const IInterface* parent);
    ~TrackExtrapolatorToCalo();

    StatusCode initialize();
    StatusCode finalize();
    
    std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const Trk::TrackParameters* track);
    std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const Trk::TrackParameters* track);
    std::vector<double> getPathLengths(const Trk::TrackParameters* track);
    StatusCode getMidPointForExtrapolations(const Trk::TrackParameters*, std::vector<double>, std::vector<double>);
    std::vector< std::pair<double, double> > getMidPointForExtrapolations(const Trk::TrackParameters*);
    std::vector<double> getParametrizedEloss(const Trk::TrackParameters* track);
    
    // Same for Trk::Track
    std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const Trk::Track* track)
    { return getExtrapolationsToLayerEntrance(getTrackParameters(track)); };
    std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const Trk::Track* track)
    { return getExtrapolationsToLayerExit(getTrackParameters(track)); };
    std::vector<double> getPathLengths(const Trk::Track* track)
    { return getPathLengths(getTrackParameters(track)); };
    StatusCode getMidPointForExtrapolations(const Trk::Track* track, std::vector<double> eta, std::vector<double> phi)
    { return getMidPointForExtrapolations(getTrackParameters(track), eta, phi); };
    std::vector< std::pair<double, double> > getMidPointForExtrapolations(const Trk::Track* track)
    { return getMidPointForExtrapolations(getTrackParameters(track)); };
    std::vector<double> getParametrizedEloss(const Trk::Track* track)
    { return getParametrizedEloss(getTrackParameters(track)); };

    // Same for INavigable4Momentum
    std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerEntrance(const INavigable4Momentum* track)
    { return getExtrapolationsToLayerEntrance(getTrackParameters(track)); };
    std::vector< const Trk::TrackParameters* > getExtrapolationsToLayerExit(const INavigable4Momentum* track)
    { return getExtrapolationsToLayerExit(getTrackParameters(track)); };
    std::vector<double> getPathLengths(const INavigable4Momentum* track)
    { return getPathLengths(getTrackParameters(track)); };
    StatusCode getMidPointForExtrapolations(const INavigable4Momentum* track, std::vector<double> eta, std::vector<double> phi)
    { return getMidPointForExtrapolations(getTrackParameters(track), eta, phi); };
    std::vector< std::pair<double, double> > getMidPointForExtrapolations(const INavigable4Momentum* track)
    { return getMidPointForExtrapolations(getTrackParameters(track)); };
    std::vector<double> getParametrizedEloss(const INavigable4Momentum* track)
    { return getParametrizedEloss(getTrackParameters(track)); };

    void setPropagationDirection(Trk::PropDirection direction);
    int getPropagationDirection() { return (int) m_propDirection; };
    const Trk::TrackParameters* getTrackParameters(const Trk::Track *track);
    const Trk::TrackParameters* getTrackParameters(const INavigable4Momentum* track);
    void setParticleHypothesis(const std::string particleHypothesis);
    
  private:
    // Properties
    ToolHandle<Trk::IExtrapolator>  m_extrapolator;
    bool m_cosmics;
    std::string m_particleHypothesisString;
    Trk::ParticleHypothesis m_particleHypothesis;
    
    // Calorimeter pointers
    ICaloSurfaceBuilder* m_surface;
    CaloDepthTool* m_calodepth;
    const CaloDetDescrManager* m_calo_dd;
    CaloCellDetPos *m_caloCellDetPos; // conversion between ATLAS frame and calo frame
    
    // Pointers and values that are reset at the end of event by m_clear
    const Trk::TrackParameters* m_currentTrackParameters;
    const Trk::Track* m_currentTrkTrack;
    const INavigable4Momentum* m_currentI4Mom;
    std::vector< const Trk::TrackParameters* > m_extrapolationsToEntrance;
    std::vector< const Trk::TrackParameters* > m_extrapolationsToExit;
    std::vector<CaloCell_ID::CaloSample> m_sampleUsed; // sample used on the extrapolation for each layer
    std::vector<double> m_pathLength;
    
    // For extrapolation
    ITrackDirectionTool *m_trackDirectionTool;
    std::vector<std::vector<double> > m_etaMax;
    std::vector<std::vector<CaloCell_ID::CaloSample> > m_caloSamples; // all samples
    Trk::PropDirection m_propDirection; // propagation direction: along or opposite to momentum

    void getCaloSamples();
    const Trk::TrackParameters* extrapolateToSample(CaloCell_ID::CaloSample,const Trk::TrackParameters*, double offset=0, bool back_of_sample = false);
    double etamax(const CaloCell_ID::CaloSample sample, const bool isTile);
    double etamin(const CaloCell_ID::CaloSample sample, const bool isTile);
    
    bool setPropagationDirection(const Trk::Track* track);
    bool setPropagationDirection(const INavigable4Momentum* track);
    
    bool newTrack(const Trk::TrackParameters* track);

    double deltaR(const Trk::TrackParameters *a, const Trk::TrackParameters *b);
    double phiMean(double a, double b) { return 0.5*(a+b) + (a*b < 0)*M_PI; };
    // Listener
    void m_clear();
};
#endif //TRACKEXTRAPOLATORTOCALO_H
