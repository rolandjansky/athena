/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTestBLayerTool_H
#define InDetTestBLayerTool_H

#include "InDetRecToolInterfaces/IInDetTestBLayerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ThreadLocalContext.h" //for Gaudi::Hive::currentContext()
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include <vector>
#include <string>
#include <memory>

namespace Trk {
  class Track;
  class ResidualPull;
}

class AtlasDetectorID;
class PixelID;
class TrackStateOnBLayerInfo;
class EventContext;

namespace InDet {


  class InDetTestBLayerTool : virtual public IInDetTestBLayerTool, public AthAlgTool  {
    
  public:
    InDetTestBLayerTool(const std::string& name, const std::string& n, const IInterface* p);
    virtual ~InDetTestBLayerTool();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual bool expectHitInBLayer(const EventContext& ctx,
                                   const Trk::Track*,
                                   bool recompute = false) const override final;

    virtual bool expectHitInBLayer(
      const Trk::TrackParameters* trackpar) const override final;

    virtual const Trk::ResidualPull* bLayerHitResidual(
      const Trk::Track*) const override;

    //// return false if extrapolation failed
    virtual bool getTrackStateOnBlayerInfo(const Trk::Track*,
                                           std::vector<TrackStateOnBLayerInfo>& infoList) const override;
    virtual bool getTrackStateOnBlayerInfo(const Trk::TrackParameters* trackpar,
                                           std::vector<TrackStateOnBLayerInfo>& infoList) const override;

    virtual bool expectHitInInnermostPixelLayer(
      const EventContext& ctx,
      const Trk::Track* track,
      bool recompute = false) const override final;
    virtual bool expectHitInInnermostPixelLayer(
      const Trk::TrackParameters* trackpar) const override final;

    virtual const Trk::ResidualPull* innermostPixelLayerHitResidual(const Trk::Track*) const override;

  
    virtual bool getTrackStateOnInnermostPixelLayerInfo(
      const Trk::Track*,
      std::vector<TrackStateOnBLayerInfo>& infoList) const override;
    
    virtual bool getTrackStateOnInnermostPixelLayerInfo(
      const Trk::TrackParameters* trackpar,
      std::vector<TrackStateOnBLayerInfo>& infoList) const override;

    virtual bool expectHitInNextToInnermostPixelLayer(
      const EventContext& ctx,
      const Trk::Track*,
      bool recompute = false) const override final;
    virtual bool expectHitInNextToInnermostPixelLayer(
      const Trk::TrackParameters* trackpar) const override final;

    virtual const Trk::ResidualPull* nextToInnermostPixelLayerHitResidual(const Trk::Track*) const override;

    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(
      const Trk::Track*,
      std::vector<TrackStateOnBLayerInfo>& infoList) const override;
    virtual bool getTrackStateOnNextToInnermostPixelLayerInfo(
      const Trk::TrackParameters* trackpar,
      std::vector<TrackStateOnBLayerInfo>& infoList) const override;

  private:
    bool expectHitInPixelLayer(const EventContext& ctx,
                               const Trk::Track*,
                               int layer,
                               bool recompute = false) const;
    bool expectHitInPixelLayer(const EventContext& ctx,
                               const Trk::TrackParameters* trackpar,
                               int layer) const;
    bool expectHitInPixelLayer(const Trk::TrackParameters* trackpar,
                               int layer) const
    {
      return expectHitInPixelLayer(
        Gaudi::Hive::currentContext(), trackpar, layer);
    }

    const Trk::ResidualPull* pixelLayerHitResidual(const Trk::Track*, int layer) const;
   

    bool getTrackStateOnPixelLayerInfo(const Trk::Track*,
                                       std::vector<TrackStateOnBLayerInfo>& infoList,
                                       int layer) const;
    bool getTrackStateOnPixelLayerInfo(const Trk::TrackParameters* trackpar,
                                       std::vector<TrackStateOnBLayerInfo>& infoList,
                                       int layer) const;

    bool isActive(const Trk::TrackParameters* trackpar) const;

    bool getPixelLayerParameters(
      const EventContext& ctx,
      const Trk::TrackParameters* trackpar,
      std::vector<std::unique_ptr<const Trk::TrackParameters>>& blayerParam,
      int layer) const;

    bool getPixelLayerParameters(
      const Trk::TrackParameters* trackpar,
      std::vector<std::unique_ptr<const Trk::TrackParameters>>& blayerParam,
      int layer) const
    {
      return getPixelLayerParameters(
        Gaudi::Hive::currentContext(), trackpar, blayerParam, layer);
    }

    double getFracGood(const Trk::TrackParameters* trackpar,
                       double phiRegionSize,
                       double etaRegionSize) const;

    /** Handle to Extrapolator AlgTool*/
    ToolHandle<Trk::IExtrapolator> m_extrapolator{
      this,
      "Extrapolator",
      {},
      "Extrapolator used to extrapolate to layers"
    };

    /** Handles to IConditionsSummaryServices for Pixels */
    ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{
      this,
      "PixelSummaryTool",
      "PixelConditionsSummaryTool/InDetPixelConditionsSummaryTool",
      "Tool to retrieve Pixel Conditions summary"
    };

    /** Handle to the residual pull calculator **/
    ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator{
      this,
      "ResidualPullCalculator",
      "Trk::ResidualPullCalculator/ResidualPullCalculator",
      "Calculate Residuals"
    };

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
    static const std::string s_layerNames[2];
  };
  
  
}// end namespace

#endif
