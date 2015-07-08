/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonClusterSegmentFinder.h"
#include "MuonPrepRawDataProviderTools/MuonPrepRawDataCollectionProviderTool.h"
#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"

// ROOT includes
#include "TTree.h"
#include "TFile.h"

// AthExHistNtup includes
#include "MuonLinearSegmentMakerUtilities/ClusterAnalysis.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonLayerEvent/MuonLayerSurface.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
     
// EDM
#include "MuonSegment/MuonSegment.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "HepMC/GenEvent.h"

// helpers
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"

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
     
namespace Muon {

 MuonClusterSegmentFinder::MuonClusterSegmentFinder(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_muonPrepRawDataCollectionProviderTool("Muon::MuonPrepRawDataCollectionProviderTool/MuonPrepRawDataCollectionProviderTool"),
    m_muonPRDSelectionTool("Muon::MuonPRDSelectionTool/MuonPRDSelectionTool"),
    m_segmentMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
    m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_segmentOverlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool")
 {
    declareInterface<IMuonClusterSegmentFinder>(this);

    declareProperty("MuonIdHelperTool",m_idHelper );    
    declareProperty("MuonEDMPrinterTool",m_printer );    
    declareProperty("MuonPrepRawDataCollectionProviderTool",m_muonPrepRawDataCollectionProviderTool );    
    declareProperty("MuonPRDSelectionTool", m_muonPRDSelectionTool );
    declareProperty("MdtSegmentMaker",m_segmentMaker);
    declareProperty("SLFitter",            m_slTrackFitter);
    declareProperty("TrackToSegmentTool",  m_trackToSegmentTool);
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor);
    declareProperty("DoTruth",m_doTruth = true);
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
    ATH_CHECK(m_muonPrepRawDataCollectionProviderTool.retrieve());
    ATH_CHECK(m_muonPRDSelectionTool.retrieve());
    ATH_CHECK(m_segmentMaker.retrieve());
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
  bool sortfunctionSP (ClusterSeg::SpacePoint i, ClusterSeg::SpacePoint j){return (fabs(i.z()) < fabs(j.z()));}
  bool sortfunctionVec (std::pair<int,int> i, std::pair<int,int> j){return (i.second < j.second);}

  const PRD_MultiTruthCollection* MuonClusterSegmentFinder::getTruth(std::string name) const {
    if( !evtStore()->contains<PRD_MultiTruthCollection>(name) ) {
      ATH_MSG_WARNING( "PRD_MultiTruthCollection " << name << " NOT found");
      return 0;
    }

    const PRD_MultiTruthCollection* truthCollection = 0;      
    if( evtStore()->retrieve(truthCollection, name).isFailure() ) {
      ATH_MSG_WARNING(  "PRD_MultiTruthCollection " << name << " NOT found");
      return 0;
    }
    return truthCollection;
  }

  bool MuonClusterSegmentFinder::matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id,int& bcode) const {
    typedef PRD_MultiTruthCollection::const_iterator iprdt;
    std::pair<iprdt, iprdt> range = truthCol.equal_range(id);
 
    // Loop over particles contributing to this cluster
    for(iprdt i = range.first; i != range.second; i++) {
      if(!i->second.isValid()) {
        ATH_MSG_INFO( "Unexpected invalid HepMcParticleLink in PRD_MultiTruthCollection" );
      } else {
        const HepMcParticleLink& link = i->second;
        if( link.cptr() && abs(link.cptr()->pdg_id()) == 13 ){
          bcode = link.barcode();
          return true;
        }
      }
    }
    return false;
  }

