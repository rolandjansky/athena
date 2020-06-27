/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETCONVERSIONFINDERTOOLS_TRACKPAIRSSELECTOR_H
#define INDETCONVERSIONFINDERTOOLS_TRACKPAIRSSELECTOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Particle/TrackParticle.h"

#include "xAODTracking/TrackParticleFwd.h"


namespace Trk
{
  class Track;
  class ITrkDistanceFinder;
}



namespace InDet {
  class ConversionFinderUtils;

  /**
     @class TrackPairsSelector
     This class selects track pairs for conversion finder
     @author Tatjana Lenz, Thomas Koffas
  */
  
  class TrackPairsSelector : public AthAlgTool {
    
  public:

    struct Cache{
      float m_distance = 9999. ; /** Distance of closest approach between the tracks **/
      float m_deltaCotTheta= 9999.; /** Delta cot theta between the tracks **/
      float m_deltaInit = 9999.; /** Distance difference between initial hits of tracks */
    };

    TrackPairsSelector(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);
   
    virtual ~TrackPairsSelector();
    
    static const InterfaceID& interfaceID();
 
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    /** Track pair selectors.Return true if the argument track fulfills the selection */
    bool selectTrackParticlePair(const xAOD::TrackParticle* trkPpos,
                                 const xAOD::TrackParticle* trkPneg,
                                 Cache& cache) const;

    bool selectTrackPair(const Trk::Track* trkpos,
                         const Trk::Track* trkneg) const;

    /** Return a map with the values calculated for the last pair
      * to decorate the vertex once it is created **/
    std::map<std::string, float> getLastValues(const Cache& cache) const;
    
  private:

    ToolHandle <InDet::ConversionFinderUtils> m_helpertool; /**Conversion helper tool.  */
    ToolHandle<Trk::ITrkDistanceFinder> m_distanceTool;  /** Distance of minimum approach tool   */
    /** Properties for track selection: all cuts are ANDed */
    double m_maxR;                 /** Maximum initial hit radius in order to apply the impact point cut*/
    double m_MinTrkAngle;          /** Minimum allowed angle between decay tracks. Used only in V0 reconstruction. */
    std::vector<double> m_maxDist; /** Maximum allowed distance of minimum approach */
    std::vector<double> m_etaCut;  /** Maximum eta difference between tracks in pair. */
    std::vector<double> m_initCut; /** Maximum distance difference between initial hits of tracks in pair. */


  };
  
}
#endif // INDETCONVERSIONFINDERTOOLS_TRACKPAIRSSELECTOR_H 

