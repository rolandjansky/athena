/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
 
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonSegment/MuonSegment.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"
//#include "TrkParameters/Perigee.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/AtaPlane.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

#include "MuonDQAUtils/HistDQADataHelper.h"
#include "MuonDQAUtils/MuonDQATrackHistTool.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"

#include "TROOT.h"
#include "TH1.h"
#include "TMath.h"

namespace Muon {

  MuonDQATrackHistTool::MuonDQATrackHistTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_hitHistTool("Muon::MuonDQAHitHistTool/MuonDQAHitHistTool"),
      m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
      m_propagator("Trk::StraightLinePropagator/MuonStraightLinePropagator"),
      m_log(0)
  {
    declareInterface<MuonDQATrackHistTool>(this);
  }

  MuonDQATrackHistTool::~MuonDQATrackHistTool(){}

  StatusCode MuonDQATrackHistTool::initialize()
  {
    
    m_log = new MsgStream(msgSvc(),name());

    StatusCode sc = AlgTool::initialize(); 
    if ( sc.isFailure() ) {
      *m_log<<MSG::FATAL<<"Could not initialize AlgTool " <<endreq; 
      return sc;
    }

    sc = m_idHelperTool.retrieve();
    if (sc.isFailure()){
      *m_log<<MSG::FATAL<<"Could not get " << m_idHelperTool <<endreq; 
      return sc;
    }
    *m_log<<MSG::INFO << "Retrieved " << m_idHelperTool << endreq;

    sc = m_helperTool.retrieve();
    if (sc.isFailure()){
      *m_log<<MSG::FATAL<<"Could not get " << m_helperTool <<endreq; 
      return sc;
    }
    *m_log<<MSG::INFO << "Retrieved " << m_helperTool << endreq;

    sc = m_hitHistTool.retrieve();
    if (sc.isFailure()){
      *m_log<<MSG::FATAL<<"Could not get " << m_hitHistTool <<endreq; 
      return sc;
    }
    *m_log<<MSG::INFO << "Retrieved " << m_hitHistTool << endreq;

    sc = m_pullCalculator.retrieve();
    if (sc.isFailure()){
      *m_log<<MSG::FATAL<<"Could not get " << m_pullCalculator <<endreq; 
      return sc;
    }
    *m_log<<MSG::INFO << "Retrieved " << m_pullCalculator << endreq;

    sc = m_propagator.retrieve();
    if( sc.isFailure() ){
      *m_log<<MSG::FATAL<<"Could not get " << m_propagator <<endreq; 
      return sc;
    }
    *m_log<<MSG::INFO << "Retrieved " << m_propagator << endreq;

    // Retrieve the MuonDetectorManager  
    StoreGateSvc* detStore = 0;
    sc = service("DetectorStore", detStore);
    sc = detStore->retrieve(m_muonMgr);
    if( sc.isFailure() ){
      *m_log<<MSG::FATAL<<"Could not get " << m_muonMgr <<endreq; 
      return sc;
    }
    *m_log<<MSG::INFO << "Retrieved " << m_muonMgr << endreq;
 
    return sc;
  } 
  
  StatusCode MuonDQATrackHistTool::finalize()
  {
    delete m_log;
    StatusCode sc = AlgTool::finalize(); 
    if( sc.isFailure() ) return sc;
    return sc;
  }


  MuonDQATrackHistTool::TrackHistos::TrackHistos() :
    wasBooked(false),wasRegistered(false),chi2(0),ndof(0),prob(0),r(0),z(0),phi(0),eta(0),dirphi(0),direta(0),momentum(0),pt(0),layers(0),mlayers(0),hitHistos()
  {}
 

