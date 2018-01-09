// /***************************************************************************
//                           ReprocessTriggerTowers.cxx  -  description
//                              -------------------
//     begin                : Tue May 18 2010
//     copyright            : (C) 2010 by Alan Watson
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//================================================
// ReprocessTriggerTowers class Implementation
// ================================================
//
//
//

// Services

#include "GaudiKernel/IIncidentSvc.h"

// This algorithm includes
#include "TrigT1CaloSim/ReprocessTriggerTowers.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
//#include "TrigT1CaloEvent/TriggerTowerCollection.h"


namespace LVL1{

const int ReprocessTriggerTowers::m_numTowers[m_Regions] = {m_nEMB,m_nEMEC,m_nFCAL1,m_nTile,m_nHEC,m_nFCAL23};

//--------------------------------
// Constructors and destructors
//--------------------------------

ReprocessTriggerTowers::ReprocessTriggerTowers
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ), 
      m_TTtool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
      m_mappingTool("LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool"),
      m_inputTowers(0),
      m_outputTowers(0)
{	
    m_InputTriggerTowerLocation  = TrigT1CaloDefs::TriggerTowerLocation;
    m_OutputTriggerTowerLocation = "ReprocessedTriggerTowers";

    // This is how you declare the paramembers to Gaudi so that
    // they can be over-written via the job options file
    
    declareProperty( "InputTriggerTowerLocation", m_InputTriggerTowerLocation ) ;
    declareProperty( "OutputTriggerTowerLocation", m_OutputTriggerTowerLocation ) ;

    m_doBCID = false;
    m_ZeroSuppress = false;
    m_LUTStrategy = 1;
    m_emThresh = 0;
    m_hadThresh = 0;
    declareProperty( "ZeroSuppress", m_ZeroSuppress );
    declareProperty( "doBCID", m_doBCID );
    declareProperty( "LUTStrategy", m_LUTStrategy );
    declareProperty( "emThreshold", m_emThresh );
    declareProperty( "hadThreshold", m_hadThresh );
    
    // Provide option to set custom thresholds by bin in 6 regions
    m_Thresholds.resize(m_Regions);
    for (int i = 0; i < m_Regions; ++i) {
	m_Thresholds[i].resize(m_numTowers[i]);
	for (int j = 0; j < m_numTowers[i]; ++j) m_Thresholds[i][j] = 0;
    }
    declareProperty( "EMBThresh", m_Thresholds[EMB]);
    declareProperty( "EMECThresh", m_Thresholds[EMEC]);
    declareProperty( "FCAL1Thresh", m_Thresholds[FCAL1]);
    declareProperty( "TileThresh", m_Thresholds[Tile]);
    declareProperty( "HECThresh", m_Thresholds[HEC]);
    declareProperty( "FCAL23Thresh", m_Thresholds[FCAL23]);
    
    // Provide custom scaling of LUT slopes in same regions
    m_Rescale.resize(m_Regions);
    for (int i = 0; i < m_Regions; ++i) {
	m_Rescale[i].resize(m_numTowers[i]);
	for (int j = 0; j < m_numTowers[i]; ++j) m_Rescale[i][j] = -1.;
    }
    declareProperty( "EMBScale", m_Rescale[EMB]);
    declareProperty( "EMECScale", m_Rescale[EMEC]);
    declareProperty( "FCAL1Scale", m_Rescale[FCAL1]);
    declareProperty( "TileScale", m_Rescale[Tile]);
    declareProperty( "HECScale", m_Rescale[HEC]);
    declareProperty( "FCAL23Scale", m_Rescale[FCAL23]);

}


//---------------------------------
// initialise()
//---------------------------------

StatusCode ReprocessTriggerTowers::initialize()
{
  /// and the L1CaloCondSvc
  ///  - note: may use tools without DB, so don't abort if fail here
  // sc = service("L1CaloCondSvc", m_l1CondSvc);
  // if (sc.isFailure()) { 
  //   ATH_MSG_WARNING( "Could not retrieve L1CaloCondSvc" );
  // }

  /// mapping tool
  ATH_CHECK( m_mappingTool.retrieve() );
  ATH_CHECK( m_TTtool.retrieve() );

  ServiceHandle<IIncidentSvc> incSvc ("IndicentSvc", name());
  ATH_CHECK( incSvc.retrieve() );

  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun");
  
  return StatusCode::SUCCESS ;
}


//-------------------------------------------------
// Update parameters at start of run
//-------------------------------------------------

