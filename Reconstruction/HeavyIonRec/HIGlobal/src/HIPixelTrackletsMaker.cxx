/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIGlobal/HIPixelTrackletsMaker.h"
#include "HIGlobal/HIPixelTracklets.h"

#include<iostream>
#include<cmath>
#include<vector>
#include<map>
#include<set>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "StoreGate/StoreGate.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "VxVertex/VxContainer.h"
#include "CLHEP/Geometry/Point3D.h"
#include "InDetIdentifier/PixelID.h"
#include "GaudiKernel/AlgFactory.h"
#include "TMath.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "HepMC/GenParticle.h"

HIPixelTrackletsMaker::HIPixelTrackletsMaker(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator) {
  declareProperty("PixelClusterContainerName",
                  m_pixelClustersName="PixelClusters");
  declareProperty("minEta",m_minEta=-3.0);
  declareProperty("maxEta",m_maxEta=3.0);
  declareProperty("minPhi",m_minPhi=-3.1416);
  declareProperty("maxPhi",m_maxPhi=3.1416);
  declareProperty("nBinsEta",m_nBinsEta=50);
  declareProperty("nBinsPhi",m_nBinsPhi=40);
  declareProperty("dEta",m_dEta=0.01);
  declareProperty("dPhi",m_dPhi=0.16);
  declareProperty("doLayerSum3",m_doLayerSum3=false);
  declareProperty("trackletsMethod",m_trackletsMethod=1); //method=1, best match; method 2, all combination
  declareProperty("backgroundFlip",m_backgroundFlip=0); //method=2 and flip=1, flip layer 1
  declareProperty("doMC",m_doMC=false);
  declareProperty("doMagOff",m_doMagOff=false);
  
  m_pixelID=0;
  m_vx=0;
  m_binSizePhi=0;
  m_binSizeEta=0;
  m_vy=0;
  m_vz=0;

  m_sigmaDetaEtaFunc = 0;
  m_sigmaDphiEtaFunc = 0;
}

StatusCode HIPixelTrackletsMaker::initialize() {
  m_binSizeEta = (m_maxEta-m_minEta)/m_nBinsEta;
  m_binSizePhi = (m_maxPhi-m_minPhi)/m_nBinsPhi;
  
  if( m_binSizeEta < m_dEta ) { 
    ATH_MSG_DEBUG( "2D histogram bin too fine in eta direction. Reset to default." );
    m_nBinsEta=50;
    m_binSizeEta = (m_maxEta-m_minEta)/m_nBinsEta;
  }
  if( m_binSizePhi < m_dPhi*(122.5-50.5)/(88.5-50.5) ) {
    ATH_MSG_DEBUG( "2D histogram bin too fine in phi direction. Reset to default." );
    m_nBinsPhi=40;
    m_binSizePhi = (m_maxPhi-m_minPhi)/m_nBinsPhi;
  }
  
  ATH_MSG_DEBUG( "In initialize()" );
  ATH_CHECK( detStore()->retrieve(m_pixelID,"PixelID") );
  
  setEdgePosition();
  //setup the sigma dphi value in magoff samples
  m_sigmaDetaEtaFunc = new TF1("sigmaDetaEtaFunc","[0]+[1]*TMath::Abs(x)+[2]*x*x+[3]*x*x*TMath::Abs(x)",-2,2);
  m_sigmaDetaEtaFunc->SetParameters(0.007515,-0.000376,0.000216,0.000796);
  m_sigmaDphiEtaFunc = new TF1("sigmaDphiEtaFunc","[0]+[1]*cosh(x)",-2,2);
  m_sigmaDphiEtaFunc->SetParameters(0.0064,0.0007);

  return StatusCode::SUCCESS;
}

