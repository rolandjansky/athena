/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IInDetConditionsSvc.h
 * @author shaun.roe@cern.ch
**/
#ifndef IInDetConditionsSvc_h
#define IInDetConditionsSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"

//Local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"

//forward declarations
class Identifier;
class IdentifierHash;
/**
 * @class IInDetConditionsSvc
 * Interface class for service providing summary of status of a detector element
**/
class IInDetConditionsSvc: virtual public IInterface{
public:
  virtual ~IInDetConditionsSvc(){
    //nop, needed for derived classes
  }
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
 
  //@name isActive methods reveal whether the element should return data
  //@{
  virtual bool isActive(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT)=0;
  virtual bool isActive(const IdentifierHash & elementHash)=0;
  virtual bool isActive(const IdentifierHash & elementHash, const Identifier & elementId)=0;
  //@}
  ///activeFraction method gives the fraction of active elements in the range of identifiers
  virtual double activeFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd)=0;  
  //@{
  
  //@name isGood methods reveal whether the data are reliable
   //@{
  virtual bool isGood(const Identifier & elementId, const InDetConditions::Hierarchy h=InDetConditions::DEFAULT)=0;
  virtual bool isGood(const IdentifierHash & elementHash)=0;
  virtual bool isGood(const IdentifierHash & elementHash, const Identifier & elementId)=0;
  //@}
  ///goodFraction method gives the fraction of good elements in the range of identifiers
  virtual double goodFraction(const IdentifierHash & elementHash, const Identifier & idStart, const Identifier & idEnd)=0;
   
};

inline const InterfaceID & IInDetConditionsSvc::interfaceID(){
  static const InterfaceID IID_IInDetConditionsSvc("IInDetConditionsSvc",1,0);
  return IID_IInDetConditionsSvc;
}
#endif
