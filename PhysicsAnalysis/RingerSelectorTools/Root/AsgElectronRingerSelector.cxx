/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AsgElectronRingerSelector.cxx 713528 2015-12-09 08:58:44Z wsfreund $
// STL includes:
#include <string>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <iostream>

#ifndef RINGER_STANDALONE
// Athena framework includes:
# include "AsgTools/AsgTool.h"
# include "PATCore/TAccept.h"
# include "PathResolver/PathResolver.h"
# include "AthContainers/exceptions.h"

// xAOD includes:
# include "xAODBase/IParticle.h"
# include "xAODCaloRings/CaloRingsContainer.h"
# include "xAODCaloRings/CaloRingsAuxContainer.h"
# include "xAODCaloRings/RingSetConfContainer.h"
# include "xAODCaloRings/RingSetConfAuxContainer.h" // always include aux container
# include "xAODCaloRings/CaloRingsDefs.h"
# include "xAODCaloRings/tools/getCaloRingsDecorator.h"
# include "xAODCaloRings/RingSetConf.h"
# include "xAODTracking/TrackParticle.h"
# include "xAODEgamma/Electron.h"
#endif // ndef RINGER_STANDALONE

// Local includes:
#include "RingerSelectorTools/AsgElectronRingerSelector.h"
#include "RingerSelectorTools/ElectronTAccept.h"
#include "RingerSelectorTools/tools/IOHelperFcns.h"
#include "RingerSelectorTools/tools/TrackPatternsHolder.h"
#include "RingerSelectorTools/tools/IRedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/RingerUseNewCppFeatures.h"
#include "RingerSelectorTools/tools/cxx/errorcheck.h"

// Root framework includes:
#include <TFile.h>

/*
 * TODO List:
 * 
 * - We need to change getTAccept logic so that it works exactly for egamma
 *   particle, and then checks if particle is electron. If true, gets track
 *   information and feed it into the RingerCommonSelector.
 */

