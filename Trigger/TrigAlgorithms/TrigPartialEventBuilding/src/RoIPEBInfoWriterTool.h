/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigPartialEventBuilding_RoIPEBInfoWriterTool_h
#define TrigPartialEventBuilding_RoIPEBInfoWriterTool_h

#include "TrigPartialEventBuilding/PEBInfoWriterToolBase.h"
#include "IRegionSelector/RegSelEnums.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRegSelTool.h"

/** @class RoIPEBInfoWriterTool
 *  @brief Tool writing ROB list corresponding to an RoI in given detectors for use in PEBInfoWriterAlg
 **/
class RoIPEBInfoWriterTool : public PEBInfoWriterToolBase {
public:
  /// Standard constructor
  RoIPEBInfoWriterTool(const std::string& type, const std::string& name, const IInterface* parent);
  /// Standard destructor
  virtual ~RoIPEBInfoWriterTool() = default;
  /// Implementation of AthAlgTool::initialize()
  virtual StatusCode initialize() override;

protected:
  /// Implementation of PEBInfoWriterToolBase::createPEBInfo
  virtual PEBInfoWriterToolBase::PEBInfo createPEBInfo(const PEBInfoWriterToolBase::Input& input) const override;

private:
  // ------------------------- Service handles ---------------------------------
  ServiceHandle<IRegSelSvc> m_regionSelector {
    this, "RegionSelector", "RegSelSvc/RegSelSvc", "Region Selector service"
  };

  ToolHandle<IRegSelTool> m_regionSelector_pix {
    this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel", "Region Selector Tool"
  };

  ToolHandle<IRegSelTool> m_regionSelector_sct {
    this, "RegSelTool_SCT", "RegSelTool/RegSelTool_SCT", "Region Selector Tool"
  };

  ToolHandle<IRegSelTool> m_regionSelector_trt {
    this, "RegSelTool_TRT", "RegSelTool/RegSelTool_TRT", "Region Selector Tool"
  };



  // ------------------------- Properties --------------------------------------
  Gaudi::Property<float> m_etaEdge {
    this, "EtaEdge", 5.0, "Upper limit of |eta| range"
  };
  Gaudi::Property<float> m_etaWidth {
    this, "EtaWidth", 0.1, "Half-width of the RoI in eta (Eta-EtaWidth; Eta+EtaWidth)"
  };
  Gaudi::Property<float> m_phiWidth {
    this, "PhiWidth", 0.1, "Half-width of the RoI in phi (Phi-PhiWidth; Phi+PhiWidth)"
  };
  Gaudi::Property<std::vector<std::string> > m_detNames {
    this, "DetNames", {},
    "List of detectors from which ROBs in RoI will be included. Naming follows RegSelEnums DETID. "
    "\"All\" is also a possible value."
  }; // No string parsing would be needed if DETID wasn't a global enum and was available in python
  Gaudi::Property<std::vector<uint32_t> > m_extraROBs {
    this, "ExtraROBs", {},
    "Static list of additional ROBs to add for partial event building in each event where the chain passes"
  };
  Gaudi::Property<std::vector<uint32_t> > m_extraSubDets {
    this, "ExtraSubDets", {},
    "Static list of additional SubDets to add for partial event building in each event where the chain passes"
  };
  // ------------------------- Other private members ---------------------------
  /// Static PEB Info which contains ExtraROBs and ExtraSubDets
  PEBInfoWriterToolBase::PEBInfo m_extraPebInfo;
  /// m_detNames translated into set of DETID, filled at initialisation
  std::set<DETID>       m_dets;
  std::set<ToolHandle<IRegSelTool> > m_tools;

};

#endif // TrigPartialEventBuilding_RoIPEBInfoWriterTool_h
