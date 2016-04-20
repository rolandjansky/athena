/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXELCONDITIONSSUMMARYSVC_H
#define PIXELCONDITIONSSERVICES_PIXELCONDITIONSSUMMARYSVC_H

#include <string>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
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



class PixelConditionsSummarySvc: public AthService, virtual public IInDetConditionsSvc{

 public:
  static const InterfaceID& interfaceID();

  PixelConditionsSummarySvc(const std::string& name, ISvcLocator* sl);
  virtual ~PixelConditionsSummarySvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);

  virtual bool isActive(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  virtual bool isActive(const IdentifierHash & elementHash);
  virtual bool isActive(const IdentifierHash & elementHash, const Identifier & elementId);
  virtual double activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd);

  virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  virtual bool isGood(const IdentifierHash & elementHash);
  virtual bool isGood(const IdentifierHash & elementHash, const Identifier & elementId);
  virtual double goodFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd);

  virtual StatusCode specialPixelMapCallBack(IOVSVC_CALLBACK_ARGS);

private:
  ServiceHandle< StoreGateSvc > m_detStore;

  const PixelID* m_pixelID;

  std::string m_specialPixelMapKey;

  const DetectorSpecialPixelMap* m_specialPixelMap;

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

inline const InterfaceID& PixelConditionsSummarySvc::interfaceID(){
  static const InterfaceID IID_PixelConditionsSummarySvc("PixelConditionsSummarySvc", 1, 0);
  return IID_PixelConditionsSummarySvc;
}

#endif
