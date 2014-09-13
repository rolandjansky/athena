/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameterSvc.h
 * header file for SCT conditions service giving numerical parameters
 * @author shaun.roe@cern.ch
**/
#ifndef SCT_ConditionsParameterSvc_h
#define SCT_ConditionsParameterSvc_h
//STL includes
#include <list>
#include <string>
#include <vector>
#include <map>
#include "boost/array.hpp"


//boost include

//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

//Athena includes
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/IIOVDbSvc.h" 
#include "AthenaPoolUtilities/CondAttrListVec.h"

//InnerDetector includes
#include "SCT_Cabling/ISCT_CablingSvc.h" 

//local includes
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"
#include "SCT_ConditionsServices/ISCT_ConditionsParameterSvc.h"

//fwd declarations
template <class TYPE> class SvcFactory;
class ISvcLocator;
class StatusCode;
class SCT_ID;
/**
 * @class SCT_ConditionsParameterSvc
 * Class to give a numerical value from conditions for each detector element (module side)
**/
class SCT_ConditionsParameterSvc: virtual public ISCT_ConditionsParameterSvc, virtual public AthService{
  friend class SvcFactory<SCT_ConditionsParameterSvc>;
public:
  ///Constructor necessary for Svc factory
  SCT_ConditionsParameterSvc(const std::string & name,  ISvcLocator* svc);

  ///(virtual) Destructor does nothing
  virtual ~SCT_ConditionsParameterSvc(){/*nop */}
  
  //@name Methods implemented from virtual baseclass methods
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  //@}
  
  ///Is the required parameter available?
  virtual bool available(const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Give the indicated value for a module identifier hash
  virtual float value(const IdentifierHash & idHash, const SCT_ConditionsServices::ParameterIndex iparam) ;
  
  ///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
  virtual float validity(const IdentifierHash & idHash, const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Is a given value within acceptable limits?
  virtual bool isValid(const float parameterValue, const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///What is the default error value for this parameter?
  virtual float invalid(const unsigned int iparam) const;
  
  ///Maximum value read in from the database
  virtual float max(const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Minimum value read in from the database
  virtual float min(const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Average value
  virtual float avg(const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Standard deviation
  virtual float sd(const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Number of values read in
  virtual unsigned int n(const SCT_ConditionsServices::ParameterIndex iparam);
  
  ///Fill a user-provided vector with the values (hopefully won't be needed?)
  virtual void getValues(std::vector<float> & userVector, const SCT_ConditionsServices::ParameterIndex iparam);
 
  ///Report whether the structure was filled
  virtual bool filled() const;
  
  ///Callback for fill from database
  StatusCode fillData(int&  i  , std::list<std::string>& keys);
  
private:
  enum {N_ELEMENTS=8176};
  //Declare Storegate container
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ISCT_CablingSvc>  m_cablingSvc;//!< Handle on SCT cabling service
  const SCT_ID*   m_pHelper;//!< ID helper for SCT
  
  //the data structure
  boost::array< boost::array<float,SCT_ConditionsServices::N_PARAMETERS>, N_ELEMENTS > m_values;
  //a structure for initialization
  boost::array<float,SCT_ConditionsServices::N_PARAMETERS> m_invalidParameters;
  //DataHandle for callback
  const DataHandle<CondAttrListVec> m_thresholdData;//!<implies that CoraCool is used
  //extended method for data structure insertion
  bool insert(const IdentifierHash & idHash, const SCT_ConditionsServices::ParameterIndex iparam, const float theValue);
  //internal values to be calculated during data filling
  float m_min[SCT_ConditionsServices::N_PARAMETERS], m_max[SCT_ConditionsServices::N_PARAMETERS];
  unsigned int m_n[SCT_ConditionsServices::N_PARAMETERS];
  float m_sum[SCT_ConditionsServices::N_PARAMETERS], m_sumsq[SCT_ConditionsServices::N_PARAMETERS];
  bool m_filled;
};


inline const InterfaceID & SCT_ConditionsParameterSvc::interfaceID(){
  //static const InterfaceID IID("SCT_ConditionsParameterSvc",1,0);
  return ISCT_ConditionsParameterSvc::interfaceID();
}

#endif
