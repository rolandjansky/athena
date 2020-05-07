/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerSelectorToolsDict.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDICT_H
#define RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDICT_H

#include <vector>

#include "RingerSelectorTools/RingerSelectorToolsDefs.h"

// These are only available if we are not in a standalone environment.
#ifndef RINGER_STANDALONE
#include "RingerSelectorTools/tools/RingerCommonSelector.h"
#include "RingerSelectorTools/AsgElectronRingerSelector.h"
#endif

#include "RingerSelectorTools/tools/RedirectMsgStream.h"

#include "RingerSelectorTools/tools/VariableDependency.h"
#include "RingerSelectorTools/tools/RingerIOVarDepObj.h"
#include "RingerSelectorTools/tools/IOHelperFcns.h"

#include "RingerSelectorTools/procedures/IRingerProcedure.h"
#include "RingerSelectorTools/procedures/Types.h"

#include "RingerSelectorTools/procedures/RingerProcedureWrapper.h"

#include "RingerSelectorTools/procedures/Normalizations.h"
#include "RingerSelectorTools/procedures/NeuralNetwork.h"
#include "RingerSelectorTools/procedures/Thresholds.h"

/*
#define RINGER_DECLARE_VARDEP_T(name, inst_name) \
  name< Ringer::EtaDependent   , Ringer::EtDependent   > inst_name ## _etaDep_etDep; \
  name< Ringer::EtaDependent   , Ringer::EtIndependent > inst_name ## _etaDep_etIndep; \
  name< Ringer::EtaIndependent , Ringer::EtDependent   > inst_name ## _etaIndep_etDep; \
  name< Ringer::EtaIndependent , Ringer::EtIndependent > inst_name ## _etaIndep_etIndep;
*/

#define _RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, segType, inst_name) \
  Ringer::RingerProcedureWrapper< type, Ringer::EtaDependent,     Ringer::EtDependent,    segType > inst_name ## _etaDep_etDep; \
  Ringer::RingerProcedureWrapper< type, Ringer::EtaIndependent,   Ringer::EtDependent,    segType > inst_name ## _etaIndep_etDep; \
  Ringer::RingerProcedureWrapper< type, Ringer::EtaDependent,     Ringer::EtIndependent,  segType > inst_name ## _etaDep_etIndep; \
  Ringer::RingerProcedureWrapper< type, Ringer::EtaIndependent,   Ringer::EtIndependent,  segType > inst_name ## _etaIndep_etIndep; 

#define RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(type, inst_name) \
  _RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, Ringer::NoSegmentation, inst_name ## _allCalo )

