/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConditionsParameterCondAlg.h"

#include "SCT_Chip.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"

namespace {//anonymous namespace introduces file-scoped functions
  //is the value 'Not-a-Number' ?
  template <typename T>
  bool is_nan(const T& val) {
    return val!=val; //NaN is only value for which this is true
  }
  //response curve fit possibilities
  enum ResponseCurveFit{NO_CURVE=0, POLYNOMIAL, GRILLO, EXPONENTIAL, LINEAR, N_FUNCS}; //'Grillo' refers to Alex Grillo, Santa Cruz, who worked on SCT electronics development
  //response curve fit function
  float responseCurve(const float param0, const float param1, const float param2, const float vthreshold, const unsigned int rcFunc) {
    float result{std::numeric_limits<float>::quiet_NaN()};
    switch (rcFunc) {
    case LINEAR:
      if (param1!=0.0) {
        result = (vthreshold - param0)/param1;
      }
      break;
    case EXPONENTIAL:
      if ((vthreshold - param2)!=0) {
        float err2{param0/(vthreshold-param2)-1.0f};
        if (err2 > 0.0) {
          result = -param1*std::log(err2);
        }
      }
      break;
    case GRILLO:
      if (param2!=0) {
        float err3{param1*(1.0f-((vthreshold-param0)/param2))};
        if (err3!=0) {
          result = (vthreshold-param0)/err3;
        }
      }
      break;
    case POLYNOMIAL:
      if (param2!=0) {
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

  void parseResponseCurveArguments(float* p, const  std::string& arguments) {
    //string is of form:
    //p0 1.284730e+03 p1 5.830000e+00 p2 -5.989900e+02
    float *p0{&p[0]};
    float *p1{&p[1]};
    float *p2{&p[2]};
    std::sscanf(arguments.c_str(), "p0 %50e p1 %50e p2 %50e", p0, p1, p2);
  }
  //folder to retrieve for threshold parameters
  const std::string chipFolderName{"/SCT/DAQ/Configuration/Chip"}; //CoraCool folder in the DB
}//namespace

SCT_ConditionsParameterCondAlg::SCT_ConditionsParameterCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/SCT/DAQ/Configuration/Chip"}
  , m_writeKey{"SCT_CondParameterData"}
  , m_cablingSvc{"SCT_CablingSvc", name}
  , m_condSvc{"CondSvc", name}
{
  declareProperty("ReadKey", m_readKey, "Key of input (raw) chip conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) average threshold conditions folder");
}

SCT_ConditionsParameterCondAlg::~SCT_ConditionsParameterCondAlg() {
}

StatusCode SCT_ConditionsParameterCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  
  // Cabling service
  ATH_CHECK(m_cablingSvc.retrieve());
  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // Read Cond Handle
  ATH_CHECK(m_readKey.initialize());
  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_ConditionsParameterCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_CondParameterData> writeHandle{m_writeKey};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListVec> readHandle{m_readKey};
  const CondAttrListVec* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListVec " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);
  
  // Construct the output Cond Object and fill it in
  SCT_CondParameterData* writeCdo{new SCT_CondParameterData()};

  // Loop over elements (i.e groups of 6 chips) in DB folder 
  const unsigned int nChipsPerModule{12};
  const unsigned int nChipsPerSide{6};
  const float mVperDacBit{2.5};
  CondAttrListVec::const_iterator modItr{readCdo->begin()};
  CondAttrListVec::const_iterator modEnd{readCdo->end()};
  for (; modItr != modEnd; modItr += nChipsPerModule) {
    // Get SN and identifiers (the channel number is serial number+1)
    const unsigned int truncatedSerialNumber{modItr->first - 1};
    const IdentifierHash& moduleHash{m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber)};
    if (not moduleHash.is_valid()) continue;
    // Loop over chips within module
   
    for (unsigned int side{0}; side!=2; ++side) {
      IdentifierHash elementHash{moduleHash + side};
      CondAttrListVec::const_iterator channelItr{modItr};
      CondAttrListVec::const_iterator channelEnd{modItr + nChipsPerSide};
      std::vector<SCT_Chip*> chipsInMod;
      chipsInMod.reserve(nChipsPerSide);
      float parameters[3]{0.0, 0.0, 0.0};
      float chipsum{0.0};
      for (; channelItr != channelEnd; ++channelItr) {
        // Can get AttributeList from second (see http://lcgapp.cern.ch/doxygen/CORAL/CORAL_1_9_3/doxygen/html/classcoral_1_1_attribute_list.html)
        //short id      = channelItr->second[2].data<short>(); //chip 0-11
        float vthr{mVperDacBit * channelItr->second[10].data<short>()};  //threshold setting
        short rcFunctionIndex{channelItr->second[15].data<short>()}; //response curve function
        std::string rcArgumentString{channelItr->second[16].data<std::string>()}; //response curve arguments
        parseResponseCurveArguments(parameters, rcArgumentString);
        //float target = channelItr->second[18].data<float>(); //trim target...use for debugging only
        float femtoCoulombThreshold{responseCurve(parameters[0], parameters[1], parameters[2], vthr, rcFunctionIndex)};
        chipsum+=femtoCoulombThreshold;
      }
      float moduleAverage{chipsum/nChipsPerSide};
      writeCdo->setValue(elementHash, SCT_CondParameterData::AVG_THRESHOLD, moduleAverage);
    }//side loop
  }//module loop

  // Record the output cond object
  if (writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_CondParameterData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    delete writeCdo;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_ConditionsParameterCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
