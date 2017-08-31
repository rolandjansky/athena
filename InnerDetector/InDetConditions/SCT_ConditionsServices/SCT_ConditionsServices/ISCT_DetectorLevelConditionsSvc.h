/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_DetectorLevelConditionsSvc.h
 * interface file for services that report on the detector or ECC/B/ECA level
 * @author gwilliam@mail.cern.ch
 **/

#ifndef ISCT_DetectorLevelConditionsSvc_h
#define ISCT_DetectorLevelConditionsSvc_h

#include <list>

class StatusCode;

/**
 * @class SCT_DetectorLevelConditionsSvc
 * Interface for services that report on the detector or ECC/B/ECA level
 **/

class ISCT_DetectorLevelConditionsSvc: virtual public IInterface {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_DetectorLevelConditionsSvc() {}

  static const InterfaceID& interfaceID();

  //@}

  ///Summarise the result of the detector as good/bad
  virtual bool isGood()=0;

  ///Summarise the result of the barrel/endcap as good/bad
  //BEC = -2/0/2 for ECC/B/ECA
  virtual bool isGood(int bec)=0;
  
  ///Fill the data structures, if not overridden then return failure
  virtual StatusCode fillData()=0;
  
  /**Fill the data structures from a callback, the arguments are needed by IOVDbSvc, 
   * if not overridden then return failure
   **/
  virtual StatusCode fillData(int& i, std::list<std::string>& l)=0;
  
  ///Report whether the structure was filled
  virtual bool filled() const =0;

  ///Report whether the tool can fill its data during the initialize phase
  virtual bool canFillDuringInitialize()=0;


 private:

};

inline const InterfaceID& ISCT_DetectorLevelConditionsSvc::interfaceID() {
  static const InterfaceID IID_SCT_DetectorLevelConditionsSvc{"SCT_DetectorLevelConditionsSvc", 1, 0};
  return IID_SCT_DetectorLevelConditionsSvc;
}

#endif // ISCT_DetectorLevelConditionsSvc_h
