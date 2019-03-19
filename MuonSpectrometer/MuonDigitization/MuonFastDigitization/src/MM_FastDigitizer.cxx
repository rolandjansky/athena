/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Gaudi - Core
#include "StoreGate/StoreGateSvc.h"

//Inputs
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

//Outputs
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepData.h"

#include "MM_FastDigitizer.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "MuonSimEvent/MMSimHitCollection.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"

#include "Identifier/Identifier.h"
#include "TH1.h"
#include "TTree.h"
#include "TFile.h"

//Random Numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGauss.h"


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace Muon;
/*******************************************************************************/ 
  MM_FastDigitizer::MM_FastDigitizer(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name, pSvcLocator) , m_activeStore(NULL) , m_detManager(NULL) , m_idHelper(NULL)
  , m_file(NULL) , m_ntuple(NULL) , m_dlx(0.) , m_dly(0.) , m_dlz(0.)
  , m_sulx(0.) , m_suly(0.) , m_tsulx(0.) , m_tsuly(0.) , m_tsulz(0.) , m_stsulx(0.) , m_stsuly(0.) , m_stsulz(0.)
  , m_ang(0.) , m_shift(0.) , m_suresx(0.) , m_suresy(0.) , m_err(0.) , m_res(0.)
  , m_pull(0.) , m_is(0) , m_seta(0) , m_sphi(0) , m_sml(0) , m_sl(0) , m_ss(0) , m_ieta(0) , m_iphi(0) , m_iml(0) , m_il(0)
  , m_ich(0) , m_istr(0) , m_exitcode(0) , m_mode(0) , m_pdg(0) , m_trkid(0) , m_gpx(0.) , m_gpy(0.) , m_gpz(0.)
  , m_gpr(0.) , m_gpp(0.) , m_dgpx(0.) , m_dgpy(0.) , m_dgpz(0.), m_dgpr(0.) , m_dgpp(0.) , m_tofCorrection(0.)
  , m_bunchTime(0.) , m_globalHitTime(0.) , m_e(0.) , m_edep(0.) , m_surfcentx(0.) , m_surfcenty(0.) , m_surfcentz(0.)
  , m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool" )
  , m_muonClusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator")
  , m_rndmSvc("AtRndmGenSvc", name )
  , m_rndmEngine(0)
  , m_inputObjectName("MicromegasSensitiveDetector")
  , m_sdoName("MMfast_SDO")
{
  declareProperty("InputObjectName", m_inputObjectName  =  "MicromegasSensitiveDetector", "name of the input object");
  declareProperty("RndmEngine",  m_rndmEngineName, "Random engine name");
  declareProperty("RndmSvc",     m_rndmSvc,        "Random Number Service used in Muon digitization");
  declareProperty("UseTimeShift", m_useTimeShift = true,        "Use time shift");
  declareProperty("EnergyThreshold", m_energyThreshold = 50, "Minimal energy to produce a PRD"  );
  declareProperty("CheckIds", m_checkIds = false,  "Turn on validity checking of Identifiers"  );
  declareProperty("MaskMultiplet", m_maskMultiplet = 0,  "0: all, 1: first, 2: second, 3: both"  );
  declareProperty("SDOname", m_sdoName = "MMfast_SDO"  );
  declareProperty("MicroTPC", m_microTPC = true,  "Turn on microTPC mode"  );
}
/*******************************************************************************/ 
MM_FastDigitizer::~MM_FastDigitizer()  {
}
/*******************************************************************************/ 
StatusCode MM_FastDigitizer::initialize() {
  StatusCode status(StatusCode::SUCCESS);

  // initialize transient event store
  ATH_MSG_DEBUG ( "Retrieved StoreGateSvc." );

  ATH_CHECK( service("ActiveStoreSvc", m_activeStore) );
  ATH_CHECK( m_muonClusterCreator.retrieve() );
  ATH_CHECK( m_sdoName.initialize() );

  // initialize transient detector store and MuonGeoModel OR MuonDetDescrManager
  StoreGateSvc* detStore=0;
  m_detManager=0;
  status = serviceLocator()->service("DetectorStore", detStore);
  if (status.isSuccess()) {
    if(detStore->contains<MuonGM::MuonDetectorManager>( "Muon" )){

      status = detStore->retrieve(m_detManager);
      if (status.isFailure()) {
        ATH_MSG_FATAL ( "Could not retrieve MuonGeoModelDetectorManager!" );
        return status;
      }
      else {
        ATH_MSG_DEBUG ( "Retrieved MuonGeoModelDetectorManager from StoreGate" );
        //initialize the MmIdHelper
        m_idHelper  = m_detManager->mmIdHelper();
        if(!m_idHelper) return status;
        ATH_MSG_DEBUG ( "Retrieved MmIdHelper " << m_idHelper );
      }
    }
  }
  else {
    ATH_MSG_FATAL ( "Could not retrieve DetectorStore!" );
    return status;
  }

  ATH_CHECK( m_idHelperTool.retrieve() );

  // check the input object name
  if (m_inputObjectName=="") {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );
  }

  // getting our random numbers stream
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  ATH_CHECK( m_rndmSvc.retrieve() );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  }


  m_file = new TFile("MM_plots.root","RECREATE");
  m_ntuple = new TTree("a","a");
  m_ntuple->Branch("dlx",&m_dlx);
  m_ntuple->Branch("dly",&m_dly);
  m_ntuple->Branch("dlz",&m_dlz);
  m_ntuple->Branch("sulx",&m_sulx);
  m_ntuple->Branch("suly",&m_suly);
  m_ntuple->Branch("tsulx",&m_tsulx);
  m_ntuple->Branch("tsuly",&m_tsuly);
  m_ntuple->Branch("tsulz",&m_tsulz);
  m_ntuple->Branch("stsulx",&m_stsulx);
  m_ntuple->Branch("stsuly",&m_stsuly);
  m_ntuple->Branch("stsulz",&m_stsulz);
  m_ntuple->Branch("ang",&m_ang);
  m_ntuple->Branch("shift",&m_shift);
  m_ntuple->Branch("suresx",&m_suresx);
  m_ntuple->Branch("suresy",&m_suresy);
  m_ntuple->Branch("err",&m_err);
  m_ntuple->Branch("res",&m_res);
  m_ntuple->Branch("pull",&m_pull);
  m_ntuple->Branch("is",&m_is);
  m_ntuple->Branch("seta",&m_seta);
  m_ntuple->Branch("sphi",&m_sphi);
  m_ntuple->Branch("sml",&m_sml);
  m_ntuple->Branch("sl",&m_sl);
  m_ntuple->Branch("ss",&m_ss);
  m_ntuple->Branch("ieta",&m_ieta);
  m_ntuple->Branch("iphi",&m_iphi);
  m_ntuple->Branch("iml",&m_iml);
  m_ntuple->Branch("il",&m_il);
  m_ntuple->Branch("ich",&m_ich);
  m_ntuple->Branch("istr",&m_istr);
  m_ntuple->Branch("exitcode",&m_exitcode);
  m_ntuple->Branch("mode",&m_mode);
  m_ntuple->Branch("pdg",&m_pdg);
  m_ntuple->Branch("trkid",&m_trkid);
  m_ntuple->Branch("gpx",&m_gpx);
  m_ntuple->Branch("gpy",&m_gpy);
  m_ntuple->Branch("gpz",&m_gpz);
  m_ntuple->Branch("gpr",&m_gpr);
  m_ntuple->Branch("gpp",&m_gpp);
  m_ntuple->Branch("dgpx",&m_dgpx);
  m_ntuple->Branch("dgpy",&m_dgpy);
  m_ntuple->Branch("dgpz",&m_dgpz);
  m_ntuple->Branch("dgpr",&m_dgpr);
  m_ntuple->Branch("dgpp",&m_dgpp);
  m_ntuple->Branch("globalHitTime", &m_globalHitTime);
  m_ntuple->Branch("tofCorrection", &m_tofCorrection);
  m_ntuple->Branch("bunchTime", &m_bunchTime);
  m_ntuple->Branch("e", &m_e);
  m_ntuple->Branch("edep", &m_edep);
  m_ntuple->Branch("surfcentx",&m_surfcentx);
  m_ntuple->Branch("surfcenty",&m_surfcenty);
  m_ntuple->Branch("surfcentz",&m_surfcentz);
  return StatusCode::SUCCESS;

}
/*******************************************************************************/ 
StatusCode MM_FastDigitizer::execute() {




  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> h_sdoContainer(m_sdoName);
  ATH_CHECK( h_sdoContainer.record ( std::make_unique<MuonSimDataCollection>() ) );

  MMPrepDataContainer* prdContainer = new MMPrepDataContainer(m_idHelper->module_hash_max());
  std::string key = "MM_Measurements";
  ATH_MSG_DEBUG(" Done! Total number of MM chambers with PRDS: " << prdContainer->numberOfCollections() << " key " << key);
  ATH_CHECK( evtStore()->record(prdContainer,key) );

  if( m_maskMultiplet == 3 ) {
    
    return StatusCode::SUCCESS;
  }
  // as the MMPrepDataContainer only allows const accesss, need a local vector as well.
  std::vector<MMPrepDataCollection*> localMMVec(m_idHelper->module_hash_max());

  const DataHandle< MMSimHitCollection > collGMSH;
  if ( evtStore()->retrieve( collGMSH,m_inputObjectName ).isFailure()) {
    ATH_MSG_WARNING("No MM hits found in SG");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Retrieved " <<  collGMSH->size() << " MM hits!");

  // Get the MicroMegas Id hit helper
  MM_SimIdToOfflineId simToOffline(*m_idHelper);

  std::map<Identifier,int> hitsPerChannel;
  int nhits = 0;

  MMSimHitCollection::const_iterator iterMM;

  for (iterMM=collGMSH->begin();iterMM!=collGMSH->end();++iterMM) {
    const MMSimHit& hit = *iterMM;

    // SimHits without energy loss are not recorded. 
    // not needed because of already done in sensitive detector
    // https://svnweb.cern.ch/trac/atlasoff/browser/MuonSpectrometer/MuonG4/MuonG4SD/trunk/src/MicromegasSensitiveDetector.cxx?rev=542333#L65
    // if(hit.depositEnergy()==0.) continue;

    m_globalHitTime = hit.globalTime();
    m_tofCorrection = hit.globalPosition().mag()/CLHEP::c_light;
    m_bunchTime = m_globalHitTime - m_tofCorrection;
    const float stripPropagationTime = 0.;
    static const float jitter = 0.;
    float MMDigitTime = m_bunchTime + jitter + stripPropagationTime;

    float timeWindowStrip = 120.; //driftvelocity gap;
    if (MMDigitTime < -timeWindowStrip || MMDigitTime > timeWindowStrip) {
      m_exitcode = 4;
      m_ntuple->Fill();
      continue;
    }

    // Read the information about the Micro Megas hit
    ATH_MSG_VERBOSE("MM hit: r " << hit.globalPosition().perp() << " z " << hit.globalPosition().z() << " mclink " << hit.particleLink() );

    //  convert simHit id to offline layer id; make sanity checks; retrieve the associated detector element.
    int simId = hit.MMId();
    Identifier layid = simToOffline.convert(simId);

    // sanity checks
    if( !m_idHelper->is_mm(layid) ){
      ATH_MSG_WARNING("MM id is not a mm id! " << m_idHelperTool->toString(layid));
    }
    if( !m_idHelper->is_muon(layid) ){
      ATH_MSG_WARNING("MM id is not a muon id! " << m_idHelperTool->toString(layid));
    }
    if( m_idHelper->is_mdt(layid)||m_idHelper->is_rpc(layid)||m_idHelper->is_tgc(layid)||m_idHelper->is_csc(layid)||m_idHelper->is_stgc(layid) ){
      ATH_MSG_WARNING("MM id has wrong technology type! " << m_idHelper->is_mdt(layid) << " " << m_idHelper->is_rpc(layid)
          << " " << m_idHelper->is_tgc(layid) << " " << m_idHelper->is_csc(layid) << " " << m_idHelper->is_stgc(layid) );
    }
    
    // remove hits in masked multiplet
    if( m_maskMultiplet == m_idHelperTool->stgcIdHelper().multilayer(layid) ) continue; 

    // get readout element
    const MuonGM::MMReadoutElement* detEl = m_detManager->getMMReadoutElement(layid);
    if( !detEl ){
      ATH_MSG_WARNING("Failed to retrieve detector element for: " << m_idHelperTool->toString(layid) );
      continue;
    }

    // collections stored per readout element
    IdentifierHash hash;
    m_idHelper->get_module_hash(layid, hash);
    MuonPrepDataCollection<Muon::MMPrepData>* col  = localMMVec[hash];
    if( !col ){
      col = new MMPrepDataCollection(hash);
      col->setIdentifier(m_idHelper->channelID(m_idHelper->parentID(layid), m_idHelper->multilayer(layid),1,1) );
      if( prdContainer->addCollection(col,hash).isFailure() ){
        ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
        delete col;col=0;
        continue;
      }
      localMMVec[hash] = col;
    }

    // surface 
    const Trk::PlaneSurface& surf = detEl->surface(layid);

    //Angle
    const Amg::Vector3D GloDire(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
    Trk::LocalDirection locDire;
    surf.globalToLocalDirection(GloDire, locDire);
    float inAngle_XZ = fabs( locDire.angleXZ() / CLHEP::degree);
    inAngle_XZ = 90. - inAngle_XZ ;

    // compute hit position within the detector element/surfaces
    Amg::Transform3D gToL = detEl->absTransform().inverse();
    Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
    Amg::Vector3D lpos = gToL*hpos;
    // surface local from (that matters)
    Amg::Vector3D slpos = surf.transform().inverse()*hpos;

    // Get MM_READOUT from MMDetectorDescription
    char side = ((int)m_idHelper->stationEta(layid)) < 0 ? 'C' : 'A';
    char sector_l = ((std::string)m_idHelper->stationNameString(m_idHelper->stationName(layid))).substr(2,1)=="L" ? 'L' : 'S';
    MMDetectorHelper aHelper;
    MMDetectorDescription* mm = aHelper.Get_MMDetector(sector_l, abs((int)m_idHelper->stationEta(layid)), (int)m_idHelper->stationPhi(layid), (int)m_idHelper->multilayer(layid), side);
    MMReadoutParameters roParam = mm->GetReadoutParameters();

    Amg::Vector3D  ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
    Amg::Vector3D  ldirTime;
    // the stereo angle vector stores the angles in rad. the vector indices are 0,1,2,3 which map to layers 1,2,3,4
    if ( std::abs( (roParam.stereoAngle).at(m_idHelper->gasGap(layid)-1) ) > 0. )
      ldirTime = ldir;
    else
      ldirTime = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), -hit.globalDirection().z());

    double scale;//, scaletop;
//    double gasgap = 5.;
    
    if (m_microTPC) scale = 0;
    else scale = -slpos.z()/ldir.z();
//    scaletop = (gasgap+slpos.z())/ldir.z();

    Amg::Vector3D hitOnSurface = slpos + scale*ldir;
//    Amg::Vector3D hitOnTopSurface = slpos + scaletop*ldir;
    Amg::Vector3D hitOnSurfaceGlobal = surf.transform()*hitOnSurface;

    // account for time offset 
    const double vdrift = 0.047;
    double shiftTimeOffset = MMDigitTime*vdrift;
    Amg::Vector3D hitAfterTimeShift(hitOnSurface.x(),hitOnSurface.y(),shiftTimeOffset);
    Amg::Vector3D hitAfterTimeShiftOnSurface = hitAfterTimeShift - (shiftTimeOffset/ldirTime.z())*ldirTime;

    double tdrift = 0;

    // resolution = -.01/3 * angle + .64/3.
// smearing needed due to depth of gasgap  
    double resolution;
    if (fabs(inAngle_XZ)<3)
      resolution = .07;
    else
      resolution = ( -.001/3.*fabs(inAngle_XZ) ) + .28/3.;
    double sp = CLHEP::RandGauss::shoot(m_rndmEngine, 0, resolution);
    ATH_MSG_VERBOSE("slpos.z " << slpos.z() << ", ldir " << ldir.z() << ", scale " << scale << ", hitOnSurface.z " << hitOnSurface.z() );
    
    double errX = 0;
    const MuonGM::MuonChannelDesign* design = detEl->getDesign(layid);
    if( !design ){
      ATH_MSG_WARNING("Failed to get design for " << m_idHelperTool->toString(layid) );
    }else{
      errX = fabs(design->inputPitch)/sqrt(12);
      ATH_MSG_DEBUG(" strips inputPitch " << design->inputPitch << " error " << errX);
    }
// add strip width to error 
    resolution = sqrt(resolution*resolution+errX*errX); 

    if( fabs(hitOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("bad propagation to surface " << hitOnSurface );
    if( fabs(hitAfterTimeShiftOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("bad propagation to surface after time shift " << hitAfterTimeShiftOnSurface );
    // smeared local position
   Amg::Vector2D posOnSurf(hitOnSurface.x()+sp,hitOnSurface.y());

//    int digiMode = 0;
    // for large angles project perpendicular to surface
    if( fabs(inAngle_XZ) > 70 ){
      posOnSurf[0]=(slpos.x()+sp);
//      digiMode = 1;
      // if using timing information use hit position after shift
    }else if( m_useTimeShift && !m_microTPC ){
      posOnSurf[0]=(hitAfterTimeShiftOnSurface.x()+sp);
//      digiMode = 2;
    }
    
    ////// fill first part of ntuple
    Amg::Vector3D repos = detEl->globalPosition();
    MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();

    std::string stName = m_idHelper->stationNameString(m_idHelper->stationName(layid));
    int isSmall = stName[2] == 'S';
    m_dlx = lpos.x();
    m_dly = lpos.y();
    m_sulx = posOnSurf.x();
    m_suly = posOnSurf.y();
    m_tsulx = hitOnSurface.x();
    m_tsuly = hitOnSurface.y();
    m_tsulz = hitOnSurface.z();
    m_stsulx = hitAfterTimeShiftOnSurface.x();
    m_stsuly = hitAfterTimeShiftOnSurface.y();
    m_stsulz = hitAfterTimeShiftOnSurface.z();
    m_ang = inAngle_XZ;
    m_shift  = shiftTimeOffset;
    m_suresx = posOnSurf.x()-hitOnSurface.x();
    m_suresy = posOnSurf.y()-hitOnSurface.y();
    m_err  = -99999.; 
    m_res  = -99999.; 
    m_pull  = -99999.; 
    m_is   = isSmall;
    m_seta =  hitHelper->GetZSector(simId);
    m_sphi = hitHelper->GetPhiSector(simId);
    m_sml  = hitHelper->GetMultiLayer(simId);
    m_sl  = hitHelper->GetLayer(simId);
    m_ss  = hitHelper->GetSide(simId);
    m_ieta = m_idHelper->stationEta(layid);
    m_iphi = m_idHelper->stationPhi(layid);
    m_iml  = m_idHelper->multilayer(layid);
    m_il  = m_idHelper->gasGap(layid);
    m_ich = -99999;
    m_istr  = -99999;
    m_exitcode = 0;
    m_mode = 0;
    m_pdg = hit.particleEncoding();
    m_trkid = hit.trackNumber();
    m_gpx  = hit.globalPosition().x();
    m_gpy  = hit.globalPosition().y();
    m_gpz  = hit.globalPosition().z();
    m_gpr  = hit.globalPosition().perp();
    m_gpp  = hit.globalPosition().phi();
    m_dgpx  = repos.x();
    m_dgpy  = repos.y();
    m_dgpz  = repos.z();
    m_dgpr  = repos.perp();
    m_dgpp  = repos.phi();
    m_edep  = hit.depositEnergy();
    m_e     = hit.kineticEnergy();
    m_surfcentx = surf.center().x();
    m_surfcenty = surf.center().y();
    m_surfcentz = surf.center().z();

    /////
    if(hit.kineticEnergy()<m_energyThreshold && abs(hit.particleEncoding())==11) {
      m_exitcode = 5;
      m_ntuple->Fill();      
      continue;
    }

    // perform bound check
    if( !surf.insideBounds(posOnSurf) ){
      m_exitcode = 1;
      m_ntuple->Fill();
      continue;
    }

    // recalculate strip number after smearing
    int stripNumber = detEl->stripNumber(posOnSurf,layid);
    //int LastStripNumber = detEl->stripNumber(posOnTopSurf, layid);

    if( stripNumber == -1 ){
      ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperTool->toString(layid) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << slpos.z() ); 
      m_exitcode = 2;
      m_ntuple->Fill();
      continue;
    }

    // Recalculate the Identifier using the strip number
    // here need to use parent ID to avoid creating corrupted identifiers
    Identifier parentID = m_idHelper->parentID(layid);
    Identifier id = m_idHelper->channelID(parentID, m_idHelper->multilayer(layid), m_idHelper->gasGap(layid),stripNumber,m_checkIds);

    // only one hit per channel
    int& counts = hitsPerChannel[id];
    ++counts;
    if( counts > 1 ) continue;
    ++nhits;


    if( stripNumber >= detEl->numberOfStrips(layid) ){
      ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperTool->toString(layid) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << slpos.z() ); 
      m_exitcode = 3;
      m_ntuple->Fill();
      continue;
    }

    // Recalculate the Identifier using the strip number
    // here need to use parent ID to avoid creating corrupted identifiers
    id = m_idHelper->channelID(parentID, m_idHelper->multilayer(layid), m_idHelper->gasGap(layid),stripNumber,m_checkIds);

    if( stripNumber != m_idHelper->channel(id) ) {
      ATH_MSG_WARNING(" bad stripNumber: in  "  << stripNumber << " from id " << m_idHelper->channel(id));
      m_exitcode = 4;
      continue;
    }

    std::vector<Identifier> rdoList;
    rdoList.push_back(id);

    ATH_MSG_VERBOSE("Global hit: r " << hit.globalPosition().perp() << " phi " << hit.globalPosition().phi() << " z " << hit.globalPosition().z());
    ATH_MSG_VERBOSE(" Calculated truth hitOnSurfaceGlobal: r " << hitOnSurfaceGlobal.perp() << " phi " << hitOnSurfaceGlobal.phi() << " z " << hitOnSurfaceGlobal.z());
    ATH_MSG_VERBOSE(" detEl: r " << repos.perp() << " phi " << repos.phi() << " z " << repos.z());
    Amg::Vector3D  gdir = surf.transform().linear()*Amg::Vector3D(0.,1.,0.);
    ATH_MSG_DEBUG(" MM detector surface direction phi " << gdir.phi());
    ATH_MSG_VERBOSE(" Surface center: r " << surf.center().perp() << " phi " << surf.center().phi() << " z " << surf.center().z());
    ATH_MSG_DEBUG(" hit:  " << m_idHelperTool->toString(id) << " hitx " << posOnSurf.x() << " hitOnSurface.x() " << hitOnSurface.x() << " residual " << posOnSurf.x() - hitOnSurface.x()
		  << " pull " << (posOnSurf.x() - hitOnSurface.x())/resolution );
    Amg::Vector3D CurrentHitInDriftGap = slpos;
    // emulating micro track in the drift volume for microTPC
    if (!m_microTPC) {
      Amg::MatrixX* cov = new Amg::MatrixX(1,1);
      cov->setIdentity();
      double scaleError = 1.;
      (*cov)(0,0) = scaleError*resolution*resolution;

      double locx = posOnSurf.x();
      Amg::Vector2D localPrd;
      bool getLocalPos = detEl->stripPosition(id,localPrd);
      if ( !getLocalPos ) continue;
      ATH_MSG_VERBOSE(" posOnSurf.x locx " << locx << " from strip position locx " << localPrd[0] << " locy " << localPrd[1]); 
      MMPrepData* prd = new MMPrepData( id,hash,localPrd,rdoList,cov,detEl,(int)tdrift,0);
      prd->setHashAndIndex(col->identifyHash(), col->size()); // <<< add this line to the MM code as well
      col->push_back(prd);

      std::vector<MuonSimData::Deposit> deposits;
      MuonSimData::Deposit deposit(hit.particleLink(), MuonMCData(hitOnSurface.x(),hitOnSurface.y()));
      deposits.push_back(deposit);

      MuonSimData simdata(deposits,0);
      simdata.setPosition(hitOnSurfaceGlobal);
      simdata.setTime(m_globalHitTime);
      h_sdoContainer->insert ( std::make_pair ( id, simdata ) );

      ATH_MSG_VERBOSE(" Prd: local x " << posOnSurf.x() << " y " << 0 );
      ATH_MSG_VERBOSE(" Prd: r " << prd->globalPosition().perp() << "  phi " << prd->globalPosition().phi() << " z " << prd->globalPosition().z());
    } else {
      for (int loop_direction = -1; loop_direction <=1; loop_direction+=2) {
        Amg::Vector3D stepInDriftGap = loop_direction * ldir * (roParam.stripPitch/std::cos(roParam.stereoAngle.at(m_idHelper->gasGap(layid)-1) ))/abs(ldir.x());
        if (loop_direction == 1) CurrentHitInDriftGap = slpos + stepInDriftGap;
        while (std::abs(CurrentHitInDriftGap.z()) <= roParam.gasThickness) {
          Amg::MatrixX* cov = new Amg::MatrixX(1,1);
          cov->setIdentity();
          (*cov)(0,0) = resolution*resolution;

          tdrift = CurrentHitInDriftGap.z() / vdrift + CLHEP::RandGauss::shoot(m_rndmEngine, 0., 5.);
          Amg::Vector2D CurrenPosOnSurf(CurrentHitInDriftGap.x(),CurrentHitInDriftGap.y());

          stripNumber = detEl->stripNumber(CurrenPosOnSurf,layid);
          if( (stripNumber >= detEl->numberOfStrips(layid)) || (stripNumber == -1) ) {
            CurrentHitInDriftGap += stepInDriftGap;
            continue;
          }
          id = m_idHelper->channelID(parentID, m_idHelper->multilayer(layid), m_idHelper->gasGap(layid),stripNumber,m_checkIds);
          if( stripNumber != m_idHelper->channel(id) ) {
            CurrentHitInDriftGap += stepInDriftGap;
            continue;
          }
          Amg::Vector2D localPrd;
          bool getLocalPos = detEl->stripPosition(id,localPrd);
          if(getLocalPos) {
           MMPrepData* prd = new MMPrepData( id,hash,localPrd,rdoList,cov,detEl,(int)tdrift,0);
           prd->setHashAndIndex(col->identifyHash(), col->size()); // <<< add this line to the MM code as well
           col->push_back(prd);

           std::vector<MuonSimData::Deposit> deposits;
           MuonSimData::Deposit deposit(hit.particleLink(), MuonMCData(CurrenPosOnSurf.x(),CurrenPosOnSurf.y()));
           deposits.push_back(deposit);
 
           MuonSimData simdata(deposits,0);
           Amg::Vector3D SDO_GP = surf.transform()*CurrentHitInDriftGap;
           simdata.setPosition(SDO_GP);
           simdata.setTime(m_globalHitTime);
           h_sdoContainer->insert ( std::make_pair ( id, simdata ) );

           ATH_MSG_VERBOSE(" Local CurrentHitInDriftGap.x() " << CurrentHitInDriftGap.x() << " CurrentHitInDriftGap.y() " << CurrentHitInDriftGap.y() << " CurrentHitInDriftGap.z() " << CurrentHitInDriftGap.z() << " drift time " << (int)tdrift );
           ATH_MSG_VERBOSE(" Prd: local x " << CurrentHitInDriftGap.x() << " y " << 0 << " drift time " << (int)tdrift << " identifier " << id );
           ATH_MSG_VERBOSE(" Prd: r " << prd->globalPosition().perp() << "  phi " << prd->globalPosition().phi() << " z " << prd->globalPosition().z());
           ATH_MSG_VERBOSE(" SDO: True Global position: x  " << SDO_GP.x() << "  y " << SDO_GP.y() << " z " << SDO_GP.z());
          }
          CurrentHitInDriftGap += stepInDriftGap;
        }
      }
    }


    m_err = resolution;
    m_ich = m_idHelper->channel(id);
    m_istr  = stripNumber;

//     const MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack( *prd, hit.globalPosition() );
//     if( rot ){
//       m_res  = rot->localParameters().get(Trk::locX)-hitOnSurface.x();
//       m_pull = m_res/rot->localErrorMatrix().error(Trk::locX);
//       delete rot;
//     }

    m_ntuple->Fill();
    // OLD CODE ENDS HERE

  } 

  if( msgLvl(MSG::DEBUG) ){
    std::map<Identifier,int>::const_iterator hit = hitsPerChannel.begin();
    std::map<Identifier,int>::const_iterator hit_end = hitsPerChannel.end();
    msg(MSG::DEBUG) << " number of channels with hit " << hitsPerChannel.size() << " nhits " << nhits << std::endl;
    for( ;hit!=hit_end;++hit ){
      msg(MSG::DEBUG) << " " << m_idHelperTool->toString(hit->first) << " ->  " << hit->second << std::endl;
    }
    msg(MSG::DEBUG) << endmsg;
  }
  return StatusCode::SUCCESS;
}
/*******************************************************************************/ 
StatusCode MM_FastDigitizer::finalize() {
  m_ntuple->Write();
  m_file->Write();
  m_file->Close();
  return StatusCode::SUCCESS;
}
/*******************************************************************************/ 
/** Function to convert Radians into Degrees  */
float MM_FastDigitizer::RadsToDegrees(float Radians)
{
  float Degrees = Radians * (180.) / M_PI;
  return Degrees;
}
