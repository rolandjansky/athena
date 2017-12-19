/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsParameterSvc.cxx
 * implementation file for SCT conditions service giving numerical parameters
 * @author shaun.roe@cern.ch
**/
#include <limits>
#include <cmath>
#include <algorithm>
#include <cstdio>

#include "SCT_ConditionsParameterSvc.h"
#include "SCT_Chip.h"
//
#include "InDetIdentifier/SCT_ID.h"


namespace {//anonymous namespace introduces file-scoped functions
  //is the value 'Not-a-Number' ?
  template <typename T>
  bool is_nan(const T & val){
    return val!=val; //NaN is only value for which this is true
  }
  //response curve fit possibilities
  enum ResponseCurveFit{NO_CURVE=0, POLYNOMIAL, GRILLO, EXPONENTIAL, LINEAR, N_FUNCS}; //'Grillo' refers to Alex Grillo, Santa Cruz, who worked on SCT electronics development
  //response curve fit function
  float responseCurve(const float param0, const float param1, const float param2, const float vthreshold, const unsigned int rcFunc){
    float result(std::numeric_limits<float>::quiet_NaN());
    switch (rcFunc){
      case LINEAR:
        if (param1!=0.0){
          result=(vthreshold - param0)/param1;
        }
        break;
      case EXPONENTIAL:
        if ((vthreshold - param2)!=0){
          float err2=(param0/(vthreshold-param2)-1.0);
          if(err2 > 0.0){
            result=-param1*std::log(err2);
          }
        }
        break;
      case GRILLO:
        if(param2!=0){
          float err3 = param1*(1.0-((vthreshold-param0)/param2));
          if(err3!=0){
            result = (vthreshold-param0)/err3;
          }
        }
        break;
      case POLYNOMIAL:
        if(param2!=0){
          result = (-param1 + std::sqrt((param1*param1)-(4.0*param2*(param0-vthreshold)))) / 2.0*param2;
        }
        break;
      default:
       //nop
        break;
    }//switch
    if (is_nan(result)) return result;
    result *= result;
    return result;
  }//func responseCurve
  void parseResponseCurveArguments(float * p, const  std::string & arguments){
    //string is of form:
    //p0 1.284730e+03 p1 5.830000e+00 p2 -5.989900e+02
    float *p0=&p[0];
    float *p1=&p[1];
    float *p2=&p[2];
    std::sscanf(arguments.c_str(), "p0 %50e p1 %50e p2 %50e",p0,p1,p2);
  }
  //folder to retrieve for threshold parameters
  const std::string chipFolderName("/SCT/DAQ/Configuration/Chip"); //CoraCool folder in the DB
}//namespace
//c'tor
SCT_ConditionsParameterSvc::SCT_ConditionsParameterSvc( const std::string& name, ISvcLocator* pSvcLocator ):
  AthService(name, pSvcLocator),
  m_detStore("DetectorStore",name),
  m_cablingSvc("SCT_CablingSvc", name),
  m_pHelper{nullptr},
  m_filled(false){
  boost::array<float,SCT_ConditionsServices::N_PARAMETERS> init;
  for (unsigned int i(0);i!=SCT_ConditionsServices::N_PARAMETERS;++i){
    m_invalidParameters[i]=invalid(i);
    m_min[i]=std::numeric_limits<float>::max();
    m_max[i]=std::numeric_limits<float>::min();
    m_n[i]=0;
    m_sum[i]=0.0;
    m_sumsq[i]=0.0;
    init[i]=0.0;
  }
  
  //initialize boost array, doesnt provide constructor
   for (unsigned int i(0);i!=N_ELEMENTS;++i){
     m_values[i]=init;
   }
}
//
StatusCode
SCT_ConditionsParameterSvc::initialize(){
  // Retrieve detector store
  if (m_detStore.retrieve().isFailure())  return msg(MSG:: FATAL)<< "Detector service  not found !" << endmsg, StatusCode::FAILURE;
  // Retrieve cabling service
  if (m_cablingSvc.retrieve().isFailure())  return msg(MSG:: ERROR)<< "Can't get the cabling service." << endmsg, StatusCode::FAILURE;
  // Retrieve SCT ID helper
  if (m_detStore->retrieve(m_pHelper, "SCT_ID").isFailure()) return msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg, StatusCode::FAILURE;
  //
  if (m_detStore->regFcn(&SCT_ConditionsParameterSvc::fillData,this, m_thresholdData,chipFolderName).isFailure()) {
    return msg(MSG::FATAL) << "Failed to register callback" << endmsg, StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//
StatusCode
SCT_ConditionsParameterSvc::finalize(){
  return StatusCode::SUCCESS;
}
//
StatusCode 
SCT_ConditionsParameterSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( ISCT_ConditionsParameterSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

///Is the required parameter available?
bool 
SCT_ConditionsParameterSvc::available(const SCT_ConditionsServices::ParameterIndex iparam){ 
  return (iparam==SCT_ConditionsServices::AVG_THRESHOLD);
}

///Give the indicated value for a module identifier hash
float 
SCT_ConditionsParameterSvc::value(const IdentifierHash & idHash, const SCT_ConditionsServices::ParameterIndex iparam){
  return m_values[idHash][iparam];
}

///Measure of how many valid values went to calculate it. Should be 1 but if, say, 3 chip values were valid out of 6, it could be less (0.5 in this case)
float 
SCT_ConditionsParameterSvc::validity(const IdentifierHash & /*idHash*/, const SCT_ConditionsServices::ParameterIndex /*iparam*/){ 
  return 1.0;
}

///Is a given value within acceptable limits?
bool 
SCT_ConditionsParameterSvc::isValid(const float parameterValue, const SCT_ConditionsServices::ParameterIndex iparam){ 
  if (iparam==SCT_ConditionsServices::AVG_THRESHOLD and is_nan(parameterValue)) return false;
  return true;
}

///What is the default error value for this parameter?
float 
SCT_ConditionsParameterSvc::invalid(const unsigned int iparam) const{
  float result(0.0);
  if (iparam==SCT_ConditionsServices::AVG_THRESHOLD){
    result=std::numeric_limits<float>::quiet_NaN();
  }
  return result;
}

///Maximum value read in from the database
float 
SCT_ConditionsParameterSvc::max(const SCT_ConditionsServices::ParameterIndex iparam){ 
  return m_max[iparam];
}

///Minimum value read in from the database
float 
SCT_ConditionsParameterSvc::min(const SCT_ConditionsServices::ParameterIndex iparam){ 
  return m_min[iparam];
}

///Average value
float 
SCT_ConditionsParameterSvc::avg(const SCT_ConditionsServices::ParameterIndex iparam){
  return (m_n[iparam]!=0)?(m_sum[iparam]/m_n[iparam]):(std::numeric_limits<float>::quiet_NaN());
}

///Standard deviation
float 
SCT_ConditionsParameterSvc::sd(const SCT_ConditionsServices::ParameterIndex iparam){ 
  float mu=m_sum[iparam]/m_n[iparam];
  return std::sqrt((m_sumsq[iparam]/m_n[iparam]) - mu*mu);
}

///Number of values read in
unsigned int 
SCT_ConditionsParameterSvc::n(const SCT_ConditionsServices::ParameterIndex iparam){ 
  return m_n[iparam];
}

///Fill a user-provided vector with the values (hopefully won't be needed?)
void 
SCT_ConditionsParameterSvc::getValues(std::vector<float> & userVector, const SCT_ConditionsServices::ParameterIndex iparam){ 
  for (unsigned int i(0);i!=m_values.size();++i){
    userVector.push_back(m_values[i][iparam]); 
  }
//no return value
}

///Report whether the structure was filled
bool 
SCT_ConditionsParameterSvc::filled() const{ 
  return m_filled;  
}

///extended methods for data structure insertion
bool 
SCT_ConditionsParameterSvc::insert(const IdentifierHash & idHash, const SCT_ConditionsServices::ParameterIndex iparam, const float theValue){
  //theValue must be valid
  if (not isValid(theValue, iparam)) return false;
  boost::array<float,SCT_ConditionsServices::N_PARAMETERS> &paramArray=m_values[idHash];
  //initial value should be invalid, only insert if this is the case
  if (not isValid(paramArray[iparam], iparam)){
    paramArray[iparam]=theValue;
    return true;
  } else {
    return false; 
  }
}

///Callback for fill from database
StatusCode 
SCT_ConditionsParameterSvc::fillData(int& /* i */ , std::list<std::string>& /*keys*/){
  if (m_detStore->retrieve(m_thresholdData,chipFolderName).isFailure())  return msg(MSG:: ERROR)<< "Could not fill chip configuration data" << endmsg, StatusCode::FAILURE;
  for (unsigned int i(0);i!=SCT_ConditionsServices::N_PARAMETERS;++i){
    m_n[i]=0;
    m_sum[i]=0.0;
    m_sumsq[i]=0.0;
    m_min[i]=std::numeric_limits<float>::max();
    m_max[i]=std::numeric_limits<float>::min();
  }
  // Loop over elements (i.e groups of 6 chips) in DB folder 
  const unsigned int nChipsPerModule(12);
  const unsigned int nChipsPerSide(6);
  const float mVperDacBit(2.5);
  //initialize the values to be invalid
  for (unsigned int i(0);i!=m_values.size();++i) m_values[i]=m_invalidParameters;
  CondAttrListVec::const_iterator modItr(m_thresholdData->begin());
  CondAttrListVec::const_iterator modEnd(m_thresholdData->end());
  for (;modItr != modEnd; modItr += nChipsPerModule) {
    // Get SN and identifiers (the channel number is serial number+1)
    const unsigned int truncatedSerialNumber(modItr->first - 1);
    const IdentifierHash& moduleHash = m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber);
    if (not moduleHash.is_valid()) continue;
    // Loop over chips within module
   
    for (unsigned int side(0);side!=2;++side){
      IdentifierHash elementHash(int(moduleHash) + side);
      CondAttrListVec::const_iterator channelItr(modItr);
      CondAttrListVec::const_iterator channelEnd(modItr + nChipsPerSide);
      std::vector<SCT_Chip*> chipsInMod;
      chipsInMod.reserve(nChipsPerSide);
      float parameters[3]={0.0, 0.0, 0.0};
      float chipsum(0.0);
      for(; channelItr != channelEnd; ++channelItr){
        // Can get AttributeList from second (see http://lcgapp.cern.ch/doxygen/CORAL/CORAL_1_9_3/doxygen/html/classcoral_1_1_attribute_list.html)
        //short id      = channelItr->second[2].data<short>(); //chip 0-11
        float vthr  = mVperDacBit * channelItr->second[10].data<short>() ;  //threshold setting
        short rcFunctionIndex  = channelItr->second[15].data<short>(); //response curve function
        std::string rcArgumentString = channelItr->second[16].data<std::string>(); //response curve arguments
        parseResponseCurveArguments(parameters, rcArgumentString);
        //float target = channelItr->second[18].data<float>(); //trim target...use for debugging only
        float femtoCoulombThreshold=responseCurve(parameters[0], parameters[1], parameters[2], vthr, rcFunctionIndex);
        chipsum+=femtoCoulombThreshold;
      }
      float moduleAverage=chipsum/nChipsPerSide;
      if (insert(elementHash, SCT_ConditionsServices::AVG_THRESHOLD, moduleAverage)){
        m_n[SCT_ConditionsServices::AVG_THRESHOLD]++;
        m_sum[SCT_ConditionsServices::AVG_THRESHOLD]+=moduleAverage;
        m_sumsq[SCT_ConditionsServices::AVG_THRESHOLD]+=moduleAverage*moduleAverage;
        m_min[SCT_ConditionsServices::AVG_THRESHOLD]=std::min(m_min[SCT_ConditionsServices::AVG_THRESHOLD], moduleAverage);
        m_max[SCT_ConditionsServices::AVG_THRESHOLD]=std::max(m_max[SCT_ConditionsServices::AVG_THRESHOLD], moduleAverage);
      } else {
        msg(MSG::WARNING) << "Insertion failed for hash: "<<elementHash<<" and parameter: "<<SCT_ConditionsServices::parameterNames[SCT_ConditionsServices::AVG_THRESHOLD] << endmsg;
      }
    }//side loop
  }//module loop
  m_filled=true;
  return StatusCode::SUCCESS;
}

