/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXELCONDITIONSSUMMARYTOOL_H
#define PIXELCONDITIONSSERVICES_PIXELCONDITIONSSUMMARYTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"


class Identifier;
class IdentifierHash;
class PixelID;
class IPixelDCSSvc;
class IPixelByteStreamErrorsSvc;
class ISpecialPixelMapSvc;
class DetectorSpecialPixelMap;
class StoreGateSvc;
class IPixelTDAQSvc;
class IBLParameterSvc;


/**
 * Returns the status of pixel modules, frontends or single pixels
 * as determined by the PixelDCSSvc, PixelByteStreamErrorsSvc and SpecialPixelMapSvc.
 *
 * ruwiedel@physik.uni-bonn.de
 *
 */



class PixelConditionsSummaryTool: public AthAlgTool, public IInDetConditionsTool{

 public:
  static InterfaceID& interfaceID();

  PixelConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~PixelConditionsSummaryTool();

  virtual StatusCode initialize() override;

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;

  virtual bool isActive(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
  virtual bool isActive(const IdentifierHash & elementHash) const override final;
  virtual bool isActive(const IdentifierHash & elementHash, const Identifier & elementId)  const override final;
  virtual double activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd)  const override final;

  virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override final;
  virtual bool isGood(const IdentifierHash & elementHash) const override final;
  virtual bool isGood(const IdentifierHash & elementHash, const Identifier & elementId) const override final;
  virtual double goodFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd) const override final;

private:
  ServiceHandle< StoreGateSvc > m_detStore;

  const PixelID* m_pixelID;

  SG::ReadCondHandleKey< DetectorSpecialPixelMap> m_specialPixelMapKey;

  ServiceHandle< ISpecialPixelMapSvc > m_specialPixelMapSvc;
  ServiceHandle< IPixelDCSSvc > m_pixelDCSSvc;
  ServiceHandle< IBLParameterSvc> m_IBLParameterSvc;
  ServiceHandle< IPixelByteStreamErrorsSvc > m_pixelBSErrorsSvc;
  ServiceHandle< IPixelTDAQSvc > m_pixelTDAQSvc;
  std::vector<std::string> m_isActiveStatus;
  std::vector<std::string> m_isActiveStates;

  bool m_useSpecialPixelMap;
  bool m_useDCS;
  bool m_useBS;
  bool m_useTDAQ;

  void getID(const Identifier& id, unsigned int& pixID, unsigned int& mchips) const;

};

inline InterfaceID& PixelConditionsSummaryTool::interfaceID(){
  static InterfaceID IID_PixelConditionsSummaryTool("PixelConditionsSummaryTool", 1, 0);
  return IID_PixelConditionsSummaryTool;
}

#endif
