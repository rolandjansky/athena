/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
 MuonTrkPhysMonitoring.cxx
 ------------------------------
 AUTHORS:     Nektarios Chr.Benekos, Austin Basye
 created:     September 2011
 modifed:     September 2013 , Christina G. Skarpathiotaki (UoP)
 description: Implementation code for the MuonTrkPhysMonitoring
 ============================================================================== */

#include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

using namespace MuonTrkPhysMonUtils;

// *********************************************************************
// Public Methods
// *********************************************************************

MuonTrkPhysMonitoring::MuonTrkPhysMonitoring( const std::string & type, const std::string & name, const IInterface* parent )
:ManagedMonitorToolBase( type, name, parent ),
m_log( msgSvc(), name ),
m_lumiblock(0),
m_eventCounter(0),
m_oOnlinePlots(0),
m_oOccupancyPlots(0),
m_oGenTrackingPlots(0),
m_oBFMonPlots(0),
m_oDetSpcfcPlots(0),
m_oCbMSIDAlignPlots(0),
m_oZSignalPlots(0)
{

	m_pullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");

	//Inital Components
	declareProperty( "MuonTrackCollections",  m_trackCollectionName);
	declareProperty( "MuonTriggerChainName",  m_MuonTriggerChainName);
	declareProperty( "TriggerDecisionTool",   m_trigDecTool);
	declareProperty( "Muon_Trigger_Items",    m_muon_triggers);
	declareProperty( "UseTriggerVector",      m_useTrigger);

	//Global Cuts.
	declareProperty("GlobalPtCut_pp",        m_GlobalPtCut_pp       = 15.00/*GeV*/);
	declareProperty("GlobalPtCut_cosmic",    m_GlobalPtCut_cosmic   = 00.00/*GeV*/);
	declareProperty("GlobalPtCut_heavyIon",  m_GlobalPtCut_heavyIon = 05.00/*GeV*/);
	declareProperty("GlobalResidualError",   m_GlobalResidualError  = 01.00/*mm*/);

	//Tag and Probe Properties
	declareProperty( "diMuonMassWindow",          m_cut_diMuonMass = 0);
	declareProperty( "InnerTrackContainer" ,      m_InnerTrackContainer );
	declareProperty( "doZTrigMatch",              m_doZTrigMatch = true);
	declareProperty( "tagMuonTrigger",            m_tagMuonTrigger = std::string("EF_mu18_medium"));

	//isolation
	declareProperty( "isoConeMax",                m_isoConeMax = 0.4);
	declareProperty( "isoConeMin",                m_isoConeMin = 0);

	//others
	declareProperty( "MuonHistSumTool",           m_muonHitSummaryTool = std::string("Muon::MuonHitSummaryTool/MuonHitSummaryTool"));
}

