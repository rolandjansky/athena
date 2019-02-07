/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ConditionsParameterTool.h
 * header file for baseclass of SCT conditions parameter tool
 * @author shaun.roe@cern.ch
 **/

#ifndef ISCT_ConditionsParameterTool_h
#define ISCT_ConditionsParameterTool_h

//Athena includes
#include "SCT_ConditionsData/SCT_CondParameterData.h"
#include "Identifier/IdentifierHash.h"

//Gaudi Includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"

//STL includes
#include <vector>

/**
 * @class ISCT_ConditionsParameterSvc
 * Base class to give a numerical value from conditions for each detector element (module side)
 **/
class ISCT_ConditionsParameterTool: virtual public IAlgTool {
 public:
  virtual ~ISCT_ConditionsParameterTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ConditionsParameterTool, 1, 0);
  
  ///Is the required parameter available?
  virtual bool available(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual bool available(const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Give the indicated value for a module identifier hash
  virtual float value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual float value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
  virtual float validity(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual float validity(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Is a given value within acceptable limits?
  virtual bool isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual bool isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///What is the default error value for this parameter?
  virtual float invalid(const unsigned int iparam, const EventContext& ctx) const =0;
  virtual float invalid(const unsigned int iparam) const =0;
  
  ///Maximum value read in from the database
  virtual float max(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual float max(const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Minimum value read in from the database
  virtual float min(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual float min(const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Average value
  virtual float avg(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual float avg(const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Standard deviation
  virtual float sd(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual float sd(const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Number of values read in
  virtual unsigned int n(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual unsigned int n(const SCT_CondParameterData::ParameterIndex iparam) const =0;
  
  ///Fill a user-provided vector with the values (hopefully won't be needed?)
  virtual void getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const =0;
  virtual void getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam) const =0;
 
  ///Report whether the structure was filled
  virtual bool filled(const EventContext& ctx) const =0;
  virtual bool filled() const =0;
};

#endif // ISCT_ConditionsParameterTool_h
