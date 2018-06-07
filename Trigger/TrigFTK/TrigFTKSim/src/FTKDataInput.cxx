/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSplitEncoder.h"
#include "TrigFTKSim/atlClustering.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <fstream>
#include <cmath> // needed for floor()
using namespace std;

FTKDataInput::FTKDataInput() :
  m_nevent(-1), m_pmap(0x0), m_pmap_unused(0x0),
  m_reghits(0x0), m_run_number(0ul), m_event_number(0ul) ,
  m_original_reghits(0), m_nplanes(0), m_nregions(0), m_nsubregions(0),
  m_ncoords(0), m_region(-1), m_subregion(-1), 
  m_Clustering(true), m_SaveClusterContent(false),
  m_DiagClustering(true), m_SctClustering(false), m_PixelClusteringMode(0), m_Ibl3DRealistic(false), m_DuplicateGanged(true),
  m_GangedPatternRecognition(false),
  m_SplitBlayerModules(false),  
  m_init(false), m_save_unused(false), m_read_clusters(false),
  m_trackinput(0), m_roadinput(0), // cy road
  m_nao_nhits_tot(0), m_nao_nclus_tot(0), m_firstEventFTK(-1),
  m_goodRegions(0),
  m_regional(false)
{;}

FTKDataInput::FTKDataInput(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused) :
  m_nevent(-1), m_pmap(pmap), m_pmap_unused(pmap_unused),
  m_reghits(0x0), m_run_number(0ul), m_event_number(0ul) ,
  m_original_reghits(0),
  m_nplanes(m_pmap->getNPlanes()), m_nregions(0), m_nsubregions(0),
  m_ncoords(0), m_region(-1), m_subregion(-1), 
  m_Clustering(true), m_SaveClusterContent(false),
  m_DiagClustering(true), m_SctClustering(false), m_PixelClusteringMode(0), m_Ibl3DRealistic(false), m_DuplicateGanged(true),
  m_GangedPatternRecognition(false),
  m_SplitBlayerModules(false),  
  m_init(false), m_save_unused(false), m_read_clusters(false),
  m_trackinput(0), m_roadinput(0), // cy road
  m_nao_nhits_tot(0), m_nao_nclus_tot(0), m_firstEventFTK(-1),
  m_goodRegions(0),
  m_regional(false)
{;}

FTKDataInput::~FTKDataInput()
{;}


/** set the plane map for used and unused layers, for the unused layers a null
    value is accepted */
void FTKDataInput::setPlaneMaps(const FTKPlaneMap *pmap, const FTKPlaneMap *pmap_unused)
{
  m_pmap = pmap;
  m_nplanes = pmap->getNPlanes();
  m_pmap_unused = pmap_unused;
}


/** initilize clustering settings (bad: using global variables!) */
void FTKDataInput::initClustering() 
{
  SAVE_CLUSTER_CONTENT = m_SaveClusterContent;
  DIAG_CLUSTERING = m_DiagClustering;
  SCT_CLUSTERING = m_SctClustering;
  PIXEL_CLUSTERING_MODE = m_PixelClusteringMode;
  IBL3D_REALISTIC = m_Ibl3DRealistic;
  DUPLICATE_GANGED = m_DuplicateGanged;
  GANGED_PATTERN_RECOGNITION = m_GangedPatternRecognition;
  SPLIT_BLAYER_MODULES = m_SplitBlayerModules;
  //
  //  new clustering class and pass options

}

