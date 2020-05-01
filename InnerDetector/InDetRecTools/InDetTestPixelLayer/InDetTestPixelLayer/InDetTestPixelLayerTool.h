/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    //    bool expectHitInPixelLayer(const Rec::TrackParticle*) const ;
    virtual bool expectHitInPixelLayer(const Trk::TrackParticleBase*, int pixel_layer, bool recompute=false) const override;
    virtual bool expectHitInPixelLayer(const Trk::Track*, int pixel_layer, bool recompute=false) const override;
    virtual bool expectHitInPixelLayer(const Trk::TrackParameters* trackpar, int pixel_layer) const override;

    virtual bool expectHit(const Trk::TrackParameters* trackpar) const override;

    virtual const Trk::ResidualPull* pixelLayerHitResidual(const Trk::Track*, int pixel_layer=0) const override;
    virtual const Trk::ResidualPull* pixelLayerHitResidual(const Trk::TrackParticleBase*, int pixel_layer=0) const override;

    //// return false if extrapolation failed
    virtual bool getTrackStateOnPixelLayerInfo(const Trk::TrackParticleBase*,
					       std::vector<TrackStateOnPixelLayerInfo>& infoList) const override;
    virtual bool getTrackStateOnPixelLayerInfo(const Trk::Track*,
					       std::vector<TrackStateOnPixelLayerInfo>& infoList) const override;
    virtual bool getTrackStateOnPixelLayerInfo(const Trk::TrackParameters* trackpar,
					       std::vector<TrackStateOnPixelLayerInfo>& infoList) const override;


    virtual double getFracGood(const Trk::TrackParticleBase* trackpar, int pixel_layer) const override;
    virtual double getFracGood(const Trk::TrackParameters* trackpar, int pixel_layer) const override;


  private:
    PixelLayerResidualPullMap pixelLayerHitResiduals(const Trk::TrackParticleBase*) const ;
    PixelLayerResidualPullMap pixelLayerHitResiduals(const Trk::Track* ) const ;

    bool IsInCorrectLayer(Identifier&,PixelIDVec&, int) const;
    bool IsInSameLayer(Identifier&,Identifier&) const;

    bool isActive(const Trk::TrackParameters* trackpar) const ;
    bool getPixelLayerParameters(const Trk::TrackParameters* trackpar,
                                 std::vector<std::unique_ptr<const Trk::TrackParameters> >& pixelLayerParam) const;
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
  };
  
  
}// end namespace

#endif