namespace Ringer {

#ifndef RINGER_STANDALONE

namespace {

//==============================================================================
/**
 * @brief Get cluster 2EM layer eta and et variables
 *
 * NOTE: It isn't being used for now.
 **/
//inline
//void getCluster2EMEtaEt(
//    const xAOD::CaloCluster *cluster, 
//    float &eta2, 
//    float &et)
//{
//  eta2 = std::fabs(cluster->etaBE(2));
//  // We need to hold the dependence variable within this structure:
//  const double energy =  cluster->e();
//  if ( eta2 < 999.) {
//    const double cosheta = std::cosh(eta2);
//    et = (cosheta != 0.) ? energy /cosheta : 0.;
//  }
//}

//==============================================================================
/**
 * @brief Check if decision bit is to be used
 **/
//inline
//bool isDecToBeUsed(
//    const int bitIntHolder, 
//    const Ringer::ElectronTAccept::bitMskWord &decsToUse)
//{
//  return decsToUse.test(bitIntHolder);
//}

} // End of private namespace


//==============================================================================
// Standard constructor
//==============================================================================
AsgElectronRingerSelector::AsgElectronRingerSelector(std::string asgToolName) :
  AsgMetadataTool(asgToolName),
  m_thresWrapper(nullptr),
  m_ringSelCommon(nullptr),
  m_trackPat(nullptr),
  m_partDecMsk(Ringer::ElectronTAccept::retrieveTAcceptTemplate()),
  m_accept(Ringer::ElectronTAccept::retrieveTAcceptTemplate()),
  m_useCutIDDecision(false),
  m_metaDataCached(false),
  m_rsConfCont(nullptr),
  m_metaIsOnOutput(false) // Check first input
{

  declareProperty("DiscriminationFileName", m_discrFileName="", 
      "The configuration file in which we will retrieve the "
      "discrimination chain.");

  declareProperty("ThresholdFileName", m_thresFileName="", 
      "The configuration file in which we will retrieve the "
      "");

  declareProperty("CutsMask", m_cutsMask, 
      "The required cuts to apply (IsEM Mask)");

  declareProperty("CutIDSelector", m_cutIDSelector, 
      "The CutID Tool Handle to run track (if required by discrimation "
      "configuration file)");

  declareProperty("RingSetConfContainerName", m_rsMetaName,
      "The RingSetConfContainer key in the MetaData StoreGate");

  declareProperty("CacheConfData",m_cacheMetaData = true,
      "Whether to cache metadata configuration or not");

  m_ringsELReader = xAOD::getCaloRingsReader();

}

//==============================================================================
// Standard destructor
//==============================================================================
AsgElectronRingerSelector::~AsgElectronRingerSelector()
{
  // Release discriminator wrapper collection memory:
  for ( size_t discrColIdx = 0; 
      discrColIdx < m_discrWrapperCol.size(); 
      ++discrColIdx ) 
  {
    if (m_discrWrapperCol[discrColIdx]) {
      m_discrWrapperCol[discrColIdx]->releaseMemory();
    }
  }

  // Release memory from threshold wrapper:
  if ( m_thresWrapper ){
    m_thresWrapper->releaseMemory();
    delete m_thresWrapper;
  }

  delete m_ringSelCommon;
  delete m_trackPat;
}

//==============================================================================
// finalize method (called by destructor)
//==============================================================================
StatusCode AsgElectronRingerSelector::finalize()
{
  return StatusCode::SUCCESS;
}



//==============================================================================
// Athena initialize method
//==============================================================================
StatusCode AsgElectronRingerSelector::initialize()
{
  // Set the unsigned mask into the bitset
  m_cutsToUse = ElectronTAccept::bitMskWord(m_cutsMask);

  ATH_MSG_DEBUG("Cut mask was set to: " << m_cutsToUse.to_string());

  // Set our configuration:
  CHECK( readConfigFiles() );

  return StatusCode::SUCCESS;
}

//==============================================================================
StatusCode AsgElectronRingerSelector::readConfigFiles()
{

  // Use colaboration tool to find true path for file:
  std::string discrFileName = PathResolverFindCalibFile( m_discrFileName );
  std::string thresFileName = PathResolverFindCalibFile( m_thresFileName );

  if( discrFileName.empty() ) {
    ATH_MSG_ERROR("Discrimination chain configuration file is not "
        "available.");
    return StatusCode::FAILURE;
  }

  if( thresFileName.empty() ) {
    ATH_MSG_ERROR("Threshold configuration file is not "
        "available.");
    return StatusCode::FAILURE;
  }

  // Read discrimination wrapper collection
  IDiscrWrapper::read(m_discrWrapperCol, discrFileName.c_str() );

  ATH_MSG_INFO( "Using DiscriminationFile: " << discrFileName.c_str());

  // Set message stream and print configuration:
  for ( unsigned discrIdx = 0; 
      discrIdx < m_discrWrapperCol.size(); 
      ++discrIdx)
  {
    m_discrWrapperCol[discrIdx]->setMsgStream( &(this->msg()));
    m_discrWrapperCol[discrIdx]->print();
  }

  // Get configuration for discrimination information flow
  IOConfStruct fileConf;
  retrieveFileConf( discrFileName.c_str(), fileConf );
  printConf( fileConf, &(this->msg()) );

  // Read discrimination wrapper collection
  IThresWrapper::read(m_thresWrapper, thresFileName.c_str() );

  ATH_MSG_INFO( "Using ThresholdFile: " << thresFileName.c_str());

  // Set message stream and print configuration:
  m_thresWrapper->setMsgStream( &(this->msg() ));
  m_thresWrapper->print();

  // Create our discrimination chain selector:
  m_ringSelCommon = new Ringer::RingerCommonSelector(
      &m_discrWrapperCol,
      m_thresWrapper,
      &m_partDecMsk,
      fileConf.useTrackPat,
      fileConf.useRawTrackPat,
      fileConf.useCaloCommittee);

  // Set RingerCommonSelector message stream:
  m_ringSelCommon->setMsgStream( &(this->msg()) );

  // Update discrimination wrapper CaloRings configuration
  if ( !m_rawConfCol.empty() ){
    for ( auto &discrWrapper : m_discrWrapperCol ) {
      discrWrapper->setRawConfCol( &m_rawConfCol );
    }
    xAOD::RingSetConf::print( m_rawConfCol, msg() );
  }

  // See if we are using CutID selector and retrieve it if so:
  if (m_useCutIDDecision) {
    if ( m_cutIDSelector.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Could not get tool: " << m_cutIDSelector );
      return StatusCode::FAILURE;
    }
  }

  // Create track pattern and make it redirect its stream as if it
  // was AsgElectronRingerSelector:
  m_trackPat = new Ringer::TrackPatternsHolder( 
      fileConf.useBLOutliers,
      fileConf.usePIXOutliers,
      fileConf.useSCTOutliers,
      fileConf.useTRTOutliers,
      fileConf.useTRTXenonHits);

  m_trackPat->setMsgStream( &(this->msg()) );

  return StatusCode::SUCCESS;

}

//=============================================================================
const Root::TAccept& AsgElectronRingerSelector::accept( 
    const xAOD::IParticle* part ) const
{
  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");

  const xAOD::Electron* el = 0;
  if( part->type()==xAOD::Type::Electron ){
    el = static_cast<const xAOD::Electron*> (part);
  } else {
    ATH_MSG_ERROR("AsgElectronRingerSelector::could not convert argument to accept");
  }

  return accept(el);
}


// =============================================================================
const Root::TAccept& AsgElectronRingerSelector::accept( 
    const xAOD::Electron* el) const
{
  ATH_MSG_DEBUG("Entering accept( const xAOD::Electron* el)");

  StatusCode sc = execute(el);

  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error while on particle AsgSelector execution.");
  }

