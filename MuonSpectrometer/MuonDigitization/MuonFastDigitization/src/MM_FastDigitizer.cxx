/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_FastDigitizer.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include <cmath>

#include <sstream>
#include <iostream>
#include <fstream>

#include "TTree.h"
#include "TFile.h"

using namespace Muon;
/*******************************************************************************/ 
MM_FastDigitizer::MM_FastDigitizer(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator) {
  
}
/*******************************************************************************/ 
StatusCode MM_FastDigitizer::initialize() {
  // initialize transient event store
  ATH_MSG_DEBUG ( "Retrieved StoreGateSvc." );
  ATH_CHECK(m_muonClusterCreator.retrieve());
  ATH_CHECK(m_sdoName.initialize());
  ATH_CHECK(m_prepDataKey.initialize());
  ATH_CHECK(m_inputObjectName.initialize());
  // initialize transient detector store and MuonGeoModel OR MuonDetDescrManager
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  // check the input object name
 
  // getting our random numbers stream
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  ATH_CHECK(m_rndmSvc.retrieve());

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
  const EventContext& ctx = Gaudi::Hive::currentContext(); 
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muonGeoMgrHandle{m_DetectorManagerKey,ctx};
  if (!muonGeoMgrHandle.isValid()){
      ATH_MSG_FATAL("Failed to retrieve the detector manager from the conditiosn store");
      return StatusCode::FAILURE;
  }
  const MuonGM::MuonDetectorManager* muonGeoMgr = *muonGeoMgrHandle;
  // Create and record the SDO container in StoreGate
  std::unique_ptr<MuonSimDataCollection> h_sdoContainer =  std::make_unique<MuonSimDataCollection>();
  
  CLHEP::HepRandomEngine* rndmEngine = getRandomEngine(m_rndmEngineName, Gaudi::Hive::currentContextEvt());

 
  std::unique_ptr<MMPrepDataContainer> prdContainer = std::make_unique<MMPrepDataContainer>(m_idHelperSvc->mmIdHelper().module_hash_max());
  
  
 
  if( m_maskMultiplet == 3 ) {
    SG::WriteHandle<MMPrepDataContainer> prdWriteHandle{m_prepDataKey,ctx};
    ATH_CHECK(prdWriteHandle.record(std::move(prdContainer)));
  
    SG::WriteHandle<MuonSimDataCollection> writHandle(m_sdoName, ctx);
    ATH_CHECK( writHandle.record (std::move(h_sdoContainer) ) );
    return StatusCode::SUCCESS;
  }
  // as the MMPrepDataContainer only allows const accesss, need a local vector as well.
  std::vector<MMPrepDataCollection*> localMMVec(m_idHelperSvc->mmIdHelper().module_hash_max());

  SG::ReadHandle< MMSimHitCollection > collGMSH{m_inputObjectName, ctx};
  if (!collGMSH.isValid()){
     ATH_MSG_ERROR("Failed to retrieve "<<m_inputObjectName.fullKey());
  }
  ATH_MSG_DEBUG( "Retrieved " <<  collGMSH->size() << " MM hits!");

  // Get the MicroMegas Id hit helper
  MM_SimIdToOfflineId simToOffline(&m_idHelperSvc->mmIdHelper());

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
    if( !m_idHelperSvc->isMM(layid) ){
      ATH_MSG_WARNING("MM id is not a mm id! " << m_idHelperSvc->toString(layid));
    }
    if( !m_idHelperSvc->isMuon(layid) ){
      ATH_MSG_WARNING("MM id is not a muon id! " << m_idHelperSvc->toString(layid));
    }
    if( m_idHelperSvc->isMdt(layid)||m_idHelperSvc->isRpc(layid)||m_idHelperSvc->isTgc(layid)||m_idHelperSvc->isCsc(layid)||m_idHelperSvc->issTgc(layid) ){
      ATH_MSG_WARNING("MM id has wrong technology type!");
    }
    
    // remove hits in masked multiplet
    if( m_maskMultiplet == m_idHelperSvc->stgcIdHelper().multilayer(layid) ) continue; 

    // get readout element
    const MuonGM::MMReadoutElement* detEl = muonGeoMgr->getMMReadoutElement(layid);
    if( !detEl ){
      ATH_MSG_WARNING("Failed to retrieve detector element for: " << m_idHelperSvc->toString(layid) );
      continue;
    }

    // collections stored per readout element
    IdentifierHash hash;
    m_idHelperSvc->mmIdHelper().get_module_hash(layid, hash);
    MuonPrepDataCollection<Muon::MMPrepData>* col  = localMMVec[hash];
    if( !col ){
      col = new MMPrepDataCollection(hash);
      col->setIdentifier(m_idHelperSvc->mmIdHelper().channelID(m_idHelperSvc->mmIdHelper().parentID(layid), m_idHelperSvc->mmIdHelper().multilayer(layid),1,1) );
      if( prdContainer->addCollection(col,hash).isFailure() ){
        ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
        delete col;col=nullptr;
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
    char side = ((int)m_idHelperSvc->mmIdHelper().stationEta(layid)) < 0 ? 'C' : 'A';
    char sector_l = ((std::string)m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(layid))).substr(2,1)=="L" ? 'L' : 'S';
    MMDetectorHelper aHelper;
    MMDetectorDescription* mm = aHelper.Get_MMDetector(sector_l, abs((int)m_idHelperSvc->mmIdHelper().stationEta(layid)), (int)m_idHelperSvc->mmIdHelper().stationPhi(layid), (int)m_idHelperSvc->mmIdHelper().multilayer(layid), side);
    MMReadoutParameters roParam = mm->GetReadoutParameters();

    Amg::Vector3D  ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
    Amg::Vector3D  ldirTime;
    // the stereo angle vector stores the angles in rad. the vector indices are 0,1,2,3 which map to layers 1,2,3,4
    if ( std::abs( (roParam.stereoAngle).at(m_idHelperSvc->mmIdHelper().gasGap(layid)-1) ) > 0. )
      ldirTime = ldir;
    else
      ldirTime = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), -hit.globalDirection().z());

    double scale;//, scaletop;
    
    if (m_microTPC) scale = 0;
    else scale = -slpos.z()/ldir.z();

    Amg::Vector3D hitOnSurface = slpos + scale*ldir;
    Amg::Vector3D hitOnSurfaceGlobal = surf.transform()*hitOnSurface;

    // account for time offset 
    const double vdrift = 0.047;
    double shiftTimeOffset = MMDigitTime*vdrift;
    Amg::Vector3D hitAfterTimeShift(hitOnSurface.x(),hitOnSurface.y(),shiftTimeOffset);
    Amg::Vector3D hitAfterTimeShiftOnSurface = hitAfterTimeShift - (shiftTimeOffset/ldirTime.z())*ldirTime;

    double tdrift = 0;

    // resolution = -.01/3 * angle + .64/3.
    double resolution;
    if (std::fabs(inAngle_XZ)<3)
      resolution = .07;
    else
      resolution = ( -.001/3.*std::fabs(inAngle_XZ) ) + .28/3.;
    double sp = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0, resolution);

    ATH_MSG_VERBOSE("slpos.z " << slpos.z() << ", ldir " << ldir.z() << ", scale " << scale << ", hitOnSurface.z " << hitOnSurface.z() );
    
    if( fabs(hitOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("bad propagation to surface " << hitOnSurface );
    if( fabs(hitAfterTimeShiftOnSurface.z()) > 0.1 ) ATH_MSG_WARNING("bad propagation to surface after time shift " << hitAfterTimeShiftOnSurface );
    // smeared local position
   Amg::Vector2D posOnSurf(hitOnSurface.x()+sp,hitOnSurface.y());

    // for large angles project perpendicular to surface
    if( std::fabs(inAngle_XZ) > 70 ){
      posOnSurf[0]=(slpos.x()+sp);
      // if using timing information use hit position after shift
    }else if( m_useTimeShift && !m_microTPC ){
      posOnSurf[0]=(hitAfterTimeShiftOnSurface.x()+sp);
    }
    
    ////// fill first part of ntuple
    Amg::Vector3D repos = detEl->globalPosition();
    const MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();

    std::string stName = m_idHelperSvc->mmIdHelper().stationNameString(m_idHelperSvc->mmIdHelper().stationName(layid));
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
    m_ieta = m_idHelperSvc->mmIdHelper().stationEta(layid);
    m_iphi = m_idHelperSvc->mmIdHelper().stationPhi(layid);
    m_iml  = m_idHelperSvc->mmIdHelper().multilayer(layid);
    m_il  = m_idHelperSvc->mmIdHelper().gasGap(layid);
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

    // perform bound check (making the call from the detector element to consider edge passivation)
    //if( !surf.insideBounds(posOnSurf) ){
    if( !detEl->insideActiveBounds(layid, posOnSurf) ) {
      m_exitcode = 1;
      m_ntuple->Fill();
      continue;
    }

    // recalculate strip number after smearing
    int stripNumber = detEl->stripNumber(posOnSurf,layid);
    //int LastStripNumber = detEl->stripNumber(posOnTopSurf, layid);

    if( stripNumber == -1 ){
      ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperSvc->toString(layid) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << slpos.z() ); 
      m_exitcode = 2;
      m_ntuple->Fill();
      continue;
    }

    // Recalculate the Identifier using the strip number
    // here need to use parent ID to avoid creating corrupted identifiers
    Identifier parentID = m_idHelperSvc->mmIdHelper().parentID(layid);
    bool is_valid{!m_checkIds};
    Identifier id = m_checkIds ? 
        m_idHelperSvc->mmIdHelper().channelID(parentID, m_idHelperSvc->mmIdHelper().multilayer(layid), m_idHelperSvc->mmIdHelper().gasGap(layid),stripNumber,is_valid) : 
        m_idHelperSvc->mmIdHelper().channelID(parentID, m_idHelperSvc->mmIdHelper().multilayer(layid), m_idHelperSvc->mmIdHelper().gasGap(layid),stripNumber);
    if (!is_valid) continue;
    // only one hit per channel
    int& counts = hitsPerChannel[id];
    ++counts;
    if( counts > 1 ) continue;
    ++nhits;


    if( stripNumber >= detEl->numberOfStrips(layid) ){
      ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperSvc->toString(layid) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << slpos.z() ); 
      m_exitcode = 3;
      m_ntuple->Fill();
      continue;
    }

    // Recalculate the Identifier using the strip number
    // here need to use parent ID to avoid creating corrupted identifiers
    
    id = m_checkIds ? m_idHelperSvc->mmIdHelper().channelID(parentID, m_idHelperSvc->mmIdHelper().multilayer(layid), m_idHelperSvc->mmIdHelper().gasGap(layid),stripNumber,is_valid):
                      m_idHelperSvc->mmIdHelper().channelID(parentID, m_idHelperSvc->mmIdHelper().multilayer(layid), m_idHelperSvc->mmIdHelper().gasGap(layid),stripNumber);

    if (!is_valid) continue;
    if( stripNumber != m_idHelperSvc->mmIdHelper().channel(id) ) {
      ATH_MSG_WARNING(" bad stripNumber: in  "  << stripNumber << " from id " << m_idHelperSvc->mmIdHelper().channel(id));
      m_exitcode = 4;
      continue;
    }

    std::vector<Identifier> rdoList;
    rdoList.push_back(id);

    ATH_MSG_VERBOSE("Global hit: r " << hit.globalPosition().perp() << " phi " << hit.globalPosition().phi() << " z " << hit.globalPosition().z());
    ATH_MSG_VERBOSE(" Calculated truth hitOnSurfaceGlobal: r " << hitOnSurfaceGlobal.perp() << " phi " << hitOnSurfaceGlobal.phi() << " z " << hitOnSurfaceGlobal.z());
    ATH_MSG_VERBOSE(" detEl: r " << repos.perp() << " phi " << repos.phi() << " z " << repos.z());
    ATH_MSG_VERBOSE(" Surface center: r " << surf.center().perp() << " phi " << surf.center().phi() << " z " << surf.center().z());
    ATH_MSG_DEBUG(" hit:  " << m_idHelperSvc->toString(id) << " hitx " << posOnSurf.x() << " hitOnSurface.x() " << hitOnSurface.x() << " residual " << posOnSurf.x() - hitOnSurface.x()
		  << " pull " << (posOnSurf.x() - hitOnSurface.x())/resolution );
    Amg::Vector3D CurrentHitInDriftGap = slpos;
    // emulating micro track in the drift volume for microTPC
    if (!m_microTPC) {
      auto cov = Amg::MatrixX(1,1);
      cov.setIdentity();
      (cov)(0, 0) = resolution * resolution;
      MMPrepData* prd = new MMPrepData(id,
                                       hash,
                                       Amg::Vector2D(posOnSurf.x(), 0.),
                                       rdoList,
                                       cov,
                                       detEl,
                                       (int)tdrift,
                                       0);
      prd->setHashAndIndex(
        col->identifyHash(),
        col->size()); // <<< add this line to the MM code as well
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
        Amg::Vector3D stepInDriftGap = loop_direction * ldir * (roParam.stripPitch/std::cos(roParam.stereoAngle.at(m_idHelperSvc->mmIdHelper().gasGap(layid)-1) ))/abs(ldir.x());
        if (loop_direction == 1) CurrentHitInDriftGap = slpos + stepInDriftGap;
        while (std::abs(CurrentHitInDriftGap.z()) <= roParam.gasThickness) {
          auto cov = Amg::MatrixX(1,1);
          cov.setIdentity();
          (cov)(0,0) = resolution*resolution;

          tdrift = CurrentHitInDriftGap.z() / vdrift + CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0., 5.);
          Amg::Vector2D CurrenPosOnSurf(CurrentHitInDriftGap.x(),CurrentHitInDriftGap.y());

          stripNumber = detEl->stripNumber(CurrenPosOnSurf,layid);
          if( (stripNumber >= detEl->numberOfStrips(layid)) || (stripNumber == -1) ) {
            CurrentHitInDriftGap += stepInDriftGap;
            continue;
          }
          id =  m_checkIds? m_idHelperSvc->mmIdHelper().channelID(parentID, m_idHelperSvc->mmIdHelper().multilayer(layid), m_idHelperSvc->mmIdHelper().gasGap(layid),stripNumber,is_valid):
                            m_idHelperSvc->mmIdHelper().channelID(parentID, m_idHelperSvc->mmIdHelper().multilayer(layid), m_idHelperSvc->mmIdHelper().gasGap(layid),stripNumber);
          if (!is_valid) continue;
          if( stripNumber != m_idHelperSvc->mmIdHelper().channel(id) ) {
            CurrentHitInDriftGap += stepInDriftGap;
            continue;
          }

          MMPrepData* prd =
            new MMPrepData(id,
                           hash,
                           Amg::Vector2D(CurrenPosOnSurf.x(), 0.),
                           rdoList,
                           cov,
                           detEl,
                           (int)tdrift,
                           0);
          prd->setHashAndIndex(
            col->identifyHash(),
            col->size()); // <<< add this line to the MM code as well
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

          CurrentHitInDriftGap += stepInDriftGap;
        }
      }
    }


    m_err = resolution;
    m_ich = m_idHelperSvc->mmIdHelper().channel(id);
    m_istr  = stripNumber;
    m_ntuple->Fill();
  } 

  if (msgLvl(MSG::DEBUG)) {
    std::map<Identifier,int>::const_iterator hit = hitsPerChannel.begin();
    std::map<Identifier,int>::const_iterator hit_end = hitsPerChannel.end();
    ATH_MSG_DEBUG(" number of channels with hit " << hitsPerChannel.size() << " nhits " << nhits);
    for( ;hit!=hit_end;++hit ){
      ATH_MSG_DEBUG(" " << m_idHelperSvc->toString(hit->first) << " ->  " << hit->second);
    }
  }
  SG::WriteHandle<MMPrepDataContainer> prdWriteHandle{m_prepDataKey,ctx};
  ATH_CHECK(prdWriteHandle.record(std::move(prdContainer)));
  
  SG::WriteHandle<MuonSimDataCollection> writHandle(m_sdoName, ctx);
  ATH_CHECK( writHandle.record (std::move(h_sdoContainer) ) );

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
float MM_FastDigitizer::RadsToDegrees(float Radians) const
{
  constexpr float ToDegree = 180. / M_PI;
  float Degrees = Radians * ToDegree;
  return Degrees;
}

CLHEP::HepRandomEngine* MM_FastDigitizer::getRandomEngine(const std::string& streamName, const EventContext& ctx) const
{
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, streamName);
  std::string rngName = name()+streamName;
  rngWrapper->setSeed( rngName, ctx );
  return rngWrapper->getEngine(ctx);
}

