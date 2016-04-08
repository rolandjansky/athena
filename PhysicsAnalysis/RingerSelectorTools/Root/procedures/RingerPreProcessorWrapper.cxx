/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerPreProcessorWrapper.cxx 670599 2015-05-28 14:15:35Z wsfreund $
#include "RingerSelectorTools/procedures/RingerPreProcessorWrapper.h"

namespace Ringer {
#if !(RINGER_USE_NEW_CPP_FEATURES)
const char* IPreProcWrapper::wrapName = "RingerPreProcessorWrapper"; 
#else
constexpr const char* IPreProcWrapper::wrapName; 
#endif

// =============================================================================
void IPreProcWrapper::writeCol(const IPreProcWrapperCollection &ppWrapperCol, 
    TDirectory *configDir)
{
  bool hasPP = !( ppWrapperCol.empty() );
  IOHelperFcns::writeVar( configDir, "hasPP", hasPP );
  // Write pre-processing wrappers:
  if (hasPP){
    // Write pre-processing 
    unsigned ppWrapperSize = ppWrapperCol.size();
    IOHelperFcns::writeVar( configDir, "ppWrapperSize", ppWrapperSize );
    for (size_t ppIdx = 0;  ppIdx < ppWrapperSize; ++ppIdx ) {
      ppWrapperCol[ppIdx]->write(configDir,
          IOHelperFcns::makeIdxStr(ppIdx).c_str());
    }
  }
}

// =============================================================================
void IPreProcWrapper::read(IPreProcWrapperCollection &ppWrapperCol, 
    TDirectory *configDir, 
    unsigned version)
{

  // Get its size:
  unsigned ppWrapperSize(0);
  IOHelperFcns::readVar( configDir, "ppWrapperSize", ppWrapperSize );

  // Cycle through pp collection and fill it:
  for ( unsigned counter = 0; counter < ppWrapperSize; ++counter) {
    std::string folderName = IPreProcWrapper::wrapName +
      IOHelperFcns::makeIdxStr(counter);

    //ATH_MSG_DEBUG("Reading PreProcessing Wrapper on folder \""
    //    << folderName << "\"" );
    TDirectory *ppDir = configDir->GetDirectory(folderName.c_str());
    if ( ppDir == nullptr || ppDir == configDir )
    {
      throw std::runtime_error(std::string("Could not find directory \"")
            + folderName + "\" containing PreProcessing Wrapper" 
            "information.");
    }
    // Get basic preprocessing wrapper information:
    preProcEnum_t ppType;
    EtaDependency fileEtaDep;
    EtDependency fileEtDep;
    SegmentationType fileSegType;
    IOHelperFcns::readVar<preProcEnum_t, unsigned int>( ppDir, 
        "ppType",
        ppType);
    IOHelperFcns::readVar<SegmentationType, unsigned int>( ppDir,
        "segType",
        fileSegType);
    IOHelperFcns::readVar<EtaDependency, unsigned int>( ppDir, 
        "etaDependency",
        fileEtaDep);
    IOHelperFcns::readVar<EtDependency, unsigned int>( ppDir, 
        "etDependency",
        fileEtDep);
    //ATH_MSG_DEBUG("It's type is " << toStr(ppType) << " and dependency is :["
    //    << toStr(fileSegType) << ","
    //    << toStr(fileEtaDep) << "," 
    //    << toStr(fileEtDep) << "]");
    // Create wrapper and append it to collection. Here we make some checks
    // whether the wrapper preprocessing type is not IDiscriminatorVarDep.
    // For some cases, we have some template specializations.
    switch ( ppType )
    {
      case PreProcessing::Type::Norm1:
      {
        if (fileEtaDep || fileEtDep) {
          throw std::runtime_error(std::string("Cannot have Norm1 Preprocessor "
                "Wrapper eta/et depedent"));
        }
        // We create pre-processing wrapper specialized for each segmentation
        // type for the norm1, which is one normalization that we shall use
        // frequently. On the other hand, as it hasn't parameters, it makes
        // no sense for it to be variable dependent:
        READ_SEG_DEP_WRAPPER(ppWrapperCol, 
            PreProcessing::Norm::Norm1VarDep, 
            fileSegType, 
            EtaIndependent, 
            EtIndependent, 
            ppDir, 
            version)
        break;
      }
      //case PreProcessing::Norm::Type::Spherization:
      //{
      //  //  We might want to uncomment this if we want to use spherization
      //  // as a normalization procedure:
      //
      //  READ_ALL_DEP_WRAPPER(ppWrapperCol, 
      //      PreProcessing::Norm::SpherizationVarDep, 
      //      fileSegType, 
      //      fileEtaDep, 
      //      fileEtDep, 
      //      ppDir, 
      //      version)
      //}
      default:
      {
        //ATH_MSG_DEBUG("Using interface for PreProcessing Wrapper on folder \""
        //    << folderName << "\" and type \"" << ppType.Data() "\"" );
        /*
         * In this case, there is no dedicated wrapper for the collection,
         * we use the interface then:
         */
        READ_ALL_DEP_WRAPPER(ppWrapperCol,
            PreProcessing::IPreProcessorVarDep,
            fileSegType,
            fileEtaDep,
            fileEtDep,
            ppDir,
            version)
      }
    }
  }
}

} // namespace Ringer
