/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ConditionsParameterSvc.h
 * header file for baseclass of SCT conditions services
 * @author shaun.roe@cern.ch
 **/

#ifndef ISCT_ConditionsParameterSvc_h
#define ISCT_ConditionsParameterSvc_h

//STL includes
#include <vector>

//Gaudi Includes
#include "GaudiKernel/IInterface.h"

//local includes
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

//fwd declarations
class IdentifierHash;

/**
 * @class ISCT_ConditionsParameterSvc
 * Base class to give a numerical value from conditions for each detector element (module side)
 **/
class ISCT_ConditionsParameterSvc: virtual public IInterface {
 public:
  virtual ~ISCT_ConditionsParameterSvc() {}
  static const InterfaceID& interfaceID(); //!< reimplemented from IInterface
  
  ///Is the required parameter available?
  virtual bool available(const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Give the indicated value for a module identifier hash
  virtual float value(const IdentifierHash& idHash, const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
  virtual float validity(const IdentifierHash& idHash, const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Is a given value within acceptable limits?
  virtual bool isValid(const float parameterValue, const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///What is the default error value for this parameter?
  virtual float invalid(const unsigned int iparam) const =0;
  
  ///Maximum value read in from the database
  virtual float max(const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Minimum value read in from the database
  virtual float min(const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Average value
  virtual float avg(const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Standard deviation
  virtual float sd(const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Number of values read in
  virtual unsigned int n(const SCT_ConditionsServices::ParameterIndex iparam)=0;
  
  ///Fill a user-provided vector with the values (hopefully won't be needed?)
  virtual void getValues(std::vector<float>& userVector, const SCT_ConditionsServices::ParameterIndex iparam)=0;
 
  ///Report whether the structure was filled
  virtual bool filled() const =0;
};


inline const InterfaceID& ISCT_ConditionsParameterSvc::interfaceID() {
  static const InterfaceID IID{"ISCT_ConditionsParameterSvc", 1, 0};
  return IID;
}

#endif // ISCT_ConditionsParameterSvc_h
