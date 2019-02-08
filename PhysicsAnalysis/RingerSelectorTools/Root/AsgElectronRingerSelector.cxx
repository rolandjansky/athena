/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AsgElectronRingerSelector.cxx 791627 2017-01-10 04:45:53Z wsfreund $
// STL includes:
#include <string>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <iostream>

#ifndef RINGER_STANDALONE
// Athena framework includes:
# include "AsgTools/AsgTool.h"
# include "PATCore/AcceptInfo.h"
# include "PATCore/AcceptData.h"
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

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#ifndef XAOD_ANALYSIS
#include "PATCore/AcceptData.h"
#endif
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

  declareProperty("CacheConfData", m_cacheMetaData = true,
      "Whether to cache metadata configuration or not");

  declareProperty("DefaultNvtx", m_defaultNvtx = 0,
      "Default value for nvtx when not using it");

  declareProperty("primaryVertexContainer",
                  m_primVtxContName="PrimaryVertices",
                  "The primary vertex container name" );

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
  ATH_CHECK( readConfigFiles() );

  return StatusCode::SUCCESS;
}

//==============================================================================
// External initialize method
//==============================================================================
StatusCode AsgElectronRingerSelector::initialize(
    const char* metaName, unsigned int cutMask,
    IDiscrWrapperCollection &discrWrapper, IThresWrapper* thresWrapper,
    RingerConfStruct &fileConf
    )
{
  setRSMetaName( metaName );

  // Set the unsigned mask into the bitset
  m_cutsToUse = ElectronTAccept::bitMskWord(cutMask);
  ATH_MSG_DEBUG("Cut mask was set to: " << m_cutsToUse.to_string());

  m_discrWrapperCol = discrWrapper;

  /// @brief The discriminator chain collection
  m_thresWrapper = thresWrapper;

  ATH_CHECK( configureFromStruct(fileConf) );

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

  // Read discrimination wrapper collection
  IThresWrapper::read(m_thresWrapper, thresFileName.c_str() );

  ATH_MSG_INFO( "Using ThresholdFile: " << thresFileName.c_str());

  // Get discrimination information flow configuration
  Ringer::RingerConfStruct fileConf;
  fileConf.retrieveConf( discrFileName.c_str() );
  fileConf.printConf( &(this->msg()), MSG::DEBUG );

  ATH_CHECK( configureFromStruct(fileConf) );

  return StatusCode::SUCCESS;

}

//=============================================================================
// Private method to be called by initialize methods
//=============================================================================
StatusCode AsgElectronRingerSelector::configureFromStruct(RingerConfStruct &fileConf)
{
  // Set message stream and print configuration:
  for ( unsigned discrIdx = 0;
      discrIdx < m_discrWrapperCol.size();
      ++discrIdx)
  {
    m_discrWrapperCol[discrIdx]->setMsgStream( &(this->msg()));
    m_discrWrapperCol[discrIdx]->print();
  }

  this->m_useNvtx = fileConf.useNvtx;

  // Set message stream and print configuration:
  m_thresWrapper->setMsgStream( &(this->msg() ));
  m_thresWrapper->print();

  // Create our discrimination chain selector:
  m_ringSelCommon = new Ringer::RingerCommonSelector(
      &m_discrWrapperCol,
      m_thresWrapper,
      fileConf);

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
asg::AcceptData AsgElectronRingerSelector::accept(
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
asg::AcceptData AsgElectronRingerSelector::accept(
    const xAOD::Electron* el, const double mu) const
{
  ATH_MSG_DEBUG("Entering accept( const xAOD::Electron* el)");

  asg::AcceptData acceptData (&m_accept);
  StatusCode sc = execute(el, mu, acceptData);

  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error while on particle AsgSelector execution.");
  }

  return acceptData;
}


// =============================================================================
asg::AcceptData AsgElectronRingerSelector::accept(
    const xAOD::Egamma* eg, const double mu ) const
{
  ATH_MSG_DEBUG("Entering accept( const xAOD::Egamma* part )");

  asg::AcceptData acceptData (&m_accept);
  StatusCode sc = execute(eg, mu, acceptData);

  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error while on particle AsgSelector execution.");
  }

  return acceptData;
}


