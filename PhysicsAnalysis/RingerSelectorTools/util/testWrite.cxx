/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: testWrite.cxx 770805 2016-08-30 14:03:33Z ssnyder $
/**
 * In this executable we want to test if wrappers can be sucessfully written.
 *
 * For that we will create different wrapper types and write them into different
 * files. Afterwards, it is needed to run testRead on the same path that
 * testWrite was run and check if the information written is correct.
 *
 **/

// STL includes:
#include <iostream>
#include <limits>
#include <type_traits>

// Root includes:
#include <TFile.h>

// Gaudi includes:
#if !defined(RINGER_STANDALONE) && !defined(XAOD_STANDALONE)
# include "GaudiKernel/SystemOfUnits.h"
using namespace Gaudi::Units;
// We just do this ugly thing so that we can have a message stream
// FIXME How can I create a dummy message stream without doing this?
#define protected public
#define private public
# include "RingerSelectorTools/AsgElectronRingerSelector.h"
#undef protected
#undef private
Ringer::AsgElectronRingerSelector __AsgRingerSel__("testWrite");
MsgStream &msg = __AsgRingerSel__.msg();
#else
#include "RingerSelectorTools/AsgElectronRingerSelector.h"
#include "AsgMessaging/MsgStream.h"
MsgStream msg("testWrite");
#endif

#define BREAKER \
  "================================================================================"

// Local includes:
#include "RingerSelectorTools/procedures/RingerProcedureWrapper.h"
#include "RingerSelectorTools/procedures/Normalizations.h"
#include "RingerSelectorTools/procedures/NeuralNetwork.h"
#include "RingerSelectorTools/procedures/Thresholds.h"
#include "RingerSelectorTools/tools/IOHelperFcns.h"
#include "RingerSelectorTools/tools/TrackPatternsHolder.h"

using namespace Ringer;

/**
 * Method dedicated for testing pre-processing wrapper creation
 **/
template<
  typename procedure_t,
  EtaDependency etaDependency,
  EtDependency etDependency,
  SegmentationType segType
>
IPreProcWrapper* createPPWrapper(const char *fileName,
    const std::vector<float> &etaDepBounderies = {0, 2.5},
    const std::vector<float> &etDepBounderies
      = {0, std::numeric_limits<float>::infinity()}
    );


/**
 * PP factory with segType, eta and et dependency
 **/
template<typename procedure_t>
procedure_t* ppFactoryEtaEtDep(SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax,
    float etMin, float etMax,
    MSG::Level lvl);

/**
 * PP factory with segType and eta dependency
 **/
template<typename procedure_t>
procedure_t* ppFactoryEtaDep(SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax,
    MSG::Level lvl);

/**
 * PP factory with segType and et dependency
 **/
template<typename procedure_t>
procedure_t* ppFactoryEtDep(SegmentationType cSegType,
    unsigned segIdx,
    float etMin, float etMax,
    MSG::Level lvl);

/**
 * PP factory with segType dependency
 **/
template<typename procedure_t>
procedure_t* ppFactoryDep(SegmentationType cSegType,
    unsigned segIdx = 0,
    MSG::Level lvl = MSG::INFO);

/**
 * PP random factory
 **/
PreProcessing::IPreProcessorVarDep* createRandomPP(
    SegmentationType cSegType,
    unsigned segIdx = 0,
    MSG::Level lvl = MSG::INFO);

/**
 * Create PP of t
 **/
template<typename procedure_t>
procedure_t* createPP(
    SegmentationType cSegType,
    unsigned segIdx = 0,
    MSG::Level lvl = MSG::INFO);

/**
 * Method dedicated for testing discriminator wrapper creation
 **/
template<
  typename procedure_t,
  EtaDependency etaDependency,
  EtDependency etDependency,
  SegmentationType segType
>
void createDiscrWrapper(const char* fileName,
    const IPreProcWrapperCollection* ppWrapper = nullptr,
    const std::vector<float> &etaDepBounderies = {0, 2.5},
    const std::vector<float> &etDepBounderies =
      {0, std::numeric_limits<float>::infinity()}
    );

/**
 * @brief Write wrapper to file
 **/
void writeWrapperOnFile(const IProcWrapperBase &wrapper,
    const char* fileName, MSG::Level);

/**
 * Creates eta/et independent neural network
 *
 * If goodStatus = false, returns a dummy neural network which always returns 0
 * as output for an pattern space of dimension size of 100.
 *
 * An old MC12 Zee x JF14 neural network is created otherwise.
 **/
template<bool goodStatus = true>
Discrimination::NNFeedForwardVarDep* createNN(SegmentationType cSegType,
    unsigned segIdx);

/**
 * Creates eta dependent, et independent neural network
 **/
template<bool goodStatus = true>
Discrimination::NNFeedForwardVarDep* createEtaDepNN(SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax);

/**
 * Creates eta independent, et dependent neural network
 **/
template<bool goodStatus = true>
Discrimination::NNFeedForwardVarDep* createEtDepNN(
    SegmentationType cSegType,
    unsigned segIdx,
    float etMin, float etMax);

/**
 * Creates eta dependent, et dependent neural network
 **/
template<bool goodStatus = true>
Discrimination::NNFeedForwardVarDep* createEtaEtDepNN(
    SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax,
    float etMin, float etMax);

/**
 * Method dedicated for testing threshold wrapper creation
 **/
template<
  typename procedure_t,
  EtaDependency etaDependency,
  EtDependency etDependency
>
void createThresWrapper(const char* fileName,
    const std::vector<float> &etaDepBounderies = {0, 2.5},
    const std::vector<float> &etDepBounderies =
      {0, std::numeric_limits<float>::infinity()},
    float thresValue = 0
    );

Discrimination::UniqueThresholdVarDep* createThres(float thresValue);
Discrimination::UniqueThresholdVarDep* createEtaDepThres(float etaMin, float etaMax,
    float thresValue);
Discrimination::UniqueThresholdVarDep* createEtDepThres(float etMin, float etMax,
    float thresValue);
Discrimination::UniqueThresholdVarDep* createEtaEtDepThres(
    float etaMin, float etaMax,
    float etMin, float etMax,
    float thresValue);

std::vector< std::vector<unsigned> > segmentationEntries =
{
  { 100 },
  { 100, (unsigned)(TrackPatternsHolder::numberOfPatterns()) },
  { 100, (unsigned)(TrackPatternsHolder::numberOfPatterns()), 10 },
  { 88,  12, (unsigned)(TrackPatternsHolder::numberOfPatterns()), 10 },
  { 8, 64, 8, 8, 4, 4, 4, (unsigned)(TrackPatternsHolder::numberOfPatterns()), 10 }
};

