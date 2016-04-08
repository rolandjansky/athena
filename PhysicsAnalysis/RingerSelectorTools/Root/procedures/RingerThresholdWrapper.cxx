/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerThresholdWrapper.cxx 670599 2015-05-28 14:15:35Z wsfreund $
#include "RingerSelectorTools/procedures/RingerThresholdWrapper.h"

namespace Ringer {
#if !(RINGER_USE_NEW_CPP_FEATURES)
const char* IThresWrapper::wrapName = "RingerThresholdWrapper"; 
#else
constexpr const char* IThresWrapper::wrapName; 
#endif

// =============================================================================
void IThresWrapper::writeWrapper(const IThresWrapper *thresWrapper, 
    const char *fileName)
{
  TFile wrapperFile(fileName, "UPDATE");
  IOHelperFcns::checkFile( wrapperFile );
  TDirectory* configDir = wrapperFile.GetDirectory("");

  // Add package version to directory
  IOHelperFcns::writeVersion(configDir);

  // Write threshold wrapper-processing wrappers:
  thresWrapper->write(configDir, IOHelperFcns::makeIdxStr(0).c_str());

  // Write and close file
  wrapperFile.Write();
  wrapperFile.Close();
}

// =============================================================================
void IThresWrapper::read(IThresWrapper *&thresWrapper, 
    const char* fileName)
{
  // Try to open file and check if nothing wrong happened:
  TFile wrapperFile(fileName, "READ");
  IOHelperFcns::checkFile( wrapperFile );
  TDirectory* configDir = wrapperFile.GetDirectory("");

  // Get written version:
  unsigned version = IOHelperFcns::getWrittenVersion( configDir );

  std::string folderName = IThresWrapper::wrapName +
    IOHelperFcns::makeIdxStr(0);

  //ATH_MSG_DEBUG("Reading Discriminatior Wrapper on folder \""
  //    << folderName << "\"" );
  TDirectory *thresDir = configDir->GetDirectory(folderName.c_str());
  if ( thresDir == nullptr || thresDir == configDir )
  {
    throw std::runtime_error(std::string("Could not find directory \"")
          + folderName + "\" containing Discriminatior Wrapper" 
          "information.");
  }

  // We create a dummy collection just to use it on the macro
  IThresWrapperCollection thresWrapperCol;

  // Get basic preprocessing wrapper information:
  thresEnum_t thresType;
  EtaDependency fileEtaDep;
  EtDependency fileEtDep;
  IOHelperFcns::readVar<thresEnum_t, unsigned int>( thresDir, 
      "thresType",
      thresType);
  IOHelperFcns::readVar<EtaDependency, unsigned int>( thresDir, 
      "etaDependency",
      fileEtaDep);
  IOHelperFcns::readVar<EtDependency, unsigned int>( thresDir, 
      "etDependency",
      fileEtDep);
  //ATH_MSG_DEBUG("It's type is " << toStr(thresType) << " and dependency is :["
  //    << toStr(fileEtaDep) << "," 
  //    << toStr(fileEtDep) << "]");
  // Create wrapper. Here we make some checks whether the wrapper preprocessing
  // type is not IThresholdVarDep.  For some cases, we have some template
  // specializations.
  switch ( thresType )
  {
    case Discrimination::Type::UniqueThreshold:
    {
      // We create a specialized unique threshold for the UniqueThreshold
      // configuration: 
      READ_ETA_ET_DEP_WRAPPER(thresWrapperCol, 
          Discrimination::UniqueThresholdVarDep, 
          NoSegmentation, 
          fileEtaDep, 
          fileEtDep, 
          thresDir, 
          version)
      break;
    }
    default:
    {
      //ATH_MSG_DEBUG("Using interface for Discriminatior Wrapper on folder \""
      //    << folderName << "\" and type \"" << thresType.Data() "\"" );
      /*
       * In this case, there is no dedicated wrapper for the collection,
       * we use the interface then:
       */
      READ_ETA_ET_DEP_WRAPPER(thresWrapperCol,
          Discrimination::IThresholdVarDep,
          NoSegmentation,
          fileEtaDep,
          fileEtDep,
          thresDir,
          version)
    }
  }

  // Set pointer to memory address from the read thres wrapper.
  thresWrapper = thresWrapperCol[0];

  // We already got all the information on the file, we can close it:
  wrapperFile.Close();
}

} // namespace Ringer