  MuonDQATrackHistTool::TrackData::TrackData() : chi2(-999.),ndof(-1),charge(-999),layers(0),mlayers(0)
  {}
  
 
  bool MuonDQATrackHistTool::book( MuonDQATrackHistTool::TrackHistos& histos, std::string prefix, bool doMDT, bool doRPC, bool doTGC, bool doCSC ) const {
    if( histos.wasBooked ) return false;
    
    //    *m_log<<MSG::INFO << "In bookHist " << endreq;
    MuonDQAHist::HistDQADataHelper* helper = MuonDQAHist::HistDQADataHelper::getHelper();
    histos.chi2 = helper->createHist("chi2",prefix);
    histos.ndof = helper->createHist("ndof",prefix);
    histos.prob = helper->createHist("prob",prefix);
    histos.r = helper->createHist("r_muon",prefix);
    histos.z = helper->createHist("z_muon",prefix);
    histos.phi = helper->createHist("phi",prefix);
    histos.eta = helper->createHist("eta",prefix);
    histos.dirphi = helper->createHist("dirphi",prefix);
    histos.direta = helper->createHist("direta",prefix);
    histos.momentum = helper->createHist("momentum",prefix);
    histos.pt = helper->createHist("pt",prefix); 
    if (doMDT) {
      histos.layers = helper->createHist("layers",prefix); 
      histos.mlayers = helper->createHist("mlayers",prefix); 
    }

    m_hitHistTool->book(histos.hitHistos,prefix,doMDT,doRPC,doTGC,doCSC);

    histos.wasBooked = true;
    return true;
  }
  
  
  bool MuonDQATrackHistTool::registerHist( ManagedMonitorToolBase::MonGroup& groupShift, ManagedMonitorToolBase::MonGroup& groupDebug, ManagedMonitorToolBase::MonGroup& groupBarrelA, ManagedMonitorToolBase::MonGroup& groupBarrelC, ManagedMonitorToolBase::MonGroup& groupEC, ManagedMonitorToolBase::MonGroup& groupECA, ManagedMonitorToolBase::MonGroup& groupECC, TrackHistos& histos ) const
  {
    *m_log << MSG::DEBUG << "In TrackHistTool registerHist" << endreq; 
    if ( histos.wasRegistered ) return false;
    StatusCode sc = StatusCode::SUCCESS;
    
    if ( histos.chi2 ){ 
      sc=groupDebug.regHist(histos.chi2);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register chi2 Histograms:" << endreq; 
	return sc;}
    }
    if ( histos.ndof ){ 
      sc=groupDebug.regHist(histos.ndof);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register ndof Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.prob ){ 
      sc=groupDebug.regHist(histos.prob);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register prob Histograms:" << endreq; 
	return sc;} 
    }

    if ( histos.r ){ 
      sc=groupDebug.regHist(histos.r);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register r Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.z ){ 
      sc=groupDebug.regHist(histos.z);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register z Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.phi ){ 
      sc=groupDebug.regHist(histos.phi);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register phi Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.eta ){ 
      sc=groupDebug.regHist(histos.eta);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register eta Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.dirphi ) { 
      sc=groupDebug.regHist(histos.dirphi);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register dirphi Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.direta ) { 
      sc=groupDebug.regHist(histos.direta);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register direta Histograms:" << endreq; 
	return sc;} 
    }

    if ( histos.momentum ){ 
      sc=groupDebug.regHist(histos.momentum);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register momentum Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.pt ){ 
      sc=groupDebug.regHist(histos.pt);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register pt Histograms:" << endreq; 
	return sc;} 
    }
    
    if ( histos.layers ){ 
      sc=groupDebug.regHist(histos.layers);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register layer Histograms:" << endreq; 
	return sc;} 
    }
    if ( histos.mlayers ){ 
      sc=groupDebug.regHist(histos.mlayers);
      if (sc.isFailure() ) {
	*m_log << MSG::FATAL << "Cannot register multilayer Histograms:" << endreq; 
	return sc;} 
    }

    m_hitHistTool->registerHist(groupShift,groupDebug,groupBarrelA,groupBarrelC,groupEC,groupECA,groupECC,histos.hitHistos);
    histos.wasRegistered = true;
    return sc;
  }
  
  
  bool MuonDQATrackHistTool::fill( TrackHistos& histos, const TrackData& trackData ) const {
    if( !histos.wasRegistered ) return false;

    if( histos.chi2 )        histos.chi2->Fill( trackData.ndof ? trackData.chi2/trackData.ndof : -1.*trackData.chi2 );
    if( histos.ndof )        histos.ndof->Fill(trackData.ndof);
    int ndof = (int)trackData.ndof;
    if( histos.prob )        histos.prob->Fill(TMath::Prob(trackData.chi2,ndof));
    
    if( histos.r )        histos.r->Fill(trackData.gpos.perp());
    if( histos.z )        histos.z->Fill(trackData.gpos.z());
    if( histos.phi )      histos.phi->Fill(trackData.gpos.phi());
    if( histos.eta )      histos.eta->Fill(trackData.gpos.eta());
    if( histos.dirphi )   histos.dirphi->Fill(trackData.gdir.phi());
    if( histos.direta )   histos.direta->Fill(trackData.gdir.eta()); 

    double momentum = trackData.gdir.mag()/1000.;
    double pt       =trackData.gdir.perp()/1000.;
    if( histos.momentum ) histos.momentum->Fill(momentum);
    if( histos.pt )       histos.pt->Fill(pt);
    
    if( histos.layers)    histos.layers->Fill((float)trackData.layers);
    if( histos.mlayers)   histos.mlayers->Fill((float)trackData.mlayers);

    m_hitHistTool->fill(histos.hitHistos,trackData.hitData);

    return true;
  }


  bool MuonDQATrackHistTool::fill( TrackHistos& histos, const MuonSegment& segment ) const {
    if( !histos.wasRegistered ) return false;

    const Trk::FitQuality* fq = segment.fitQuality();
    double chi2 = 999.;
    int ndof = -1;
    if( fq ) 
      {
	chi2 = fq->chiSquared();
	ndof = fq->numberDoF();
	if( histos.chi2 )        histos.chi2->Fill( ndof ? chi2/ndof : -1.*chi2 );
	if( histos.ndof )        histos.ndof->Fill(ndof);
	if( histos.prob )        histos.prob->Fill(TMath::Prob(chi2,ndof));
      }
    
    if( histos.r )        histos.r->Fill(segment.globalPosition().perp());
    if( histos.z )        histos.z->Fill(segment.globalPosition().z());
    if( histos.phi )      histos.phi->Fill(segment.globalPosition().phi());
    if( histos.eta )      histos.eta->Fill(segment.globalPosition().eta());
    if( histos.dirphi )   histos.dirphi->Fill(segment.globalDirection().phi());
    if( histos.direta )   histos.direta->Fill(segment.globalDirection().eta()); 
    
    if(histos.layers || histos.mlayers) {    
      const MdtIdHelper *m_mdtIdHelper = m_muonMgr->mdtIdHelper();
      std::set<int> multiLayers;
      std::set<int> tubeLayers;
      const std::vector<const Trk::MeasurementBase*> measurements = segment.containedMeasurements();
      for(unsigned int i =0; i < measurements.size(); ++i) {
	const Trk::MeasurementBase* meas = measurements[i];
	Identifier measId = m_helperTool->getIdentifier(*meas);      
	if (m_mdtIdHelper->is_mdt(measId)) {
	  multiLayers.insert(m_mdtIdHelper->multilayer(measId));
	  tubeLayers.insert( (m_mdtIdHelper->multilayer(measId)-1)*4 + m_mdtIdHelper->tubeLayer(measId) );
	}
      }
      if( histos.layers )   histos.layers->Fill((float)tubeLayers.size());
      if( histos.mlayers )  histos.mlayers->Fill((float)multiLayers.size());
    }
    
    fill(histos.hitHistos,segment,ndof ? chi2/ndof : -1.*chi2);
    
    return true;
  }


  bool MuonDQATrackHistTool::fill( MuonDQAHitHistTool::HitHistos& histos, const MuonSegment& segment, const double chi2 ) const {

    if( !histos.wasRegistered ) return false;

    double momentum(10000.);
    double charge(0.);
    const Trk::AtaPlane* segPars = m_helperTool->createTrackParameters( segment, momentum, charge );    
    if( !segPars ) return false;

    HitPairVec hits;
    hits.reserve(segment.containedMeasurements().size());

    Trk::MagneticFieldProperties magProps;
    
    // loop over Contained measurements
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = segment.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = segment.containedMeasurements().end();
    for( ; mit!=mit_end ; ++mit ){
      
      // check whether state is a measurement
      const Trk::MeasurementBase* meas = *mit;
      if( !meas ) continue;

      const Trk::TrackParameters* pars = m_propagator->propagate(*segPars,meas->associatedSurface(),Trk::anyDirection,false,magProps);
      if( !pars ) continue;

      hits.push_back( std::make_pair(meas,pars) );

    }
    
    delete segPars;

    // extract data 
    MuonDQAHitHistTool::HitData hitData;
    extract(hitData,hits,chi2);

    HitPairVec::iterator it = hits.begin();
    HitPairVec::iterator it_end = hits.end();
    for( ;it!=it_end;++it ) delete it->second;
    m_hitHistTool->fill( histos, hitData );

    return true;
  }


  bool MuonDQATrackHistTool::fill( TrackHistos& histos, const Trk::Track& track ) const {
    if( !histos.wasRegistered ) return false;

    const Trk::FitQuality* fq = track.fitQuality();
    double chi2 = 999.;
    int ndof = -1;
    if( fq ) {
      chi2 = fq->chiSquared();
      ndof = fq->numberDoF();

      if( histos.chi2 )        histos.chi2->Fill( ndof ? chi2/ndof : -1.*chi2 );
      if( histos.ndof )        histos.ndof->Fill(ndof);
      if( histos.prob )        histos.prob->Fill(TMath::Prob(chi2,ndof));
    }
    
    const Trk::Perigee* pp = track.perigeeParameters();
    if( pp ){
      if( histos.r )        histos.r->Fill(pp->position().perp());
      if( histos.z )        histos.z->Fill(pp->position().z());
      if( histos.phi )      histos.phi->Fill(pp->position().phi());
      if( histos.eta )      histos.eta->Fill(pp->position().eta());
      if( histos.dirphi )   histos.dirphi->Fill(pp->momentum().phi());
      if( histos.direta )   histos.direta->Fill(pp->momentum().eta()); 
      if( histos.momentum ) histos.momentum->Fill(pp->momentum().mag()/1000.);
      if( histos.pt )       histos.pt->Fill(pp->momentum().perp()/1000.);
    }

    if(histos.layers || histos.mlayers) {    
      const MdtIdHelper *m_mdtIdHelper = m_muonMgr->mdtIdHelper();
      std::set<int> multiLayers;
      std::set<int> tubeLayers;
      /* for(unsigned int i =0; i < track.numberOfMeasurementBases(); ++i) {
	 const Trk::MeasurementBase* meas = track.containedMeasurements(i);
	 Identifier measId = m_helperTool->getIdentifier(*meas);      
	 if (m_mdtIdHelper->is_mdt(measId)) {
	 multiLayers.insert(m_mdtIdHelper->multilayer(measId));
	 tubeLayers.insert( (m_mdtIdHelper->multilayer(measId)-1)*4 + m_mdtIdHelper->tubeLayer(measId) );
	 if( histos.layers )   histos.layers->Fill((float)tubeLayers.size());
	 if( histos.mlayers )  histos.mlayers->Fill((float)multiLayers.size());
	 }
	 }
	 } */
      const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
      for( ; tsit!=tsit_end ; ++tsit) {
     	const Trk::MeasurementBase* rio = (*tsit)->measurementOnTrack();
     	if (!rio) continue;
     	Identifier id = m_helperTool->getIdentifier(*rio);
     	multiLayers.insert(m_mdtIdHelper->multilayer(id));
     	tubeLayers.insert( (m_mdtIdHelper->multilayer(id)-1)*4 + m_mdtIdHelper->tubeLayer(id) );
      }
      if( histos.layers )   histos.layers->Fill((float)tubeLayers.size());
      if( histos.mlayers )  histos.mlayers->Fill((float)multiLayers.size());
    }
    
    fill(histos.hitHistos,track, ndof ? chi2/ndof : -1.*chi2);
    
    return true;
  }


  void MuonDQATrackHistTool::extract( MuonDQAHitHistTool::HitData& hitData, const HitPairVec& hits, const double chi2 ) const {

    std::set<Identifier> mdtChId;
    std::set<Identifier> cscEtaChId;
    std::set<Identifier> cscPhiChId;
    std::set<Identifier> rpcEtaChId;
    std::set<Identifier> rpcPhiChId;
    std::set<Identifier> tgcEtaChId;
    std::set<Identifier> tgcPhiChId;
    
    // loop over TSOSs
    HitPairCit hit = hits.begin();
    HitPairCit hit_end = hits.end();
    for( ; hit!=hit_end ; ++hit ){

      const Trk::MeasurementBase* meas = hit->first;
      const Trk::TrackParameters* pars = hit->second;
      
      // check whether we have no zero pointers
      if( !meas || !pars ) continue;
      // check whether state is a measurement

      Identifier id = m_helperTool->getIdentifier(*meas);
      if( id == Identifier() ) continue;

      Identifier chid = m_idHelperTool->chamberId(id);
      
      bool measuresPhi = m_idHelperTool->measuresPhi(id);
      MuonDQAHitHistTool::DetData* detData = 0;
      MuonDQAHitHistTool::DetData* detDataAll = 0;
      MuonDQAHitHistTool::DetData* detDataSubdet = 0;
      MuonDQAHitHistTool::DetData* detDataSector = 0;

      const MdtIdHelper *m_mdtIdHelper = m_muonMgr->mdtIdHelper();
      const RpcIdHelper *m_rpcIdHelper = m_muonMgr->rpcIdHelper();
      const CscIdHelper *m_cscIdHelper = m_muonMgr->cscIdHelper();
      const TgcIdHelper *m_tgcIdHelper = m_muonMgr->tgcIdHelper(); // compilation warning
      
      hitData.residualData.all.push_back(MuonDQAHitHistTool::DetData());
      detDataAll = &hitData.residualData.all.back();
      ++hitData.countData.all.nall;

      if( m_idHelperTool->isMdt(id) ){ // MDTs
	hitData.residualData.mdts.push_back(MuonDQAHitHistTool::DetData());
	detData = &hitData.residualData.mdts.back();
	mdtChId.insert(chid);
	++hitData.countData.mdts.nmdt;
	std::string ChName = convertChamberName(m_mdtIdHelper->stationName(id),m_mdtIdHelper->stationEta(id),m_mdtIdHelper->stationPhi(id),"MDT");
	if ( !(m_idHelperTool->isEndcap(id)) && m_mdtIdHelper->stationEta(id)>0 ){ // Barrel A
	  hitData.residualData.mdtsBarrelA.push_back(MuonDQAHitHistTool::DetData());
	  detDataSubdet = &hitData.residualData.mdtsBarrelA.back();
	  ++hitData.countData.mdtsBarrelA.nmdt;
	  if ( m_mdtIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.mdtsBarrelA1.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA1.back();
	    ++hitData.countData.mdtsBarrelA1.nmdt;
	    hitData.countData.mdtsBarrelA1.name = ChName;
	    hitData.countData.mdtsBarrelA1.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.mdtsBarrelA2.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA2.back();
	    ++hitData.countData.mdtsBarrelA2.nmdt;
	    hitData.countData.mdtsBarrelA2.name = ChName;
	    hitData.countData.mdtsBarrelA2.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.mdtsBarrelA3.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA3.back();
	    ++hitData.countData.mdtsBarrelA3.nmdt;
	    hitData.countData.mdtsBarrelA3.name = ChName;
	    hitData.countData.mdtsBarrelA3.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.mdtsBarrelA4.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA4.back();
	    ++hitData.countData.mdtsBarrelA4.nmdt;
	    hitData.countData.mdtsBarrelA4.name = ChName;
	    hitData.countData.mdtsBarrelA4.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.mdtsBarrelA5.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA5.back();
	    ++hitData.countData.mdtsBarrelA5.nmdt;
	    hitData.countData.mdtsBarrelA5.name = ChName;
	    hitData.countData.mdtsBarrelA5.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.mdtsBarrelA6.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA6.back();
	    ++hitData.countData.mdtsBarrelA6.nmdt;
	    hitData.countData.mdtsBarrelA6.name = ChName;
	    hitData.countData.mdtsBarrelA6.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.mdtsBarrelA7.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA7.back();
	    ++hitData.countData.mdtsBarrelA7.nmdt;
	    hitData.countData.mdtsBarrelA7.name = ChName;
	    hitData.countData.mdtsBarrelA7.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.mdtsBarrelA8.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelA8.back();
	    ++hitData.countData.mdtsBarrelA8.nmdt;
	    hitData.countData.mdtsBarrelA8.name = ChName;
	    hitData.countData.mdtsBarrelA8.chi2 = chi2;
	  }
	}
	else if ( !(m_idHelperTool->isEndcap(id)) && m_mdtIdHelper->stationEta(id)<0 ){ // Barrel C
	  hitData.residualData.mdtsBarrelC.push_back(MuonDQAHitHistTool::DetData());
	  detDataSubdet = &hitData.residualData.mdtsBarrelC.back();
	  ++hitData.countData.mdtsBarrelC.nmdt;
	  if ( m_mdtIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.mdtsBarrelC1.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC1.back();
	    ++hitData.countData.mdtsBarrelC1.nmdt;
	    hitData.countData.mdtsBarrelC1.name = ChName;
	    hitData.countData.mdtsBarrelC1.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.mdtsBarrelC2.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC2.back();
	    ++hitData.countData.mdtsBarrelC2.nmdt;
	    hitData.countData.mdtsBarrelC2.name = ChName;
	    hitData.countData.mdtsBarrelC2.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.mdtsBarrelC3.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC3.back();
	    ++hitData.countData.mdtsBarrelC3.nmdt;
	    hitData.countData.mdtsBarrelC3.name = ChName;
	    hitData.countData.mdtsBarrelC3.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.mdtsBarrelC4.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC4.back();
	    ++hitData.countData.mdtsBarrelC4.nmdt;
	    hitData.countData.mdtsBarrelC4.name = ChName;
	    hitData.countData.mdtsBarrelC4.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.mdtsBarrelC5.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC5.back();
	    ++hitData.countData.mdtsBarrelC5.nmdt;
	    hitData.countData.mdtsBarrelC5.name = ChName;
	    hitData.countData.mdtsBarrelC5.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.mdtsBarrelC6.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC6.back();
	    ++hitData.countData.mdtsBarrelC6.nmdt;
	    hitData.countData.mdtsBarrelC6.name = ChName;
	    hitData.countData.mdtsBarrelC6.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.mdtsBarrelC7.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC7.back();
	    ++hitData.countData.mdtsBarrelC7.nmdt;
	    hitData.countData.mdtsBarrelC7.name = ChName;
	    hitData.countData.mdtsBarrelC7.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.mdtsBarrelC8.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsBarrelC8.back();
	    ++hitData.countData.mdtsBarrelC8.nmdt;
	    hitData.countData.mdtsBarrelC8.name = ChName;
	    hitData.countData.mdtsBarrelC8.chi2 = chi2;
	  }
	}
	else if ( m_idHelperTool->isEndcap(id) && m_mdtIdHelper->stationEta(id)>0 ){ // EC A
	  hitData.residualData.mdtsECA.push_back(MuonDQAHitHistTool::DetData());
	  detDataSubdet = &hitData.residualData.mdtsECA.back();
	  ++hitData.countData.mdtsECA.nmdt;
	  if ( m_mdtIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.mdtsECA1.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA1.back();
	    ++hitData.countData.mdtsECA1.nmdt;
	    hitData.countData.mdtsECA1.name = ChName;
	    hitData.countData.mdtsECA1.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.mdtsECA2.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA2.back();
	    ++hitData.countData.mdtsECA2.nmdt;
	    hitData.countData.mdtsECA2.name = ChName;
	    hitData.countData.mdtsECA2.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.mdtsECA3.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA3.back();
	    ++hitData.countData.mdtsECA3.nmdt;
	    hitData.countData.mdtsECA3.name = ChName;
	    hitData.countData.mdtsECA3.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.mdtsECA4.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA4.back();
	    ++hitData.countData.mdtsECA4.nmdt;
	    hitData.countData.mdtsECA4.name = ChName;
	    hitData.countData.mdtsECA4.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.mdtsECA5.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA5.back();
	    ++hitData.countData.mdtsECA5.nmdt;
	    hitData.countData.mdtsECA5.name = ChName;
	    hitData.countData.mdtsECA5.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.mdtsECA6.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA6.back();
	    ++hitData.countData.mdtsECA6.nmdt;
	    hitData.countData.mdtsECA6.name = ChName;
	    hitData.countData.mdtsECA6.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.mdtsECA7.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA7.back();
	    ++hitData.countData.mdtsECA7.nmdt;
	    hitData.countData.mdtsECA7.name = ChName;
	    hitData.countData.mdtsECA7.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.mdtsECA8.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECA8.back();
	    ++hitData.countData.mdtsECA8.nmdt;
	    hitData.countData.mdtsECA8.name = ChName;
	    hitData.countData.mdtsECA8.chi2 = chi2;
	  }
	}
	else if ( m_idHelperTool->isEndcap(id) && m_mdtIdHelper->stationEta(id)<0 ){ // EC C
	  hitData.residualData.mdtsECC.push_back(MuonDQAHitHistTool::DetData());
	  detDataSubdet = &hitData.residualData.mdtsECC.back();
	  ++hitData.countData.mdtsECC.nmdt;
	  if ( m_mdtIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.mdtsECC1.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC1.back();
	    ++hitData.countData.mdtsECC1.nmdt;
	    hitData.countData.mdtsECC1.name = ChName;
	    hitData.countData.mdtsECC1.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.mdtsECC2.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC2.back();
	    ++hitData.countData.mdtsECC2.nmdt;
	    hitData.countData.mdtsECC2.name = ChName;
	    hitData.countData.mdtsECC2.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.mdtsECC3.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC3.back();
	    ++hitData.countData.mdtsECC3.nmdt;
	    hitData.countData.mdtsECC3.name = ChName;
	    hitData.countData.mdtsECC3.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.mdtsECC4.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC4.back();
	    ++hitData.countData.mdtsECC4.nmdt;
	    hitData.countData.mdtsECC4.name = ChName;
	    hitData.countData.mdtsECC4.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.mdtsECC5.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC5.back();
	    ++hitData.countData.mdtsECC5.nmdt;
	    hitData.countData.mdtsECC5.name = ChName;
	    hitData.countData.mdtsECC5.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.mdtsECC6.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC6.back();
	    ++hitData.countData.mdtsECC6.nmdt;
	    hitData.countData.mdtsECC6.name = ChName;
	    hitData.countData.mdtsECC6.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.mdtsECC7.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC7.back();
	    ++hitData.countData.mdtsECC7.nmdt;
	    hitData.countData.mdtsECC7.name = ChName;
	    hitData.countData.mdtsECC7.chi2 = chi2;
	  }
	  if ( m_mdtIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.mdtsECC8.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.mdtsECC8.back();
	    ++hitData.countData.mdtsECC8.nmdt;
	    hitData.countData.mdtsECC8.name = ChName;
	    hitData.countData.mdtsECC8.chi2 = chi2;
	  }
	}
      }
      else if( m_idHelperTool->isCsc(id) ){
	if( measuresPhi ) { 
	  hitData.residualData.cscsPhi.push_back(MuonDQAHitHistTool::DetData());
	  detData = &hitData.residualData.cscsPhi.back();
	  if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.cscsPhi1.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi1.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.cscsPhi2.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi2.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.cscsPhi3.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi3.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.cscsPhi4.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi4.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.cscsPhi5.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi5.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.cscsPhi6.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi6.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.cscsPhi7.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi7.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.cscsPhi8.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi8.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.cscsPhi9.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi9.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.cscsPhi10.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi10.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.cscsPhi11.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi11.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.cscsPhi12.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi12.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.cscsPhi13.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi13.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.cscsPhi14.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi14.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.cscsPhi15.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi15.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.cscsPhi16.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsPhi16.back();
	  }
	  cscPhiChId.insert(chid);
	  ++hitData.countData.cscs.nphi;
	}else{
	  hitData.residualData.cscsEta.push_back(MuonDQAHitHistTool::DetData());
	  detData = &hitData.residualData.cscsEta.back();
	  if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.cscsEta1.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta1.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==1 ){
	    hitData.residualData.cscsEta2.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta2.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.cscsEta3.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta3.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==2 ){
	    hitData.residualData.cscsEta4.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta4.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.cscsEta5.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta5.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==3 ){
	    hitData.residualData.cscsEta6.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta6.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.cscsEta7.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta7.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==4 ){
	    hitData.residualData.cscsEta8.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta8.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.cscsEta9.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta9.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==5 ){
	    hitData.residualData.cscsEta10.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta10.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.cscsEta11.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta11.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==6 ){
	    hitData.residualData.cscsEta12.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta12.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.cscsEta13.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta13.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==7 ){
	    hitData.residualData.cscsEta14.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta14.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSL" && m_cscIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.cscsEta15.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta15.back();
	  }else if ( m_idHelperTool->chamberNameString(id)=="CSS" && m_cscIdHelper->stationPhi(id)==8 ){
	    hitData.residualData.cscsEta16.push_back(MuonDQAHitHistTool::DetData());
	    detDataSector = &hitData.residualData.cscsEta16.back();
	  }
	  cscEtaChId.insert(chid);
	  ++hitData.countData.cscs.neta;
	}
      }
      	
      else if( m_idHelperTool->isRpc(id) ){
	if( measuresPhi ){                          // RPCs phi
	  hitData.residualData.rpcsPhi.push_back(MuonDQAHitHistTool::DetData());
	  detData = &hitData.residualData.rpcsPhi.back();
	  rpcPhiChId.insert(chid);
	  ++hitData.countData.rpcs.nphi;
	  if ( m_rpcIdHelper->stationEta(id)>0 ) { // Barrel A
	    hitData.residualData.rpcsPhiA.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.rpcsPhiA.back();
	    if ( m_rpcIdHelper->stationPhi(id)==1 ){
	      hitData.residualData.rpcsPhiA1.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA1.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==2 ){
	      hitData.residualData.rpcsPhiA2.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA2.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==3 ){
	      hitData.residualData.rpcsPhiA3.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA3.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==4 ){
	      hitData.residualData.rpcsPhiA4.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA4.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==5 ){
	      hitData.residualData.rpcsPhiA5.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA5.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==6 ){
	      hitData.residualData.rpcsPhiA6.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA6.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==7 ){
	      hitData.residualData.rpcsPhiA7.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA7.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==8 ){
	      hitData.residualData.rpcsPhiA8.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiA8.back();
	    }
	  }
	  else if ( m_rpcIdHelper->stationEta(id)<0 ) { // Barrel C
	    hitData.residualData.rpcsPhiC.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.rpcsPhiC.back();
	    if ( m_rpcIdHelper->stationPhi(id)==1 ){
	      hitData.residualData.rpcsPhiC1.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC1.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==2 ){
	      hitData.residualData.rpcsPhiC2.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC2.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==3 ){
	      hitData.residualData.rpcsPhiC3.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC3.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==4 ){
	      hitData.residualData.rpcsPhiC4.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC4.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==5 ){
	      hitData.residualData.rpcsPhiC5.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC5.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==6 ){
	      hitData.residualData.rpcsPhiC6.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC6.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==7 ){
	      hitData.residualData.rpcsPhiC7.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC7.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==8 ){
	      hitData.residualData.rpcsPhiC8.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsPhiC8.back();
	    }
	  }
	}else{                           // RPCs eta
	  hitData.residualData.rpcsEta.push_back(MuonDQAHitHistTool::DetData());
	  detData = &hitData.residualData.rpcsEta.back();
	  rpcEtaChId.insert(chid);
	  ++hitData.countData.rpcs.neta;
	  if ( m_rpcIdHelper->stationEta(id)>0 ) { // Barrel A
	    hitData.residualData.rpcsEtaA.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.rpcsEtaA.back();
	    if ( m_rpcIdHelper->stationPhi(id)==1 ){
	      hitData.residualData.rpcsEtaA1.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA1.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==2 ){
	      hitData.residualData.rpcsEtaA2.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA2.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==3 ){
	      hitData.residualData.rpcsEtaA3.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA3.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==4 ){
	      hitData.residualData.rpcsEtaA4.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA4.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==5 ){
	      hitData.residualData.rpcsEtaA5.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA5.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==6 ){
	      hitData.residualData.rpcsEtaA6.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA6.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==7 ){
	      hitData.residualData.rpcsEtaA7.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA7.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==8 ){
	      hitData.residualData.rpcsEtaA8.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaA8.back();
	    }
	  }
	  else if ( m_rpcIdHelper->stationEta(id)<0 ) { // Barrel C
	    hitData.residualData.rpcsEtaC.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.rpcsEtaC.back();
	    if ( m_rpcIdHelper->stationPhi(id)==1 ){
	      hitData.residualData.rpcsEtaC1.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC1.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==2 ){
	      hitData.residualData.rpcsEtaC2.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC2.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==3 ){
	      hitData.residualData.rpcsEtaC3.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC3.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==4 ){
	      hitData.residualData.rpcsEtaC4.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC4.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==5 ){
	      hitData.residualData.rpcsEtaC5.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC5.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==6 ){
	      hitData.residualData.rpcsEtaC6.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC6.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==7 ){
	      hitData.residualData.rpcsEtaC7.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC7.back();
	    }
	    if ( m_rpcIdHelper->stationPhi(id)==8 ){
	      hitData.residualData.rpcsEtaC8.push_back(MuonDQAHitHistTool::DetData());
	      detDataSector = &hitData.residualData.rpcsEtaC8.back();
	    }
	  }
	}
	
      }else if( m_idHelperTool->isTgc(id) ){
	if( measuresPhi ){                            // TGCs phi
	  hitData.residualData.tgcsPhi.push_back(MuonDQAHitHistTool::DetData());
	  detData = &hitData.residualData.tgcsPhi.back();
	  if ( m_tgcIdHelper->stationEta(id)>0 ) { // EC A
	    hitData.residualData.tgcsPhiA.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.tgcsPhiA.back();
	  }
	  else if ( m_tgcIdHelper->stationEta(id)<0 ) { // EC C
	    hitData.residualData.tgcsPhiC.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.tgcsPhiC.back();
	  }
	  tgcPhiChId.insert(chid);
	  ++hitData.countData.tgcs.nphi;
	}else{                            // TGCs eta
	  hitData.residualData.tgcsEta.push_back(MuonDQAHitHistTool::DetData());
	  detData = &hitData.residualData.tgcsEta.back();
	  if ( m_tgcIdHelper->stationEta(id)>0 ) { // EC A
	    hitData.residualData.tgcsEtaA.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.tgcsEtaA.back();
	  }
	  else if ( m_tgcIdHelper->stationEta(id)<0 ) { // EC C
	    hitData.residualData.tgcsEtaC.push_back(MuonDQAHitHistTool::DetData());
	    detDataSubdet = &hitData.residualData.tgcsEtaC.back();
	  }
	  tgcEtaChId.insert(chid);
	  ++hitData.countData.tgcs.neta;
	}
      }else{
	*m_log << MSG::WARNING << " Unknown detector type " << endreq;
      }

      // Chamber name
      if( m_idHelperTool->isMdt(id) ){
	if (detDataSector!=0) {
	  detDataSector->name = convertChamberName(m_mdtIdHelper->stationName(id),m_mdtIdHelper->stationEta(id),m_mdtIdHelper->stationPhi(id),"MDT");
	}
      }
      
      // pointer to resPull
      const Trk::ResidualPull* resPull = m_pullCalculator->residualPull(meas, pars, Trk::ResidualPull::Unbiased );
      if (detData!=0) {
	detData->pull  = resPull ? resPull->pull().front() : -9999;
	detData->res   = resPull ? resPull->residual().front() : -9999;
      }
      if (detDataAll!=0) {
	detDataAll->pull  = resPull ? resPull->pull().front() : -9999;
	detDataAll->res   = resPull ? resPull->residual().front() : -9999;
      }
      if (detDataSubdet!=0) {
	detDataSubdet->pull  = resPull ? resPull->pull().front() : -9999;
	detDataSubdet->res   = resPull ? resPull->residual().front() : -9999;
      }
      if (detDataSector!=0) {
	detDataSector->pull  = resPull ? resPull->pull().front() : -9999;
	detDataSector->res   = resPull ? resPull->residual().front() : -9999;
      }
      delete resPull;

      if (detData!=0) {
	detData->error = meas->localCovariance()(Trk::locX,Trk::locX);
	//      detData->error = 0.;
	detData->lposX = meas->localParameters()[Trk::locX];
      }
      if (detDataAll!=0) {
	detDataAll->error = 0.;
	detDataAll->lposX = meas->localParameters()[Trk::locX];
      }
      if (detDataSubdet!=0) {
	detDataSubdet->error = meas->localCovariance()(Trk::locX,Trk::locX);
	detDataSubdet->lposX = meas->localParameters()[Trk::locX];
      }
      if (detDataSector!=0) {
	detDataSector->error = meas->localCovariance()(Trk::locX,Trk::locX);
	detDataSector->lposX = meas->localParameters()[Trk::locX];
      }

      const Amg::Vector2D* locPos = meas->associatedSurface().globalToLocal(pars->position());      
