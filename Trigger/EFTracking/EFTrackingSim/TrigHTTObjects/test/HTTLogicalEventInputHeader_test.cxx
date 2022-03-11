/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTLogicalEventInputHeader_test.cxx: Unit tests for HTTLogicalEventInputHeader
 */

#include <cstdio>
#include <iostream>
#include <cassert>
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"

int main(int, char**)
{
  // some random values for checking things, just put them all up here
  const double eta(0.2), phi(0.5);
  const int evtnum(8675309);
  const int layer(3);
  const double qoverpt(-0.00002);
  const double d0(1.1), z0(2.2);
  const int pdgcode(-13), barcode(123);
  const double px(100.), py(-200.), pz(300.);


  HTTLogicalEventInputHeader header;
  HTTTowerInputHeader tower;
  tower.setEta(eta); 
  tower.setPhi(phi);
  header.addTower(tower);
  HTTTowerInputHeader *towerOut = header.getTower(0);
  std::cout << "Output tower eta = " << towerOut->eta() << " and phi = " << towerOut->phi() << std::endl;

  HTTEventInfo event;
  event.setRunNumber(evtnum);
  header.newEvent(event);
  std::cout << "Event number = " << evtnum << std::endl;

  HTTOptionalEventInfo optional;
  HTTCluster cluster;
  HTTHit hits[5];
  for (unsigned i = 0; i < 5; i++) {
    hits[i].setPhiIndex(i*i);
    hits[i].setEtaIndex(i+1);
    hits[i].setHitType(HitType::mapped);
    hits[i].setLayer(layer);
    cluster.push_backHitList(hits[i]);    
  };
  optional.addOfflineCluster(cluster);

  HTTOfflineTrack track;
  track.setQOverPt(qoverpt);
  track.setD0(d0);  
  track.setZ0(z0);  
  optional.addOfflineTrack(track);
  HTTTruthTrack truth;
  truth.setPDGCode(pdgcode);
  truth.setBarcode(barcode);
  truth.setPX(px);
  truth.setPY(py);
  truth.setPZ(pz);
  optional.addTruthTrack(truth);

  header.setOptional(optional);

  for (unsigned i = 0; i < 5; i++) {
    std::cout << "layer = " << (header.optional().getOfflineClusters()[0].getHitList())[i].getLayer() 
	      << " and phi = " << (header.optional().getOfflineClusters()[0].getHitList())[i].getPhiIndex() 
	      << " and eta = " << (header.optional().getOfflineClusters()[0].getHitList())[i].getEtaIndex() 
	      << std::endl;
  };

  std::cout << "q/pt = " << header.optional().getOfflineTracks()[0].getQOverPt() 
	    << " and d0 = " << header.optional().getOfflineTracks()[0].getD0()  
	    << " and z0 = " << header.optional().getOfflineTracks()[0].getZ0() << std::endl;


  std::cout << "Truth pdg = " << header.optional().getTruthTracks()[0].getPDGCode() 
	    << " and barcode = " << header.optional().getTruthTracks()[0].getBarcode() << std::endl;

  std::cout << "Truth px = " << header.optional().getTruthTracks()[0].getPX() 
	    << " and py = " <<  header.optional().getTruthTracks()[0].getPY() 
	    << " and pz = " <<  header.optional().getTruthTracks()[0].getPZ() 
	    << " and eta = " <<  header.optional().getTruthTracks()[0].getEta() 
	    << " and phi = " <<  header.optional().getTruthTracks()[0].getPhi() 
	    << std::endl;

  return 0;

}