#define RINGER_DECLARE_ALLSEGTYPES_WRAPPER_T(type, inst_name) \
  _RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, Ringer::NoSegmentation, inst_name ## _allCalo ) \
  _RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, Ringer::TrackCalSegmentation, inst_name ## _trackCalSeg ) \
  /*_RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, Ringer::TrackCalPatTypeSegmentation, inst_name ## _trackCalSeg )*/ \
  _RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, Ringer::TrackCalJointSections, inst_name ## _jSec ) \
  _RINGER_DECLARE_WRAPPER_T_WITH_SEG_TYPE_(type, Ringer::TrackCalJointLayers, inst_name ## _jLay )

template void Ringer::IOHelperFcns::readVar<Ringer::EtaDependency,unsigned>(TDirectory*, const char*, Ringer::EtaDependency&);
template void Ringer::IOHelperFcns::readVar<Ringer::EtDependency,unsigned>(TDirectory*, const char*, Ringer::EtDependency&);
template void Ringer::IOHelperFcns::readVar<Ringer::SegmentationType,unsigned>(TDirectory*, const char*, Ringer::SegmentationType&);
template void Ringer::IOHelperFcns::readVar<Ringer::PreProcessing::Type::PreProcessorTypes,unsigned>(TDirectory*, const char*, Ringer::PreProcessing::Type::PreProcessorTypes&);
template void Ringer::IOHelperFcns::readVar<Ringer::Discrimination::Type::DiscriminatorTypes,unsigned>(TDirectory*, const char*, Ringer::Discrimination::Type::DiscriminatorTypes&);
template void Ringer::IOHelperFcns::readVar<Ringer::Discrimination::Type::ThresholdTypes,unsigned>(TDirectory*, const char*, Ringer::Discrimination::Type::ThresholdTypes&);
template void Ringer::IOHelperFcns::readVar(TDirectory*, const char*, unsigned&);
template void Ringer::IOHelperFcns::readVar(TDirectory*, const char*, float&);
//-------------- For now, we also include the write variable methods
template void Ringer::IOHelperFcns::writeVar<const Ringer::EtaDependency,const unsigned>(TDirectory*, const char*, const Ringer::EtaDependency&);
template void Ringer::IOHelperFcns::writeVar<const Ringer::EtDependency,const unsigned>(TDirectory*, const char*, const Ringer::EtDependency&);
template void Ringer::IOHelperFcns::writeVar<const Ringer::SegmentationType,const unsigned>(TDirectory*, const char*, const Ringer::SegmentationType&);
template void Ringer::IOHelperFcns::writeVar<const Ringer::PreProcessing::Type::PreProcessorTypes,const unsigned>(TDirectory*, const char*, const Ringer::PreProcessing::Type::PreProcessorTypes&);
template void Ringer::IOHelperFcns::writeVar<const Ringer::Discrimination::Type::DiscriminatorTypes,const unsigned>(TDirectory*, const char*, const Ringer::Discrimination::Type::DiscriminatorTypes&);
template void Ringer::IOHelperFcns::writeVar<const Ringer::Discrimination::Type::ThresholdTypes,const unsigned>(TDirectory*, const char*, const Ringer::Discrimination::Type::ThresholdTypes&);
template void Ringer::IOHelperFcns::writeVar(TDirectory*, const char*, unsigned&);
template void Ringer::IOHelperFcns::writeVar(TDirectory*, const char*, float&);
//---------------------------------------------------------------------

namespace RingerSelectorToolsDict {

struct dict {

  //std::vector<long long>             m_vecLongLong;
  //std::vector<unsigned long long>    m_vecULongLong;
  //std::vector<unsigned long>    m_vecULong;
  //std::vector<Ringer::EtaDependency>    m_vecEtaDep;
  std::vector<Ringer::PreProcessing::Norm::Norm1VarDep*>    m_vecNorm1;
  std::vector<std::vector<Ringer::PreProcessing::Norm::Norm1VarDep*> >    m_vecVecNorm1;

  std::vector<Ringer::Discrimination::NNFeedForwardVarDep*>    m_vecNN;
  std::vector<std::vector<Ringer::Discrimination::NNFeedForwardVarDep*> >  m_vecVecNN;

  //Ringer::RingerProcedureWrapper<Ringer::PreProcessing::Norm1VarDep, 
  //                               Ringer::EtaIndependent,
  //                               Ringer::EtIndependent,
  //                               Ringer::NoSegmentation> discrDummy;

  /// Wrappers
  /// @{
  // Add all interface wrappers:
  RINGER_DECLARE_ALLSEGTYPES_WRAPPER_T(Ringer::PreProcessing::IPreProcessorVarDep, iwrap_ipp)
  RINGER_DECLARE_ALLSEGTYPES_WRAPPER_T(Ringer::Discrimination::IDiscriminatorVarDep, iwrap_idiscr)
  RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(Ringer::Discrimination::IThresholdVarDep, iwrap_ithres)
  // Now we add some the specialized procedure wrappers:
  // Norm1:
  RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(Ringer::PreProcessing::Norm::Norm1VarDep, norm1)
  // NNFeedForward:
  RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(Ringer::Discrimination::NNFeedForwardVarDep, nnff)
  // Threshold
  RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(Ringer::Discrimination::UniqueThresholdVarDep, uniqueThres)
  RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(Ringer::Discrimination::LinearPileupCorrectionThresholdVarDep, linearThres)
  /// @}

};

} // private namespace


#ifdef G__DICTIONARY   // ROOT6 dictionary compilation
// MN:  pull in the Ringer enums and variables for ROOT6 on dictionary library loading
#include "TInterpreter.h"
namespace Ringer {
   struct DeclareHeader {
     DeclareHeader() {
        gInterpreter->Declare("#include \"RingerSelectorTools/RingerSelectorToolsDefs.h\"");
        gInterpreter->Declare("#include \"RingerSelectorTools/procedures/Types.h\"");
        gInterpreter->Declare("#include \"RingerSelectorTools/tools/IOHelperFcns.h\"");
     }
   } declareHeader;
}
#endif

#endif // RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDICT_H
