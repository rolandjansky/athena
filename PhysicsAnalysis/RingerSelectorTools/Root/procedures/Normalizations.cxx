/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Normalizations.cxx 770805 2016-08-30 14:03:33Z ssnyder $
#include "RingerSelectorTools/procedures/Normalizations.icc"
#include "AsgMessaging/MsgStream.h"

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
template <typename T> int sign(T val) {
  return (T(0) < val) - (val < T(0));
}

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
  msg() << lvl << "Constant normalization with params * constant: " << 1/m_constantInv << endmsg;
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
  msg() << lvl << "Sequential with params: * stopEnergy: " << m_stopEnergy << endmsg
        << lvl << "                        * energyThres: " << m_energyThres << endmsg;
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
  msg() << lvl << "Spherization with params: * deslocation: " << m_deslocation << endmsg
        << lvl << "                          * normInv: " << m_normInv << endmsg;
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
  msg() << lvl << "MinMax with params * deslocation: " << m_deslocation << endmsg
        << lvl << "                   * normInv: " << m_normInv << endmsg;
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
float ExtraPatternsNorm::normEta(float eta) const
{
  // FIXME: This normalization is not optimal, however first implementation used it
  return ( std::abs( eta ) - m_etaMin ) * sign(eta) / m_etaMax;
}

// =============================================================================
float ExtraPatternsNorm::normEt(float et) const
{
  // TODO Cache values
  float b = ( m_etMax - m_etMin ) * .5;
  return ( et - m_etMin ) / b - 1;
}

// =============================================================================
float ExtraPatternsNorm::normPileupEstimation(float pileupEstimation) const
{
  // FIXME: We probably shouldn't be clipping pileup-estimator
  return ( pileupEstimation > m_pileupEstimationMax)? 1 : pileupEstimation / m_pileupEstimationMax;
}

// =============================================================================
void ExtraPatternsNorm::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  //msg() << lvl << " ExtraPatternsNorm * eta_norm = abs( eta - " << m_etaMin << " ) / ( " << m_etaMax - m_etaMin << " ) " << endmsg
  msg() << lvl << " ExtraPatternsNorm * eta_norm = ( abs( eta ) - " << m_etaMin << " ) * sign( eta ) / ( " << m_etaMax << " ) " << endmsg
        << lvl << "                   * et_norm = ( et - " << m_etMin << " ) / ( " << ( m_etMax - m_etMin ) * .5 << " ) - 1 " << endmsg
        << lvl << "                   * pileup_estimation_norm = ( pile_esitmation ) / " << m_pileupEstimationMax << endmsg;
}

// =============================================================================
void ExtraPatternsNorm::read(ExtraPatternsNorm *newObj,
    TDirectory *configDir,
    unsigned /*version*/ ) // TODO If normalization changes (and it should, keep track of the version parameter)
{
  IOHelperFcns::readVar(configDir, "__ExtraPatternsNorm__etMin",               newObj->m_etMin               );
  IOHelperFcns::readVar(configDir, "__ExtraPatternsNorm__etMax",               newObj->m_etMax               );
  IOHelperFcns::readVar(configDir, "__ExtraPatternsNorm__etaMin",              newObj->m_etaMin              );
  IOHelperFcns::readVar(configDir, "__ExtraPatternsNorm__etaMax",              newObj->m_etaMax              );
  IOHelperFcns::readVar(configDir, "__ExtraPatternsNorm__pileupEstimationMax", newObj->m_pileupEstimationMax );
}

// =============================================================================
void ExtraPatternsNorm::execute(std::vector<float> &/*inputSpace*/) const {;}

// =============================================================================
void ExtraPatternsNorm::write(TDirectory *baseDir, const char* idxStr ) const
{
	typedef RingerProcedureType<ExtraPatternsNorm>::procEnum_t procEnum_t;
  // NOTE: we make sure to create extra patterns norm directory if base dir is
  // not one (this will happen when user calls ExtraPatternsNorm instead of
  // ExtraPatternsNormVarDep) to always have a ExtraPatternsNorm dir in the saved file.
  TDirectory* configDir = baseDir;
  if ( !TString(baseDir->GetName()).BeginsWith( "ExtraPatternsNorm") ){
    configDir = IOHelperFcns::makeDir( baseDir, ("ExtraPatternsNorm" + std::string(idxStr) ).c_str() );
  }
  // Write object version:
  unsigned v = IOHelperFcns::version();
  IOHelperFcns::writeVar( configDir, "writtenOnPkgVersion", v );
  IOHelperFcns::writeVar< const procEnum_t, const unsigned int>( configDir, "procType", Ringer::PreProcessing::Type::PreProcessorTypes::ExtraPatternsNorm );
  IOHelperFcns::writeVar(configDir, "__ExtraPatternsNorm__etMin",               m_etMin               ) ;
  IOHelperFcns::writeVar(configDir, "__ExtraPatternsNorm__etMax",               m_etMax               ) ;
  IOHelperFcns::writeVar(configDir, "__ExtraPatternsNorm__etaMin",              m_etaMin              ) ;
  IOHelperFcns::writeVar(configDir, "__ExtraPatternsNorm__etaMax",              m_etaMax              ) ;
  IOHelperFcns::writeVar(configDir, "__ExtraPatternsNorm__pileupEstimationMax", m_pileupEstimationMax ) ;
  // Write EtaDependency enumeration as unsigned int:
  IOHelperFcns::writeVar< const EtaDependency, const unsigned int>( configDir,
      "etaDependency",
      EtaDependency::EtaIndependent );
  // Write EtDependency enumeration as unsigned int:
  IOHelperFcns::writeVar< const EtDependency, const unsigned int>( configDir,
      "etDependency",
      EtDependency::EtIndependent );
  float etMin = 0, etMax = 50000.;
  IOHelperFcns::writeVar(configDir, "etaMin",        m_etaMin        );
  IOHelperFcns::writeVar(configDir, "etaMax",        m_etaMax        );
  IOHelperFcns::writeVar(configDir, "etMin",          etMin          );
  IOHelperFcns::writeVar(configDir, "etMax",          etMax          );
}

} // namespace Norm
} // namespace PreProcessing
} // namespace Ringer