  return m_accept;
}


// =============================================================================
const Root::TAccept& AsgElectronRingerSelector::accept( 
    const xAOD::Egamma* eg ) const
{
  ATH_MSG_DEBUG("Entering accept( const xAOD::Egamma* part )");

  StatusCode sc = execute(eg);

  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error while on particle AsgSelector execution.");
  }

  return m_accept;
}


// =============================================================================
StatusCode AsgElectronRingerSelector::execute(
    const xAOD::Electron* el) const
{

  ATH_MSG_DEBUG("Entering execute(const xAOD::Electron* el...)");

#if RINGER_USE_NEW_CPP_FEATURES
  // In this case, we only do this to have a more harmonic code:
  Root::TAccept &partDecMsk_ref = m_partDecMsk;
  TrackPatternsHolder *trackPat_ref = m_trackPat;
#else
  // Well, since we do not have mutable properties, we need to do this ugly
  // things...
  Root::TAccept &partDecMsk_ref = 
    const_cast<Root::TAccept&>(m_partDecMsk);
  TrackPatternsHolder *trackPat_ref = 
    const_cast<TrackPatternsHolder*>(m_trackPat);
#endif

  // Clear particle decision mask and previous result (set everything as if it
  // was passed), prepare to refill it:
  partDecMsk_ref.clear();

  m_ringSelCommon->clear();

  // No error occurred so far, flag it:
  partDecMsk_ref.setCutResult( 
      BitdefElectron::NoErrorBit, 
      true);

  // Set if it was requested to execute CutID:
  partDecMsk_ref.setCutResult( 
      BitdefElectron::ExecCutID, 
      m_useCutIDDecision );

  if (!el){
    ATH_MSG_ERROR("Invalid electron pointer.");

    partDecMsk_ref.setCutResult( 
        BitdefElectron::NoErrorBit, 
        false);

    return StatusCode::FAILURE;
  }

  // First, check if we can retrieve decoration:
  const xAOD::CaloRingsLinks *caloRingsLinks(nullptr);
  try {
    caloRingsLinks = &(m_ringsELReader->operator()(*el));
  } catch ( const std::exception &e) {
    ATH_MSG_ERROR("Couldn't retrieve CaloRingsLinks. Reason: " 
        << e.what());
  }

  if ( caloRingsLinks->empty() ){

    ATH_MSG_ERROR("Particle does not have CaloRings decoratorion.");

    partDecMsk_ref.setCutResult( 
        BitdefElectron::NoErrorBit, 
        false);

    return StatusCode::FAILURE;
  }

  // FIXME The indexing should be an enum, previous test should ensure that the
  // size is large enought to access index.
  if ( !caloRingsLinks->at(0).isValid() ){
    ATH_MSG_DEBUG("Ignoring candidate with invalid ElementLink.");

    partDecMsk_ref.setCutResult( 
        BitdefElectron::NoErrorBit, 
        false);

    return StatusCode::SUCCESS;
  }


  // For now, we are using only the first cluster 
  const xAOD::CaloRings *clrings = *(caloRingsLinks->at(0));
  
  // Check if everything is ok with CaloRings ElementLink vector:
  if (caloRingsLinks->empty() || !(clrings) ){

    ATH_MSG_ERROR("There isn't CaloRings Decoration available" 
        " for input particle." );

    partDecMsk_ref.setCutResult( 
        BitdefElectron::NoErrorBit, 
        false );

    return StatusCode::FAILURE;
  }

  // Get track information:
  const xAOD::TrackParticle *track = el->trackParticle();

  // Fill track pattern holder with new information:
  trackPat_ref->extractPatternsFrom( track, el );

  // Put cluster calibrated eta/et into DepVarStruct:
  const xAOD::CaloCluster *cluster = el->caloCluster();
  const DepVarStruct depVar( cluster->eta(), 
      cluster->et() );

  // We head now into the RingerChain execution:
  try {
    // Execute Ringer Selector Common:
    if ( m_ringSelCommon->execute(
          depVar, 
          clrings, 
          trackPat_ref).isFailure() )
    {

      ATH_MSG_ERROR("Error while executing "
          "RingerCommonSelector.");

      partDecMsk_ref.setCutResult( 
          BitdefElectron::NoErrorBit, 
          false);

    }
  } catch (const std::exception &e ) {

    ATH_MSG_ERROR("Error while executing "
        "RingerCommonSelector. Reason: " << e.what() );

    partDecMsk_ref.setCutResult( 
        BitdefElectron::NoErrorBit, 
        false);

  }
  
  // Add the CutID track decision bit (if requested):
  if ( m_useCutIDDecision ) {
    try {
      partDecMsk_ref.setCutResult( 
          BitdefElectron::CutIDDec, 
          static_cast<bool>(m_cutIDSelector->accept(el)) );

    } catch ( const std::exception &e ) {

      ATH_MSG_ERROR("Error while executing "
          "AsgElectronRingerSelector. Reason:" << e.what() );

      partDecMsk_ref.setCutResult( 
          BitdefElectron::NoErrorBit, 
          false );

      partDecMsk_ref.setCutResult( 
          BitdefElectron::CutIDDec , 
          false );

    }
  } else { 

    // If we do not run it, set track decision to true:
    partDecMsk_ref.setCutResult( 
        BitdefElectron::CutIDDec , 
        true );

  }

  // We have finished, then fill decision mask:
  fillTAccept();

  // Check if an error occurred, and flag it:
  if ( partDecMsk_ref.getCutResult( 
        BitdefElectron::NoErrorBit ) )
  {
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}

//==============================================================================
StatusCode AsgElectronRingerSelector::execute(const xAOD::Egamma* eg) const
{
  ATH_MSG_DEBUG("entering execute(const xAOD::Egamma* eg...)");

  if (eg){
    //size_t cDiscr = 0;
  } else {
    ATH_MSG_ERROR("Egamma pointer is null.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Fill the m_accept from the m_partDecMsk mask using the m_cutsToUse
//=============================================================================
void AsgElectronRingerSelector::fillTAccept() const 
{
#if RINGER_USE_NEW_CPP_FEATURES
  // In this case, we only do this to have a more harmonic code:
  Root::TAccept &accept_ref = m_accept;
#else
  // Well, since we do not have mutable properties, we need to do this ugly
  // things...
  Root::TAccept &accept_ref = const_cast<Root::TAccept&>(m_accept);
#endif
  for (unsigned bit = 0; bit < BitdefElectron::NUsedBits(); ++bit ){
    // m_partDec mask is set to true if passed cut.
    // m_cutsToUse is set to true if cut is to be used.
    accept_ref.setCutResult( bit, 
        (m_partDecMsk.getCutResult(bit)) || !m_cutsToUse[bit] );
  }
}

//==============================================================================
StatusCode AsgElectronRingerSelector::beginInputFile()
{

  // Tell the user what's happening:
  ATH_MSG_DEBUG( "Entered new file, checking if it is needed to " 
      "update metadata information.");

  if ( m_cacheMetaData && m_metaDataCached ) {
    ATH_MSG_DEBUG( "Metadata is already cached." );
    return StatusCode::SUCCESS;
  }

  // Read the metadata object:
  m_rsConfCont = nullptr;

  bool failedToRetrieveInInput(false);

  // First we try to obtain configuration on the inputMetaStore. If this does
  // not work, we try to obtain it on the outputMetaStore.
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
  if ( !m_metaIsOnOutput && ( ( m_rsConfCont = inputMetaStore()->
        tryConstRetrieve< xAOD::RingSetConfContainer >( m_rsMetaName ) )
      == nullptr ) ) 
  {
    if ( !m_metaIsOnOutput ) {
      failedToRetrieveInInput = true;
      ATH_MSG_DEBUG("Couldn't retrieve on inputMetaStore, will "
          "try to retrieve from outputMetaStore.");
    }
  } else {
    ATH_MSG_DEBUG("Retrieved meta from inputMetaStore.");
  }
#else
  if ( !m_metaIsOnOutput && 
      inputMetaStore()->retrieve( m_rsConfCont, m_rsMetaName ) )
  {
    if ( !m_metaIsOnOutput ) {
      failedToRetrieveInInput = true;
      ATH_MSG_DEBUG("Couldn't retrieve on inputMetaStore, will "
          "try to retrieve from outputMetaStore.");
    }
  } else {
    ATH_MSG_DEBUG("Retrieved meta from inputMetaStore.");
  }
#endif

  // We also attempt to retrieve on outputMetaStore if inputMetaStore failed:
  if ( ( m_metaIsOnOutput || failedToRetrieveInInput ) &&
      outputMetaStore()->retrieve( m_rsConfCont, m_rsMetaName ).isFailure())
  {
    // FIXME: This should be done by manually setting the metadata information
    // if it is not set (if manually set, do not attempt to retrieve on the 
    // metadata store).
    ATH_MSG_WARNING( "Couldn't retrieve rings configuration on both "
        "inputMetaStore and outputMetaStore. Setting default value, "
        "(which might be wrong)." );
    m_rawConfCol.clear();
    m_rawConfCol = { 
        {8,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::PS,
          Ringer::CalJointSection::EM,
          false, false,
          0, 8,
        0, 88},
        {64,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::EM1,
          Ringer::CalJointSection::EM,
          false, false,
          8, 72,
        0, 88},
        {8,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::EM2,
          Ringer::CalJointSection::EM,
          false, false,
          72, 80, 
        0, 88},
        {8,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::EM3,
          Ringer::CalJointSection::EM,
          false, false,
          80, 88, 
        0, 88},
        {4,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::HAD1,
          Ringer::CalJointSection::HAD,
          false, false,
          88, 92, 
        88, 100},
        {4,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::HAD2,
          Ringer::CalJointSection::HAD,
          false, false,
          92, 96, 
        88, 100},
        {4,
          std::vector<CaloSampling::CaloSample>(), 0., 0., 0., 0.,
          Ringer::CalJointLayer::HAD3,
          Ringer::CalJointSection::HAD,
          false, false,
          96, 100,
        88, 100} };
    for ( auto &discrWrapper : m_discrWrapperCol ) {
      discrWrapper->setRawConfCol( &m_rawConfCol );
    }
    xAOD::RingSetConf::print( m_rawConfCol, msg() );
    m_metaDataCached = true;
  } else {
    // Flag that meta is on outputMeta rather than the input 
    if ( failedToRetrieveInInput ){
      m_metaIsOnOutput = true;
      ATH_MSG_DEBUG("Retrieved meta from outputMetaStore.");
    }
  }

  ATH_MSG_DEBUG("Successfully retrieved store, "
      "trying to get RawConfiguration Collection.");

  // A little sanity check:
  if( !m_rsConfCont || m_rsConfCont->empty() ) {
     ATH_MSG_ERROR( "Metadata " << m_rsMetaName << " is not available on file." );
     return StatusCode::FAILURE;
  }

  // Retrieve the raw configuration:
  xAOD::RingSetConf::getRawConfCol( m_rawConfCol, m_rsConfCont );

  // Sign that it can be cached, if we want to cache it for the whole run:
  m_metaDataCached = true;

  // Pass its pointer into each wrapper in the discrimination chain:
  for ( auto &discrWrapper : m_discrWrapperCol ) {
    discrWrapper->setRawConfCol( &m_rawConfCol );
  }
  xAOD::RingSetConf::print( m_rawConfCol, msg() );

  ATH_MSG_DEBUG( "Successfully retrieve configuration info.");

  return StatusCode::SUCCESS;
}

//==============================================================================
StatusCode AsgElectronRingerSelector::beginEvent(){
  // This doesn't do anything for now, return gracefully:
  return StatusCode::SUCCESS;
}

#endif // RINGER_STANDALONE

// =============================================================================
void AsgElectronRingerSelector::retrieveFileConf(const char *fileName, 
    IOConfStruct &fileConf)
{
  // Try to open file and check if nothing wrong happened:
  TFile file(fileName, "READ");
  IOHelperFcns::checkFile( file );

  TDirectory* configDir = file.GetDirectory("");

  // When start using a version that this properties MUST be defined
  // in the file, change the max to the version value and implement
  // the else routine where it does throw.
  if ( IOHelperFcns::getWrittenVersion(configDir) 
      < std::numeric_limits<unsigned int>::max() )
  {
    try {
      IOHelperFcns::readVar( configDir, 
          "useTrackPat", 
          fileConf.useTrackPat );
    } catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir, 
          "useRawTrackPat", 
          fileConf.useRawTrackPat );
    } catch( std::runtime_error & ){;}
    //try {
    //  IOHelperFcns::readVar( configDir,
    //      "useRawCalStdPat",
    //      fileConf.useRawCalStdPat );
    //} catch( std::runtime_error & ){;}
    //try {
    //  IOHelperFcns::readVar( configDir,
    //      "useCalStdPat",
    //      fileConf.useCaloCommittee );
    //} catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir,
          "useCaloCommittee",
          fileConf.useCaloCommittee );
    } catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir,
          "useBLOutliers",
          fileConf.useBLOutliers );
    } catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir,
          "usePIXOutliers",
          fileConf.usePIXOutliers );
    } catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir,
          "useSCTOutliers",
          fileConf.useSCTOutliers );
    } catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir,
          "useTRTOutliers",
          fileConf.useTRTOutliers );
    } catch( std::runtime_error & ){;}
    try {
      IOHelperFcns::readVar( configDir,
          "useTRTXenonHits",
          fileConf.useTRTXenonHits );
    } catch( std::runtime_error & ){;}
  } /* else if */

  file.Close();
  return;
}

