/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTestBLayerTool_H
#define InDetTestBLayerTool_H

#include "InDetRecToolInterfaces/IInDetTestBLayerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "InDetTestBLayer/TrackStateOnBLayerInfo.h"

#include <vector>
#include <string>

namespace Trk {  class Track; class IExtrapolator;  class TrackParticleBase; class IResidualPullCalculator;}
namespace Rec { class TrackParticle; }
class IInDetConditionsSvc;
class AtlasDetectorID;
class Identifier;
class PixelID;

namespace InDet {


  class InDetTestBLayerTool : virtual public IInDetTestBLayerTool, public AthAlgTool  {
    
  public: 

    InDetTestBLayerTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    virtual ~InDetTestBLayerTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    //    bool expectHitInBLayer(const Rec::TrackParticle*) const ;
    virtual bool expectHitInBLayer(const Trk::TrackParticleBase*, bool recompute=false) const ;
    virtual bool expectHitInBLayer(const Trk::Track*, bool recompute=false) const ;
    virtual bool expectHitInBLayer(const Trk::TrackParameters* trackpar) const ;

    virtual const Trk::ResidualPull* bLayerHitResidual(const Trk::Track* ) const ;
    virtual const Trk::ResidualPull* bLayerHitResidual(const Trk::TrackParticleBase*) const ;

    //// return false if extrapolation failed
    virtual bool getTrackStateOnBlayerInfo(const Trk::TrackParticleBase*, 
					   std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    virtual bool getTrackStateOnBlayerInfo(const Trk::Track*, 
					   std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    virtual bool getTrackStateOnBlayerInfo(const Trk::TrackParameters* trackpar, 
					   std::vector<TrackStateOnBLayerInfo>& infoList)  const;

    virtual bool expectHitInInnermostPixelLayer(const Trk::TrackParticleBase*, bool recompute=false) const;
    virtual bool expectHitInInnermostPixelLayer(const Trk::Track*, bool recompute=false) const;
    virtual bool expectHitInInnermostPixelLayer(const Trk::TrackParameters* trackpar) const;

    virtual const Trk::ResidualPull* innermostPixelLayerHitResidual(const Trk::Track* ) const;
    virtual const Trk::ResidualPull* innermostPixelLayerHitResidual(const Trk::TrackParticleBase*) const;
 
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParticleBase*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::Track*, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    virtual bool getTrackStateOnInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, 
				   std::vector<TrackStateOnBLayerInfo>& infoList)  const;

    virtual bool expectHitInNextToInnermostPixelLayer(const Trk::TrackParticleBase*, bool recompute=false) const;
    virtual bool expectHitInNextToInnermostPixelLayer(const Trk::Track*, bool recompute=false) const;
    virtual bool expectHitInNextToInnermostPixelLayer(const Trk::TrackParameters* trackpar) const;
    
    virtual const Trk::ResidualPull* nextToInnermostPixelLayerHitResidual(const Trk::Track* ) const;
    virtual const Trk::ResidualPull* nextToInnermostPixelLayerHitResidual(const Trk::TrackParticleBase*) const;
    
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParticleBase*, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::Track*, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(const Trk::TrackParameters* trackpar, 
							std::vector<TrackStateOnBLayerInfo>& infoList)  const;
    

  private:

    bool isActive(const Trk::TrackParameters* trackpar) const ;
    bool getPixelLayerParameters(const Trk::TrackParameters* trackpar, std::vector<const Trk::TrackParameters*>& blayerParam, int layer) const;
    double getFracGood(const Trk::TrackParameters* trackpar, double phiRegionSize, double etaRegionSize) const;

    /** Pointer to Extrapolator AlgTool*/
    ToolHandle< Trk::IExtrapolator >  m_extrapolator;    

    /** Handles to IConditionsSummaryServices for Pixels */
    ServiceHandle <IInDetConditionsSvc> m_pixelCondSummarySvc;

    /** pointer to the residual pull calculator **/
    ToolHandle < Trk::IResidualPullCalculator > m_residualPullCalculator;

    /** detector helper*/
    const AtlasDetectorID* m_idHelper;

    /**ID pixel helper*/
    const PixelID* m_pixelId;

    bool m_checkActiveAreas;
    bool m_configured;

    bool m_checkDeadRegions;
    double m_phiRegionSize;
    double m_etaRegionSize;
    double m_goodFracCut;
    bool m_checkAtLeastNearestNeighbors;
  };
  
  
}// end namespace

#endif
