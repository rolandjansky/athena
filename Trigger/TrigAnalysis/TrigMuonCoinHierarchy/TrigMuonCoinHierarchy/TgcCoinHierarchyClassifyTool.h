/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYCLASSIFYTOOL_H 
#define TRIGGER_TGCCOINHIERARCHYCLASSIFYTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigMuonCoinHierarchy/ITgcCoinHierarchyClassifyTool.h"
#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

class TgcIdHelper;
class Identifier;

namespace Trigger {

class TgcCoinHierarchyClassifyTool : virtual public ITgcCoinHierarchyClassifyTool, virtual public AthAlgTool {
 public:
  /** Constructor */
  TgcCoinHierarchyClassifyTool(const std::string& t, const std::string& n, const IInterface* p);

  /** Destructor */
  virtual ~TgcCoinHierarchyClassifyTool(void) {};

  /** Query Interface */
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  /** Provide InterfaceID */
  static const InterfaceID& interfaceID() { return ITgcCoinHierarchyClassifyTool::interfaceID(); }

  /** Initialize */
  virtual StatusCode initialize();
  /** Finalize */
  virtual StatusCode finalize();

  /** Make TgcCoinHierarchyTriggerSector's from TgcCoinDataContainer and TgcPrepDataContainer */
  virtual StatusCode classify(std::vector<TgcCoinHierarchyTriggerSector*>* pTrigSectorCombs);

  /** Get TgcCoinHierarchy::STATION enum from an Indentifier */
  virtual TgcCoinHierarchy::STATION getSTATION(const Identifier identify) const;
  /** Get trigger sector from an Identifier */
  virtual bool getTriggerSector(const Identifier identify, unsigned int& isAside, unsigned int& isForward, 
                                unsigned int& phi) const;
  /** Get isStrip from an Identifier */
  virtual bool isStrip(const Identifier identify) const;

  private:
  /** TgcIdHelper */
  const TgcIdHelper* m_tgcHelper;
  /** Map from trigger sector to TgcCoinHierarchyTriggerSector number */
  int m_idVector[TgcCoinHierarchy::NSIDES][TgcCoinHierarchy::NREGIONS][TgcCoinHierarchy::NPHIS][TgcCoinHierarchy::NTIMING];
  /** TgcCoinHierarchy instance used for conversions of enum's etc. */
  TgcCoinHierarchy m_TCH;
  
  /** TgcCoinDataContainer name */
  std::string m_tgcCoinDataContainerName[3];
  //std::string m_tgcCoinDataContainerName;
  /** TgcPrepDataContainer name */
  std::string m_tgcPrepDataContainerName[3];
  //std::string m_tgcPrepDataContainerName;

};

} // end of namespace Trigger 

#endif // TRIGGER_TGCCOINHIERARCHYCLASSIFYTOOL_H