// =============================================================================
StatusCode AsgElectronRingerSelector::execute(
    const xAOD::Electron* el, const double mu,
    asg::AcceptData& acceptData) const
{

  ATH_MSG_DEBUG("Entering execute(const xAOD::Electron* el...)");

  m_ringSelCommon->clear();

  // No error occurred so far, flag it:
  acceptData.setCutResult(
      BitdefElectron::NoErrorBit,
      true);

  // Set if it was requested to execute CutID:
  acceptData.setCutResult(
      BitdefElectron::ExecCutID,
      m_useCutIDDecision );

  if (!el){
    ATH_MSG_ERROR("Invalid electron pointer.");

    acceptData.setCutResult(
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

    acceptData.setCutResult(
        BitdefElectron::NoErrorBit,
        false);

    return StatusCode::FAILURE;
  }

  // FIXME The indexing should be an enum, previous test should ensure that the
  // size is large enought to access index.
  if ( !caloRingsLinks->at(0).isValid() ){
    ATH_MSG_DEBUG("Ignoring candidate with invalid ElementLink.");

    acceptData.setCutResult(
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

    acceptData.setCutResult(
        BitdefElectron::NoErrorBit,
        false );

    return StatusCode::FAILURE;
  }

  // Get track information:
  TrackPatternsHolder *trackPat_ref{nullptr};
  if( el->type()==xAOD::Type::Electron ){
    const xAOD::TrackParticle *track = el->trackParticle();
    trackPat_ref = m_trackPat;
    // Fill track pattern holder with new information:
    trackPat_ref->extractPatternsFrom( track, el );
  }

  float pileup_estimation{0};
  // Determine pile-up estimator and retrieve its value (based on Likelihood tool code)
  //bool doCentralityTransform = m_rootTool->doCentralityTransform;
  if( mu < 0 ){ // use npv if mu is negative (not given)
    //if (doCentralityTransform) ip = static_cast<double>(m_useCaloSumsCont ? this->getFcalEt() : m_fcalEtDefault);
    pileup_estimation = m_nVtx;
  } else {
    pileup_estimation = mu;
  }

  // Put cluster calibrated eta/et into DepVarStruct
  const xAOD::CaloCluster *cluster = el->caloCluster();
  const DepVarStruct depVar( cluster->et() * 1e-3,
                             cluster->eta(),
                             pileup_estimation );
  // We head now into the RingerChain execution:
  try {
    // Execute Ringer Selector Common:
    if ( m_ringSelCommon->execute(
          depVar,
          clrings,
          trackPat_ref,
          acceptData).isFailure() )
    {

      ATH_MSG_ERROR("Error while executing "
          "RingerCommonSelector.");

      acceptData.setCutResult(
          BitdefElectron::NoErrorBit,
          false);

    }
  } catch (const std::exception &e ) {

    ATH_MSG_ERROR("Error while executing "
        "RingerCommonSelector. Reason: " << e.what() );

    acceptData.setCutResult(
        BitdefElectron::NoErrorBit,
        false);

  }

  // Add the CutID track decision bit (if requested):
  if ( m_useCutIDDecision ) {
    try {
      acceptData.setCutResult(
          BitdefElectron::CutIDDec,
          static_cast<bool>(m_cutIDSelector->accept(el)) );

    } catch ( const std::exception &e ) {

      ATH_MSG_ERROR("Error while executing "
          "AsgElectronRingerSelector. Reason:" << e.what() );

      acceptData.setCutResult(
          BitdefElectron::NoErrorBit,
          false );

      acceptData.setCutResult(
          BitdefElectron::CutIDDec ,
          false );

    }
  } else {

    // If we do not run it, set track decision to true:
    acceptData.setCutResult(
        BitdefElectron::CutIDDec ,
        true );

  }

  // Clear unused bits.
  for (unsigned bit = 0; bit < BitdefElectron::NUsedBits(); ++bit ){
    // m_partDec mask is set to true if passed cut.
    // m_cutsToUse is set to true if cut is to be used.
    if (!m_cutsToUse[bit]) {
      acceptData.setCutResult (bit, false);
    }
  }

  // Check if an error occurred, and flag it:
  if ( acceptData.getCutResult(
        BitdefElectron::NoErrorBit ) )
  {
    return StatusCode::SUCCESS;
  } else {
    return StatusCode::FAILURE;
  }
}

//==============================================================================
StatusCode AsgElectronRingerSelector::execute(const xAOD::Egamma* eg, const double mu,
                                              asg::AcceptData& acceptData) const
{
  ATH_MSG_DEBUG("entering execute(const xAOD::Egamma* eg...)");

  if (eg){
    // We cast it to electron and use xAOD::type to determine whether it is an electron or not
    this->execute( static_cast<const xAOD::Electron*>(eg), mu, acceptData );
  } else {
    ATH_MSG_ERROR("Egamma pointer is null.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==============================================================================
StatusCode AsgElectronRingerSelector::beginInputFile()
{
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
  ATH_MSG_VERBOSE( "inputMetaStore: \n " << inputMetaStore()->dump() );
  ATH_MSG_VERBOSE( "outputMetaStore: \n " << outputMetaStore()->dump() );
#endif

  // Tell the user what's happening:
  ATH_MSG_DEBUG( "Entered new file, checking if it is needed to "
      "update metadata information.");

  if ( m_cacheMetaData && m_metaDataCached ) {
    ATH_MSG_DEBUG( "Metadata is already cached." );
    return StatusCode::SUCCESS;
  }

  // Read the metadata object:
  m_rsConfCont = nullptr;

  bool readFromInput(false), readFromOutput(false);

  // First we try to obtain configuration on the inputMetaStore. If this does
  // not work, we try to obtain it on the outputMetaStore.
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
  m_rsConfCont = inputMetaStore()->tryConstRetrieve< xAOD::RingSetConfContainer >( m_rsMetaName );
  readFromInput = !m_metaIsOnOutput && ( m_rsConfCont != nullptr );
#else
  readFromInput = !m_metaIsOnOutput && inputMetaStore()->retrieve( m_rsConfCont, m_rsMetaName ).isSuccess();
#endif

  if ( !readFromInput )
  {
    ATH_MSG_DEBUG("Retrieved meta from inputMetaStore.");
  } else if ( !m_metaIsOnOutput ) {
    ATH_MSG_DEBUG("Couldn't retrieve on inputMetaStore, "
        "trying to retrieve from outputMetaStore.");
  }

  readFromOutput = ( m_metaIsOnOutput || !readFromInput ) && outputMetaStore()->retrieve( m_rsConfCont, m_rsMetaName ).isSuccess();

  // We also attempt to retrieve on outputMetaStore if inputMetaStore failed:
  if ( !readFromOutput )
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
    if ( !readFromInput ){
      m_metaIsOnOutput = true;
      ATH_MSG_DEBUG("Retrieved meta from outputMetaStore.");
    }
    // A little sanity check:
    if( !m_rsConfCont || m_rsConfCont->empty() ) {
       ATH_MSG_ERROR( "Metadata " << m_rsMetaName << " is not available on file." );
       return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved store, "
        "trying to get RawConfiguration Collection.");

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
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// Helper method to get the number of primary vertices (copied from
// ElectronLikelihoodTool)
//
// This probably should be added to another tool which would provide this
// calculated to every other tool which needs it
//=============================================================================
unsigned int AsgElectronRingerSelector::getNPrimVertices() const
{
  static bool PVExists = true;
  unsigned int nVtx(0);
  const xAOD::VertexContainer* vxContainer(0);
  if(PVExists)
  {
    if ( StatusCode::SUCCESS != evtStore()->retrieve( vxContainer, m_primVtxContName ) )
    {
      ATH_MSG_WARNING ( "Vertex container not found with name: " << m_primVtxContName );
      PVExists = false; // if retrieve failed, don't try to retrieve again
      return nVtx;
    }
    for ( unsigned int i=0; i<vxContainer->size(); i++ )
    {
      const xAOD::Vertex* vxcand = vxContainer->at(i);
      if ( vxcand->nTrackParticles() >= 2 ) nVtx++;
    }
  }
  return nVtx;
}


//==============================================================================
StatusCode AsgElectronRingerSelector::beginEvent(){
  // Fill the number of vertex
  this->m_nVtx = (this->m_useNvtx)?(getNPrimVertices()):m_defaultNvtx;
  return StatusCode::SUCCESS;
}

#endif // RINGER_STANDALONE

// =============================================================================
void RingerConfStruct::retrieveConf(const char *fileName)
{
  // Try to open file and check if nothing wrong happened:
  TFile file(fileName, "READ");
  IOHelperFcns::checkFile( file );

  TDirectory* configDir = file.GetDirectory("RingerConfStruct");

  this->version = 0;

  // Get io conf struct version:
  try {
    IOHelperFcns::readVar( configDir,
        "RingerConfStruct__version__",
        this->version );
  } catch( std::runtime_error & ){;}

  try {
    IOHelperFcns::readVar( configDir,
        "useTrackPat",
        this->useTrackPat );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useRawTrackPat",
        this->useRawTrackPat );
  } catch( std::runtime_error & ){;}
  //try {
  //  IOHelperFcns::readVar( configDir,
  //      "useRawCalStdPat",
  //      this->useRawCalStdPat );
  //} catch( std::runtime_error & ){;}
  //try {
  //  IOHelperFcns::readVar( configDir,
  //      "useCalStdPat",
  //      this->useCaloCommittee );
  //} catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useCaloCommittee",
        this->useCaloCommittee );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useBLOutliers",
        this->useBLOutliers );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "usePIXOutliers",
        this->usePIXOutliers );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useSCTOutliers",
        this->useSCTOutliers );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useTRTOutliers",
        this->useTRTOutliers );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useTRTXenonHits",
        this->useTRTXenonHits );
  } catch( std::runtime_error & ){;}
  try {
    IOHelperFcns::readVar( configDir,
        "useNvtx",
        this->useNvtx );
  } catch( std::runtime_error & ){;}

  if ( version < 1 ){
    // nvtx only started being used on version 17
    this->useNvtx = false;
  }

  file.Close();
  return;
}

