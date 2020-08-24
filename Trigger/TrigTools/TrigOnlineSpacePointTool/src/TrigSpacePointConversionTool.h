/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_SPACEPOINT_CONVERSION_TOOL__
#define __TRIG_SPACEPOINT_CONVERSION_TOOL__

#include "GaudiKernel/ToolHandle.h"
#include "TrigInDetToolInterfaces/ITrigSpacePointConversionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include <string>
#include <vector>

#include "TrkSpacePoint/SpacePointContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"


#include "IRegionSelector/IRegSelTool.h"

class AtlasDetectorID;
class SCT_ID;
class PixelID;


class ITrigL2LayerNumberTool;

class TrigSpacePointConversionTool : virtual public ITrigSpacePointConversionTool, public AthAlgTool {
 public:

  // standard AlgTool methods
  TrigSpacePointConversionTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigSpacePointConversionTool(){};
		
  // standard Athena methods
  StatusCode initialize() override;
  StatusCode finalize() override;

  //concrete implementations

  virtual StatusCode getSpacePoints(const IRoiDescriptor&, std::vector<TrigSiSpacePointBase>&, int&, int&, const EventContext& ctx) const override final;

 protected:

  ToolHandle<ITrigL2LayerNumberTool> m_layerNumberTool;

  const AtlasDetectorID* m_atlasId;
  const SCT_ID*  m_sctId;
  const PixelID* m_pixelId;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

  std::string m_pixelSpContName,m_sctSpContName;// offline/EF containers
  SG::ReadHandleKey<SpacePointContainer> m_sctSpacePointsContainerKey;
  SG::ReadHandleKey<SpacePointContainer> m_pixelSpacePointsContainerKey;

  bool m_filter_phi;
  bool m_useBeamTilt;
  bool m_useNewScheme;

  void shiftSpacePoints(std::vector<TrigSiSpacePointBase>&, const EventContext&) const;
  void transformSpacePoints(std::vector<TrigSiSpacePointBase>&, const EventContext&) const;

  /// new region selector tools
  ToolHandle<IRegSelTool> m_regsel_pix;
  ToolHandle<IRegSelTool> m_regsel_sct;

};
#endif
