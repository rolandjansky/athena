/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TrigEgammaRec.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEgammaRec

AUTHORS:  Teresa Fonseca Martin and Cibran Santamarina Rios
          Phillip Urquijo
          Alessandro Tricoli
          Ryan Mackenzie White (xAOD)

CREATED:  Jun 2006
MODIFIED: April 2014

PURPOSE:  Algorithm is an adaptation for the trigger of the egammaBuilder.cxx
	  algorithm from package offline/Reconstruction/egammaRec.
          Algorithm which makes a egammaObjectCollection for each Trigger Element (TE). 
	  For each cluster 
          create a new egamma object and fills it then in the egammaContainer.
	  The algorithm takes care to make the objects containing shower 
	  shape variables, leakage into had. calo, objects for track matching 
	  and eventually conversions (not yet available).

          It retrieves the CaloCluster, Cell and TrackParticle Containers 
	  from the TE. It modifies the corresponding properties of the offline tools
	  so they retrieve from StoreGate the corresponding Containers.

********************************************************************/

// INCLUDE HEADER FILES:

// Trigger specific files
#include "TrigEgammaRec/TrigEgammaRec.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

// Offline files:
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IEMBremCollectionBuilder.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "egammaInterfaces/IEMAmbiguityTool.h"

// xAOD
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "egammaRecEvent/egammaRecContainer.h"

#include "egammaEvent/egammaParamDefs.h"


// INCLUDE GAUDI HEADER FILES:
 
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

// needed for online monitor histograms
class ISvcLocator;

namespace {
    template <class DEST,class SRC>
        inline DEST** my_pp_cast(SRC** ptr) {
            return (DEST**)(ptr);
        }
}

/////////////////////////////////////////////////////////////////

// GETTER for Shower Shape monitoring
#define GETTER(_name_) float getShowerShape_##_name_(const xAOD::Egamma* eg) \
{ float val{-99}; \
    eg->showerShapeValue(val,xAOD::EgammaParameters::_name_); \
    return val; } 
GETTER(e237)
GETTER(e277)
GETTER(ethad1)    
GETTER(weta1)
GETTER(weta2)
GETTER(f1)
GETTER(e2tsts1) 
GETTER(emins1)
GETTER(wtots1)
GETTER(fracs1)    
#undef GETTER

// GETTER for Isolation monitoring
#define GETTER(_name_) float getIsolation_##_name_(const xAOD::Egamma* eg) \
{ float val{-99}; \
    eg->isolationValue(val,xAOD::EgammaParameters::_name_); \
    return val; } 
GETTER(etcone)
GETTER(etcone20)
GETTER(etcone30)
GETTER(etcone40)    
GETTER(ptcone20)
GETTER(ptcone30)
GETTER(ptcone40)    
#undef GETTER    

// GETTERs for CaloCluster monitoring   
#define GETTER(_name_) float getCluster_##_name_(const xAOD::Egamma* eg) \
{    if(eg && eg->caloCluster()) \
        return eg->caloCluster()->_name_(); \
    else return -99.;}
GETTER(et)
GETTER(phi)
GETTER(eta)   
#undef GETTER

// GETTERs for Track monitoring   
#define GETTER(_name_) float getTrack_##_name_(const xAOD::Electron* eg) \
{    if(eg && eg->trackParticle()) \
        return eg->trackParticle()->_name_(); \
    else return -99.;}
GETTER(pt)
GETTER(phi)
GETTER(eta)   
#undef GETTER
// GETTERs for Track details monitoring    
#define GETTER(_name_) float getTrackSummary_##_name_(const xAOD::Electron* eg) \
{ uint8_t val_uint8{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_uint8,xAOD::_name_); \
        return val_uint8; } \
    else return -99; }
GETTER(numberOfBLayerHits) 
GETTER(numberOfPixelHits)
GETTER(numberOfSCTHits)    
GETTER(numberOfTRTHits)
GETTER(numberOfTRTHighThresholdHits)
GETTER(numberOfTRTHighThresholdOutliers)
GETTER(numberOfTRTOutliers)
#undef GETTER

// GETTERs for Calo-Track monitoring
#define GETTER(_name_) float getCaloTrackMatch_##_name_(const xAOD::Electron* eg) \
{ float val={-99.}; \
    if(eg && eg->trackParticle()){ \
        eg->trackCaloMatchValue(val,xAOD::EgammaParameters::_name_);} \
    return val; }
GETTER(deltaEta0)
GETTER(deltaPhi0)
GETTER(deltaPhiRescaled0)    
GETTER(deltaEta1)
GETTER(deltaPhi1)
GETTER(deltaPhiRescaled1)    
GETTER(deltaEta2)
GETTER(deltaPhi2)
GETTER(deltaPhiRescaled2)    
GETTER(deltaEta3)
GETTER(deltaPhi3)
GETTER(deltaPhiRescaled3) 
#undef GETTER    

// Additional monitoring function   
//
    float EtCluster37(const xAOD::Egamma* eg){
        if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        float eta2   = fabs(cluster->etaBE(2)); 
        return cluster->e()/cosh(eta2);      
    }
    else return -99.;
}
float Eoverp(const xAOD::Electron* eg)	{
    if(eg && (eg->trackParticle()) && (eg->caloCluster())){
        return eg->caloCluster()->et()/ eg->trackParticle()->pt();   
    }
    else return -99.;
}
float rTRT  (const xAOD::Electron* eg) {
    if(eg && eg->trackParticle()){ 
        uint8_t trtHits   = 0;
        eg->trackParticleSummaryValue(trtHits,xAOD::numberOfTRTHits);
        uint8_t trtHTHits = 0; 
        eg->trackParticleSummaryValue(trtHTHits,xAOD::numberOfTRTHighThresholdHits);
        if(trtHits!=0) {
            return ( (double)trtHTHits / (double)trtHits ); 
        }
        else return -99.;
    }
    else return -99.;
}
//  CONSTRUCTOR:

