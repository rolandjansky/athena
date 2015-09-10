/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Normalizations.cxx 667905 2015-05-18 19:07:55Z wsfreund $
#include "RingerSelectorTools/procedures/Normalizations.icc"

#include <algorithm>

/** 
 * @brief Namespace dedicated for Ringer utilities 
 **/
namespace Ringer
{

/** 
* @brief Namespace dedicated for Ringer pre-processing utilities 
**/
namespace PreProcessing
{

/** 
 * @brief Namespace dedicated for Ringer normalization utilities 
 **/
namespace Norm {

/** 
 * @brief Helper functions:
 **/
namespace {
// =============================================================================
inline
void applySequentialNorm( std::vector<float> &inputSpace, 
    const float norm, 
    const float stopEnergy )
{
  bool fixed = false;
  float curNorm = norm;
  for ( std::vector<float>::iterator itr = inputSpace.begin();
        itr != inputSpace.end();
        ++itr){
    if ( curNorm < stopEnergy ){ 
      fixed = true;
    }
    if ( fixed ) { // Do not increase noise description:
      curNorm = norm;
    } else {
      *itr /= curNorm;
      curNorm -= *itr; 
    }
  }

  if (!norm) return;

  float invNorm = 1/norm;
  for ( std::vector<float>::iterator itr = inputSpace.begin();
        itr != inputSpace.end();
        ++itr){
    *itr *= invNorm;
  }
}

// =============================================================================
struct minus {
  float operator()(float in) {
    return -in;
  }
};


// =============================================================================
struct invert {
  float operator()(float in) {
    if(in){
      return 1/in;
    } else {
      throw std::runtime_error("Attempt to invert zero");
    }
  }
};

} // Private namespace

// =============================================================================
void ConstantValue::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  msg() << lvl << "Constant normalization: " << 1/m_constantInv << endreq;
}

// =============================================================================
void ConstantValue::read(ConstantValue *newObj, 
    TDirectory *configDir, 
    unsigned /*version*/ )
{
  IOHelperFcns::readVar(configDir, "constantInv", newObj->m_constantInv); 
}

// =============================================================================
void ConstantValue::write(TDirectory *configDir, const char *) const
{
  IOHelperFcns::writeVar(configDir, "constantInv", m_constantInv );
}

// =============================================================================
void Sequential::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  msg() << lvl << "stopEnergy: " << m_stopEnergy << endreq;
  msg() << lvl << "energyThres: " << m_energyThres << endreq;
}

// =============================================================================
void Sequential::read(Sequential *newObj, 
    TDirectory *configDir, 
    unsigned /*version*/ )
{
  IOHelperFcns::readVar(configDir, "stopEnergy", newObj->m_stopEnergy); 
  IOHelperFcns::readVar(configDir, "energyThres", newObj->m_energyThres); 
}

// =============================================================================
void Sequential::write(TDirectory *configDir, const char *) const
{
  IOHelperFcns::writeVar(configDir, "stopEnergy", m_stopEnergy );
  IOHelperFcns::writeVar(configDir, "energyThres", m_energyThres );
}

// =============================================================================
void Sequential::execute(std::vector<float> &inputSpace) const {
#ifndef NDEBUG
  ATH_MSG_DEBUG("Applying sequential. Input space is: " << inputSpace);
#endif
  float norm1 = getNorm(inputSpace,1);
  if ( norm1 < m_stopEnergy ) {
    float max = getMax(inputSpace); 
    // If normalization value is very low, do not normalize:
    if ( norm1 < m_energyThres || max < m_energyThres ) return;
    // Otherwise normalize with the greater norm value:
    if ( norm1 < max ){
      applyNorm(inputSpace,max);
    } else {
      applyNorm(inputSpace,norm1);
    }
  } else {
    // Otherwise use sequential normalization:
    applySequentialNorm( inputSpace, norm1, m_stopEnergy );
  }
#ifndef NDEBUG
  ATH_MSG_DEBUG("Pattern space is: " << inputSpace);
#endif
}

// =============================================================================
void Spherization::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  msg() << lvl << "deslocation: " << m_deslocation << endreq;
  msg() << lvl << "normInv: " << m_normInv << endreq;
}

// =============================================================================
void Spherization::read(Spherization *newObj, 
    TDirectory *configDir, 
    unsigned /*version*/ )
{
  IOHelperFcns::readVar(configDir, "deslocation", newObj->m_deslocation ); 
  IOHelperFcns::readVar(configDir, "normInv",     newObj->m_normInv     ); 
  newObj->m_dim = newObj->m_deslocation.size();
}

// =============================================================================
void Spherization::write(TDirectory *configDir, const char *) const
{
  IOHelperFcns::writeVar(configDir, "deslocation", m_deslocation );
  IOHelperFcns::writeVar(configDir, "normInv",     m_normInv     );
}

// =============================================================================
Spherization::Spherization(
    const std::vector<float> &dataMean,
    const std::vector<float> &dataStd)
  : m_dim(dataMean.size())
{
  if ( dataMean.empty() || dataStd.empty() ){
    throw std::runtime_error(std::string("Either DataMean or DataStd "
          "vectors have no dimension.") );
  }
  if ( dataMean.size() != dataStd.size() ){
    throw std::runtime_error(std::string("DataMean and DataStd vectors do "
          "no have same dimension."));
  }
  m_deslocation.resize(m_dim);
  m_normInv.resize(m_dim);
  try {
    std::transform( dataMean.begin(), 
        dataMean.end(),
        m_deslocation.begin(),
        minus()  );

    std::transform( dataStd.begin(), 
        dataStd.end(),
        m_normInv.begin(),
        invert() );
  } catch ( const std::runtime_error & ){
    throw std::runtime_error(std::string("Attempted to create Spherization "
          "with null standard deviation"));
  }
}


// =============================================================================
void MinMax::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  msg() << lvl << "deslocation: " << m_deslocation << endreq;
  msg() << lvl << "normInv: " << m_normInv << endreq;
}

// =============================================================================
void MinMax::read(MinMax *newObj, 
    TDirectory *configDir, 
    unsigned /*version*/ )
{
  IOHelperFcns::readVar(configDir, "deslocation",  newObj->m_deslocation     ); 
  IOHelperFcns::readVar(configDir, "normInv",      newObj->m_normInv         );
  newObj->m_dim = newObj->m_deslocation.size();
}

// =============================================================================
void MinMax::write(TDirectory *configDir, const char* ) const
{
  IOHelperFcns::writeVar(configDir, "deslocation", m_deslocation );
  IOHelperFcns::writeVar(configDir, "normInv",     m_normInv     );
}

// =============================================================================
MinMax::MinMax(
    const std::vector<float> min,
    const std::vector<float> max)
  : m_dim(min.size())
{
  if ( min.empty() || max.empty() ){
    throw std::runtime_error(std::string("Either Min or max vectors have no "
          "dimension."));
  }
  if ( min.size() != max.size() ){
    throw std::runtime_error(std::string("Min and max vectors do no have same "
          "dimension."));
  }

  m_deslocation.reserve(m_dim);
  m_normInv.reserve(m_dim);

  for (size_t idx = 0; idx < m_dim; ++idx ) {
    m_deslocation.push_back(-((max[idx]+min[idx])/2));
    m_normInv.push_back(2/(max[idx]-min[idx]));
    if (!m_normInv[idx]) {
      throw std::runtime_error(
          std::string("MinMax should have different minimum and maximum "
            "values."));
    }
  }
}

} // namespace Norm
} // namespace PreProcessing
} // namespace Ringer

