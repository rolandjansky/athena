/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYCLASSIFYTOOL_H 
#define TRIGGER_TGCCOINHIERARCHYCLASSIFYTOOL_H 

#include "TrigMuonCoinHierarchy/ITgcCoinHierarchyClassifyTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace Trigger {

class TgcCoinHierarchyClassifyTool : virtual public ITgcCoinHierarchyClassifyTool, virtual public AthAlgTool {
 public:
  /** Constructor */
  TgcCoinHierarchyClassifyTool(const std::string& t, const std::string& n, const IInterface* p);

  virtual ~TgcCoinHierarchyClassifyTool()=default;

  /** Query Interface */
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  /** Provide InterfaceID */
  static const InterfaceID& interfaceID() { return ITgcCoinHierarchyClassifyTool::interfaceID(); }

  /** Initialize */
  virtual StatusCode initialize();

  /** Make TgcCoinHierarchyTriggerSector's from TgcCoinDataContainer and TgcPrepDataContainer */
  virtual StatusCode classify(std::vector<TgcCoinHierarchyTriggerSector*>* pTrigSectorCombs);

  /** Get TgcCoinHierarchy::STATION enum from an Indentifier */
  virtual TgcCoinHierarchy::STATION getSTATION(const Identifier identify) const;
  /** Get trigger sector from an Identifier */
  virtual bool getTriggerSector(const Identifier identify, unsigned int& isAside, unsigned int& isForward, unsigned int& phi) const;
  /** Get isStrip from an Identifier */
  virtual bool isStrip(const Identifier identify) const;

  private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  /** Map from trigger sector to TgcCoinHierarchyTriggerSector number */
  int m_idVector[TgcCoinHierarchy::NSIDES][TgcCoinHierarchy::NREGIONS][TgcCoinHierarchy::NPHIS][TgcCoinHierarchy::NTIMING];
  /** TgcCoinHierarchy instance used for conversions of enum's etc. */
  TgcCoinHierarchy m_TCH;
  
  /** TgcCoinDataContainer name */
  std::string m_tgcCoinDataContainerName[3];
  /** TgcPrepDataContainer name */
  std::string m_tgcPrepDataContainerName[3];

};

} // end of namespace Trigger 

#endif // TRIGGER_TGCCOINHIERARCHYCLASSIFYTOOL_H