// =============================================================================
void RingerConfStruct::writeConf( char* fileName )
{
  // Try to open file and check if nothing wrong happened:
  TFile file(fileName, "UPDATE");
  IOHelperFcns::checkFile( file );

  TDirectory* configDir = file.mkdir("RingerConfStruct");

  IOHelperFcns::writeVar( configDir,
      "useTrackPat",
      this->useTrackPat );
  IOHelperFcns::writeVar( configDir,
      "useRawTrackPat",
      this->useRawTrackPat );
  //IOHelperFcns::writeVar( configDir,
  //    "useRawCalStdPat",
  //    this->useRawCalStdPat );
  //IOHelperFcns::writeVar( configDir,
  //    "useCalStdPat",
  //    this->useCaloCommittee );
  IOHelperFcns::writeVar( configDir,
      "useCaloCommittee",
      this->useCaloCommittee );
  IOHelperFcns::writeVar( configDir,
      "useBLOutliers",
      this->useBLOutliers );
  IOHelperFcns::writeVar( configDir,
      "usePIXOutliers",
      this->usePIXOutliers );
  IOHelperFcns::writeVar( configDir,
      "useSCTOutliers",
      this->useSCTOutliers );
  IOHelperFcns::writeVar( configDir,
      "useTRTOutliers",
      this->useTRTOutliers );
  IOHelperFcns::writeVar( configDir,
      "useTRTXenonHits",
      this->useTRTXenonHits );
  IOHelperFcns::writeVar( configDir,
      "useNvtx",
      this->useNvtx );
  IOHelperFcns::writeVar( configDir,
      "RingerConfStruct__version__",
      this->version );
  file.Close();
}

