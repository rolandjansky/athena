/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_DetectorLevelConditionsTool.h
 * interface file for tools that report on the detector or ECC/B/ECA level
 * @author gwilliam@mail.cern.ch
 **/

#ifndef ISCT_DetectorLevelConditionsTool_h
#define ISCT_DetectorLevelConditionsTool_h

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include <list>

class StatusCode;

/**
 * @class SCT_DetectorLevelConditionsTool
 * Interface for tools that report on the detector or ECC/B/ECA level
 **/

class ISCT_DetectorLevelConditionsTool: virtual public IAlgTool {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_DetectorLevelConditionsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_DetectorLevelConditionsTool, 1, 0);
  //@}

  ///Summarise the result of the detector as good/bad
  virtual bool isGood() const =0;
  virtual bool isGood(const EventContext& ctx) const =0;

  ///Summarise the result of the barrel/endcap as good/bad
  //BEC = -2/0/2 for ECC/B/ECA
  virtual bool isGood(int bec) const =0;
  virtual bool isGood(int bec, const EventContext& ctx) const =0;
  
 private:

};

#endif // ISCT_DetectorLevelConditionsTool_h
