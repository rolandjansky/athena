/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// ClusterSegmentCombinationFinder.cxx (c) ATLAS Detector software
// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////

// AthExHistNtup includes
#include "ClusterSegmentCombinationFinder.h"
#include "MuonLinearSegmentMakerUtilities/ClusterAnalysis.h"

// ROOT includes
#include "TTree.h"
#include "TFile.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "HepMC/GenEvent.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSegment/SegmentCollection.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

 bool sortfunctionMB (const Trk::MeasurementBase* i, const Trk::MeasurementBase* j){return (fabs(i->globalPosition().z()) < fabs(j->globalPosition().z()));}
 bool sortfunctionSP (ClusterSeg::SpacePoint i, ClusterSeg::SpacePoint j){return (fabs(i.z()) < fabs(j.z()));}
 bool sortfunctionVec (std::pair<int,int> i, std::pair<int,int> j){return (i.second < j.second);}

// Constructors
////////////////
Muon::ClusterSegmentCombinationFinder::ClusterSegmentCombinationFinder( const std::string& t,
                           const std::string& n,
                           const IInterface*  p )
    : 
  AthAlgTool(t,n,p),
    m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool" ),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
    m_extrapolator("Trk::Extrapolator/MuonExtrapolator"),
    m_mdtCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"), 
    m_segmentMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
    m_segmentOverlapRemovalTool("Muon::MuonSegmentOverlapRemovalTool/MuonSegmentOverlapRemovalTool")
  {
    declareInterface<IMooSegmentCombinationFinder>(this);   
 
    declareProperty("SLFitter",            m_slTrackFitter);
    declareProperty("TrackToSegmentTool",  m_trackToSegmentTool);
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor);
    declareProperty("DoNtuple",m_doNtuple = true);
    declareProperty("TgcPrepDataContainerHits", m_keyTgcHits = "TGC_Measurements");
    declareProperty("TgcPrepDataContainer", m_keyTgc = "TGC_Clusters");
    declareProperty("IdHelper",            m_idHelperTool);
    declareProperty("DoTruth",m_doTruth = true);
    declareProperty("TrackCleaner",        m_trackCleaner);
    declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator);
    declareProperty( "MuonExtrapolator",    m_extrapolator);
    declareProperty("MdtRotCreator",                     m_mdtCreator);
    declareProperty("SegmentMaker",m_segmentMaker); 
    declareProperty("SegmentCollectionName",m_segmentCollectionName="tmpMuonSegments");
  }


// Destructor
///////////////
Muon::ClusterSegmentCombinationFinder::~ClusterSegmentCombinationFinder()
{}

// Initialize
////////////////////////////
StatusCode 
Muon::ClusterSegmentCombinationFinder::initialize()
{

    if( m_mdtCreator.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_mdtCreator );
      return StatusCode::FAILURE;
    }

    if( m_segmentMaker.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_segmentMaker );
      return StatusCode::FAILURE;
    }

    if( m_segmentOverlapRemovalTool.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_segmentOverlapRemovalTool );
      return StatusCode::FAILURE;
    }

    if( m_extrapolator.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_extrapolator );
      return StatusCode::FAILURE;
    }

    if( m_slTrackFitter.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_slTrackFitter );
      return StatusCode::FAILURE;
    }

    if( m_ambiguityProcessor.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Could not get " << m_ambiguityProcessor );
      return StatusCode::FAILURE;
    }

    if(m_trackToSegmentTool.retrieve().isFailure()) {
      ATH_MSG_FATAL ( "Could not retrieve " << m_trackToSegmentTool << ". Aborting ..." );
      return StatusCode::FAILURE;
    }

    if(m_residualPullCalculator.retrieve().isFailure()) {
      ATH_MSG_FATAL ( "Could not retrieve " << m_residualPullCalculator << ". Aborting ..." );
      return StatusCode::FAILURE;
    }

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

// Finalize
// ///////////////
StatusCode 
Muon::ClusterSegmentCombinationFinder::finalize()
{

    if( m_doNtuple ){
      TDirectory* cdir = gDirectory;
      m_file->cd();
      m_tree->Write();
      m_file->Write();
      m_file->Close();
      delete m_ntuple;
      gDirectory = cdir;
    }

    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
}

