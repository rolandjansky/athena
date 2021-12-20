/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_Digitization/AFP_PileUpTool.h"
#include <algorithm>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <map>
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandPoissonQ.h"
//#include "AFP_G4_SD/AFP_SensitiveDetector.h" 

AFP_PileUpTool::AFP_PileUpTool(const std::string& type,
                const std::string& name,
                const IInterface* parent) :
    PileUpToolBase(type, name, parent),
    m_totToChargeTransformation ("totToChargeTransformation", "1909 + x*363 + x*x*141"),
    m_mergeSvc    ("PileUpMergeSvc", name), 
    m_atRndmGenSvc("AtRndmGenSvc", name),
    m_rndEngine(0),
    m_CollectionEff(0.6),       // Collection efficincy
    m_ConversionSpr(40.0),      // Photon-Electron conversion spread in ps
    m_RiseTime(400.),           // Pulse rise time in ps
    m_FallTime(1200.),          // Pulse fall time in ps 
    m_TofSignalTimeRangeLength(2000.), // in ps
    m_TimeOffset(105500.),      // Offset reflecting time that proton travels from IP to AFP
    m_CfSignalDelay(200.),      // Pulse delay for const. frac. discrimination
    m_CfdThr(0.5),              // Constant fraction threshold
    m_SiT_ChargeCollEff(0.55),   // CCE, adjusted to describe data
    m_SiT_ChargeCollEffSigma(0.2), // sigma(CCE), adjusted to describe data
    m_SiT_NoiseMu(160),         // unit = number of eh pairs
    m_SiT_NoiseSigma(10),       // unit = number of eh pairs
    m_SiT_Energy2ChargeFactor(1000000./3.6), // number of eh pairs per MeV
    m_SiT_ToTThresholdForHit(1),
    m_ArrSize(645120)
{
    declareInterface<AFP_PileUpTool>(this); //Temporary for back-compatibility with 17.3.X.Y
    //declareInterface<IPileUpTool>(this); //Temporary for back-compatibility with 17.3.X.Y

    // Quantum efficincy of PMT in 100 nm steps int(WeaveLength-100)/100
    m_QuantumEff_PMT[0] = 0.00;
    m_QuantumEff_PMT[1] = 0.15;
    m_QuantumEff_PMT[2] = 0.20;
    m_QuantumEff_PMT[3] = 0.15;
    m_QuantumEff_PMT[4] = 0.05;
    m_QuantumEff_PMT[5] = 0.01;
    m_QuantumEff_PMT[6] = 0.00;
    
    // Offsets for TOF TDC (to have the average TDC consistent between all trains and bars)
    for(int i=0; i<4; ++i){
        for(int j=0; j<4; ++j){
            for(int k=0; k<4; ++k){
                m_TDC_offsets[i][j][k] = 0.0;
            }
        }
    }
    m_TDC_offsets[0][0][0] = -54.246796;
    m_TDC_offsets[0][0][1] = -62.727692;
    m_TDC_offsets[0][0][2] = -72.272064;
    m_TDC_offsets[0][0][3] = -70.434174;
    m_TDC_offsets[0][1][0] = 27.556335;
    m_TDC_offsets[0][1][1] = 27.173157;
    m_TDC_offsets[0][1][2] = 15.687866;
    m_TDC_offsets[0][1][3] = 1.523438;
    m_TDC_offsets[0][2][0] = 52.387573;
    m_TDC_offsets[0][2][1] = 55.950256;
    m_TDC_offsets[0][2][2] = 58.279297;
    m_TDC_offsets[0][2][3] = 52.943115;
    m_TDC_offsets[0][3][0] = 102.818848;
    m_TDC_offsets[0][3][1] = 100.652344;
    m_TDC_offsets[0][3][2] = 95.852905;
    m_TDC_offsets[0][3][3] = 89.628296;
    m_TDC_offsets[3][0][0] = -1.629608;
    m_TDC_offsets[3][0][1] = -1.427582;
    m_TDC_offsets[3][0][2] = -8.480835;
    m_TDC_offsets[3][0][3] = -17.730377;
    m_TDC_offsets[3][1][0] = 41.865295;
    m_TDC_offsets[3][1][1] = 50.760559;
    m_TDC_offsets[3][1][2] = 62.929993;
    m_TDC_offsets[3][1][3] = 58.732971;
    m_TDC_offsets[3][2][0] = 67.860107;
    m_TDC_offsets[3][2][1] = 66.973755;
    m_TDC_offsets[3][2][2] = 67.798340;
    m_TDC_offsets[3][2][3] = 68.117615;
    m_TDC_offsets[3][3][0] = 106.085693;
    m_TDC_offsets[3][3][1] = 102.385132;
    m_TDC_offsets[3][3][2] = 99.008240;
    m_TDC_offsets[3][3][3] = 89.424988;
    
    for(int i=0; i<16; ++i)
        m_ChargeVsTot_LUT[i] = m_totToChargeTransformation.Eval(i);
    
    // NOTE: The following variables are actually re-initialized by AFP_DigiTop::initialize() or AFP_PileUpTool::initialize()
    
    m_digitCollection = 0; // initializing to null pointer
    m_SiDigiCollection = 0;

    // end of ToolBox variables

    m_TDSimHitCollectionName = "AFP_TDSimHitCollection";
    m_TDDigiCollectionName    = "AFP_TDDigiCollection";

    m_SIDSimHitCollectionName = "AFP_SIDSimHitCollection";
    m_SiDigiCollectionName    = "AFP_SiDigiCollection";


    m_AFPSiHitsContainerName = "AFPSiHitContainer";
    m_AFPHitsContainerNameToF = "AFPToFHitContainer";

    m_mergedTDSimHitList = 0; // initialized to null pointer
    m_mergedSIDSimHitList = 0;  

    declareProperty( "AFPSiHitsContainerName", m_AFPSiHitsContainerName = "AFPSiHitContainer" );
    declareProperty( "AFPHitsContainerNameToF", m_AFPHitsContainerNameToF = "AFPToFHitContainer" );

    declareProperty("TDSimHitCollection" , m_TDSimHitCollectionName, "Name of the input Collection of the simulated TD Hits");
    declareProperty("TDDigiCollectionName", m_TDDigiCollectionName,    "Name of the Collection to hold the output from the AFP digitization, TD part");

    declareProperty("SIDSimHitCollection" , m_SIDSimHitCollectionName, "Name of the input Collection of the simulated SiD hits");
    declareProperty("SiDigiCollectionName", m_SiDigiCollectionName,    "Name of the Collection to hold the output from the AFP digitization, SiD part");

    declareProperty("RndmSvc",                m_atRndmGenSvc,              "Random Number Service used in AFP digitization" );
    declareProperty("mergeSvc",               m_mergeSvc,                  "Store to hold the pile-ups");

    declareProperty("CollectionEff", m_CollectionEff);
    declareProperty("ConversionSpr", m_ConversionSpr);
    declareProperty("FallTime", m_FallTime);
    declareProperty("RiseTime", m_RiseTime);
    declareProperty("TofSignalTimeRangeLength", m_TofSignalTimeRangeLength);
    declareProperty("CfSignalDelay", m_CfSignalDelay);
    declareProperty("CfdThr", m_CfdThr);
    declareProperty("SiT_ChargeCollEff", m_SiT_ChargeCollEff);
    declareProperty("SiT_ChargeCollEffSigma", m_SiT_ChargeCollEffSigma);
    declareProperty("SiT_NoiseMu", m_SiT_NoiseMu);
    declareProperty("SiT_NoiseSigma", m_SiT_NoiseSigma);
    declareProperty("SiT_Energy2ChargeFactor", m_SiT_Energy2ChargeFactor);
    declareProperty("SiT_ChargeThresholdForHit", m_SiT_ToTThresholdForHit);

    m_deposited_charge = new float[m_ArrSize]{};
    m_deposited_energy = new float[m_ArrSize]{};
    
    m_SignalVect = std::vector<double>( 2*static_cast<unsigned int>( m_TofSignalTimeRangeLength ) );
    for(unsigned int i=0; i<m_SignalVect.size(); ++i)
    {
        m_SignalVect[i] = SignalFun( i+0.5, m_RiseTime, m_FallTime);
    }
    
    for( int i=0; i<4; i++) {
        for( int j=0; j<4; j++) {
            for( int k=0; k<4; k++) {
                // NOTE the signal histograms and the methods processing those signals assume that 1 bin = 1 ps
                m_SignalHist[i][j][k] = TH1F(Form("m_SignalHist%d%d%d", i, j, k), "", m_TofSignalTimeRangeLength, 0, m_TofSignalTimeRangeLength);
            }
        }
    }

}


