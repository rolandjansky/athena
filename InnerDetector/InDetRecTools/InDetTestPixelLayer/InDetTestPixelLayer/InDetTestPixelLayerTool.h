/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTestPixelLayerTool_H
#define InDetTestPixelLayerTool_H

#include "InDetRecToolInterfaces/IInDetTestPixelLayerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "InDetTestPixelLayer/TrackStateOnPixelLayerInfo.h"

#include "InDetTestPixelLayer/PixelIDLayerComp.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <mutex>
#include <string>
#include <vector>

namespace Trk {  class Track; class IExtrapolator;  class TrackParticleBase; class IResidualPullCalculator;}
namespace Rec { class TrackParticle; }
class AtlasDetectorID;
class Identifier;
class PixelID;

namespace InDet {

  typedef std::map<Identifier,const Trk::ResidualPull*,PixelIDLayerComp> PixelLayerResidualPullMap;
  typedef std::vector<Identifier> PixelIDVec;

  class InDetTestPixelLayerTool : virtual public IInDetTestPixelLayerTool, public AthAlgTool  {
    
  public: 

    InDetTestPixelLayerTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    virtual ~InDetTestPixelLayerTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    //    bool expectHitInPixelLayer(const Rec::TrackParticle*) const ;
    virtual bool expectHitInPixelLayer(const Trk::TrackParticleBase*, int pixel_layer, bool recompute=false) const;
    virtual bool expectHitInPixelLayer(const Trk::Track*, int pixel_layer, bool recompute=false) const;
    virtual bool expectHitInPixelLayer(const Trk::TrackParameters* trackpar, int pixel_layer) const;

    virtual bool expectHit(const Trk::TrackParameters* trackpar) const;

    virtual const Trk::ResidualPull* pixelLayerHitResidual(const Trk::Track*, int pixel_layer=0) const;
    virtual const Trk::ResidualPull* pixelLayerHitResidual(const Trk::TrackParticleBase*, int pixel_layer=0) const;

    //// return false if extrapolation failed
    virtual bool getTrackStateOnPixelLayerInfo(const Trk::TrackParticleBase*,
					       std::vector<TrackStateOnPixelLayerInfo>& infoList) const;
    virtual bool getTrackStateOnPixelLayerInfo(const Trk::Track*,
					       std::vector<TrackStateOnPixelLayerInfo>& infoList) const;
    virtual bool getTrackStateOnPixelLayerInfo(const Trk::TrackParameters* trackpar,
					       std::vector<TrackStateOnPixelLayerInfo>& infoList) const;


    virtual double getFracGood(const Trk::TrackParticleBase* trackpar, int pixel_layer) const;
    virtual double getFracGood(const Trk::TrackParameters* trackpar, int pixel_layer) const;
    virtual double getFracGood() const;

    virtual double getExtrapolPixelEta() const;
    virtual double getExtrapolPixelPhi() const;

  private:

    void pixelLayerHitResiduals(const Trk::TrackParticleBase*) const ;
    void pixelLayerHitResiduals(const Trk::Track* ) const ;

    bool IsInCorrectLayer(Identifier&,PixelIDVec&, int) const;
    bool IsInSameLayer(Identifier&,Identifier&) const;

    bool isActive(const Trk::TrackParameters* trackpar) const ;
    bool getPixelLayerParameters(const Trk::TrackParameters* trackpar, std::vector<const Trk::TrackParameters*>& pixelLayerParam) const;
    double getFracGood(const Trk::TrackParameters* trackpar, double phiRegionSize, double etaRegionSize) const;

    /** Pointer to Extrapolator AlgTool*/
    ToolHandle< Trk::IExtrapolator >  m_extrapolator;    

    /** Handles to IConditionsSummaryServices for Pixels */
    ToolHandle <IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelSummaryTool", "PixelConditionsSummaryTool/InDetPixelConditionsSummaryTool", "Tool to retrieve Pixel Conditions summary"};

    /** pointer to the residual pull calculator **/
    ToolHandle < Trk::IResidualPullCalculator > m_residualPullCalculator;

    /** detector helper*/
    const AtlasDetectorID* m_idHelper;

    /**ID pixel helper*/
    const PixelID* m_pixelId;

    bool m_checkActiveAreas;
    bool m_configured;

    bool m_checkDeadRegions;
    bool m_checkDisabledFEs;
    double m_phiRegionSize;
    double m_etaRegionSize;
    double m_goodFracCut;

    /** For cached objects */
    struct CacheEntry {
      EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
      double m_cachedFracGood;
      const Trk::TrackParameters* m_cachedTrkParam;
      const Trk::Track* m_cached_track_residual;
      PixelLayerResidualPullMap m_residual_map;
      std::recursive_mutex m_mutex;
      void checkEvent(const EventContext& ctx) {
        if (m_evt!=ctx.evt()) {
          m_cachedFracGood = -9999.;
          m_cachedTrkParam = nullptr;
          m_cached_track_residual = nullptr;
          m_residual_map.clear();
          m_evt = ctx.evt();
        }
      }
    };
    mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
  };
  
  
}// end namespace

#endif
