/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IDSCANZFINDER_TRIGZFINDERALG_H
#define IDSCANZFINDER_TRIGZFINDERALG_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// STL includes
#include <string>

#include "TrkSpacePoint/SpacePointContainer.h"

// Include tool handling
#include "TrigZFinder.h"

// For m_pixelHelperKey
#include "InDetIdentifier/PixelID.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/TrigComposite.h"

#include "AthenaBaseComps/AthAlgTool.h"
//New athenaMT monitoring
#include "AthenaMonitoringKernel/Monitored.h"

// Tiny wrapper algorithm to call ZFinder tool.
/**
 * @class TrigZFinderAlg
 * @brief 
 **/
class TrigZFinderAlg : public AthReentrantAlgorithm {
public:
  TrigZFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigZFinderAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  ToolHandle<TrigZFinder> m_zFinderTool{this, "ZFinderTool", "TrigZFinder/TrigZFinder"};
  SG::ReadHandleKey<SpacePointContainer> m_pixelSpKey{this, "PixelSpKey", "PixelTrigSpacePoints", " "};
  SG::ReadHandleKey<PixelID> m_pixelHelperKey{this, "pixelHelperKey", "DetectorStore+PixelID", " "};
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_zFinderKey{this, "zFinderKey", "Undefined", ""};
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  
};

#endif // IDSCANZFINDER_TRIGZFINDERALG_H
