/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_LAYER_NUMBER_TOOL__
#define __TRIG_L2_LAYER_NUMBER_TOOL__

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"

namespace InDetDD {
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

class TrigL2LayerNumberTool : virtual public ITrigL2LayerNumberTool, public AthAlgTool {
 public:

  // standard AlgTool methods
  TrigL2LayerNumberTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigL2LayerNumberTool(){};
		
  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();

  //concrete implementations

  virtual int maxSiliconLayerNum()  {return m_MaxSiliconLayerNum;}
  virtual int offsetEndcapPixels()  {return m_OffsetEndcapPixels;}
  virtual int offsetBarrelSCT()  {return m_OffsetBarrelSCT;}
  virtual int offsetEndcapSCT()  {return m_OffsetEndcapSCT;}
  virtual void report();//prints out the above 

 protected:

  //cached values

  int m_MaxSiliconLayerNum;
  int m_OffsetEndcapPixels;
  int m_OffsetBarrelSCT;
  int m_OffsetEndcapSCT;
  const InDetDD::PixelDetectorManager* m_pixelManager;
  const InDetDD::SCT_DetectorManager* m_sctManager;
};
#endif