//      const Trk::LocalPosition* locPos = meas->associatedSurface().globalToLocal(pars->position());
      if (detData!=0) {
	detData->lposY = locPos ? (*locPos)[Trk::locY] : -9999.;
      }
      if (detDataAll!=0) {
	detDataAll->lposY = locPos ? (*locPos)[Trk::locY] : -9999.;
      }
      if (detDataSubdet!=0) {
	detDataSubdet->lposY = locPos ? (*locPos)[Trk::locY] : -9999.;
      }
      if (detDataSector!=0) {
	detDataSector->lposY = locPos ? (*locPos)[Trk::locY] : -9999.;
      }
      delete locPos;
      
    }

    hitData.countData.all.nallCh = mdtChId.size() + cscEtaChId.size() + cscPhiChId.size() + rpcEtaChId.size() + rpcPhiChId.size() + tgcEtaChId.size() + tgcPhiChId.size(); 
    hitData.countData.mdts.nmdtCh = mdtChId.size();
    hitData.countData.cscs.netaCh = cscEtaChId.size(); 
    hitData.countData.cscs.nphiCh = cscPhiChId.size(); 
    hitData.countData.rpcs.netaCh = rpcEtaChId.size(); 
    hitData.countData.rpcs.nphiCh = rpcPhiChId.size(); 
    hitData.countData.tgcs.netaCh = tgcEtaChId.size(); 
    hitData.countData.tgcs.nphiCh = tgcPhiChId.size(); 

  }

  void MuonDQATrackHistTool::extract( MuonDQAHitHistTool::HitData& hitData, const Trk::Track& track, const double chi2 ) const {

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ) return;

    HitPairVec hits;
    hits.reserve(states->size());
    
    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){

      // scattering center       
      if( (*tsit)->type(Trk::TrackStateOnSurface::Scatterer ) ) { 
	continue;
      }
  
      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ){	
	continue;
      }

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ){
	continue;
      }
      
      hits.push_back( std::make_pair(meas,pars) );

    }

    extract(hitData,hits,chi2);
  }

  void MuonDQATrackHistTool::extract( TrackData& trackData, const Trk::Track& track ) const {
    const Trk::FitQuality* fq = track.fitQuality();
    if( fq ) {
      trackData.chi2 = fq->chiSquared();
      trackData.ndof = fq->numberDoF();
    }
    
    const Trk::Perigee* pp = track.perigeeParameters();
    if( pp ){
      trackData.charge = pp->charge();
      trackData.gpos = pp->position();
      trackData.gdir = pp->momentum();
    }

    const MdtIdHelper *m_mdtIdHelper = m_muonMgr->mdtIdHelper();
    std::set<int> multiLayers;
    std::set<int> tubeLayers;
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit) {
      const Trk::MeasurementBase* rio = (*tsit)->measurementOnTrack();
      if (!rio) continue;
      Identifier id = m_helperTool->getIdentifier(*rio);
      multiLayers.insert(m_mdtIdHelper->multilayer(id));
      tubeLayers.insert( (m_mdtIdHelper->multilayer(id)-1)*4 + m_mdtIdHelper->tubeLayer(id) );
    }
    trackData.layers = tubeLayers.size();
    trackData.mlayers = multiLayers.size();
    
    extract( trackData.hitData, track, trackData.chi2 );
  }


  bool MuonDQATrackHistTool::fill( MuonDQAHitHistTool::HitHistos& histos, const Trk::Track& track, const double chi2 ) const {
    if( !histos.wasRegistered ) return false;


    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      return false;
    }

    HitPairVec hits;
    hits.reserve(states->size());
    
    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      // scattering center 
      if( (*tsit)->type(Trk::TrackStateOnSurface::Scatterer ) ) {  
	continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ){	
	continue;
      }

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ){
	continue;
      }
      
      hits.push_back( std::make_pair(meas,pars) );

    }

    // extract data 
    MuonDQAHitHistTool::HitData hitData;
    extract(hitData,hits,chi2);

    m_hitHistTool->fill( histos, hitData );

    return true;
  }

  /*  bool MuonDQATrackHistTool::fill( MuonDQATrackHistTool::TrackHistos& histos, const TrackCollection& tracks ) const {
      if( !histos.wasRegistered ) return false;
      TrackCollection::const_iterator tit = tracks.begin();
      TrackCollection::const_iterator tit_end = tracks.end();
      for( ;tit!=tit_end;++tit ){
      if( !*tit ) continue;
      fill(histos,**tit);
      }
      return true;
      } */

}
 
 
