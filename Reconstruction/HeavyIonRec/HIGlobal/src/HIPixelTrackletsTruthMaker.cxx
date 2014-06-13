/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIPixelTrackletsTruthMaker.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"
#include "HIGlobal/HIPixelTracklets.h"


#include <vector>
#include <map>

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "Identifier/Identifier.h"
#include "StoreGate/StoreGate.h"
#include "InDetIdentifier/PixelID.h"

HIPixelTrackletsTruthMaker::HIPixelTrackletsTruthMaker(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
  declareProperty("TruthNamePixel", m_PRDTruthNamePixel   = "PRD_MultiTruthPixel");
  declareProperty("pixelTrackletsCollectionName",m_pixelTrackletsCollectionName = "pixelTrackletsCollection");
  declareProperty("pixelTrackletsTruthName", m_pixelTrackletsTruthName = "pixelTrackletsTruth");
  pixelID=0;
  m_truthCollectionPixel=0;
  m_storeGate=0;
}

StatusCode HIPixelTrackletsTruthMaker::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode HIPixelTrackletsTruthMaker::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode HIPixelTrackletsTruthMaker::execute() {
  StatusCode sc;
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Detector store not found !");
    return sc;
  }
  sc=detStore->retrieve(pixelID,"PixelID");
  if( sc.isFailure()) {
    ATH_MSG_ERROR( "PixelID helper not found !");
    return StatusCode::FAILURE;
  }
  const HIPixelTrackletsCollection* tracklets=0;
  sc = evtStore()->retrieve(tracklets, m_pixelTrackletsCollectionName);
  if(sc.isFailure()){
    ATH_MSG_ERROR ("pixelTrackletsCollection named: "<<m_pixelTrackletsCollectionName<<" NOT found");
    return sc;
  }
  if(!m_PRDTruthNamePixel.empty()) {
    sc = evtStore()->retrieve(m_truthCollectionPixel,m_PRDTruthNamePixel);
    if(sc.isFailure()){
      ATH_MSG_WARNING ("Pixel PRD_MultiTruthCollection "<<m_PRDTruthNamePixel<<" NOT found");
      return sc;
    }
  }
  
  HIPixelTrackletsTruthCollection* pttc = new HIPixelTrackletsTruthCollection(tracklets);

  BuildTrackletsTruth(pttc,*tracklets);

  
  sc = evtStore()->record(pttc,m_pixelTrackletsTruthName, false);
  if(sc.isFailure()){
    ATH_MSG_ERROR ("PixelTrackletsTruthCollection '" << m_pixelTrackletsTruthName << "' could not be registered in StoreGate !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void HIPixelTrackletsTruthMaker::BuildTrackletsTruth(HIPixelTrackletsTruthCollection* pttc, const HIPixelTrackletsCollection &tracklets) {

  for(unsigned itrk=0; itrk<tracklets.size(); itrk++){
    //ElementLink<DataVector<HIPixelTracklets> > ptrklet(tracklets,itrk);
    ElementLink<HIPixelTrackletsCollection> ptrklet(tracklets,itrk);
    const HIPixelTracklets* hitrk = tracklets[itrk];
    const std::vector<PixelCluster*>* clusters = hitrk->GetClusters();
    std::vector<HepMcParticleLink> firstMCParts[3];
    //int innerLayer = 0;
    //int outLayer = 1;
    //ATH_MSG_INFO("size=" << clusters->size());
    for(unsigned icl=0; icl<clusters->size(); icl++) {
      //ATH_MSG_INFO("icl=" << icl);
      const PixelCluster& clus = *(*clusters)[icl];
      int layer = pixelID->layer_disk(clus.detectorElement()->identify()); //layer=0,1,2
      //ATH_MSG_INFO("layer="<<layer);
      //if(icl==0) innerLayer = layer;
      //if(icl==1) outLayer = layer; 
      Identifier id = clus.identify();
      typedef PRD_MultiTruthCollection::const_iterator iprdt;
      std::pair<iprdt,iprdt> range;
      range = m_truthCollectionPixel->equal_range(id);
      
      for(iprdt ip=range.first; ip!=range.second; ip++) {
	firstMCParts[layer].push_back( ip->second );
      }
    }
    bool haveGenPart = false;
    HepMcParticleLink* part=0;
    //ATH_MSG_INFO("innerLayer:outLayer=" << innerLayer << ":" << outLayer );
    //layer 0-1, 0-2 matching
    for(unsigned ip=0; ip<firstMCParts[0].size(); ip++) {
      for(unsigned ip1=0; ip1<firstMCParts[1].size(); ip1++) {
	if( firstMCParts[1][ip1] == firstMCParts[0][ip] ){
	  haveGenPart = true;
	  part = &firstMCParts[0][ip];
	  break;
	}
      } //layer 1
      if(haveGenPart) break;
      for(unsigned ip2=0; ip2<firstMCParts[2].size(); ip2++) {
	if( firstMCParts[2][ip2]==firstMCParts[0][ip] ) {
	  haveGenPart = true;
	  part = &firstMCParts[0][ip];
	  break;    
	}
      } //layer 2
      if( haveGenPart ) break;
    } //layer 0
    
    //layer 1-2 matching
    if( !haveGenPart ) {
      for(unsigned ip1=0; ip1<firstMCParts[1].size(); ip1++) {
	for(unsigned ip2=0; ip2<firstMCParts[2].size(); ip2++) {
	  if( firstMCParts[2][ip2]==firstMCParts[1][ip1] ) {
	    haveGenPart = true;
	    part = &firstMCParts[1][ip1];
	    break;
	  }
	} //layer 2
	if( haveGenPart ) break;
      } //layer 1
    } //not matched yet
    
    if( haveGenPart ) {
      pttc->insert(std::make_pair(ptrklet,*part));
    }
  }
  
  //   typedef HIPixelTrackletsTruthCollection::iterator truit;
  //   for(truit it=pttc->begin(); it!=pttc->end(); it++){
  //     ATH_MSG_INFO("tracklets: " << (it->first).index() );
  //     ATH_MSG_INFO("px " << ((it->second).cptr())->momentum().px() );
  //   }
  
}
