/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPixelConditionsSvc_h
#define IPixelConditionsSvc_h
//STL includes
//#include <list>
//#include <string>

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//local includes
//#include "InDetConditionsSummaryService/InDetHierarchy.h"

//forward declarations
class Identifier;
class IdentifierHash;

/**
 * @class IPixelConditionsSvc
 * Base class for SCT conditions services so they can be used in the summary service
**/
class IPixelConditionsSvc: virtual public IInterface{
public:
  virtual ~IPixelConditionsSvc(){}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface

  /*  
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
   virtual bool canReportAbout(InDetConditions::Hierarchy h) = 0;
  
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier & elementId,InDetConditions::Hierarchy h=InDetConditions::DEFAULT) = 0;
  
  //@todo introduce hash identifier method
  virtual bool isGood(const IdentifierHash & hashId) = 0;

  //these next two/three methods could be improved...work it out.
  ///Fill the data structures, if not overridden then return failure
  virtual StatusCode fillData()=0;

  //Fill the data structures from a callback, the arguments are needed by IOVDbSvc, 
  //  if not overridden then return failure
  //
  virtual StatusCode fillData(int& i , std::list<std::string>& l)=0;

  ///Report whether the structure was filled
  virtual bool filled() const = 0;

  ///Report whether the tool can fill its data during the initialize phase
  virtual bool canFillDuringInitialize()=0;
  */
  
};

inline const InterfaceID & IPixelConditionsSvc::interfaceID(){
  static const InterfaceID IID("IPixelConditionsSvc",1,0);
  return IID;
}

#endif