void Muon::ClusterSegmentCombinationFinder::getTruth() {
  std::string name = "TGC_TruthMap";
  if( !evtStore()->contains<PRD_MultiTruthCollection>(name) ) {
    ATH_MSG_WARNING( "PRD_MultiTruthCollection " << name << " NOT found");
    return;
  }
	
  if( evtStore()->retrieve(m_truthCollectionTGC, name).isFailure() ) {
    ATH_MSG_WARNING(  "PRD_MultiTruthCollection " << name << " NOT found");
    return;
  }
  ATH_MSG_DEBUG(  "PRD_MultiTruthCollection " << name << " found ");

  name = "MDT_TruthMap";
  if( !evtStore()->contains<PRD_MultiTruthCollection>(name) ) {
    ATH_MSG_WARNING( "PRD_MultiTruthCollection " << name << " NOT found");
    return;
  }

  if( evtStore()->retrieve(m_truthCollectionMDT, name).isFailure() ) {
    ATH_MSG_WARNING(  "PRD_MultiTruthCollection " << name << " NOT found");
    return;
  }
  ATH_MSG_DEBUG(  "PRD_MultiTruthCollection " << name << " found ");
 

  return; 
}

bool Muon::ClusterSegmentCombinationFinder::matchTruth(const PRD_MultiTruthCollection& truthCol, const Identifier& id,int& bcode) { 
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

Muon::IMooSegmentCombinationFinder::Output*
Muon::ClusterSegmentCombinationFinder::findSegments( const MdtPrepDataContainer* mdtCont,
                                                     const TgcPrepDataContainer* tgcCont )
{  
    ATH_MSG_DEBUG ("Executing " << name() << "...");

    if( m_doTruth ) getTruth();
    if( m_ntuple ) m_ntuple->reset();

    std::vector<const MuonClusterOnTrack*> clusters;
    std::vector<ClusterSeg::Cluster*> tgcClust;

    TgcPrepDataContainer::const_iterator it = tgcCont->begin();
    for (; it!=tgcCont->end(); ++it) {
      MuonPrepDataCollection<TgcPrepData>::const_iterator pit = (*it).getDataPtr()->begin();
      for(; pit!=(*it).getDataPtr()->end(); ++pit) {
        bool tmatch = false;
        int barcode = 0;
        if(m_doTruth){ 
          const Identifier& id = (*pit)->identify();
          tmatch = matchTruth(*m_truthCollectionTGC,id,barcode);
        }
        const MuonCluster* cl = dynamic_cast<const MuonCluster*>(*pit);
        if( !cl ) continue;
        const MuonClusterOnTrack* clust = m_clusterCreator->createRIO_OnTrack( *cl, cl->globalPosition() );
        std::string chamberId = m_idHelperTool->toStringChamber(clust->identify());
        if (m_idHelperTool->measuresPhi( clust->identify())){
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
        if(truthSeed) m_ntuple->fill(chi2/dof,ClusterSeg::FillType::chi2T);
        m_ntuple->fill(chi2/dof,ClusterSeg::FillType::chi2);
        ATH_MSG_DEBUG( "the chi2 is " << chi2 << "the dof are " << dof << " and the chi2/dof is " << chi2/dof );
        segTrkColl->push_back(segtrack);
        trackSeeds.push_back(std::make_pair(gp,gd));
        TGChits.push_back(vec1);
        truthVec.push_back(truthSeed);
        barcodeVec.push_back(trackBarcode);
      } else {
        ATH_MSG_INFO( "segment fit failed" );
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

    std::vector<const Muon::MdtDriftCircleOnTrack*> MDTs;
    const std::vector<const Muon::MuonClusterOnTrack*> TGCs; 
    for(unsigned int i=0;i<resolvedTracks->size();i++){
      bool goodTruthSeed = false;
      MdtPrepDataContainer::const_iterator mpit = mdtCont->begin();
      MdtPrepDataContainer::const_iterator mpit_end = mdtCont->end();
      for( ; mpit!=mpit_end; ++mpit ) {
        // skip empty collections
        if( (*mpit)->empty() ) continue;
        const DataVector<const Trk::TrackParameters>* tpVec = resolvedTracks->at(i)->trackParameters();
        double Z = (*(resolvedTracks->at(i)->measurementsOnTrack()))[0]->globalPosition().z(); 
        if (resolvedTruthVec[i] == true && resolvedBarcodeVec[i] == true) goodTruthSeed = true;
        if (!tpVec) ATH_MSG_WARNING("the trackParametersVec didn't work ");
        MuonPrepDataCollection<Muon::MdtPrepData>::const_iterator mit = (*mpit).getDataPtr()->begin();
        for(; mit!=(*mpit).getDataPtr()->end(); ++mit) {
          double mdtZ = (*mit)->globalPosition().z();
          if (TMath::Sign(1.,Z) != TMath::Sign(1.,mdtZ) || fabs(mdtZ) < 13400. || fabs(mdtZ) > 15000.) continue;
          bool tmatch(false);
          int barcode(0);
          if(m_doTruth){
            const Identifier& id = (*mit)->identify();
            if (goodTruthSeed) tmatch = matchTruth(*m_truthCollectionMDT,id,barcode); 
            else tmatch = false;
          }
          MdtDriftCircleOnTrack* mdt = const_cast<MdtDriftCircleOnTrack*>(m_mdtCreator->createRIO_OnTrack(**mit,(*mit)->globalPosition()));
          if (!mdt) continue;
          const Trk::Surface& msurf = mdt->associatedSurface();
          const Trk::TrackParameters* exPars = m_extrapolator->extrapolate(*(*tpVec)[0],msurf,Trk::PropDirection::anyDirection,false,Trk::ParticleHypothesis::muon);
          if (!exPars) {
            ATH_MSG_WARNING(" the extrapolator didn't work");
            continue;
          }
          if (exPars->parameters()[Trk::locR] < 0.) m_mdtCreator->updateSign(*mdt,Trk::LEFT);
          const Trk::MeasurementBase* mBase = dynamic_cast<const Trk::MeasurementBase*>(mdt);
          const Trk::ResidualPull* pullCalc = m_residualPullCalculator->residualPull(mBase,exPars,Trk::ResidualPull::ResidualType::HitOnly,Trk::TrackState::MeasurementType::MDT);
          if (pullCalc){
            std::vector<double> theRes = pullCalc->residual();
            if( theRes.size() > 0){
              if(m_doTruth && tmatch) m_ntuple->fill(TMath::Abs(theRes[0]),ClusterSeg::FillType::residualT);
              m_ntuple->fill(TMath::Abs(theRes[0]),ClusterSeg::FillType::residual);
              double resCut(15.);
              if(resolvedTGChits[i].size() < 5 ) resCut = 25.;
              if(TMath::Abs(theRes[0]) < resCut){
                const MdtDriftCircleOnTrack* mdt_const = const_cast<const MdtDriftCircleOnTrack*>(mdt);
                MDTs.push_back(mdt_const);
              }
            }
          } 
        }//loop over MDTPrepData 
      }//loop over MdtPrepDataContainer

      //now you need to separate for MDT's from different sectors
      std::vector<std::vector<const MdtDriftCircleOnTrack*>> sectorMDTs;
      //first check that there exists multiple sectors and sort
      bool multSectors(false);
      int currentSector(-1);
      std::vector<std::pair<int,int>> vecPairs;
      for (unsigned int y = 0;y<MDTs.size();y++){
        Identifier chId = m_idHelperTool->chamberId(MDTs[y]->identify());
        int sector = m_idHelperTool->sector(chId);
        if(y==0) currentSector = sector;
        if(currentSector != sector) multSectors = true;
        vecPairs.push_back(std::make_pair(y,sector));
      }
   
      if(multSectors){
        //now sort and make map of how many of each sector there are
        std::sort(vecPairs.begin(),vecPairs.end(),sortfunctionVec); 
        int indexCounter(0);
        std::map<int,int> sectorMap;
        for (unsigned int h=0;h<vecPairs.size();h++){
          Identifier chId = m_idHelperTool->chamberId(MDTs[vecPairs[h].first]->identify());
          int sector = m_idHelperTool->sector(chId);
          if (h == 0) currentSector = sector;
          if (currentSector == sector ) {
            indexCounter++;
          } else {
            sectorMap.insert( std::make_pair(currentSector,indexCounter));
            currentSector = sector;
            indexCounter = 1;
          }   
          if (h == vecPairs.size() -1) sectorMap.insert( std::make_pair(currentSector,indexCounter));
        }
        //now if there are fill multiple vectors of sectorMDTs
        std::vector<const MdtDriftCircleOnTrack*>::const_iterator it = MDTs.begin();
        int index(0);
        std::map<int,int>::iterator mapit = sectorMap.begin();
        for (;mapit!=sectorMap.end();mapit++){
          std::vector<const MdtDriftCircleOnTrack*> theMDT;
          int theSize = (*mapit).second+index;
          for(int k=index;k<theSize;k++){theMDT.push_back(MDTs[vecPairs[k].first]);}
          index += (*mapit).second; 
          sectorMDTs.push_back(theMDT);  
        }
      } else {
        sectorMDTs.push_back(MDTs);
      }   

      const std::vector<const MuonClusterOnTrack*>& TGCs = const_cast<const std::vector<const MuonClusterOnTrack*>&>(resolvedTGChits[i]);
      std::vector<std::vector<const MdtDriftCircleOnTrack*>>::iterator mdtit = sectorMDTs.begin(); 
      for (; mdtit!=sectorMDTs.end(); mdtit++){
        std::vector<const MuonSegment*>* segments = m_segmentMaker->find(resolvedTrackSeeds[i].first,resolvedTrackSeeds[i].second,*mdtit,TGCs);   
        if(segments && segments->size() > 0){
          appendSegments.insert(appendSegments.end(),segments->begin(),segments->end()); 
          ATH_MSG_DEBUG( "the size of the segment collection is " << segments->size() );
          for(unsigned int j = 0;j< segments->size();j++){ATH_MSG_DEBUG( "the " << j << "th segment contains " << (*segments)[j]->numberOfContainedROTs() << " ROTs " );}
        } else ATH_MSG_WARNING("finding of the muon segments failed");
      }
    }//resolved Track loop 

    std::vector<const MuonSegment*> resolvedSegments = m_segmentOverlapRemovalTool->removeDuplicates(appendSegments);
 
    Trk::SegmentCollection* segmentCollection = new Trk::SegmentCollection();
    segmentCollection->reserve(resolvedSegments.size());
    for( std::vector<const MuonSegment*>::iterator sit = resolvedSegments.begin(); sit!=resolvedSegments.end();++sit ){
      segmentCollection->push_back( const_cast<MuonSegment*>(*sit) );
    }
    ATH_MSG_DEBUG( " the size of the segmentCollection is " << segmentCollection->size() );

    IMooSegmentCombinationFinder::Output* output = new Muon::IMooSegmentCombinationFinder::Output();
    output->segmentCollection = segmentCollection;

    delete resolvedTracks;
    delete segTrkColl;

    if (m_doNtuple) {
      m_ntuple->fill(tgcClust);
      m_tree->Fill();
    }

    return output;
}

Amg::Vector3D Muon::ClusterSegmentCombinationFinder::intersectPlane( const Trk::PlaneSurface* surf, const Amg::Vector3D& pos,
                                                           const Amg::Vector3D& dir ) const {
  const Amg::Vector3D& planepostion = surf->center();
  const Amg::Vector3D& planenormal = surf->normal();
  double denom = dir.dot(planenormal);
  double u = planenormal.dot(planepostion - pos)/denom;
  return pos + u * dir;
}

Trk::Track* Muon::ClusterSegmentCombinationFinder::fit( const std::vector<const Trk::MeasurementBase*>& vec2, const Trk::TrackParameters& startpar ) const {
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