// =============================================================================
void AsgElectronRingerSelector::writeConf(const char* fileName, 
    IOConfStruct &fileConf)
{
  // Try to open file and check if nothing wrong happened:
  TFile file(fileName, "UPDATE");
  IOHelperFcns::checkFile( file );

  TDirectory* configDir = file.GetDirectory("");

  IOHelperFcns::writeVar( configDir, 
      "useTrackPat", 
      fileConf.useTrackPat );
  IOHelperFcns::writeVar( configDir, 
      "useRawTrackPat", 
      fileConf.useRawTrackPat );
  //IOHelperFcns::writeVar( configDir,
  //    "useRawCalStdPat",
  //    fileConf.useRawCalStdPat );
  //IOHelperFcns::writeVar( configDir,
  //    "useCalStdPat",
  //    fileConf.useCaloCommittee );
  IOHelperFcns::writeVar( configDir,
      "useCaloCommittee",
      fileConf.useCaloCommittee );
  IOHelperFcns::writeVar( configDir,
      "useBLOutliers",
      fileConf.useBLOutliers );
  IOHelperFcns::writeVar( configDir,
      "usePIXOutliers",
      fileConf.usePIXOutliers );
  IOHelperFcns::writeVar( configDir,
      "useSCTOutliers",
      fileConf.useSCTOutliers );
  IOHelperFcns::writeVar( configDir,
      "useTRTOutliers",
      fileConf.useTRTOutliers );
  IOHelperFcns::writeVar( configDir,
      "useTRTXenonHits",
      fileConf.useTRTXenonHits );
  file.Close();
}

// =============================================================================
void AsgElectronRingerSelector::printConf( IOConfStruct &fileConf, 
    MsgStream *msg, MSG::Level lvl)
{
  if ( msg && msg->level() <= lvl ){
    auto flags = static_cast<std::ios_base::fmtflags>(msg->flags());
    (*msg) << lvl << "File configuration is: " << endreq;
    (*msg) << lvl << std::boolalpha << "useTrackPat: " << fileConf.useTrackPat 
      << " | useRawTrackPat: " << fileConf.useRawTrackPat 
      << " | useCaloCommittee: " << fileConf.useCaloCommittee
      << " | useBLOutliers: " << fileConf.useBLOutliers
      << " | usePIXOutliers: " << fileConf.usePIXOutliers
      << " | useSCTOutliers: " << fileConf.useSCTOutliers
      << " | useTRTOutliers: " << fileConf.useTRTOutliers
      << " | useTRTXenonHits: " << fileConf.useTRTXenonHits 
      << endreq;
    // reset previous cout flags
    msg->flags(flags);
  }
}


} // namespace Ringer

