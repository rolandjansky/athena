/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
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
  , m_file(NULL) , m_ntuple(NULL) , slx(0.) , sly(0.) , slz(0.) , dlx(0.) , dly(0.) , dlz(0.)
  , sulx(0.) , suly(0.) , tsulx(0.) , tsuly(0.) , tsulz(0.) , stsulx(0.) , stsuly(0.) , stsulz(0.)
  , ang(0.) , shift(0.) , resx(0.) , resy(0.) , resz(0.) , suresx(0.) , suresy(0.) , err(0.) , res(0.)
  , pull(0.) , is(0) , seta(0) , sphi(0) , sml(0) , sl(0) , ss(0) , ieta(0) , iphi(0) , iml(0) , il(0)
  , ich(0) , istr(0) , exitcode(0) , mode(0) , pdg(0) , trkid(0) , gpx(0.) , gpy(0.) , gpz(0.)
  , gpr(0.) , gpp(0.) , dgpx(0.) , dgpy(0.) , dgpz(0.), dgpr(0.) , dgpp(0.) , tofCorrection(0.)
  , bunchTime(0.) , globalHitTime(0.) , e(0.) , edep(0.) , surfcentx(0.) , surfcenty(0.) , surfcentz(0.)
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
  m_ntuple->Branch("slx",&slx);
  m_ntuple->Branch("sly",&sly);
  m_ntuple->Branch("slz",&slz);
  m_ntuple->Branch("dlx",&dlx);
  m_ntuple->Branch("dly",&dly);
  m_ntuple->Branch("dlz",&dlz);
  m_ntuple->Branch("sulx",&sulx);
  m_ntuple->Branch("suly",&suly);
  m_ntuple->Branch("tsulx",&tsulx);
  m_ntuple->Branch("tsuly",&tsuly);
  m_ntuple->Branch("tsulz",&tsulz);
  m_ntuple->Branch("stsulx",&stsulx);
  m_ntuple->Branch("stsuly",&stsuly);
  m_ntuple->Branch("stsulz",&stsulz);
  m_ntuple->Branch("ang",&ang);
  m_ntuple->Branch("shift",&shift);
  m_ntuple->Branch("resx",&resx);
  m_ntuple->Branch("resy",&resy);
  m_ntuple->Branch("suresx",&suresx);
  m_ntuple->Branch("suresy",&suresy);
  m_ntuple->Branch("resz",&resz);
  m_ntuple->Branch("err",&err);
  m_ntuple->Branch("res",&res);
  m_ntuple->Branch("pull",&pull);
  m_ntuple->Branch("is",&is);
  m_ntuple->Branch("seta",&seta);
  m_ntuple->Branch("sphi",&sphi);
  m_ntuple->Branch("sml",&sml);
  m_ntuple->Branch("sl",&sl);
  m_ntuple->Branch("ss",&ss);
  m_ntuple->Branch("ieta",&ieta);
  m_ntuple->Branch("iphi",&iphi);
  m_ntuple->Branch("iml",&iml);
  m_ntuple->Branch("il",&il);
  m_ntuple->Branch("ich",&ich);
  m_ntuple->Branch("istr",&istr);
  m_ntuple->Branch("exitcode",&exitcode);
  m_ntuple->Branch("mode",&mode);
  m_ntuple->Branch("pdg",&pdg);
  m_ntuple->Branch("trkid",&trkid);
  m_ntuple->Branch("gpx",&gpx);
  m_ntuple->Branch("gpy",&gpy);
  m_ntuple->Branch("gpz",&gpz);
  m_ntuple->Branch("gpr",&gpr);
  m_ntuple->Branch("gpp",&gpp);
  m_ntuple->Branch("dgpx",&dgpx);
  m_ntuple->Branch("dgpy",&dgpy);
  m_ntuple->Branch("dgpz",&dgpz);
  m_ntuple->Branch("dgpr",&dgpr);
  m_ntuple->Branch("dgpp",&dgpp);
  m_ntuple->Branch("globalHitTime", &globalHitTime);
  m_ntuple->Branch("tofCorrection", &tofCorrection);
  m_ntuple->Branch("bunchTime", &bunchTime);
  m_ntuple->Branch("e", &e);
  m_ntuple->Branch("edep", &edep);
  m_ntuple->Branch("surfcentx",&surfcentx);
  m_ntuple->Branch("surfcenty",&surfcenty);
  m_ntuple->Branch("surfcentz",&surfcentz);
  return StatusCode::SUCCESS;

}
/*******************************************************************************/ 
StatusCode MM_FastDigitizer::execute() {




  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> h_sdoContainer(m_sdoName);
  auto sdoContainer = std::make_unique<MuonSimDataCollection>(*h_sdoContainer);
  ATH_CHECK( h_sdoContainer.record ( std::move (sdoContainer)) );

  MMPrepDataContainer* prdContainer = new MMPrepDataContainer(m_idHelper->detectorElement_hash_max());
  std::string key = "MM_Measurements";
  ATH_MSG_DEBUG(" Done! Total number of MM chambers with PRDS: " << prdContainer->numberOfCollections() << " key " << key);
  ATH_CHECK( evtStore()->record(prdContainer,key) );

  if( m_maskMultiplet == 3 ) {
    
    return StatusCode::SUCCESS;
  }
  // as the MMPrepDataContainer only allows const accesss, need a local vector as well.
  std::vector<MMPrepDataCollection*> localMMVec(m_idHelper->detectorElement_hash_max());

  const DataHandle< GenericMuonSimHitCollection > collGMSH;
  if ( evtStore()->retrieve( collGMSH,m_inputObjectName ).isFailure()) {
    ATH_MSG_WARNING("No MM hits found in SG");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Retrieved " <<  collGMSH->size() << " MM hits!");

  // Get the MicroMegas Id hit helper
  MM_SimIdToOfflineId simToOffline(*m_idHelper);

  std::map<Identifier,int> hitsPerChannel;
  int nhits = 0;

  GenericMuonSimHitCollection::const_iterator iterMM;

  const GenericMuonSimHit* previousHit = 0;

  for (iterMM=collGMSH->begin();iterMM!=collGMSH->end();++iterMM) {
    const GenericMuonSimHit& hit = *iterMM;

    // SimHits without energy loss are not recorded. 
    // not needed because of already done in sensitive detector
    // https://svnweb.cern.ch/trac/atlasoff/browser/MuonSpectrometer/MuonG4/MuonG4SD/trunk/src/MicromegasSensitiveDetector.cxx?rev=542333#L65
    // if(hit.depositEnergy()==0.) continue;

    if( previousHit && abs(hit.particleEncoding())==13 && abs(previousHit->particleEncoding())==13 ) {
      Amg::Vector3D diff = previousHit->localPosition() - hit.localPrePosition();
      ATH_MSG_VERBOSE("second hit from a muon: prev " <<  previousHit->localPosition() << " current " << hit.localPrePosition() 
		      << " diff " << diff );
      if( diff.mag() < 0.1 ) continue;
    }

    globalHitTime = hit.globalpreTime();
    tofCorrection = hit.globalPosition().mag()/CLHEP::c_light;
    bunchTime = globalHitTime - tofCorrection;
    const float stripPropagationTime = 0.;
    static const float jitter = 0.;
    float MMDigitTime = bunchTime + jitter + stripPropagationTime;

    float timeWindowStrip = 120.; //driftvelocity gap;
    if (MMDigitTime < -timeWindowStrip || MMDigitTime > timeWindowStrip) {
      exitcode = 4;
      m_ntuple->Fill();
      continue;
    }

    // Read the information about the Micro Megas hit
    ATH_MSG_VERBOSE("MM hit: r " << hit.globalPosition().perp() << " z " << hit.globalPosition().z() << " mclink " << hit.particleLink() );

    //  convert simHit id to offline layer id; make sanity checks; retrieve the associated detector element.
    int simId = hit.GenericId();
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
    m_idHelper->get_detectorElement_hash(layid, hash);
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
    if ( std::abs( (roParam.stereoAngel).at(m_idHelper->gasGap(layid)-1) ) > 0. )
      ldirTime = ldir;
    else
      ldirTime = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), -hit.globalDirection().z());

    double scale;//, scaletop;
//    double gasgap = 5.;
    
    scale = -slpos.z()/ldir.z();
//    scaletop = (gasgap+slpos.z())/ldir.z();

    Amg::Vector3D hitOnSurface = slpos + scale*ldir;
//    Amg::Vector3D hitOnTopSurface = slpos + scaletop*ldir;
    Amg::Vector3D hitOnSurfaceGlobal = surf.transform()*hitOnSurface;

    // account for time offset 
    const double vdrift = 0.047;
    double shiftTimeOffset = MMDigitTime*vdrift;
    Amg::Vector3D hitAfterTimeShift(hitOnSurface.x(),hitOnSurface.y(),shiftTimeOffset);
    Amg::Vector3D hitAfterTimeShiftOnSurface = hitAfterTimeShift - (shiftTimeOffset/ldirTime.z())*ldirTime;

    // resolution = -.01/3 * angle + .64/3.
    double resolution;
    if (fabs(inAngle_XZ)<3)
      resolution = .07;
    else
      resolution = ( -.001/3.*fabs(inAngle_XZ) ) + .28/3.;
    double sp = CLHEP::RandGauss::shoot(m_rndmEngine, 0, resolution);

    ATH_MSG_VERBOSE("slpos.z " << slpos.z() << ", ldir " << ldir.z() << ", scale " << scale << ", hitOnSurface.z " << hitOnSurface.z() );
    
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
    }else if( m_useTimeShift ){
      posOnSurf[0]=(hitAfterTimeShiftOnSurface.x()+sp);
//      digiMode = 2;
    }
    
    ////// fill first part of ntuple
    Amg::Vector3D repos = detEl->globalPosition();
    MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();

    std::string stName = m_idHelper->stationNameString(m_idHelper->stationName(layid));
    int isSmall = stName[2] == 'S';
    slx = hit.localPosition().x();
    sly = hit.localPosition().y();
    slz = hit.localPosition().z();
    dlx = lpos.x();
    dly = lpos.y();
    sulx = posOnSurf.x();
    suly = posOnSurf.y();
    tsulx = hitOnSurface.x();
    tsuly = hitOnSurface.y();
    tsulz = hitOnSurface.z();
    stsulx = hitAfterTimeShiftOnSurface.x();
    stsuly = hitAfterTimeShiftOnSurface.y();
    stsulz = hitAfterTimeShiftOnSurface.z();
    ang = inAngle_XZ;
    shift  = shiftTimeOffset;
    resx = hit.localPosition().x() - lpos.x();
    resy = hit.localPosition().y() - lpos.y();
    resz = hit.localPosition().z() - lpos.z();
    suresx = posOnSurf.x()-hitOnSurface.x();
    suresy = posOnSurf.y()-hitOnSurface.y();
    err  = -99999.; 
    res  = -99999.; 
    pull  = -99999.; 
    is   = isSmall;
    seta =  hitHelper->GetZSector(simId);
    sphi = hitHelper->GetPhiSector(simId);
    sml  = hitHelper->GetMultiLayer(simId);
    sl  = hitHelper->GetLayer(simId);
    ss  = hitHelper->GetSide(simId);
    ieta = m_idHelper->stationEta(layid);
    iphi = m_idHelper->stationPhi(layid);
    iml  = m_idHelper->multilayer(layid);
    il  = m_idHelper->gasGap(layid);
    ich = -99999;
    istr  = -99999;
    exitcode = 0;
    mode = 0;
    pdg = hit.particleEncoding();
    trkid = hit.trackNumber();
    gpx  = hit.globalPosition().x();
    gpy  = hit.globalPosition().y();
    gpz  = hit.globalPosition().z();
    gpr  = hit.globalPosition().perp();
    gpp  = hit.globalPosition().phi();
    dgpx  = repos.x();
    dgpy  = repos.y();
    dgpz  = repos.z();
    dgpr  = repos.perp();
    dgpp  = repos.phi();
    edep  = hit.depositEnergy();
    e     = hit.kineticEnergy();
    surfcentx = surf.center().x();
    surfcenty = surf.center().y();
    surfcentz = surf.center().z();

    /////
    if(hit.kineticEnergy()<m_energyThreshold && abs(hit.particleEncoding())==11) {
      exitcode = 5;
      m_ntuple->Fill();      
      continue;
    }

    // perform bound check
    if( !surf.insideBounds(posOnSurf) ){
      exitcode = 1;
      m_ntuple->Fill();
      continue;
    }

    // recalculate strip number after smearing
    int stripNumber = detEl->stripNumber(posOnSurf,layid);
    //int LastStripNumber = detEl->stripNumber(posOnTopSurf, layid);

    if( stripNumber == -1 ){
      ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperTool->toString(layid) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << slpos.z() ); 
      exitcode = 2;
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
      exitcode = 3;
      m_ntuple->Fill();
      continue;
    }

    // Recalculate the Identifier using the strip number
    // here need to use parent ID to avoid creating corrupted identifiers
    id = m_idHelper->channelID(parentID, m_idHelper->multilayer(layid), m_idHelper->gasGap(layid),stripNumber,m_checkIds);

    if( stripNumber != m_idHelper->channel(id) ) {
      ATH_MSG_WARNING(" bad stripNumber: in  "  << stripNumber << " from id " << m_idHelper->channel(id));
      exitcode = 4;
      continue;
    }

    std::vector<Identifier> rdoList;
    rdoList.push_back(id);
    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    cov->setIdentity();
    (*cov)(0,0) = resolution*resolution;
    MMPrepData* prd = new MMPrepData( id,hash,posOnSurf,rdoList,cov,detEl);
    prd->setHashAndIndex(col->identifyHash(), col->size()); // <<< add this line to the MM code as well
    col->push_back(prd);
    ATH_MSG_VERBOSE("Global hit: r " << hit.globalPosition().perp() << " phi " << hit.globalPosition().phi() << " z " << hit.globalPosition().z());
    ATH_MSG_VERBOSE(" Prd: r " << prd->globalPosition().perp() << "  phi " << prd->globalPosition().phi() << " z " << prd->globalPosition().z());
    ATH_MSG_VERBOSE(" Calculated truth hitOnSurfaceGlobal: r " << hitOnSurfaceGlobal.perp() << " phi " << hitOnSurfaceGlobal.phi() << " z " << hitOnSurfaceGlobal.z());
    ATH_MSG_VERBOSE(" detEl: r " << repos.perp() << " phi " << repos.phi() << " z " << repos.z());
    ATH_MSG_VERBOSE(" Surface center: r " << surf.center().perp() << " phi " << surf.center().phi() << " z " << surf.center().z());

    ATH_MSG_VERBOSE("Local hit in Det Element Frame: x " << hit.localPosition().x() << " y " << hit.localPosition().y() << " z " << hit.localPosition().z());
    ATH_MSG_VERBOSE(" Prd: local posOnSurf.x() " << posOnSurf.x() << " posOnSurf.y() " << posOnSurf.y() );
    // 		    << " detEl: x " << lpos.x() << " y " << lpos.y() << " z " << lpos.z() << " strip " << stripNumber);
    ATH_MSG_DEBUG(" hit:  " << m_idHelperTool->toString(id) << " hitx " << posOnSurf.x() << " hitOnSurface.x() " << hitOnSurface.x() << " residual " << posOnSurf.x() - hitOnSurface.x()
		  << " pull " << (posOnSurf.x() - hitOnSurface.x())/resolution );
    err = resolution;
    ich = m_idHelper->channel(id);
    istr  = stripNumber;

//     const MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack( *prd, hit.globalPosition() );
//     if( rot ){
//       res  = rot->localParameters().get(Trk::locX)-hitOnSurface.x();
//       pull = res/rot->localErrorMatrix().error(Trk::locX);
//       delete rot;
//     }

    m_ntuple->Fill();
    // create SDO 
    MuonSimData::Deposit deposit(hit.particleLink(), MuonMCData(hitOnSurface.x(),hitOnSurface.y()));
    //Record the SDO collection in StoreGate
    std::vector<MuonSimData::Deposit> deposits;
    deposits.push_back(deposit);
    h_sdoContainer->insert ( std::make_pair ( id, MuonSimData(deposits,0) ) );
    // OLD CODE ENDS HERE

    previousHit = &hit;
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