//---------------------------------------------------------------------------------------
MuonTrkPhysMonitoring::~MuonTrkPhysMonitoring()
{
	ATH_MSG_INFO(" deleting MuonTrkPhysMonitoring ");
}

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuonTrkPhysMonitoring:: initialize()
{
	ATH_MSG_DEBUG("MuonTrkPhysMonitoring::initialize() to setup tools/services");
	if( ManagedMonitorToolBase::initialize().isFailure()) return StatusCode::FAILURE;
	if( setupTools().isFailure()) return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode MuonTrkPhysMonitoring::bookHistogramsRecurrent()
{

	m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Entry" << endreq;

	// TODO Check where m_environment is set?
	if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || 
            m_environment == AthenaMonManager::online || m_environment == AthenaMonManager::AOD) {

		m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Envrionment [" << m_environment << "]" << endreq;

		//A bit clever way to build a string with a placeholder. Further down the placeholder is substituted with the MonGroup name via Form(string.c_str(), name)
		std::string muonMon_path = "Muon/MuonTrkPhysMonitoring";
		if (m_MuonTriggerChainName == "NoMuonTriggerSelection") muonMon_path = muonMon_path+"/NoTrigger/" + "%s" + m_trackCollectionName;
		else muonMon_path  = muonMon_path+"/TriggerAware/" + "%s" + m_trackCollectionName + "__"+m_MuonTriggerChainName;


		if(newRun){

			m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): New Run" << endreq;

			InitializePlots(m_oOccupancyPlots, MonGroup(this,Form(muonMon_path.c_str(), "TrackingPerformance/General/"),run,ATTRIB_UNMANAGED));

			if ( AthenaMonManager::environment() == AthenaMonManager::online ) { // book histograms that are only made in the online environment...

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Initializing Online Plots" << endreq;

				InitializePlots(m_oOnlinePlots, MonGroup(this,Form(muonMon_path.c_str(), "TrackingPerformance/General/"),run,ATTRIB_UNMANAGED));
			}
			else {

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking Offline Plots" << endreq;
				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking Offline Plots" << endreq;

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking MS_ID_Alignment Plots" << endreq;
				bookHistograms_CbMSIDAlign(Form(muonMon_path.c_str(), "MS_ID_Alignment/"));

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking B_Field_Alignment Plots" << endreq;
				bookHistograms_BFMon(Form(muonMon_path.c_str(), "B_Field_Alignment/"));

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking TrackingPerformance/Z_Signal Plots" << endreq;
				bookHistograms_ZSignal(Form(muonMon_path.c_str(), "TrackingPerformance/Z_Signal/"));

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking Detector_Specific Plots" << endreq;
				bookHistograms_DetSpcfc(Form(muonMon_path.c_str(), "Detector_Specific/"));

				m_log << MSG::DEBUG << "MuonTrkPhysMonitoring::bookHistogramsRecurrent(): Booking TrackingPerformance/General Plots" << endreq;
				bookHistograms_GenTracking(Form(muonMon_path.c_str(), "TrackingPerformance/General/"));

			}// if not online Environment

		} //  if NewRun
		
	} //Environment

	ATH_MSG_DEBUG("MuonTrkPhysMonitoring : Histograms Successfully Booked");
	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode MuonTrkPhysMonitoring::fillHistograms()
{
	ATH_MSG_DEBUG("Inside fillHistograms()");
	if( !(m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || 
              m_environment == AthenaMonManager::online || m_environment == AthenaMonManager::AOD) ) return StatusCode::SUCCESS;

	m_lumiblock = retrieveLumiBlock();
	if( !passTrigger() )  return StatusCode::SUCCESS;
	handleMuonContainer(m_trackCollectionName);
	++m_eventCounter;
	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode MuonTrkPhysMonitoring::procHistograms()
{
	if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || 
            m_environment == AthenaMonManager::online || m_environment == AthenaMonManager::AOD ) {
		m_oOccupancyPlots.finalize();
		ATH_MSG_DEBUG("MuonTrkPhysMonitoring finalize()");
		if(endOfLumiBlock){
			if(m_environment == AthenaMonManager::online)
				m_oOnlinePlots.finalize();
		}
		if(endOfRun){
			if(m_environment != AthenaMonManager::online) {
				procHistograms_GenTracking();
				procHistograms_DetSpcfc();
				procHistograms_CbMSIDAlign();
				procHistograms_BFMon();
				procHistograms_ZSignal();
			}//Is offline
		} // isEndOfRun

	} //environment

	return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::handleMuonContainer( std::string collectionName) {
	// //==================================================================
	// // Gather Segment Container ...
	// //==================================================================
	const xAOD::MuonSegmentContainer* SegmentContainer = nullptr;
	if ( ( collectionName == "Muons" && evtStore()->retrieve(SegmentContainer,"MuonSegments").isFailure() ) ){
		ATH_MSG_WARNING( "Unable to retrieve reconstructed segms!" );
		return false;
	}

	//Gather IDTrack Container from StoreGate
	const xAOD::TrackParticleContainer* IDTrackContainer;
	if(  evtStore()->retrieve(IDTrackContainer, m_InnerTrackContainer).isFailure() || !IDTrackContainer){
		ATH_MSG_WARNING( "Could not retrieve ID track container!" );
		return false;
	}
	// /** get the container of the original muons - */
	const xAOD::MuonContainer* MuonJar=0;
	if ( evtStore()->retrieve(MuonJar, collectionName).isFailure() || MuonJar == 0 ) {
		ATH_MSG_WARNING("No muon container found in TDS " << collectionName);
		return false;
	}

	ATH_MSG_DEBUG( "Number of Muons Before Cuts " << MuonJar->size() << "m_MuonCollectionFlag   " << collectionName);

	//Continue to the Next Event if there are no muons.
	if ( MuonJar->empty() ) return true;

	for (const auto muon: *MuonJar){
		handleMuon(muon, MuonJar, IDTrackContainer, SegmentContainer);
	}

	return true;
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::handleMuon( const xAOD::Muon* muon, const xAOD::MuonContainer* MuonJar,
									   const xAOD::TrackParticleContainer* IDTrackContainer, const xAOD::MuonSegmentContainer* SegmentContainer){
	// ATH_MSG_DEBUG("Beginning Muon Selection...");
	const xAOD::TrackParticle* InDetTrackParticle = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	if (!InDetTrackParticle) return;

	const xAOD::TrackParticle* MSTrackParticle = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);

	if (!MSTrackParticle) return;

	m_oOccupancyPlots.fill(muon, passIDTrackQuality(InDetTrackParticle), passMSTrackQuality(muon,3,3,3,9));

	if( !passPtCut( muon->pt() ) ) return;

	ATH_MSG_DEBUG("Muon Selection Complete... Pass");
	fillT0Plots(SegmentContainer, MSTrackParticle, muon, m_environment != AthenaMonManager::online);

	if (m_environment == AthenaMonManager::online) return;
	//TODO make the signatures more consistent (use reference if possible, not pointer)
	fillHistograms_DetSpcfc(muon);
	fillHistograms_GenTracking(muon);
	fillHistograms_CbMSIDAlign(muon, MuonJar);
	fillHistograms_ZSignal(muon, MuonJar, IDTrackContainer);
}

//---------------------------------------------------------------------------------------
StatusCode MuonTrkPhysMonitoring::finalize()
{
	ATH_CHECK(ManagedMonitorToolBase::finalize());
	ATH_MSG_DEBUG("MuonTrkPhysMonitoring::finalize() ");
	return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode MuonTrkPhysMonitoring::setupTools()
//---------------------------------------------------------------------------------------
{
	//initializing tools
	ATH_CHECK( m_pullCalculator.retrieve() );
	ATH_CHECK( m_muonHitSummaryTool.retrieve() );
	ATH_CHECK_FATAL( m_helperTool.retrieve() );
	ATH_CHECK_FATAL( m_idHelperTool.retrieve() );

	return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------------------
//My Tools
//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::passIDTrackQuality(const xAOD::TrackParticle *trackParticle)
{
	// TODO this can be replaced with usage of selector tool
	if(RetrieveHitInfo(trackParticle, xAOD::expectBLayerHit) && RetrieveHitInfo(trackParticle, xAOD::numberOfBLayerHits) == 0) return false;
	if(RetrieveHitInfo(trackParticle, xAOD::numberOfPixelHits) + RetrieveHitInfo(trackParticle, xAOD::numberOfPixelDeadSensors) <= 1) return false;
	if(RetrieveHitInfo(trackParticle, xAOD::numberOfSCTHits) + RetrieveHitInfo(trackParticle, xAOD::numberOfSCTDeadSensors) <= 5) return false;
	if(RetrieveHitInfo(trackParticle, xAOD::numberOfPixelHoles) + RetrieveHitInfo(trackParticle, xAOD::numberOfSCTHoles) > 3) return false;

	int numTRTHits      = RetrieveHitInfo(trackParticle, xAOD::numberOfTRTHits);
	int numTRTOutliers  = RetrieveHitInfo(trackParticle, xAOD::numberOfTRTOutliers);
	int numTRTSum       = numTRTHits + numTRTOutliers;

	if(fabs(trackParticle->eta()) < 1.9){
		if(numTRTSum <=5 || numTRTOutliers >= 0.9*numTRTSum) return false;
	}
	else{
		if(numTRTSum > 5 && numTRTOutliers >= 0.9*numTRTSum) return false;
	}

	ATH_MSG_DEBUG( "ID...Pass");
	return true;
}

//---------------------------------------------------------------------------------------
//bool MuonTrkPhysMonitoring::passMSTrackQuality(const xAOD::TrackParticle *MSTrackParticle, int HitReqI, int HitReqM, int HitReqO, int HitReqTotal)
bool MuonTrkPhysMonitoring::passMSTrackQuality(const xAOD::Muon* muon, int HitReqI, int HitReqM, int HitReqO, int HitReqTotal)
{
	// // TODO this can be replaced with usage of selector tool

       int innerHits = int(RetrieveHitInfo(muon, xAOD::innerSmallHits)+RetrieveHitInfo(muon, xAOD::innerLargeHits));
       int middleHits = int(RetrieveHitInfo(muon, xAOD::middleSmallHits)+RetrieveHitInfo(muon, xAOD::middleLargeHits));
       int outerHits = int(RetrieveHitInfo(muon, xAOD::outerSmallHits)+RetrieveHitInfo(muon, xAOD::outerLargeHits));

	if (innerHits < HitReqI) return false;
	if (middleHits < HitReqM) return false;
	if (outerHits < HitReqO) return false;

	if (innerHits+middleHits+outerHits < HitReqTotal) return false;
	return true;
}

//---------------------------------------------------------------------------------------

bool MuonTrkPhysMonitoring::passPtCut( float pt ) {
	return 0.001*pt > m_GlobalPtCut_pp;
}
//---------------------------------------------------------------------------------------

bool MuonTrkPhysMonitoring::passTrigger() {
	//   if(!m_useTrigger) return true;
	//   if (m_muon_triggers.size() == 0 || m_trigDecTool.empty()) return false;
	//   for (unsigned int ii = 0; ii < m_muon_triggers.size(); ii++ ) {
	//     ATH_MSG_DEBUG(MSG::DEBUG << "Checking trigger " << m_muon_triggers[ii]);
	//     // TODO -> deprecated, replace this with newer version
	//   if( m_trigDecTool->isPassed(m_muon_triggers[ii]) ){
	//     ATH_MSG_DEBUG("Fired trigger "<< m_muon_triggers[ii]);
	//     return true;
	//   }
	// }
	//   ATH_MSG_DEBUG("No Trigger Fired!");
	return true;
}