void ReprocessTriggerTowers::handle(const Incident& inc) 
{
  if (inc.type()=="BeginRun") {
    ATH_MSG_DEBUG ( "Updating settings at start of run" );
    
    /// Set all thresholds to user-specified values except where "hot channel" settings were in use
  /** Initialise user-specified parameters:
        Fill std::map of parameter values, using channel ID as the key.
        Requires looping over all tower coordinates, filling values for each.
        However, we only fill entries when a user value has been specified - otherwise
        these will be taken from COOL. */
  
    m_channelNoiseCuts.clear();
    m_channelScales.clear();
  
    for (int iregion = 0; iregion < m_Regions; ++iregion) {
      int layer = ( (iregion == EMB || iregion == EMEC || iregion == FCAL1) ? 0 : 1 );
      int defaultThreshold = ( (iregion == EMB || iregion == EMEC || iregion == FCAL1) ? m_emThresh : m_hadThresh );
      for (int itower = 0; itower < m_numTowers[iregion]; ++itower) {
	// Noise cut for this eta bin
	int thresh = ( (m_Thresholds[iregion][itower] > 0) ? m_Thresholds[iregion][itower] : defaultThreshold );
	// Compute ids and store threshold for all towers
	int nPhi = 64;
	float etap = 0.1*(itower+0.5);
	float etam = -etap;
	if (iregion == EMEC || iregion == HEC) {  // endcap granularity is not uniform
	  if (itower < 10) etap += 1.5;
	  else {
	    nPhi = 32;
	    etap =  ( (itower != 13) ? (itower-10)*0.2 + 2.6 : 3.15 );
	  }
	  etam = -etap;
	}
	else if (iregion == FCAL1 || iregion == FCAL23) { // nominal eta bins, not physical tower sizes
	  nPhi = 16;
	  etap = 0.425*(itower+0.5) + 3.2;
	  if (iregion == FCAL1) etam = -etap;
	  else {
	    if (itower == 0 || itower == 2) etam = -0.425*(itower+1.5) - 3.2;
	    else                            etam = -0.425*(itower-0.5) - 3.2;
	  }
	}
	float dPhi = 2.*M_PI/nPhi;
	for (int iphi = 0; iphi < nPhi; ++iphi) {
	  float phi = dPhi*(iphi+0.5);
	  int channelp = channelId( etap, phi, layer).id();
	  int channelm = channelId( etam, phi, layer).id();
	  if (thresh > 0) { 
	    m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(channelp,thresh));
	    m_channelNoiseCuts.insert(std::map<unsigned int, int>::value_type(channelm,thresh));
	  }
	  if ((m_Rescale[iregion][itower])>=0) {
	    m_channelScales.insert(std::map<unsigned int, float>::value_type(channelp,m_Rescale[iregion][itower]));
	    m_channelScales.insert(std::map<unsigned int, float>::value_type(channelm,m_Rescale[iregion][itower]));
	  }
	} // loop over phi 
	
      } // loop over towers within region
    } // loop over regions
    
  }

  return ;
}



//---------------------------------
// finalise()
//---------------------------------