// =============================================================================
void RingerConfStruct::printConf( MsgStream *msg, MSG::Level lvl )
{
  if ( msg && msg->level() <= lvl ){
    auto flags = static_cast<std::ios_base::fmtflags>(msg->flags());
    (*msg) << lvl << "Ringer Configuration is: " << endmsg;
    (*msg) << lvl << std::boolalpha
      << "\n * useTrackPat      : " << (( this->useTrackPat      )?"yes":"no")
      << "\n * useRawTrackPat   : " << (( this->useRawTrackPat   )?"yes":"no")
      << "\n * useCaloCommittee : " << (( this->useCaloCommittee )?"yes":"no")
      << "\n * useBLOutliers    : " << (( this->useBLOutliers    )?"yes":"no")
      << "\n * usePIXOutliers   : " << (( this->usePIXOutliers   )?"yes":"no")
      << "\n * useSCTOutliers   : " << (( this->useSCTOutliers   )?"yes":"no")
      << "\n * useTRTOutliers   : " << (( this->useTRTOutliers   )?"yes":"no")
      << "\n * useTRTXenonHits  : " << (( this->useTRTXenonHits  )?"yes":"no")
      << "\n * useNvtx          : " << (( this->useNvtx          )?"yes":"no")
      << endmsg;
    // reset previous cout flags
    msg->flags(flags);
  }
}


} // namespace Ringer