AFP_PileUpTool::~AFP_PileUpTool(){
    delete [] m_deposited_charge;
    delete [] m_deposited_energy;
}

StatusCode AFP_PileUpTool::initialize()
{
    ATH_MSG_DEBUG ( "AFP_PileUpTool::initialize() called" );
    ATH_MSG_DEBUG ( "    CollectionEff: " << m_CollectionEff << endreq
    //		 << " ScalePixel: " << m_ScalePixel << endreq
                );

    if (m_atRndmGenSvc.retrieve().isFailure()) { 
        ATH_MSG_FATAL ( "Could not retrieve RandomNumber Service!" ); 
        return StatusCode::FAILURE; 
    }
    else { 
        ATH_MSG_DEBUG ( "Retrieved RandomNumber Service" ); 
    }

    m_mergedTDSimHitList = new AFP_TDSimHitCollection("mergedTDSimHitList");
    m_mergedSIDSimHitList = new AFP_SIDSimHitCollection("mergedSIDSimHitList");

    return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::recoAll()
{
    ATH_CHECK( recoSiHits() );
    ATH_CHECK( recoToFHits() );

    return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::recoSiHits()
{
    ATH_MSG_DEBUG("AFP_PileUpTool recoSiHits ");

    // create output containers
    m_SiHitCollection = new xAOD::AFPSiHitContainer();
    ATH_CHECK( evtStore()->record(m_SiHitCollection, m_AFPSiHitsContainerName) );
    xAOD::AFPSiHitAuxContainer* siHitAuxContainer = new xAOD::AFPSiHitAuxContainer();
    ATH_CHECK( evtStore()->record(siHitAuxContainer, m_AFPSiHitsContainerName + "Aux.") );
    m_SiHitCollection->setStore(siHitAuxContainer);

    // retrieve digi data
    const AFP_SiDigiCollection  *container = nullptr;

    ATH_CHECK( evtStore()->retrieve(container, m_SiDigiCollectionName) );
    //  if (evtStore()->retrieve(container, m_SiDigiCollectionName).isFailure()) {
    //    ATH_MSG_WARNING("AFP_PileUpTool: Could not find simulated digi container");
    //    return StatusCode::SUCCESS;
    //  }
    //  else
    ATH_MSG_DEBUG("AFP_PileUpTool: Simulated digi container retrieved");

    newXAODHitSi (m_SiHitCollection, container);

    return StatusCode::SUCCESS;
}


void  AFP_PileUpTool::newXAODHitSi (xAOD::AFPSiHitContainer* siHitContainer, const AFP_SiDigiCollection* container) const
{
    AFP_SiDigiConstIter it    = container->begin();
    AFP_SiDigiConstIter itend = container->end();
    
    for (; it != itend; it++) {
        xAOD::AFPSiHit* xAODSiHit = new xAOD::AFPSiHit();
        siHitContainer->push_back(xAODSiHit);
        xAODSiHit->setStationID(it->m_nStationID);
        xAODSiHit->setPixelLayerID(it->m_nDetectorID);
        xAODSiHit->setPixelColIDChip(80-it->m_nPixelRow); // Chip is rotated by 90 degree Row-->Col
        xAODSiHit->setPixelRowIDChip(336-it->m_nPixelCol); // Chip	is rotated by 90 degree	Col-->Row
        xAODSiHit->setDepositedCharge( it->m_fADC );
        int tot = charge2tot( it->m_fADC );
        tot = tot<16 ? tot : 16;
        xAODSiHit->setTimeOverThreshold( tot );
    }
    ATH_MSG_DEBUG("AFP_PileUpTool:  Filled xAOD::AFPSiHit");
}


StatusCode AFP_PileUpTool::recoToFHits()
{
    ATH_MSG_DEBUG("AFP_PileUpTool recoToFHits ");

    // create output containers
    m_TDHitCollection   = new xAOD::AFPToFHitContainer();
    ATH_CHECK( evtStore()->record(m_TDHitCollection, m_AFPHitsContainerNameToF) );
    xAOD::AFPToFHitAuxContainer* tofHitAuxContainer = new xAOD::AFPToFHitAuxContainer();
    ATH_CHECK( evtStore()->record(tofHitAuxContainer, m_AFPHitsContainerNameToF + "Aux.") );
    m_TDHitCollection->setStore(tofHitAuxContainer);

    // retrieve digi data
    const AFP_TDDigiCollection  *container = nullptr;
    if (evtStore()->retrieve(container, m_TDDigiCollectionName).isFailure()) {
        ATH_MSG_WARNING("AFP_PileUpTool: Could not find simulated digi container");
        return StatusCode::SUCCESS;
    }
    else
        ATH_MSG_DEBUG("AFP_PileUpTool: Simulated digi container retrieved");

    newXAODHitToF (m_TDHitCollection, container);

    return StatusCode::SUCCESS;
}


void  AFP_PileUpTool::newXAODHitToF (xAOD::AFPToFHitContainer* tofHitContainer, const AFP_TDDigiCollection* container) const
{
    AFP_TDDigiConstIter it    = container->begin();
    AFP_TDDigiConstIter itend = container->end();

    for (; it != itend; it++) {
        xAOD::AFPToFHit* xAODToFHit = new xAOD::AFPToFHit();
        tofHitContainer->push_back(xAODToFHit);
        xAODToFHit->setStationID(it->m_nStationID);
        xAODToFHit->setHptdcChannel(-1);
        xAODToFHit->setBarInTrainID(it->m_nDetectorID%10-1);
        xAODToFHit->setTrainID(it->m_nDetectorID/10-1);
        xAODToFHit->setHptdcID(-1);
        xAODToFHit->setPulseLength(it->m_fADC);
        xAODToFHit->setTime(it->m_fTDC);
    }
    ATH_MSG_DEBUG("AFP_PileUpTool:  Filled xAOD::AFPToFHit");
}


StatusCode AFP_PileUpTool::processAllSubEvents(){

    ATH_MSG_DEBUG ( "AFP_PileUpTool::processAllSubEvents()" );

    //retrieve the PileUpMergeSvc if necessary
    if(!m_mergeSvc) {
        if(!m_mergeSvc.retrieve().isSuccess() || !m_mergeSvc) {
        ATH_MSG_FATAL("digitize: Could not find PileUpMergeSvc");
        return StatusCode::FAILURE;
        }
        else ATH_MSG_DEBUG("digitize: retrieved PileUpMergeSvc");
    }
    else ATH_MSG_DEBUG("digitize: PileUpMergeSvc already available"); 

    typedef PileUpMergeSvc::TimedList<AFP_TDSimHitCollection>::type TimedTDSimHitCollList;
    typedef PileUpMergeSvc::TimedList<AFP_SIDSimHitCollection>::type TimedSIDSimHitCollList;  

    TimedTDSimHitCollList TDSimHitCollList;
    TimedSIDSimHitCollList SIDSimHitCollList;  

    if (m_mergeSvc->retrieveSubEvtsData(m_TDSimHitCollectionName, TDSimHitCollList).isFailure()) { 


        ATH_MSG_FATAL ( "Could not fill TimedTDSimHitCollList" ); return StatusCode::FAILURE; 
    }
    else { ATH_MSG_DEBUG ( "Retrieved TimedTDSimHitCollList" ); }

    ATH_MSG_DEBUG ( " PileUp: Merge " << TDSimHitCollList.size() << " AFP_TDSimHitCollection with key " << m_TDSimHitCollectionName );


    if (m_mergeSvc->retrieveSubEvtsData(m_SIDSimHitCollectionName, SIDSimHitCollList).isFailure()) { 


        ATH_MSG_FATAL ( "Could not fill TimedSIDSimHitCollList" ); return StatusCode::FAILURE; 
    }
    else { ATH_MSG_DEBUG ( "Retrieved TimedSIDSimHitCollList" ); }

    ATH_MSG_DEBUG ( " PileUp: Merge " << SIDSimHitCollList.size() << " AFP_SIDSimHitCollection with key " << m_SIDSimHitCollectionName );


    TimedHitCollection<AFP_TDSimHit> thpcAFP_TDPmt;

    TimedTDSimHitCollList::iterator iColl  (TDSimHitCollList.begin());
    TimedTDSimHitCollList::iterator endColl(TDSimHitCollList.end());

    while (iColl != endColl) {
        
        const AFP_TDSimHitCollection* tmpColl(iColl->second);
        thpcAFP_TDPmt.insert(iColl->first, tmpColl);
        ATH_MSG_DEBUG ( " AFP_TDSimHitCollection found with " << tmpColl->size() << " hits " << iColl->first );
        
        ++iColl;
    }

    TimedHitCollection<AFP_SIDSimHit> thpcAFP_SiPmt;

    TimedSIDSimHitCollList::iterator iSiColl  (SIDSimHitCollList.begin());
    TimedSIDSimHitCollList::iterator endSiColl(SIDSimHitCollList.end());

    while (iSiColl != endSiColl) {
        
        const AFP_SIDSimHitCollection* tmpSiColl(iSiColl->second);
        thpcAFP_SiPmt.insert(iSiColl->first, tmpSiColl);
        ATH_MSG_DEBUG ( " AFP_SIDSimHitCollection found with " << tmpSiColl->size() << " hits " << iSiColl->first );
        
        ++iSiColl;
    }

    m_rndEngine = m_atRndmGenSvc->GetEngine("AFPRndEng");

    if (recordContainers(this->evtStore(), m_TDDigiCollectionName).isFailure()){
        ATH_MSG_FATAL ( " AFP DigiTop :: Could not record the empty TiD digit container in StoreGate " ); return StatusCode::FAILURE; 
    }
    else { ATH_MSG_DEBUG ( " AFP DigiTop :: TiD digit container is recorded in StoreGate " ); }


    if (recordSiCollection(this->evtStore(), m_SiDigiCollectionName).isFailure()){
        ATH_MSG_FATAL ( " AFP DigiTop :: Could not record the empty SiD digit container in StoreGate " ); return StatusCode::FAILURE; 
    }
    else { ATH_MSG_DEBUG ( " AFP DigiTop :: SiD digit container is recorded in StoreGate " ); }

    fillTDDigiCollection(thpcAFP_TDPmt, m_rndEngine);
    fillSiDigiCollection(thpcAFP_SiPmt);  

    ATH_CHECK( recoAll() );

    return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::prepareEvent(const unsigned int nInputEvents){

    ATH_MSG_DEBUG ( "AFP_PileUpTool::prepareEvent() called for " << nInputEvents << " input events" );

    StatusCode sc = recordContainers(this->evtStore(), m_TDDigiCollectionName);

    if (sc.isFailure()) { ATH_MSG_FATAL ( " Could not record the empty TiD digit container in StoreGate " ); return sc; }
    else                { ATH_MSG_DEBUG ( " TiD digit container is recorded in StoreGate " ); }

    m_mergedTDSimHitList->clear();

    sc = recordSiCollection(this->evtStore(), m_SiDigiCollectionName);

    if (sc.isFailure()) { ATH_MSG_FATAL ( " Could not record the empty SiD digit container in StoreGate " ); return sc; }
    else                { ATH_MSG_DEBUG ( " SiD digit container is recorded in StoreGate " ); }

    m_mergedSIDSimHitList->clear();

    return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::processBunchXing(int bunchXing,
                                                SubEventIterator bSubEvents,
                                                SubEventIterator eSubEvents) {
    ATH_MSG_DEBUG ( "AFP_PileUpTool::processBunchXing() " << bunchXing );
    SubEventIterator iEvt = bSubEvents;
    for (; iEvt!=eSubEvents; iEvt++) {
        StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
        //PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());   // does not do anything
        ATH_MSG_VERBOSE ("SubEvt StoreGate " << seStore.name() << " :"
                        << " bunch crossing : " << bunchXing
                        << " time offset : " << iEvt->time()
                        << " event number : " << iEvt->ptr()->eventNumber()
                        << " run number : " << iEvt->ptr()->runNumber()
                        );

        const AFP_TDSimHitCollection* tmpColl = 0;

        if (!seStore.retrieve(tmpColl, m_TDSimHitCollectionName).isSuccess()) {
        
        ATH_MSG_ERROR ( "SubEvent AFP_TDSimHitCollection not found in StoreGate " << seStore.name() );
        
        return StatusCode::FAILURE;
        }
        
        ATH_MSG_DEBUG ( "AFP_TDSimHitCollection found with " << tmpColl->size() << " hits" );
        
        AFP_TDSimHitCollection::const_iterator iPmt = tmpColl->begin();
        AFP_TDSimHitCollection::const_iterator ePmt = tmpColl->end();

        for (; iPmt!=ePmt; ++iPmt) m_mergedTDSimHitList->push_back((*iPmt));


        const AFP_SIDSimHitCollection* tmpSiColl = 0;

        if (!seStore.retrieve(tmpSiColl, m_SIDSimHitCollectionName).isSuccess()) {
        
        ATH_MSG_ERROR ( "SubEvent AFP_SIDSimHitCollection not found in StoreGate " << seStore.name() );
        
        return StatusCode::FAILURE;
        }
        
        ATH_MSG_DEBUG ( "AFP_TDSimHitCollection found with " << tmpSiColl->size() << " hits" );
        
        AFP_SIDSimHitCollection::const_iterator iSiPmt = tmpSiColl->begin();
        AFP_SIDSimHitCollection::const_iterator eSiPmt = tmpSiColl->end();

        for (; iSiPmt!=eSiPmt; ++iSiPmt) m_mergedSIDSimHitList->push_back((*iSiPmt));
        
        
        m_rndEngine = m_atRndmGenSvc->GetEngine("AFPRndEng");

    }

    return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::mergeEvent()
{
    fillTDDigiCollection(m_mergedTDSimHitList, m_rndEngine);
    fillSiDigiCollection(m_mergedSIDSimHitList);

    return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::finalize()
{
    return StatusCode::SUCCESS;
}


void AFP_PileUpTool::resetSignalHistograms(){
    for( int i=0; i<4; i++) {
        for( int j=0; j<4; j++) {
            for( int k=0; k<4; k++) {
                m_SignalHist[i][j][k].Reset();
            }
        }
    }
}


void AFP_PileUpTool::fillTDDigiCollection(TimedHitCollection<AFP_TDSimHit>& thpcAFP, CLHEP::HepRandomEngine* rndEngine) 
{
    resetSignalHistograms();
    
    TimedHitCollection<AFP_TDSimHit> thpc = thpcAFP;
    TimedHitCollection<AFP_TDSimHit>::const_iterator i, e, it;
    
    while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
        int Station     = (*it)->m_nStationID;
        int Detector    = (*it)->m_nDetectorID;
        int SensitiveElement = (*it)->m_nSensitiveElementID;
        float GlobalTime     =  (*it)->m_fGlobalTime;
        float WaveLength = (*it)->m_fWaveLength;

        if(SensitiveElement%2 == 1) createTDDigi(Station, Detector, SensitiveElement, GlobalTime, WaveLength, rndEngine);
    }
    
    StoreTDDigi();
}

void AFP_PileUpTool::fillTDDigiCollection(const AFP_TDSimHitCollection* AFP_TDSimHitCollection, CLHEP::HepRandomEngine* rndEngine)
{

    resetSignalHistograms();

    AFP_TDSimHitConstIter it    = AFP_TDSimHitCollection->begin();
    AFP_TDSimHitConstIter itend = AFP_TDSimHitCollection->end();

    for (; it != itend; it++) {
        int Station     = it->m_nStationID;
        int Detector = it->m_nDetectorID;
        int SensitiveElement = it->m_nSensitiveElementID;
        float GlobalTime     =  it->m_fGlobalTime;
        float WaveLength = it->m_fWaveLength;

        if(SensitiveElement%2 == 1) createTDDigi(Station, Detector, SensitiveElement, GlobalTime, WaveLength, rndEngine);
    }
    
    StoreTDDigi();
}


void AFP_PileUpTool::fillSiDigiCollection(TimedHitCollection<AFP_SIDSimHit>& thpcAFP) 
{
    TimedHitCollection<AFP_SIDSimHit>           thpc = thpcAFP;
    TimedHitCollection<AFP_SIDSimHit>::const_iterator i, e, it;

    long index = 0;
    while ( index != m_ArrSize ) // here just 6 layers per detector are considered
    {
                m_deposited_charge[index++] = 0;
    } 

    while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {
        
        int Station     = (*it)->m_nStationID;
        int Detector    = (*it)->m_nDetectorID;
        int PixelRow    = (*it)->m_nPixelRow;
        int PixelCol    = (*it)->m_nPixelCol;
        float PreStepX  = (*it)->m_fPreStepX;
        float PreStepY  = (*it)->m_fPreStepY;
        float PreStepZ  = (*it)->m_fPreStepZ;
        float PostStepX = (*it)->m_fPostStepX;
        float PostStepY = (*it)->m_fPostStepY;
        float PostStepZ = (*it)->m_fPostStepZ;
        float DepEnergy = (*it)->m_fEnergyDeposit;

        createSiDigi(Station, Detector, PixelRow, PixelCol, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ,  DepEnergy);
    }
    
    StoreSiDigi();
}


void AFP_PileUpTool::fillSiDigiCollection(const AFP_SIDSimHitCollection* AFP_SIDSimHitCollection)
{
    AFP_SIDSimHitConstIter it    = AFP_SIDSimHitCollection->begin();
    AFP_SIDSimHitConstIter itend = AFP_SIDSimHitCollection->end();

    long index = 0;
    while ( index != m_ArrSize ) // here just 6 layers per detector are considered
    {
                m_deposited_charge[index++] = 0;
    } 

    for (; it != itend; it++) {
        int Station     = it->m_nStationID;
        int Detector    = it->m_nDetectorID;
        int PixelRow    = it->m_nPixelRow;
        int PixelCol    = it->m_nPixelCol;
        float PreStepX  = it->m_fPreStepX;
        float PreStepY  = it->m_fPreStepY;
        float PreStepZ  = it->m_fPreStepZ;
        float PostStepX = it->m_fPostStepX;
        float PostStepY = it->m_fPostStepY;
        float PostStepZ = it->m_fPostStepZ;
        float DepEnergy = it->m_fEnergyDeposit;

        createSiDigi(Station, Detector, PixelRow, PixelCol, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ, DepEnergy);
    }
    StoreSiDigi();
}



bool AFP_PileUpTool::isPhotoelectronInduced(double lambda, CLHEP::HepRandomEngine* rndEngine) const
{
    double qEff = getQE( lambda );
    return CLHEP::RandFlat::shoot(rndEngine, 0.0, 1.0) < qEff*m_CollectionEff;
}


void AFP_PileUpTool::addPhotoconvTimeSmear(double & t, CLHEP::HepRandomEngine* rndEngine) const
{
    t += CLHEP::RandGaussQ::shoot(rndEngine, 5.* m_ConversionSpr, m_ConversionSpr);
}


void AFP_PileUpTool::createTDDigi(int Station, int Detector, int SensitiveElement, float GlobalTime, float WaveLength, CLHEP::HepRandomEngine* rndEngine)
{
    ATH_MSG_DEBUG ( " iterating Pmt " << Station << "  " << Detector << "  " << SensitiveElement << " " << GlobalTime << WaveLength ); 

    if ( Station >3 || Station < 0 || Detector >49 || Detector < 10 || (SensitiveElement-1)/2>1 || (SensitiveElement-1)/2<0) {
        ATH_MSG_FATAL ( "Wrong station, detector or sensitive detector id" );
        return;
    }
    
    if( isPhotoelectronInduced(WaveLength, rndEngine) )
    {
        const int train = Detector/10 - 1;
        const int bar = Detector%10-1;
        
        double photoelectronTime = GlobalTime - m_TimeOffset - m_TDC_offsets[Station][train][bar];
        addPhotoconvTimeSmear( photoelectronTime, rndEngine );
        addSignalFunc( m_SignalHist[Station][train][bar], photoelectronTime );
    }
}


double AFP_PileUpTool::getTDC(const TH1F & hSignal) const
{
    const int nBinsDelay = static_cast<int>(m_CfSignalDelay); // assuming that 1 bin = 1 ps
    TH1F hSignal_delayed(hSignal);
    for(int l = hSignal.GetNbinsX(); l>0; l-- ){
        double val = l > nBinsDelay ? hSignal.GetBinContent(l-nBinsDelay) : 0;
        hSignal_delayed.SetBinContent(l, val);
    }
    TH1F hSignal_forTDC(hSignal);
    hSignal_forTDC.Add(&hSignal, &hSignal_delayed, -m_CfdThr, 1);
    
    const int bin = hSignal_forTDC.FindFirstBinAbove(0);
    double TDC = hSignal_forTDC.GetBinCenter( bin );
    if( bin-1 <= nBinsDelay ) // very erly signals 
        TDC = 0;
    return TDC;
}


void AFP_PileUpTool::StoreTDDigi(void)
{
    for( int i=0; i<4; i++) {
        for( int j=0; j<4; j++) {
            for( int k=0; k<4; k++){
                
                const TH1F & hSignal = m_SignalHist[i][j][k];
                const double ADC = hSignal.GetBinContent( hSignal.GetMaximumBin() );
                
                if( ADC > 0 ){

                    const double TDC = getTDC( hSignal );
                    
                    AFP_TDDigi* tddigi = new AFP_TDDigi ();
                    tddigi->m_nStationID=i;
                    tddigi->m_nDetectorID=10*(j+1)+k+1; // restoring original numeration of bars and trains
                    tddigi->m_nSensitiveElementID=-1; // this variable is currently redundant
                    tddigi->m_fADC=ADC;
                    tddigi->m_fTDC=TDC;

                    m_digitCollection->Insert(*tddigi);
                }
            }
        }
    }
}


void AFP_PileUpTool::createSiDigi(int Station, int Detector, int PixelRow, int PixelCol, float PreStepX, float PreStepY, float PreStepZ, float PostStepX, float PostStepY, float PostStepZ, float DepEnergy)
{
    ATH_MSG_DEBUG ( " iterating Pmt, station " << Station << ", detector " << Detector << ", pixel_col " << PixelCol << ", pixel_row " << PixelRow << ", dep_energy" << DepEnergy << " (x,y,z)_pre (" << PreStepX <<"," << PreStepY <<"," << PreStepZ <<"), (x,y,z)_post (" << PostStepX <<"," << PostStepY <<"," << PostStepZ <<")" ); 
    
    double cce = generateSiCCE();
    double depositedCharge = DepEnergy * cce * m_SiT_Energy2ChargeFactor;
    
    ATH_MSG_DEBUG ( "deposted charge for given hit " << depositedCharge );
    
    if ((PixelCol>=336) || (PixelRow >= 80) || (Station >= 4) || (Detector >= 6 ) )
    {
        if (Detector == 11) 
        {
            ATH_MSG_DEBUG ( "Hit in the vacuum layer in front of the station " << Station );
        }
        else
        {
            ATH_MSG_WARNING ( "WRONG NUMBER of PIXEL coordinates or station or detector !!!:" );
            ATH_MSG_WARNING ( "station [max 4] " << Station << ", detector [max 6]" << Detector << ", pixel_col [max 336] " << PixelCol << ", pixel_row [max 80] " << PixelRow );
        }
        return;
    }   
    
    m_deposited_charge[6*336*80*Station + 80*336*Detector + 80*PixelCol + PixelRow] += depositedCharge;
    m_deposited_energy[6*336*80*Station + 80*336*Detector + 80*PixelCol + PixelRow] += DepEnergy;
}

void AFP_PileUpTool::StoreSiDigi(void)
{
    long index = 0;
    //  const long indexend = (long)width * (long)height * (long)depth;
    while ( index != m_ArrSize ) // here just 6 layers per detector are considered
    {
        // adding random noise
        m_deposited_charge[index] += generateSiNoise();
        
        int tot = charge2tot( m_deposited_charge[index] );

        if (tot >= m_SiT_ToTThresholdForHit )
        {
            
            ATH_MSG_DEBUG ( " total # of pairs from dep_energy (with all effects included) " << m_deposited_charge[index]);
            ATH_MSG_DEBUG ( " total # of pairs from dep_energy (true value)" << m_SiT_Energy2ChargeFactor*m_deposited_energy[index]);
            
            int station  = (int)(index/(80*336*6)); 
            int detector = (int)((index-station*80*336*6)/(80*336)); 
            int column   = (int)((index-station*80*336*6-detector*80*336)/80);
            int row      = (int)(index-station*80*336*6-detector*80*336-column*80);
            
            ATH_MSG_DEBUG ( " reversed mapping, station " << station << ", detector " << detector << ", pixel_col " << column << ", pixel_row " << row ); 
            
            
            AFP_SiDigi* sidigi = new AFP_SiDigi();	
            
            sidigi->m_nStationID = station;
            sidigi->m_nDetectorID = detector;
            sidigi->m_nPixelCol = column;
            sidigi->m_nPixelRow = row;
            sidigi->m_fADC = tot2charge(tot);
            sidigi->m_fTDC = 0.;
            
            
            ATH_MSG_DEBUG ( " size: " << m_SiDigiCollection->size());
            
            m_SiDigiCollection->Insert(*sidigi); 
            //m_SiDigiCollection->push_back(sidigi); 

        }
        
        index++;	    
    }
}


double AFP_PileUpTool::SignalFun(double Time, double RiseTime, double FallTime, double offset) const
{
    double f=0;
    Time -= offset;
    if ( Time < 0) return f;
    double p = (FallTime-RiseTime*log(1.+FallTime/RiseTime))/log(10.);
    f = pow(Time/p,RiseTime/p)*exp(-(Time/p));
    f /= pow(RiseTime/p,RiseTime/p)*exp(-(RiseTime/p));
    return f;
}


void AFP_PileUpTool::addSignalFunc(TH1F & h, double x) const
{
    int xDiscrete = static_cast<int>(x);
    int iMin = xDiscrete;
    int iMax = h.GetNbinsX();
    if(xDiscrete<0)
    {
        iMin = 0;
        iMax = h.GetNbinsX() + xDiscrete - 1; 
    }
    for(int i = iMin; i<=iMax; ++i){
        h.SetBinContent(i, h.GetBinContent(i) + m_SignalVect[i-xDiscrete]);
    }
}


double AFP_PileUpTool::getQE(double lambda) const{
    int id = (static_cast<int>(lambda)-100)/100;
    if(id > 6 || id < 0) return 0;
    return m_QuantumEff_PMT[id];
}


StatusCode AFP_PileUpTool::recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string key_digitCnt) 
{
    m_digitCollection = new AFP_TDDigiCollection();

    StatusCode sc = evtStore->record(m_digitCollection, key_digitCnt);
    return sc;
}

StatusCode AFP_PileUpTool::recordSiCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_SidigitCnt) 
{
    m_SiDigiCollection = new AFP_SiDigiCollection();

    StatusCode sc = evtStore->record(m_SiDigiCollection, key_SidigitCnt);

    return sc;
    //return StatusCode::SUCCESS;
}