/** Process the event:cluster raw hits and convert them to FTKHits */
void FTKDataInput::processEvent(bool clearOrig)
{
  // reset statistics
  m_nao_nhits_tot=0;
  m_nao_nclus_tot=0;

  // reset hit list
  m_hits.clear();
  m_hits_unused.clear();

  if (m_regional) {
    // process only the important regions and exit
    for (int ireg=0;ireg!=m_nregions;++ireg) {
      if (m_goodRegions[ireg]) processRegion(ireg, clearOrig);
    }

    // exit
    return;
  }


  if(FTKSetup::getDBG()) {
    cout << "DBG: Original hits = " << m_original_hits.size() << endl;
  }
  m_nao_nhits_tot = m_original_hits.size();

  if (!m_read_clusters) {
      // if raw channels are read the clustering algorithm can be applyied (default)
    if (m_Clustering ) {
      atlClusteringLNF(m_original_hits);
      atlClusteringBlayer(m_original_hits);  // split blayer modules in half, if requested
    }
    else {
      // If the clustering it is not executed the FTKRawHit.m_truth field has to be fixed
      vector<FTKRawHit>::iterator ihit = m_original_hits.begin();
      vector<FTKRawHit>::const_iterator ihit_end = m_original_hits.end();
      for (;ihit!=ihit_end;++ihit) { // loop over the raw hits
	FTKRawHit &curhit = *ihit;
	MultiTruth mt;
	MultiTruth::Barcode uniquecode(curhit.getEventIndex(),curhit.getBarcode());
	mt.maximize(uniquecode,curhit.getBarcodePt());
	curhit.setTruth(mt);
      } // end hit
    }
  }

  if(FTKSetup::getDBG()) {
    cout << "DBG: Clustered hits = " << m_original_hits.size() << endl;
  }
  m_nao_nclus_tot = m_original_hits.size();

  // read SCTtrk values (Constantinos case) and push SCTtrk hits into hits vector:
  // Note that it assumes the raw data input and root track input are fully in sync!
  if(m_trackinput && m_roadinput) {
    for(int i=0;i<m_nregions;i++) {
      // for now, only read tracks from "current" region
      if (i!=m_region) continue;
      int res = m_trackinput[i]->nextEvent();
      if(res<0) {
	FTKSetup::PrintMessage(ftk::sevr,"SCTtrk input ran out before hit input");
      }
      res = m_roadinput[i]->nextEvent();
      if(res<0) {
	FTKSetup::PrintMessage(ftk::sevr,"SCTtrk road input ran out before hit input");
      }
      const FTKTrack *cur_track = m_trackinput[i]->nextTrack(m_region);
      while (cur_track) {
	FTKRawHit tmphit(cur_track,m_pmap->getSCTtrkPlane());
	m_original_hits.push_back(tmphit);
	cur_track = m_trackinput[i]->nextTrack(m_region);
      }
    }
  }

  // filter the hit end convert input hit format into FTKHit
  vector<FTKRawHit>::iterator irawhit = m_original_hits.begin();
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  
  // create one list for each plane to order the hits
  list<FTKHit> *planelist = new list<FTKHit>[m_nplanes];

  for (;irawhit!=m_original_hits.end();++irawhit) { // loop over clusters
    FTKRawHit &rawhit = *irawhit;

    // check if end-cap hits should be accepted
    if (rawhit.getBarrelEC()!=0 && ftkset.getBarrelOnly() == true) {
      // skip this hit
      continue;
    }
    // check if SCT hits should be accepted
    if(rawhit.getIsSCT() && ftkset.getSCTtrkMode()) {
      // skip this hit
      continue;
    }
    if (m_pmap->getMap(rawhit.getHitType(),rawhit.getBarrelEC()!=0,rawhit.getLayer()).getPlane() != -1) {
      // accept this hit
      FTKHit tmphit = rawhit.getFTKHit(m_pmap);
      assert(rawhit.getTruth());
      tmphit.setTruth(*(rawhit.getTruth())); // AB - attach truth data to the FTKHit.
      planelist[tmphit.getPlane()].push_front(tmphit);
    }
    else if (m_save_unused) {
      // accept this hit
      FTKHit tmphit = rawhit.getFTKHit(m_pmap_unused);
      assert(rawhit.getTruth());
      tmphit.setTruth(*(rawhit.getTruth())); // AB - attach truth data to the FTKHit.
      m_hits_unused.push_back(tmphit);
    }
  } // end loop over clusters

  for (int ipl=0;ipl<m_nplanes;++ipl) { // loop over the planes
    list<FTKHit>::iterator tmphit = planelist[ipl].begin();
    for (;tmphit!=planelist[ipl].end();++tmphit) {
      m_hits.push_back(*tmphit);
    }
  } // end loop over the planes

  // clear raw hits since we don't need them anymore
  if(clearOrig)
    m_original_hits.clear();

  delete [] planelist;
}


