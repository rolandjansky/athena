/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: testRead.cxx 770805 2016-08-30 14:03:33Z ssnyder $
/**
 * In this executable we want to test if wrappers can be sucessfully read.
 *
 * This will attempt to read the files written using testWrite.
 **/

// STL includes:
#include <iostream>
#include <limits>

// Root includes:
#include <TFile.h>
#include <THashList.h>

// Gaudi includes:
#if !defined(RINGER_STANDALONE) && !defined(XAOD_STANDALONE)
# include "GaudiKernel/SystemOfUnits.h"
using namespace Gaudi::Units;
// We just do this ugly thing so that we can have a message stream
// FIXME How can I create a dummy message stream?
#define protected public
#define private public
# include "RingerSelectorTools/AsgElectronRingerSelector.h"
#undef protected
#undef private
Ringer::AsgElectronRingerSelector __AsgRingerSel__("testRead");
MsgStream &msg = __AsgRingerSel__.msg();
#else
#include "RingerSelectorTools/AsgElectronRingerSelector.h"
#include "AsgMessaging/MsgStream.h"
MsgStream msg("testRead");
#endif

#define BREAKER \
  "================================================================================"

// Local includes:
#include "RingerSelectorTools/procedures/RingerProcedureWrapper.h"
#include "RingerSelectorTools/procedures/Normalizations.h"
#include "RingerSelectorTools/procedures/NeuralNetwork.h"
#include "RingerSelectorTools/procedures/Thresholds.h"
#include "RingerSelectorTools/tools/IOHelperFcns.h"
#include "RingerSelectorTools/tools/cxx/remove_pointer.h"

using namespace Ringer;

/**
 * Read first wrapper on file with type wrapper_t
 **/
template< class wrapper_t >
void readWrapperFromFile(wrapper_t *&wrapper, const char* fileName);

/**
 * Read wrapper collection at file named fileName with base wrapper type
 * wrapperBase_t
 **/
template<typename wrapperBase_t>
void readCollectionFromFile(const char* fileName);

template<>
void readCollectionFromFile<IThresWrapper>(const char* fileName);

