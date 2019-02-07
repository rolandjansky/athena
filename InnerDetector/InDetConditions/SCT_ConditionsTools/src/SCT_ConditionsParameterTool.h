/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameterTool.h
 * header file for SCT conditions tool giving numerical parameters
 * @author shaun.roe@cern.ch
 **/
#ifndef SCT_ConditionsParameterTool_h
#define SCT_ConditionsParameterTool_h
//STL includes
#include <list>
#include <vector>

//boost include
#include "boost/array.hpp"

//Gaudi Includes
#include "AthenaBaseComps/AthAlgTool.h"

//Athena includes
#include "StoreGate/ReadCondHandleKey.h"

//InnerDetector includes
#include "SCT_ConditionsData/SCT_CondParameterData.h"

//local includes
#include "SCT_ConditionsTools/ISCT_ConditionsParameterTool.h"

/**
 * @class SCT_ConditionsParameterTool
 * Class to give a numerical value from conditions for each detector element (module side)
 **/
class SCT_ConditionsParameterTool: public extends<AthAlgTool, ISCT_ConditionsParameterTool> {
 public:
  ///Constructor necessary for Svc factory
  SCT_ConditionsParameterTool(const std::string &type, const std::string &name, const IInterface *parent);

  ///(virtual) Destructor does nothing
  virtual ~SCT_ConditionsParameterTool() = default;
  
  //@name Methods implemented from virtual baseclass methods
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  ///Is the required parameter available?
  virtual bool available(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual bool available(const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Give the indicated value for a module identifier hash
  virtual float value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual float value(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
  virtual float validity(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual float validity(const IdentifierHash& idHash, const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Is a given value within acceptable limits?
  virtual bool isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual bool isValid(const float parameterValue, const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///What is the default error value for this parameter?
  virtual float invalid(const unsigned int iparam, const EventContext& ctx) const override;
  virtual float invalid(const unsigned int iparam) const override;
  
  ///Maximum value read in from the database
  virtual float max(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual float max(const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Minimum value read in from the database
  virtual float min(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual float min(const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Average value
  virtual float avg(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual float avg(const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Standard deviation
  virtual float sd(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual float sd(const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Number of values read in
  virtual unsigned int n(const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual unsigned int n(const SCT_CondParameterData::ParameterIndex iparam) const override;
  
  ///Fill a user-provided vector with the values (hopefully won't be needed?)
  virtual void getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam, const EventContext& ctx) const override;
  virtual void getValues(std::vector<float>& userVector, const SCT_CondParameterData::ParameterIndex iparam) const override;
 
  ///Report whether the structure was filled
  virtual bool filled(const EventContext& ctx) const override;
  virtual bool filled() const override;
  
 private:
  //the data structure
  SG::ReadCondHandleKey<SCT_CondParameterData> m_condKey{this, "CondKey", "SCT_CondParameterData", "SCT parameters"};
  const SCT_CondParameterData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_ConditionsParameterTool_h
