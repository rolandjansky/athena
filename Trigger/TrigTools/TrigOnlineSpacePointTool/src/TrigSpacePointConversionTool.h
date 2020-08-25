/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class AtlasDetectorID;
class SCT_ID;
class PixelID;
class IRegSelSvc;
class IBeamCondSvc;

class ITrigL2LayerNumberTool;

class TrigSpacePointConversionTool : virtual public ITrigSpacePointConversionTool, public AthAlgTool {
 public:

  // standard AlgTool methods
  TrigSpacePointConversionTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigSpacePointConversionTool(){};
		
  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();

  //concrete implementations

  virtual StatusCode getSpacePoints(const IRoiDescriptor&, std::vector<TrigSiSpacePointBase>&, int&, int&, std::map<Identifier, std::vector<long int> >*clustermap=nullptr);

 protected:

  ToolHandle<ITrigL2LayerNumberTool> m_layerNumberTool;
  const AtlasDetectorID* m_atlasId;
  const SCT_ID*  m_sctId;
  const PixelID* m_pixelId;
  std::string    m_regionSelectorName;
  IRegSelSvc*    m_regionSelector;
  IBeamCondSvc*  m_beamCondSvc; 

  std::string m_pixelSpContName,m_sctSpContName;// offline/EF containers
  SG::ReadHandleKey<SpacePointContainer> m_sctSpacePointsContainerKey;
  SG::ReadHandleKey<SpacePointContainer> m_pixelSpacePointsContainerKey;
  bool m_filter_phi;
  bool m_useBeamTilt;
  bool m_useNewScheme;
  bool m_usePixelSpacePoints;

  void shiftSpacePoints(std::vector<TrigSiSpacePointBase>&);
  void transformSpacePoints(std::vector<TrigSiSpacePointBase>&);

};
#endif