  void MuonClusterSegmentFinder::findSegments( const std::vector<const TgcPrepDataCollection*>& tgcCols, std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments ) const {
    ATH_MSG_DEBUG ("Executing " << name() << "...");
	
    std::vector<const MuonClusterOnTrack*> clusters;
    std::vector<ClusterSeg::Cluster*> tgcClust;

    const PRD_MultiTruthCollection* truthCollectionTGC = m_doTruth ? getTruth("TGC_TruthMap") : 0;
    
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
        std::string chamberId = m_idHelper->toStringChamber(clust->identify()); /// fix me, NEVER use strings in algoritmic code!!!!
        if (m_idHelper->measuresPhi( clust->identify())){
          clusters.push_back(clust);
          ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),true,tmatch,barcode,chamberId);
          tgcClust.push_back(cluster);
        } else {
          clusters.push_back(clust);
          ClusterSeg::Cluster* cluster = new ClusterSeg::Cluster(clust->globalPosition().x(),clust->globalPosition().y(),clust->globalPosition().z(),false,tmatch,barcode,chamberId);
          tgcClust.push_back(cluster);
        }
      }
    }  
	
    TrackCollection* segTrkColl = new TrackCollection;
	
    ClusterSeg::ClusterAnalysis theAnalysis;
    std::vector<std::vector<ClusterSeg::SpacePoint>> sPoints = theAnalysis.analyse(tgcClust);
    ATH_MSG_DEBUG(" there were " << sPoints.size() << " seeds loaded ");
	
    std::vector<std::vector<const MuonClusterOnTrack*>> TGChits;
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D>> trackSeeds;
    std::vector<std::pair<Amg::Vector3D,Amg::Vector3D>> resolvedTrackSeeds;
    std::vector<bool> truthVec;
    std::vector<bool> resolvedTruthVec;
    std::vector<bool> barcodeVec;
    std::vector<bool> resolvedBarcodeVec;
    std::vector<MuonSegmentKey> keyVector;
    bool truthSeed(true);
    int fakeCounter(0);
    unsigned int barcodeCounter(0);
    int barcodeVal(0);
    bool trackBarcode(false);
    for(auto sit:sPoints){
      if (sit.size() < 2) continue;
      ATH_MSG_DEBUG(" starting a new seed of size " << sit.size());
      std::vector<const MuonClusterOnTrack*> vec1;
      std::vector<const Trk::MeasurementBase*> vec2;
      ATH_MSG_DEBUG("Clusters has size " << clusters.size());
      for(unsigned int i=0;i<sit.size();i++){
        int spEit = sit[i].eit();
        ATH_MSG_DEBUG(" the space point index is ");
        vec1.push_back(clusters[spEit]);
        vec2.push_back(clusters[spEit]);
        int spPit = sit[i].pit();
        vec1.push_back(clusters[spPit]);
        vec2.push_back(clusters[spPit]);
        if (!(sit[i].isMatch())) fakeCounter++;
        else if (barcodeCounter == 0) {
          barcodeVal = sit[i].barcode();
          barcodeCounter++;
        } else if (barcodeVal == sit[i].barcode()) barcodeCounter++;
      }
      if (barcodeCounter == sit.size()) trackBarcode = true;
      if (fakeCounter != 0) truthSeed = false;
      std::sort (sit.begin(),sit.end(), sortfunctionSP);
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
        keyVector.push_back(keyEntry);
        double chi2 = segtrack->fitQuality()->chiSquared();
        double dof = segtrack->fitQuality()->doubleNumberDoF();
        if( m_ntuple ){
          if(truthSeed) m_ntuple->fill(chi2/dof,ClusterSeg::FillType::chi2T);
          m_ntuple->fill(chi2/dof,ClusterSeg::FillType::chi2);
        }
        ATH_MSG_DEBUG( "the chi2 is " << chi2 << "the dof are " << dof << " and the chi2/dof is " << chi2/dof );
        segTrkColl->push_back(segtrack);
        trackSeeds.push_back(std::make_pair(gp,gd));
        TGChits.push_back(vec1);
        truthVec.push_back(truthSeed);
        barcodeVec.push_back(trackBarcode);
      } else {
        ATH_MSG_DEBUG( "segment fit failed" );
      }
	 
    }//end sPoints iteration
    if(segTrkColl->size() > 0) ATH_MSG_DEBUG( "it made at least one track " );
	
    std::map<int,bool> themap;
    CompareMuonSegmentKeys compareKeys;
    if(keyVector.size() > 1){
      for (unsigned int i=0;i<keyVector.size();i++){themap.insert(std::pair<int,bool>(i,true));}
      for(unsigned int i=0;i<keyVector.size()-1;i++){
        for(unsigned int j=i+1;j<keyVector.size();j++){
          CompareMuonSegmentKeys::OverlapResult overlap = compareKeys(keyVector[i],keyVector[j]);
          if(overlap == 0 || overlap == 2) themap[j] = false;
          if(overlap == 1) themap[i] = false;
        }
      }
    }
	
    TrackCollection* resolvedTracks = new TrackCollection;
    std::vector<std::vector<const MuonClusterOnTrack*>> resolvedTGChits;
    for(unsigned int i=0;i<keyVector.size();i++){
      if (themap[i] == true ){
        resolvedTrackSeeds.push_back(std::make_pair(trackSeeds[i].first,trackSeeds[i].second));
        resolvedTGChits.push_back(TGChits[i]);
        resolvedTracks->push_back(new Trk::Track(*(segTrkColl->at(i))));
        resolvedBarcodeVec.push_back(barcodeVec[i]);
        resolvedTruthVec.push_back(truthVec[i]);
      }
    }
    if(keyVector.size() == 1) {
      resolvedTrackSeeds.push_back(std::make_pair(trackSeeds[0].first,trackSeeds[0].second));
      resolvedTGChits.push_back(TGChits[0]);
      resolvedTracks->push_back(new Trk::Track(*(segTrkColl->at(0))));
      resolvedBarcodeVec.push_back(barcodeVec[0]);
      resolvedTruthVec.push_back(truthVec[0]);
    }
    ATH_MSG_DEBUG("Resolved track candidates: old size " << segTrkColl->size() << " new size " << resolvedTracks->size() );

	
    //Now Associate MDT's
    std::vector<const Muon::MuonSegment*> appendSegments; 
    
    // sector mapping 
    MuonSectorMapping sectorMapping;

	
    const std::vector<const Muon::MuonClusterOnTrack*> TGCs;
    for(unsigned int i=0;i<resolvedTracks->size();i++){
      bool goodTruthSeed = false;

      // get start parameters from TGC segment 
      const DataVector<const Trk::TrackParameters>* tpVec = resolvedTracks->at(i)->trackParameters();
      if( !tpVec || tpVec->empty() || !tpVec->front() ) continue;
      const Trk::TrackParameters& startPars = *tpVec->front();
      
      const std::vector<const MuonClusterOnTrack*>& TGCs = const_cast<const std::vector<const MuonClusterOnTrack*>&>(resolvedTGChits[i]);
      if( TGCs.empty() ) continue;

      // get needed Identifier information
      const Identifier& id = TGCs.front()->identify();
      MuonStationIndex::DetectorRegionIndex regionIndex = m_idHelper->regionIndex(id); 
      MuonStationIndex::LayerIndex layerIndex = m_idHelper->layerIndex(id);
      
      MuonLayerSurface::SurfacePtr surfacePtr(startPars.associatedSurface().clone());
      std::shared_ptr<const Trk::TrackParameters> parsPtr(startPars.clone());
      
      // get sectors and loop over them
      std::vector<int> sectors;
      sectorMapping.getSectors(startPars.position().phi(),sectors);
      for( auto sector : sectors ){

        MuonLayerSurface layerSurface( surfacePtr, sector, regionIndex, layerIndex ) ;
        MuonSystemExtension::Intersection intersection( parsPtr, layerSurface);
        
        std::vector<const MdtPrepDataCollection*> mdtCols;
        if( !m_muonPrepRawDataCollectionProviderTool->getLayerData( sector, MuonStationIndex::MDT, regionIndex, layerIndex, mdtCols ) ){
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
        ATH_MSG_DEBUG("Running mdt segment finding: MDTs " << MDTs.size() << " TGCs " << TGCs.size());
        std::vector<const MuonSegment*>* segments = m_segmentMaker->find(resolvedTrackSeeds[i].first,resolvedTrackSeeds[i].second,MDTs,TGCs);   
        if(segments && segments->size() > 0){
          appendSegments.insert(appendSegments.end(),segments->begin(),segments->end());
          ATH_MSG_DEBUG( "the size of the segment collection is " << segments->size() );
          for(unsigned int j = 0;j< segments->size();j++){ATH_MSG_DEBUG( "the " << j << "th segment contains " << (*segments)[j]->numberOfContainedROTs() << " ROTs " );}
        } else ATH_MSG_DEBUG("finding of the muon segments failed");
      }//resolved Track loop
    }
    std::vector<const MuonSegment*> resolvedSegments = m_segmentOverlapRemovalTool->removeDuplicates(appendSegments);
    
    // add to output
    for( auto seg : resolvedSegments ){
      segments.push_back( std::shared_ptr<const MuonSegment>(seg) );
    }
	
    delete resolvedTracks;
    delete segTrkColl;
	
    if (m_doNtuple) {
      m_ntuple->fill(tgcClust);
      m_tree->Fill();
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
          msg(MSG::DEBUG) << endreq;
        }
        delete segtrack;
        segtrack = 0;
      }
    }
    return segtrack;
  }


}