StatusCode HIPixelTrackletsMaker::finalize() {
  ATH_MSG_DEBUG( "In finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode HIPixelTrackletsMaker::execute() {
  const DataHandle<PixelClusterContainer> clusters;
  ATH_CHECK( evtStore()->retrieve(clusters, m_pixelClustersName) );
  
  const VxContainer *vxc = nullptr;
  ATH_CHECK( evtStore()->retrieve( vxc, "VxPrimaryCandidate" ) );
  
  //excluding dummy vertex
  VxContainer::const_iterator vxb = vxc->begin();
  VxContainer::const_iterator vxe = vxc->end();
  --vxe;
  //no reconstructed vertex, just one dummy vertex
  HIPixelTrackletsCollection* trkcoll = new HIPixelTrackletsCollection();
  if( vxb==vxe ) {
    
    //right now conflict with PixelTrackeltsMaker program, turn them on later
    ATH_CHECK( evtStore()->record(trkcoll,"pixelTrackletsCollection") );
    return StatusCode::SUCCESS;
  }
  
  m_vx = (*vxc)[0]->recVertex().position().x();
  m_vy = (*vxc)[0]->recVertex().position().y();
  m_vz = (*vxc)[0]->recVertex().position().z();
  
 
  setEdgePhi();
  
  clearVectors();
  layerClusters(clusters);
  
  removeGangedNoise(0,22,m_gangedClusIndexLayer0);
  removeGangedNoise(1,38,m_gangedClusIndexLayer1);
  removeGangedNoise(2,52,m_gangedClusIndexLayer2);
  removeOverLapCluster(0,22);
  removeOverLapCluster(1,38);
  removeOverLapCluster(2,52);
    
  //  const PRD_MultiTruthCollection* prdlist=0;
//   if( detStore->retrieve(prdlist, "PRD_MultiTruthPixel").isFailure() ){
//     log << MSG::DEBUG << "PixClusterFillerTool::AssociateWithTruth could not retrieve pixel PRD list";
//   } else {
//     for(mapiiitr itr=m_gangedNoiseLayer[0].begin(); itr!=m_gangedNoiseLayer[0].end(); itr++) {
//       if( itr->second ) {
// 	int index = itr->first;
// 	Identifier id = m_clusterLayer[0][index].identify();
// 	std::vector<int> m_mc_barcode;
// 	typedef std::multimap<Identifier, HepMcParticleLink>::const_iterator idlink;
// 	std::pair<idlink, idlink> links = prdlist->equal_range(id);
// 	for(idlink thelink=links.first; thelink!=links.second; ++thelink){
// 	  if(!thelink->second.isValid() ){
// 	    m_mc_barcode.push_back(0);
// 	  }else{
// 	    m_mc_barcode.push_back(thelink->second->barcode());
// 	  }
// 	}
// 	log << MSG::DEBUG << "Index=" << index << ", Barcode size=" << m_mc_barcode.size() << endreq;
//       }
//     }
//   }
  
  mapClusters();
  
  std::map<int, std::vector<HIPixelTracklets*> > trkletsMapLayer[3];
  makeTracklets(0,1,trkletsMapLayer[0]);
  if( m_trackletsMethod==1 ) {
    makeTracklets(0,2,trkletsMapLayer[1]);
    if( m_doLayerSum3 ) 
      makeTracklets(1,2,trkletsMapLayer[2]);
  }
  // for(int i=0; i<3; i++) {
//     log << MSG::DEBUG << "layer: " << i << endreq;
//     typedef std::map<int,int>::iterator mit;
//     for(mit it=m_usedClusterMap[i].begin(); it!=m_usedClusterMap[i].end(); it++) {
//       if( it->second ) {
// 	log << MSG::DEBUG << it->first;
//       }
//     }
//     log << MSG::DEBUG << endreq;
//   }
  
  

  // int ntrs1 = 0;
//   for(mapit it=trkletsMapLayer[1].begin();it!=trkletsMapLayer[1].end(); it++) {
//     int ntr = (it->second).size();
//     ntrs1 += ntr;
//   }

//   int ntrs2 = 0;
//   for(mapit it=trkletsMapLayer[2].begin();it!=trkletsMapLayer[2].end(); it++) {
//     int ntr = (it->second).size();
//     ntrs2 += ntr;
//   }
  //  log << MSG::DEBUG << "01:02:12=" << trkcoll->size() << ":" << ntrs1 << ":" << ntrs2 << endreq;
  
  if( trkletsMapLayer[2].size() )
    removeDuplicateLayerToLayer(1,trkletsMapLayer[2],trkletsMapLayer[0],trkletsMapLayer[1], trkcoll);
  
  std::map<int,std::vector<HIPixelTracklets*> > ghostMap;
  if( trkletsMapLayer[1].size() )
    removeDuplicateLayerToLayer(0,trkletsMapLayer[1],trkletsMapLayer[0],ghostMap,trkcoll);
  
  typedef std::map<int, std::vector<HIPixelTracklets*> >::iterator mapit;
  for(mapit it=trkletsMapLayer[0].begin();it!=trkletsMapLayer[0].end(); it++) {
    int ntr = (it->second).size();
    for(int i=0; i<ntr; i++) {
      trkcoll->push_back( (it->second)[i] );
    }
  }
  
  //log << MSG::DEBUG << "total: " << trkcoll->size() << endreq;
  //record tracklets collection to store gate;
  ATH_CHECK( evtStore()->record(trkcoll,"pixelTrackletsCollection") );
  
  return StatusCode::SUCCESS;;
}


void HIPixelTrackletsMaker::clearVectors() {
  for(int i=0; i<3; i++ ) {
    m_usedClusterMap[i].clear();
    m_clusterMap[i].clear();
    m_filledCells[i].clear();
    m_clusterLayer[i].clear();
    m_etaValueLayer[i].clear();
    m_phiValueLayer[i].clear();
  }
  
  for(int i=0; i<13; i++) {
    for(int j=0; j<22; j++) {
      m_gangedClusIndexLayer0[j][i].clear();
    }
    for(int j=0; j<38; j++) {
      m_gangedClusIndexLayer1[j][i].clear();
    }
    for(int j=0; j<52; j++) {
      m_gangedClusIndexLayer2[j][i].clear();
    }
  }
  for(int i=0; i<3; i++) {
    m_gangedNoiseLayer[i].clear();
  }
  for(int i=0; i<3; i++) {
    m_overlapClusLayer[i].clear();
  }
}

void HIPixelTrackletsMaker::layerClusters (const DataHandle<PixelClusterContainer>& clusters) {
  //log << MSG::DEBUG << "layerClusters beginning." << endreq;
  //clear first
  for (const PixelClusterCollection* coll : *clusters) {
    for (const PixelCluster* cluster_p : *coll) {
      const PixelCluster& cluster = *cluster_p;
      int barrel = m_pixelID->barrel_ec(cluster.detectorElement()->identify());
      if( barrel !=0 ) continue;
      int layer = m_pixelID->layer_disk(cluster.detectorElement()->identify());
      m_clusterLayer[layer].push_back(cluster);
    }
  }

  for(int layer=0; layer<3; layer++) {
    for(unsigned int i=0; i<m_clusterLayer[layer].size(); i++) {
      const PixelCluster& clusi0 = m_clusterLayer[layer][i];
      Identifier idClus = clusi0.detectorElement()->identify();
      if( !clusi0.gangedPixel() ) continue;
      
      int etaMo = m_pixelID->eta_module( idClus ) + 6;
      int phiMo = m_pixelID->phi_module( idClus );
      //log << MSG::DEBUG << "layer:etaMo:phiMo=" << layer << ":" << etaMo << ":" << phiMo << endreq;
      const std::vector<Identifier>& rdoList = clusi0.rdoList();
      std::vector<IndexEtaPhi> pixsColl;
      for(unsigned int ir=0; ir<rdoList.size(); ir++) {
	Identifier idPix = rdoList[ir];
	int etaInd = m_pixelID->eta_index( idPix );
	int phiInd = m_pixelID->phi_index( idPix );
	IndexEtaPhi indexEtaPhi(etaInd, phiInd);
	pixsColl.push_back(indexEtaPhi);
      }
      if(layer==0)  m_gangedClusIndexLayer0[phiMo][etaMo][i] = pixsColl;
      else if(layer==1) m_gangedClusIndexLayer1[phiMo][etaMo][i] = pixsColl;
      else if(layer==2) m_gangedClusIndexLayer2[phiMo][etaMo][i] = pixsColl;
    }
  }
  //log << MSG::DEBUG << "layerClusters ending." << endreq;
}

void HIPixelTrackletsMaker::mapClusters() {
  const int nCells = m_nBinsEta*m_nBinsPhi;
  
  for(int layer=0; layer<3; layer++) {
    std::multimap<int, int> celli;
    //if(0) ATH_MSG_DEBUG( "layer: " << layer );
    for(unsigned int i=0; i<m_clusterLayer[layer].size(); i++) {
      const PixelCluster& clusi0 = m_clusterLayer[layer][i];
      
      //impose disabled modules by hand, no good MC available now
      if(m_doMagOff && layer==1) {
	if( m_pixelID->eta_module(clusi0.identify())==0 &&  m_pixelID->phi_module(clusi0.identify())==13 ) continue;
	if( m_pixelID->eta_module(clusi0.identify())==5 &&  m_pixelID->phi_module(clusi0.identify())==9 ) continue;
      }
      
      mapiiitr ito = m_overlapClusLayer[layer].find(i);
      if(ito!=m_overlapClusLayer[layer].end() && (ito->second)>0 ){
      	continue;
      }
      
      mapiiitr itr = m_gangedNoiseLayer[layer].find(i);
      if(itr!=m_gangedNoiseLayer[layer].end() && (itr->second)>0 ) {
	continue; //this is non-zero because of the removal of ganged noise
      }
      
      double cxi0 = clusi0.globalPosition().x() - m_vx;
      double cyi0 = clusi0.globalPosition().y() - m_vy;
      double czi0 = clusi0.globalPosition().z() - m_vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      //To keep the correctness of the vector index, we can not do before processing here.
      //Big Error if use continue before there two lines,
      //the index of m_etaValueLayer and m_phiValueLayer will be wrong
      if( m_trackletsMethod==2 && m_backgroundFlip==1 ) {
	//if( layer==1 ) etai0 *= -1.;
	if( layer==1 ) {
	  if( phii0>0 ) phii0 -= TMath::Pi();
	  else phii0 += TMath::Pi();
	}
	// if( layer==1 ) {
// 	  if( phii0>0 ) phii0 -= TMath::Pi();
// 	  else phii0 += TMath::Pi();
// 	}
      }
      m_etaValueLayer[layer][i]=etai0;
      m_phiValueLayer[layer][i]=phii0;
      
      
      //do a energy cut before any process
      if( clusi0.energyLoss()<energyThresh(layer,etai0) ) continue;
      
      int beta = (int)((etai0-m_minEta)/m_binSizeEta); 
      int bphi = (int)((phii0-m_minPhi)/m_binSizePhi);
      if( beta<0 || beta>m_nBinsEta-1 || bphi<0 || bphi>m_nBinsPhi-1 )  continue;
      int bin = beta + bphi*m_nBinsEta;
      //if(0) ATH_MSG_DEBUG( "index:eta:phi:beta:bphi=" << i << ":" << etai0 << ":" << phii0 << ":" << beta << ":" << bphi );
      celli.insert(std::pair<int,int>(bin,i));
    } //clusters in one layer
    
    std::multimap<int,int>::iterator it;
    std::pair< std::multimap<int,int>::iterator, std::multimap<int,int>::iterator > itp;
    for(int bc=0; bc<nCells; bc++) {
      itp = celli.equal_range(bc);
      std::vector<int> iclus;
      for(it=itp.first; it!=itp.second; it++) {
	iclus.push_back((*it).second);
      }
      if( iclus.size() ) {
	m_clusterMap[layer][bc] = iclus;
	m_filledCells[layer].insert(bc);
      }
    }
    //log << MSG::DEBUG << "Layer=" << layer << ", Cells=" << m_filledCells[layer].size() << endreq;
  } //end of layer
  
}

void HIPixelTrackletsMaker::makeTracklets(int innerLayer, int outLayer, std::map<int,std::vector<HIPixelTracklets*> > & trackletsCellMap) {
  //log << MSG::DEBUG << "makeTracklets beginning!" << endreq;
  std::set<int>::iterator it = m_filledCells[innerLayer].begin();
  std::set<int> trackletsCellKeys;
  std::vector<int> usedClustersIndex[3];
  for( ; it!=m_filledCells[innerLayer].end(); it++) {
    int bin = *it;
    //log << MSG::DEBUG << "Cell: " << bin << " # of clusters: " << m_clusterMap[innerLayer][bin].size() << endreq;
    std::vector<HIPixelTracklets*> trkletsCollInCell;
    makeTrackletsInCell(innerLayer,outLayer,bin,trkletsCollInCell, usedClustersIndex);
    //log << MSG::DEBUG << "Size: " << trkletsCollInCell.size() << endreq;
    if( !trkletsCollInCell.size() ) continue; 
    //removed duplicate with existing trklets, only have to consider neighboring cells
    trackletsCellKeys.insert(bin);
    trackletsCellMap.insert(std::pair<int,std::vector<HIPixelTracklets*> >(bin,trkletsCollInCell));
  }

  //update used cluster map
  for(unsigned int ic=0; ic<usedClustersIndex[innerLayer].size(); ic++) {
    int index = usedClustersIndex[innerLayer][ic];
    m_usedClusterMap[innerLayer][index]++;
  }
  for(unsigned int ic=0; ic<usedClustersIndex[outLayer].size(); ic++) {
    int index =usedClustersIndex[outLayer][ic];
    m_usedClusterMap[outLayer][index]++;
  }

  //log << MSG::DEBUG << "Start to merge overlap or ganged!" << endreq;
  typedef std::map<int,std::vector<HIPixelTracklets*> >::iterator mapit;
  //Merge duplicate in cell first, then loop neighboring cells
  for(std::set<int>::iterator it=trackletsCellKeys.begin(); it!=trackletsCellKeys.end(); it++) {
    int bin = *it;
    mapit im = trackletsCellMap.find(bin);
    if( im==trackletsCellMap.end() ) continue;
    std::vector<HIPixelTracklets*> trkletsCollInCell = im->second;
    std::map<int,int> removedInCell;
    for(unsigned int itrk=0; itrk<trkletsCollInCell.size(); itrk++) {
      if( removedInCell[itrk] ) continue;
      HIPixelTracklets* trki = trkletsCollInCell[itrk];
      for(unsigned int jtrk=itrk+1; jtrk<trkletsCollInCell.size(); jtrk++) {
	if( removedInCell[jtrk] ) continue;
	HIPixelTracklets* trkne = trkletsCollInCell[jtrk];
	//int whichOneToKeep = 0;
	int whichOneToKeep = checkGangedOrOverlap(trki,trkne);
	if( !whichOneToKeep ) continue; //no need to merge
	if( whichOneToKeep==1 ) {
	  trki->Merge(trkne);
	  removedInCell[jtrk]++;
	}
	else if( whichOneToKeep==2 ) {
	  removedInCell[itrk]++;
	  trkne->Merge(trki);
	  break;
	}
	else {
          ATH_MSG_ERROR( " returned value of checkGangedOrOverlap() function is wrong!" );
        }
      }  //jth tracklet
    } //ith tracklet
    
    if( removedInCell.size() ) {
      std::vector<HIPixelTracklets*> newtrkletsCollInCell;
      for(unsigned int itrk=0; itrk<trkletsCollInCell.size(); itrk++) {
        if( removedInCell[itrk] ) continue;
        newtrkletsCollInCell.push_back(trkletsCollInCell[itrk]);
      }
      trackletsCellMap.erase(bin);
      if( newtrkletsCollInCell.size() ) {
        trackletsCellMap.insert(std::pair<int,std::vector<HIPixelTracklets*> >(bin,newtrkletsCollInCell) );
      }
    } //finished cleanning
    
  } //all cells

  //Merge neighboring cells
  std::map<int, int> comparedWithAllOthersBinLabel; //bins that are compared with all others
  for(std::set<int>::iterator it=trackletsCellKeys.begin(); it!=trackletsCellKeys.end(); it++) {
    int bin = *it;
    mapit im = trackletsCellMap.find(bin);
    if( im==trackletsCellMap.end() ) continue;
    std::vector<HIPixelTracklets*> trkletsCollInCell = im->second;
    std::map<int, int> removedInCell;
    int etaBin,phiBin;
    returnCorrespondingCell(bin,etaBin,phiBin);
    for(unsigned int itrk=0; itrk<trkletsCollInCell.size(); itrk++) {
      HIPixelTracklets* trki = trkletsCollInCell[itrk];
      bool iRemoved = false;
      for(int ib=etaBin-1; ib<=etaBin+1; ib++) {
	if( iRemoved ) break;
	for(int jb=phiBin-1; jb<=phiBin+1; jb++) {
	  if( iRemoved ) break;
	  if( ib==etaBin && jb==phiBin ) continue; //the same cell, already processed above
	  if(ib<0 || ib>m_nBinsEta-1 ) continue;  //edge in eta direction
	  int neighborBin = ib + jb*m_nBinsEta;
	  if(jb<0 || jb>m_nBinsPhi-1 ) {    //edge in phi direction, need more care                             
	    if( jb==-1 ) neighborBin = ib+ (m_nBinsPhi-1)*m_nBinsEta ;
	    else if ( jb==m_nBinsPhi ) neighborBin = ib;
	    else {
              ATH_MSG_ERROR( " phi bin out of boundary!!! " );
            }
	  }
	  mapiiitr itcom = comparedWithAllOthersBinLabel.find(neighborBin);
	  if( itcom!=comparedWithAllOthersBinLabel.end() && (itcom->second)>0 ) continue; //this bin already processed
	  
	  std::vector<HIPixelTracklets*> trkletsCollInNeighborCell;
	  std::map<int,int> removedInNeighborCell;
	  std::map<int, std::vector<HIPixelTracklets*> >::iterator itm;
	  itm = trackletsCellMap.find(neighborBin);
	  if( itm==trackletsCellMap.end() ) continue;
	  trkletsCollInNeighborCell = itm->second;
	  for(unsigned int ine=0; ine<trkletsCollInNeighborCell.size(); ine++) {
	    HIPixelTracklets* trkne = trkletsCollInNeighborCell[ine];
	    //int whichOneToKeep = 0;
	    int whichOneToKeep = checkGangedOrOverlap(trki,trkne);
	    if( !whichOneToKeep ) continue;

	    if( whichOneToKeep==1 ) {
	      removedInNeighborCell[ine]++;
	      trki->Merge(trkne);
	    }
	    else if(whichOneToKeep==2 ) {
	      removedInCell[itrk]++;
              trkne->Merge(trki);
              iRemoved = true;
	      break;
            }
            else {
              ATH_MSG_ERROR( " returned value of checkGangedOrOverlap() function is wrong!" );
            }
	    
	  } //neighboring cells
	  
	  if( removedInNeighborCell.size() ) {
	    std::vector<HIPixelTracklets*> newtrkletsCollInNeighborCell;
	    for(unsigned int ine=0; ine<trkletsCollInNeighborCell.size(); ine++) {
	      if( removedInNeighborCell[ine] )  continue;
	      newtrkletsCollInNeighborCell.push_back(trkletsCollInNeighborCell[ine] );
	    }
	    trackletsCellMap.erase(neighborBin);
	    if(newtrkletsCollInNeighborCell.size() ) {
	      trackletsCellMap.insert(std::pair<int,std::vector<HIPixelTracklets*> >(neighborBin,newtrkletsCollInNeighborCell) );
	    }
	  } //end of clean in neighbor cell
	  
	} //phi neighbor
      } //eta neighbor
      
    } //tracklets in one cell
    
    //clean this cell                                                                                          
    if( removedInCell.size() ) {
      std::vector<HIPixelTracklets*> newtrkletsCollInCell;
      for(unsigned int itrk=0; itrk<trkletsCollInCell.size(); itrk++) {
        if( removedInCell[itrk] ) continue;
        newtrkletsCollInCell.push_back(trkletsCollInCell[itrk]);
      }
      trackletsCellMap.erase(bin);
      if( newtrkletsCollInCell.size() ) {
        trackletsCellMap.insert(std::pair<int,std::vector<HIPixelTracklets*> >(bin,newtrkletsCollInCell) );
      }
    } // end of clean cell 
    
    //label this cell as processed
    comparedWithAllOthersBinLabel[bin]++;
    
  } //all cells
  
  
//   for(it=trackletsCellKeys.begin(); it!=trackletsCellKeys.end(); it++) {
//     int bin = *it;
//     mapit im = trackletsCellMap.find(bin);
//     if( im==trackletsCellMap.end() ) continue;
//     std::vector<HIPixelTracklets*> trkletsCollInCell = im->second;
//     std::map<int,int> removedInCell;
//     //std::map<int,int> removedInCellReason; //first index=removed index, second index=index caused removing
//     int etaBin,phiBin;
//     returnCorrespondingCell(bin,etaBin,phiBin);
    
//     for(unsigned int itrk=0; itrk<trkletsCollInCell.size(); itrk++) {
//       log << MSG::DEBUG << "etaBin:phiBin=" << etaBin << ":" << phiBin << ", itrk=" << itrk << endreq; 
//       if( removedInCell[itrk] ) continue;
//       HIPixelTracklets* trki = trkletsCollInCell[itrk];
//       log << MSG::DEBUG << "id0:id1=" << (*(trki->GetClusters()))[0]->identify() << ":" << (*(trki->GetClusters()))[1]->identify() << endreq;
//       bool iRemoved = false;
//       for(int ib=etaBin-1; ib<=etaBin+1; ib++) {
// 	if( iRemoved ) break;
// 	for(int jb=phiBin-1; jb<=phiBin+1; jb++) {
// 	  if( iRemoved ) break;
// 	  if(ib<0 || ib>m_nBinsEta-1 ) continue;  //edge in eta direction     
// 	  int neighborBin = ib + jb*m_nBinsEta;
// 	  std::vector<HIPixelTracklets*> trkletsCollInNeighborCell;
// 	  std::map<int,int> removedInNeighborCell;
// 	  if( ib==etaBin && jb==phiBin ) trkletsCollInNeighborCell = trkletsCollInCell;
// 	  else {
// 	    if(jb<0 || jb>m_nBinsPhi-1 ) {    //edge in phi direction, need more care 
// 	      if( jb==-1 ) neighborBin = ib+ (m_nBinsPhi-1)*m_nBinsEta ;
// 	      else if ( jb==m_nBinsPhi ) neighborBin = ib;
// 	      else { log << MSG::ERROR << " phi bin out of boundary!!! " << endreq; }
// 	    }
// 	    std::map<int, std::vector<HIPixelTracklets*> >::iterator itm;
// 	    itm = trackletsCellMap.find(neighborBin);
// 	    if( itm==trackletsCellMap.end() ) continue;
// 	    trkletsCollInNeighborCell = itm->second;
// 	  }
// 	  for(unsigned int ine=0; ine<trkletsCollInNeighborCell.size(); ine++) {
// 	    log << MSG::DEBUG << "Compared:etaBin:phiBin=" << ib << ":" << jb << ", ine=" << ine << endreq;
// 	    if( iRemoved ) break;
// 	    HIPixelTracklets* trkne = trkletsCollInNeighborCell[ine];
// 	    if( ib==etaBin && jb==phiBin && trkne==trki ) continue; //itself
// 	    if( ib==etaBin && jb==phiBin ) {
// 	      if( removedInCell[ine] ) continue;
// 	    }
// 	    else {
// 	      if( removedInNeighborCell[ine] ) continue;
// 	    }
// 	    int whichOneToKeep = 0;
// 	    checkGangedOrOverlap(trki,trkne,whichOneToKeep);
// 	    log << MSG::DEBUG << "WhichOneToKeep=" << whichOneToKeep << endreq;
// 	    // 	    log << MSG::DEBUG << "bin:itrk=" << bin << ":" << itrk << "(";
// // 	    for(int ttr=0; ttr<trki->GetClusters()->size(); ttr++) {
// // 	      log << (*(trki->GetClusters()))[ttr]->identify() << ":";
// // 	    }
// // 	    log << MSG::DEBUG << endreq;
// // 	    log << MSG::DEBUG << "bin:itrk=" << neighborBin << ":" << ine << "(";
// // 	    for(int ttr=0; ttr<trkne->GetClusters()->size(); ttr++) {
// // 	      log << (*(trkne->GetClusters()))[ttr]->identify() << ":";
// // 	    }
// // 	    log << MSG::DEBUG << endreq;
// 	    if( !whichOneToKeep ) continue;
// 	    if( whichOneToKeep==1 ) { 
// 	      if(!(ib==etaBin && jb==phiBin)) {
// 		removedInNeighborCell[ine]++; 
// 		trki->Merge(trkne);
// 	      }
// 	      else  {
// 		removedInCell[ine]++;  //removed by tracklet in the same cell
// 		trki->Merge(trkne);
// 	      }
// 	    }
// 	    else if(whichOneToKeep==2 ) { removedInCell[itrk]++;
// 	      trkne->Merge(trki);
// 	      iRemoved = true;
// 	    }
// 	    else { log<< MSG::ERROR << " returned value of checkGangedOrOverlap() function is wrong!" << endreq; }
// 	  } //end of loop tracklets in neighbor cell
	  
// 	  if( removedInNeighborCell.size() && !(ib==etaBin && jb==phiBin) ) {
// 	    std::vector<HIPixelTracklets*> newtrkletsCollInNeighborCell;
// 	    for(unsigned int ine=0; ine<trkletsCollInNeighborCell.size(); ine++) {
// 	      if( removedInNeighborCell[ine] )  continue;
// 	      newtrkletsCollInNeighborCell.push_back(trkletsCollInNeighborCell[ine] );
// 	    }
// 	    trackletsCellMap.erase(neighborBin);
// 	    if(newtrkletsCollInNeighborCell.size() ) {
// 	      trackletsCellMap.insert(std::pair<int,std::vector<HIPixelTracklets*> >(neighborBin,newtrkletsCollInNeighborCell) );
// 	    }
// 	  } //end of clean in neighbor cell
// 	} //end of j
//       } //end of i
      
//     } // end of tracklets in cell
    
//     //clean this cell
//     if( removedInCell.size() ) {
//       std::vector<HIPixelTracklets*> newtrkletsCollInCell;
//       for(unsigned int itrk=0; itrk<trkletsCollInCell.size(); itrk++) {
// 	if( removedInCell[itrk] ) continue;
// 	newtrkletsCollInCell.push_back(trkletsCollInCell[itrk]);
//       }
//       trackletsCellMap.erase(bin);
//       if( newtrkletsCollInCell.size() ) {
// 	trackletsCellMap.insert(std::pair<int,std::vector<HIPixelTracklets*> >(bin,newtrkletsCollInCell) );
//       }
//     } // end of clean cell
  
//   } //end of loop all cells with tracklets
  
}

void HIPixelTrackletsMaker::makeTrackletsInCell(int innerLayer, int outLayer, int bin, std::vector<HIPixelTracklets*> & trkletsCollWithDup, std::vector<int> usedClustersIndex[3]) {
  ATH_MSG_DEBUG( "makeTrackletsInCell() begin!" );
  double etaCutUsed = m_dEta;
  double phiCutUsed = m_dPhi;

  if( (innerLayer+outLayer)==2 ) {
    etaCutUsed *= 2;
    phiCutUsed *= (122.5-50.5)/(88.5-50.5);
  }
  if( (innerLayer+outLayer)==3 ) phiCutUsed *= (122.5-88.5)/(88.5-50.5);
    
  int etaBin,phiBin;
  returnCorrespondingCell(bin,etaBin,phiBin);
  ATH_MSG_DEBUG( "etaBin:phiBin " << etaBin << ":" << phiBin );
  
  //local variable to keep the used clusters
  //std::vector<int> usedClustersIndex[3]; //for both three layers
  
  // std::vector<HIPixelTracklets*> trkletsCollWithDup;
  std::vector<int> iclus = m_clusterMap[innerLayer][bin];
  //bins in the out layer should be considered;
  for(unsigned int iic=0; iic<iclus.size(); iic++) {
    int innerClusterIndex = iclus[iic];
    mapiiitr itr0 = m_usedClusterMap[innerLayer].find(innerClusterIndex);
    if( itr0!=m_usedClusterMap[innerLayer].end() && itr0->second>0 )  continue;
    
    const PixelCluster & clusi0 = m_clusterLayer[innerLayer][innerClusterIndex];
    double cxi0 = clusi0.globalPosition().x() - m_vx;
    double cyi0 = clusi0.globalPosition().y() - m_vy;
    double phii0 = m_phiValueLayer[innerLayer][innerClusterIndex];
    double etai0 = m_etaValueLayer[innerLayer][innerClusterIndex];
    
    if(innerLayer==1) ATH_MSG_DEBUG( "innerIndex:" << innerClusterIndex );
    //multi satisfaction? restore them and only use the smallest r one
    //or use the smallest deta
    std::vector<int> minclus1label;
    std::vector<double> rcan;
    std::vector<double> etacan;
    for(int i=etaBin-1; i<=etaBin+1; i++) {
      for(int j=phiBin-1; j<=phiBin+1; j++) {
	if(i<0 || i>m_nBinsEta-1 ) continue;  //edge in eta direction             
	int outBin = i + j*m_nBinsEta;
	if(j<0 || j>m_nBinsPhi-1 ) {    //edge in phi direction, need more care   
	  if( j==-1 ) outBin = i+ (m_nBinsPhi-1)*m_nBinsEta ;
	  else if ( j==m_nBinsPhi ) outBin = i;
	  else {
            ATH_MSG_ERROR( " phi bin out of boundary!!! " );
          }
	}
	std::map<int, std::vector<int> >::iterator itm;
	itm = m_clusterMap[outLayer].find(outBin);
	if( itm==m_clusterMap[outLayer].end() ) continue;
	std::vector<int> oclus = itm->second;
	//log << MSG::DEBUG << "Out Layer bin=" << outBin << ", Size=" << oclus.size() << endreq;
	for(unsigned int ioc=0; ioc<oclus.size(); ioc++) {
	  int outClusterIndex = oclus[ioc];
	  mapiiitr itr1 = m_usedClusterMap[outLayer].find(outClusterIndex);
	  if( itr1!=m_usedClusterMap[outLayer].end() && itr1->second>0 ) continue;
	  
	  const PixelCluster & clusi1 = m_clusterLayer[outLayer][outClusterIndex];
	  double phii1 = m_phiValueLayer[outLayer][outClusterIndex];
	  double etai1 = m_etaValueLayer[outLayer][outClusterIndex];
	  //std::cout << "phi1=" << phii1 << "?" << atan2(cyi1,cxi1) << std::endl;
	  double deta = etai0 - etai1;
	  double dphi = phii0 - phii1;
	  if( dphi>M_PI )  dphi -= 2*M_PI;
	  if( dphi<-M_PI ) dphi += 2*M_PI;
	  if(innerLayer==1 ) ATH_MSG_DEBUG( "outIndex:deta:dphi=" << outClusterIndex << ":" << deta << ":" << dphi );
	  if( !m_doMagOff ) { //with magnetic field
	    if( fabs(deta)<etaCutUsed && fabs(dphi)<phiCutUsed ){
	      //one more constraint on deta base the fitting function we got for sigma deta
	      //For magnetic off, there is no pt measurement
	      if( m_trackletsMethod==1 ) {
		double cxi1 = clusi1.globalPosition().x() - m_vx;
		double cyi1 = clusi1.globalPosition().y() - m_vy;
		double O1 = 0.5*(cyi1-cyi0+cxi1*cxi1/cyi1-cxi0*cxi0/cyi0)/(cxi1/cyi1-cxi0/cyi0);
		double O2 = 0.5*(cxi1-cxi0+cyi1*cyi1/cxi1-cyi0*cyi0/cxi0)/(cyi1/cxi1-cyi0/cxi0);
		double radius = sqrt(O1*O1+O2*O2);
		double pt = 0.0006*radius;
		double par0 = 0.0;
		double par1 = 0.0;
		fittingParSigmaDeta(innerLayer+outLayer-1,etai0,par0,par1);
		if( par0>0. && par1>0. ) {
		  double sigmaDeta = sqrt(par0+par1/(pt*pt));
		  if( fabs(deta)>3*sigmaDeta ) continue; //3 sigma delta eta cut
		}
	      } //method 1
	      minclus1label.push_back(outClusterIndex);
	      double r = sqrt(deta*deta/(etaCutUsed*etaCutUsed)+dphi*dphi/(phiCutUsed*phiCutUsed) );
	      rcan.push_back(r);
	      etacan.push_back(deta);
	    }
	  } //end of if magnetic field
	  else { //magnetic off 
	    double sigmaDphi = m_sigmaDphiEtaFunc->Eval(etai0);
	    double sigmaDeta = m_sigmaDetaEtaFunc->Eval(etai0);
	    // 	    double par0 = 0.0;
// 	    double par1 = 0.0;
// 	    fittingParSigmaDeta(innerLayer+outLayer-1,etai0,par0,par1);
// 	    double sigmaDeta = sqrt(par0+par1/(0.15*0.15)); //150MeV evaluation
	    double normalizedR = sqrt(deta*deta/(sigmaDeta*sigmaDeta)+dphi*dphi/(sigmaDphi*sigmaDphi));
	    //3 sigma cut here
	    if( normalizedR < 3*sqrt(2.) ) {
	      minclus1label.push_back(outClusterIndex);
	      rcan.push_back(normalizedR);
	      etacan.push_back(normalizedR); //use this to consistent with magnetic on code
	    }
	  }
	  
	}  //end of the out layer loop
      }  //end of j bin
    }  //end of i bin
    
    
    if(rcan.size()==0) continue;
    
    const HepGeom::Point3D<double> vPosition(m_vx,m_vy,m_vz);
    if( m_trackletsMethod==1 ) {
      double etamin = 9999.;
      int minclus1can = minclus1label[0];
      
      for(unsigned int ir=0; ir<etacan.size(); ir++){
	if( fabs(etacan[ir]) < etamin ){
	  etamin = fabs(etacan[ir]);
	  minclus1can = minclus1label[ir];
	}
      }
      
      //const PixelCluster& minClus = m_clusterLayer[outLayer][minclus1can];
      
      //Can't do this, will cause bias in high multiplicity events.
      //Right now only update used clusters map after the looping, 
      //So the share of the second layer clusters is allowed.
      //Need detailed study to solve this ambiguity.
      //m_usedClusterMap[innerLayer][innerClusterIndex]++;
      //m_usedClusterMap[outLayer][minclus1can]++;
      usedClustersIndex[innerLayer].push_back(innerClusterIndex);
      usedClustersIndex[outLayer].push_back(minclus1can);
      
      std::vector<PixelCluster*> twoClusters;
      twoClusters.push_back(&m_clusterLayer[innerLayer][innerClusterIndex]);
      twoClusters.push_back(&m_clusterLayer[outLayer][minclus1can]);
      for(unsigned int ir=0; ir<minclus1label.size(); ir++) {
	if( minclus1label[ir]==minclus1can ) continue;
	int index = minclus1label[ir];
	twoClusters.push_back(&m_clusterLayer[outLayer][index]);
      }
      
      trkletsCollWithDup.push_back(new HIPixelTracklets(vPosition,twoClusters));
      //if(0 ) ATH_MSG_DEBUG( "innerIndex:outIndex=" << innerClusterIndex << ":" << minclus1can );
    }
    
    //     //Right now according to Brian's suggestion, to test if the background are
    //     //the same with one to multiple satisfaction be treated as multiple tracklets
    if( m_trackletsMethod==2 ) {
      for(unsigned int ir=0; ir<minclus1label.size(); ir++) {
	int index = minclus1label[ir];
	std::vector<PixelCluster*> twoClusters;
	twoClusters.push_back(&m_clusterLayer[innerLayer][innerClusterIndex]);
	twoClusters.push_back(&m_clusterLayer[outLayer][index]);
	trkletsCollWithDup.push_back(new HIPixelTracklets(vPosition,twoClusters));
      }
    }
    //double phii1 = 
    //projection to 2-layer, and if multiple satisfaction, use them to solve ambiguity
    //std::vector<bool> projectedCan;
    // for(unsigned int ir=0; ir<rcan.size(); ir++){ 
    //       double phii1 = m_phiValueLayer[outLayer][minclus1can];
    //       double etai1 = m_etaValueLayer[outLayer][minclus1can];
    //       std::cout << "phican=" << phii1 << std::endl;
    //       bool projected = false;
    if( (innerLayer+outLayer)==1 ) {
      for(int i=etaBin-2; i<=etaBin+2; i++) {
	for(int j=phiBin-2; j<=phiBin+2; j++) {
	  if(i<0 || i>m_nBinsEta-1 ) continue;  //edge in eta direction    
	  int outBin = i + j*m_nBinsEta;
	  if(j<0 || j>m_nBinsPhi-1 ) {    //edge in phi direction, need more care
	    if( j==-1 ) outBin = i+ (m_nBinsPhi-1)*m_nBinsEta ;
	    else if ( j==m_nBinsPhi ) outBin = i;
	    else if ( j==-2 ) { 
	      if(m_nBinsPhi>1) outBin = i + (m_nBinsPhi-2)*m_nBinsEta ;
	      else continue;
	    }
	    else if ( j==m_nBinsPhi+1 ) {
	      if(m_nBinsPhi>1)   outBin = i + m_nBinsEta;
	      else continue;
	    }
	    else {
              ATH_MSG_ERROR( "Bin in phi direction has an invalid value! " );
            }
	  }
	  std::map<int,std::vector<int> >::iterator itm;
	  itm = m_clusterMap[2].find(outBin);
	  if(itm==m_clusterMap[2].end() ) continue;
	  std::vector<int> clus = itm->second;
	  for(unsigned int ic=0; ic<clus.size(); ic++) {
	    int clusIndex = clus[ic];
	    mapiiitr itr2 = m_usedClusterMap[2].find(clusIndex);
	    if( itr2!=m_usedClusterMap[2].end() && (itr2->second)>0 ) continue;
	    //const PixelCluster & clusi2 = m_clusterLayer[2][clusIndex];
	    //double czi2 = clusi2.globalPosition().z() - m_vz;
	    //  double phii2 = atan2(cyi2,cxi2);
	    // 	    double ri2 = sqrt(cxi2*cxi2+cyi2*cyi2);
	    // 	    double thetai2 = atan2(ri2,czi2);
	    // 	    double etai2 = -TMath::Log(TMath::Tan(thetai2/2));
	    double phii2 = m_phiValueLayer[2][clusIndex];
	    double etai2 = m_etaValueLayer[2][clusIndex];
	    //std::cout << "phi2=" << phii2 << "?" << atan2(cyi2,cxi2) << std::endl;
	    double deta = etai0 - etai2;
	    double dphi = phii0 - phii2;
	    if( dphi>M_PI )  dphi -= 2*M_PI;
	    if( dphi<-M_PI ) dphi += 2*M_PI;
	    //if(0) ATH_MSG_DEBUG( "projection Index:" << clusIndex );
	    if( fabs(deta)<2*etaCutUsed && fabs(dphi)<2*phiCutUsed*(122.5-50.5)/(88.5-50.5)) {
	      // if( m_trackletsMethod==1 ) {
	      //double cxi2 = clusi2.globalPosition().x() - m_vx;
	      //double cyi2 = clusi2.globalPosition().y() - m_vy;
	      // 	      		double O1 = 0.5*(cyi2-cyi0+cxi2*cxi2/cyi2-cxi0*cxi0/cyi0)/(cxi2/cyi2-cxi0/cyi0);
	      // 	      		double O2 = 0.5*(cxi2-cxi0+cyi2*cyi2/cxi2-cyi0*cyi0/cxi0)/(cyi2/cxi2-cyi0/cxi0);
	      // 	      		double radius = sqrt(O1*O1+O2*O2);
	      // 	      		double pt = 0.0006*radius;
	      // 	      		double par0 = 0.0;
	      // 	      		double par1 = 0.0;
	      // 	      		fittingParSigmaDeta(1,etai0,par0,par1);
	      // 	      		if( par0>0. && par1>0. ) {
	      // 	      		  double sigmaDeta = sqrt(par0+par1/(pt*pt));
	      // 	      		  if( fabs(deta)<3*sigmaDeta ) { //3 sigma delta eta cut
	      // 	      		    //projected = true;
	      // 	      		    m_usedClusterMap[2][clusIndex]++;
	      // 	      		  }
	      // 	      		}
	      // 	      		else m_usedClusterMap[2][clusIndex]++;
	      // 	      }
	      //else m_usedClusterMap[2][clusIndex]++;
	      m_usedClusterMap[2][clusIndex]++;
	    }
	  }   
	} //loop over j neighbors
      } //loop over i neighbors
    } //end of projection
    
    
      //projectedCan.push_back(projected);
    //} //end of all candidates loop
    
    // //need the projection to solve ambiguity when multiple satisfaction
//     double etamin = 9999.;                                                                                
//     int minclus1can = minclus1label[0];                                                                  
//     // //     for(unsigned int ir=0; ir<rcan.size(); ir++){                                                    
//     // //       if( rcan[ir] < rmin ){                                                                         
//     // //   rmin = rcan[ir];                                                                                   
//     // //   minclus1can = minclus1label[ir];                                                                  
//     // //       }                          
//     if( rcan.size()>1 ) {
//       for(unsigned int ir=0; ir<rcan.size(); ir++){ 
// 	if( ) {}
//       }
//     }
    
  } //end of i clusters
  
  
  // int duplicate = 0;
//   std::vector<HIPixelTracklets*> trkletsCollWithoutDup;
//   if( trkletsCollWithDup.size()>1 ) {
//     removeDuplicateInCell(trkletsCollWithDup,trkletsCollWithoutDup, duplicate);
//   }
//   if( !duplicate ) {
//     for( unsigned int i=0; i<trkletsCollWithDup.size(); i++ )
//       trkletsColl.push_back( trkletsCollWithDup[i] );
//   }
//   else {
//     for( unsigned int i=0; i<trkletsCollWithoutDup.size();i++ )
//       trkletsColl.push_back( trkletsCollWithoutDup[i] );
//   }
  
 //  if(0) {
//     for(unsigned int i=0; i<trkletsColl.size(); i++) {
      
//     }
}

void HIPixelTrackletsMaker::returnCorrespondingCell(int bin, int & i, int & j) {
  j = (int)(bin/m_nBinsEta);
  i = bin - j*m_nBinsEta;
} 

void HIPixelTrackletsMaker::makeTrackletsCellToCell(int /* innerLayer */, int /* outLayer */, int /* etaBin */, int /* phiBin */, const std::vector<int> & /* iclus */, const std::vector<int> & /* oclus */, std::vector<HIPixelTracklets*> & /* trkletsColl */) {
  //bin is the innerLayer bin number, used only for projection
//   log << MSG::DEBUG << "makeTrackletsCellToCell begin! " << endreq;
//   double etaCutUsed = m_dEta;
//   double phiCutUsed = m_dPhi;
//   if( (innerLayer+outLayer)==2 ) phiCutUsed *= (122.5-50.5)/(88.5-50.5);
//   if( (innerLayer+outLayer)==3 ) phiCutUsed *= (122.5-88.5)/(88.5-50.5);
  
//   for(unsigned int iic=0; iic<iclus.size(); iic++) {
//     int innerClusterIndex = iclus[iic];
//     if( m_usedClusterMap[innerLayer].find(innerClusterIndex)!=m_usedClusterMap[innerLayer].end() ) continue;
//     const PixelCluster & clusi0 = m_clusterLayer[innerLayer][innerClusterIndex];
//     double cxi0 = clusi0.globalPosition().x() - m_vx;
//     double cyi0 = clusi0.globalPosition().y() - m_vy;
//     double czi0 = clusi0.globalPosition().z() - m_vz;
//     double phii0 = atan2(cyi0,cxi0);
//     double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
//     double thetai0 = atan2(ri0,czi0);
//     double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
    
//     if(innerLayer==1) log << MSG::DEBUG << "innerIndex:" << innerClusterIndex << endreq;
//     //multi satisfaction? restore them and only use the smallest r one
//     std::vector<int> minclus1label;
//     std::vector<double> rcan;
//     for(unsigned int ioc=0; ioc<oclus.size(); ioc++) {
//       int outClusterIndex = oclus[ioc];
//       if( m_usedClusterMap[outLayer].find(outClusterIndex)!=m_usedClusterMap[outLayer].end() ) continue;
//       const PixelCluster & clusi1 = m_clusterLayer[outLayer][outClusterIndex];
//       double cxi1 = clusi1.globalPosition().x() - m_vx;
//       double cyi1 = clusi1.globalPosition().y() - m_vy;
//       double czi1 = clusi1.globalPosition().z() - m_vz;
//       double phii1 = atan2(cyi1,cxi1);
//       double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
//       double thetai1 = atan2(ri1,czi1);
//       double etai1 = -TMath::Log(TMath::Tan(thetai1/2));

//       double deta = etai0 - etai1;
//       double dphi = phii0 - phii1;
//       if( dphi>M_PI )  dphi -= 2*M_PI;
//       if( dphi<-M_PI ) dphi += 2*M_PI;
//       if(innerLayer==1 ) log << MSG::DEBUG << "outIndex:deta:dphi=" << outClusterIndex << ":" << deta << ":" << dphi << endreq;
//       if( fabs(deta)<etaCutUsed && fabs(dphi)<phiCutUsed ){
// 	minclus1label.push_back(outClusterIndex);
// 	double r = sqrt(deta*deta/(etaCutUsed*etaCutUsed)+dphi*dphi/(phiCutUsed*phiCutUsed) );
// 	rcan.push_back(r);
//       }
//     }  //end of the out layer loop
    
//     if(rcan.size()==0) continue;
//     double rmin = 9999.;
//     int minclus1can = 0;
//     for(unsigned int ir=0; ir<rcan.size(); ir++){
//       if( rcan[ir] < rmin ){
// 	rmin = rcan[ir];
// 	minclus1can = minclus1label[ir];
//       }
//     }
//     const PixelCluster& minClus = m_clusterLayer[outLayer][minclus1can];
//     m_usedClusterMap[innerLayer][innerClusterIndex]++;
//     m_usedClusterMap[outLayer][minclus1can]++;
//     std::vector<PixelCluster*> twoClusters;
//     twoClusters.push_back(&m_clusterLayer[innerLayer][innerClusterIndex]);
//     twoClusters.push_back(&m_clusterLayer[outLayer][minclus1can]);
//     const HepGeom::Point3D<double> vPosition(m_vx,m_vy,m_vz);
//     trkletsColl.push_back(new HIPixelTracklets(vPosition,twoClusters));
//     if( innerLayer==1 ) log << MSG::DEBUG << "innerIndex:outIndex=" << innerClusterIndex << ":" << minclus1can << endreq;
//     //in priciple, should project to 2-layer and exclude those compatible with existing tracklets as used clusters, not done yet???
//     if( (innerLayer+outLayer)==1 ) {
//       for(int i=etaBin-2; i<=etaBin+2; i++) {
// 	for(int j=phiBin-2; j<=phiBin+2; j++) {
// 	  if(i<0 || i>m_nBinsEta-1 ) continue;  //edge in eta direction    
// 	  int outBin = i + j*m_nBinsEta;
// 	  if(j<0 || j>m_nBinsPhi-1 ) {    //edge in phi direction, need more care
// 	    if( j==-1 ) outBin = i+ (m_nBinsPhi-1)*m_nBinsEta ;
// 	    else if ( j==m_nBinsPhi ) outBin = i;
// 	    else { continue;}
// 	  }
// 	  std::map<int,std::vector<int> >::iterator itm;
// 	  itm = m_clusterMap[2].find(outBin);
// 	  if(itm==m_clusterMap[2].end() ) continue;
// 	  std::vector<int> clus = itm->second;
// 	  for(unsigned int ic=0; ic<clus.size(); ic++) {
// 	    int clusIndex = clus[ic];
// 	    if( m_usedClusterMap[2].find(clusIndex)!=m_usedClusterMap[2].end() ) continue;
// 	    const PixelCluster & clusi2 = m_clusterLayer[2][clusIndex];
// 	    double cxi2 = clusi2.globalPosition().x() - m_vx;
// 	    double cyi2 = clusi2.globalPosition().y() - m_vy;
// 	    double czi2 = clusi2.globalPosition().z() - m_vz;
// 	    double phii2 = atan2(cyi2,cxi2);
// 	    double ri2 = sqrt(cxi2*cxi2+cyi2*cyi2);
// 	    double thetai2 = atan2(ri2,czi2);
// 	    double etai2 = -TMath::Log(TMath::Tan(thetai2/2));
// 	    double deta = etai0 - etai2;
// 	    double dphi = phii0 - phii2;
// 	    if( dphi>M_PI )  dphi -= 2*M_PI;
// 	    if( dphi<-M_PI ) dphi += 2*M_PI;
// 	    if( fabs(deta)<2.0*etaCutUsed && fabs(dphi)<2.0*phiCutUsed*(122.5-50.5)/(88.5-50.5)) {
// 	      m_usedClusterMap[2][clusIndex]++;
// 	    }
// 	  }   
// 	}
//       } //loop over i neighbors
      
//     }
//   }
  // log << MSG::DEBUG << "makeTrackletsCellToCell end! " << endreq;
}

int HIPixelTrackletsMaker::checkGangedOrOverlap(const HIPixelTracklets* trk1, const HIPixelTracklets* trk2) {
  //whichOneToKeep=0, not ganged or overlap
  //whichOneToKeep=1, keep trk1
  //whichOneToKeep=2, keep trk2
  //whichOneToKeep = 0; //not ganged or overlap
  int gangeLayer0 = 0;
  int overlapLayer0 = 0;
  int sameLayer0 = 0;
  int gangeLayer1 = 0;
  int overlapLayer1 = 0;
  int sameLayer1 = 0;
  const std::vector<PixelCluster*> *clus1Coll = trk1->GetClusters();
  const std::vector<PixelCluster*> *clus2Coll = trk2->GetClusters();
  const PixelCluster & clus10 = *(*clus1Coll)[0];
  const PixelCluster & clus11 = *(*clus1Coll)[1];
  const PixelCluster & clus20 = *(*clus2Coll)[0];
  const PixelCluster & clus21 = *(*clus2Coll)[1];
  Identifier id10 = clus10.identify();
  Identifier id11 = clus11.identify();
  Identifier id20 = clus20.identify();
  Identifier id21 = clus21.identify();
  if( id10==id20 && (trk1->GetEta()==trk2->GetEta()) && (trk1->GetPhi()==trk2->GetPhi()) ) sameLayer0 = 1;
  if( id11==id21 && (trk1->GetEtaS()==trk2->GetEtaS()) && (trk1->GetPhiS()==trk2->GetPhiS()) ) sameLayer1 = 1;
  
  if( !sameLayer0 ) {
    gangeLayer0 = (clus10.rdoList().size()<clus20.rdoList().size()) ? 
      gangedNoiseClusEqualOrIncld(clus10,clus20):gangedNoiseClusEqualOrIncld(clus20,clus10);
  }
  if( !sameLayer1 ) {
    gangeLayer1 = (clus11.rdoList().size()<clus21.rdoList().size()) ?
      gangedNoiseClusEqualOrIncld(clus11,clus21):gangedNoiseClusEqualOrIncld(clus21,clus11);
  }
  
  // if( gangeLayer0 ) {
  //     std::vector<Identifier> rdo10 = clus10.rdoList();
  //     std::vector<Identifier> rdo20 = clus20.rdoList();
  //     log << MSG::DEBUG << "clus10:eta_index:phi_index ";
  //     for(unsigned int i0=0; i0<rdo10.size(); i0++) {
  //       Identifier idp = rdo10[i0];
  //       log << MSG::DEBUG << m_pixelID->eta_index(idp) << ":" << m_pixelID->phi_index(idp) << ",";
  //     }
  //     log << MSG::DEBUG << endreq;
  //     log << MSG::DEBUG << "clus20:eta_index:phi_index ";
  //     for(unsigned int i0=0; i0<rdo20.size(); i0++) {
  //       Identifier idp = rdo20[i0];
  //       log << MSG::DEBUG << m_pixelID->eta_index(idp) << ":" << m_pixelID->phi_index(idp) << ",";
  //     }
  //     log<< MSG::DEBUG <<endreq;
  //   }
  //   if( gangeLayer1 ) {
  //     std::vector<Identifier> rdo10 = clus11.rdoList();
  //     std::vector<Identifier> rdo20 = clus21.rdoList();
  //     log << MSG::DEBUG << "clus10:eta_index:phi_index ";
  //     for(unsigned int i0=0; i0<rdo10.size(); i0++) {
  //       Identifier idp = rdo10[i0];
  //       log << MSG::DEBUG << m_pixelID->eta_index(idp) << ":" << m_pixelID->phi_index(idp) << ",";
  //     }
  //     log << MSG::DEBUG << endreq;
  //     log << MSG::DEBUG << "clus20:eta_index:phi_index ";
  //     for(unsigned int i0=0; i0<rdo20.size(); i0++) {
  //       Identifier idp = rdo20[i0];
  //       log << MSG::DEBUG << m_pixelID->eta_index(idp) << ":" << m_pixelID->phi_index(idp) << ",";
  //     }
  //     log<< MSG::DEBUG <<endreq;
  //   }
  
  if( (sameLayer0&&gangeLayer1) ) {
    //if(0) ATH_MSG_DEBUG( "same0,gang1" );
    if( clus11.rdoList().size()<clus21.rdoList().size() ) return 2;
    else return 1;
  }
  if( (sameLayer0&&sameLayer1) ) {
    ATH_MSG_ERROR( "Two trklets have exactly the same clusters." );
    //     log << MSG::DEBUG << "size1:size2=" << clus1Coll->size() << ":" << clus2Coll->size() << endreq;
    //     log << MSG::DEBUG << "ganged10:ganged20=" << clus10.gangedPixel() << ":" << clus20.gangedPixel() << endreq;
    //     log << MSG::DEBUG << "ganged11:ganged21=" << clus11.gangedPixel() << ":" << clus21.gangedPixel() << endreq;
    //     log << MSG::DEBUG << "phi10:phi20=" << trk1->GetPhi() << ":" << trk2->GetPhi() << endreq;
    //     log << MSG::DEBUG << "phi11:phi21=" << trk1->GetPhiS() << ":" << trk2->GetPhiS() << endreq;
    return 1;
  }
  if( gangeLayer0&&sameLayer1 ) {
    //if(0) ATH_MSG_DEBUG( "gang0,same1" );
    if( clus10.rdoList().size()<clus20.rdoList().size() ) return 2;
    else return 1;
  }
  if( gangeLayer0&&gangeLayer1 ) {
    //if(0) ATH_MSG_DEBUG( "gang0,gang1" );
    if( clus10.rdoList().size()<clus20.rdoList().size() ) return 2;
    else if( clus10.rdoList().size()>clus20.rdoList().size() ) return 1;
    else {
      if( clus11.rdoList().size()<clus21.rdoList().size() ) return 2;
      else return 1;
    }
  }
  
  double eta10 = trk1->GetEta();
  double eta20 = trk2->GetEta();
  double phi10 = trk1->GetPhi();
  double phi20 = trk2->GetPhi();
  double deta = eta10 - eta20;
  double dphi = phi10 - phi20;
  if( dphi>M_PI ) dphi -= 2*M_PI;
  if( dphi<-M_PI ) dphi += 2*M_PI;
  double eta11 = trk1->GetEtaS();
  double phi11 = trk1->GetPhiS();
  double eta21 = trk2->GetEtaS();
  double phi21 = trk2->GetPhiS();
  double deta1 = eta11 - eta21;
  double dphi1 = phi11 - phi21;
  if( dphi1>M_PI ) dphi1 -= 2*M_PI;
  if( dphi1<-M_PI ) dphi1 += 2*M_PI;
  
  double ETAMERGEBOX0 = 0.01;
  double PHIMERGEBOX0 = 0.01;
  double ETAMERGEBOX1 = 0.01;
  double PHIMERGEBOX1 = 0.01;
  if( fabs(deta)<ETAMERGEBOX0 && fabs(dphi)<PHIMERGEBOX0 && fabs(deta1)<ETAMERGEBOX1 && fabs(dphi1)<PHIMERGEBOX1 ){
    //if( clus10.gangedPixel() && clus20.gangedPixel() ) gangeLayer0 = 1;
    //if( clus11.gangedPixel() && clus21.gangedPixel() ) gangeLayer1 = 1;
    int phi10M = m_pixelID->phi_module(id10);
    int eta10M = m_pixelID->eta_module(id10);
    int phi11M = m_pixelID->phi_module(id11);
    int eta11M = m_pixelID->eta_module(id11);
    int phi20M = m_pixelID->phi_module(id20);
    int eta20M = m_pixelID->eta_module(id20);
    int phi21M = m_pixelID->phi_module(id21);
    int eta21M = m_pixelID->eta_module(id21);
    if( !sameLayer0 )  if( phi10M!=phi20M || eta10M!=eta20M ) overlapLayer0 = 1;
    if( !sameLayer1 )  if( phi11M!=phi21M || eta11M!=eta21M ) overlapLayer1 = 1;
    double r10 = sqrt( clus10.globalPosition().x()*clus10.globalPosition().x() + clus10.globalPosition().y()*clus10.globalPosition().y() + clus10.globalPosition().z()*clus10.globalPosition().z() );
    double r20 = sqrt( clus20.globalPosition().x()*clus20.globalPosition().x() + clus20.globalPosition().y()*clus20.globalPosition().y() + clus20.globalPosition().z()*clus20.globalPosition().z() ); 
    double r11 = sqrt( clus11.globalPosition().x()*clus11.globalPosition().x() + clus11.globalPosition().y()*clus11.globalPosition().y() + clus11.globalPosition().z()*clus11.globalPosition().z() );
    double r21 = sqrt( clus21.globalPosition().x()*clus21.globalPosition().x() + clus21.globalPosition().y()*clus21.globalPosition().y() + clus21.globalPosition().z()*clus21.globalPosition().z() ); 
    
    if( (sameLayer0&&overlapLayer1)  ) {
      //if(0) ATH_MSG_DEBUG( "same0,overlap1" );
      if( r11>r21 ) return 2;
      else return 1;
    }
    if( gangeLayer0&&overlapLayer1 ) {
      //if(0) ATH_MSG_DEBUG( "gang0,overlap1" );
      if( clus10.rdoList().size()<clus20.rdoList().size() ) return 2;
      else if( clus10.rdoList().size()>clus20.rdoList().size() ) return 1;
      else {
	if( r11>r21 )  return 2;
	else return 1;
      }
    }
    if( overlapLayer0&&sameLayer1 ) {
      //if(0) ATH_MSG_DEBUG( "overlap0, same1" );
      if( r10>r20 ) return 2;
      else return 1;
    }
    if( overlapLayer0&&gangeLayer1 ) {
      //if(0) ATH_MSG_DEBUG( "overlap0,gang1" );
      if( r10>r20 ) return 2;
      else return 1;
    }
    if( overlapLayer0&&overlapLayer1 ) {
      //if(0) ATH_MSG_DEBUG( "overlap0,overlap1" );
      if( r10>r20 ) return 2;
      else return 1;
    }
    // if( gangeoverlapLayer0 || gangeLayer0 || overlapLayer1 || gangeLayer1 ) {
    //       whichOneToKeep = 1;
//       // if( clus10.gangedPixel() ) {
//       // 	if( clus10.rdoList().size()<clus20.rdoList().size() ) whichOneToKeep = 2;
//       // 	else whichOneToKeep = 1;
//       //       }
//       //       else whichOneToKeep = 1;
//       if( (gangeLayer0&&(clus10.rdoList().size()<clus20.rdoList().size())) || 
// 	  (gangeLayer1&&(clus11.rdoList().size()<clus21.rdoList().size())) ||
// 	  (overlapLayer0&&(r10>r20)) ||
// 	  (overlapLayer1&&(r11>r21)) )   
// 	whichOneToKeep = 2;
//     }
  } //satisfying box
  
  return 0;
}

//void HIPixelTrackletsMaker::removeDuplicateInCell(const std::vector<HIPixelTracklets*> & trkletsCollWithDup, std::vector<HIPixelTracklets*> & trkletsColl, int & hasDuplicate) {
  //log << MSG::DEBUG << "removeDuplicateInCell() begin! " << endreq;
 //  std::vector<int> removedTrklets;
//   for(unsigned int i=0; i<trkletsCollWithDup.size(); i++){
//     bool removed = false;
//     for(unsigned int ir=0; ir<removedTrklets.size(); ir++) {
//       if( ((int)i)==removedTrklets[ir] ) {
// 	removed = true;
// 	break;
//       }
//     }
//     if( removed ) continue;
//     HIPixelTracklets* trk1 = trkletsCollWithDup[i];
//     for(unsigned int j=i+1; j<trkletsCollWithDup.size(); j++) {
//       HIPixelTracklets* trk2 = trkletsCollWithDup[j];
//       int whichOneToKeep = checkGangedOrOverlap(trk1,trk2);
//       if(0) log << MSG::DEBUG << "whichOneToKeep: " << whichOneToKeep << endreq;
//       if( !whichOneToKeep ) continue;
//       if( whichOneToKeep==1 ) removedTrklets.push_back(j);
//       else if(whichOneToKeep==2 ) { removedTrklets.push_back(i); }
//       else { log<< MSG::ERROR << " returned value of checkGangedOrOverlap() function is wrong!" << endreq; }
//     }
//   }
//   if( removedTrklets.size() ) { 
//     hasDuplicate = 1;
//     for(unsigned int i=0; i<trkletsCollWithDup.size(); i++){
//       bool removed = false;
//       for(unsigned int ir=0; ir<removedTrklets.size(); ir++) {
// 	if( ((int)i)==removedTrklets[ir] ) {
// 	  removed = true;
// 	  break;
// 	}
//       }
//       if( removed ) continue;
//       trkletsColl.push_back(trkletsCollWithDup[i]);
//     }
//   }
//   else hasDuplicate = 0;

//}

//void HIPixelTrackletsMaker::removeDuplicateInNeighborCell(const std::vector<HIPixelTracklets*> & trackletsInCell, const std::vector<HIPixelTracklets*> & trackletsInNeighborCell, std::vector<HIPixelTracklets*> & newTrackletsInNeighborCell, int & hasDuplicate) {
  //log << MSG::DEBUG << "removeDuplicateInNeighborCell() begin!" << endreq;
//   hasDuplicate = 0;
//   std::vector<int> removedTrklets;
//   for(unsigned int ic=0; ic<trackletsInCell.size(); ic++) {
//     HIPixelTracklets* trk1 = trackletsInCell[ic];
//     for(unsigned int jn=0; jn<trackletsInNeighborCell.size(); jn++) {
//       HIPixelTracklets* trk2 = trackletsInNeighborCell[jn];
//       int whichOneToKeep = checkGangedOrOverlap(trk1,trk2);
//       if( !whichOneToKeep ) continue;
//       //has duplicate
//       hasDuplicate = 1;
//       removedTrklets.push_back(jn);
//     }
//   }
  
//   if( hasDuplicate ) {
//     for(unsigned int jn=0; jn<trackletsInNeighborCell.size(); jn++) {
//       bool removed = false;
//       for(unsigned int ir=0; ir<removedTrklets.size(); ir++) {
// 	if( ((int)jn)==removedTrklets[ir] ) {
// 	  removed = true;
// 	  break;
// 	}
//       }
//       if( removed ) continue;
//       newTrackletsInNeighborCell.push_back( trackletsInNeighborCell[jn] );
//     }
//   } //end of has duplicate

//}

void HIPixelTrackletsMaker::removeDuplicateLayerToLayer(int innerLayer, const std::map<int, std::vector<HIPixelTracklets*> > & toBeCleanedColl, std::map<int, std::vector<HIPixelTracklets*> > & referenceColl0, std::map<int, std::vector<HIPixelTracklets*> > & referenceColl1, HIPixelTrackletsCollection* trkcoll) {
  typedef std::map<int,std::vector<HIPixelTracklets*> >::const_iterator mapit;
  for(mapit it=toBeCleanedColl.begin(); it!=toBeCleanedColl.end(); it++) {
    int bin = it->first;
    std::vector<int> removedTrks;
    std::vector<HIPixelTracklets*> trksBin = it->second;
    for(unsigned int tbt=0; tbt<trksBin.size(); tbt++) {
      HIPixelTracklets* htrk = trksBin[tbt];
      //double etaIn = htrk->GetEta();
      double etaOut = htrk->GetEtaS();
      //double phiIn = htrk->GetPhi();
      double phiOut = htrk->GetPhiS();
      bool removed = false;
      int etaBin,phiBin;
      returnCorrespondingCell(bin,etaBin,phiBin);
      for(int ib=etaBin-1; ib<=etaBin+1; ib++) {
	if(removed) break;
	for(int jb=phiBin-1; jb<=phiBin+1; jb++) {
	  if(removed) break;
	  if(ib<0 || ib>m_nBinsEta-1 ) continue;  //edge in eta direction           
	  int neighborBin = ib + jb*m_nBinsEta;
	  if(jb<0 || jb>m_nBinsPhi-1 ) {    //edge in phi direction, need more care 
	    if( jb==-1 ) neighborBin = ib+ (m_nBinsPhi-1)*m_nBinsEta ;
	    else if ( jb==m_nBinsPhi ) neighborBin = ib;
	    else {
              ATH_MSG_ERROR( " phi bin out of boundary!!! " );
            }
	  }
	  //compare to reference one
	  std::vector<HIPixelTracklets*> trksRef0;
	  std::vector<HIPixelTracklets*> trksRef1;
	  mapit iref0 = referenceColl0.find(neighborBin);
	  mapit iref1 = referenceColl1.find(neighborBin);
	  if( iref0!=referenceColl0.end() ) 
	    trksRef0 = iref0->second;
	  if( iref1 != referenceColl1.end() )
	    trksRef1 = iref1->second;
	  
	  for(unsigned int it0=0; it0<trksRef0.size(); it0++) {
	    HIPixelTracklets* hrefi = trksRef0[it0];
	    double etaRef=hrefi->GetEta();
	    double phiRef=hrefi->GetPhi();
	    // if(innerLayer==0 ) {
// 	      etaRef = hrefi->GetEtaS();
// 	      phiRef = hrefi->GetPhiS();
// 	    }
	    // else { log<<MSG::ERROR << "input parameter for removeDuplicateLayerToLayer() is wrong! " << endreq; }
	    double deta = etaOut - etaRef;
	    double dphi = phiOut - phiRef;
	    if( dphi>M_PI )  dphi -= 2*M_PI;
	    if( dphi<-M_PI ) dphi += 2*M_PI;
// 	    if( innerLayer==0 ) {
// 	      deta = etaOut - etaRef;
// 	      dphi = (phiOut - phiRef);
// 	      if( dphi>M_PI )  dphi -= 2*M_PI;
// 	      if( dphi<-M_PI ) dphi += 2*M_PI;
// 	    }
	    double etaCutUsed = m_dEta;
	    double phiCutUsed = m_dPhi*(122.5-50.5)/(88.5-50.5);
	    if( fabs(deta)<2*etaCutUsed && fabs(dphi)<2*phiCutUsed ) {
	      removed = true;
	      //merge the two tracklets
	      hrefi->Merge(htrk);
	      //break;
	    }
	  }
	  
	  if(removed) break;
	  
	  for(unsigned int it1=0; it1<trksRef1.size(); it1++) {
            HIPixelTracklets* hrefi = trksRef1[it1];
            double etaRef=0.0;
	    double phiRef=0.0;
	    if( innerLayer==1 ) {
	      etaRef = hrefi->GetEta();
	      phiRef = hrefi->GetPhi();
	    }
	    double deta = etaOut - etaRef;
            double dphi = phiOut - phiRef;
            if( dphi>M_PI )  dphi -= 2*M_PI;
	    if( dphi<-M_PI ) dphi += 2*M_PI;
	    if( fabs(deta)<m_dEta && fabs(dphi)<m_dPhi ) {
              removed = true;
	      hrefi->Merge(htrk);
              //break;
            }
	  }
	  
	} //end of jb cell
      }//end of ib cell
      
      if( !removed ) trkcoll->push_back(htrk);
    } //end of to be checked tracklets collection
  } //end of to be checked tracklets map collection
  
}


double HIPixelTrackletsMaker::energyThresh(int layer, double eta) {
  double ene = -10000.;
  double cosheta = cosh(eta);
  if( layer==0 ) ene = -0.013+0.048*cosheta;
  if( layer==1 ) ene = -0.013+0.048*cosheta;
  if( layer==2 ) ene = -0.0095+0.042*cosheta;
  return ene;
}

//to test if pixsIds contained by pixsId1
bool HIPixelTrackletsMaker::gangedNoiseClus(const std::vector<IndexEtaPhi> & pixsId0, const std::vector<IndexEtaPhi> & pixsId1) {
  if( pixsId0.size() >= pixsId1.size() ) return false;
  for(unsigned ip=0; ip<pixsId0.size(); ip++) {
    IndexEtaPhi indexEtaPhi0 = pixsId0[ip];
    bool same = false;
    for(unsigned jp=0; jp<pixsId1.size(); jp++) {
      if( indexEtaPhi0==pixsId1[jp] ) {
	same = true;
	break;
      }
    }
    if( !same ) return false;
  }
  return true;
}

bool HIPixelTrackletsMaker::gangedNoiseClusEqualOrIncld(const PixelCluster & clus0, const PixelCluster & clus1) {
  if( clus0.rdoList().size()>clus1.rdoList().size() ) return false;
  Identifier id0 = clus0.identify();
  Identifier id1 = clus1.identify();
  if( m_pixelID->layer_disk(id0) != m_pixelID->layer_disk(id1) ) return false;
  if( m_pixelID->eta_module(id0) != m_pixelID->eta_module(id1) ) return false;
  if( m_pixelID->phi_module(id0) != m_pixelID->phi_module(id1) ) return false;
  
  std::vector<Identifier> rdoList[2];
  std::vector<IndexEtaPhi> pixsId[2];
  rdoList[0] = clus0.rdoList();
  rdoList[1] = clus1.rdoList();
  for(int i=0; i<2; i++) {
    for(unsigned int ir=0; ir<rdoList[i].size(); ir++) {
      Identifier idPix = rdoList[i][ir];
      int etaInd = m_pixelID->eta_index( idPix );
      int phiInd = m_pixelID->phi_index( idPix );
      IndexEtaPhi indexEtaPhi(etaInd, phiInd);
      pixsId[i].push_back(indexEtaPhi);
    }
  }
  for(unsigned ip=0; ip<pixsId[0].size(); ip++) {
    IndexEtaPhi indexEtaPhi0 = pixsId[0][ip];
    bool same = false;
    for(unsigned jp=0; jp<pixsId[1].size(); jp++) {
      if( indexEtaPhi0==pixsId[1][jp] ) {
        same = true;
        break;
      }
    }
    if( !same ) return false;
  }
  return true;
}

void HIPixelTrackletsMaker::removeGangedNoise(int layer, int arrSize0,
					      const std::map<int, std::vector<IndexEtaPhi> > gangedClusColl[][13]) {
  for(int i=0; i<arrSize0; i++) {
    for(int j=0; j<13; j++) {
      std::map<int,std::vector<IndexEtaPhi> > gangedClus = gangedClusColl[i][j];
      typedef std::map<int,std::vector<IndexEtaPhi> >::iterator mapItr;
      for(mapItr itr=gangedClus.begin(); itr!=gangedClus.end(); itr++) {
	int index0 = itr->first;
	mapiiitr itrIn = m_gangedNoiseLayer[layer].find(index0);
	if( itrIn!=m_gangedNoiseLayer[layer].end() && (itrIn->second)>0 ) continue;
	std::vector<IndexEtaPhi> pixsId0 = itr->second;
	bool noise0 = false;
	mapItr jtr = itr;
	jtr++;
	for( ;jtr!=gangedClus.end(); jtr++) {
	  int index1 = jtr->first;
	  //const PixelCluster& clusi1 = m_clusterLayer[layer][index1];
	  mapiiitr itrInj = m_gangedNoiseLayer[layer].find(index1);
	  if( itrInj!=m_gangedNoiseLayer[layer].end() && (itrInj->second)>0 ) continue;
	  std::vector<IndexEtaPhi> pixsId1 = jtr->second;
	  if( pixsId0.size() < pixsId1.size() ) {
	    noise0 = gangedNoiseClus(pixsId0,pixsId1);
	    if(noise0) break;
	  }
	  else if( pixsId0.size() > pixsId1.size() ) {
	    if( gangedNoiseClus(pixsId1,pixsId0) )  m_gangedNoiseLayer[layer][index1]++;
	  }
	}
	
	if(noise0) m_gangedNoiseLayer[layer][index0]++;
      } //end of phi module
    } //end of eta module
  }  //end of layer
  
}

void HIPixelTrackletsMaker::fittingParSigmaDeta(int tracklet, double eta, double & par0, double & par1) {
  //tracklet =0(01tracklet),1(02tracklet),2(12tracklet) 
  if( eta<0 ) eta *= -1.0;
  double p0[3] = {0.00272,0.002522,0.001683};
  double p1[3] = {-0.001157,-0.001403,-0.0005425};
  double p2[3] = {0.002417,0.005254,-0.0005558};
  double p3[3] = {-0.02762,-0.03725,-0.006897};
  double p4[3] = {0.09337,0.1118,0.03075};
  double p5[3] = {-0.1682,-0.1908,-0.06221};
  double p6[3] = {0.1747,0.1935,0.06942}; 
  double p7[3] = {-0.1028,-0.1132,-0.04284};
  double p8[3] = {0.03167,0.035,0.01363};
  double p9[3] = {-0.003953,-0.004407,-0.001742};
  double eta2 = eta*eta;
  double eta3 = eta2*eta;
  double eta4 = eta3*eta;
  double eta5 = eta4*eta;
  double eta6 = eta5*eta;
  double eta7 = eta6*eta;
  double eta8 = eta7*eta;
  double eta9 = eta8*eta;
  
  double sqrtpar0 = p0[tracklet] + p1[tracklet]*eta
    + p2[tracklet]*eta2 + p3[tracklet]*eta3
    + p4[tracklet]*eta4 + p5[tracklet]*eta5
    + p6[tracklet]*eta6 + p7[tracklet]*eta7
    + p8[tracklet]*eta8 + p9[tracklet]*eta9;
  
  par0 = sqrtpar0*sqrtpar0;
  
  double coshp0[3] = {0.0007189,0.001034,0.0005296};
  double coshp1[3] = {0.0003232,0.0005247,0.0002668};
  
  double sqrtpar1 = coshp0[tracklet] + coshp1[tracklet]*cosh(eta);
  par1 = sqrtpar1*sqrtpar1;
  
}

void HIPixelTrackletsMaker::setEdgePosition() {
  //Layer 0, non-middle eta_module
//   m_edgePositionLayer0[0][0]= HepGeom::Point3D<double>(47.151,-7.51194,0);
//   m_edgePositionLayer0[0][1]= HepGeom::Point3D<double>(47.3574,6.07621,0);
//   m_edgePositionLayer0[0][2]= HepGeom::Point3D<double>(43.7272,19.1723,0);
//   m_edgePositionLayer0[0][3]= HepGeom::Point3D<double>(36.5544,30.715,0);
//   m_edgePositionLayer0[0][4]= HepGeom::Point3D<double>(26.4203,39.7694,0);
//   m_edgePositionLayer0[0][5]= HepGeom::Point3D<double>(14.1457,45.602,0);
//   m_edgePositionLayer0[0][6]= HepGeom::Point3D<double>(0.725206,47.7401,0);
//   m_edgePositionLayer0[0][7]= HepGeom::Point3D<double>(-12.7539,46.0106,0);
//   m_edgePositionLayer0[0][8]= HepGeom::Point3D<double>(-25.2002,40.5536,0);
//   m_edgePositionLayer0[0][9]= HepGeom::Point3D<double>(-35.6045,31.8113,0);
//   m_edgePositionLayer0[0][10]= HepGeom::Point3D<double>(-43.1247,20.4915,0);
//   m_edgePositionLayer0[0][11]= HepGeom::Point3D<double>(-47.151,7.51173,0);
//   m_edgePositionLayer0[0][12]= HepGeom::Point3D<double>(-47.3669,-6.07564,0);
//   m_edgePositionLayer0[0][13]= HepGeom::Point3D<double>(-43.7272,-19.1723,0);
//   m_edgePositionLayer0[0][14]= HepGeom::Point3D<double>(-36.5544,-30.7151,0);
//   m_edgePositionLayer0[0][15]= HepGeom::Point3D<double>(-26.4203,-39.7694,0);
//   m_edgePositionLayer0[0][16]= HepGeom::Point3D<double>(-14.1458,-45.6019,0);
//   m_edgePositionLayer0[0][17]= HepGeom::Point3D<double>(-0.700912,-47.7452,0);
//   m_edgePositionLayer0[0][18]= HepGeom::Point3D<double>(12.754,-46.0106,0);
//   m_edgePositionLayer0[0][19]= HepGeom::Point3D<double>(25.2,-40.5536,0);
//   m_edgePositionLayer0[0][20]= HepGeom::Point3D<double>(35.6046,-31.8112,0);
//   m_edgePositionLayer0[0][21]= HepGeom::Point3D<double>(43.1394,-20.4716,0);
  //eta module =0
  if( m_doMC) {
    m_edgePositionLayer0[6][0]= HepGeom::Point3D<double>(47.1301,-7.50448,0);
    m_edgePositionLayer0[6][1]= HepGeom::Point3D<double>(47.3352,6.07764,0);
    m_edgePositionLayer0[6][2]= HepGeom::Point3D<double>(43.7056,19.1673,0);
    m_edgePositionLayer0[6][3]= HepGeom::Point3D<double>(36.5351,30.7043,0);
    m_edgePositionLayer0[6][4]= HepGeom::Point3D<double>(26.4048,39.7536,0);
    m_edgePositionLayer0[6][5]= HepGeom::Point3D<double>(14.1353,45.5824,0);
    m_edgePositionLayer0[6][6]= HepGeom::Point3D<double>(0.720779,47.7184,0);
    m_edgePositionLayer0[6][7]= HepGeom::Point3D<double>(-12.7475,45.9889,0);
    m_edgePositionLayer0[6][8]= HepGeom::Point3D<double>(-25.1921,40.533,0);
    m_edgePositionLayer0[6][9]= HepGeom::Point3D<double>(-35.5911,31.7936,0);
    m_edgePositionLayer0[6][10]= HepGeom::Point3D<double>(-43.1068,20.4785,0);
    m_edgePositionLayer0[6][11]= HepGeom::Point3D<double>(-47.1301,7.50434,0);
    m_edgePositionLayer0[6][12]= HepGeom::Point3D<double>(-47.3352,-6.07761,0);
    m_edgePositionLayer0[6][13]= HepGeom::Point3D<double>(-43.7056,-19.1674,0);
    m_edgePositionLayer0[6][14]= HepGeom::Point3D<double>(-36.5544,-30.7151,0);
    m_edgePositionLayer0[6][15]= HepGeom::Point3D<double>(-26.4048,-39.7536,0);
    m_edgePositionLayer0[6][16]= HepGeom::Point3D<double>(-14.1355,-45.5823,0);
    m_edgePositionLayer0[6][17]= HepGeom::Point3D<double>(-0.720828,-47.7184,0);
    m_edgePositionLayer0[6][18]= HepGeom::Point3D<double>(12.753,-45.9884,0);
    m_edgePositionLayer0[6][19]= HepGeom::Point3D<double>(25.1922,-40.5329,0);
    m_edgePositionLayer0[6][20]= HepGeom::Point3D<double>(35.5911,-31.7936,0);
    m_edgePositionLayer0[6][21]= HepGeom::Point3D<double>(43.1067,-20.4786,0);
  }
  else {
    m_edgePositionLayer0[6][0]= HepGeom::Point3D<double>(46.8564,-7.98204,0);
    m_edgePositionLayer0[6][1]= HepGeom::Point3D<double>(46.8041,5.95713,0);
    m_edgePositionLayer0[6][2]= HepGeom::Point3D<double>(43.231,18.9177,0);
    m_edgePositionLayer0[6][3]= HepGeom::Point3D<double>(35.9599,30.1234,0);
    m_edgePositionLayer0[6][4]= HepGeom::Point3D<double>(26.0242,39.1681,0);
    m_edgePositionLayer0[6][5]= HepGeom::Point3D<double>(13.2229,45.0432,0);
    m_edgePositionLayer0[6][6]= HepGeom::Point3D<double>(-0.0277846,47.196,0);
    m_edgePositionLayer0[6][7]= HepGeom::Point3D<double>(-13.8027,45.171,0);
    m_edgePositionLayer0[6][8]= HepGeom::Point3D<double>(-25.6482,40.0084,0);
    m_edgePositionLayer0[6][9]= HepGeom::Point3D<double>(-36.2607,30.6589,0);
    m_edgePositionLayer0[6][10]= HepGeom::Point3D<double>(-43.7151,19.6881,0);
    m_edgePositionLayer0[6][11]= HepGeom::Point3D<double>(-47.6206,6.83068,0);
    m_edgePositionLayer0[6][12]= HepGeom::Point3D<double>(-47.8763,-7.01871,0);
    m_edgePositionLayer0[6][13]= HepGeom::Point3D<double>(-43.8981,-19.9827,0);
    m_edgePositionLayer0[6][14]= HepGeom::Point3D<double>(-37.9635,-32.0132,0);
    m_edgePositionLayer0[6][15]= HepGeom::Point3D<double>(-26.3648,-40.5608,0);
    m_edgePositionLayer0[6][16]= HepGeom::Point3D<double>(-14.7199,-46.2391,0);
    m_edgePositionLayer0[6][17]= HepGeom::Point3D<double>(-0.782656,-48.2718,0);
    m_edgePositionLayer0[6][18]= HepGeom::Point3D<double>(12.8671,-46.5442,0);
    m_edgePositionLayer0[6][19]= HepGeom::Point3D<double>(24.9859,-40.946,0);
    m_edgePositionLayer0[6][20]= HepGeom::Point3D<double>(35.3492,-32.3925,0);
    m_edgePositionLayer0[6][21]= HepGeom::Point3D<double>(42.7465,-20.9552,0);
  }

  for(int ie=0; ie<13; ie++) {
    if(ie==6) continue;
    for(int ip=0; ip<22; ip++) {
      m_edgePositionLayer0[ie][ip] = m_edgePositionLayer0[6][ip];
    }
  }
  //special care for disabled modules, subject to change
  m_edgePositionLayer0[2][5] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer0[7][7] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer0[7][8] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer0[9][6] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer0[9][13] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer0[6][14]= HepGeom::Point3D<double>(0,0,0);

  //Layer 1, non-middle eta modules
//   m_edgePositionLayer1[0][0] = HepGeom::Point3D<double>(85.151,-7.51192,0);
//   m_edgePositionLayer1[0][1] = HepGeom::Point3D<double>(85.2261,6.60601,0);
//   m_edgePositionLayer1[0][2] = HepGeom::Point3D<double>(82.9764,20.5438,0);
//   m_edgePositionLayer1[0][3] = HepGeom::Point3D<double>(78.4633,33.9208,0);
//   m_edgePositionLayer1[0][4] = HepGeom::Point3D<double>(71.81,46.3728,0);
//   m_edgePositionLayer1[0][5] = HepGeom::Point3D<double>(63.2064,57.5643,0);
//   m_edgePositionLayer1[0][6] = HepGeom::Point3D<double>(52.8319,67.217,0);
//   m_edgePositionLayer1[0][7] = HepGeom::Point3D<double>(41.0298,75.0132,0);
//   m_edgePositionLayer1[0][8] = HepGeom::Point3D<double>(28.1648,80.7153,0);
//   m_edgePositionLayer1[0][9] = HepGeom::Point3D<double>(14.5176,84.24,0);
//   m_edgePositionLayer1[0][10] = HepGeom::Point3D<double>(0.454489,85.4805,0);
//   m_edgePositionLayer1[0][11] = HepGeom::Point3D<double>(-13.6212,84.3894,0);
//   m_edgePositionLayer1[0][12] = HepGeom::Point3D<double>(-27.3255,80.9965,0);
//   m_edgePositionLayer1[0][13] = HepGeom::Point3D<double>(-40.2844,75.3942,0);
//   m_edgePositionLayer1[0][14] = HepGeom::Point3D<double>(-52.1444,67.7353,0);
//   m_edgePositionLayer1[0][15] = HepGeom::Point3D<double>(-62.5822,58.2288,0);
//   m_edgePositionLayer1[0][16] = HepGeom::Point3D<double>(-71.3128,47.1338,0);
//   m_edgePositionLayer1[0][17] = HepGeom::Point3D<double>(-78.1138,34.734,0);
//   m_edgePositionLayer1[0][18] = HepGeom::Point3D<double>(-82.7533,21.4247,0);
//   m_edgePositionLayer1[0][19] = HepGeom::Point3D<double>(-85.151,7.51188,0);
//   m_edgePositionLayer1[0][20] = HepGeom::Point3D<double>(-85.2392,-6.60343,0);
//   m_edgePositionLayer1[0][21] = HepGeom::Point3D<double>(-82.9764,-20.5436,0);
//   m_edgePositionLayer1[0][22] = HepGeom::Point3D<double>(-78.4633,-33.9208,0);
//   m_edgePositionLayer1[0][23] = HepGeom::Point3D<double>(-71.8099,-46.373,0);
//   m_edgePositionLayer1[0][24] = HepGeom::Point3D<double>(-63.1979,-57.5599,0);
//   m_edgePositionLayer1[0][25] = HepGeom::Point3D<double>(-52.862,-67.1768,0);
//   m_edgePositionLayer1[0][26] = HepGeom::Point3D<double>(-41.084,-74.9614,0);
//   m_edgePositionLayer1[0][27] = HepGeom::Point3D<double>(-28.1909,-80.7091,0);
//   m_edgePositionLayer1[0][28] = HepGeom::Point3D<double>(-14.5179,-84.2398,0);
//   m_edgePositionLayer1[0][29] = HepGeom::Point3D<double>(-0.454564,-85.4805,0);
//   m_edgePositionLayer1[0][30] = HepGeom::Point3D<double>(13.6213,-84.3894,0);
//   m_edgePositionLayer1[0][31] = HepGeom::Point3D<double>(27.3256,-80.9965,0);
//   m_edgePositionLayer1[0][32] = HepGeom::Point3D<double>(40.2843,-75.3942,0);
//   m_edgePositionLayer1[0][33] = HepGeom::Point3D<double>(52.1444,-67.7353,0);
//   m_edgePositionLayer1[0][34] = HepGeom::Point3D<double>(62.5871,-58.237,0);
//   m_edgePositionLayer1[0][35] = HepGeom::Point3D<double>(71.3127,-47.134,0);
//   m_edgePositionLayer1[0][36] = HepGeom::Point3D<double>(78.1056,-34.7594,0);
//   m_edgePositionLayer1[0][37] = HepGeom::Point3D<double>(82.7655,-21.4031,0);
  //eta module==0
  if( m_doMC ) {
    m_edgePositionLayer1[6][0] = HepGeom::Point3D<double>(85.1301,-7.50431,0);
    m_edgePositionLayer1[6][1] = HepGeom::Point3D<double>(85.2042,6.60997,0);
    m_edgePositionLayer1[6][2] = HepGeom::Point3D<double>(82.9542,20.5441,0);
    m_edgePositionLayer1[6][3] = HepGeom::Point3D<double>(78.4414,33.9175,0);
    m_edgePositionLayer1[6][4] = HepGeom::Point3D<double>(71.7889,46.366,0);
    m_edgePositionLayer1[6][5] = HepGeom::Point3D<double>(63.1782,57.5497,0);
    m_edgePositionLayer1[6][6] = HepGeom::Point3D<double>(52.8441,67.1637,0);
    m_edgePositionLayer1[6][7] = HepGeom::Point3D<double>(41.0687,74.9454,0);
    m_edgePositionLayer1[6][8] = HepGeom::Point3D<double>(28.1648,80.7153,0);
    m_edgePositionLayer1[6][9] = HepGeom::Point3D<double>(14.5085,84.2197,0);
    m_edgePositionLayer1[6][10] = HepGeom::Point3D<double>(0.44865,85.4591,0);
    m_edgePositionLayer1[6][11] = HepGeom::Point3D<double>(-13.6234,84.3674,0);
    m_edgePositionLayer1[6][12] = HepGeom::Point3D<double>(-27.3241,80.9744,0);
    m_edgePositionLayer1[6][13] = HepGeom::Point3D<double>(-40.2793,75.3726,0);
    m_edgePositionLayer1[6][14] = HepGeom::Point3D<double>(-52.1359,67.7148,0);
    m_edgePositionLayer1[6][15] = HepGeom::Point3D<double>(-62.5704,58.21,0);
    m_edgePositionLayer1[6][16] = HepGeom::Point3D<double>(-71.2981,47.1173,0);
    m_edgePositionLayer1[6][17] = HepGeom::Point3D<double>(-78.0809,34.7395,0);
    m_edgePositionLayer1[6][18] = HepGeom::Point3D<double>(-82.7339,21.4139,0);
    m_edgePositionLayer1[6][19] = HepGeom::Point3D<double>(-85.1301,7.50432,0);
    m_edgePositionLayer1[6][20] = HepGeom::Point3D<double>(-85.2042,-6.60989,0);
    m_edgePositionLayer1[6][21] = HepGeom::Point3D<double>(-82.9542,-20.544,0);
    m_edgePositionLayer1[6][22] = HepGeom::Point3D<double>(-78.4414,-33.9176,0);
    m_edgePositionLayer1[6][23] = HepGeom::Point3D<double>(-71.7889,-46.366,0);
    m_edgePositionLayer1[6][24] = HepGeom::Point3D<double>(-63.1782,-57.5496,0);
    m_edgePositionLayer1[6][25] = HepGeom::Point3D<double>(-52.8442,-67.1635,0);
    m_edgePositionLayer1[6][26] = HepGeom::Point3D<double>(-41.0687,-74.9454,0);
    m_edgePositionLayer1[6][27] = HepGeom::Point3D<double>(-28.1732,-80.6828,0);
    m_edgePositionLayer1[6][28] = HepGeom::Point3D<double>(-14.5088,-84.2196,0);
    m_edgePositionLayer1[6][29] = HepGeom::Point3D<double>(-0.44879,-85.4591,0);
    m_edgePositionLayer1[6][30] = HepGeom::Point3D<double>(13.6235,-84.3674,0);
    m_edgePositionLayer1[6][31] = HepGeom::Point3D<double>(27.324,-80.9744,0);
    m_edgePositionLayer1[6][32] = HepGeom::Point3D<double>(40.2793,-75.3726,0);
    m_edgePositionLayer1[6][33] = HepGeom::Point3D<double>(52.1359,-67.7149,0);
    m_edgePositionLayer1[6][34] = HepGeom::Point3D<double>(62.5704,-58.21,0);
    m_edgePositionLayer1[6][35] = HepGeom::Point3D<double>(71.2979,-47.1174,0);
    m_edgePositionLayer1[6][36] = HepGeom::Point3D<double>(78.0809,-34.7395,0);
    m_edgePositionLayer1[6][37] = HepGeom::Point3D<double>(82.7339,-21.414,0);
  }
  else {
    m_edgePositionLayer1[6][0] = HepGeom::Point3D<double>(88.2536,-0.0648348,0);
    m_edgePositionLayer1[6][1] = HepGeom::Point3D<double>(84.9916,7.42634,0);
    m_edgePositionLayer1[6][2] = HepGeom::Point3D<double>(82.5558,20.4391,0);
    m_edgePositionLayer1[6][3] = HepGeom::Point3D<double>(75.4826,49.1505,0);
    m_edgePositionLayer1[6][4] = HepGeom::Point3D<double>(71.0565,46.3127,0);
    m_edgePositionLayer1[6][5] = HepGeom::Point3D<double>(59.481,62.917,0);
    m_edgePositionLayer1[6][6] = HepGeom::Point3D<double>(51.6186,67.6636,0);
    m_edgePositionLayer1[6][7] = HepGeom::Point3D<double>(39.471,75.3923,0);
    m_edgePositionLayer1[6][8] = HepGeom::Point3D<double>(26.5885,81.0676,0);
    m_edgePositionLayer1[6][9] = HepGeom::Point3D<double>(13.5388,83.9716,0);
    m_edgePositionLayer1[6][10] = HepGeom::Point3D<double>(-5.19304,86.3047,0);
    m_edgePositionLayer1[6][11] = HepGeom::Point3D<double>(-14.7654,83.8697,0);
    m_edgePositionLayer1[6][12] = HepGeom::Point3D<double>(-29.6876,80.2638,0);
    m_edgePositionLayer1[6][13] = HepGeom::Point3D<double>(-41.7104,75.6461,0);
    m_edgePositionLayer1[6][14] = HepGeom::Point3D<double>(-62.8931,62.8274,0);
    m_edgePositionLayer1[6][15] = HepGeom::Point3D<double>(-63.6759,57.3297,0);
    m_edgePositionLayer1[6][16] = HepGeom::Point3D<double>(-72.6837,45.8944,0);
    m_edgePositionLayer1[6][17] = HepGeom::Point3D<double>(-78.9991,33.6067,0);
    m_edgePositionLayer1[6][18] = HepGeom::Point3D<double>(-83.2075,20.5137,0);
    m_edgePositionLayer1[6][19] = HepGeom::Point3D<double>(-87.278,2.2241,0);
    m_edgePositionLayer1[6][20] = HepGeom::Point3D<double>(-86.0763,-9.72429,0);
    m_edgePositionLayer1[6][21] = HepGeom::Point3D<double>(-83.4003,-27.8838,0);
    m_edgePositionLayer1[6][22] = HepGeom::Point3D<double>(-78.258,-37.1023,0);
    m_edgePositionLayer1[6][23] = HepGeom::Point3D<double>(-67.8643,-60.217,0);
    m_edgePositionLayer1[6][24] = HepGeom::Point3D<double>(-61.6332,-62.0703,0);
    m_edgePositionLayer1[6][25] = HepGeom::Point3D<double>(-47.4107,-75.2182,0);
    m_edgePositionLayer1[6][26] = HepGeom::Point3D<double>(-40.5112,-76.1884,0);
    m_edgePositionLayer1[6][27] = HepGeom::Point3D<double>(-28.4876,-81.379,0);
    m_edgePositionLayer1[6][28] = HepGeom::Point3D<double>(-14.0656,-85.1733,0);
    m_edgePositionLayer1[6][29] = HepGeom::Point3D<double>(2.36216,-87.5249,0);
    m_edgePositionLayer1[6][30] = HepGeom::Point3D<double>(17.0897,-85.1494,0);
    m_edgePositionLayer1[6][31] = HepGeom::Point3D<double>(29.7864,-81.5057,0);
    m_edgePositionLayer1[6][32] = HepGeom::Point3D<double>(45.4551,-74.4848,0);
    m_edgePositionLayer1[6][33] = HepGeom::Point3D<double>(56.0132,-66.3999,0);
    m_edgePositionLayer1[6][34] = HepGeom::Point3D<double>(62.7862,-58.3066,0);
    m_edgePositionLayer1[6][35] = HepGeom::Point3D<double>(74.3825,-44.5011,0);
    m_edgePositionLayer1[6][36] = HepGeom::Point3D<double>(77.8396,-35.1067,0);
    m_edgePositionLayer1[6][37] = HepGeom::Point3D<double>(83.4305,-19.9293,0);
  }
  for(int ie=0;ie<13;ie++) {
    if( ie==6 ) continue;
    for(int ip=0;ip<38;ip++) {
      m_edgePositionLayer1[ie][ip] = m_edgePositionLayer1[6][ip];
    }
  }
  m_edgePositionLayer1[1][27] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer1[3][34] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer1[5][18] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer1[6][8] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer1[6][13] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer1[7][27] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer1[11][9] = HepGeom::Point3D<double>(0,0,0);
  
  // //layer 2
//   m_edgePositionLayer2[0][0] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][1] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][2] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][3] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][4] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][5] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][6] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][7] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][8] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][9] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][10] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][11] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][12] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][13] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][14] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][15] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][16] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][17] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][18] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][19] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][20] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][21] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][22] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][23] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][24] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][25] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][26] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][27] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][28] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][29] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][30] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][31] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][32] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][33] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][34] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][35] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][36] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][37] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][38] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][39] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][40] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][41] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][42] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][43] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][44] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][45] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][46] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][47] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][48] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][49] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][50] = HepGeom::Point3D<double>(,0);
//   m_edgePositionLayer2[0][51] = HepGeom::Point3D<double>(,0);
//   for(int ie=1;ie<13;ie++) {
  //if(ie==6) continue;
  //     for(int ip=0;ip<52;ip++) {
//       m_edgePositionLayer2[ie][ip] = m_edgePositionLayer2[0][ip];
//     }
//   }

  //eta module==0
  if( m_doMC ) {
    m_edgePositionLayer2[6][0] = HepGeom::Point3D<double>(119.366,-0.306937,0);
    m_edgePositionLayer2[6][1] = HepGeom::Point3D<double>(118.533,14.0836,0);
    m_edgePositionLayer2[6][2] = HepGeom::Point3D<double>(115.971,28.2675,0);
    m_edgePositionLayer2[6][3] = HepGeom::Point3D<double>(111.718,42.0403,0);
    m_edgePositionLayer2[6][4] = HepGeom::Point3D<double>(105.836,55.2006,0);
    m_edgePositionLayer2[6][5] = HepGeom::Point3D<double>(98.4108,67.5545,0);
    m_edgePositionLayer2[6][6] = HepGeom::Point3D<double>(89.5503,78.9245,0);
    m_edgePositionLayer2[6][7] = HepGeom::Point3D<double>(79.3839,89.1433,0);
    m_edgePositionLayer2[6][8] = HepGeom::Point3D<double>(68.0605,98.0614,0);
    m_edgePositionLayer2[6][9] = HepGeom::Point3D<double>(55.7415,105.584,0);
    m_edgePositionLayer2[6][10] = HepGeom::Point3D<double>(42.6152,111.5,0);
    m_edgePositionLayer2[6][11] = HepGeom::Point3D<double>(28.8646,115.824,0);
    m_edgePositionLayer2[6][12] = HepGeom::Point3D<double>(14.6926,118.459,0);
    m_edgePositionLayer2[6][13] = HepGeom::Point3D<double>(0.313413,119.387,0);
    m_edgePositionLayer2[6][14] = HepGeom::Point3D<double>(-14.0828,118.533,0);
    m_edgePositionLayer2[6][15] = HepGeom::Point3D<double>(-28.2676,115.971,0);
    m_edgePositionLayer2[6][16] = HepGeom::Point3D<double>(-42.0411,111.718,0);
    m_edgePositionLayer2[6][17] = HepGeom::Point3D<double>(-55.1998,105.836,0);
    m_edgePositionLayer2[6][18] = HepGeom::Point3D<double>(-67.555,98.4106,0);
    m_edgePositionLayer2[6][19] = HepGeom::Point3D<double>(-78.9244,89.5503,0);
    m_edgePositionLayer2[6][20] = HepGeom::Point3D<double>(-89.1428,79.3843,0);
    m_edgePositionLayer2[6][21] = HepGeom::Point3D<double>(-98.0622,68.0599,0);
    m_edgePositionLayer2[6][22] = HepGeom::Point3D<double>(-105.55,55.7442,0);
    m_edgePositionLayer2[6][23] = HepGeom::Point3D<double>(-111.5,42.6151,0);
    m_edgePositionLayer2[6][24] = HepGeom::Point3D<double>(-115.824,28.8642,0);
    m_edgePositionLayer2[6][25] = HepGeom::Point3D<double>(-118.459,14.6928,0);
    m_edgePositionLayer2[6][26] = HepGeom::Point3D<double>(-119.366,0.30746,0);
    m_edgePositionLayer2[6][27] = HepGeom::Point3D<double>(-118.554,-14.0789,0);
    m_edgePositionLayer2[6][28] = HepGeom::Point3D<double>(-115.971,-28.2681,0);
    m_edgePositionLayer2[6][29] = HepGeom::Point3D<double>(-111.718,-42.0402,0);
    m_edgePositionLayer2[6][30] = HepGeom::Point3D<double>(-105.836,-55.2003,0);
    m_edgePositionLayer2[6][31] = HepGeom::Point3D<double>(-98.4108,-67.5545,0);
    m_edgePositionLayer2[6][32] = HepGeom::Point3D<double>(-89.5504,-78.9242,0);
    m_edgePositionLayer2[6][33] = HepGeom::Point3D<double>(-79.3843,-89.1428,0);
    m_edgePositionLayer2[6][34] = HepGeom::Point3D<double>(-68.0601,-98.062,0);
    m_edgePositionLayer2[6][35] = HepGeom::Point3D<double>(-55.7441,-105.55,0);
    m_edgePositionLayer2[6][36] = HepGeom::Point3D<double>(-42.6084,-111.533,0);
    m_edgePositionLayer2[6][37] = HepGeom::Point3D<double>(-28.8647,-115.824,0);
    m_edgePositionLayer2[6][38] = HepGeom::Point3D<double>(-14.6925,-118.459,0);
    m_edgePositionLayer2[6][39] = HepGeom::Point3D<double>(-0.307489,-119.366,0);
    m_edgePositionLayer2[6][40] = HepGeom::Point3D<double>(14.0831,-118.533,0);
    m_edgePositionLayer2[6][41] = HepGeom::Point3D<double>(28.2675,-115.971,0);
    m_edgePositionLayer2[6][42] = HepGeom::Point3D<double>(42.0402,-111.718,0);
    m_edgePositionLayer2[6][43] = HepGeom::Point3D<double>(55.2046,-105.858,0);
    m_edgePositionLayer2[6][44] = HepGeom::Point3D<double>(67.5545,-98.4108,0);
    m_edgePositionLayer2[6][45] = HepGeom::Point3D<double>(78.9245,-89.5503,0);
    m_edgePositionLayer2[6][46] = HepGeom::Point3D<double>(89.1428,-79.3843,0);
    m_edgePositionLayer2[6][47] = HepGeom::Point3D<double>(98.0619,-68.0601,0);
    m_edgePositionLayer2[6][48] = HepGeom::Point3D<double>(105.55,-55.7443,0);
    m_edgePositionLayer2[6][49] = HepGeom::Point3D<double>(111.5,-42.6147,0);
    m_edgePositionLayer2[6][50] = HepGeom::Point3D<double>(115.824,-28.8646,0);
    m_edgePositionLayer2[6][51] = HepGeom::Point3D<double>(118.458,-14.6933,0);
  }
  else {
    m_edgePositionLayer2[6][0] = HepGeom::Point3D<double>(118.856,-0.141466,0);
    m_edgePositionLayer2[6][1] = HepGeom::Point3D<double>(118.296,14.9092,0);
    m_edgePositionLayer2[6][2] = HepGeom::Point3D<double>(115.617,29.4153,0);
    m_edgePositionLayer2[6][3] = HepGeom::Point3D<double>(110.79,47.7475,0);
    m_edgePositionLayer2[6][4] = HepGeom::Point3D<double>(104.331,58.8077,0);
    m_edgePositionLayer2[6][5] = HepGeom::Point3D<double>(97.1683,69.6358,0);
    m_edgePositionLayer2[6][6] = HepGeom::Point3D<double>(87.2682,82.3367,0);
    m_edgePositionLayer2[6][7] = HepGeom::Point3D<double>(78.2927,89.6121,0);
    m_edgePositionLayer2[6][8] = HepGeom::Point3D<double>(67.3514,98.0421,0);
    m_edgePositionLayer2[6][9] = HepGeom::Point3D<double>(47.216,113.437,0);
    m_edgePositionLayer2[6][10] = HepGeom::Point3D<double>(39.7942,112.709,0);
    m_edgePositionLayer2[6][11] = HepGeom::Point3D<double>(23.1996,118.484,0);
    m_edgePositionLayer2[6][12] = HepGeom::Point3D<double>(7.58601,120.805,0);
    m_edgePositionLayer2[6][13] = HepGeom::Point3D<double>(-13.0882,123.443,0);
    m_edgePositionLayer2[6][14] = HepGeom::Point3D<double>(-17.0891,118.43,0);
    m_edgePositionLayer2[6][15] = HepGeom::Point3D<double>(-35.7345,115.685,0);
    m_edgePositionLayer2[6][16] = HepGeom::Point3D<double>(-50.4536,110.454,0);
    m_edgePositionLayer2[6][17] = HepGeom::Point3D<double>(-59.3522,104.547,0);
    m_edgePositionLayer2[6][18] = HepGeom::Point3D<double>(-73.7116,95.8951,0);
    m_edgePositionLayer2[6][19] = HepGeom::Point3D<double>(-81.0365,88.2693,0);
    m_edgePositionLayer2[6][20] = HepGeom::Point3D<double>(-93.3152,76.5248,0);
    m_edgePositionLayer2[6][21] = HepGeom::Point3D<double>(-107.729,59.9585,0);
    m_edgePositionLayer2[6][22] = HepGeom::Point3D<double>(-106.634,54.2148,0);
    m_edgePositionLayer2[6][23] = HepGeom::Point3D<double>(-115.171,37.4616,0);
    m_edgePositionLayer2[6][24] = HepGeom::Point3D<double>(-119.926,21.5596,0);
    m_edgePositionLayer2[6][25] = HepGeom::Point3D<double>(-120.529,9.89077,0);
    m_edgePositionLayer2[6][26] = HepGeom::Point3D<double>(-119.749,-0.797685,0);
    m_edgePositionLayer2[6][27] = HepGeom::Point3D<double>(-121.447,-27.441,0);
    m_edgePositionLayer2[6][28] = HepGeom::Point3D<double>(-116.499,-35.4377,0);
    m_edgePositionLayer2[6][29] = HepGeom::Point3D<double>(-111.044,-56.4284,0);
    m_edgePositionLayer2[6][30] = HepGeom::Point3D<double>(-105.61,-57.7632,0);
    m_edgePositionLayer2[6][31] = HepGeom::Point3D<double>(-96.8937,-73.6257,0);
    m_edgePositionLayer2[6][32] = HepGeom::Point3D<double>(-86.2768,-86.9695,0);
    m_edgePositionLayer2[6][33] = HepGeom::Point3D<double>(-76.2225,-95.3128,0);
    m_edgePositionLayer2[6][34] = HepGeom::Point3D<double>(-66.186,-101.223,0);
    m_edgePositionLayer2[6][35] = HepGeom::Point3D<double>(-50.0731,-111.886,0);
    m_edgePositionLayer2[6][36] = HepGeom::Point3D<double>(-40.1943,-115.295,0);
    m_edgePositionLayer2[6][37] = HepGeom::Point3D<double>(-23.3139,-119.68,0);
    m_edgePositionLayer2[6][38] = HepGeom::Point3D<double>(-13.7459,-119.273,0);
    m_edgePositionLayer2[6][39] = HepGeom::Point3D<double>(7.57669,-122.22,0);
    m_edgePositionLayer2[6][40] = HepGeom::Point3D<double>(17.8084,-119.561,0);
    m_edgePositionLayer2[6][41] = HepGeom::Point3D<double>(30.7296,-116.588,0);
    m_edgePositionLayer2[6][42] = HepGeom::Point3D<double>(45.3921,-111.939,0);
    m_edgePositionLayer2[6][43] = HepGeom::Point3D<double>(62.4298,-105.068,0);
    m_edgePositionLayer2[6][44] = HepGeom::Point3D<double>(68.2626,-98.5485,0);
    m_edgePositionLayer2[6][45] = HepGeom::Point3D<double>(81.7677,-88.5954,0);
    m_edgePositionLayer2[6][46] = HepGeom::Point3D<double>(95.6387,-75.5985,0);
    m_edgePositionLayer2[6][47] = HepGeom::Point3D<double>(102.672,-64.4395,0);
    m_edgePositionLayer2[6][48] = HepGeom::Point3D<double>(116.59,-44.3766,0);
    m_edgePositionLayer2[6][49] = HepGeom::Point3D<double>(111.389,-42.3308,0);
    m_edgePositionLayer2[6][50] = HepGeom::Point3D<double>(116.267,-27.736,0);
    m_edgePositionLayer2[6][51] = HepGeom::Point3D<double>(119.333,-11.7592,0);
  }
  for(int ie=0;ie<13;ie++) {
    if(ie==6) continue;
    for(int ip=0;ip<52;ip++) {
      m_edgePositionLayer2[ie][ip] = m_edgePositionLayer2[6][ip];
    }
  }
  m_edgePositionLayer2[-6+6][31] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-6+6][41] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-6+6][42] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-5+6][42] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-4+6][30] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-4+6][42] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-4+6][43] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-4+6][48] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-3+6][6] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-2+6][30] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[-1+6][30] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[0+6][9] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[0+6][13] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[0+6][27] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[0+6][36] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[0+6][43] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[1+6][32] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[1+6][41] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[2+6][32] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[2+6][41] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[2+6][50] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[3+6][41] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[4+6][19] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[4+6][41] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[5+6][33] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[5+6][40] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[5+6][41] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[6+6][32] = HepGeom::Point3D<double>(0,0,0);
  m_edgePositionLayer2[6+6][41] = HepGeom::Point3D<double>(0,0,0);
}