StatusCode ReprocessTriggerTowers::finalize()
{
   
   ATH_MSG_INFO( "Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode ReprocessTriggerTowers::execute( )
{

  //make a message logging stream

  ATH_MSG_DEBUG ( "in execute()" ); 

  // Create containers for this event
  m_outputTowers = new DataVector<TriggerTower>;       // Output tower container

  // Retrieve the TriggerTowerContainer
  if (evtStore()->contains<TriggerTowerCollection>(m_InputTriggerTowerLocation)) {
    StatusCode sc = evtStore()->retrieve( m_inputTowers, m_InputTriggerTowerLocation);  
    if ( sc==StatusCode::SUCCESS ) {
       // Check size of TriggerTowerCollection - zero would indicate a problem
      if ( m_inputTowers->size() == 0)
         ATH_MSG_WARNING( "Empty TriggerTowerContainer - looks like a problem" );
  
      // Reprocess the TriggerTowers
      preProcess();

    }
    else ATH_MSG_WARNING( "Error retrieving TriggerTowers" );
  }
  else ATH_MSG_WARNING( "No TriggerTowerContainer at " << m_InputTriggerTowerLocation );
  
  // Save reprocessed towers in TES
  store();
  
  // Clean up at end of event
   m_outputTowers=0;
 
  return StatusCode::SUCCESS ;
}

/** Simulate Pre-Processing of ADC data from input TriggerTowerCollection.
    Put results in a new TriggerTowerCollection */
void ReprocessTriggerTowers::preProcess() {

  //make a message logging stream
  ATH_MSG_DEBUG ( "in preProcess()" );
  
  /** Retrieve conditions for L1TriggerTowerTool */
  StatusCode scCond = m_TTtool->retrieveConditions();
  if (scCond.isFailure()) {
    ATH_MSG_WARNING( "Failed to retrieve conditions" );
    return;
  }
  /** Iterate through input TriggerTowerCollection */
  TriggerTowerCollection::const_iterator it = m_inputTowers->begin();
  for ( ; it != m_inputTowers->end(); ++it) {

    /** Find ChannelIDs for this TT */
    double eta = (*it)->eta();
    double phi = (*it)->phi();
    L1CaloCoolChannelId emId  = channelId(eta, phi, 0);
    L1CaloCoolChannelId hadId = channelId(eta, phi, 1);
    int emChannel = emId.id();
    int hadChannel = hadId.id();
    
    /// vectors to store intermediate results of preprocessing
    std::vector<int> emFIR;
    std::vector<int> emLUTIn;
    std::vector<int> emLUTOut;
    std::vector<int> emBCIDResult;
    std::vector<int> emRange;
    std::vector<int> emBCIDDecision;
      
    /** Retrieve and process EM ADC data */
    std::vector<int> emDigits = (*it)->emADC();
      
    /// Get original parameter values from DB
    std::vector<int> firCoeffs;
    int energyLow = 0;
    int energyHigh = 0;
    int decisionSource = 0;
    std::vector<unsigned int> decisionConditions;
    unsigned int peakFinderStrategy = 0;
    int satLow = 0;
    int satHigh = 0;
    int satLevel = 0;
    int startBit = 0;
    int slope = 0;
    int offset = 0;
    int cut = 0;
    int pedValue = 0;
    float pedMean = 0.;
    int strategy = 0;
    bool disabled = 0;
    m_TTtool->firParams(emId, firCoeffs);
    m_TTtool->bcidParams(emId, energyLow, energyHigh, decisionSource, decisionConditions, peakFinderStrategy, satLow, satHigh, satLevel);
    m_TTtool->lutParams(emId, startBit, slope, offset, cut, pedValue, pedMean, strategy, disabled);
    
    /// Adjust LUT slope if specified by user
    std::map<unsigned int, float>::iterator itScale=m_channelScales.find( emChannel );
    if (itScale != m_channelScales.end()) slope *= itScale->second;    
    
    /// If user wishes to change LUT strategy, or change calibration with strategy 1, need to recalculate the LUT offset for the channel
    if ( (m_LUTStrategy >= 0 && strategy != m_LUTStrategy) || itScale != m_channelScales.end() ) {
       int firCoeffSum = 0;
       for (unsigned int i = 0; i < firCoeffs.size(); ++i) firCoeffSum += firCoeffs[i];
       float firPed = pedMean * firCoeffSum;
       float offsetReal = 0.;
       if ( (m_LUTStrategy == 1 && strategy != m_LUTStrategy) ||
	    (m_LUTStrategy == 1 && itScale != m_channelScales.end()) ||
	    (m_LUTStrategy < 0  && strategy == 1 && itScale != m_channelScales.end()) ) {
	  offsetReal = firPed*slope/pow(2.,startBit)  - slope*0.5;
          offset = int(offsetReal + 0.5);
	  if (offset < 0) offset = 0;
          strategy = m_LUTStrategy;
       }
       else if (m_LUTStrategy == 0 && strategy != m_LUTStrategy) {
          offsetReal = firPed/pow(2.,startBit);
          offset = int(offsetReal + 0.5);
	  if (offset < 0) offset = 0;
          strategy = m_LUTStrategy;
       }
       else if (m_LUTStrategy > 1) ATH_MSG_DEBUG( "Unknown LUT Strategy selected. Ignore");
    }
    /// Simulate filter
    m_TTtool->fir(emDigits, firCoeffs, emFIR);
    m_TTtool->dropBits(emFIR, startBit, emLUTIn);
      
    /// BCID decision range
    std::vector<int> emDecisionRange;
    m_TTtool->bcidDecisionRange(emLUTIn, emDigits, emId, emDecisionRange);

    /// BCID simulation, if requested
    if (m_doBCID) {
      m_TTtool->bcid(emFIR, emLUTIn, emDigits, energyLow, energyHigh, decisionSource, decisionConditions, peakFinderStrategy, satLow, satHigh, satLevel, emBCIDResult, emBCIDDecision);
    }
    else {
      /// Copy original BCID output from TT into BCIDResult vector at appropriate location
      int bcid = (*it)->emBCID();
      for (int slice = 0; slice < (int)emDigits.size(); ++slice) {
	if (slice == (*it)->emADCPeak()) emBCIDResult.push_back(bcid);
	else emBCIDResult.push_back(0);
      }     
      /// Recalculate whether it would have passed BCID (i.e. whether filter changed decision range)
      m_TTtool->bcidDecision(emBCIDResult, emDecisionRange, emId, emBCIDDecision);
    }
    
    /// Set user-specified value for threshold, if specified, otherwise take from COOL
    int thresh = cut;
    std::map<unsigned int, unsigned int>::iterator itThresh=m_channelNoiseCuts.find( emChannel );
    if (itThresh != m_channelNoiseCuts.end()) thresh = itThresh->second;
    //else ATH_MSG_VERBOSE( "No user-specified threshold for channel " << std::hex << emChannel << std::dec );

    /// LUT simulation
    m_TTtool->lut(emLUTIn, slope, offset, thresh, pedValue, strategy, disabled, emLUTOut);
 
    /// Suppress LUT ET where BCID failed. Force full scale if saturated.
    std::vector<int>::iterator itlut = emLUTOut.begin();
    std::vector<int>::const_iterator itbcid = emBCIDDecision.begin();
    while (itlut != emLUTOut.end()) {
      if (itbcid != emBCIDDecision.end()) {
	if ((*itbcid)==0) (*itlut)=0;
        ++itbcid;
      }
      ++itlut;
    }
    // Force full scale for saturated towers
    std::vector<int> emETOut;
    m_TTtool->applyEtRange(emLUTOut, emDecisionRange, emId, emETOut);
    
    /** Repeat for Had ADC data */
    std::vector<int> hadFIR;
    std::vector<int> hadLUTIn;
    std::vector<int> hadLUTOut;
    std::vector<int> hadBCIDResult;
    std::vector<int> hadRange;
    std::vector<int> hadBCIDDecision;

    std::vector<int> hadDigits = (*it)->hadADC();

    m_TTtool->firParams(hadId, firCoeffs);
    m_TTtool->bcidParams(hadId, energyLow, energyHigh, decisionSource, decisionConditions, peakFinderStrategy, satLow, satHigh, satLevel);
    m_TTtool->lutParams(hadId, startBit, slope, offset, cut, pedValue, pedMean, strategy, disabled);
    
    /// Adjust LUT slope if specified by user
    itScale=m_channelScales.find( hadChannel );
    if (itScale != m_channelScales.end()) slope *= itScale->second;    
    
    /// If user wishes to change LUT strategy, or change calibration with strategy 1, need to recalculate the LUT offset for the channel
    if ( (m_LUTStrategy >= 0 && strategy != m_LUTStrategy) || itScale != m_channelScales.end() ) {
       int firCoeffSum = 0;
       for (unsigned int i = 0; i < firCoeffs.size(); ++i) firCoeffSum += firCoeffs[i];
       float firPed = pedMean * firCoeffSum;
       float offsetReal = 0.;
       if ( (m_LUTStrategy == 1 && strategy != m_LUTStrategy) ||
	    (m_LUTStrategy == 1 && itScale != m_channelScales.end()) ||
	    (m_LUTStrategy < 0  && strategy == 1 && itScale != m_channelScales.end()) ) {
	  offsetReal = firPed*slope/pow(2.,startBit)  - slope*0.5;
          offset = int(offsetReal + 0.5);
	  if (offset < 0) offset = 0;
          strategy = m_LUTStrategy;
       }
       else if (m_LUTStrategy == 0 && strategy != m_LUTStrategy) {
          offsetReal = firPed/pow(2.,startBit);
          offset = int(offsetReal + 0.5);
	  if (offset < 0) offset = 0;
          strategy = m_LUTStrategy;
       }
       else if (m_LUTStrategy > 1) ATH_MSG_DEBUG( "Unknown LUT Strategy selected. Ignore");
    }
    
    /// Simulate filter
    m_TTtool->fir(hadDigits, firCoeffs, hadFIR);
    m_TTtool->dropBits(hadFIR, startBit, hadLUTIn);
    
    /// BCID decision range
    std::vector<int> hadDecisionRange;
    m_TTtool->bcidDecisionRange(hadLUTIn, hadDigits, hadId, hadDecisionRange);
    
    /// BCID, if requested
    if (m_doBCID) {
      m_TTtool->bcid(hadFIR, hadLUTIn, hadDigits, energyLow, energyHigh, decisionSource, decisionConditions, peakFinderStrategy, satLow, satHigh, satLevel, hadBCIDResult, hadBCIDDecision);
    }
    else {
      /// Copy original BCID output from TT into BCIDResult vector at appropriate location
      int bcid = (*it)->hadBCID();
      for (int slice = 0; slice < (int)hadDigits.size(); ++slice) {
	if (slice == (*it)->hadADCPeak()) hadBCIDResult.push_back(bcid);
	else hadBCIDResult.push_back(0);
      }     
      /// need to recalculate BCID decision though (this is not stored)
      m_TTtool->bcidDecision(hadBCIDResult, hadDecisionRange, hadId, hadBCIDDecision);
    }
    /// Set user-specified value for threshold, if available, otherwise take from COOL
    thresh = cut;
    itThresh=m_channelNoiseCuts.find( hadChannel );
    if (itThresh != m_channelNoiseCuts.end()) thresh = itThresh->second;
    //else ATH_MSG_VERBOSE( "No user-specified threshold for channel " << std::hex << hadChannel << std::dec );

    /// LUT simulation
    m_TTtool->lut(hadLUTIn, slope, offset, thresh, pedValue, strategy, disabled, hadLUTOut);
    
    /// Suppress LUT ET where BCID failed
    itlut = hadLUTOut.begin();
    itbcid = hadBCIDDecision.begin();
    while (itlut != hadLUTOut.end()) {
      if (itbcid != hadBCIDDecision.end()) {
	if ((*itbcid)==0) (*itlut)=0;
	++itbcid;
      }
      ++itlut;
    }
    // Force full scale for saturated towers
    std::vector<int> hadETOut;
    m_TTtool->applyEtRange(hadLUTOut, hadDecisionRange, hadId, hadETOut);
    
          
    /** Create new TriggerTower containing results of reprocessing */

    /// Zero suppression, if requested. Triggered sample in ET vectors = ADC peak (the way L1TriggerTowerTool works)
    int emADCpeak = (*it)->emADCPeak();
    int hadADCpeak = (*it)->hadADCPeak();
    bool output = true;
    if (m_ZeroSuppress && emETOut[emADCpeak]==0 && hadETOut[hadADCpeak]==0) output = false;
    
    if (output) {
      /// Output only peak slice for LUT ET and BCID results (simple, robust, and what we normally read out)
      std::vector<int> emET;
      emET.push_back(emETOut[emADCpeak]);
      std::vector<int> emBCID;
      emBCID.push_back(emBCIDResult[emADCpeak]);

      std::vector<int> hadET;
      hadET.push_back(hadETOut[hadADCpeak]);
      std::vector<int> hadBCID;
      hadBCID.push_back(hadBCIDResult[hadADCpeak]);
    
      /// Create output TT and add to vector
      std::vector<int> emExt  = (*it)->emBCIDext();
      std::vector<int> hadExt = (*it)->hadBCIDext();
      TriggerTower* tower = new TriggerTower(phi, eta, (*it)->key(),
                            emDigits,  emET,  emExt,   emBCID, 0,  0, emADCpeak,
                            hadDigits, hadET, hadExt, hadBCID, 0,  0, hadADCpeak);
      
      m_outputTowers->push_back(tower);
    } // if (output)
    
  } // Loop over towers
  
}

/** place final ROI objects in the TES. */
void LVL1::ReprocessTriggerTowers::store() {

  StatusCode sc = evtStore()->overwrite(m_outputTowers, m_OutputTriggerTowerLocation, true);

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE ( "Stored " <<m_outputTowers->size()
                      << " TriggerTowers at " << m_OutputTriggerTowerLocation );
  }
  else {
     ATH_MSG_ERROR( "failed to write TriggerTowers to  "
         << m_OutputTriggerTowerLocation );
  } 

  return;

} //end store

/** Function to compute Id from eta/phi/layer.
    Unlike L1TriggerTowerTool::channelID this function can cope with old geometries (hence
    the hard-coded numbers). So don't remove this until all ATLAS-CSC datasets are irrevokably
    deprecated */
L1CaloCoolChannelId LVL1::ReprocessTriggerTowers::channelId(double eta, double phi, int layer) {
   int crate, module, channel;
   m_mappingTool->mapping(eta, phi, layer, crate, module, channel);
   int slot = module + 5;
   int pin = channel % 16;
   int asic = channel / 16;
   return L1CaloCoolChannelId(crate, L1CaloModuleType::Ppm, slot, pin, asic, false);
}


} // end of LVL1 namespace bracket