// =============================================================================
int main( /*int argc, char* argv[]*/){

#if !defined(XAOD_STANDALONE) && !defined(RINGER_STANDALONE)
  const float GeVf = GeV;
#endif

  msg.setLevel(MSG::DEBUG);

  // Eta and Et dependency bounderies:
  const std::vector<float> etaIndepBounderies = {0, 2.5};
  const std::vector<float> etaDepBounderies =
        {0, .1, .6, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47, 2.5};

  const std::vector<float> etIndepBounderies = {0, std::numeric_limits<float>::infinity()};
  const std::vector<float> etDepBounderies =
#if !defined(XAOD_STANDALONE) && !defined(RINGER_STANDALONE)
        {0, 5*GeVf, 10*GeVf, 15*GeVf, 20*GeVf, 30*GeVf, 40*GeVf, 50*GeVf, 60*GeVf, 70*GeVf, 80*GeVf, //}
#else
        {0, 5*1e3, 10*1e3, 15*1e3, 20*1e3, 30*1e3, 40*1e3, 50*1e3, 60*1e3, 70*1e3, 80*1e3,
#endif
        std::numeric_limits<float>::infinity()};

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Testing simple discriminator wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write simple discriminator using interface (without using normalization)
  // ----------------------------------------------------------------------------
  // Create independent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // No pre-processings available.
  // Write it as "basicWrapper"
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  // Create eta dependent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // No pre-processings available.
  // Write it as "basicEtaDepWrapper"
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaDependent,
    EtIndependent,
    NoSegmentation >("basicEtaDepWrapper.root",
        nullptr,
        etaDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  // Create Et dependent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // No pre-processings available.
  // Write it as "basicEtDepWrapper"
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaIndependent,
    EtDependent,
    NoSegmentation >("basicEtDepWrapper.root",
        nullptr,
        etaIndepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  // Create eta, Et dependent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // No pre-processings available.
  // Write it as "basicEtDepWrapper"
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaDependent,
    EtDependent,
    NoSegmentation >("basicEtaEtDepWrapper.root",
        nullptr,
        etaDepBounderies,
        etDepBounderies);

  // Turn off the messages:
  msg.setLevel(MSG::ERROR);
  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Testing creation of badly configured wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;
  // -------------------------- END --- END --- END -----------------------------

  // ----------------------------------------------------------------------------
  // This block shoudln't work
  // ----------------------------------------------------------------------------
  // Create independent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // No pre-processings available.
  // It shouldn't be written because its eta size is different from unit.
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("shouldntWrite.root",
        nullptr,
        etaDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  // Create independent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // No pre-processings available.
  // It shouldn't be written because its eta size is different from unit.
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("shouldntWrite.root",
        nullptr,
        etaIndepBounderies,
        etDepBounderies);
  msg.setLevel(MSG::DEBUG);
  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating PP wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;
  // ----------------------------------------------------------------------------

  // ----------------------------------------------------------------------------
  // Start testing pre-processings
  // ----------------------------------------------------------------------------
  // Create eta, Et dependent and non segmented pp wrapper.
  // Test it allocating all possible permutations of seg/eta/et dependency.
  // No pre-processings available.
  // Write it as "basicPPWrapper"
  auto *basicPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicPPWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtaDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaDependent,
    EtIndependent,
    NoSegmentation >("basicEtaDepPPWrapper.root",
        etaDepBounderies);
  if (basicEtaDepPPWrapper) basicEtaDepPPWrapper->releaseMemory();
  delete basicEtaDepPPWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaIndependent,
    EtDependent,
    NoSegmentation >("basicEtDepPPWrapper.root",
        etaIndepBounderies,
        etDepBounderies);
  if (basicEtDepPPWrapper) basicEtDepPPWrapper->releaseMemory();
  delete basicEtDepPPWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtaEtDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaDependent,
    EtDependent,
    NoSegmentation >("basicEtEtaDepPPWrapper.root",
        etaDepBounderies,
        etDepBounderies);
  if (basicEtaEtDepPPWrapper) basicEtaEtDepPPWrapper->releaseMemory();
  delete basicEtaEtDepPPWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtaEtTrackCalDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaDependent,
    EtDependent,
    TrackCalSegmentation >("basicEtEtaTrackCalDepPPWrapper.root",
        etaDepBounderies,
        etDepBounderies);
  if (basicEtaEtTrackCalDepPPWrapper) basicEtaEtTrackCalDepPPWrapper->releaseMemory();
  delete basicEtaEtTrackCalDepPPWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtaEtTrackCalPatTypeDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaDependent,
    EtDependent,
    TrackCalPatTypeSegmentation >("basicEtEtaTrackCalPatTypeDepPPWrapper.root",
        etaDepBounderies,
        etDepBounderies);
  if (basicEtaEtTrackCalDepPPWrapper) basicEtaEtTrackCalPatTypeDepPPWrapper->releaseMemory();
  delete basicEtaEtTrackCalPatTypeDepPPWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtaEtSectionDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaDependent,
    EtDependent,
    TrackCalJointSections >("basicEtaEtSectionDepPPWrapper.root",
        etaDepBounderies,
        etDepBounderies);
  if (basicEtaEtSectionDepPPWrapper) basicEtaEtSectionDepPPWrapper->releaseMemory();
  delete basicEtaEtSectionDepPPWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicEtaEtLayerDepPPWrapper = createPPWrapper<PreProcessing::IPreProcessorVarDep,
    EtaDependent,
    EtDependent,
    TrackCalJointLayers >("basicEtaEtLayerDepPPWrapper.root",
        etaDepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicNorm1Wrapper = createPPWrapper<PreProcessing::Norm::Norm1VarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicNorm1Wrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicLayerDepNorm1Wrapper = createPPWrapper<PreProcessing::Norm::Norm1VarDep,
    EtaIndependent,
    EtIndependent,
    TrackCalJointLayers >("basicLayerDepNorm1Wrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicNorm2Wrapper = createPPWrapper<PreProcessing::Norm::Norm2VarDep,
    EtaIndependent,
    EtIndependent,
    TrackCalJointLayers >("basicNorm2Wrapper.root");
  if (basicNorm2Wrapper) basicNorm2Wrapper->releaseMemory();
  delete basicNorm2Wrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicSqrtWrapper = createPPWrapper<PreProcessing::Norm::SqrtVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicSqrtWrapper.root");
  if (basicSqrtWrapper) basicSqrtWrapper->releaseMemory();
  delete basicSqrtWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicConstantValueWrapper = createPPWrapper<PreProcessing::Norm::ConstantValueVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicConstantValueWrapper.root");
  if (basicConstantValueWrapper) basicConstantValueWrapper->releaseMemory();
  delete basicConstantValueWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicMevToGevWrapper = createPPWrapper<PreProcessing::Norm::MevToGevVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicMevToGevWrapper.root");
  if (basicMevToGevWrapper) basicMevToGevWrapper->releaseMemory();
  delete basicMevToGevWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicSequentialWrapper = createPPWrapper<PreProcessing::Norm::SequentialVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicSequentialWrapper.root");
  if (basicSequentialWrapper) basicSequentialWrapper->releaseMemory();
  delete basicSequentialWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicSpherizationWrapper = createPPWrapper<PreProcessing::Norm::SpherizationVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicSpherizationWrapper.root");
  if (basicSpherizationWrapper) basicSpherizationWrapper->releaseMemory();
  delete basicSpherizationWrapper;
  msg << MSG::INFO << BREAKER << endmsg;

  auto *basicMinMaxWrapper = createPPWrapper<PreProcessing::Norm::MinMaxVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("basicMinMaxWrapper.root");
  if (basicMinMaxWrapper) basicMinMaxWrapper->releaseMemory();
  delete basicMinMaxWrapper;
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating discriminator with PP wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write discriminators with pre-processings
  // ----------------------------------------------------------------------------
  // Create Eta, Et dependent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // PP available {Unspecialized Norm1}.
  // Write it as "randomNorm1DiscrWrapper.root"
  IPreProcWrapperCollection randomNorm1PP = {basicPPWrapper};
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaDependent,
    EtDependent,
    NoSegmentation >("randomNorm1PPDiscrWrapper.root",
        &randomNorm1PP,
        etaDepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  // Create Eta, Et dependent and non segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // PP available {Unspecialized}.
  // Write it as "randomDepPPrandomDepPP"
  IPreProcWrapperCollection randomDepPPrandomDepPP = {
    basicEtaEtLayerDepPPWrapper, basicEtaEtLayerDepPPWrapper
  };
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaDependent,
    EtDependent,
    NoSegmentation >("randomDepPPrandomDepPPDiscrWrapper.root",
        &randomDepPPrandomDepPP,
        etaDepBounderies,
        etDepBounderies);
  // ----------------------------------------------------------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating segmented discriminator with PP wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write segmented discriminators with pre-processings
  // ----------------------------------------------------------------------------
  // Create Eta, Et dependent and segmented wrapper.
  // Test it allocating all possible permutations of eta/et dependency.
  // PP available {Unspecialized Unspecialized}.
  // Write it as "randomDepPPrandomDepPPDiscrSegmentedWrapper"
  createDiscrWrapper<Discrimination::IDiscriminatorVarDep,
    EtaDependent,
    EtDependent,
    TrackCalJointLayers >("randomDepPPrandomDepPPDiscrSegmentedWrapper.root",
        &randomDepPPrandomDepPP,
        etaDepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating specialized discriminator:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write specialized discriminator wrapper with no pre-processings
  // ----------------------------------------------------------------------------
  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("NNWrapper.root");
  msg << MSG::INFO << BREAKER << endmsg;

  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaDependent,
    EtDependent,
    TrackCalJointLayers >("FullDepNNWrapper.root",
        {},
        etaDepBounderies,
        etDepBounderies);
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating specialized discriminator with PP wrappers:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write specialized discriminator wrapper with specialized pre-processings
  // ----------------------------------------------------------------------------
  IPreProcWrapperCollection basicNorm1WrapperVec = {basicNorm1Wrapper};
  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("Norm1NNWrapper.root", &basicNorm1WrapperVec);
  msg << MSG::INFO << BREAKER << endmsg;

  IPreProcWrapperCollection fullDepWrapperVec0 = {basicLayerDepNorm1Wrapper};
  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaDependent,
    EtDependent,
    TrackCalJointLayers >("Norm1FullDepNNWrapper.root",
        &fullDepWrapperVec0,
        etaDepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  IPreProcWrapperCollection fullDepWrapperVec = {basicLayerDepNorm1Wrapper, basicLayerDepNorm1Wrapper};
  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaDependent,
    EtDependent,
    TrackCalJointLayers >("Norm1Norm1FullDepNNWrapper.root",
        &fullDepWrapperVec,
        etaDepBounderies,
        etDepBounderies);
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating thresholds:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write thresholds
  // ----------------------------------------------------------------------------
  createThresWrapper<Discrimination::IThresholdVarDep,
    EtaIndependent,
    EtIndependent >("basicThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::IThresholdVarDep,
    EtaDependent,
    EtIndependent >("basicEtaDepThres.root",
        etaDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::IThresholdVarDep,
    EtaIndependent,
    EtDependent >("basicEtDepThres.root",
        etaIndepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::IThresholdVarDep,
    EtaDependent,
    EtDependent >("basicEtaEtDepThres.root",
        etaDepBounderies,
        etDepBounderies);
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Creating specialized thresholds:" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  // ----------------------------------------------------------------------------
  // Write specialized thresholds
  // ----------------------------------------------------------------------------
  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaIndependent,
    EtIndependent >("uniqueThres.root");
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaDependent,
    EtIndependent >("uniqueEtaDepThres.root",
        etaDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaIndependent,
    EtDependent >("uniqueEtDepThres.root",
        etaIndepBounderies,
        etDepBounderies);
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaDependent,
    EtDependent >("uniqueEtaEtDepThres.root",
        etaDepBounderies,
        etDepBounderies);
  // -------------------------- END --- END --- END -----------------------------


  // ----------------------------------------------------------------------------
  // Creating test discrimination wrappers:
  // ----------------------------------------------------------------------------
  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("ElectronRingerDiscrTestLoose.root", &basicNorm1WrapperVec);
  msg << MSG::INFO << BREAKER << endmsg;

  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("ElectronRingerDiscrTestMedium.root", &basicNorm1WrapperVec);
  msg << MSG::INFO << BREAKER << endmsg;

  createDiscrWrapper<Discrimination::NNFeedForwardVarDep,
    EtaIndependent,
    EtIndependent,
    NoSegmentation >("ElectronRingerDiscrTestTight.root", &basicNorm1WrapperVec);
  msg << MSG::INFO << BREAKER << endmsg;
  // -------------------------- END --- END --- END -----------------------------

  // ----------------------------------------------------------------------------
  // Creating test threshold wrappers:
  // ----------------------------------------------------------------------------
  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaIndependent,
    EtIndependent >("ElectronRingerThresTestLoose.root",
        etaIndepBounderies,
        etIndepBounderies,
        0.948 );
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaIndependent,
    EtIndependent >("ElectronRingerThresTestMedium.root",
        etaIndepBounderies,
        etIndepBounderies,
        0.2644 );
  msg << MSG::INFO << BREAKER << endmsg;

  createThresWrapper<Discrimination::UniqueThresholdVarDep,
    EtaIndependent,
    EtIndependent >("ElectronRingerThresTestTight.root",
        etaIndepBounderies,
        etIndepBounderies,
        0.2826 );
  // -------------------------- END --- END --- END -----------------------------

  msg << MSG::INFO << BREAKER << endmsg;
  msg << MSG::INFO << "Finished!" << endmsg;
  msg << MSG::INFO << BREAKER << endmsg;

  return 0;
}

// =============================================================================
void writeWrapperOnFile(const IProcWrapperBase &wrapper, const char* fileName,
    MSG::Level lvl)
{
  msg << lvl << "Writing to file!" << endmsg;

  TFile wrapperFile(fileName, "RECREATE");
  auto wrapperDir = wrapperFile.GetDirectory("");

  // Write wrapper on the file:
  wrapper.write(wrapperDir, IOHelperFcns::makeIdxStr(0).c_str());

  if ( msg.level() <= lvl ){
    wrapperDir->ls();
  }

  // Write and close file
  wrapperFile.Write();
  wrapperFile.Close();
}

// =============================================================================
template<
  typename procedure_t,
  EtaDependency etaDependency,
  EtDependency etDependency,
  SegmentationType segType
>
IPreProcWrapper* createPPWrapper(const char *fileName,
    const std::vector<float> &etaDepBounderies,
    const std::vector<float> &etDepBounderies)
{
  // Typedef to make things easier
  typedef RingerProcedureWrapper<procedure_t,
          etaDependency,
          etDependency,
          segType> wrapper_t;

  typedef typename RingerProcedureType<procedure_t>::procEnum_t procEnum_t;

  msg << MSG::INFO << "Attempting to create a RingerProcedureWrapper<"
        << toStr(procedure_t::template procType<procEnum_t>() )
        << ((std::is_same<procedure_t,PreProcessing::IPreProcessorVarDep>::value)?",":"(VarDep),")
        << toStr(etaDependency) << ","
        << toStr(etDependency) << ","
        << toStr(segType) << "> with file name: " << fileName << "." << endmsg;

  unsigned etaDepProc = etaDepBounderies.size() - 1;
  unsigned etDepProc = etDepBounderies.size() - 1;

  IPreProcWrapper *ret(nullptr);

  // Loop over variable dependencies
  for ( unsigned uSegType = NoSegmentation;
      uSegType < NSegmentations;
      ++uSegType )
  {
    SegmentationType cSegType = static_cast<SegmentationType>(uSegType);
    //if ( segType == SegmentationType::TrackCalPatTypeSegmentation ) {
    //  continue;
    //}
    for ( unsigned uEtaDep = EtaIndependent; uEtaDep <= EtaDependent; ++uEtaDep ){
      EtaDependency etaDep = static_cast<EtaDependency>(uEtaDep);
      for ( unsigned uEtDep = EtIndependent; uEtDep <= EtDependent; ++uEtDep ){
        EtDependency etDep = static_cast<EtDependency>(uEtDep);

        // Change message level accondingly if we are creating correct wrapper
        // type
        MSG::Level lvl(MSG::VERBOSE);
        if ( cSegType == segType &&
            etaDep == etaDependency &&
            etDep == etDependency )
        {
          lvl = MSG::INFO;
        }


        // The collection holder
        typename wrapper_t::PPDepProcCollection ppCol(
            numberOfSegments(cSegType),
            std::vector< std::vector<procedure_t*> >(
                etDepProc, std::vector<procedure_t*>(etaDepProc,
                  nullptr)));

        // Fill collection
        for ( unsigned segDepIdx = 0; segDepIdx < numberOfSegments(cSegType); ++segDepIdx ){
          for ( unsigned etDepIdx = 0; etDepIdx < etDepProc; ++etDepIdx){
            // Get et bounderies:
            float etMin(etDepBounderies[etDepIdx]),
                  etMax(etDepBounderies[etDepIdx+1]);
            for ( unsigned etaDepIdx = 0; etaDepIdx < etaDepProc; ++etaDepIdx){
              // Get eta bounderies:
              float etaMin(etaDepBounderies[etaDepIdx]),
                    etaMax(etaDepBounderies[etaDepIdx+1]);


              // Fill collection:
              if (etaDep) {
                if (etDep) {
                  ppCol[segDepIdx][etDepIdx][etaDepIdx] =
                    ppFactoryEtaEtDep<procedure_t>(cSegType,
                        segDepIdx,
                        etaMin,
                        etaMax,
                        etMin,
                        etMax,
                        MSG::VERBOSE);
                } else {
                  ppCol[segDepIdx][etDepIdx][etaDepIdx] =
                    ppFactoryEtaDep<procedure_t>(cSegType,
                        segDepIdx,
                        etaMin,
                        etaMax,
                        MSG::VERBOSE);
                }
              } else {
                if (etDep) {
                  ppCol[segDepIdx][etDepIdx][etaDepIdx] =
                    ppFactoryEtDep<procedure_t>(cSegType,
                        segDepIdx,
                        etMin,
                        etMax,
                        MSG::VERBOSE);
                } else {
                  ppCol[segDepIdx][etDepIdx][etaDepIdx] =
                    ppFactoryDep<procedure_t>(cSegType,
                        segDepIdx,
                        MSG::VERBOSE);
                }
              }
            }
          }
        } // Finished looping and filling collection

        // Attempt to create a wrapper with filled collection:
        try {

          msg << lvl << "Attempting to create wrapper with ("
            << toStr(segType) << "," << toStr(etaDep) << "," << toStr(etDep)
            << ") pre-processing" << endmsg;

          //msg << lvl << ppCol << endmsg;

          // Create wrapper:
          auto wrapper = new wrapper_t(ppCol);
          // Use dummy message stream:
          wrapper->setMsgStream(&msg);
          // Print wrapper:
          wrapper->print(MSG::DEBUG);

          // If it succeed, write it into file:
          writeWrapperOnFile(*wrapper, fileName, MSG::VERBOSE);
          //IPreProcWrapperCollection ppWrapperCol = {wrapper};
          //IPreProcWrapper::writeCol( ppWrapperCol, fileName );

          // We do not release memory for the wrapper, it will be given as a
          // return parameter.
          ret = wrapper;

          msg << lvl << "Succeed!" << endmsg;

        } catch (const std::runtime_error &e) {

          msg << MSG::VERBOSE << "Couldn't create wrapper. Reason: "
            << e.what() << endmsg;

          // Release collection memory
          for ( unsigned segDepIdx = 0; segDepIdx < numberOfSegments(cSegType); ++segDepIdx ){
            for ( unsigned etDepIdx = 0; etDepIdx < etDepProc; ++etDepIdx){
              for ( unsigned etaDepIdx = 0; etaDepIdx < etaDepProc; ++etaDepIdx){
                delete ppCol[segDepIdx][etDepIdx][etaDepIdx];
                ppCol[segDepIdx][etDepIdx][etaDepIdx] = nullptr;
              }
            }
          }
        } // Finished creating wrapper and handling possible errors

      }
    }
  }  // Finished looping over segType/eta/et.
  return ret;
}

// =============================================================================
template<typename procedure_t>
procedure_t* ppFactoryEtaEtDep(SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax,
    float etMin, float etMax,
    MSG::Level lvl)
{
  procedure_t* pp(nullptr);
  if ( std::is_same<procedure_t,PreProcessing::IPreProcessorVarDep>::value )
  {
    pp = reinterpret_cast<procedure_t*>(createRandomPP(cSegType, segIdx, lvl));
  } else {
    pp = createPP<procedure_t>( cSegType, segIdx, lvl );
  }
  pp->setEtaEtDep(etaMin, etaMax, etMin, etMax);
  return pp;
}

// =============================================================================
template<typename procedure_t>
procedure_t* ppFactoryEtaDep(SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax,
    MSG::Level lvl)
{
  procedure_t* pp(nullptr);
  if ( std::is_same<procedure_t,PreProcessing::IPreProcessorVarDep>::value )
  {
    pp = reinterpret_cast<procedure_t*>(createRandomPP(cSegType, segIdx, lvl));
  } else {
    pp = createPP<procedure_t>( cSegType, segIdx, lvl );
  }
  pp->setEtaDep(etaMin, etaMax);
  return pp;
}

// =============================================================================
template<typename procedure_t>
procedure_t* ppFactoryEtDep(SegmentationType cSegType,
    unsigned segIdx,
    float etMin, float etMax,
    MSG::Level lvl)
{
  procedure_t* pp(nullptr);
  if ( std::is_same<procedure_t,PreProcessing::IPreProcessorVarDep>::value )
  {
    pp = reinterpret_cast<procedure_t*>(createRandomPP(cSegType, segIdx, lvl));
  } else {
    pp = createPP<procedure_t>( cSegType, segIdx, lvl );
  }
  pp->setEtDep(etMin, etMax);
  return pp;
}

// =============================================================================
template<typename procedure_t>
procedure_t* ppFactoryDep(SegmentationType cSegType,
    unsigned segIdx,
    MSG::Level lvl)
{
  if ( std::is_same<procedure_t,PreProcessing::IPreProcessorVarDep>::value )
  {
    return reinterpret_cast<procedure_t*>(createRandomPP( cSegType, segIdx, lvl));
  } else {
    return createPP<procedure_t>( cSegType, segIdx, lvl );
  }
}

// =============================================================================
PreProcessing::IPreProcessorVarDep* createRandomPP(SegmentationType cSegType,
    unsigned segIdx,
    MSG::Level lvl)
{
  static unsigned sequenceLoop = 0;
  switch ( static_cast<PreProcessing::Type::PreProcessorTypes>(2+((sequenceLoop++)%7)) ) {
    case PreProcessing::Type::Norm1:
      msg << lvl << "Creating Norm1" << endmsg;
      return new PreProcessing::Norm::Norm1VarDep;
      break;
    case PreProcessing::Type::Norm2:
      msg << lvl << "Creating Norm2" << endmsg;
      return new PreProcessing::Norm::Norm2VarDep;
      break;
    case PreProcessing::Type::Sqrt:
      msg << lvl << "Creating Sqrt" << endmsg;
      return new PreProcessing::Norm::SqrtVarDep;
      break;
    case PreProcessing::Type::ConstantValue:
      msg << lvl << "Creating ConstantValue" << endmsg;
      return new PreProcessing::Norm::ConstantValueVarDep(987);
      break;
    case PreProcessing::Type::Sequential:
      msg << lvl << "Creating Sequential" << endmsg;
      return new PreProcessing::Norm::SequentialVarDep(100,.001);
      break;
    case PreProcessing::Type::Spherization:
    {
      msg << lvl << "Creating Spherization" << endmsg;
      auto std = new PreProcessing::Norm::SpherizationVarDep(
          std::vector<float>( segmentationEntries[cSegType][segIdx], 21 ),
          std::vector<float>( segmentationEntries[cSegType][segIdx], 61 )
          );
      return std;
      break;
    }
    case PreProcessing::Type::MinMax:
    {
      msg << lvl << "Creating MinMax" << endmsg;
      auto minMax = new PreProcessing::Norm::MinMaxVarDep(
          std::vector<float>( segmentationEntries[cSegType][segIdx], 11 ),
          std::vector<float>( segmentationEntries[cSegType][segIdx], 92 )
          );
      return minMax;
      break;
    }
    default:
      throw std::runtime_error(std::string( "Couldn't create pre-processing." ));
  }
}

// =============================================================================
template<typename procedure_t>
procedure_t* createPP(SegmentationType cSegType, unsigned segIdx,
    MSG::Level /*lvl*/)
{
  if ( Ringer::is_same<procedure_t,PreProcessing::Norm::Norm1VarDep>::value ){
    return reinterpret_cast<procedure_t*>(new PreProcessing::Norm::Norm1VarDep);
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::Norm2VarDep>::value ) {
    return reinterpret_cast<procedure_t*>(new PreProcessing::Norm::Norm2VarDep);
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::SqrtVarDep>::value ) {
    return reinterpret_cast<procedure_t*>(new PreProcessing::Norm::SqrtVarDep);
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::ConstantValueVarDep>::value ) {
    return reinterpret_cast<procedure_t*>(new PreProcessing::Norm::ConstantValueVarDep(123));
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::MevToGevVarDep>::value ) {
    return reinterpret_cast<procedure_t*>(new PreProcessing::Norm::MevToGevVarDep);
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::SequentialVarDep>::value ) {
    auto seq = new PreProcessing::Norm::SequentialVarDep(100,.001);
    return reinterpret_cast<procedure_t*>(seq);
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::SpherizationVarDep>::value ) {
    auto std = new PreProcessing::Norm::SpherizationVarDep(
        std::vector<float>( segmentationEntries[cSegType][segIdx], 21 ),
        std::vector<float>( segmentationEntries[cSegType][segIdx], 61 )
        );
    return reinterpret_cast<procedure_t*>(std);
  } else if ( Ringer::is_same<procedure_t,PreProcessing::Norm::MinMaxVarDep>::value ) {
    auto minMax = new PreProcessing::Norm::MinMaxVarDep(
        std::vector<float>( segmentationEntries[cSegType][segIdx], 11 ),
        std::vector<float>( segmentationEntries[cSegType][segIdx], 92 )
        );
    return reinterpret_cast<procedure_t*>(minMax);
  } else {
    throw std::runtime_error(std::string("Unknown pre-processing type."));
  }
}

// =============================================================================
template<
  typename procedure_t,
  EtaDependency etaDependency,
  EtDependency etDependency,
  SegmentationType segType
>
void createDiscrWrapper(const char *fileName,
    const IPreProcWrapperCollection* ppWrapperCol,
    const std::vector<float> &etaDepBounderies,
    const std::vector<float> &etDepBounderies)
{
  // Typedef to make things easier
  typedef RingerProcedureWrapper<procedure_t,
          etaDependency,
          etDependency,
          segType> wrapper_t;

  typedef typename RingerProcedureType<procedure_t>::procEnum_t procEnum_t;

  msg << MSG::INFO << "Attempting to create a RingerProcedureWrapper<"
        << toStr(procedure_t::template procType<procEnum_t>() )
        << ((Ringer::is_same<procedure_t,Discrimination::IDiscriminatorVarDep>::value)?",":"(VarDep),")
        << toStr(etaDependency) << ","
        << toStr(etDependency) << ","
        << toStr(segType) << "> with file name: " << fileName << "." << endmsg;

  unsigned etaDepProc = etaDepBounderies.size() - 1;
  unsigned etDepProc = etDepBounderies.size() - 1;

  // Loop over variable dependencies
  for ( unsigned uEtaDep = EtaIndependent; uEtaDep <= EtaDependent; ++uEtaDep ){
    for ( unsigned uEtDep = EtIndependent; uEtDep <= EtDependent; ++uEtDep ){
      // The enumerations
      EtaDependency etaDep = static_cast<EtaDependency>(uEtaDep);
      EtDependency etDep = static_cast<EtDependency>(uEtDep);

      // Change message level accondingly if we are creating correct wrapper
      // type
      MSG::Level lvl(MSG::VERBOSE);
      if ( etaDep == etaDependency &&
          etDep == etDependency )
      {
        lvl = MSG::INFO;
      }

      // The collection holder
      typename wrapper_t::DiscrDepProcCollection discrCol(numberOfSegments(segType),
          std::vector< std::vector<procedure_t*> >(
              etDepProc, std::vector<procedure_t*>(etaDepProc,
                nullptr)));

      // Fill collection
      for ( unsigned segDepIdx = 0; segDepIdx < numberOfSegments(segType); ++segDepIdx ){
        for ( unsigned etDepIdx = 0; etDepIdx < etDepProc; ++etDepIdx){
          // Get et bounderies:
          float etMin(etDepBounderies[etDepIdx]),
                etMax(etDepBounderies[etDepIdx+1]);
          for ( unsigned etaDepIdx = 0; etaDepIdx < etaDepProc; ++etaDepIdx){
            // Get eta bounderies:
            float etaMin(etaDepBounderies[etaDepIdx]),
                  etaMax(etaDepBounderies[etaDepIdx+1]);

            // Fill collection:
            if (etaDep) {
              if (etDep) {
                discrCol[segDepIdx][etDepIdx][etaDepIdx] = createEtaEtDepNN(
                    segType, segDepIdx,
                    etaMin,
                    etaMax,
                    etMin,
                    etMax);
              } else {
                discrCol[segDepIdx][etDepIdx][etaDepIdx] = createEtaDepNN(
                    segType, segDepIdx,
                    etaMin,
                    etaMax);
            }
            } else {
              if (etDep) {
                discrCol[segDepIdx][etDepIdx][etaDepIdx] = createEtDepNN(
                    segType, segDepIdx,
                    etMin,
                    etMax);
              } else {
                discrCol[segDepIdx][etDepIdx][etaDepIdx] = createNN(segType, segDepIdx);
              }
            }
          }
        }
      } // Finished looping and filling collection

      // Attempt to create a wrapper with filled collection:
      try {

        msg << lvl << "Attempting to create wrapper with ("
          << toStr(etaDep) << "," << toStr(etDep) << ") discriminator"
          << endmsg;

        wrapper_t* wrapper(nullptr);
        // Create wrapper:
        if ( !ppWrapperCol || ppWrapperCol->empty() ){
          wrapper = new wrapper_t(discrCol);
        } else {
          wrapper = new wrapper_t(*ppWrapperCol, discrCol);
        }
        // Use dummy message stream:
        wrapper->setMsgStream(&msg);
        // Print wrapper:
        wrapper->print(MSG::DEBUG);

        // Delete file if it already exists:
        IOHelperFcns::deleteFile( fileName );

        // Create IDiscrWrapperCollection and write it:
        //writeWrapperOnFile(*wrapper, fileName, MSG::VERBOSE);
        IDiscrWrapperCollection discrWrapperCol = {wrapper};
        IDiscrWrapper::writeCol(discrWrapperCol, fileName);

        // Add file io information:
        AsgElectronRingerSelector::IOConfStruct ioConf;
        ioConf.useBLOutliers = true;
        AsgElectronRingerSelector::writeConf( fileName, ioConf );
        AsgElectronRingerSelector::printConf(ioConf, &msg );

        // Release wrapper memory:
        //wrapper->releaseMemory();
        //delete wrapper;

        msg << lvl << "Succeed!" << endmsg;

      } catch (const std::runtime_error &e) {

        msg << MSG::VERBOSE << "Couldn't create wrapper. Reason: "
          << e.what() << endmsg;

        // Release collection memory
        for ( unsigned segDepIdx = 0; segDepIdx < numberOfSegments(segType); ++segDepIdx ){
          for ( unsigned etDepIdx = 0; etDepIdx < etDepProc; ++etDepIdx){
            for ( unsigned etaDepIdx = 0; etaDepIdx < etaDepProc; ++etaDepIdx){
              delete discrCol[segDepIdx][etDepIdx][etaDepIdx];
              discrCol[segDepIdx][etDepIdx][etaDepIdx] = nullptr;
            }
          }
        }
      } // Finished creating wrapper and handling possible errors

    }
  } // Finished looping over eta/et.
}

// =============================================================================
template<bool goodStatus>
Discrimination::NNFeedForwardVarDep* createNN(SegmentationType cSegType,
    unsigned segIdx)
{
  if ( goodStatus ){
    switch (cSegType)
    {
      case SegmentationType::NoSegmentation:
      {
        std::vector<unsigned int> nodes = {100,16,1};
          std::vector<float> weights = {0.028688,-0.062593,-0.110670,-0.109672,-0.224397,-0.113062,0.039054,-0.529236,-2.888686,-0.605837,-0.994450,0.006954,-0.056765,-0.031750,-0.503685,-0.494298,-0.221056,0.400392,-0.531358,-0.629313,0.089911,-1.007010,-0.629093,-0.716350,-0.273729,-0.826810,-0.214472,-0.975512,-0.396733,-0.003727,-0.276535,-0.056521,0.186771,-0.077535,-0.058493,-0.245285,-0.982163,-0.797398,-0.073127,-0.115995,-0.007802,-1.154354,-0.132477,-0.594071,0.124951,-0.115163,-0.164945,-0.095116,-0.324021,-0.066871,-0.051343,-0.719736,-0.064108,0.144933,-0.404720,-0.052662,-0.203778,-0.495595,0.025789,-0.115119,-0.153406,-1.088269,-1.284559,0.007588,-0.376025,-0.218487,-0.014306,0.037344,-0.159262,0.033259,-0.300734,-0.328634,-1.224337,0.318360,-0.545602,0.263710,-0.034113,-0.121256,-0.271094,-0.406514,0.282135,0.078523,0.364275,-0.856901,-0.442518,-0.008571,-0.030869,0.089742,-1.372179,0.552057,-0.457882,-0.200123,-1.674263,-0.621302,-0.498098,-0.299907,-1.576183,0.046803,-0.091531,-0.096722,1.568295,0.231312,-0.640472,-3.136615,-2.410643,-13.031759,33.412286,-8.683243,1.475885,0.969468,0.017705,0.295497,0.938570,-0.365949,-1.487067,-1.581031,-7.219507,-5.611233,-2.667463,-3.709057,-0.538870,-0.711813,0.433348,-0.594230,-0.844310,-1.288160,-0.741892,-1.830146,-0.325822,-1.631410,-1.751475,-1.720901,-0.898951,-13.288799,-1.933127,-3.081192,-6.221801,-1.104714,-3.841797,-3.328604,-5.193892,-0.808612,-0.796501,-0.288802,7.600919,-0.329103,-2.054659,-0.080070,-7.080676,-0.922352,-4.327209,-3.596929,-1.918411,-3.564654,3.974327,-2.244009,-4.969694,-8.027518,-6.920259,-8.452600,-2.703256,-6.863256,-3.882042,-6.151130,-26.816965,-94.151225,-14.878026,-9.943644,-37.491924,-46.556123,-1.433659,-2.125969,0.698555,1.188699,-1.000490,-0.471744,-0.684885,-4.686223,11.678164,-9.628196,12.210666,-2.342332,-1.902629,-1.247211,-1.661623,-1.388010,-1.868174,-1.932632,-1.209638,2.603166,0.492622,1.332110,-1.645321,1.261750,-4.505736,0.740274,-0.668193,-0.267155,0.334967,3.742837,0.740339,-0.073527,-0.324355,-0.064818,-0.051392,0.094401,0.154956,-0.067873,0.440951,0.138248,-0.572323,-0.010880,-0.047062,-0.180201,-0.479383,-0.188006,-0.424571,-0.457951,-0.638549,-0.298774,-0.708894,-0.359068,-0.192183,-1.064735,-0.140763,-0.518375,0.131144,-0.282072,-0.277306,-0.393560,0.307943,-0.087220,-0.106205,-0.135257,-0.082795,-0.240611,-0.450951,-0.439167,-0.101002,-0.123107,-0.063369,-0.086694,-0.493826,0.086611,0.178572,-0.215764,-0.120014,0.351601,-0.273922,0.485342,-0.132585,0.111573,-0.076256,0.067623,-0.109474,0.206897,-0.093580,-0.016174,0.008848,-0.061735,0.014456,-0.013979,-0.631338,0.191005,-0.191373,-0.267904,0.389943,0.057172,-0.028360,-0.138402,-0.162590,-0.098487,-0.271238,-0.218225,0.084942,-0.490171,-0.076231,-0.104584,-0.075355,-0.206857,1.956858,1.263630,-1.606997,0.258611,-0.221201,-0.126122,0.055639,-0.052784,0.267501,-0.976859,0.077316,-0.236870,-1.100098,-0.340871,0.266472,-0.033575,-1.106904,-0.048352,-0.034147,-0.027628,-0.907428,-0.114443,0.639170,-5.046371,2.117636,12.534221,4.109699,1.002582,-6.121682,-1.300009,0.000709,0.001165,-0.206458,0.354377,-3.450948,3.458475,17.388021,19.380078,5.190390,4.633401,9.791459,1.829216,29.071445,2.496838,3.528770,5.186754,1.097003,1.898567,1.687048,1.712853,18.696546,2.422834,2.972473,1.986865,1.978833,2.538006,8.025286,10.573317,16.791273,3.481498,3.077972,1.144574,0.829633,5.784941,3.109096,7.093387,53.518546,45.052680,9.139784,13.222146,2.750695,3.407295,6.188783,3.406966,-23.018505,2.266498,5.030206,8.614132,5.835313,5.992136,7.205853,6.317150,0.141573,7.108026,-93.605318,-111.675271,2.469222,2.625255,1.189963,0.092941,1.280597,0.471347,-0.712683,-0.367087,0.920240,-0.766278,0.996182,10.240765,8.472205,8.538664,0.794154,3.866253,2.359055,2.030608,1.149638,2.585729,2.002529,1.638214,1.052308,-1.465720,-1.196036,-3.249994,1.560568,-20.990768,0.155179,-1.015941,0.794045,-0.179825,-19.952022,1.910052,1.051576,0.060563,0.031857,0.070955,0.007167,0.378318,0.023833,0.649955,4.275588,0.027542,0.627095,-0.024055,0.048692,0.653087,-0.136869,0.853252,0.711888,0.746423,0.917728,1.660293,0.943031,0.591943,-0.978663,1.058206,0.267564,-0.642447,0.024186,0.398969,-0.004766,0.372238,0.077377,-0.042264,0.054782,0.081330,0.037063,0.236487,0.187889,-0.077143,-0.182055,-0.012978,-0.179301,-0.157928,0.005376,-0.020677,5.841787,0.412119,0.708508,0.070569,-0.645499,-0.112014,0.125128,0.024352,0.703745,0.062582,0.280714,0.045282,0.155499,0.189150,0.299881,0.054184,-0.049919,0.111692,0.026760,0.135167,0.270543,0.148473,-0.015246,-0.100352,0.165556,0.170405,0.228728,0.319828,0.835729,-0.198271,0.439356,-0.002287,0.066184,0.327133,0.233683,0.278432,-3.083528,-1.286595,1.464285,0.698704,0.024064,0.079657,0.007752,0.066683,0.711628,-0.769123,0.203221,0.116553,0.573138,0.035675,-0.406918,0.183639,-1.605187,-0.058066,0.070112,0.147746,-2.392498,-2.745882,1.263215,0.181240,0.343909,-3.675908,4.112136,0.180781,-4.340636,-2.242731,-2.255890,-2.695255,-1.605709,0.047641,-0.994458,5.288881,3.168465,4.077442,1.480216,1.922621,-1.683723,1.113707,0.608307,1.749436,1.209098,3.086876,0.551001,-1.049812,0.239036,-1.736003,-1.371147,29.087469,0.396884,1.155119,4.757856,-0.492357,-2.349446,0.096400,-3.124810,0.448417,0.128918,0.592998,0.331566,0.300982,0.001730,0.269922,-3.360424,-15.594998,-0.537339,9.110059,-2.362906,-1.938478,-1.143943,-0.152321,-2.628727,-0.891215,-1.643069,-4.414059,-10.897783,-6.394373,0.097377,-11.008657,-2.151860,-11.556536,-16.268739,-31.602494,-11.505709,-1.876816,-6.329613,1.669843,0.571898,-2.337885,-1.365757,-0.944771,0.970493,-0.442543,0.995064,-3.749592,-5.691184,-4.139481,-0.788088,4.890539,2.398736,2.532407,1.881680,1.500637,0.623273,-30.264643,9.645718,10.514421,8.475971,0.121070,2.874658,4.640818,1.437012,1.848824,-2.715429,-0.324435,0.844252,6.818916,2.389538,0.107453,0.263178,4.235293,0.609985,1.110795,0.093694,-1.311855,2.269984,5.058177,1.137608,1.277659,-0.033944,-0.164212,-2.001901,-7.228751,-1.036251,1.976840,-2.504326,4.353166,-1.511555,-1.452743,-0.663433,-0.976442,-0.693744,0.198484,-0.407872,-0.506523,-0.590362,4.043677,6.314065,0.126364,-0.194009,-16.236056,4.068819,1.224119,1.543314,-0.128289,1.302668,1.465868,1.603645,-0.031988,0.175951,0.158367,0.120493,-0.004462,7.682568,0.469458,-0.000680,-7.551085,0.606771,0.200480,1.057136,0.555353,4.748190,0.039710,0.263192,0.593023,3.582425,1.947296,0.517452,-0.076818,0.682754,0.357066,0.075757,-0.007521,0.317154,1.510008,0.580695,5.009438,0.728683,-0.267382,2.371758,0.446246,-0.167985,-7.106345,-0.097237,3.584461,4.274035,4.095569,-1.738810,-6.914329,-3.810478,1.208624,1.140845,2.095668,0.575353,0.626909,-2.226223,-2.056425,-1.312785,-1.551202,-2.271757,-0.504885,-0.277969,-0.431348,2.347458,-0.027946,-0.392081,-0.388553,8.689883,4.189324,-0.862448,-5.671942,-1.845137,9.754921,-10.775090,72.349025,6.208780,6.869102,0.123899,0.383385,0.675272,-0.332477,-0.960122,-31.058836,-7.949740,-3.758648,7.108799,11.528299,8.149853,-0.615473,-0.439979,-0.677038,-0.723783,29.196677,-0.859601,-1.499570,-0.576167,-1.667575,-1.829887,-1.398700,-0.515988,41.517160,0.713953,30.545081,-6.188533,-7.091922,-9.414879,-1.904592,-7.658521,-2.284183,-7.772925,21.863106,20.947830,-1.751241,13.477022,-7.682248,-8.168043,-20.637880,10.466483,0.089507,-5.340472,-3.344616,-8.410060,-2.282409,-6.121732,-6.918085,-6.073172,-11.025504,-8.407571,-6.208835,-4.529416,-5.678833,-5.714158,-1.018523,19.611186,-2.653425,-1.897749,-3.690523,0.166875,-0.394813,0.722076,-0.189588,-0.930066,0.943887,-0.845358,-5.028208,-4.240671,-15.165471,-0.622049,-3.294203,1.575702,3.755827,-1.110502,-1.434140,-1.507328,-12.166484,2.660743,2.035036,0.891480,2.444064,-2.451179,-1.473334,0.802695,-0.310561,0.390210,0.371984,-15.530832,1.328315,-6.511445,-1.310474,1.158307,20.504969,0.870710,1.396723,0.227466,-1.831463,-2.830913,-5.143690,-0.271427,-0.426230,-0.240882,-0.115466,0.874429,1.650408,4.967956,1.912887,2.400235,1.008770,2.862430,1.166314,0.933473,2.330710,2.444949,1.830853,1.049244,2.607336,2.804982,3.107458,3.294002,3.170131,13.693583,76.926670,3.845448,23.346861,9.812383,2.391273,10.606641,1.963476,2.967598,1.526888,0.866287,4.209006,18.102486,0.533338,1.445390,0.872831,0.853834,0.273607,1.321337,0.123892,0.412314,0.355130,-4.645920,0.283349,-6.881706,0.901227,-4.195997,0.916571,-0.388821,0.565599,1.117080,0.812389,5.323900,1.317251,0.320280,0.308174,-26.829184,1.149731,1.275011,2.000144,-1.470072,-0.767747,0.202361,-1.587088,1.155752,1.098314,1.257929,-3.200987,0.668693,4.033012,0.999027,-1.208894,1.632769,1.014471,0.723843,2.399647,-2.593154,-9.382237,1.349194,-0.021662,-3.713423,-1.746662,-6.558139,-1.554877,-2.434262,1.989675,0.249551,-2.443816,-3.779906,-2.636414,-1.277373,-4.628381,-0.505515,-3.990376,-11.436412,-2.227947,-2.823871,-6.863975,-1.281835,-3.663759,-0.310102,0.149827,2.020108,2.642968,1.497769,3.041259,2.015148,1.067337,-0.013860,0.981229,0.631989,1.125228,0.752725,-1.872923,0.565742,0.566382,0.054049,0.409526,-0.925388,-0.245978,-0.132565,-0.171097,-3.199698,2.047109,-2.335864,-0.230441,-3.463833,-2.292295,-0.984754,-0.119444,-0.079340,-1.964507,-0.163262,-0.044391,-3.299204,-1.317512,-1.653288,-360.611655,-0.792871,-1.526987,-5.112393,-0.545486,-8.757870,-0.044763,0.033397,-1.166951,-3.586482,-3.397790,-0.226581,-0.680198,-0.470130,-0.421405,-3.563391,-0.720706,0.001545,-2.030093,-3.074116,-0.915965,-2.567866,0.300819,-2.735884,-2.183676,4.409152,0.644427,-0.628248,-3.267413,-3.548809,-4.949955,3.555070,8.891552,3.580243,1.056337,-1.324928,-1.612563,-0.576405,-0.681977,2.571778,1.858078,1.474100,0.776536,3.534101,0.802548,-0.213475,0.434609,-4.311330,-1.056947,0.292802,-0.536683,-9.690235,-3.111939,-0.113073,5.260060,0.512237,1.040290,34.479682,2.148753,-3.126262,-2.802152,-0.047654,-0.012042,1.508002,5.959009,1.083541,1.424925,-1.638421,-3.557558,-1.322984,4.979366,2.018343,1.150012,1.216431,-0.280425,2.333079,1.243301,0.841088,1.886149,1.498563,1.629537,3.290616,1.685732,2.013451,0.862097,1.742377,-25.609594,1.644792,4.482274,-18.463892,1.851310,0.167115,-0.404057,0.565838,4.045443,1.878586,0.753386,-16.788055,0.126791,-10.594349,3.300475,-0.634618,0.747863,7.905016,3.666997,31.245129,3.526583,9.021474,8.262771,10.631880,-3.141672,4.410128,6.165324,12.481022,5.635590,8.770696,8.735834,10.870203,3.874316,21.608977,4.526612,1.156461,1.276455,-0.976077,-0.288614,1.000854,0.298251,0.383862,0.127455,-10.489012,-1.725379,-7.637858,9.150683,2.160111,1.407201,1.326115,1.018763,2.385681,1.744191,1.730315,7.003605,-10.912519,4.530976,3.405832,13.760234,-1.191210,-1.174960,0.549645,0.459820,3.840778,12.674068,-2.126857,-3.227273,0.883224,5.710771,2.435913,-63.068527,3.723472,10.381427,-6.550155,-5.879445,-0.212391,-0.055838,-0.030079,0.349785,0.068710,3.085991,5.712933,5.268697,4.278224,2.301436,0.565240,0.997076,0.590336,0.721163,0.832828,1.161940,0.703974,1.480778,0.505237,1.669236,1.958621,1.691473,0.893302,19.534316,1.782585,1.056738,-3.575074,16.543404,26.596598,24.057324,6.166616,11.768488,2.438942,0.139397,2.026144,3.923045,8.086570,7.847441,5.052775,3.426013,3.642981,2.268567,-0.649373,3.161143,7.047121,2.205177,5.594592,-61.189816,8.415619,8.300800,-2.673700,-2.996654,5.218760,-23.693685,-2.322776,8.623765,4.627958,3.368395,46.500027,5.639402,-8.165272,0.796185,-0.706099,0.135805,0.868030,0.306016,0.789311,7.124334,4.242289,9.727023,0.681696,3.756994,3.622855,1.035487,1.025518,1.451887,1.854145,53.213359,-1.431522,-0.091550,-1.656084,-29.300245,1.570393,-0.978860,-1.027790,0.540321,-0.337222,-0.445398,5.875282,-1.581966,-2.580338,-0.264537,1.197146,-1.569363,-1.206515,-4.797875,-4.215951,8.528007,-6.191259,-2.189876,-2.501495,-0.966387,-1.200414,-1.520622,1.784751,3.031218,2.192351,1.142476,3.948292,1.763238,1.458057,1.396658,0.608675,1.724458,1.444704,-0.337446,0.570233,-0.657234,0.994609,-1.613324,-1.018631,9.949645,0.400291,0.977540,2.793460,9.773625,-2.396520,-2.783999,-2.708673,0.222687,-0.809978,0.368899,0.299932,-0.449218,0.185562,-0.459102,19.015891,-5.459218,-5.169562,-2.773216,-2.557959,-1.678736,-1.842239,-2.521119,-2.590653,-1.349186,-6.084066,13.539631,-2.590764,-6.882887,-4.029900,-1.488305,-4.217717,-0.591442,-4.848400,-2.261591,-1.905966,0.042691,-46.258913,-34.478861,-0.503401,-0.420428,-2.167570,-0.846137,0.870438,-2.407570,0.566669,-4.854715,12.200811,-2.699115,2.800773,7.892013,12.472750,0.180263,1.710783,0.879254,-2.253537,-2.573041,23.346808,6.059341,2.580303,14.766641,5.840801,4.124855,2.460706,1.147568,-1.740196,-0.196835,0.446965,-4.702591,1.964575,-0.052577,0.050285,-0.335195,0.303068,-0.149456,0.085326,-0.495100,1.934900,1.936413,1.653900,0.018581,-0.061852,-0.204984,-1.333097,-1.018476,-0.999683,-1.024398,-2.082589,-1.729892,-1.682701,-1.436921,-0.425914,0.474230,-0.613048,2.100885,-0.575348,-0.652350,0.685484,0.915968,0.304119,33.087667,22.601027,-0.104921,-0.177293,0.234389,0.198040,-0.238190,0.024597,-0.234056,2.822241,0.004142,-0.021054,-1.552198,0.397562,0.077244,-0.810139,0.196844,0.939802,-0.136742,0.592149,-0.255330,0.163302,0.318883,-0.064531,0.062070,0.694546,0.209702,-0.010243,-0.073304,0.410424,0.204043,0.361079,-0.055767,-0.101269,-0.050839,0.051487,0.718138,0.694693,0.435695,-0.256583,1.158518,1.995850,0.439841,-6.203604,-0.985698,-0.155184,0.593911,2.482176,0.946247,-8.436521,2.401151,1.810159,-1.035090,1.092773,0.036518,0.601562,0.147424,-1.833577,-0.126662,-0.374717,0.021811,-1.851743,2.796177,7.877599,0.255243,1.929069,0.112273,1.512485,-0.092995,2.856183,2.717226,-2.437687,29.901422,-2.085927,-19.040682,7.366650,-6.152353,2.929612,2.416426,0.020353,0.695380,-1.096458,-0.048956,-4.537238,-1.349666,-1.191487,-4.125253,-1.868902,-0.829698,0.339131,-0.988083,-0.657337,-1.539595,-0.878505,0.020183,-0.606991,-6.575499,-1.118187,-5.901197,-1.902425,-1.830654,-0.680334,-5.982117,-0.672636,-0.566174,0.880422,-0.227259,16.390507,16.269985,0.259872,-0.607438,-0.343732,-0.055939,-0.167441,-0.216072,-0.726638,-1.074767,-0.762545,0.036361,-1.061878,-0.472379,0.016452,-0.140865,10.090679,-0.235577,-0.524869,0.212008,-0.585822,7.323222,-0.135613,0.180593,-0.011106,-0.042434,-12.922468,-1.085171,-0.292372,-0.325988,-1.179218,-1.970896,-16.414723,0.919156,1.402566,2.696173,-0.709757,1.263208,-4.311061,-1.912170,0.871238,3.954405,-0.151088,-1.171526,-4.635838,-1.823951,-1.739963,-1.850814,-0.681339,-4.348577,6.466663,-1.359048,-2.297508,0.873118,-2.400759,-1.233752,-11.830300,-4.348966,0.058590,-0.093309,-0.269750,-0.034210,2.940893,0.216562,-0.620224,-5.364289,2.643353,-14.314586,0.580299,-7.502483,3.299922,3.749239,0.036450,2.614238,2.437889,-0.351271,-1.562823,-1.413144,-5.498119,-3.459754,0.428530,-0.058354,-0.386101,-0.304795,-0.544952,-1.530170,-0.663988,-0.703933,-0.727071,-1.581068,-0.311638,-1.667302,-1.823642,-1.680038,1.580470,177.705612,-1.861632,6.898606,1.315608,-1.085525,-5.474480,-1.913068,-1.113184,-0.820879,-0.721986,-0.105046,0.573232,-0.567043,-0.447164,-1.654838,-6.803239,-0.794338,-4.504619,-2.908429,-4.093875,-3.466304,-3.426859,-2.315266,-5.298508,-10.366847,-17.253376,-8.906028,-8.385869,-6.196990,-2.296948,-5.304315,-18.097126,-25.213401,1.869964,-2.902289,-1.899073,-1.705180,-1.426860,-2.510971,0.838146,0.526502,-0.782129,0.808493,-0.845322,-12.005751,-18.375311,-9.925110,-1.189432,-1.891818,-5.313697,-1.043915,-1.121079,-1.301175,-2.109128,-2.104007,2.245467,0.933534,-3.669234,0.044000,-3.064914,-3.341542,-2.857548,-0.249489,-0.592423,-0.582685,0.247260,7.506503,1.967544,0.566159,0.832930,-0.418475,-0.390811,-0.564865,0.433205,0.504496,-0.647795,-0.236603,-0.652715,-0.487510,-0.544541,0.332621,0.466587,0.523348};
        std::vector<float> bias = {-350.736300,1.530207,-9.805541,-1.676745,6.448848,-0.185940,-0.834116,1.642355,-0.557261,0.211599,-1.207916,-1.731354,-0.243328,-0.166474,0.338915,1.356859,-0.370182};
        return new Discrimination::NNFeedForwardVarDep(nodes,weights,bias);
      }
      default:
      {
        std::vector<unsigned int> nodes = {segmentationEntries[cSegType][segIdx],1};
        std::vector<float> weights(segmentationEntries[cSegType][segIdx],.01);
        std::vector<float> bias={0};
        return new Discrimination::NNFeedForwardVarDep(nodes, weights, bias);
      }
    }
  } else {
    std::vector<unsigned int> nodes = {100,1};
    std::vector<float> weights(100,1);
    std::vector<float> bias={0};
    return new Discrimination::NNFeedForwardVarDep(nodes, weights, bias);
  }
}

// =============================================================================
template<bool goodStatus>
Discrimination::NNFeedForwardVarDep* createEtaDepNN(
    SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax)
{
  Discrimination::NNFeedForwardVarDep* nn = createNN(cSegType, segIdx);
  nn->setEtaDep(etaMin, etaMax);
  return nn;
}

// =============================================================================
template<bool goodStatus>
Discrimination::NNFeedForwardVarDep* createEtDepNN(
    SegmentationType cSegType,
    unsigned segIdx,
    float etMin, float etMax)
{
  Discrimination::NNFeedForwardVarDep* nn = createNN(cSegType, segIdx);
  nn->setEtDep(etMin, etMax);
  return nn;
}

// =============================================================================
template<bool goodStatus>
Discrimination::NNFeedForwardVarDep* createEtaEtDepNN(SegmentationType cSegType,
    unsigned segIdx,
    float etaMin, float etaMax,
    float etMin, float etMax)
{
  Discrimination::NNFeedForwardVarDep* nn = createNN(cSegType, segIdx);
  nn->setEtaEtDep(etaMin, etaMax, etMin, etMax);
  return nn;
}

// =============================================================================
template<
  typename procedure_t,
  EtaDependency etaDependency,
  EtDependency etDependency
>
void createThresWrapper(const char* fileName,
    const std::vector<float> &etaDepBounderies,
    const std::vector<float> &etDepBounderies,
    float thresValue)
{
  // Typedef to make things easier
  typedef RingerProcedureWrapper<procedure_t,
          etaDependency,
          etDependency> wrapper_t;

  typedef typename RingerProcedureType<procedure_t>::procEnum_t procEnum_t;

  msg << MSG::INFO << "Attempting to create a RingerProcedureWrapper<"
        << toStr(procedure_t::template procType<procEnum_t>() )
        << ((Ringer::is_same<procedure_t,Discrimination::IThresholdVarDep>::value)?",":"(VarDep),")
        << toStr(etaDependency) << ","
        << toStr(etDependency) << "> with file name: " << fileName << "." << endmsg;

  unsigned etaDepProc = etaDepBounderies.size() - 1;
  unsigned etDepProc = etDepBounderies.size() - 1;

  // Loop over variable dependencies
  for ( unsigned uEtaDep = EtaIndependent; uEtaDep <= EtaDependent; ++uEtaDep ){
    for ( unsigned uEtDep = EtIndependent; uEtDep <= EtDependent; ++uEtDep ){
      // The enumerations
      EtaDependency etaDep = static_cast<EtaDependency>(uEtaDep);
      EtDependency etDep = static_cast<EtDependency>(uEtDep);

      // Change message level accondingly if we are creating correct wrapper
      // type
      MSG::Level lvl(MSG::VERBOSE);
      if ( etaDep == etaDependency &&
          etDep == etDependency )
      {
        lvl = MSG::INFO;
      }

      // The collection holder
      typename wrapper_t::ThresDepProcCollection thresCol( etDepProc,
          std::vector<procedure_t*>(etaDepProc, nullptr));

      // Fill collection
      for ( unsigned etDepIdx = 0; etDepIdx < etDepProc; ++etDepIdx){
        // Get et bounderies:
        float etMin(etDepBounderies[etDepIdx]),
              etMax(etDepBounderies[etDepIdx+1]);
        for ( unsigned etaDepIdx = 0; etaDepIdx < etaDepProc; ++etaDepIdx){
          // Get eta bounderies:
          float etaMin(etaDepBounderies[etaDepIdx]),
                etaMax(etaDepBounderies[etaDepIdx+1]);

          // Fill collection:
          if (etaDep) {
            if (etDep) {
              thresCol[etDepIdx][etaDepIdx] = createEtaEtDepThres(
                  etaMin,
                  etaMax,
                  etMin,
                  etMax,
                  thresValue);
            } else {
              thresCol[etDepIdx][etaDepIdx] = createEtaDepThres(
                  etaMin,
                  etaMax,
                  thresValue);
          }
          } else {
            if (etDep) {
              thresCol[etDepIdx][etaDepIdx] = createEtDepThres(
                  etMin,
                  etMax,
                  thresValue);
            } else {
              thresCol[etDepIdx][etaDepIdx] = createThres(thresValue);
            }
          }
        }
      } // Finished looping and filling collection

      // Attempt to create a wrapper with filled collection:
      try {

        msg << lvl << "Attempting to create wrapper with ("
          << toStr(etaDep) << "," << toStr(etDep) << ") discriminator"
          << endmsg;

        // Create wrapper:
        auto wrapper = new wrapper_t(thresCol);

        // Use dummy message stream:
        wrapper->setMsgStream(&msg);
        // Print wrapper:
        wrapper->print(MSG::DEBUG);

        // Delete file if it already exists:
        IOHelperFcns::deleteFile( fileName );

        // If it succeed, write it into file:
        //writeWrapperOnFile(*wrapper, fileName, MSG::VERBOSE);
        IThresWrapper::writeWrapper(wrapper, fileName);

        // Release wrapper memory:
        //wrapper->releaseMemory();
        //delete wrapper;

        msg << lvl << "Succeed!" << endmsg;

      } catch (const std::runtime_error &e) {

        msg << MSG::VERBOSE << "Couldn't create wrapper. Reason: "
          << e.what() << endmsg;

        // Release collection memory
        for ( unsigned etDepIdx = 0; etDepIdx < etDepProc; ++etDepIdx){
          for ( unsigned etaDepIdx = 0; etaDepIdx < etaDepProc; ++etaDepIdx){
            delete thresCol[etDepIdx][etaDepIdx];
            thresCol[etDepIdx][etaDepIdx] = nullptr;
          }
        }
      } // Finished creating wrapper and handling possible errors

    }
  } // Finished looping over eta/et.
}


// =============================================================================
Discrimination::UniqueThresholdVarDep* createThres(float thresValue)
{
  return new Discrimination::UniqueThresholdVarDep(thresValue);
}

// =============================================================================
Discrimination::UniqueThresholdVarDep* createEtaDepThres(
    float etaMin, float etaMax,
    float thresValue)
{
  Discrimination::UniqueThresholdVarDep* thres = createThres(thresValue);
  thres->setEtaDep(etaMin, etaMax);
  return thres;
}

// =============================================================================
Discrimination::UniqueThresholdVarDep* createEtDepThres(
    float etMin, float etMax,
    float thresValue)
{
  Discrimination::UniqueThresholdVarDep* thres = createThres(thresValue);
  thres->setEtDep(etMin, etMax);
  return thres;
}

// =============================================================================
Discrimination::UniqueThresholdVarDep* createEtaEtDepThres(
    float etaMin, float etaMax,
    float etMin, float etMax,
    float thresValue)
{
  Discrimination::UniqueThresholdVarDep* thres = createThres(thresValue);
  thres->setEtaEtDep(etaMin, etaMax, etMin, etMax);
  return thres;
}