int main(){

  // Change message level and greet user
  msg.setLevel(MSG::DEBUG);

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading discriminator using interface "
    "without using normalization." << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read simple discriminator using interface (without using normalization)
  // ----------------------------------------------------------------------------
  // Read "basicWrapperWrong" using correct template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation > *basicWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicWrapper)>::type
  >( basicWrapper, "basicWrapper.root" );
  msg << MSG::INFO << BREAKER << endmsg;

  // Read as "basicEtaDepWrapper" using correct template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaDependent,
      EtIndependent,
      NoSegmentation > *basicEtaDepWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaDepWrapper)>::type
  >( basicEtaDepWrapper, "basicEtaDepWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtDepWrapper" using correct template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaIndependent,
      EtDependent,
      NoSegmentation > *basicEtDepWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtDepWrapper)>::type
  >( basicEtDepWrapper, "basicEtDepWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaEtDepWrapper.root" using correct template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaDependent,
      EtDependent,
      NoSegmentation > *basicEtaEtDepWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaEtDepWrapper)>::type
  >( basicEtaEtDepWrapper, "basicEtaEtDepWrapper.root");
  // -------------------------- END --- END --- END -----------------------------

  msg.setLevel(MSG::ERROR);

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading discriminator with wrong types!" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;
  // ----------------------------------------------------------------------------
  // Read simple discriminator using interface with WRONG types
  // ----------------------------------------------------------------------------
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
    EtaDependent,
    EtDependent,
    NoSegmentation > *basicWrapperWrong(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicWrapperWrong)>::type
  >( basicWrapperWrong, "basicWrapper.root" );
  msg << MSG::INFO << BREAKER << endmsg;

  // Read as "basicEtaDepWrapperWrong" using wrong template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicEtaDepWrapperWrong(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaDepWrapperWrong)>::type
  >( basicEtaDepWrapperWrong, "basicEtaDepWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtDepWrapperWrong" using wrong template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicEtDepWrapperWrong(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtDepWrapperWrong)>::type
  >( basicEtDepWrapperWrong, "basicEtDepWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaEtDepWrapperWrong.root" using wrong template
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointLayers > *basicEtaEtDepWrapperWrong(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaEtDepWrapperWrong)>::type
  >( basicEtaEtDepWrapperWrong, "basicEtaEtDepWrapper.root");
  // -------------------------- END --- END --- END -----------------------------
  msg.setLevel(MSG::DEBUG);

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading pre-processings!" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;
  // ----------------------------------------------------------------------------
  // Start testing pre-processings
  // ----------------------------------------------------------------------------
  // Read "basicPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicPPWrapper)>::type
  >( basicPPWrapper, "basicPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaDependent,
      EtIndependent,
      NoSegmentation > *basicEtaDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaDepPPWrapper)>::type
  >( basicEtaDepPPWrapper, "basicEtaDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaIndependent,
      EtDependent,
      NoSegmentation > *basicEtDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtDepPPWrapper)>::type
  >( basicEtDepPPWrapper, "basicEtDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtEtaDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaDependent,
      EtDependent,
      NoSegmentation > *basicEtEtaDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtEtaDepPPWrapper)>::type
  >( basicEtEtaDepPPWrapper, "basicEtEtaDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtEtaTrackCalDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaDependent,
      EtDependent,
      TrackCalSegmentation > *basicEtEtaTrackCalDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtEtaTrackCalDepPPWrapper)>::type
  >( basicEtEtaTrackCalDepPPWrapper, "basicEtEtaTrackCalDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtEtaTrackCalPatTypeDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaDependent,
      EtDependent,
      TrackCalPatTypeSegmentation > *basicEtEtaTrackCalPatTypeDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtEtaTrackCalPatTypeDepPPWrapper)>::type
  >( basicEtEtaTrackCalPatTypeDepPPWrapper, "basicEtEtaTrackCalPatTypeDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaEtSectionDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointSections > *basicEtaEtSectionDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaEtSectionDepPPWrapper)>::type
  >( basicEtaEtSectionDepPPWrapper, "basicEtaEtSectionDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaEtLayerDepPPWrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointLayers > *basicEtaEtLayerDepPPWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaEtLayerDepPPWrapper)>::type
  >( basicEtaEtLayerDepPPWrapper, "basicEtaEtLayerDepPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicNorm1Wrapper.root"
  RingerProcedureWrapper<PreProcessing::IPreProcessorVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicNorm1Wrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicNorm1Wrapper)>::type
  >( basicNorm1Wrapper, "basicNorm1Wrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicNorm1Wrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::Norm1VarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointLayers > *basicLayerDepNorm1Wrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicLayerDepNorm1Wrapper)>::type
  >( basicLayerDepNorm1Wrapper, "basicLayerDepNorm1Wrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicNorm2Wrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::Norm2VarDep,
      EtaIndependent,
      EtIndependent,
      TrackCalJointLayers > *basicNorm2Wrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicNorm2Wrapper)>::type
  >( basicNorm2Wrapper, "basicNorm2Wrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicSqrtWrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::SqrtVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicSqrtWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicSqrtWrapper)>::type
  >( basicSqrtWrapper, "basicSqrtWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicConstantValueWrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::ConstantValueVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicConstantValueWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicConstantValueWrapper)>::type
  >( basicConstantValueWrapper, "basicConstantValueWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicMevToGevWrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::MevToGevVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicMevToGevWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicMevToGevWrapper)>::type
  >( basicMevToGevWrapper, "basicMevToGevWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicSequentialWrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::SequentialVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicSequentialWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicSequentialWrapper)>::type
  >( basicSequentialWrapper, "basicSequentialWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicSpherizationWrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::SpherizationVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicSpherizationWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicSpherizationWrapper)>::type
  >( basicSpherizationWrapper, "basicSpherizationWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicMinMaxWrapper.root"
  RingerProcedureWrapper<PreProcessing::Norm::MinMaxVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *basicMinMaxWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicMinMaxWrapper)>::type
  >( basicMinMaxWrapper, "basicMinMaxWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading discriminators with pre-processings!" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read discriminators with pre-processings
  // ----------------------------------------------------------------------------
  // Read "randomNorm1PPDiscrWrapper.root"
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaDependent,
      EtDependent,
      NoSegmentation > *randomNorm1PPDiscrWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(randomNorm1PPDiscrWrapper)>::type
  >( randomNorm1PPDiscrWrapper, "randomNorm1PPDiscrWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "randomNorm1PPDiscrWrapper.root"
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaDependent,
      EtDependent,
      NoSegmentation > *randomDepPPrandomDepPPDiscrWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(randomDepPPrandomDepPPDiscrWrapper)>::type
  >( randomDepPPrandomDepPPDiscrWrapper, "randomDepPPrandomDepPPDiscrWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;
  // ----------------------------------------------------------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading segmented discriminator PP wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read segmented discriminators with pre-processings
  // ----------------------------------------------------------------------------
  // Read "randomDepPPrandomDepPPDiscrSegmentedWrapper.root"
  RingerProcedureWrapper<Discrimination::IDiscriminatorVarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointLayers > *randomDepPPrandomDepPPDiscrSegmentedWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(randomDepPPrandomDepPPDiscrSegmentedWrapper)>::type
  >( randomDepPPrandomDepPPDiscrSegmentedWrapper, "randomDepPPrandomDepPPDiscrSegmentedWrapper.root");
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading specialized discriminator:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read specialized discriminator wrapper with no pre-processings
  // ----------------------------------------------------------------------------
  // Read "NNWrapper.root"
  RingerProcedureWrapper<Discrimination::NNFeedForwardVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *NNWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(NNWrapper)>::type
  >( NNWrapper, "NNWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "NNWrapper.root"
  RingerProcedureWrapper<Discrimination::NNFeedForwardVarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointLayers > *FullDepNNWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(FullDepNNWrapper)>::type
  >( FullDepNNWrapper, "FullDepNNWrapper.root");
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading specialized discriminator with PP wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read specialized discriminator wrapper with specialized pre-processings
  // ----------------------------------------------------------------------------
  // Read "Norm1NNWrapper.root"
  RingerProcedureWrapper<Discrimination::NNFeedForwardVarDep,
      EtaIndependent,
      EtIndependent,
      NoSegmentation > *Norm1NNWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(Norm1NNWrapper)>::type
  >( Norm1NNWrapper, "Norm1NNWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "Norm1Norm1FullDepNNWrapper.root"
  RingerProcedureWrapper<Discrimination::NNFeedForwardVarDep,
      EtaDependent,
      EtDependent,
      TrackCalJointLayers > *Norm1Norm1FullDepNNWrapper(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(Norm1Norm1FullDepNNWrapper)>::type
  >( Norm1Norm1FullDepNNWrapper, "Norm1Norm1FullDepNNWrapper.root");
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading threshold wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read thresholds
  // ----------------------------------------------------------------------------
  // Read "basicThres.root"
  RingerProcedureWrapper<Discrimination::IThresholdVarDep,
      EtaIndependent,
      EtIndependent > *basicThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicThres)>::type
  >( basicThres, "basicThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaDepThres.root"
  RingerProcedureWrapper<Discrimination::IThresholdVarDep,
      EtaDependent,
      EtIndependent > *basicEtaDepThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaDepThres)>::type
  >( basicEtaDepThres, "basicEtaDepThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtDepThres.root"
  RingerProcedureWrapper<Discrimination::IThresholdVarDep,
      EtaIndependent,
      EtDependent > *basicEtDepThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtDepThres)>::type
  >( basicEtDepThres, "basicEtDepThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "basicEtaEtDepThres.root"
  RingerProcedureWrapper<Discrimination::IThresholdVarDep,
      EtaDependent,
      EtDependent > *basicEtaEtDepThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(basicEtaEtDepThres)>::type
  >( basicEtaEtDepThres, "basicEtaEtDepThres.root");
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Reading specialized threshold wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Read speaclized thresholds
  // ----------------------------------------------------------------------------
  // Read "uniqueThres.root"
  RingerProcedureWrapper<Discrimination::UniqueThresholdVarDep,
      EtaIndependent,
      EtIndependent > *uniqueThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(uniqueThres)>::type
  >( uniqueThres, "uniqueThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "uniqueEtaDepThres.root"
  RingerProcedureWrapper<Discrimination::UniqueThresholdVarDep,
      EtaDependent,
      EtIndependent > *uniqueEtaDepThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(uniqueEtaDepThres)>::type
  >( uniqueEtaDepThres, "uniqueEtaDepThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "uniqueEtDepThres.root"
  RingerProcedureWrapper<Discrimination::UniqueThresholdVarDep,
      EtaIndependent,
      EtDependent > *uniqueEtDepThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(uniqueEtDepThres)>::type
  >( uniqueEtDepThres, "uniqueEtDepThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Read "uniqueEtaEtDepThres.root"
  RingerProcedureWrapper<Discrimination::UniqueThresholdVarDep,
      EtaDependent,
      EtDependent > *uniqueEtaEtDepThres(nullptr);
  readWrapperFromFile<
    typename Ringer::remove_pointer<decltype(uniqueEtaEtDepThres)>::type
  >( uniqueEtaEtDepThres, "uniqueEtaEtDepThres.root");
  // -------------------------- END --- END --- END -----------------------------


  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Special reading" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Test reading multiple wrappers
  // ----------------------------------------------------------------------------
  readCollectionFromFile<IDiscrWrapper>("Norm1NNWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;
  readCollectionFromFile<IDiscrWrapper>("Norm1Norm1FullDepNNWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;
  readCollectionFromFile<IThresWrapper>("uniqueEtaEtDepThres.root");
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Finished!" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  return 0;
}

// =============================================================================
template< class wrapper_t >
void readWrapperFromFile(wrapper_t *&wrapper, const char* fileName)
{

  msg << MSG::INFO << "Reading wrapper: " << wrapper_t::staticFullName()
    << ", at file named: " << fileName << endmsg;

  // Read file and bring everything to memory:
  TFile wrapperFile(fileName, "READ");
  wrapperFile.ReadAll();
  TDirectory *baseDir = wrapperFile.GetDirectory("");

  std::shared_ptr<THashList> wrapperDirList(nullptr);
  if ( !( wrapperDirList = IOHelperFcns::getDirList(baseDir) ) ) {
    throw std::runtime_error(std::string("There are no directories available"));
  }

  msg << MSG::INFO << "Number(s) of folder(s) within this TFile is "
    << wrapperDirList->GetSize()
    << endmsg;

  try {

    TIter iter( wrapperDirList.get() );
    TDirectory* wrapperDir(nullptr);

    bool foundDir = false;

    while ( (wrapperDir = static_cast<TDirectoryFile*>(iter())) ){
      if ( wrapperDir->GetName() ==
            ( std::string(wrapper_t::wrapName) +
              IOHelperFcns::makeIdxStr(0) ) )
      {
        msg << MSG::INFO << "Found directory named "
          << wrapperDir->GetName() << endmsg;
        foundDir = true;
        break;
      }
    }

    if ( !foundDir ){
      throw std::runtime_error(std::string("Couldn't find the wrapper directory"));
    }

    wrapper = wrapper_t::read(wrapperDir,IOHelperFcns::version());

    if (wrapper == nullptr){
      throw std::runtime_error(std::string("Returned void pointer."));
    }

    // Use dummy message stream:
    wrapper->setMsgStream(&msg);

    msg << MSG::INFO << "Printing wrapper " << endmsg;

    // to print wrapper information:
    wrapper->print();

  } catch ( const std::runtime_error &e){
    msg << MSG::ERROR << "Couldn't read due to error: "
      << e.what() << endmsg;
  }

  // Close file
  wrapperFile.Close();
}

// =============================================================================
template<typename wrapperBase_t>
void readCollectionFromFile(const char* fileName)
{

  msg << MSG::INFO << "Reading file: " << fileName << endmsg;

  // Get wrapper collection and read it:
  typename wrapperBase_t::WrapperCollection col;
  wrapperBase_t::read( col, fileName );

  unsigned counter(0);
  for ( auto *wrapper : col ) {
    if (wrapper){
      msg << MSG::INFO << "Printing wrapper " << wrapper->fullName()
        << " at position " << IOHelperFcns::makeIdxStr(counter++) << endmsg;
      wrapper->setMsgStream(&msg);wrapper->print();
    } else {
      msg << MSG::ERROR << "Couldn't print wrapper on file " << fileName << endmsg;
    }
  }

  msg << MSG::INFO << "File configuration: " << endmsg;
  AsgElectronRingerSelector::IOConfStruct ioConf;
  AsgElectronRingerSelector::retrieveFileConf( fileName, ioConf );
  AsgElectronRingerSelector::printConf(ioConf, &msg);
}

// =============================================================================
template<>
void readCollectionFromFile<IThresWrapper>(const char* fileName)
{

  msg << MSG::INFO << "Reading file: " << fileName << endmsg;

  // Get wrapper collection and read it:
  IThresWrapper *wrapper;
  IThresWrapper::read( wrapper, fileName );
  if (wrapper) {wrapper->setMsgStream(&msg);  wrapper->print();}
  else msg << MSG::ERROR << "Couldn't print wrapper on file " << fileName << endmsg;
}



