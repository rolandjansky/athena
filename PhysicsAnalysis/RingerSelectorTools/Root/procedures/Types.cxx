/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Types.cxx 667905 2015-05-18 19:07:55Z wsfreund $
#include "RingerSelectorTools/procedures/Types.h"

#include "RingerSelectorTools/procedures/Normalizations.h"
#include "RingerSelectorTools/procedures/NeuralNetwork.h"
#include "RingerSelectorTools/procedures/Thresholds.h"

#include "RingerSelectorTools/tools/Exceptions.h"

namespace Ringer {

// =============================================================================
template<>
preProcEnum_t getType(const char* cStr){
  using namespace PreProcessing;
  std::string strType(cStr);
  if        ( strType == IPreProcessorVarDep::procType() ){
    return preProcEnum_t::IPreProcessorVarDep;
  } else if ( strType == Norm::Norm1::procType() ){
    return preProcEnum_t::Norm1;
  } else if ( strType == Norm::Norm2::procType() ){
    return preProcEnum_t::Norm2;
  } else if ( strType == Norm::Sqrt::procType() ){
    return preProcEnum_t::Sqrt;
  } else if ( strType == Norm::ConstantValue::procType() ){
    return preProcEnum_t::ConstantValue;
  } else if ( strType == Norm::Sequential::procType() ){
    return preProcEnum_t::Sequential;
  } else if ( strType == Norm::Spherization::procType() ){
    return preProcEnum_t::Spherization;
  } else if ( strType == Norm::MinMax::procType() ){
    return preProcEnum_t::MinMax;
  } else if ( strType == IPreProcessor::procType() ){
    return preProcEnum_t::IPreProcessor;
  } else {
    throw Exceptions::no_such_type("PreProcessorType",strType,"Types");
  }
}

// =============================================================================
template<>
discrEnum_t getType(const char* cStr){
  using namespace Discrimination;
  std::string strType(cStr);
  if        ( strType == IDiscriminatorVarDep::procType() ){
    return discrEnum_t::IDiscriminatorVarDep;
  } else if ( strType == NNFeedForward::procType() ){
    return discrEnum_t::NNFeedForward;
  } else if ( strType == IDiscriminator::procType() ){
    return discrEnum_t::IDiscriminator;
  } else {
    throw Exceptions::no_such_type("DiscriminatorType",strType,"Types");
  }
}

// =============================================================================
template<>
thresEnum_t getType(const char* cStr){
  using namespace Discrimination;
  std::string strType(cStr);
  if        ( strType == IThresholdVarDep::procType() ){
    return thresEnum_t::IThresholdVarDep;
  } else if ( strType == UniqueThreshold::procType() ){
    return thresEnum_t::UniqueThreshold;
  } else if ( strType == IDiscriminator::procType() ){
    return thresEnum_t::IThreshold;
  } else {
    throw Exceptions::no_such_type("ThresholdType",strType,"Types");
  }
}
// =============================================================================
const char* toStr(preProcEnum_t e){
  using namespace PreProcessing;
  switch (e){
    case preProcEnum_t::IPreProcessorVarDep:
      return IPreProcessorVarDep::procType();
      break;
    case preProcEnum_t::Norm1:
      return Norm::Norm1::procType();
      break;
    case preProcEnum_t::Norm2:
      return Norm::Norm2::procType();
      break;
    case preProcEnum_t::Sqrt:
      return Norm::Sqrt::procType();
      break;
    case preProcEnum_t::ConstantValue:
      return Norm::ConstantValue::procType();
      break;
    case preProcEnum_t::Sequential:
      return Norm::Sequential::procType();
      break;
    case preProcEnum_t::Spherization:
      return Norm::Spherization::procType();
      break;
    case preProcEnum_t::MinMax:
      return Norm::MinMax::procType();
      break;
    case preProcEnum_t::IPreProcessor:
      return IPreProcessor::procType();
      break;
    default:
      throw Exceptions::no_such_type(e,"Types");
  }
}

// =============================================================================
const char* toStr(discrEnum_t e){
  using namespace Discrimination;
  switch (e){
    case discrEnum_t::IDiscriminatorVarDep:
      return IDiscriminatorVarDep::procType();
      break;
    case discrEnum_t::NNFeedForward:
      return NNFeedForward::procType();
      break;
    case discrEnum_t::IDiscriminator:
      return IDiscriminator::procType();
      break;
    default:
      throw Exceptions::no_such_type(e,"Types");
  }
}

// =============================================================================
const char* toStr(thresEnum_t t){
  using namespace Discrimination;
  switch (t){
    case thresEnum_t::IThresholdVarDep:
      return IThresholdVarDep::procType();
      break;
    case thresEnum_t::UniqueThreshold:
      return UniqueThreshold::procType();
      break;
    case thresEnum_t::IThreshold:
      return IThreshold::procType();
      break;
    default:
      throw Exceptions::no_such_type(t,"Types");
  }
}

} // namespace Ringer
