/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IInDetTestBLayerTool_H
#define IInDetTestBLayerTool_H
#include "GaudiKernel/AlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include <vector>
#include <string>

namespace Trk {  class Track;  class TrackParticleBase; class IResidualPullCalculator;}
namespace InDet { class TrackStateOnBLayerInfo; }

namespace InDet {


  static const InterfaceID IID_IInDetTestBLayerTool("InDet::IInDetTestBLayerTool", 1, 0);

  class IInDetTestBLayerTool : virtual public IAlgTool
  {
    
  public:

    static const InterfaceID& interfaceID() { return IID_IInDetTestBLayerTool; };

    virtual ~IInDetTestBLayerTool() {};
    
    virtual bool expectHitInBLayer(const Trk::TrackParticleBase*, bool recompute=false) const=0;
    virtual bool expectHitInBLayer(const Trk::Track*, bool recompute=false) const=0;
    virtual bool expectHitInBLayer(const Trk::TrackParameters* trackpar) const=0;

    virtual const Trk::ResidualPull* bLayerHitResidual(const Trk::Track* ) const=0;
    virtual const Trk::ResidualPull* bLayerHitResidual(const Trk::TrackParticleBase*) const=0;
 
    virtual bool getTrackStateOnBlayerInfo(const Trk::TrackParticleBase*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnBlayerInfo(const Trk::Track*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnBlayerInfo(const Trk::TrackParameters* trackpar, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;


    virtual bool expectHitInInnermostPixelLayer(const Trk::TrackParticleBase*, bool recompute=false) const=0;
    virtual bool expectHitInInnermostPixelLayer(const Trk::Track*, bool recompute=false) const=0;
    virtual bool expectHitInInnermostPixelLayer(const Trk::TrackParameters* trackpar) const=0;

    virtual const Trk::ResidualPull* innermostPixelLayerHitResidual(const Trk::Track* ) const=0;
    virtual const Trk::ResidualPull* innermostPixelLayerHitResidual(const Trk::TrackParticleBase*) const=0;
 
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParticleBase*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::Track*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;

    virtual bool expectHitInNextToInnermostPixelLayer(const Trk::TrackParticleBase*, bool recompute=false) const=0;
    virtual bool expectHitInNextToInnermostPixelLayer(const Trk::Track*, bool recompute=false) const=0;
    virtual bool expectHitInNextToInnermostPixelLayer(const Trk::TrackParameters* trackpar) const=0;
    
    virtual const Trk::ResidualPull* nextToInnermostPixelLayerHitResidual(const Trk::Track* ) const=0;
    virtual const Trk::ResidualPull* nextToInnermostPixelLayerHitResidual(const Trk::TrackParticleBase*) const=0;
    
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParticleBase*, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::Track*, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const=0;


  };
  
  
}// end namespace

#endif
