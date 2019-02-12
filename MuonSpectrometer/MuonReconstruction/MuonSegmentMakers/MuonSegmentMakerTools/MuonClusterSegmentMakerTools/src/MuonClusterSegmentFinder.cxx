/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterSegmentFinder.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"

// ROOT includes
#include "TTree.h"
#include "TFile.h"

// AthExHistNtup includes
#include "MuonLinearSegmentMakerUtilities/ClusterAnalysis.h"
     
// EDM
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "HepMC/GenEvent.h"

// interfaces
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonClusterization/IMuonClusterizationTool.h"
     
namespace Muon {

 MuonClusterSegmentFinder::MuonClusterSegmentFinder(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_layerHashProvider("Muon::MuonLayerHashProviderTool/MuonLayerHashProviderTool"),
    m_muonPRDSelectionTool("Muon::MuonPRDSelectionTool/MuonPRDSelectionTool"),
    m_segmentMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
    m_clusterTool("Muon::MuonClusterizationTool/MuonClusterizationTool"),
    m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_segmentOverlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool")
 {

    declareInterface<IMuonClusterSegmentFinder>(this);
    
    declareProperty("MuonClusterizationTool", m_clusterTool);
    declareProperty("MuonIdHelperTool",m_idHelper );    
    declareProperty("MuonEDMPrinterTool",m_printer );    
    declareProperty("MuonPRDSelectionTool", m_muonPRDSelectionTool );
    declareProperty("MdtSegmentMaker",m_segmentMaker);
    declareProperty("SLFitter",            m_slTrackFitter);
    declareProperty("TrackToSegmentTool",  m_trackToSegmentTool);
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor);
    declareProperty("DoNtuple",m_doNtuple = false);
    declareProperty("TrackCleaner",        m_trackCleaner);

  }

 MuonClusterSegmentFinder::~MuonClusterSegmentFinder() { }

  StatusCode MuonClusterSegmentFinder::finalize() {

    if( m_doNtuple ){
      TDirectory* cdir = gDirectory;
      m_file->cd();
      m_tree->Write();
      m_file->Write();
      m_file->Close();
      delete m_ntuple;
      gDirectory = cdir;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode MuonClusterSegmentFinder::initialize() {

    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_layerHashProvider.retrieve());
    ATH_CHECK(m_muonPRDSelectionTool.retrieve());
    ATH_CHECK(m_segmentMaker.retrieve());
    ATH_CHECK(m_clusterTool.retrieve());
    ATH_CHECK(m_clusterCreator.retrieve());
    ATH_CHECK(m_trackToSegmentTool.retrieve());
    ATH_CHECK(m_slTrackFitter.retrieve());
    ATH_CHECK(m_ambiguityProcessor.retrieve());
    ATH_CHECK(m_helper.retrieve());
    ATH_CHECK(m_trackCleaner.retrieve());
    ATH_CHECK(m_segmentOverlapRemovalTool.retrieve());

    if( m_doNtuple ){
      TDirectory* cdir = gDirectory;
      m_file = new TFile("ClusterNtuple.root","RECREATE");
      m_tree = new TTree("clusters","clusters");
      m_ntuple = new ClusterSeg::ClusterNtuple();
      m_ntuple->initForWrite(*m_tree);
      gDirectory = cdir;
    }else{
      m_file = 0;
      m_tree = 0;
      m_ntuple = 0;
    }

    return StatusCode::SUCCESS;
  }
    
  bool sortfunctionMB (const Trk::MeasurementBase* i, const Trk::MeasurementBase* j){return (fabs(i->globalPosition().z()) < fabs(j->globalPosition().z()));}
  bool sortfunctionSP (ClusterSeg::SpacePoint i, ClusterSeg::SpacePoint j){
return (fabs(i.z()) < fabs(j.z()));}
  bool sortfunctionVec (std::pair<int,int> i, std::pair<int,int> j){return (i.second < j.second);}

  bool MuonClusterSegmentFinder::matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id,int& bcode) const {
    typedef PRD_MultiTruthCollection::const_iterator iprdt;
    std::pair<iprdt, iprdt> range = truthCol.equal_range(id); 
    // Loop over particles contributing to this cluster
    for(iprdt i = range.first; i != range.second; i++) {
      if(!i->second.isValid()) {
        ATH_MSG_INFO( "Unexpected invalid HepMcParticleLink in PRD_MultiTruthCollection" );
      } else {
        const HepMcParticleLink& link = i->second;
        if( link.cptr() ){
          if (abs(link.cptr()->pdg_id()) == 13 ){
            bcode = link.barcode();
            return true;
          }
        }
      }
    }
    return false;
  }

