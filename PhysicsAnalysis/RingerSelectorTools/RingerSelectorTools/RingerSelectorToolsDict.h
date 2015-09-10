/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerSelectorToolsDict.h 693573 2015-09-07 19:15:49Z wsfreund $
#ifndef RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDICT_H
#define RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDICT_H

#include <vector>

#include "RingerSelectorTools/RingerSelectorToolsDefs.h"

#include "RingerSelectorTools/tools/cxx/RingerUseNewCppFeatures.h"

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

namespace RingerSelectorToolsDict {

struct dict {

  //std::vector<long long>             m_vecLongLong;
  //std::vector<unsigned long long>    m_vecULongLong;
  //std::vector<unsigned long>    m_vecULong;
  //std::vector<Ringer::EtaDependency>    m_vecEtaDep;


  /// Wrappers
  /// @{
  //Ringer::RingerProcedureWrapper< Ringer::PreProcessing::IPreProcessing,Ringer::EtaDependent,Ringer::EtDependent,Ringer::TrackCalJointLayers> namedummy;
  //RINGER_DECLARE_ALLSEGTYPES_WRAPPER_T(Ringer::PreProcessing::IPreProcessorVarDep, iwrap_ipp)
  //RINGER_DECLARE_ALLSEGTYPES_WRAPPER_T(Ringer::PreProcessing::IPreProcessor, iwrap_ipp)
  //RINGER_DECLARE_ALLSEGTYPES_WRAPPER_T(Ringer::Discrimination::IDiscriminatorVarDep, iwrap_idiscr)
  //RINGER_DECLARE_ONLYALLCALO_WRAPPER_T(Ringer::Discrimination::IThresholdVarDep, iwrap_ithres)
  /// @}

};

} // private namespace


#ifdef G__DICTIONARY   // ROOT6 dictionary compilation
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
// MN:  pull in the Ringer enums and variables for ROOT6 on dictionary library loading
#include "TInterpreter.h"
namespace Ringer {
   struct DeclareHeader {
     DeclareHeader() {
        gInterpreter->Declare("#include \"RingerSelectorTools/RingerSelectorToolsDefs.h\"");
        gInterpreter->Declare("#include \"RingerSelectorTools/procedures/Types.h\"");
        //gInterpreter->Declare("#include \"RingerSelectorTools/tools/IOHelperFcns.h\"");
     }
   } declareHeader;
}
#endif
#endif

#endif // RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDICT_H