void HIPixelTrackletsMaker::setEdgePhi() {
  for(int ie=0; ie<13; ie++) {
    for(int ip=0; ip<22; ip++) {
      double x = m_edgePositionLayer0[ie][ip].x();
      double y = m_edgePositionLayer0[ie][ip].y();
      double phi = (x==0.||y==0.)?3.1416:atan2(y-m_vy,x-m_vx);
      m_edgePhiLayer0[ie][ip]=phi;
    }
    for(int ip=0; ip<38; ip++) {
      double x = m_edgePositionLayer1[ie][ip].x();
      double y = m_edgePositionLayer1[ie][ip].y();
      double phi = (x==0.||y==0.)?3.1416:atan2(y-m_vy,x-m_vx);
      m_edgePhiLayer1[ie][ip]=phi;
    }
    for(int ip=0; ip<52; ip++) {
      double x = m_edgePositionLayer2[ie][ip].x();
      double y = m_edgePositionLayer2[ie][ip].y();
      double phi = (x==0.||y==0.)?3.1416:atan2(y-m_vy,x-m_vx);
      m_edgePhiLayer2[ie][ip]=phi;      
    }
  }
  
}

void HIPixelTrackletsMaker::removeOverLapCluster(int layer, int arrSize0) {
  for(unsigned int i=0; i<m_clusterLayer[layer].size(); i++) {
    const PixelCluster& clusi0 = m_clusterLayer[layer][i];
    Identifier id = clusi0.identify();
    double cxi0 = clusi0.globalPosition().x() - m_vx;
    double cyi0 = clusi0.globalPosition().y() - m_vy;
    double phii0 = atan2(cyi0,cxi0);
    int etaMo = m_pixelID->eta_module(id);
    int phiMo = m_pixelID->phi_module(id);
    double edgePhi = 3.1416;
    if( phiMo<arrSize0-1 ) {
      if( layer==0 ) edgePhi = m_edgePhiLayer0[etaMo+6][phiMo+1];
      else if(layer==1 ) edgePhi = m_edgePhiLayer1[etaMo+6][phiMo+1];
      else  edgePhi = m_edgePhiLayer2[etaMo+6][phiMo+1];
      if( phii0>edgePhi ) {
	//condition deal with the situation around CLHEP::pi/-CLHEP::pi
	if( edgePhi>0 || (edgePhi<0&&phii0<0) )   m_overlapClusLayer[layer][i]++;
      }
      
    }
    else {
      if( layer==0 ) edgePhi = m_edgePhiLayer0[etaMo+6][0];
      else if(layer==1 ) edgePhi = m_edgePhiLayer1[etaMo+6][0];
      else  edgePhi = m_edgePhiLayer2[etaMo+6][0];
      if( phii0>edgePhi ) m_overlapClusLayer[layer][i]++;
    }
  }
}
