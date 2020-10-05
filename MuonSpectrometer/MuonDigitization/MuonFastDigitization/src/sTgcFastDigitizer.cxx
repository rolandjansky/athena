/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "sTgcFastDigitizer.h"

#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"
#include "CLHEP/Random/RandFlat.h"
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/RNGWrapper.h"

#include "TTree.h"
#include "TFile.h"

using namespace Muon;

sTgcFastDigitizer::sTgcFastDigitizer(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_detManager(nullptr),
    m_channelTypes(0),
    m_file(nullptr),
    m_ntuple(nullptr),
    m_dlx(0.),
    m_dly(0.),
    m_dlz(0.),
    m_sulx(0.),
    m_suly(0.),
    m_tsulx(0.),
    m_tsuly(0.),
    m_tsulz(0.),
    m_suresx(0.),
    m_suresy(0.),
    m_errx(0.),
    m_erry(0.),
    m_res(0.),
    m_pull(0.),
    m_is(0),
    m_seta(0),
    m_sphi(0),
    m_sml(0),
    m_sl(0),
    m_ss(0),
    m_stype(0),
    m_ieta(0),
    m_iphi(0),
    m_iml(0),
    m_il(0),
    m_ich(0),
    m_istr(0),
    m_itype(0),
    m_ipadeta(0),
    m_ipadphi(0),
    m_exitcode(0),
    m_mode(0),
    m_pdg(0),
    m_trkid(0),
    m_bct(0),
    m_tb(0.),
    m_tj(0.),
    m_tg4(0.),
    m_ttof(0.),
    m_gpx(0.),
    m_gpy(0.),
    m_gpz(0.),
    m_gpr(0.),
    m_gpp(0.),
    m_dgpx(0.),
    m_dgpy(0.),
    m_dgpz(0.),
    m_dgpr(0.),
    m_dgpp(0.),
    m_edep(0.),
    m_as(0.),
    m_at(0.),
    m_surfcentx(0.),
    m_surfcenty(0.),
    m_surfcentz(0.),
    m_sdoName("STGCfast_SDO"),
    m_timeWindowOffsetWire(0.),
    m_timeWindowOffsetStrip(0.),
    m_timeWindowWire(24.95), // TGC  29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
    m_timeWindowStrip(24.95), // TGC  40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
    m_bunchCrossingTime(24.95) // 24.95 ns =(40.08 MHz)^(-1)
{
  declareProperty("ChannelTypes", m_channelTypes = 3);
  declareProperty("RndmEngine",  m_rndmEngineName, "Random engine name");
  declareProperty("EnergyThreshold", m_energyThreshold = 50, "Minimal energy of incoming particle to produce a PRD"  );
  declareProperty("EnergyDepositThreshold", m_energyDepositThreshold = 0.00052,  "Minimal energy deposit to produce a PRD"  );
  declareProperty("CheckIds", m_checkIds = false,  "Turn on validity checking of Identifiers"  );
  declareProperty("SDOname", m_sdoName = "sTGCfast_SDO"  );
  declareProperty("MergePrds", m_mergePrds = true );
}

StatusCode sTgcFastDigitizer::initialize() {
  ATH_CHECK(detStore()->retrieve(m_detManager));
  ATH_CHECK(m_rndmSvc.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_muonClusterCreator.retrieve());
  ATH_CHECK(m_sdoName.initialize());

  if( !readFileOfTimeJitter() ) return StatusCode::FAILURE; 

  m_file = new TFile("sTgcplots.root","RECREATE");

  m_ntuple = new TTree("a","a");
  m_ntuple->Branch("dlx",&m_dlx);
  m_ntuple->Branch("dly",&m_dly);
  m_ntuple->Branch("dlz",&m_dlz);
  m_ntuple->Branch("sulx",&m_sulx);
  m_ntuple->Branch("suly",&m_suly);
  m_ntuple->Branch("tsulx",&m_tsulx);
  m_ntuple->Branch("tsuly",&m_tsuly);
  m_ntuple->Branch("tsulz",&m_tsulz);
  m_ntuple->Branch("suresx",&m_suresx);
  m_ntuple->Branch("suresy",&m_suresy);
  m_ntuple->Branch("errx",&m_errx);
  m_ntuple->Branch("erry",&m_erry);
  m_ntuple->Branch("res",&m_res);
  m_ntuple->Branch("pull",&m_pull);
  m_ntuple->Branch("is",&m_is);
  m_ntuple->Branch("seta",&m_seta);
  m_ntuple->Branch("sphi",&m_sphi);
  m_ntuple->Branch("sml",&m_sml);
  m_ntuple->Branch("sl",&m_sl);
  m_ntuple->Branch("ss",&m_ss);
  m_ntuple->Branch("stype",&m_stype);
  m_ntuple->Branch("ieta",&m_ieta);
  m_ntuple->Branch("iphi",&m_iphi);
  m_ntuple->Branch("iml",&m_iml);
  m_ntuple->Branch("il",&m_il);
  m_ntuple->Branch("ich",&m_ich);
  m_ntuple->Branch("istr",&m_istr);
  m_ntuple->Branch("itype",&m_itype);
  m_ntuple->Branch("ipadeta",&m_ipadeta);
  m_ntuple->Branch("ipadphi",&m_ipadphi);
  m_ntuple->Branch("exitcode",&m_exitcode);
  m_ntuple->Branch("mode",&m_mode);
  m_ntuple->Branch("pdg",&m_pdg);
  m_ntuple->Branch("trkid",&m_trkid);
  m_ntuple->Branch("bct",&m_bct);
  m_ntuple->Branch("tb",&m_tb);
  m_ntuple->Branch("tj",&m_tj);
  m_ntuple->Branch("tg4",&m_tg4);
  m_ntuple->Branch("ttof",&m_ttof);
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
  m_ntuple->Branch("edep",&m_edep);
  m_ntuple->Branch("at",&m_at);
  m_ntuple->Branch("as",&m_as);
  m_ntuple->Branch("surfcentx",&m_surfcentx);
  m_ntuple->Branch("surfcenty",&m_surfcenty);
  m_ntuple->Branch("surfcentz",&m_surfcentz);

  return StatusCode::SUCCESS;
}

StatusCode sTgcFastDigitizer::execute() {
  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> h_sdoContainer(m_sdoName);
  ATH_CHECK( h_sdoContainer.record ( std::make_unique<MuonSimDataCollection>() ) );

  CLHEP::HepRandomEngine* rndmEngine = getRandomEngine(m_rndmEngineName, Gaudi::Hive::currentContextEvt());

  sTgcPrepDataContainer* prdContainer = new sTgcPrepDataContainer(m_idHelperSvc->stgcIdHelper().module_hash_max());
  
  // as the sTgcPrepDataContainer only allows const accesss, need a local vector as well.
  std::vector<sTgcPrepDataCollection*> localsTgcVec(m_idHelperSvc->stgcIdHelper().module_hash_max());
  std::vector<sTgcPrepData*> sTgcprds;
  std::vector<int> sTgcflag;

  const DataHandle< sTGCSimHitCollection > collGMSH;
  ATH_CHECK(evtStore()->retrieve( collGMSH,"sTGCSensitiveDetector"));

  ATH_MSG_DEBUG( "Retrieved " <<  collGMSH->size() << " sTgc hits!");
  sTgcHitIdHelper* hitHelper=sTgcHitIdHelper::GetHelper();
  sTgcSimIdToOfflineId simToOffline(&m_idHelperSvc->stgcIdHelper());


  std::map<Identifier,int> hitsPerChannel;
  int nhits = 0;
  IdentifierHash hashLast = 0;

  sTGCSimHitCollection::const_iterator itersTgc;
  for (itersTgc=collGMSH->begin();itersTgc!=collGMSH->end();++itersTgc) {
    std::vector<sTgcPrepData*> sTgc_prdeta;
    const sTGCSimHit& hit = *itersTgc;

    float globalHitTime = hit.globalTime();
    float tofCorrection = hit.globalPosition().mag()/CLHEP::c_light;
  
    // bunch time
    float bunchTime = globalHitTime - tofCorrection;
  
    int simId = hit.sTGCId();
    Identifier layid = simToOffline.convert(simId);
    ATH_MSG_VERBOSE("sTgc hit: r " << hit.globalPosition().perp() << " z " << hit.globalPosition().z() << " mclink " << hit.particleLink() 
		    << " -- " << m_idHelperSvc->toString(layid));
 
    std::string stName = m_idHelperSvc->stgcIdHelper().stationNameString(m_idHelperSvc->stgcIdHelper().stationName(layid));
    int isSmall = stName[2] == 'S';
    const MuonGM::sTgcReadoutElement* detEl = m_detManager->getsTgcReadoutElement(layid);
    if( !detEl ){
      ATH_MSG_WARNING("Failed to retrieve detector element for: " << m_idHelperSvc->toString(layid) );
      continue;
    }
    
    IdentifierHash hash;
    m_idHelperSvc->stgcIdHelper().get_module_hash(layid, hash);

    bool lastHit = false;
    if(itersTgc + 1 ==collGMSH->end()) lastHit = true;

    if(m_mergePrds) {
     if( (hash != hashLast || lastHit)  && sTgcprds.size()>0 ) {
       MuonPrepDataCollection<Muon::sTgcPrepData>* col  = localsTgcVec[hashLast];
      // new collection hash will be made
      // first store the sTgc eta prds
      // merge the prds that fire closeby strips

      for (unsigned int i=0; i<sTgcprds.size(); ++i){
        // skip the merged prds
        if(sTgcflag[i]==1) continue;

        bool merge = false;
        unsigned int jmerge = -1;
        Identifier id_prd = sTgcprds[i]->identify();
        int strip = m_idHelperSvc->stgcIdHelper().channel(id_prd);
        int gasGap  = m_idHelperSvc->stgcIdHelper().gasGap(id_prd);
        int layer   = m_idHelperSvc->stgcIdHelper().multilayer(id_prd);
        ATH_MSG_VERBOSE("  sTgcprds " <<  sTgcprds.size() <<" index "<< i << " strip " << strip << " gasGap " << gasGap << " layer " << layer );
        for (unsigned int j=i+1; j<sTgcprds.size(); ++j){
          Identifier id_prdN = sTgcprds[j]->identify();
          int stripN = m_idHelperSvc->stgcIdHelper().channel(id_prdN);
          int gasGapN  = m_idHelperSvc->stgcIdHelper().gasGap(id_prdN);
          int layerN   = m_idHelperSvc->stgcIdHelper().multilayer(id_prdN);
          if( gasGapN==gasGap && layerN==layer ) {
            ATH_MSG_VERBOSE(" next sTgcprds strip same gasGap and layer index " << j << " strip " << stripN << " gasGap " << gasGapN << " layer " << layerN );
            if(abs(strip-stripN)<2) {
              merge = true;
              jmerge = j;
              break;
            }
          }
        }

        if(!merge) {
          ATH_MSG_VERBOSE(" add isolated sTgcprds strip " << strip << " gasGap " << gasGap << " layer " << layer );
          std::vector<Identifier> rdoList;
          rdoList.push_back(id_prd);
          double covX = sTgcprds[i]->localCovariance()(Trk::locX,Trk::locX);
          Amg::MatrixX* covN = new Amg::MatrixX(1,1);
          covN->setIdentity();
          (*covN)(0,0) = covX;
          sTgcPrepData* prdN = new sTgcPrepData(id_prd, hashLast, sTgcprds[i]->localPosition(), rdoList, covN, sTgcprds[i]->detectorElement(), sTgcprds[i]->getBcBitMap());
          prdN->setHashAndIndex(col->identifyHash(), col->size());
          col->push_back(prdN);
        } else {
          unsigned int nmerge = 0;
          std::vector<Identifier> rdoList;
          std::vector<unsigned int> mergeIndices;
          std::vector<int> mergeStrips;
          rdoList.push_back(id_prd);
          sTgcflag[i] = 1;
          mergeIndices.push_back(i);
          mergeStrips.push_back(strip);
          unsigned int nmergeStrips = 1;
          unsigned int nmergeStripsMax = 25;
          for (unsigned int k=0; k < nmergeStripsMax; ++k) {
            for (unsigned int j=jmerge; j<sTgcprds.size(); ++j){
              Identifier id_prdN = sTgcprds[j]->identify();
              int stripN = m_idHelperSvc->stgcIdHelper().channel(id_prdN);
              if( abs(mergeStrips[k]-stripN) == 1 ) {
                int gasGapN  = m_idHelperSvc->stgcIdHelper().gasGap(id_prdN);
                int layerN   = m_idHelperSvc->stgcIdHelper().multilayer(id_prdN);
                if( gasGapN==gasGap && layerN==layer ) {
                  nmerge++;
                  rdoList.push_back(id_prdN);
                  sTgcflag[j] = 1;
                  mergeIndices.push_back(j);
                  mergeStrips.push_back(stripN);
                  nmergeStrips++;
                }
              }
            }
            if(k>=nmergeStrips) break;
          }
          ATH_MSG_VERBOSE(" add merged sTgcprds nmerge " << nmerge << " strip " << strip << " gasGap " << gasGap << " layer " << layer );

          // start off from strip in the middle
          int stripSum = 0;
          for (unsigned int k =0; k<mergeStrips.size(); ++k) {
            stripSum += mergeStrips[k];
          }
          stripSum = stripSum/mergeStrips.size();

          unsigned int j = jmerge;
          for (unsigned int k =0; k<mergeStrips.size(); ++k) {
            if(mergeStrips[k]==stripSum) j = mergeIndices[k];
            ATH_MSG_VERBOSE(" merged strip nr " << k <<  " strip " << mergeStrips[k] << " index " << mergeIndices[k]);
          }
          ATH_MSG_VERBOSE(" Look for strip nr " << stripSum << " found at index " << j);

          double covX = sTgcprds[j]->localCovariance()(Trk::locX, Trk::locX);
          Amg::MatrixX* covN = new Amg::MatrixX(1,1);
          covN->setIdentity();
          (*covN)(0,0) = 6.*(nmerge + 1.)*covX;
          ATH_MSG_VERBOSE(" make merged prepData at strip " << m_idHelperSvc->stgcIdHelper().channel(sTgcprds[j]->identify()));

          sTgcPrepData* prdN = new sTgcPrepData(sTgcprds[j]->identify(), hashLast, sTgcprds[j]->localPosition(), rdoList, covN, sTgcprds[j]->detectorElement(), sTgcprds[j]->getBcBitMap());
          prdN->setHashAndIndex(col->identifyHash(), col->size());
          col->push_back(prdN);
        }
      } // end loop sTgcprds[i]
      // clear vector and delete elements
      sTgcflag.clear();
      sTgcprds.clear();
     }
    }

    MuonPrepDataCollection<Muon::sTgcPrepData>* col  = localsTgcVec[hash];
    if( !col ){
      col = new sTgcPrepDataCollection(hash);
      col->setIdentifier(m_idHelperSvc->stgcIdHelper().channelID(m_idHelperSvc->stgcIdHelper().parentID(layid), m_idHelperSvc->stgcIdHelper().multilayer(layid),1,1,1) );
      if( prdContainer->addCollection(col,hash).isFailure() ){
	ATH_MSG_WARNING("Failed to add collection with hash " << (int)hash );
	delete col;col=0;
	continue;
      } else {
         ATH_MSG_VERBOSE(" added collection with hash " << (int)hash << " last hash " << (int)hashLast );
      }
      localsTgcVec[hash] = col;
    }

    Identifier parentId = m_idHelperSvc->stgcIdHelper().parentID(layid);

    // select whether to produce only strips or strips + wires or strips + wires + pads
    int ftype = m_channelTypes == 3 ? 0 : 1;
    int ltype = m_channelTypes == 1 ? 1 : 2;
    
    for( int type=ftype;type<=ltype;++type ){

      // only produce wire hits for outer two stations
      if( type == 2 && abs(m_idHelperSvc->stgcIdHelper().stationEta(layid)) < 3 ) continue;

      // first create surface identifier
      Identifier id = m_idHelperSvc->stgcIdHelper().channelID(parentId, m_idHelperSvc->stgcIdHelper().multilayer(layid), m_idHelperSvc->stgcIdHelper().gasGap(layid),type,1,m_checkIds);
      ATH_MSG_VERBOSE("handling layer " << m_idHelperSvc->toString(id) << "   type " << type );
      const Trk::PlaneSurface& surf = detEl->surface(id);
      Amg::Transform3D gToL = detEl->absTransform().inverse();
      Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
      Amg::Vector3D lpos = gToL*hpos;
      // surface local position (that matters)
      Amg::Vector3D slpos = surf.transform().inverse()*hpos;
      
      // propagate sim hit position to surface
      Amg::Vector3D ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z()); //sleontsi
      double scale = -slpos.z()/ldir.z();
      Amg::Vector3D hitOnSurface = slpos + scale*ldir;

      // the position resolution m_is depending on the incident angle
      //double resolution = .18;
      const Amg::Vector3D GloDire(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
      Trk::LocalDirection LocDirection;
      surf.globalToLocalDirection(GloDire, LocDirection);

      float inAngle_time = 90 - std::abs( LocDirection.angleXZ() / CLHEP::degree);
      float inAngle_space = 90 - std::abs( LocDirection.angleYZ() / CLHEP::degree);
      if(inAngle_time > 90)  inAngle_time  = inAngle_time  -90.;
      if(inAngle_space > 90) inAngle_space = inAngle_space -90.;

      // bctagging
      float jitter = 0;//jitterInitial; // calculated m_at central strip but also used in all the strips fired by the same hit 

      //const float stripPropagationTime = 3.3*CLHEP::ns/CLHEP::m * detEl->distanceToReadout(posOnSurf_strip, elemId); // 8.5*ns/m was used until MC10. 
      const float stripPropagationTime = 0.; // 8.5*ns/m was used until MC10. 
    
      // shift by 0.5 to get all current bc in time
      float sDigitTime = bunchTime + jitter + stripPropagationTime+0.5;
    
      uint16_t bctag = bcTagging(sDigitTime, type);

      // smear
      // for the strips apply gaussian smearing, for the wire gangs or pads do nothing here
      double sp = hitOnSurface.x();
      double resolution = 0;
      if( type == 1 ){
	resolution = getResolution(inAngle_time);
	sp = CLHEP::RandGauss::shoot(rndmEngine, hitOnSurface.x(), resolution);
      }

      
      Amg::Vector2D  posOnSurf(sp,hitOnSurface.y());
      Amg::Vector2D  locHitPosOnSurf(hitOnSurface.x(),hitOnSurface.y());

      /// now fill most of the ntuple
      Amg::Vector3D repos = detEl->globalPosition();

      // Local position wrt Det element (NOT to surface)
      m_dlx = lpos.x();
      m_dly = lpos.y();
      m_dlz = lpos.z();
      m_sulx = posOnSurf.x();
      m_suly = posOnSurf.y();
      m_tsulx = hitOnSurface.x();
      m_tsuly = hitOnSurface.y();
      m_tsulz = hitOnSurface.z();
      m_suresx = posOnSurf.x()-hitOnSurface.x();
      m_suresy = posOnSurf.y()-hitOnSurface.y();
      m_errx = -99999.;
      m_erry = -99999.;
      m_res  = -99999.; 
      m_pull  = -99999.; 
      m_is   = isSmall;
      m_seta = hitHelper->GetZSector(simId);
      m_sphi = hitHelper->GetPhiSector(simId);
      m_sml  = hitHelper->GetMultiLayer(simId);
      m_sl  = hitHelper->GetLayer(simId);
      m_ss  = hitHelper->GetSide(simId);
      m_stype = type;
      m_ieta = m_idHelperSvc->stgcIdHelper().stationEta(id);
      m_iphi = m_idHelperSvc->stgcIdHelper().stationPhi(id);
      m_iml  = m_idHelperSvc->stgcIdHelper().multilayer(id);
      m_il  = m_idHelperSvc->stgcIdHelper().gasGap(id);
      m_ich = -99999;
      m_istr  = -99999;
      m_itype = -99999;
      m_ipadeta = -99999;
      m_ipadphi = -99999;
      m_exitcode = 0;
      m_mode = 0;
      m_pdg = hit.particleEncoding();
      m_trkid = hit.trackNumber();
      m_bct = bctag;
      m_tb = bunchTime;
      m_tj = jitter;
      m_tg4 = globalHitTime;
      m_ttof = tofCorrection;
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
      m_at    = inAngle_time;
      m_as    = inAngle_space;
      m_surfcentx = surf.center().x();
      m_surfcenty = surf.center().y();
      m_surfcentz = surf.center().z();

      // cut on depositEnergy(0.52KeV) to simulation the detector efficiency(95% for strips)
      if( type ==1 && hit.depositEnergy()<m_energyDepositThreshold)  {
         ATH_MSG_VERBOSE("Drop SimHit with depositEnergy = " << m_edep << " in the strip response!");
	 m_exitcode = 6;
	 m_ntuple->Fill();      
         continue;
      } 

      /// done filling
      if ( !surf.insideBounds(posOnSurf) ) {
	m_exitcode = 1;
	m_ntuple->Fill();
	continue;
      }

      if(sDigitTime < -m_bunchCrossingTime+m_timeWindowOffsetStrip || +m_bunchCrossingTime+m_timeWindowOffsetStrip+ m_timeWindowStrip < sDigitTime) {
	m_exitcode = 4;
	m_ntuple->Fill();
        ATH_MSG_DEBUG( "Strip: Digitized time is outside of time window. " << sDigitTime
		       << " bunchTime: " << bunchTime << " time jitter: " << jitter
		       << " propagation time: " << stripPropagationTime );
        continue;
      }

      bctag = bcTagging(sDigitTime, type);

      int stripNumber = detEl->stripNumber(locHitPosOnSurf,id);
      if( stripNumber == -1 ){
	ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperSvc->toString(id) 
			<< " pos " << posOnSurf.x() << " - " << hitOnSurface.x() << " z " << slpos.z() << " type " << type  ); 
	m_exitcode = 2;
	m_ntuple->Fill();
	continue;
      }

      // create channel identifier
      id = m_idHelperSvc->stgcIdHelper().channelID(parentId, m_idHelperSvc->stgcIdHelper().multilayer(id), m_idHelperSvc->stgcIdHelper().gasGap(id),type,stripNumber,m_checkIds);
      ATH_MSG_VERBOSE(" Unsmeared hit id " << m_idHelperSvc->toString(id) );

      int& counts = hitsPerChannel[id];
      ++counts;
      if( counts > 1 ) continue;
      ++nhits;


      // recalculate using smeared position
      stripNumber = detEl->stripNumber(posOnSurf,id);
      if( stripNumber == -1 ){
	ATH_MSG_WARNING("Failed to obtain strip number " << m_idHelperSvc->toString(id) 
			<< " pos " << posOnSurf.x() << " - " << hitOnSurface.x() << " z " << slpos.z() << " type " << type  ); 
	m_exitcode = 2;
	m_ntuple->Fill();
	continue;
      }

      // create channel identifier
      id = m_idHelperSvc->stgcIdHelper().channelID(parentId, m_idHelperSvc->stgcIdHelper().multilayer(id), m_idHelperSvc->stgcIdHelper().gasGap(id),type,stripNumber,m_checkIds);

      if( type != m_idHelperSvc->stgcIdHelper().channelType(id) ) ATH_MSG_WARNING(" bad type: in  "  << type << " from id " << m_idHelperSvc->stgcIdHelper().channelType(id)
								<< " strip " << stripNumber << " from id " << m_idHelperSvc->stgcIdHelper().channel(id)
								<< " eta " <<  m_idHelperSvc->stgcIdHelper().stationEta(id)
								<< " local pos " << slpos );

      // assign strip position to PRD for wires and pads
      if( type != 1 ){
	Amg::Vector2D locpos(0,0);
	if( !detEl->stripPosition(id,locpos ) ){
	  ATH_MSG_WARNING("Failed to obtain local position for identifier " << m_idHelperSvc->toString(id) );
	  m_exitcode = 3;
	  m_ntuple->Fill();
	  continue; 
	}
	posOnSurf = locpos;
      }

      std::vector<Identifier> rdoList;
      rdoList.push_back(id);

      double errX = 500;

      if( type == 2 ){
	const MuonGM::MuonChannelDesign* design = detEl->getDesign(id);
	if( !design ){
	  ATH_MSG_WARNING("Failed to get design for " << m_idHelperSvc->toString(id) );
	}else{
	  errX = std::abs(design->inputPitch)/std::sqrt(12);
	}
      }else if( type == 1 ){
        errX = resolution;
      }
      if( type == 0 ){

	const MuonGM::MuonPadDesign* design = detEl->getPadDesign(id);
	if( !design ){
	  ATH_MSG_WARNING("Failed to get design for " << m_idHelperSvc->toString(id) );
	}else{
	  errX = design->channelWidth(posOnSurf,true)/sqrt(12);

	  // Peter Kluit: inputPhiPitch m_is in degrees
	  Amg::Vector3D lposPad(posOnSurf.x(),posOnSurf.y(),0.);
	  double radius = (surf.transform()*lposPad).perp();
	  errX = design->inputPhiPitch*M_PI/180.*radius/sqrt(12);
	}
      }
      
      Amg::MatrixX* cov = new Amg::MatrixX(1,1);
      cov->setIdentity();
      (*cov)(0,0) = errX*errX;      

      sTgcPrepData* prd = new sTgcPrepData( id,hash,posOnSurf,rdoList,cov,detEl, bctag);

      if(type!=1 || lastHit || !m_mergePrds) {
        // always store last hit
        prd->setHashAndIndex(col->identifyHash(), col->size());
        col->push_back(prd);
      } else {
        // collect the eta prds in a different vector
        sTgcprds.push_back(prd);
        sTgcflag.push_back(0);
      }
      hashLast = hash;

      /// fill final bits of the ntuple
      m_suresx = posOnSurf.x()-hitOnSurface.x();
      m_suresy = posOnSurf.y()-hitOnSurface.y();
      m_sulx = posOnSurf.x();
      m_suly = posOnSurf.y();
      m_errx = errX;
      m_ich = m_idHelperSvc->stgcIdHelper().channel(id);
      m_istr  = stripNumber;
      m_itype = m_idHelperSvc->stgcIdHelper().channelType(id);
      if( type == 0 ){
        m_ipadeta = m_idHelperSvc->stgcIdHelper().padEta(id);
        m_ipadphi = m_idHelperSvc->stgcIdHelper().padPhi(id);
      }

      ATH_MSG_VERBOSE("Global hit: r " << hit.globalPosition().perp() << " phi " << hit.globalPosition().phi() << " z " << hit.globalPosition().z());
      ATH_MSG_VERBOSE(" Prd: r " << prd->globalPosition().perp() << "  phi " << prd->globalPosition().phi() << " z " << prd->globalPosition().z());

      ATH_MSG_VERBOSE(" detEl: r " << repos.perp() << " phi " << repos.phi() << " z " << repos.z());
      ATH_MSG_VERBOSE(" Surface center: r " << surf.center().perp() << " phi " << surf.center().phi() << " z " << surf.center().z());

      ATH_MSG_VERBOSE(" Prd: local posOnSurf.x() " << posOnSurf.x() << " posOnSurf.y() " << posOnSurf.y() );

      ATH_MSG_DEBUG(" hit:  " << m_idHelperSvc->toString(id) << " hitx " << posOnSurf.x() << " residual " << posOnSurf.x() - hitOnSurface.x() << " hitOnSurface.x() " << hitOnSurface.x() << " errorx " << m_errx << " pull " << (posOnSurf.x() - hitOnSurface.x())/m_errx);

      m_ntuple->Fill();
      // create SDO 
      MuonSimData::Deposit deposit(hit.particleLink(), MuonMCData(hit.depositEnergy(),m_ttof));
      //Record the SDO collection in StoreGate
      std::vector<MuonSimData::Deposit> deposits;
      deposits.push_back(deposit);
      MuonSimData simData(deposits, hit.particleEncoding());
      simData.setPosition(hit.globalPosition());
      simData.setTime(globalHitTime);
      h_sdoContainer->insert ( std::make_pair ( id, simData ) );


    } // end of loop channelType
  } // end of loop SimHits
  if( msgLvl(MSG::DEBUG) ){
    std::map<Identifier,int>::const_iterator hit = hitsPerChannel.begin();
    std::map<Identifier,int>::const_iterator hit_end = hitsPerChannel.end();
    ATH_MSG_DEBUG(" number of channels with hit " << hitsPerChannel.size() << " nhits " << nhits);
    for( ;hit!=hit_end;++hit ){
      ATH_MSG_DEBUG(" " << m_idHelperSvc->toString(hit->first) << " ->  " << hit->second);
    }
  }
  std::string key = "STGC_Measurements";
  ATH_MSG_DEBUG(" Done! Total number of sTgc chambers with PRDS: " << prdContainer->numberOfCollections() << " key " << key);
  if (evtStore()->record(prdContainer,key).isFailure())  {
    ATH_MSG_ERROR ( "Unable to record sTgcPrepData container in StoreGate" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode sTgcFastDigitizer::finalize() {
  m_ntuple->Write();
  m_file->Write();
  m_file->Close();
  return StatusCode::SUCCESS;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcFastDigitizer::getResolution(float inAngle_space) const
{
  // 0 degree --> 0.18mm; 10 degree --> 
  const int NIndex = 2; 
  double Reso[NIndex] = {0.06, 0.30};
  double Angle[NIndex] = {0., 60.};
 
  int index = 1;
  inAngle_space = std::abs(inAngle_space);
  while(index<NIndex && inAngle_space>Angle[index]){
    index++;
  }

  double Resolution = 0.;

  if(index==NIndex) Resolution = Reso[NIndex-1];
  else Resolution = Reso[index-1] + (inAngle_space - Angle[index-1]) / (Angle[index]-Angle[index-1]) * (Reso[index] - Reso[index-1]);

  ATH_MSG_VERBOSE("sTgcFastDigitizer::getResolution(" << inAngle_space << ") = " <<Resolution );

  return Resolution;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
uint16_t sTgcFastDigitizer::bcTagging(float digitTime, int channelType) const {

  double offset, window;
  if(channelType == 1) { // strip 
    offset = m_timeWindowOffsetStrip;
    window = m_timeWindowStrip;
  }
  else { // wire gangs or pad 
    offset = m_timeWindowOffsetWire;
    window = m_timeWindowWire;
  }

  uint16_t bctag = 0;
  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
    bctag = (bctag | 0x1);
  }
  if(                     offset < digitTime && digitTime <                      offset+window) {
    bctag = (bctag | 0x2);
  }
  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
    bctag = (bctag | 0x4);
  }
  return bctag;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcFastDigitizer::readFileOfTimeJitter()
{

  const char* const fileName = "sTGC_Digitization_timejitter.dat";
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");

  std::ifstream ifs;
  if (fileWithPath != "") {
    ifs.open(fileWithPath.c_str(), std::ios::in);
  }
  else {
    ATH_MSG_ERROR("readFileOfTimeJitter(): Could not find file " << fileName);
    return false;
  }

  if(ifs.bad()){
    ATH_MSG_ERROR("readFileOfTimeJitter(): Could not find file " << fileName);
    return false;
  }

  int angle = 0;
  int bins = 0;
  int i = 0;
  float prob = 0.;

  while(ifs.good()){
    ifs >> angle >> bins;
    if (ifs.eof()) break;
    if( msgLvl(MSG::VERBOSE) ) ATH_MSG_VERBOSE("readFileOfTimeJitter(): Timejitter, angle, Number of bins, prob. dist.: " << angle << " " << bins << " ");
    m_vecAngle_Time.resize(i + 1);
    for (int j = 0; j < 41/*bins*/; j++) {
      ifs >> prob;
      m_vecAngle_Time[i].push_back(prob);
      if( msgLvl(MSG::VERBOSE) ){
	if( j == 0) ATH_MSG_VERBOSE("readFileOfTimeJitter(): ");
	ATH_MSG_VERBOSE(prob << " ");
      }
    }
    i++;
  }
  ifs.close();
  return true;
}

CLHEP::HepRandomEngine* sTgcFastDigitizer::getRandomEngine(const std::string& streamName, const EventContext& ctx) const
{
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, streamName);
  std::string rngName = name()+streamName;
  rngWrapper->setSeed( rngName, ctx );
  return rngWrapper->getEngine(ctx);
}

