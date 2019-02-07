/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigPartialEventBuilding_StaticPEBInfoWriterTool_h
#define TrigPartialEventBuilding_StaticPEBInfoWriterTool_h

#include "TrigPartialEventBuilding/PEBInfoWriterToolBase.h"

/** @class StaticPEBInfoWriterTool
 *  @brief Tool writing a configurable constant list of ROBs and SubDetectors for use in PEBInfoWriterAlg
 **/
class StaticPEBInfoWriterTool : public PEBInfoWriterToolBase {
public:
  /// Standard constructor
  StaticPEBInfoWriterTool(const std::string& type, const std::string& name, const IInterface* parent);
  /// Standard destructor
  virtual ~StaticPEBInfoWriterTool();
  /// Implementation of AthAlgTool::initialize()
  virtual StatusCode initialize() override;

protected:
  /// Implementation of PEBInfoWriterToolBase::createPEBInfo
  virtual PEBInfoWriterToolBase::PEBInfo createPEBInfo(const PEBInfoWriterToolBase::Input& input) const override;

private:
  /// The list of ROBs added to the PEB Info
  Gaudi::Property<std::vector<uint32_t> > m_robList {
    this, "ROBList", {},
    "The list of ROBs to add for partial event building in each event where the chain passes"
  };
  /// The list of SubDets added to the PEB Info
  Gaudi::Property<std::vector<uint32_t> > m_subDetList {
    this, "SubDetList", {},
    "The list of SubDets to add for partial event building in each event where the chain passes"
  };
  /// Static PEB Info built at initialize and added in every event
  PEBInfoWriterToolBase::PEBInfo m_pebi;
};

#endif // TrigPartialEventBuilding_StaticPEBInfoWriterTool_h
