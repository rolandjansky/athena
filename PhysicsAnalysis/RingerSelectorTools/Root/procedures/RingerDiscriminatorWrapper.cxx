/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerDiscriminatorWrapper.cxx 670599 2015-05-28 14:15:35Z wsfreund $
#include "RingerSelectorTools/procedures/RingerDiscriminatorWrapper.h"

namespace Ringer {
#if !(RINGER_USE_NEW_CPP_FEATURES)
const char* IDiscrWrapper::wrapName = "RingerDiscriminatorWrapper"; 
#else
constexpr const char* IDiscrWrapper::wrapName; 
#endif

// =============================================================================
void IDiscrWrapper::writeCol(const IDiscrWrapperCollection &discrWrapperCol, 
    const char *fileName)
{
  TFile wrapperFile(fileName, "UPDATE");
  IOHelperFcns::checkFile( wrapperFile );

  TDirectory* configDir = wrapperFile.GetDirectory("");

  // Add package version to directory
  IOHelperFcns::writeVersion(configDir);

  // Write discrimination wrappers:
  unsigned discrWrapperSize = discrWrapperCol.size();
  IOHelperFcns::writeVar( configDir, "discrWrapperSize", discrWrapperSize );
  for (size_t discrIdx = 0;  discrIdx < discrWrapperSize; ++discrIdx ) {
    // Write wrapper on the file:
    discrWrapperCol[discrIdx]->write(configDir, 
        IOHelperFcns::makeIdxStr(discrIdx).c_str());
  }

  // Write and close file
  wrapperFile.Write();
  wrapperFile.Close();
}

// =============================================================================
void IDiscrWrapper::read(IDiscrWrapperCollection &discrWrapperCol, 
    const char* fileName)
{
  // Try to open file and check if nothing wrong happened:
  TFile wrapperFile(fileName, "READ");
  IOHelperFcns::checkFile( wrapperFile );
  TDirectory* configDir = wrapperFile.GetDirectory("");

  // Get written version:
  unsigned version = IOHelperFcns::getWrittenVersion( configDir );

  // Get its size:
  unsigned discrWrapperSize(0);
  IOHelperFcns::readVar( configDir, "discrWrapperSize", discrWrapperSize );

  // Cycle through discrimination collection and fill it:
  for ( unsigned counter = 0; counter < discrWrapperSize; ++counter) {
    std::string folderName = IDiscrWrapper::wrapName +
      IOHelperFcns::makeIdxStr(counter);

    //ATH_MSG_DEBUG("Reading Discriminatior Wrapper on folder \""
    //    << folderName << "\"" );
    TDirectory *discrDir = configDir->GetDirectory(folderName.c_str());
    if ( discrDir == nullptr || discrDir == configDir )
    {
      throw std::runtime_error(std::string("Could not find directory \"")
            + folderName + "\" containing Discriminatior Wrapper" 
            "information.");
    }
    // Get basic preprocessing wrapper information:
    discrEnum_t discrType;
    EtaDependency fileEtaDep;
    EtDependency fileEtDep;
    SegmentationType fileSegType;
    IOHelperFcns::readVar<discrEnum_t, unsigned int>( discrDir, 
        "discrType",
        discrType);
    IOHelperFcns::readVar<SegmentationType, unsigned int>( discrDir,
        "segType",
        fileSegType);
    IOHelperFcns::readVar<EtaDependency, unsigned int>( discrDir, 
        "etaDependency",
        fileEtaDep);
    IOHelperFcns::readVar<EtDependency, unsigned int>( discrDir, 
        "etDependency",
        fileEtDep);
    //ATH_MSG_DEBUG("It's type is " << toStr(discrType) << " and dependency is :["
    //    << toStr(fileSegType) << ","
    //    << toStr(fileEtaDep) << "," 
    //    << toStr(fileEtDep) << "]");
    // Create wrapper and append it to collection. Here we make some checks
    // whether the wrapper preprocessing type is not IDiscriminatorVarDep.
    // For some cases, we have some template specializations.
    switch ( discrType )
    {
      case Discrimination::Type::NNFeedForward:
      {
        // We create discrimination wrapper specialized for each segmentation
        // type for the NeuralNetwork, which is one discriminator that we shall
        // use frequently.
        READ_ALL_DEP_WRAPPER(discrWrapperCol, 
            Discrimination::NNFeedForwardVarDep, 
            fileSegType, 
            fileEtaDep, 
            fileEtDep, 
            discrDir, 
            version)
        break;
      }
      default:
      {
        //ATH_MSG_DEBUG("Using interface for Discriminatior Wrapper on folder \""
        //    << folderName << "\" and type \"" << discrType.Data() "\"" );
        /*
         * In this case, there is no dedicated wrapper for the collection,
         * we use the interface then:
         */
        READ_ALL_DEP_WRAPPER(discrWrapperCol,
            Discrimination::IDiscriminatorVarDep,
            fileSegType,
            fileEtaDep,
            fileEtDep,
            discrDir,
            version)
      }
    }
  }

  // We already got all the information on the file, we can close it:
  wrapperFile.Close();
}

} // namespace Ringer