TrigEgammaRec::TrigEgammaRec(const std::string& name,ISvcLocator* pSvcLocator):
    HLT::FexAlgo(name, pSvcLocator)
{

    // The following properties are specified at run-time
    // (declared in jobOptions file)

    declareProperty("ElectronContainerAliasSuffix", m_electronContainerAliasSuffix="egamma_electron");
    declareProperty("PhotonContainerAliasSuffix",   m_photonContainerAliasSuffix="egamma_photon");

    // ShowerBuilder (trigger specific)
    declareProperty("ShowerBuilderTool",  m_showerBuilder, "Handle to Shower Builder");
    // FourMomBuilder (trigger specific)
    declareProperty("FourMomBuilderTool", m_fourMomBuilder, "Handle to FourMomBuilder");
    // ConversionBuilder 
    declareProperty("ConversionBuilderTool", m_conversionBuilder, "Handle to ConversionBuilder");
    // TrackMatchBuilder
    declareProperty("TrackMatchBuilderTool", m_trackMatchBuilder, "Handle to TrackMatchBuilder");
    // AmbiguityTool
    declareProperty("AmbiguityTool", m_ambiguityTool, "Handle to AmbiguityTool");
    
    // Set flag for track matching
    declareProperty("doTrackMatching",m_doTrackMatching = false);
    // Set flag for conversions 
    declareProperty("doConversions",m_doConversions = false);

    // Monitoring
    typedef const DataVector<xAOD::Egamma> xAODEgammaDV_type;
    typedef const DataVector<xAOD::Electron> xAODElectronDV_type;
    typedef const DataVector<xAOD::Photon> xAODPhotonDV_type;
     // Showershape monitoring accesible from xAOD::Egamma (common to Electron / Photon)  
    declareMonitoredCollection("E237",	 *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_e237);
    declareMonitoredCollection("E277",	 *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_e277);
    declareMonitoredCollection("EtHad1", *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_ethad1);
    declareMonitoredCollection("WEta1",	 *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_weta1);
    declareMonitoredCollection("WEta2",	 *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_weta2);
    declareMonitoredCollection("F1",	 *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_f1);
    declareMonitoredCollection("E2tsts1",*my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_e2tsts1);
    declareMonitoredCollection("Emins1", *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_emins1);
    declareMonitoredCollection("wtots1", *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_wtots1);
    declareMonitoredCollection("Fracs1", *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getShowerShape_fracs1);
    
    declareMonitoredCollection("Ph_e237",	 *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_e237);
    declareMonitoredCollection("Ph_e277",	 *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_e277);
    declareMonitoredCollection("Ph_ethad1", *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_ethad1);
    declareMonitoredCollection("Ph_weta1",	 *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_weta1);
    declareMonitoredCollection("Ph_weta2",	 *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_weta2);
    declareMonitoredCollection("Ph_f1",	 *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_f1);
    declareMonitoredCollection("Ph_e2tsts1",*my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_e2tsts1);
    declareMonitoredCollection("Ph_emins1", *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_emins1);
    declareMonitoredCollection("Ph_wtots1", *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_wtots1);
    declareMonitoredCollection("Ph_fracs1", *my_pp_cast <xAODEgammaDV_type>(&m_photon_container), &getShowerShape_fracs1);
    
    declareMonitoredCollection("EtCluster37",	*my_pp_cast<xAODEgammaDV_type>(&m_electron_container), &EtCluster37); 
    declareMonitoredCollection("EtConeIso",	*my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getIsolation_etcone);
    declareMonitoredCollection("Eta",	        *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getCluster_eta);
    declareMonitoredCollection("Phi",	        *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getCluster_phi);
    declareMonitoredCollection("ClusterEt",     *my_pp_cast <xAODEgammaDV_type>(&m_electron_container), &getCluster_et);

    //Track-related monitoring accesible from xAOD::Electron
    declareMonitoredCollection("nBLayerHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfBLayerHits);
    declareMonitoredCollection("nPixelHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfPixelHits);
    declareMonitoredCollection("nSCTHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfSCTHits);
    declareMonitoredCollection("nTRTHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTHits);
    declareMonitoredCollection("nTRTHitsHighTh",         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTHighThresholdHits);
    declareMonitoredCollection("nTRTHitsHighThOutliers", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTHighThresholdOutliers);
    declareMonitoredCollection("nTRTHitsOutliers",       *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTOutliers);
    declareMonitoredCollection("TrackPt",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrack_pt);
    declareMonitoredCollection("dEta",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCaloTrackMatch_deltaEta2);
    declareMonitoredCollection("dPhi",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCaloTrackMatch_deltaPhi2);
    declareMonitoredCollection("Eoverp",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &Eoverp);
    declareMonitoredCollection("rTRT",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &rTRT);

    //Vertex-related monitoring for Photons
    //TBD
}

// DESTRUCTOR:
  
TrigEgammaRec::~TrigEgammaRec()
{  }
////////////////////////////////////////////////////////////////////////////
//          beginRun method:


HLT::ErrorCode TrigEgammaRec::hltStart()
{
    if(msgLvl() <= MSG::INFO) msg() << MSG::INFO << "in Start()" << endreq;

    return HLT::OK;
}

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:

HLT::ErrorCode TrigEgammaRec::hltInitialize() {

    msg() << MSG::INFO << "in initialize()" << endreq;

    // Global timers
    if (timerSvc()) m_timerTotal = addTimer("Total");

    // Pointer to Tool Service
    IToolSvc* p_toolSvc = 0;
    if (service("ToolSvc", p_toolSvc).isFailure()) {
        msg() << MSG::FATAL << "REGTEST: Tool Service not found " << endreq;
        return HLT::BAD_JOB_SETUP;
    }

    if(m_doTrackMatching){
        if (m_trackMatchBuilder.empty()) {
            ATH_MSG_ERROR("EMTrackMatchBuilder is empty");
            return HLT::BAD_JOB_SETUP;
        }

        if((m_trackMatchBuilder.retrieve()).isFailure()) {
            ATH_MSG_ERROR("Unable to retrieve "<<m_trackMatchBuilder);
            return HLT::BAD_JOB_SETUP;
        }
        else{
            ATH_MSG_DEBUG("Retrieved Tool "<<m_trackMatchBuilder);
            if (timerSvc()) m_timerTool1 = addTimer("EMTrackMatchBuilder");
        }
    }

    if(m_doTrackMatching && m_doConversions){
        if (m_conversionBuilder.empty()) {
            ATH_MSG_ERROR("EMTrackMatchBuilder is empty");
            return HLT::BAD_JOB_SETUP;
        }

        if((m_conversionBuilder.retrieve()).isFailure()) {
            ATH_MSG_ERROR("Unable to retrieve "<<m_conversionBuilder);
            return HLT::BAD_JOB_SETUP;
        }
        else{
            ATH_MSG_DEBUG("Retrieved Tool "<<m_conversionBuilder);
            if (timerSvc()) m_timerTool2 = addTimer("EMConversionBuilder");
        }
    }
    if (m_ambiguityTool.empty()) {
        ATH_MSG_ERROR("EMAmbiguityTool is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_ambiguityTool.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_ambiguityTool);
        return HLT::BAD_JOB_SETUP;
    }
    else{
        ATH_MSG_DEBUG("Retrieved Tool "<<m_ambiguityTool);
        if (timerSvc()) m_timerTool3 = addTimer("EMAmbiguityTool");
    }
    
    if (m_fourMomBuilder.empty()) {
        ATH_MSG_ERROR("EMFourMomentumBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_fourMomBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_fourMomBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else{
        ATH_MSG_DEBUG("Retrieved Tool "<<m_fourMomBuilder);
        if (timerSvc()) m_timerTool4 = addTimer("EMFourMomBuilder");
    }

    if (m_showerBuilder.empty()) {
        ATH_MSG_ERROR("EMShowerBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_showerBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_showerBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_showerBuilder);
        if (timerSvc()) m_timerTool5 = addTimer("EMShowerBuilder");
    }
    

    ATH_MSG_DEBUG("Obtain CaloCellDetPos tool for calo-frame variables");
    m_caloCellDetPos = new CaloCellDetPos();

    //print summary info
    if(msgLvl() <= MSG::INFO) {
        msg() << MSG::INFO << "REGTEST: xAOD Reconstruction for Run2" << endreq;
        msg() << MSG::INFO << "REGTEST: Initialization completed successfully, tools initialized:  " << endreq;
        msg() << MSG::INFO << "REGTEST: TrackMatch tool: " << m_trackMatchBuilder << endreq;
        msg() << MSG::INFO << "REGTEST: Conversions tool: " << m_conversionBuilder << endreq;
        msg() << MSG::INFO << "REGTEST: Shower  tool: " << m_showerBuilder << endreq;
        msg() << MSG::INFO << "REGTEST: Four Momentum tool: " << m_fourMomBuilder << endreq; 
        msg() << MSG::INFO << "REGTEST: Ambiguity tool: " << m_ambiguityTool << endreq;

        if (m_electronContainerAliasSuffix.size() > 0 ) 
            msg() << MSG::VERBOSE << "REGTEST: suffix added to ElectronContainer alias: " << m_electronContainerAliasSuffix << endreq;
        if (m_photonContainerAliasSuffix.size() > 0 ) 
            msg() << MSG::VERBOSE << "REGTEST: suffix added to PhotonContainer alias: " << m_photonContainerAliasSuffix << endreq;
    }
    return HLT::OK;
}

/////////////////////////////////////////////////
// FINALIZE METHOD:

HLT::ErrorCode TrigEgammaRec::hltFinalize() {
    delete m_caloCellDetPos;
    if(msgLvl() <= MSG::INFO) msg() << MSG::INFO << "Finalization of TrigEgammaRec completed successfully"
        << endreq;
    

    return HLT::OK;
}
/////////////////////////////////////////////////////////////////
//          endRun method:

HLT::ErrorCode TrigEgammaRec::hltEndRun()
{
    if(msgLvl() <= MSG::INFO) msg() << MSG::INFO << "in endRun()" << endreq;
    return HLT::OK;
}


//////////////////////////////////////////////////

HLT::ErrorCode TrigEgammaRec::hltExecute( const HLT::TriggerElement* inputTE,
        HLT::TriggerElement* outputTE )
{
    if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "Executing HLT alg. TrigEgammaRec" << endreq;
        msg() << MSG::DEBUG << "inputTE->getId(): " << inputTE->getId() << endreq;
    } 
    // Time total TrigEgammaRec execution time.
    if (timerSvc()) m_timerTotal->start();

    //m_eg_container = 0;
    m_electron_container = 0;
    m_photon_container = 0;

    //**********************************************************************
    // Retrieve cluster container
    // get pointer to CaloClusterContainer from the trigger element
    std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer;

    HLT::ErrorCode stat = getFeatures(inputTE, vectorClusterContainer);
    if ( stat!= HLT::OK ) {
        msg() << MSG::ERROR << " REGTEST: No CaloClusterContainers retrieved for the trigger element" << endreq;
        return HLT::OK;
    }
    //debug message
    if ( msgLvl() <= MSG::VERBOSE)
        msg() << MSG::VERBOSE << " REGTEST: Got " << vectorClusterContainer.size()
            << " CaloClusterContainers associated to the TE " << endreq;

    // Check that there is only one ClusterContainer in the RoI
    if (vectorClusterContainer.size() != 1){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << "REGTEST: Size of vectorClusterContainer is not 1, it is: "
                << vectorClusterContainer.size()
                << endreq;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    // Get the last ClusterContainer
    const xAOD::CaloClusterContainer* clusContainer = vectorClusterContainer.back();
    if(!clusContainer){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << " REGTEST: Retrieval of CaloClusterContainer from vector failed"
                << endreq;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
        << clusContainer->size() << " calo clusters in container" << endreq;

    if(clusContainer->size() < 1){
        return HLT::OK;
    }

    //**********************************************************************
    // Create an EgammaRec container  
    // should this be a pointer in the class?
    EgammaRecContainer eg_container;

    // Create collections used in the navigation
    // Electrons
    m_electron_container = new xAOD::ElectronContainer();
    xAOD::ElectronAuxContainer *electronAux = new xAOD::ElectronAuxContainer();
    
    // Photons
    m_photon_container = new xAOD::PhotonContainer();
    xAOD::PhotonAuxContainer *photonAux = new xAOD::PhotonAuxContainer();

    // Get into egKey userfriendly key given by jobOptions property (if any)
    // Expanding to have separate Electron / Photon keys
    std::string electronKey="";
    std::string photonKey = "";
    if (m_electronContainerAliasSuffix.size() > 0 ) {
        electronKey = m_electronContainerAliasSuffix;
        if(msgLvl() <= MSG::VERBOSE) 
            msg() << MSG::VERBOSE << "REGTEST: electronKey for Electron container: " << electronKey << endreq;
    }
    if (m_photonContainerAliasSuffix.size() > 0 ) {
        photonKey = m_photonContainerAliasSuffix;
        if(msgLvl() <= MSG::VERBOSE) 
            msg() << MSG::VERBOSE << "REGTEST: egKey for egamma container: " << photonKey << endreq;
    }


    std::string electronContSGKey = "";
    std::string photonContSGKey = "";
    HLT::ErrorCode sc = getUniqueKey( m_electron_container, electronContSGKey, electronKey);
    if (sc != HLT::OK) { 
        msg() << MSG::DEBUG << "Could not retrieve the electron container key" << endreq;
        return sc;                                                                       
    } 
    sc = getUniqueKey( m_photon_container, photonContSGKey, photonKey);
    if (sc != HLT::OK) { 
        msg() << MSG::DEBUG << "Could not retrieve the photon container key" << endreq;
        return sc;                                                                       
    } 


    // Attach to TE and write to StoreGate (needed only for POOL persistency)
    //3rd argument:  always will return the name used to store the container
    //             if no empty as input will give this alias in SG to the container
    //4th argument: will add customs' key to container name
    
    std::string electronContSGName = electronContSGKey; //if you put no empty string here you'll get an alias in SG
    std::string photonContSGName = photonContSGKey; //if you put no empty string here you'll get an alias in SG
    if ( store()->record(m_electron_container, electronContSGKey).isFailure()) {
        msg() << MSG::ERROR << "REGTEST: trigger ElectronContainer registration failed" << endreq;
        return HLT::ERROR;
    }
    if ( store()->record(electronAux,electronContSGKey + "Aux.").isFailure() ) {
        msg() << MSG::ERROR << "REGTEST: trigger ElectronAuxContainer registration failed" << endreq;
        delete electronAux;
        return HLT::ERROR;
    }
    m_electron_container->setStore(electronAux);
    
    if ( store()->record(m_photon_container, photonContSGKey).isFailure()) {
        msg() << MSG::ERROR << "REGTEST: trigger PhotonContainer registration failed" << endreq;
        return HLT::ERROR;
    }
    if ( store()->record(photonAux, photonContSGKey + "Aux.").isFailure() ) {
        msg() << MSG::ERROR << "REGTEST: trigger PhotonAuxContainer registration failed" << endreq;
        delete photonAux;
        return HLT::ERROR;
    }
    m_photon_container->setStore(photonAux);
    //***************************************************************************************************************
    // Retrieve from TE containers needed for tool execution, set corresponding flag to be used in execution
    //***************************************************************************************************************

    // Shower Shape & CaloCellContainer
    const CaloCellContainer* pCaloCellContainer = 0;

    // Get vector of pointers to all CaloCellContainers from TE
    std::vector<const CaloCellContainer*> vectorCellContainer;

    stat = getFeatures(inputTE, vectorCellContainer);
    if ( stat != HLT::OK ) {
        //m_showerBuilderToExec = false;
        msg() << MSG::ERROR << "REGTEST: No CaloCellContainers retrieved for the trigger element, shower builder to be executed: " << m_showerBuilder << endreq;
        //Should this be ERROR? Maybe just set to run showerbuilder to false?
        return HLT::ERROR;
        //May need to add ERROR codes for online debugging
    } else{
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " REGTEST: Got " << vectorCellContainer.size()
            << " CaloCellContainers associated to the TE " << endreq;

        // Check that there is only one CellContainer in the RoI
        if (vectorCellContainer.size() != 1){
            //m_showerBuilderToExec = false;
            msg() << MSG::ERROR
                << "REGTEST: Size of calo cell container vector is not 1 but " << vectorCellContainer.size()
                << " shower builder to be executed: " << m_showerBuilder
                << endreq;
            return HLT::ERROR;
            //May need to add ERROR codes for online debugging
        } else{

            // Get the last CellContainer if ShoweShapeTool is going to be run
            pCaloCellContainer = vectorCellContainer.back();

            if(!pCaloCellContainer){
                msg() << MSG::ERROR
                    << "Retrieval of CaloCellContainer from vector failed, m_showerBuilderToExec: " << m_showerBuilder
                    << endreq;
                return HLT::ERROR;
                //May need to add ERROR codes for online debugging
            } else{
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Running m_showerBuilder: " << m_showerBuilder << endreq;
            } //pCaloCellContainer
        }
    }

    //**********************************************************************
    // If TrackMatchBuilder tool is going to be run 
    // get pointer to TrackParticleContainer from the trigger element and set flag for execution

    const xAOD::TrackParticleContainer* pTrackParticleContainer = 0;
    if (m_doTrackMatching){
        std::vector<const xAOD::TrackParticleContainer*> vectorTrackParticleContainer;
        stat = getFeatures(inputTE, vectorTrackParticleContainer);

        // in case a TrackParticleContainer is retrieved from the TE
        if (stat != HLT::OK) {
            m_doTrackMatching=false;
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
                << " REGTEST: no TrackParticleContainer from TE, m_trackMatchBuilder: " << m_trackMatchBuilder << endreq;

        } else {
            // check that it is not empty
            if (vectorTrackParticleContainer.size() < 1) {
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                    << " REGTEST: empty TrackParticleContainer from TE, m_trackMatchBuilder: " << m_trackMatchBuilder << endreq;

            } else {
                // Get the pointer to last TrackParticleContainer 
                pTrackParticleContainer = vectorTrackParticleContainer.back();
                m_doTrackMatching = true;
                if(!pTrackParticleContainer){
                    m_doTrackMatching = false;
                    msg() << MSG::ERROR
                        << " REGTEST: Retrieval of TrackParticleContainer from vector failed"
                        << endreq;
                }
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "m_doTrackMatching: " << m_doTrackMatching << endreq;
            }                
        }
    }//m_trackMatching

    //**********************************************************************
    // If Conversion Builder or EMFourMomBuilder are going to be run
    const xAOD::VertexContainer* pVxContainer= 0;
    std::vector<const xAOD::VertexContainer*> vectorVxContainer;
    stat = getFeatures(inputTE,vectorVxContainer);
    // in case a VxContainer is retrieved from the TE
    if (stat != HLT::OK) {
        m_doConversions = false;
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
            << " REGTEST: no VxContainer from TE, m_doConversions: " << m_doConversions << endreq;
    } else {
        // check that it is not empty
        if (vectorVxContainer.size() < 1) {
            m_doConversions= false;
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                << " REGTEST: no VxContainer from TE, m_doConversions: " << m_doConversions << endreq;
        } else {
            // Get the pointer to last VxContainer 
            pVxContainer = vectorVxContainer.back();
            if(!pVxContainer){
                m_doConversions = false;
                msg() << MSG::ERROR
                    << " REGTEST: Retrieval of VxContainer from vector failed"
                    << endreq;
            }
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "m_doConversions " << m_doConversions << endreq;
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Following offline egammaRec code

    // loop over clusters. 
    // Add egammaRec into container
    // then do next steps
    for( unsigned int i = 0; i<clusContainer->size(); i++){
        // create a new egamma object + corresponding egDetail container
        const ElementLink< xAOD::CaloClusterContainer > clusterLink( *clusContainer, i );
        const std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink}; 
        egammaRec* egRec = new egammaRec(); 
        egRec->setCaloClusters( elClusters );
        eg_container.push_back( egRec );
    } // End attaching clusters to egammaRec
    //
    // Conversions
    //
    // Need method which takes egRec and VxContainer
    if(m_doConversions){
        if (timerSvc()) m_timerTool2->start(); //timer
        for(auto egRec : eg_container) {
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
                << "REGTEST:: Running ConversionBuilder" << endreq;
            if(m_conversionBuilder->hltExecute(egRec,pVxContainer).isFailure())
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
                    << "REGTEST: no Conversion" << endreq;
        }
        if (timerSvc()) m_timerTool2->stop(); //timer
    } //m_doConversions/
    
    // Check for Track Match. 
    
    if(m_doTrackMatching){
        for(auto egRec : eg_container) {
            if (timerSvc()) m_timerTool1->start(); //timer
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
                << "REGTEST:: Running TrackMatchBuilder" << endreq;
            if(m_trackMatchBuilder->trackExecute(egRec,pTrackParticleContainer).isFailure())
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
                    << "REGTEST: no Track matched to this cluster" << endreq;
            if (timerSvc()) m_timerTool1->stop(); //timer
        } // End track match building
    } //m_trackMatchBuilderToExec


    // Run the ambiguity resolving to decide if egRec is Electron or Photon
    //Get the ElementLinks for Tracks, Clusters, Vertices
    ElementLinkVector<xAOD::CaloClusterContainer> clusterLinks;
    stat=getFeaturesLinks< xAOD::CaloClusterContainer, xAOD::CaloClusterContainer > (inputTE, clusterLinks, "");
    if ( stat != HLT::OK ) {
        ATH_MSG_ERROR("REGTEST: No CaloClusterLinks retrieved for the trigger element");
    } else{
        ATH_MSG_DEBUG(" REGTEST: Got CaloClusterLinks from TE");
    }
                
    ElementLinkVector<xAOD::TrackParticleContainer> trackLinks;
    stat=getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE, trackLinks, "");
    if ( stat != HLT::OK ) {
        ATH_MSG_ERROR("REGTEST: No TrackParticleLinks retrieved for the trigger element");
            //May need to add ERROR codes for online debugging
    } else{
        ATH_MSG_DEBUG(" REGTEST: Got TrackParticleLinks from TE");
    }
    
    ElementLinkVector<xAOD::VertexContainer> vxLinks;
    stat=getFeaturesLinks< xAOD::VertexContainer, xAOD::VertexContainer > (inputTE, vxLinks, "");
    if ( stat != HLT::OK ) {
        ATH_MSG_ERROR("REGTEST: No VertexLinks retrieved for the trigger element"); 
            //May need to add ERROR codes for online debugging
    } else{
        ATH_MSG_DEBUG(" REGTEST: Got VertexLinks from TE");
    }
    // Decorators for aux data
    // Calorimeter frame variables
    static SG::AuxElement::Decorator<float> etaCalo ("etaCalo");
    static SG::AuxElement::Decorator<float> phiCalo ("phiCalo");
    double tmp_etaCalo=-999.;
    double tmp_phiCalo=-999.;
    for(const auto& egRec : eg_container){
        // For now set author as Electron
        ATH_MSG_DEBUG("REGTEST:: Running AmbiguityTool");
        if (timerSvc()) m_timerTool3->start(); //timer
        unsigned int author = m_ambiguityTool->ambiguityResolve(egRec->vertex(),egRec->trackParticle());
        if (timerSvc()) m_timerTool3->stop(); //timer
        ATH_MSG_DEBUG("REGTEST:: AmbiguityTool Author " << author);
        // Set author
        // Do we set as TrigElectron/Photon?
        if (author == xAOD::EgammaParameters::AuthorUnknown) continue;//
        if (author == xAOD::EgammaParameters::AuthorElectron || 
                author == xAOD::EgammaParameters::AuthorAmbiguous){
            //Get Electron
            //eg->setAuthor(xAOD::EgammaParameters::AuthorTrigElectron);
            xAOD::Electron *electron = new xAOD::Electron();
            m_electron_container->push_back(electron);
            electron->setAuthor(author);
            std::vector< ElementLink< xAOD::CaloClusterContainer > > el_clusterLinks;
            for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i){
                const xAOD::CaloCluster *clus = egRec->caloCluster(i);
                bool isBarrel = xAOD::EgammaHelpers::isBarrel(clus);
                CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
                m_caloCellDetPos->getDetPosition(sample, clus->eta(), clus->phi0(), tmp_etaCalo, tmp_phiCalo); 
                ATH_MSG_DEBUG("Decorate the CaloClusters with Calo-frame coords.");
                etaCalo(*clus) = tmp_etaCalo;
                phiCalo(*clus) = tmp_phiCalo;
                ATH_MSG_DEBUG("Try to find ElementLink for CC with pT = " << clus->pt());
                for(const auto cl_link : clusterLinks){
                    if(!cl_link.isValid()) continue;
                    ATH_MSG_DEBUG("Try next EleLink, pt = " << (*cl_link)->pt());
                    if( *cl_link == clus ){
                        el_clusterLinks.push_back(cl_link);
                        ATH_MSG_DEBUG("Found matching CaloClusterLink to CaloCluster object in egRec, pt = "<< (*cl_link)->pt());
                    }
                }
            }
            electron->setCaloClusterLinks( el_clusterLinks);

            std::vector< ElementLink< xAOD::TrackParticleContainer > > el_trackLinks;
            for (unsigned int i = 0 ; i < egRec->getNumberOfTrackParticles(); ++i){
                const xAOD::TrackParticle *trk = egRec->trackParticle(i);
                ATH_MSG_DEBUG("Try to find ElementLink for TP with pT = " << trk->pt());
                for(const auto trk_link : trackLinks){
                    if(!trk_link.isValid()) continue;
                    ATH_MSG_DEBUG("Try next EleLink, pt = " << (*trk_link)->pt());
                    if( *trk_link == trk ){
                        el_trackLinks.push_back(trk_link);
                        ATH_MSG_DEBUG("Found matching TrackParticleLink to TrackParticle object in egRec, pt = "<< (*trk_link)->pt());
                    }
                }
            }
            electron->setTrackParticleLinks( el_trackLinks);

            //Set DeltaEta, DeltaPhi , DeltaPhiRescaled
            float deltaEta = static_cast<float>(egRec->deltaEta(0));
            float deltaPhi = static_cast<float>(egRec->deltaPhi(0));
            float deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(0));
            electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta0 );
            electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi0 );
            electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled0 );

            deltaEta = static_cast<float>(egRec->deltaEta(1));
            deltaPhi = static_cast<float>(egRec->deltaPhi(1));
            deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(1));
            electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta1 );
            electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi1 );
            electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled1);

            deltaEta = static_cast<float>(egRec->deltaEta(2));
            deltaPhi = static_cast<float>(egRec->deltaPhi(2));
            deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(2));
            electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta2 );
            electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi2 );
            electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled2);

            deltaEta = static_cast<float>(egRec->deltaEta(3));
            deltaPhi = static_cast<float>(egRec->deltaPhi(3));
            deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(3));
            electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta3 );
            electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi3 );
            electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled3);

            float deltaPhiLast = static_cast<float>(egRec->deltaPhiLast ());
            electron->setTrackCaloMatchValue(deltaPhiLast,xAOD::EgammaParameters::deltaPhiFromLastMeasurement );

        }//Electron
        if (author == xAOD::EgammaParameters::AuthorPhoton
                || author == xAOD::EgammaParameters::AuthorAmbiguous){
            //  eg->setAuthor(xAOD::EgammaParameters::AuthorTrigPhoton);
            xAOD::Photon *photon = new xAOD::Photon();
            m_photon_container->push_back(photon);
            photon->setAuthor(author);
            std::vector< ElementLink< xAOD::CaloClusterContainer > > ph_clusterLinks;
            for (unsigned int i = 0 ; i < egRec->getNumberOfClusters(); ++i){
                const xAOD::CaloCluster *clus = egRec->caloCluster(i);
                ATH_MSG_DEBUG("Decorate the CaloClusters with Calo-frame coords.");
                bool isBarrel = xAOD::EgammaHelpers::isBarrel(clus);
                CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
                m_caloCellDetPos->getDetPosition(sample, clus->eta(), clus->phi0(), tmp_etaCalo, tmp_phiCalo); 
                ATH_MSG_DEBUG("Decorate the CaloClusters with Calo-frame coords.");
                etaCalo(*clus) = tmp_etaCalo;
                phiCalo(*clus) = tmp_phiCalo;
                ATH_MSG_DEBUG("Try to find ElementLink for CC with pT = " << clus->pt());
                for(const auto cl_link : clusterLinks){
                    if(!cl_link.isValid()) continue;
                    ATH_MSG_DEBUG("Try next EleLink, pt = " << (*cl_link)->pt());
                    if( *cl_link == clus ){
                        ph_clusterLinks.push_back(cl_link);
                        ATH_MSG_DEBUG("Found matching CaloClusterLink to CaloCluster object in egRec, pt = "<< (*cl_link)->pt());
                    }
                }
            }
            photon->setCaloClusterLinks( ph_clusterLinks);
            std::vector< ElementLink< xAOD::VertexContainer > > ph_vxLinks;
            for (unsigned int i = 0 ; i < egRec->getNumberOfVertices(); ++i){
                const xAOD::Vertex *vx = egRec->vertex(i);
                for(const auto vx_link : vxLinks){
                    if(!vx_link.isValid()) continue;
                    if( *vx_link == vx ){
                        ph_vxLinks.push_back(vx_link);
                        ATH_MSG_DEBUG("Found matching VertexLink to Vertex object in egRec");
                    }
                }
            }
            photon->setVertexLinks( ph_vxLinks);
            // Transfer deltaEta/Phi info
            float deltaEta = egRec->deltaEtaVtx(), deltaPhi = egRec->deltaPhiVtx();
            if (!photon->setVertexCaloMatchValue( deltaEta, xAOD::EgammaParameters::convMatchDeltaEta1) )
            {
                ATH_MSG_WARNING("Could not transfer deltaEta to photon");
                continue;
            }
            if (!photon->setVertexCaloMatchValue( deltaPhi,xAOD::EgammaParameters::convMatchDeltaPhi1) )
            {
                ATH_MSG_WARNING("Could not transfer deltaEta to photon");
                continue;
            }
        } //Photon
    }//end loop of egRec

    //Dress the Electron objects
    for (const auto& eg : *m_electron_container){
        // EMFourMomentum
        if (timerSvc()) m_timerTool4->start(); //timer
        if(msgLvl() <= MSG::DEBUG) 
            msg() << MSG::DEBUG << "about to run hltExecute(eg,index)" << endreq;
        if(m_fourMomBuilder->hltExecute(eg,0)); // What is the index?
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: problem with fourmom tool" << endreq; 
        if (timerSvc()) m_timerTool4->stop(); //timer

        // Shower Shape
        // Track Isolation tool no longer runs for trigger in showershape, but we can do ourselves
        if (timerSvc()) m_timerTool5->start(); //timer
        ATH_MSG_DEBUG("about to run recoExecute(eg,pCaloCellContainer,pTrackParticleContainer)");
        if( m_showerBuilder->recoExecute(eg,pCaloCellContainer,pTrackParticleContainer) );
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: no shower built for this cluster" << endreq;
        if (timerSvc()) m_timerTool5->stop(); //timer
    }
    //Dress the Photon objects
    for (const auto& eg : *m_photon_container){
        // EMFourMomentum
        if (timerSvc()) m_timerTool4->start(); //timer
        if(msgLvl() <= MSG::DEBUG) 
            msg() << MSG::DEBUG << "about to run hltExecute(eg,index)" << endreq;
        if(m_fourMomBuilder->hltExecute(eg,0)); // What is the index?
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: problem with fourmom tool" << endreq; 
        if (timerSvc()) m_timerTool4->stop(); //timer

        // Shower Shape
        if (timerSvc()) m_timerTool5->start(); //timer
        ATH_MSG_DEBUG("about to run recoExecute(eg,pCaloCellContainer,pTrackParticleContainer)");
        if( m_showerBuilder->recoExecute(eg,pCaloCellContainer,pTrackParticleContainer) );
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: no shower built for this cluster" << endreq;
        if (timerSvc()) m_timerTool5->stop(); //timer
    }


    // attach electron container to the TE
    stat = reAttachFeature( outputTE, m_electron_container, electronContSGName, electronKey);
    if (stat != HLT::OK){
        msg() << MSG::ERROR << "REGTEST: trigger xAOD::ElectronContainer attach to TE and record into StoreGate failed" << endreq;
        return stat;
    } else{
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::VERBOSE << "electronKey for electron container: " << electronKey << endreq;
            msg() << MSG::DEBUG << "REGTEST: xAOD::ElectronContainer created and attached to TE: " << electronContSGName 
                << " with alias: " << electronContSGKey
                << endreq;
        }
    }

    // lock the electron collection
    if ( (store()->setConst(m_electron_container)).isFailure() ) {
        msg() << MSG::ERROR
            << " REGTEST: cannot set m_electron_container to const "
            << endreq;
        return HLT::OK;
    } else {
        // debug message
        if(msgLvl() <= MSG::VERBOSE) {
           msg() << MSG::VERBOSE << "electron container locked with key: " << electronContSGName 
                << " and address: " << m_electron_container << endreq;
        }
    }
    
    // attach photon container to the TE
    stat = reAttachFeature( outputTE, m_photon_container, photonContSGName, photonKey);
    if (stat != HLT::OK){
        msg() << MSG::ERROR << "REGTEST: trigger xAOD::PhotonContainer attach to TE and record into StoreGate failed" << endreq;
        return stat;
    } else{
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::VERBOSE << "photonKey for photon container: " << photonKey << endreq;
            msg() << MSG::DEBUG << "REGTEST: xAOD::PhotonContainer created and attached to TE: " << photonContSGName 
                << " with alias: " << photonContSGKey
                << endreq;
        }
    }

    // lock the photon collection
    if ( (store()->setConst(m_photon_container)).isFailure() ) {
        msg() << MSG::ERROR
            << " REGTEST: cannot set m_photon_container to const "
            << endreq;
        return HLT::OK;
    } else {
        // debug message
        if(msgLvl() <= MSG::VERBOSE) {
           msg() << MSG::VERBOSE << "photon container locked with key: " << photonContSGName 
                << " and address: " << m_photon_container << endreq;
        }
    }



    // debug message
    if(msgLvl() <= MSG::DEBUG) {
        //const xAOD::CaloCluster* clus=0;
        //const xAOD::TrackParticle* trackParticle=0; 
        float val_float=-99;
        //DEBUG output for Egamma container
        msg() << MSG::DEBUG << " REGTEST: xAOD Reconstruction variables: " << endreq; 
        //Cluster and ShowerShape info
        for (const auto& eg : *m_electron_container){
            //REGTEST printout
            if (eg) {
                msg() << MSG::DEBUG << " REGTEST: egamma energy: " << eg->e() << endreq;
                msg() << MSG::DEBUG << " REGTEST: egamma eta: " << eg->eta() << endreq;
                msg() << MSG::DEBUG << " REGTEST: egamma phi: " << eg->phi() << endreq;
            } else{
                msg() << MSG::DEBUG << " REGTEST: problems with egamma pointer" << endreq;
            }

            //msg() << MSG::VERBOSE << " REGTEST: cluster variables " << endreq;
            //clus = eg->caloCluster();
            if (eg->caloCluster()) {
                msg() << MSG::DEBUG << " REGTEST: egamma cluster transverse energy: " << eg->caloCluster()->et() << endreq;
                msg() << MSG::DEBUG << " REGTEST: egamma cluster eta: " << eg->caloCluster()->eta() << endreq;
                msg() << MSG::DEBUG << " REGTEST: egamma cluster phi: " << eg->caloCluster()->phi() << endreq;
                ATH_MSG_DEBUG(" REGTEST: egamma Calo-frame coords. etaCalo = " << eg->caloCluster()->auxdata<float>("etaCalo"));
                ATH_MSG_DEBUG(" REGTEST: egamma Calo-frame coords. phiCalo = " << eg->caloCluster()->auxdata<float>("phiCalo"));
            } else{
                msg() << MSG::DEBUG << " REGTEST: problems with egamma cluster pointer" << endreq;
            }
            
            msg() << MSG::DEBUG << " REGTEST: EMShower variables "        << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::ethad); 
            msg() << MSG::DEBUG << " REGTEST: ethad    =  " << val_float << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::e011); 
            msg() << MSG::DEBUG << " REGTEST: e011     =  " << val_float << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::e132); 
            msg() << MSG::DEBUG << " REGTEST: e132     =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone); 
            msg() << MSG::DEBUG << " REGTEST: etcone   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone15); 
            msg() << MSG::DEBUG << " REGTEST: etcone15   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone20); 
            msg() << MSG::DEBUG << " REGTEST: etcone20   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone25); 
            msg() << MSG::DEBUG << " REGTEST: etcone25   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone30); 
            msg() << MSG::DEBUG << " REGTEST: etcone30   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone35); 
            msg() << MSG::DEBUG << " REGTEST: etcone35   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone40); 
            msg() << MSG::DEBUG << " REGTEST: etcone40   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone15_ptcorrected); 
            msg() << MSG::DEBUG << " REGTEST: etcone15_ptcorrected   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone20_ptcorrected); 
            msg() << MSG::DEBUG << " REGTEST: etcone20_ptcorrected   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone25_ptcorrected); 
            msg() << MSG::DEBUG << " REGTEST: etcone25_ptcorrected   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone30_ptcorrected); 
            msg() << MSG::DEBUG << " REGTEST: etcone30_ptcorrected   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone35_ptcorrected); 
            msg() << MSG::DEBUG << " REGTEST: etcone35_ptcorrected   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::etcone40_ptcorrected); 
            msg() << MSG::DEBUG << " REGTEST: etcone40_ptcorrected   =  " << val_float << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::e237); 
            msg() << MSG::DEBUG << " REGTEST: e237     =  " << val_float << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::e335); 
            msg() << MSG::DEBUG << " REGTEST: e335     =  " << val_float << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2ts1); 
            msg() << MSG::DEBUG << " REGTEST: e2ts1    =  " << val_float << endreq;
            eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2tsts1); 
            msg() << MSG::DEBUG << " REGTEST: e2tsts1  =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::ptcone20); 
            msg() << MSG::DEBUG << " REGTEST: ptcone20   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::ptcone30); 
            msg() << MSG::DEBUG << " REGTEST: ptcone30   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::ptcone40); 
            msg() << MSG::DEBUG << " REGTEST: ptcone40   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::nucone20); 
            msg() << MSG::DEBUG << " REGTEST: nucone20   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::nucone30); 
            msg() << MSG::DEBUG << " REGTEST: nucone30   =  " << val_float << endreq;
            eg->isolationValue(val_float,xAOD::EgammaParameters::nucone40); 
            msg() << MSG::DEBUG << " REGTEST: nucone40   =  " << val_float << endreq;
            //DEBUG info for Electrons which by definition have a track match
            msg() << MSG::VERBOSE << " REGTEST: trackmatch variables "        << endreq;
            //clus = eg->caloCluster();
            //trackParticle = eg->trackParticle();
            
            if(eg->trackParticle()){
                msg() << MSG::DEBUG << " REGTEST: pt=  " << eg->trackParticle()->pt() << endreq;
                msg() << MSG::VERBOSE << " REGTEST: charge=  " << eg->trackParticle()->charge() << endreq;
                msg() << MSG::DEBUG << " REGTEST: E/p=  " << eg->caloCluster()->et() / eg->trackParticle()->pt() << endreq;
                //msg() << MSG::DEBUG << " REGTEST: Num. tracks=  " << trkmatch->tracksInBroadWindow() << endreq;
                eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaEta1);
                msg() << MSG::DEBUG << " REGTEST: Delta eta 1st sampling=  " << val_float << endreq;
                eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhi2);
                msg() << MSG::DEBUG << " REGTEST: Delta phi 2nd sampling=  " << val_float << endreq;
            } else{
                msg() << MSG::DEBUG << " REGTEST: no electron eg->trackParticle() pointer" << endreq; 
            }
        }
        //DEBUG infor for Photons and conversions
        msg() << MSG::DEBUG << "HLTAlgo Execution of xAOD TrigEgammaRec completed successfully" << endreq;
    } 

    // Time total TrigEgammaRec execution time.
    if (timerSvc()) m_timerTotal->stop();

    return HLT::OK;
}
