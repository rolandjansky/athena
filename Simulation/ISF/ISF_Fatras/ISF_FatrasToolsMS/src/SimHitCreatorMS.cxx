/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SimHitCreatorMS.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Interfaces/IParticleHelper.h"
// Trk
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"
// MuonSpectrometer includes
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"

#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Random/RandGauss.h"

//================ Constructor =================================================

iFatras::SimHitCreatorMS::SimHitCreatorMS(const std::string& t,
    const std::string& n,
    const IInterface*  p ) : 
    base_class(t,n,p),
    m_incidentSvc("IncidentSvc", n),
    m_extrapolator(""),
    m_measTool("Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"),
    m_mdtSimHitCollection(nullptr),
    m_rpcSimHitCollection(nullptr),
    m_tgcSimHitCollection(nullptr),
    m_cscSimHitCollection(nullptr),
    m_mmSimHitCollection(nullptr),
    m_stgcSimHitCollection(nullptr),
    m_mdtCollectionName("MDT_Hits"),
    m_rpcCollectionName("RPC_Hits"),
    m_tgcCollectionName("TGC_Hits"),
    m_cscCollectionName("CSC_Hits"),
    m_mmCollectionName("MM_Hits"),
    m_stgcCollectionName("sTGC_Hits"),
    m_randomSvc("AtDSFMTGenSvc", n),
    m_randomEngineName("FatrasRnd"),
    m_randomEngine(nullptr),  
    m_mdtHitIdHelper(nullptr),
    m_rpcHitIdHelper(nullptr),
    m_cscHitIdHelper(nullptr),
    m_tgcHitIdHelper(nullptr),
    m_mmOffToSimId(nullptr),
    m_stgcOffToSimId(nullptr),
    m_muonMgr(nullptr),
    m_mdtSigmaDriftRadius(0.08),
    m_BMGid(-1),
    m_createAllMdtHits(true)
{
  //  template for property decalration
    declareProperty("MeasurementTool",              m_measTool);
    declareProperty("MdtSigmaDriftRadius",          m_mdtSigmaDriftRadius);  
    // Random number svc 
    declareProperty("RandomNumberService",          m_randomSvc,        "Random number generator");
    declareProperty("RandomStreamName",             m_randomEngineName, "Name of the random number stream");
    // extrapolator
    declareProperty("Extrapolator",                 m_extrapolator);
    declareProperty("MDTCollectionName",            m_mdtCollectionName);
    declareProperty("RPCCollectionName",            m_rpcCollectionName);
    declareProperty("TGCCollectionName",            m_tgcCollectionName);
    declareProperty("CSCCollectionName",            m_cscCollectionName);
    declareProperty("MMCollectionName",             m_mmCollectionName);
    declareProperty("sTGCCollectionName",           m_stgcCollectionName);
    declareProperty("CreateAllMdtHits",             m_createAllMdtHits);
}

//================ Initialisation =================================================

StatusCode iFatras::SimHitCreatorMS::initialize()
{

  // Get Extrapolator from ToolService
  ATH_CHECK(m_extrapolator.retrieve());
  // Get IdHelper from ToolService
  ATH_CHECK(m_idHelperSvc.retrieve());
  // the MS helpers for the different technologies
  m_mdtHitIdHelper = MdtHitIdHelper::GetHelper();
  m_rpcHitIdHelper = RpcHitIdHelper::GetHelper(m_idHelperSvc->rpcIdHelper().gasGapMax());
  m_tgcHitIdHelper = TgcHitIdHelper::GetHelper();
  m_cscHitIdHelper = CscHitIdHelper::GetHelper();

  ATH_CHECK(detStore()->retrieve(m_muonMgr));

  m_mmOffToSimId = new MM_SimIdToOfflineId(&m_idHelperSvc->mmIdHelper());
  m_stgcOffToSimId = new sTgcSimIdToOfflineId(&m_idHelperSvc->stgcIdHelper());
  
  // get measurement tool
  ATH_CHECK(m_measTool.retrieve());

  // Random number service
  ATH_CHECK(m_randomSvc.retrieve());
  //Get own engine with own seeds:
  m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
      ATH_MSG_ERROR( "[ --- ] Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
  }


  // Athena/Gaudi framework
  ATH_CHECK(m_incidentSvc.retrieve());
  // register to the incident service: BeginEvent for TrackCollection
  m_incidentSvc->addListener( this, IncidentType::BeginEvent);

  ATH_MSG_INFO( "[ mutrack ] initialize() successful." );

     m_BMGpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1;
      if(m_BMGpresent){
        ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
        m_BMGid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
        for(int phi=6; phi<8; phi++) { // phi sectors
          for(int eta=1; eta<4; eta++) { // eta sectors
            for(int side=-1; side<2; side+=2) { // side
              if( !m_muonMgr->getMuonStation("BMG", side*eta, phi) ) continue;
              for(int roe=1; roe<= ((m_muonMgr->getMuonStation("BMG", side*eta, phi) )->nMuonReadoutElements()); roe++) { // iterate on readout elemets
                const MuonGM::MdtReadoutElement* mdtRE =
                      dynamic_cast<const MuonGM::MdtReadoutElement*> ( ( m_muonMgr->getMuonStation("BMG", side*eta, phi) )->getMuonReadoutElement(roe) ); // has to be an MDT
                if(mdtRE) initDeadChannels(mdtRE);
              }
            }
          }
        }
      }


  return StatusCode::SUCCESS;
}