void FTKDataInput::processRegion(int curreg, bool clearOrig)
{
  
  if (m_read_FTKhits_directly) {
      vector<FTKHit> &curhits = m_reghits[curreg];
      vector<FTKHit*> &curhits_read = *(m_reghits_read[curreg]);
      curhits.clear();
      vector<FTKHit*>::iterator ihit = curhits_read.begin();
      for(; ihit!=curhits_read.end(); ++ihit){
          FTKHit tmphit = **ihit;
          curhits.push_back(tmphit);
      }
      
  }
  else {
     // set the reference the list used by the current region
     vector<FTKRawHit> &currawhits = *(m_original_reghits[curreg]);
     vector<FTKHit> &curhits = m_reghits[curreg];

     // reset hit list
     curhits.clear();


     if(FTKSetup::getDBG()) {
       cout << "DBG: Original hits = " << currawhits.size() << endl;
     }
     m_nao_nhits_tot += currawhits.size();

     if (!m_read_clusters) {
         // if raw channels are read the clustering algorithm can be applyied (default)
       if (m_Clustering ) {
         atlClusteringLNF(currawhits);
         atlClusteringBlayer(currawhits);  // split blayer modules in half, if requested
       }
       else {
         // If the clustering it is not executed the FTKRawHit.m_truth field has to be fixed
         vector<FTKRawHit>::iterator ihit = currawhits.begin();
         vector<FTKRawHit>::const_iterator ihit_end = currawhits.end();
         for (;ihit!=ihit_end;++ihit) { // loop over the raw hits
       FTKRawHit &curhit = *ihit;
       MultiTruth mt;
       MultiTruth::Barcode uniquecode(curhit.getEventIndex(),curhit.getBarcode());
       mt.maximize(uniquecode,curhit.getBarcodePt());
      cout << "actually I'm here ! " << endl; 
       curhit.setTruth(mt);
         } // end hit
       }
     }

     if(FTKSetup::getDBG()) {
       cout << "DBG: Clustered hits = " << m_original_hits.size() << endl;
     }
     m_nao_nclus_tot += currawhits.size();

     // read SCTtrk values (Constantinos case) and push SCTtrk hits into hits vector:
     // Note that it assumes the raw data input and root track input are fully in sync!
     if(m_trackinput && m_roadinput) {
       // for now, only read tracks from "current" region
       int res = m_trackinput[curreg]->nextEvent();
       if(res<0) {
         FTKSetup::PrintMessage(ftk::sevr,"SCTtrk input ran out before hit input");
       }
       res = m_roadinput[curreg]->nextEvent();
       if(res<0) {
         FTKSetup::PrintMessage(ftk::sevr,"SCTtrk road input ran out before hit input");
       }
       const FTKTrack *cur_track = m_trackinput[curreg]->nextTrack(m_region);
       while (cur_track) {
         FTKRawHit tmphit(cur_track,m_pmap->getSCTtrkPlane());
         m_original_hits.push_back(tmphit);
         cur_track = m_trackinput[curreg]->nextTrack(m_region);
       }
     }

     // filter the hit end convert input hit format into FTKHit
     vector<FTKRawHit>::iterator irawhit = currawhits.begin();
     FTKSetup &ftkset = FTKSetup::getFTKSetup();
     
     // create one list for each plane to order the hits
     list<FTKHit> *planelist = new list<FTKHit>[m_nplanes];

     for (;irawhit!=currawhits.end();++irawhit) { // loop over clusters
       FTKRawHit &rawhit = *irawhit;

       // check if end-cap hits should be accepted
       if (rawhit.getBarrelEC()!=0 && ftkset.getBarrelOnly() == true) {
         // skip this hit
         continue;
       }
       // check if SCT hits should be accepted
       if(rawhit.getIsSCT() && ftkset.getSCTtrkMode()) {
         // skip this hit
         continue;
       }
       if (m_pmap->getMap(rawhit.getHitType(),rawhit.getBarrelEC()!=0,rawhit.getLayer()).getPlane() != -1) {
         // accept this hit
         FTKHit tmphit = rawhit.getFTKHit(m_pmap);
         if(rawhit.getTruth()!=nullptr)
           tmphit.setTruth(*(rawhit.getTruth())); // AB - attach truth data to the FTKHit.
         planelist[tmphit.getPlane()].push_front(tmphit);
       }
       else if (m_save_unused) {
         // accept this hit
         FTKHit tmphit = rawhit.getFTKHit(m_pmap_unused);
         if(rawhit.getTruth()!=nullptr)
           tmphit.setTruth(*(rawhit.getTruth())); // AB - attach truth data to the FTKHit.
         m_hits_unused.push_back(tmphit);
       }
     } // end loop over clusters

     for (int ipl=0;ipl<m_nplanes;++ipl) { // loop over the planes
       list<FTKHit>::iterator tmphit = planelist[ipl].begin();
       for (;tmphit!=planelist[ipl].end();++tmphit) {
         curhits.push_back(*tmphit);
       }
     } // end loop over the planes

     // clear raw hits since we don't need them anymore
     if(clearOrig)
       currawhits.clear();

     delete [] planelist;
  }
}
