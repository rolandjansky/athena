// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoTool.h
 * header file for tool allowing one to declare modules as bad
 * @author shaun.roe@cern.ch
 **/

#ifndef SCT_ModuleVetoTool_h
#define SCT_ModuleVetoTool_h

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsData/SCT_ModuleVetoCondData.h"

//Gaudi includes
#include "GaudiKernel/EventContext.h"

//STL includes
#include <vector>

//forward declarations
class SCT_ID;

/**
 * @class SCT_ModuleVetoTool
 * Tool allowing one to manually declare detector elements as 'bad' in the joboptions file
 **/
class SCT_ModuleVetoTool: public extends<AthAlgTool, ISCT_ConditionsTool> {
 public:
  //@name Tool methods
  //@{
  SCT_ModuleVetoTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_ModuleVetoTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  ///Can the service report about the given component? (chip, module...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId) const override;
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual void getDetectorElementStatus(const EventContext& ctx, InDet::SiDetectorElementStatus &element_status, 
                                        SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const override;

 private:
  StringArrayProperty m_badElements{this, "BadModuleIdentifiers", {}, "list of bad detector elements (= module sides)"};
  SCT_ModuleVetoCondData m_localCondData{};
  const SCT_ID* m_pHelper{nullptr};
  bool m_useDatabase{false};
  BooleanProperty m_maskLayers{this, "MaskLayers", false, "Mask full layers/disks in overlay"};
  IntegerProperty m_maskSide{this, "MaskSide", -1, "Mask full modules (-1), inner (0) or outer (1) sides"};
  IntegerArrayProperty m_layersToMask{this, "LayersToMask", {}, "Which barrel layers to mask out, goes from 0 to N-1"};
  IntegerArrayProperty m_disksToMask{this, "DisksToMask", {}, "Which endcap disks to mask out, goes from -N+1 to N+1 , skipping zero"}; 

  // ReadCondHandleKey
  SG::ReadCondHandleKey<SCT_ModuleVetoCondData> m_condKey{this, "CondKey", "SCT_ModuleVetoCondData", "SCT modules to be vetoed"};
  // Fill data from m_badElements
  StatusCode fillData();
  // Provides SCT_ModuleVetoCondData pointer
  const SCT_ModuleVetoCondData* getCondData(const EventContext& ctx) const;

};

#endif // SCT_ModuleVetoTool_h