  void MuonClusterSegmentFinder::getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont,
						    std::vector<const Muon::TgcPrepDataCollection*>* tgcCols, 
						    std::vector<const Muon::RpcPrepDataCollection*>* rpcCols,
						    const PRD_MultiTruthCollection* tgcTruthColl, const PRD_MultiTruthCollection* rpcTruthColl,
						    Trk::SegmentCollection* segColl) const {

    if(tgcCols){
      Muon::TgcPrepDataContainer* clusterPRD=new Muon::TgcPrepDataContainer(m_idHelper->tgcIdHelper().module_hash_max());
      for(const auto tgcCol : *tgcCols){
	Muon::TgcPrepDataCollection* clusteredCol = m_clusterTool->cluster(*tgcCol);
	if(clusteredCol) clusterPRD->addCollection(clusteredCol, tgcCol->identifyHash() ).ignore();
      }
      std::vector<const Muon::TgcPrepDataCollection*> theTGCs;
      Muon::TgcPrepDataContainer::const_iterator theIt = clusterPRD->begin();
      int theEmpty(0),contInt(-1);
      for (;theIt!=clusterPRD->end();theIt++){
	contInt++;
        if ((*theIt)->size() == 0) {
          theEmpty++;
	}
        theTGCs.push_back(*theIt);
      }

      findSegments(theTGCs,mdtPrdCont,segColl,tgcTruthColl);
    }//end if TGC

    if(rpcCols){
      Muon::RpcPrepDataContainer* clusterPRD=new Muon::RpcPrepDataContainer(m_idHelper->rpcIdHelper().module_hash_max());
      for(const auto rpcCol : *rpcCols){
	Muon::RpcPrepDataCollection* clusteredCol = m_clusterTool->cluster(*rpcCol);
        if(clusteredCol) clusterPRD->addCollection(clusteredCol, rpcCol->identifyHash() ).ignore();
      }
      std::vector<const Muon::RpcPrepDataCollection*> theRPCs;
      Muon::RpcPrepDataContainer::const_iterator theIt = clusterPRD->begin();
      int theEmpty(0),contInt(-1);
      for (;theIt!=clusterPRD->end();theIt++){
        contInt++;
        if ((*theIt)->size() == 0) {
          theEmpty++;
        }
        theRPCs.push_back(*theIt);
      }

      findSegments(theRPCs,mdtPrdCont,segColl,rpcTruthColl);
    }//end if RPC

  }

  void MuonClusterSegmentFinder::getClusterSegments(const Muon::MdtPrepDataContainer* mdtPrdCont,
						    const Muon::RpcPrepDataContainer* rpcPrdCont, const Muon::TgcPrepDataContainer* tgcPrdCont,
						    const PRD_MultiTruthCollection* tgcTruthColl, const PRD_MultiTruthCollection* rpcTruthColl,
						    Trk::SegmentCollection* segColl) const {

    if (tgcPrdCont) { 

      Muon::TgcPrepDataContainer* clusterPRD = m_clusterTool->cluster(*tgcPrdCont);
      std::vector<const Muon::TgcPrepDataCollection*> theTGCs;
      Muon::TgcPrepDataContainer::const_iterator theIt = clusterPRD->begin();
      int theEmpty(0),contInt(-1);
      for (;theIt!=clusterPRD->end();theIt++){
        contInt++;
        if ((*theIt)->size() == 0) {
          theEmpty++;
        }
        theTGCs.push_back(*theIt);
      }

      findSegments(theTGCs,mdtPrdCont,segColl,tgcTruthColl);
    }//end if TGC

    if (rpcPrdCont) {

      Muon::RpcPrepDataContainer* clusterPRD = m_clusterTool->cluster(*rpcPrdCont);
      std::vector<const Muon::RpcPrepDataCollection*> theRPCs;
      Muon::RpcPrepDataContainer::const_iterator theIt = clusterPRD->begin();
      int theEmpty(0),contInt(-1);
      for (;theIt!=clusterPRD->end();theIt++){
        contInt++;
        if ((*theIt)->size() == 0) {
          theEmpty++;
        }
        theRPCs.push_back(*theIt);
      }

      findSegments(theRPCs,mdtPrdCont,segColl,rpcTruthColl);
    }//end if rpc

  }

  void MuonClusterSegmentFinder::findSegments( std::vector<const TgcPrepDataCollection*>& tgcCols, const Muon::MdtPrepDataContainer* mdtPrdCont,
					       Trk::SegmentCollection* segColl, const PRD_MultiTruthCollection* tgcTruthColl) const {
    ATH_MSG_INFO ("Executing " << name() << "...");
    ATH_MSG_DEBUG("start with "<<segColl->size()<<" segments");

    candEvent*  thisEvent = new candEvent;

    makeClusterVecs(tgcTruthColl,tgcCols,thisEvent);
	
    ClusterSeg::ClusterAnalysis theAnalysis;
    ATH_MSG_DEBUG("the size of Clust is " << thisEvent->Clust().size() );
    std::vector<std::vector<ClusterSeg::SpacePoint>> sPoints = theAnalysis.analyse(thisEvent->Clust());
	
    processSpacePoints(thisEvent,sPoints);
    if(thisEvent->segTrkColl()->size() > 0) ATH_MSG_DEBUG( "it made at least one track " );
    else ATH_MSG_DEBUG("processSpacePoints didn't make anything");

    std::map<int,bool> themap;
    findOverlap(themap,thisEvent);
    resolveCollections(themap,thisEvent);

    getSegments(thisEvent, mdtPrdCont, segColl);

    ATH_MSG_DEBUG("now have " << segColl->size()<<" segments" );

    if (m_doNtuple) {
      m_ntuple->fill(thisEvent->Clust());
      m_tree->Fill();
    }
    delete thisEvent;
  }

  void MuonClusterSegmentFinder::findSegments(std::vector<const RpcPrepDataCollection*>& rpcCols, const Muon::MdtPrepDataContainer* mdtPrdCont,
					      Trk::SegmentCollection* segColl, const PRD_MultiTruthCollection* rpcTruthColl ) const {
    ATH_MSG_INFO ("Executing " << name() << "...");
    ATH_MSG_DEBUG("start with "<<segColl->size()<<" segments");

    candEvent * thisEvent = new candEvent;

    makeClusterVecs(rpcTruthColl,rpcCols,thisEvent);
 
    ClusterSeg::ClusterAnalysis theAnalysis;
    ATH_MSG_DEBUG("the size of Clust is " << thisEvent->Clust().size() );
    std::vector<std::vector<ClusterSeg::SpacePoint>> sPoints = theAnalysis.analyse(thisEvent->Clust()); 

    processSpacePoints(thisEvent,sPoints);
    if(thisEvent->segTrkColl()->size() > 0) ATH_MSG_DEBUG( "it made at least one track " );
    else ATH_MSG_DEBUG("processSpacePoints didn't make anything");

    std::map<int,bool> themap;
    findOverlap(themap,thisEvent);
    resolveCollections(themap,thisEvent);

    getSegments(thisEvent,mdtPrdCont,segColl);

    ATH_MSG_DEBUG("now have " << segColl->size()<<" segments" );

    if (m_doNtuple) {
      m_ntuple->fill(thisEvent->Clust());
      m_tree->Fill();
    }
    delete thisEvent;

  }

  void MuonClusterSegmentFinder::findOverlap(std::map<int,bool>& themap,candEvent * theEvent) const {	
    CompareMuonSegmentKeys compareKeys;
    if(theEvent->keyVector().size() > 1){
      for (unsigned int i=0;i<theEvent->keyVector().size();i++){themap.insert(std::pair<int,bool>(i,true));}
      for(unsigned int i=0;i<theEvent->keyVector().size()-1;i++){
        for(unsigned int j=i+1;j<theEvent->keyVector().size();j++){
          CompareMuonSegmentKeys::OverlapResult overlap = compareKeys(theEvent->keyVector()[i],theEvent->keyVector()[j]);
          if(overlap == 0 || overlap == 2) themap[j] = false;
          if(overlap == 1) themap[i] = false;
        }
      }
    }
  }
	
  Trk::Track* MuonClusterSegmentFinder::fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const {
    Trk::Track* segtrack = m_slTrackFitter->fit(vec2,startpar,false,Trk::nonInteracting);
	 
    if(segtrack) {
      ATH_MSG_DEBUG( "segment fit succeeded");
	
      Trk::Track* cleanedTrack = m_trackCleaner->clean(*segtrack);
      if( cleanedTrack && cleanedTrack != segtrack ){
        delete segtrack;
        segtrack = cleanedTrack;
      }else{ ATH_MSG_DEBUG("track remains unchanged");}
	
	
      if( !m_helper->goodTrack(*segtrack,30) && vec2.size() > 4) {
        if(msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << "bad segment fit:" ;
          if (segtrack->fitQuality()) msg(MSG::DEBUG) << "with chi^2/nDoF = " << segtrack->fitQuality()->chiSquared() << "/" << segtrack->fitQuality()->numberDoF() ;
          msg(MSG::DEBUG) << endmsg;
        }
        delete segtrack;
        segtrack = 0;
      }
    }
    return segtrack;
  }

  void MuonClusterSegmentFinder::makeClusterVecs(const std::vector<const Muon::MuonClusterOnTrack*>& clustCol,candEvent* theEvent) const {
    for( std::vector<const Muon::MuonClusterOnTrack*>::const_iterator colIt = clustCol.begin(); colIt != clustCol.end(); ++ colIt ){
      const MuonClusterOnTrack* clust = *colIt;
      MuonStationIndex::PhiIndex pIndex = m_idHelper->phiIndex(clust->identify());
      bool tmatch(false);
      int barcode(0);
      if (m_idHelper->measuresPhi( clust->identify())){
        theEvent->clusters().push_back(clust);
        ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),true,MuonStationIndex::TechnologyIndex::TGC,pIndex,tmatch,barcode);
        theEvent->Clust().push_back(cluster);
      } else {
        theEvent->clusters().push_back(clust);
        ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),false,MuonStationIndex::TechnologyIndex::TGC,pIndex,tmatch,barcode);
        theEvent->Clust().push_back(cluster);
      }
    }
  }

  void MuonClusterSegmentFinder::makeClusterVecs(const PRD_MultiTruthCollection* truthCollectionTGC, const std::vector<const TgcPrepDataCollection*>& tgcCols,candEvent* theEvent) const {
    for( std::vector<const TgcPrepDataCollection*>::const_iterator colIt = tgcCols.begin(); colIt != tgcCols.end(); ++ colIt ){
      TgcPrepDataCollection::const_iterator pit = (*colIt)->begin();
      for(; pit!=(*colIt)->end(); ++pit) {
        const MuonCluster* cl = *pit;
        if( !cl ) continue;
        bool tmatch = false;
        int barcode = 0;
        if(truthCollectionTGC){
          const Identifier& id = (*pit)->identify();
          tmatch = matchTruth(*truthCollectionTGC,id,barcode);
        }
        const MuonClusterOnTrack* clust = m_clusterCreator->createRIO_OnTrack( *cl, cl->globalPosition() );
        MuonStationIndex::PhiIndex pIndex = m_idHelper->phiIndex(clust->identify());
        if (m_idHelper->measuresPhi( clust->identify())){
          theEvent->clusters().push_back(clust);
          ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),true,MuonStationIndex::TechnologyIndex::TGC,pIndex,tmatch,barcode);
          theEvent->Clust().push_back(cluster);
        } else {
          theEvent->clusters().push_back(clust);
          ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),false,MuonStationIndex::TechnologyIndex::TGC,pIndex,tmatch,barcode);
          theEvent->Clust().push_back(cluster);
        }
      }
    } 	
  }
 
  void MuonClusterSegmentFinder::makeClusterVecs(const PRD_MultiTruthCollection* truthCollectionRPC, const std::vector<const RpcPrepDataCollection*>& rpcCols,candEvent* theEvent) const {
    for( std::vector<const RpcPrepDataCollection*>::const_iterator colIt = rpcCols.begin(); colIt != rpcCols.end(); ++ colIt ){
      RpcPrepDataCollection::const_iterator pit = (*colIt)->begin();
      for(; pit!=(*colIt)->end(); ++pit) {
        const MuonCluster* cl = *pit;
        if( !cl ) continue;
        bool tmatch = false;
        int barcode = 0;
        if(truthCollectionRPC){
          const Identifier& id = (*pit)->identify();
          tmatch = matchTruth(*truthCollectionRPC,id,barcode);
        }
        const MuonClusterOnTrack* clust = m_clusterCreator->createRIO_OnTrack( *cl, cl->globalPosition() );
        MuonStationIndex::PhiIndex pIndex = m_idHelper->phiIndex(clust->identify());
        if (m_idHelper->measuresPhi( clust->identify())){
          theEvent->clusters().push_back(clust);
          ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),true,MuonStationIndex::TechnologyIndex::RPC,pIndex,tmatch,barcode);
          theEvent->Clust().push_back(cluster);
        } else {
          theEvent->clusters().push_back(clust);
          ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),false,MuonStationIndex::TechnologyIndex::RPC,pIndex,tmatch,barcode);
          theEvent->Clust().push_back(cluster);
        }
      }
    }
  }
 
  void MuonClusterSegmentFinder::processSpacePoints(candEvent* theEvent, std::vector<std::vector<ClusterSeg::SpacePoint>>& sPoints) const { 
    bool truthSeed(true);
    int fakeCounter(0);
    unsigned int barcodeCounter(0);
    int barcodeVal(0);

    for(auto sit : sPoints){
      if (sit.size() < 2) continue;
      std::vector<const MuonClusterOnTrack*> vec1;
      std::vector<const Trk::MeasurementBase*> vec2;

      for(unsigned int i=0;i<sit.size();i++){
        int spEit = sit[i].eit();
        vec1.push_back(theEvent->clusters()[spEit]);
        vec2.push_back(theEvent->clusters()[spEit]);
        int spPit = sit[i].pit();
        vec1.push_back(theEvent->clusters()[spPit]);
        vec2.push_back(theEvent->clusters()[spPit]);
        if (!(sit[i].isMatch())) fakeCounter++;
        else if (barcodeCounter == 0) {
          barcodeVal = sit[i].barcode();
          barcodeCounter++;
        } else if (barcodeVal == sit[i].barcode()) barcodeCounter++;
      }
	  
      if (fakeCounter != 0) truthSeed = false;
      std::sort (vec2.begin(),vec2.end(), sortfunctionMB);
      Trk::TrackParameters* startpar = 0;
      Amg::Vector3D gp(sit.front().x(),sit.front().y(),sit.front().z());
      Amg::Vector3D gd(sit.back().x()-sit.front().x(),sit.back().y()-sit.front().y(),sit.back().z()-sit.front().z());
      Amg::Vector3D perpos = gp + -10*(gd.unit());
      if(perpos.dot(gd) < 0) gd = -1*gd;
      startpar = new Trk::Perigee(perpos, gd, 0, perpos);
      ATH_MSG_DEBUG("It is starting a fit with " << vec2.size() << "Measurement Base elements and " << startpar );
      Trk::Track* segtrack = fit(vec2,*startpar);
      delete startpar;
      if(segtrack) {
        MuonSegmentKey keyEntry = MuonSegmentKey(vec2);
        theEvent->keyVector().push_back(keyEntry);
        double chi2 = segtrack->fitQuality()->chiSquared();
        double dof = segtrack->fitQuality()->doubleNumberDoF();
        if( m_ntuple ){
          if(truthSeed) m_ntuple->fill(chi2/dof,ClusterSeg::FillType::chi2T);
          m_ntuple->fill(chi2/dof,ClusterSeg::FillType::chi2);
        }
        ATH_MSG_DEBUG( "the chi2 is " << chi2 << "the dof are " << dof << " and the chi2/dof is " << chi2/dof );
        theEvent->segTrkColl()->push_back(segtrack);
        theEvent->trackSeeds().push_back(std::make_pair(gp,gd));
        theEvent->hits().push_back(vec1);
      } else {
        ATH_MSG_DEBUG( "segment fit failed" );
      }	 
    }
  }
  
  void MuonClusterSegmentFinder::resolveCollections(std::map<int,bool> themap,candEvent* theEvent) const {
    for(unsigned int i=0;i<theEvent->keyVector().size();i++){
      if (themap[i] == true ){
        theEvent->resolvedTrackSeeds().push_back(std::make_pair(theEvent->trackSeeds()[i].first,theEvent->trackSeeds()[i].second));
        theEvent->resolvedhits().push_back(theEvent->hits()[i]);
        theEvent->resolvedTracks()->push_back(new Trk::Track(*(theEvent->segTrkColl()->at(i))));
      }
    }
    if(theEvent->keyVector().size() == 1) {
      theEvent->resolvedTrackSeeds().push_back(std::make_pair(theEvent->trackSeeds()[0].first,theEvent->trackSeeds()[0].second));
      theEvent->resolvedhits().push_back(theEvent->hits()[0]);
      theEvent->resolvedTracks()->push_back(new Trk::Track(*(theEvent->segTrkColl()->at(0))));
    }
    ATH_MSG_DEBUG("Resolved track candidates: old size " << theEvent->segTrkColl()->size() << " new size " << theEvent->resolvedTracks()->size() );
  }
 
  void MuonClusterSegmentFinder::getSegments(candEvent* theEvent, const Muon::MdtPrepDataContainer* mdtPrdCont, Trk::SegmentCollection* segColl) const {

    std::vector<const Muon::MuonSegment*> appendSegments; 
        
    const std::vector<const Muon::MuonClusterOnTrack*> MCOTs;
    for(unsigned int i=0;i<theEvent->resolvedTracks()->size();i++){

      const DataVector<const Trk::TrackParameters>* tpVec = theEvent->resolvedTracks()->at(i)->trackParameters();
      if( !tpVec || tpVec->empty() || !tpVec->front() ) continue;
      const Trk::TrackParameters& startPars = *tpVec->front();
      
      const std::vector<const MuonClusterOnTrack*>& MCOTs = const_cast<const std::vector<const MuonClusterOnTrack*>&>(theEvent->resolvedhits()[i]);
      if( MCOTs.empty() ) continue;

 
      const Identifier& id = MCOTs.front()->identify();
      MuonStationIndex::DetectorRegionIndex regionIndex = m_idHelper->regionIndex(id); 
      MuonStationIndex::LayerIndex layerIndex = m_idHelper->layerIndex(id);
      
      MuonLayerSurface::SurfacePtr surfacePtr(startPars.associatedSurface().clone());
      std::shared_ptr<const Trk::TrackParameters> parsPtr(startPars.clone());
      
      // get sectors and loop over them
      std::vector<int> sectors;
      theEvent->sectorMapping().getSectors(startPars.position().phi(),sectors);
      for( auto sector : sectors ){

        MuonLayerSurface layerSurface( surfacePtr, sector, regionIndex, layerIndex ) ;
        MuonSystemExtension::Intersection intersection( parsPtr, layerSurface);
        
        std::vector<const MdtPrepDataCollection*> mdtCols;
        if( !getLayerData( sector, regionIndex, layerIndex, mdtPrdCont, mdtCols ) ){
          ATH_MSG_DEBUG("Failed to get MDT PRD collections ");
          continue;
        }
        std::vector<const Muon::MdtDriftCircleOnTrack*> MDTs;
        for( auto mdtCol : mdtCols ){
          if( !m_muonPRDSelectionTool->calibrateAndSelectMdt( intersection, *mdtCol, MDTs ) ) {
            ATH_MSG_DEBUG("Failed to calibrate MDT PRD collection ");
            continue;
          }
        }
        ATH_MSG_DEBUG("Running mdt segment finding: MDTs " << MDTs.size() << " MCOTs " << MCOTs.size());
	m_segmentMaker->find(theEvent->resolvedTrackSeeds()[i].first,theEvent->resolvedTrackSeeds()[i].second,MDTs,MCOTs,false,segColl);
	ATH_MSG_DEBUG( "the size of the segment collection is " << segColl->size() );
	for(unsigned int j = 0;j< segColl->size();j++){
	  Trk::Segment* tseg=segColl->at(j);
	  ATH_MSG_DEBUG( "the " << j << "th segment contains " << (dynamic_cast<MuonSegment*>(tseg))->numberOfContainedROTs() << " ROTs " );
	}
      }
    }
  }

  bool MuonClusterSegmentFinder::getLayerData( int sector, MuonStationIndex::DetectorRegionIndex regionIndex,MuonStationIndex::LayerIndex layerIndex, 
					       const Muon::MdtPrepDataContainer* input, std::vector<const MdtPrepDataCollection*>& output ) const {

    // get technologies in the given layer
    unsigned int sectorLayerHash = MuonStationIndex::sectorLayerHash( regionIndex, layerIndex );

    // get hashes
    const MuonLayerHashProviderTool::HashVec& hashes = m_layerHashProvider->getHashes( sector, MuonStationIndex::TechnologyIndex::MDT, sectorLayerHash );

    // skip empty inputs
    if( hashes.empty() ) return true;

    // loop over hashes
    for( MuonLayerHashProviderTool::HashVec::const_iterator it=hashes.begin();it!=hashes.end();++it ){
      // skip if not found
      Muon::MdtPrepDataContainer::const_iterator colIt = input->indexFind(*it);
      if( colIt == input->end() ) {
	//ATH_MSG_WARNING("Cannot find hash " << *it << " in container at " << location);
	continue;
      }
      ATH_MSG_VERBOSE("  adding " << m_idHelper->toStringChamber((*colIt)->identify()) << " size " << (*colIt)->size());
      // else add
      output.push_back(*colIt);
    }
    return true;
  }
}
