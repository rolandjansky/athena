/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimHitCreatorMS.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_FatrasToolsMS/SimHitCreatorMS.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Interfaces/IParticleHelper.h"
// Trk
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"
// MuonSpectrometer includes
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"

#include "MuonTGRecTools/IMuonTGMeasTool.h"

#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Random/RandGauss.h"

//================ Constructor =================================================

iFatras::SimHitCreatorMS::SimHitCreatorMS(const std::string& t,
    const std::string& n,
    const IInterface*  p ) : 
 AthAlgTool(t,n,p),
    m_incidentSvc("IncidentSvc", n),
    m_extrapolator(""),
    m_measTool("Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"),
    m_mdtCollectionName("MDT_Hits"),
    m_rpcCollectionName("RPC_Hits"),
    m_tgcCollectionName("TGC_Hits"),
    m_cscCollectionName("CSC_Hits"),
    m_mmCollectionName("MM_Hits"),
    m_stgcCollectionName("sTGC_Hits"),
    m_randomSvc("AtDSFMTGenSvc", n),
    m_randomEngineName("FatrasRnd"),
    m_randomEngine(0),  
    m_mdtHitIdHelper(0),
    m_rpcHitIdHelper(0),
    m_cscHitIdHelper(0),
    m_tgcHitIdHelper(0),
    m_mmOffToSimId(NULL),
    m_stgcOffToSimId(NULL),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_muonMgr(0),
    m_mdtSigmaDriftRadius(0.08),
    m_createAllMdtHits(true)
{
    declareInterface<ISimHitCreator>(this);
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

//================ Destructor =================================================

iFatras::SimHitCreatorMS::~SimHitCreatorMS()
{}


//================ Initialisation =================================================

StatusCode iFatras::SimHitCreatorMS::initialize()
{

  // Get Extrapolator from ToolService
  if (m_extrapolator.retrieve().isFailure()) {
      ATH_MSG_FATAL(  "[ --- ] Could not retrieve " << m_extrapolator );
      return StatusCode::FAILURE;
  }
  // Get IdHelper from ToolService
  if (m_idHelperTool.retrieve().isFailure()) {
      ATH_MSG_FATAL(  "[ --- ] Could not retrieve " << m_idHelperTool );
      return StatusCode::FAILURE;
  }
  // the MS helpers for the different technologies
  m_mdtHitIdHelper = MdtHitIdHelper::GetHelper(); 
  m_rpcHitIdHelper = RpcHitIdHelper::GetHelper(); 
  m_tgcHitIdHelper = TgcHitIdHelper::GetHelper(); 
  m_cscHitIdHelper = CscHitIdHelper::GetHelper(); 
  // m_sTgcHitIdHelper = sTgcHitIdHelper::GetHelper(); 
  // m_mmHitIdHelper = MicromegasHitIdHelper::GetHelper(); 


  if (detStore()->retrieve(m_muonMgr).isFailure()) {
      ATH_MSG_FATAL( "[ --- ] Cannot retrieve MuonDetectorManager..." );
      return StatusCode::FAILURE;
  }

  m_mmOffToSimId = new MM_SimIdToOfflineId(*(m_muonMgr->mmIdHelper())); 
  m_stgcOffToSimId = new sTgcSimIdToOfflineId(*(m_muonMgr->stgcIdHelper())); 
  
 // get measurement tool
  if (m_measTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "[ --- ] Could not find TG measurement tool " );
      return StatusCode::FAILURE;
  } else {
      ATH_MSG_INFO( "[ muhit ] TG measurement tool booked ");
  }

  // Random number service
  if ( m_randomSvc.retrieve().isFailure() ) {
      ATH_MSG_ERROR( "[ --- ] Could not retrieve " << m_randomSvc );
      return StatusCode::FAILURE;
  }
  //Get own engine with own seeds:
  m_randomEngine = m_randomSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
      ATH_MSG_ERROR( "[ --- ] Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
  }


  // Athena/Gaudi framework
  if (m_incidentSvc.retrieve().isFailure()){
      ATH_MSG_WARNING("[ sihit ] Could not retrieve " << m_incidentSvc << ". Exiting.");
      return StatusCode::FAILURE;
  }
  // register to the incident service: BeginEvent for TrackCollection
  m_incidentSvc->addListener( this, IncidentType::BeginEvent);

  ATH_MSG_INFO( "[ mutrack ] initialize() successful." );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode iFatras::SimHitCreatorMS::finalize()
{
    StatusCode sc = AlgTool::finalize();
    return sc;
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
      if ( evtStore()->contains<GenericMuonSimHitCollection>(m_mmCollectionName) ){
	if ( (evtStore()->retrieve(m_mmSimHitCollection , m_mmCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve GenericMuonSimHitCollection " << m_mmCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_mmSimHitCollection = new GenericMuonSimHitCollection( m_mmCollectionName);
	if ( (evtStore()->record(m_mmSimHitCollection, m_mmCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record GenericMuonSimHitCollection " << m_mmCollectionName);
             delete m_mmSimHitCollection; m_mmSimHitCollection=0;
        }
      }
      if ( evtStore()->contains<GenericMuonSimHitCollection>(m_stgcCollectionName) ){
	if ( (evtStore()->retrieve(m_stgcSimHitCollection , m_stgcCollectionName)).isFailure() )
	  ATH_MSG_ERROR( "[ --- ] Unable to retrieve GenericMuonSimHitCollection " << m_stgcCollectionName);
	// (b)     if no ... try to create it      
      } else {
	m_stgcSimHitCollection = new GenericMuonSimHitCollection( m_stgcCollectionName);
	if ( (evtStore()->record(m_stgcSimHitCollection, m_stgcCollectionName, true)).isFailure() ) {
             ATH_MSG_ERROR( "[ --- ] Unable to record GenericMuonSimHitCollection " << m_stgcCollectionName);
             delete m_stgcSimHitCollection; m_stgcSimHitCollection=0;
        }
      }
   }
   return;
}


//================ Track Creation Interface  =====================================
void iFatras::SimHitCreatorMS::createHits(const ISF::ISFParticle& isp,
					  const std::vector<Trk::HitInfo>& hits) const {

  //std::cout <<"creating MS hits"<< hits.size()<< std::endl;
    
  // iterate and assign as well the layer
  std::vector<Trk::HitInfo>::const_iterator plIter    = hits.begin();
  std::vector<Trk::HitInfo>::const_iterator plIterEnd = hits.end();
  for ( ; plIter != plIterEnd; ++plIter ){
    // get the parameters & associated layer
    const Trk::TrackParameters* parm = (*plIter).trackParms;
    double timeInfo = (*plIter).time;
    const Trk::Layer*        currLay = m_extrapolator->trackingGeometry()->associatedLayer( parm->position() );
    //std::cout <<"hit position:"<<parm->position()<<","<<currLay <<":"<<parm->position().mag()<< std::endl; 
    //std::cout <<"hit timing:"<< timeInfo << std::endl;

    if (!currLay) continue;

    Identifier id(currLay->layerType());
 
    // NSW hits
    if ( m_idHelperTool->mdtIdHelper().is_mm(id) ||  m_idHelperTool->mdtIdHelper().is_stgc(id) ) {   
      // hit ID
      int simID = offIdToSimId(id);
      // local position : at MTG layer ( corresponds to the middle of the gas gap ) 
      const Amg::Vector3D locPos= currLay->surfaceRepresentation().transform().inverse()*parm->position();
      //Trk::GlobalPosition locPosRot = HepGeom::RotateY3D(M_PI)*HepGeom::RotateZ3D(+M_PI/2.)*currLay->surfaceRepresentation().transform().inverse()*parm->position();
      //std::cout << "local,local rotated:" << locPos<<"," << locPosRot << std::endl;
      // generating particle info
      double mom  = parm->momentum().mag();
      double mass = isp.mass();
      double eKin = sqrt( mom*mom+mass*mass) - mass;
      // the rest of information needs adjustment once full sim hits available
      double energyDeposit = 1.;
      double stepLength = 1.;
      const Amg::Vector3D pos=parm->position();
      const Amg::Vector3D unitMom=parm->momentum().normalized();
      GenericMuonSimHit nswHit = GenericMuonSimHit(simID,timeInfo,timeInfo, pos, locPos, pos, locPos, 
						   isp.pdgCode(),eKin,unitMom, 
						   energyDeposit,stepLength, isp.barcode()) ;
      
      if ( m_muonMgr->mmIdHelper()->is_mm(id) )  m_mmSimHitCollection->Insert(nswHit); 
      else  m_stgcSimHitCollection->Insert(nswHit); 

      ATH_MSG_VERBOSE("[ muhit ] NSW hit created.");           
         
      /*
	
      // validation/fast digit 
      if ( m_muonMgr->mmIdHelper()->is_mm(id) ) {
        const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(id);
        if (mm) {
          Trk::GlobalPosition g2re = mm->transform(id).inverse()*(*plIter)->position();
	  std::cout <<"MM local position:MTG,MRG:"<< locPos <<","<<g2re<<std::endl;
	  std::cout <<currLay->surfaceRepresentation().center()<< ","<<mm->center(id)<< std::endl;
	  Trk::LocalPosition lp(g2re.x(),g2re.y());         
	  int nCh = mm->stripNumber(lp,id);
	  std::cout <<"MM layer, strip number,length:"<<  m_muonMgr->mmIdHelper()->gasGap(id) <<","
	  <<nCh<<","<<mm->getDesign(id)->channelLength(nCh)<<":"<<  mm->getDesign(id)->gasGapThickness() << std::endl;
	  std::cout<<"distance to nearest channel, readout:"<<mm->getDesign(id)->distanceToChannel(lp) <<","<< mm->distanceToReadout(lp,id)<<std::endl;     
        }
      } else if ( m_muonMgr->stgcIdHelper()->is_stgc(id) ) {
        const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(id);
	if (stgc) {
	  Trk::GlobalPosition g2re = stgc->transform(id).inverse()*(*plIter)->position();
	  std::cout <<"STGC local position:MTG,MRG:"<< locPos<<","<<g2re<<std::endl;
	  Trk::LocalPosition lp(g2re.x(),g2re.y());         
	  std::cout <<currLay->surfaceRepresentation().center()<< ","<<stgc->center(id)<<"," <<m_muonMgr->stgcIdHelper()->channelType(id)<<  std::endl;
	  int nCh = stgc->stripNumber(lp,id);
	  std::cout << "channel number:"<< nCh << std::endl; 
	  std::cout <<"sTGC layer, strip number,length:"<<  m_muonMgr->stgcIdHelper()->gasGap(id) <<","
	  <<nCh<<","<<stgc->getDesign(id)->channelLength(nCh)<<"," << stgc->getDesign(id)->gasGapThickness() << std::endl;
	  std::cout <<"distance to nearest channel, readout:"<<stgc->getDesign(id)->distanceToChannel(lp) <<","<< stgc->distanceToReadout(lp,id)<<std::endl;
	  
	  // test wire gangs
	  Identifier wid=m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
	  m_muonMgr->stgcIdHelper()->stationEta(id),
	  m_muonMgr->stgcIdHelper()->stationPhi(id),
	  m_muonMgr->stgcIdHelper()->multiplet(id),
	  m_muonMgr->stgcIdHelper()->gasGap(id),2,1);
	  int nW = stgc->stripNumber(lp,wid);
	  std::cout << "wire gang number:"<< nW << std::endl; 
	  std::cout <<"sTGC layer, wire number,length:"<<  m_muonMgr->stgcIdHelper()->gasGap(wid) <<","
	  <<nW<<","<<stgc->getDesign(wid)->channelLength(nW)<<":"<< stgc->getDesign(wid)->gasGapThickness() <<  std::endl;
	  std::cout<<"distance to nearest channel, readout:"<<stgc->getDesign(wid)->distanceToChannel(lp) <<","<< stgc->distanceToReadout(lp,wid)<<std::endl;
	  
	}
      }
      */   // end NSW validation
    }  else if (m_idHelperTool->mdtIdHelper().is_mdt(id)) {    // (A) special treatment for MDTs to find closest channel and nearby hits
      double pitch = 0.;
      // get the identifier 
      Identifier hid = m_measTool->nearestDetEl(currLay,parm,false,pitch);
      // 
      if (m_idHelperTool->mdtIdHelper().valid(hid)) {
	// create first hit 
	bool hitCreated = createHit(isp, currLay,parm,hid,timeInfo,pitch, true);
      
	if (m_createAllMdtHits) {
	  // nearby hits - check range 
	  const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(hid);  
	  if (!mdtROE) continue;	   
	  int tMax = mdtROE->getNtubesperlayer();
	  int tCur = m_idHelperTool->mdtIdHelper().tube(hid);  
	  // recalculate id
	  int next=-1; 
	  while (tCur+next>0) {
	    Identifier nextId = m_idHelperTool->mdtIdHelper().channelID(m_idHelperTool->mdtIdHelper().stationName(hid),
									m_idHelperTool->mdtIdHelper().stationEta(hid),
									m_idHelperTool->mdtIdHelper().stationPhi(hid),
									m_idHelperTool->mdtIdHelper().multilayer(hid),
									m_idHelperTool->mdtIdHelper().tubeLayer(hid),
									tCur+next);
	    if (!m_idHelperTool->mdtIdHelper().valid(nextId)) break;
	    hitCreated = createHit(isp, currLay,parm,nextId,timeInfo,pitch,true);
	    if (!hitCreated) break;
	    next--;
	  }
	  next = 1;
	  while (tCur+next <= tMax) {
	    Identifier nextId = m_idHelperTool->mdtIdHelper().channelID(m_idHelperTool->mdtIdHelper().stationName(hid),
									m_idHelperTool->mdtIdHelper().stationEta(hid),
									m_idHelperTool->mdtIdHelper().stationPhi(hid),
									m_idHelperTool->mdtIdHelper().multilayer(hid),
									m_idHelperTool->mdtIdHelper().tubeLayer(hid),
									tCur+next);
	    if (!m_idHelperTool->mdtIdHelper().valid(nextId)) break;
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
   if (m_idHelperTool->mdtIdHelper().is_mdt(id)) {
            
     int simId = m_mdtHitIdHelper->BuildMdtHitId(m_idHelperTool->mdtIdHelper().stationNameString(m_idHelperTool->mdtIdHelper().stationName(id)),
						 m_idHelperTool->mdtIdHelper().stationPhi(id), m_idHelperTool->mdtIdHelper().stationEta(id),
						 m_idHelperTool->mdtIdHelper().multilayer(id), m_idHelperTool->mdtIdHelper().tubeLayer(id),
						 m_idHelperTool->mdtIdHelper().tube(id));
     
     ATH_MSG_VERBOSE(  "[ muhit ] Creating MDTSimHit with identifier " <<  simId );

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
  } else if (m_idHelperTool->rpcIdHelper().is_rpc(id)) { 
    // local position from the rpc's
    const Amg::Vector3D localPos = m_muonMgr->getRpcReadoutElement(id)->globalToLocalCoords(parm->position(),id);
    int simId = m_rpcHitIdHelper->BuildRpcHitId(m_idHelperTool->rpcIdHelper().stationNameString(m_idHelperTool->rpcIdHelper().stationName(id)),
						m_idHelperTool->rpcIdHelper().stationPhi(id), m_idHelperTool->rpcIdHelper().stationEta(id),
						m_idHelperTool->rpcIdHelper().doubletZ(id),   m_idHelperTool->rpcIdHelper().doubletR(id),
						m_idHelperTool->rpcIdHelper().gasGap(id),     m_idHelperTool->rpcIdHelper().doubletPhi(id),
						m_idHelperTool->rpcIdHelper().measuresPhi(id));
    
    ATH_MSG_VERBOSE(  "[ muhit ] Creating RPCSimHit with identifier " <<  simId );

    double energyDeposit= 1.5e-03 + 3.9e-04*CLHEP::RandLandau::shoot(m_randomEngine);
    while (energyDeposit<0.)  energyDeposit= 1.5e-03 + 3.9e-04*CLHEP::RandLandau::shoot(m_randomEngine);
    
    // a new simhit
    RPCSimHit rpcHit = RPCSimHit(simId,globalTimeEstimate, localPos, isp.barcode(), localPos, energyDeposit,1.,isp.pdgCode(),isp.momentum().mag() ) ; 
    m_rpcSimHitCollection->Insert(rpcHit); 

  } else if (m_idHelperTool->tgcIdHelper().is_tgc(id) && !m_idHelperTool->tgcIdHelper().isStrip(id) ) { 
    
    // take eta hits only
    // local position
    const Amg::Vector3D localPos = Amg::Translation3D(-1.4, 0., 0.)*m_muonMgr->getTgcReadoutElement(id)->globalToLocalCoords(parm->position(),id);
    // local direction
    Amg::Vector3D localDir = m_muonMgr->getTgcReadoutElement(id)->globalToLocalTransf(id).rotation()*parm->momentum().normalized();
    
    int simId = m_tgcHitIdHelper->BuildTgcHitId(m_idHelperTool->tgcIdHelper().stationNameString(m_idHelperTool->tgcIdHelper().stationName(id)),
						m_idHelperTool->tgcIdHelper().stationPhi(id), m_idHelperTool->tgcIdHelper().stationEta(id),
						m_idHelperTool->tgcIdHelper().gasGap(id));
    
    ATH_MSG_VERBOSE(  "[ muhit ] Creating TGCSimHit with identifier " <<  simId );
    
    // TO DO: adust energy deposit and step length ( 1keV,3 mm ? )
    double energyDeposit= 1.3e-03 + 6.e-04*CLHEP::RandGauss::shoot(m_randomEngine);
    while (energyDeposit<0.) energyDeposit= 1.3e-03 + 6.e-04*CLHEP::RandGauss::shoot(m_randomEngine);
    double stepLength=3.;
    
    // a new simhit
    TGCSimHit tgcHit = TGCSimHit(simId,globalTimeEstimate, localPos, localDir, isp.barcode(), energyDeposit, stepLength ) ;
    m_tgcSimHitCollection->Insert(tgcHit); 
  } else if (m_idHelperTool->cscIdHelper().is_csc(id)) { 
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
    
    int simId = m_cscHitIdHelper->BuildCscHitId(m_idHelperTool->cscIdHelper().stationNameString(m_idHelperTool->cscIdHelper().stationName(id)),
						m_idHelperTool->cscIdHelper().stationPhi(id), m_idHelperTool->cscIdHelper().stationEta(id),
						m_idHelperTool->cscIdHelper().chamberLayer(id), m_idHelperTool->cscIdHelper().wireLayer(id));
    
    ATH_MSG_VERBOSE(  "[ muhit ] Creating CSCSimHit with identifier " <<  simId );
    
    // a new simhit
    CSCSimHit cscHit = CSCSimHit(simId,globalTimeEstimate,energyDeposit, hitStart, hitEnd, lundcode, isp.barcode() ) ;
    m_cscSimHitCollection->Insert(cscHit); 
  }
  
  return false; 
}

int iFatras::SimHitCreatorMS::offIdToSimId(Identifier id) const{

  if ( m_muonMgr->mmIdHelper()->is_mm(id) ) {
  
    int simID = m_mmOffToSimId->convert(id);

    //verify
    Identifier check_id = m_mmOffToSimId->convert(simID);
    
    if ( check_id != id ) {

      ATH_MSG_WARNING("MM Id conversion error!");
      ATH_MSG_WARNING(m_muonMgr->mmIdHelper()->print_to_string(id));
      ATH_MSG_WARNING(m_muonMgr->mmIdHelper()->print_to_string(check_id));

    }

    return simID;
  }

  if ( m_muonMgr->stgcIdHelper()->is_stgc(id) ) {
      
    int simID = m_stgcOffToSimId->convert(id);

    //verify
    Identifier check_id = m_stgcOffToSimId->convert(simID);
    
    if ( check_id != id ) {

      ATH_MSG_WARNING("sTGC Id conversion error!");
      ATH_MSG_WARNING(m_muonMgr->stgcIdHelper()->print_to_string(id));
      ATH_MSG_WARNING(m_muonMgr->stgcIdHelper()->print_to_string(check_id));

    }

    return simID;
  }


  return 0; 
}