//================ Event Initialization =========================================
//
void iFatras::SimHitCreatorMS::handle( const Incident& inc ) {
  // check the incident type 
  if ( inc.type() == IncidentType::BeginEvent ){
      ATH_MSG_VERBOSE("[ muon sim hit ] Setting MuonSimHit collections up");
      // check if the hit collection already contains:
      // (a)     if yes ... try to retrieve it
      if ( evtStore()->contains<MDTSimHitCollection>(m_mdtCollectionName) ){
	if ( (evtStore()->retrieve(m_mdtSimHitCollection , m_mdtCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve MDTSimHitCollection " << m_mdtCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_mdtSimHitCollection = new MDTSimHitCollection( m_mdtCollectionName);
	if ( (evtStore()->record(m_mdtSimHitCollection, m_mdtCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record MDTSimHitCollection " << m_mdtCollectionName);
             delete m_mdtSimHitCollection; m_mdtSimHitCollection=0;
        }
      }
      if ( evtStore()->contains<RPCSimHitCollection>(m_rpcCollectionName) ){
	if ( (evtStore()->retrieve(m_rpcSimHitCollection , m_rpcCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve RPCSimHitCollection " << m_rpcCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_rpcSimHitCollection = new RPCSimHitCollection( m_rpcCollectionName);
	if ( (evtStore()->record(m_rpcSimHitCollection, m_rpcCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record RPCSimHitCollection " << m_rpcCollectionName);
             delete m_rpcSimHitCollection; m_rpcSimHitCollection=0;
        }
      }
      if ( evtStore()->contains<TGCSimHitCollection>(m_tgcCollectionName) ){
	if ( (evtStore()->retrieve(m_tgcSimHitCollection , m_tgcCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve TGCSimHitCollection " << m_tgcCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_tgcSimHitCollection = new TGCSimHitCollection( m_tgcCollectionName);
	if ( (evtStore()->record(m_tgcSimHitCollection, m_tgcCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record TGCSimHitCollection " << m_tgcCollectionName);
             delete m_tgcSimHitCollection; m_tgcSimHitCollection=0;
        }
      }
      if ( evtStore()->contains<CSCSimHitCollection>(m_cscCollectionName) ){
	if ( (evtStore()->retrieve(m_cscSimHitCollection , m_cscCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve CSCSimHitCollection " << m_cscCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_cscSimHitCollection = new CSCSimHitCollection( m_cscCollectionName);
	if ( (evtStore()->record(m_cscSimHitCollection, m_cscCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record CSCSimHitCollection " << m_cscCollectionName);
             delete m_cscSimHitCollection; m_cscSimHitCollection=0;
        }
      }
      if ( evtStore()->contains<MMSimHitCollection>(m_mmCollectionName) ){
	if ( (evtStore()->retrieve(m_mmSimHitCollection , m_mmCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve MMSimHitCollection " << m_mmCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_mmSimHitCollection = new MMSimHitCollection( m_mmCollectionName);
	if ( (evtStore()->record(m_mmSimHitCollection, m_mmCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record MMSimHitCollection " << m_mmCollectionName);
             delete m_mmSimHitCollection; m_mmSimHitCollection=0;
        }
      }
      if ( evtStore()->contains<sTGCSimHitCollection>(m_stgcCollectionName) ){
	if ( (evtStore()->retrieve(m_stgcSimHitCollection , m_stgcCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve sTGCSimHitCollection " << m_stgcCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_stgcSimHitCollection = new sTGCSimHitCollection( m_stgcCollectionName);
	if ( (evtStore()->record(m_stgcSimHitCollection, m_stgcCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record sTGCSimHitCollection " << m_stgcCollectionName);
             delete m_stgcSimHitCollection; m_stgcSimHitCollection=0;
        }
      }
   }
   return;
}


//================ Track Creation Interface  =====================================
void iFatras::SimHitCreatorMS::createHits(const ISF::ISFParticle& isp,
					  const std::vector<Trk::HitInfo>& hits) const {
    
  // iterate and assign as well the layer
  std::vector<Trk::HitInfo>::const_iterator plIter    = hits.begin();
  std::vector<Trk::HitInfo>::const_iterator plIterEnd = hits.end();
  for ( ; plIter != plIterEnd; ++plIter ){
    // get the parameters & associated layer
    const Trk::TrackParameters* parm = (*plIter).trackParms;
    double timeInfo = (*plIter).time;
    const Trk::Layer*        currLay = m_extrapolator->trackingGeometry()->associatedLayer( parm->position() );

    if (!currLay) continue;

    Identifier id(currLay->layerType());
 
    // NSW hits
    if ( m_idHelperSvc->isMM(id) ||  m_idHelperSvc->issTgc(id) ) {   
      // hit ID
      int simID = offIdToSimId(id);
      // local position : at MTG layer ( corresponds to the middle of the gas gap ) 
      // generating particle info
      double mom  = parm->momentum().mag();
      double mass = isp.mass();
      double eKin = sqrt( mom*mom+mass*mass) - mass;
      // the rest of information needs adjustment once full sim hits available
      double energyDeposit = 1.;
      const Amg::Vector3D& pos=parm->position();
      const Amg::Vector3D unitMom=parm->momentum().normalized();

      MMSimHit nswMMHit = MMSimHit(simID,timeInfo, pos, 
				 isp.pdgCode(),eKin,unitMom, 
				 energyDeposit, isp.barcode()) ;
      sTGCSimHit nswsTGCHit = sTGCSimHit(simID,timeInfo, pos, 
				     isp.pdgCode(), unitMom, 
				     energyDeposit, isp.barcode()) ;
      
      if ( m_idHelperSvc->isMM(id) )  m_mmSimHitCollection->Insert(nswMMHit); 
      else  m_stgcSimHitCollection->Insert(nswsTGCHit); 

      ATH_MSG_VERBOSE("[ muhit ] NSW hit created.");           

    }  else if (m_idHelperSvc->isMdt(id)) {    // (A) special treatment for MDTs to find closest channel and nearby hits
      double pitch = 0.;
      // get the identifier 
      Identifier hid = m_measTool->nearestDetEl(currLay,parm,false,pitch);
      // 
      if (m_idHelperSvc->mdtIdHelper().valid(hid)) {
	// create first hit 
	bool hitCreated = createHit(isp, currLay,parm,hid,timeInfo,pitch, true);
	if (m_createAllMdtHits) {
	  // nearby hits - check range 
	  const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(hid);  
	  if (!mdtROE) continue;	   
	  int tMax = mdtROE->getNtubesperlayer();
	  int tCur = m_idHelperSvc->mdtIdHelper().tube(hid);  
	  // recalculate id
	  int next=-1; 
	  while (tCur+next>0) {
	    Identifier nextId = m_idHelperSvc->mdtIdHelper().channelID(m_idHelperSvc->mdtIdHelper().stationName(hid),
									m_idHelperSvc->mdtIdHelper().stationEta(hid),
									m_idHelperSvc->mdtIdHelper().stationPhi(hid),
									m_idHelperSvc->mdtIdHelper().multilayer(hid),
									m_idHelperSvc->mdtIdHelper().tubeLayer(hid),
									tCur+next);
	    if (!m_idHelperSvc->mdtIdHelper().valid(nextId)) break;
	    hitCreated = createHit(isp, currLay,parm,nextId,timeInfo,pitch,true);
	    if (!hitCreated) break;
	    next--;
	  }
	  next = 1;
	  while (tCur+next <= tMax) {
	    Identifier nextId = m_idHelperSvc->mdtIdHelper().channelID(m_idHelperSvc->mdtIdHelper().stationName(hid),
									m_idHelperSvc->mdtIdHelper().stationEta(hid),
									m_idHelperSvc->mdtIdHelper().stationPhi(hid),
									m_idHelperSvc->mdtIdHelper().multilayer(hid),
									m_idHelperSvc->mdtIdHelper().tubeLayer(hid),
									tCur+next);
	    if (!m_idHelperSvc->mdtIdHelper().valid(nextId)) break;
	    hitCreated = createHit(isp, currLay,parm,nextId,timeInfo,pitch,true);
	    if (!hitCreated) break;
	    next++; 
	  }
	}  // hits outside central plane
      }  // hit corresponding to the intersection with the central plane  
    } else { // (B) not an MDT nor NSW layer
      double pitch = 0.;
      Identifier hid = m_measTool->nearestDetEl(currLay,parm,false,pitch);
      
      // check if this is valid 
      if ( hid.get_identifier32().get_compact()>0 && createHit(isp, currLay,parm,hid,timeInfo,pitch,false) )
	ATH_MSG_VERBOSE("[ muhit ] Hit in MS created.");
	// reset id for the other side
      hid = m_measTool->nearestDetEl(currLay,parm,true,pitch);
      // again check if this is valid
      if ( hid.get_identifier32().get_compact()>0 && createHit(isp, currLay,parm,hid,timeInfo,pitch,true) )     
	  ATH_MSG_VERBOSE("[ muhit ] Hit in MS created.");           
    }  // end of asocciated layer check            
  } // end of the loop over TrackParameters    
}


bool iFatras::SimHitCreatorMS::createHit(const ISF::ISFParticle& isp,
					 const Trk::Layer* lay,const Trk::TrackParameters* parm, Identifier id, double globalTimeEstimate, double /* pitch */, bool /* smear */) const
{
   // MDT SECTION 
   if (m_idHelperSvc->isMdt(id)) {
            
     int simId = m_mdtHitIdHelper->BuildMdtHitId(m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(id)),
						 m_idHelperSvc->mdtIdHelper().stationPhi(id), m_idHelperSvc->mdtIdHelper().stationEta(id),
						 m_idHelperSvc->mdtIdHelper().multilayer(id), m_idHelperSvc->mdtIdHelper().tubeLayer(id),
						 m_idHelperSvc->mdtIdHelper().tube(id));
     
     ATH_MSG_VERBOSE(  "[ muhit ] Creating MDTSimHit with identifier " <<  simId );
     // local position from the mdt's i
     const MuonGM::MdtReadoutElement* MdtRoEl = m_muonMgr->getMdtReadoutElement(id);
     if(m_BMGpresent && m_idHelperSvc->mdtIdHelper().stationName(id) == m_BMGid ) {
       auto myIt = m_DeadChannels.find(MdtRoEl->identify());
       if( myIt != m_DeadChannels.end() ){
         if( std::find( (myIt->second).begin(), (myIt->second).end(), id) != (myIt->second).end() ) {
           ATH_MSG_DEBUG("Skipping tube with identifier " << m_idHelperSvc->mdtIdHelper().show_to_string(id) );
           return false;
         }
       }
     }
     // local position from the mdt's 
     const Amg::Vector3D  localPos = m_muonMgr->getMdtReadoutElement(id)->globalToLocalCoords(parm->position(),id);
     // drift radius
     double residual = m_measTool->residual(lay,parm,id);     
     if (fabs(residual)<15.075) {

       double dlh = sqrt(15.075*15.075-residual*residual); 
       double de = 0.02*dlh/15.075;
       double energyDeposit= de + 0.005*CLHEP::RandGauss::shoot(m_randomEngine);
       while (energyDeposit<0.)  energyDeposit= de + 0.005*CLHEP::RandGauss::shoot(m_randomEngine);
    
       // a new simhit                                            
       MDTSimHit mdtHit = MDTSimHit(simId,globalTimeEstimate,
				    fabs(residual),
				    localPos,
				    isp.barcode(),
                                    2*dlh, energyDeposit, isp.pdgCode(),isp.momentum().mag() ) ;
       m_mdtSimHitCollection->Insert(mdtHit);
       return true;
     } else {
       return false;
     }
  } else if (m_idHelperSvc->isRpc(id)) { 
    // local position from the rpc's
    const Amg::Vector3D localPos = m_muonMgr->getRpcReadoutElement(id)->globalToLocalCoords(parm->position(),id);
    int simId = m_rpcHitIdHelper->BuildRpcHitId(m_idHelperSvc->rpcIdHelper().stationNameString(m_idHelperSvc->rpcIdHelper().stationName(id)),
						m_idHelperSvc->rpcIdHelper().stationPhi(id), m_idHelperSvc->rpcIdHelper().stationEta(id),
						m_idHelperSvc->rpcIdHelper().doubletZ(id),   m_idHelperSvc->rpcIdHelper().doubletR(id),
						m_idHelperSvc->rpcIdHelper().gasGap(id),     m_idHelperSvc->rpcIdHelper().doubletPhi(id),
						m_idHelperSvc->rpcIdHelper().measuresPhi(id));
    
    ATH_MSG_VERBOSE(  "[ muhit ] Creating RPCSimHit with identifier " <<  simId );

    double energyDeposit= 1.5e-03 + 3.9e-04*CLHEP::RandLandau::shoot(m_randomEngine);
    while (energyDeposit<0.)  energyDeposit= 1.5e-03 + 3.9e-04*CLHEP::RandLandau::shoot(m_randomEngine);
    
    // a new simhit
    RPCSimHit rpcHit = RPCSimHit(simId,globalTimeEstimate, localPos, isp.barcode(), localPos, energyDeposit,1.,isp.pdgCode(),isp.momentum().mag() ) ; 
    m_rpcSimHitCollection->Insert(rpcHit); 

  } else if (m_idHelperSvc->isTgc(id) && !m_idHelperSvc->tgcIdHelper().isStrip(id) ) { 
    
    // take eta hits only
    // local position
    const Amg::Vector3D localPos = Amg::Translation3D(-1.4, 0., 0.)*m_muonMgr->getTgcReadoutElement(id)->globalToLocalCoords(parm->position(),id);
    // local direction
    Amg::Vector3D localDir = m_muonMgr->getTgcReadoutElement(id)->globalToLocalTransf(id).rotation()*parm->momentum().normalized();
    
    int simId = m_tgcHitIdHelper->BuildTgcHitId(m_idHelperSvc->tgcIdHelper().stationNameString(m_idHelperSvc->tgcIdHelper().stationName(id)),
						m_idHelperSvc->tgcIdHelper().stationPhi(id), m_idHelperSvc->tgcIdHelper().stationEta(id),
						m_idHelperSvc->tgcIdHelper().gasGap(id));
    
    ATH_MSG_VERBOSE(  "[ muhit ] Creating TGCSimHit with identifier " <<  simId );
    
    // TO DO: adust energy deposit and step length ( 1keV,3 mm ? )
    double energyDeposit= 1.3e-03 + 6.e-04*CLHEP::RandGauss::shoot(m_randomEngine);
    while (energyDeposit<0.) energyDeposit= 1.3e-03 + 6.e-04*CLHEP::RandGauss::shoot(m_randomEngine);
    double stepLength=3.;
    
    // a new simhit
    TGCSimHit tgcHit = TGCSimHit(simId,globalTimeEstimate, localPos, localDir, isp.barcode(), energyDeposit, stepLength ) ;
    m_tgcSimHitCollection->Insert(tgcHit); 
  } else if (m_idHelperSvc->isCsc(id)) { 
    // one of eta/phi hits only
    
    Amg::Vector3D dir(parm->momentum().normalized());
    
    //!< TO DO : to be fixed
    float energyDeposit= 0.24e-03 + 1.1e-03*CLHEP::RandGauss::shoot(m_randomEngine);
    while (energyDeposit<0.) energyDeposit= 0.24e-03 + 1.1e-03*CLHEP::RandGauss::shoot(m_randomEngine);
    // cos of incident angle
    float cs=fabs(dir.dot(parm->associatedSurface().normal())); 
    float hitlength = 5./cs;
    //!< end of TO DO
    
    Amg::Vector3D startPos(parm->position()-0.5*hitlength*dir);
    Amg::Vector3D endPos(parm->position()+0.5*hitlength*dir);
    Amg::Vector3D hitStart = m_muonMgr->getCscReadoutElement(id)->globalToLocalCoords(startPos,id); 
    Amg::Vector3D hitEnd   = m_muonMgr->getCscReadoutElement(id)->globalToLocalCoords(endPos,id); 
    
    // the lundcode (from CSCSensitiveDetector)
    int lundcode=0;
    int pdgcode= isp.pdgCode();
    
    if (pdgcode == 22) lundcode=1;
    else if (pdgcode == 13 ) lundcode=6;
    else if (pdgcode == -13) lundcode=5; 
    else lundcode = 999;
    // else if Trk::nonInteracting  lundcode = 999;
    
    int simId = m_cscHitIdHelper->BuildCscHitId(m_idHelperSvc->cscIdHelper().stationNameString(m_idHelperSvc->cscIdHelper().stationName(id)),
						m_idHelperSvc->cscIdHelper().stationPhi(id), m_idHelperSvc->cscIdHelper().stationEta(id),
						m_idHelperSvc->cscIdHelper().chamberLayer(id), m_idHelperSvc->cscIdHelper().wireLayer(id));
    
    ATH_MSG_VERBOSE(  "[ muhit ] Creating CSCSimHit with identifier " <<  simId );
    
    // a new simhit
    CSCSimHit cscHit = CSCSimHit(simId,globalTimeEstimate,energyDeposit, hitStart, hitEnd, lundcode, isp.barcode() ) ;
    m_cscSimHitCollection->Insert(cscHit); 
  }
  
  return false; 
}

int iFatras::SimHitCreatorMS::offIdToSimId(Identifier id) const{

  if (m_idHelperSvc->isMM(id)) {
  
    int simID = m_mmOffToSimId->convert(id);

    //verify
    Identifier check_id = m_mmOffToSimId->convert(simID);
    
    if ( check_id != id ) {

      ATH_MSG_WARNING("MM Id conversion error!");
      ATH_MSG_WARNING(m_idHelperSvc->mmIdHelper().print_to_string(id));
      ATH_MSG_WARNING(m_idHelperSvc->mmIdHelper().print_to_string(check_id));

    }

    return simID;
  } else if (m_idHelperSvc->issTgc(id)) {
      
    int simID = m_stgcOffToSimId->convert(id);

    //verify
    Identifier check_id = m_stgcOffToSimId->convert(simID);
    
    if ( check_id != id ) {

      ATH_MSG_WARNING("sTGC Id conversion error!");
      ATH_MSG_WARNING(m_idHelperSvc->stgcIdHelper().print_to_string(id));
      ATH_MSG_WARNING(m_idHelperSvc->stgcIdHelper().print_to_string(check_id));

    }

    return simID;
  }


  return 0; 
}
void iFatras::SimHitCreatorMS::initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl) {
  PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
  int nGrandchildren = cv->getNChildVols();
  if(nGrandchildren <= 0) return;

  Identifier detElId = mydetEl->identify();

  int name = m_idHelperSvc->mdtIdHelper().stationName(detElId);
  int eta = m_idHelperSvc->mdtIdHelper().stationEta(detElId);
  int phi = m_idHelperSvc->mdtIdHelper().stationPhi(detElId);
  int ml = m_idHelperSvc->mdtIdHelper().multilayer(detElId);
  std::vector<Identifier> deadTubes;
  
    for(int layer = 1; layer <= mydetEl->getNLayers(); layer++){
    for(int tube = 1; tube <= mydetEl->getNtubesperlayer(); tube++){
      bool tubefound = false;
      for(unsigned int kk=0; kk < cv->getNChildVols(); kk++) {
        int tubegeo = cv->getIdOfChildVol(kk) % 100;
        int layergeo = ( cv->getIdOfChildVol(kk) - tubegeo ) / 100;
        if( tubegeo == tube && layergeo == layer ) {
          tubefound=true;
          break;
        }
        if( layergeo > layer ) break; // don't loop any longer if you cannot find tube anyway anymore
      }
      if(!tubefound) {
        Identifier deadTubeId = m_idHelperSvc->mdtIdHelper().channelID( name, eta, phi, ml, layer, tube );
        deadTubes.push_back( deadTubeId );
        ATH_MSG_VERBOSE("adding dead tube (" << tube  << "), layer(" <<  layer
                        << "), phi(" << phi << "), eta(" << eta << "), name(" << name
                        << "), multilayerId(" << ml << ") and identifier " << deadTubeId <<" .");
      }
    }
  }
  std::sort(deadTubes.begin(), deadTubes.end());
  m_DeadChannels[detElId] = deadTubes;
  return;
}

