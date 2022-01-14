/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// ClusterAnalysis.cxx (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "MuonLinearSegmentMakerUtilities/ClusterAnalysis.h"
#include "TTree.h"
#include <set>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <utility> 
#include "TMath.h" // for TMath::Sign()

namespace ClusterSeg {

  bool sortfunctionTGC (Cluster* i, Cluster* j)  { return (i->z() < j->z()); }
  bool sortfunctionRPC (Cluster* i, Cluster* j)  { return (i->rCyl() < j->rCyl()); }
  
  ClusterAnalysis::ClusterAnalysis() : 
    m_tree(nullptr), 
    m_ncalls(-1),
    m_writeOut(false),
    m_ang_cut(0.5),
    m_ang2_cut(1.0),
    m_dist_cut(100.0)
  {
    m_ntuple.init();
  }

  ClusterAnalysis::ClusterAnalysis( TTree& tree ) : 
    m_tree(&tree),
    m_writeOut(false),
    m_ang_cut(0.5),
    m_ang2_cut(1.0),
    m_dist_cut(100.0)
  {
    m_ntuple.initForRead(tree);
  }
  

  void ClusterAnalysis::analyseWrite() {
    MsgStream log(Athena::getMessageSvc(),"ClusterAnalysis::analyseWrite");
    m_h_barcodes = std::make_unique<TH2F>("barcodes","",1000,0,1000,1000,0,1000);
    m_h_barcodes->GetXaxis()->SetTitle("layer 1");
    m_h_barcodes->GetYaxis()->SetTitle("layer 3");

    m_h_corr = std::make_unique<TH2F>("phi_theta_corr","",100,0.,M_PI_2,100,0,M_PI_2);
    m_h_corr->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_corr->GetYaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_rz = std::make_unique<TH2F>("rz_spacePoints","",1000,0,12000,3000,1000,13000);
    m_h_rz->GetXaxis()->SetTitle("Z [mm]");
    m_h_rz->GetYaxis()->SetTitle("R [mm]");
    m_h_xy = std::make_unique<TH2F>("xy_spacePoints","",3000,-13000,13000,3000,-13000,13000);
    m_h_xy->GetXaxis()->SetTitle("X [mm]");
    m_h_xy->GetYaxis()->SetTitle("Y [mm]");

    m_h_miss_RZ = std::make_unique<TH2F>("rz_missing_spacePoints","",1000,12000,16000,3000,1000,13000);
    m_h_miss_RZ->GetXaxis()->SetTitle("Z [mm]");
    m_h_miss_RZ->GetYaxis()->SetTitle("R [mm]");
    m_h_miss_XY = std::make_unique<TH2F>("xy_missing_spacePoints","",3000,-13000,13000,3000,-13000,13000);
    m_h_miss_XY->GetXaxis()->SetTitle("X [mm]");
    m_h_miss_XY->GetYaxis()->SetTitle("Y [mm]");

    m_h_phi = std::make_unique<TH1F>("phi","",100,0,M_PI_2);
    m_h_phi->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta = std::make_unique<TH1F>("theta","",100,0,M_PI_2);
    m_h_theta->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle = std::make_unique<TH1F>("angle","",100,0,M_PI);
    m_h_angle->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle->GetYaxis()->SetTitle("Number of Entries");

    m_h_phi12 = std::make_unique<TH1F>("phi12","",100,0,M_PI_2);
    m_h_phi12->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi12->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta12 = std::make_unique<TH1F>("theta12","",100,0,M_PI_2);
    m_h_theta12->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta12->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle12 = std::make_unique<TH1F>("angle12","",100,0,M_PI);
    m_h_angle12->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle12->GetYaxis()->SetTitle("Number of Entries");

    m_h_phi13 = std::make_unique<TH1F>("phi13","",100,0,M_PI_2);
    m_h_phi13->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi13->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta13 = std::make_unique<TH1F>("theta13","",100,0,M_PI_2);
    m_h_theta13->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta13->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle13 = std::make_unique<TH1F>("angle13","",100,0,M_PI);
    m_h_angle13->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle13->GetYaxis()->SetTitle("Number of Entries");

    m_h_phi23 = std::make_unique<TH1F>("phi23","",100,0,M_PI_2);
    m_h_phi23->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi23->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta23 = std::make_unique<TH1F>("theta23","",100,0,M_PI_2);
    m_h_theta23->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta23->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle23 = std::make_unique<TH1F>("angle23","",100,0,M_PI);
    m_h_angle23->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle23->GetYaxis()->SetTitle("Number of Entries");

    m_h_R = std::make_unique<TH1F>("R","",100,0,1000);
    m_h_R->GetXaxis()->SetTitle("|#Delta R|");
    m_h_R->GetYaxis()->SetTitle("Number of Entries");
    m_h_XY = std::make_unique<TH1F>("XY","",100,0,1000);
    m_h_XY->GetXaxis()->SetTitle("|#Delta XY|");
    m_h_XY->GetYaxis()->SetTitle("Number of Entries"); 
    m_h_R_t = std::make_unique<TH1F>("R_t","",100,0,1000);
    m_h_R_t->GetXaxis()->SetTitle("|#Delta R|");
    m_h_R_t->GetYaxis()->SetTitle("Number of Entries");
    m_h_XY_t = std::make_unique<TH1F>("XY_t","",100,0,1000);
    m_h_XY_t->GetXaxis()->SetTitle("|#Delta XY|");
    m_h_XY_t->GetYaxis()->SetTitle("Number of Entries");

    m_h_numseeds = std::make_unique<TH1F>("num_seeds","",200,0,200);
    m_h_numseeds->GetXaxis()->SetTitle("Number of Seeds");
    m_h_numseeds->GetYaxis()->SetTitle("Number of Entries");    

    m_h_phi_t = std::make_unique<TH1F>("phi_t","",100,0,M_PI_2);
    m_h_phi_t->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi_t->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta_t = std::make_unique<TH1F>("theta_t","",100,0,M_PI_2);
    m_h_theta_t->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta_t->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle_t = std::make_unique<TH1F>("angle_t","",100,0,M_PI);
    m_h_angle_t->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle_t->GetYaxis()->SetTitle("Number of Entries");

    m_h_phi_t12 = std::make_unique<TH1F>("phi_t12","",100,0,M_PI_2);
    m_h_phi_t12->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi_t12->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta_t12 = std::make_unique<TH1F>("theta_t12","",100,0,M_PI_2);
    m_h_theta_t12->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta_t12->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle_t12 = std::make_unique<TH1F>("angle_t12","",100,0,M_PI);
    m_h_angle_t12->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle_t12->GetYaxis()->SetTitle("Number of Entries");
    
    m_h_phi_t13 = std::make_unique<TH1F>("phi_t13","",100,0,M_PI_2);
    m_h_phi_t13->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi_t13->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta_t13 = std::make_unique<TH1F>("theta_t13","",100,0,M_PI_2);
    m_h_theta_t13->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta_t13->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle_t13 = std::make_unique<TH1F>("angle_t13","",100,0,M_PI);
    m_h_angle_t13->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle_t13->GetYaxis()->SetTitle("Number of Entries");
  
    m_h_phi_t23 = std::make_unique<TH1F>("phi_t23","",100,0,M_PI_2);
    m_h_phi_t23->GetXaxis()->SetTitle("|#Delta #phi| [rad]");
    m_h_phi_t23->GetYaxis()->SetTitle("Number of Entries");
    m_h_theta_t23 = std::make_unique<TH1F>("theta_t23","",100,0,M_PI_2);
    m_h_theta_t23->GetXaxis()->SetTitle("|#Delta #theta| [rad]");
    m_h_theta_t23->GetYaxis()->SetTitle("Number of Entries");
    m_h_angle_t23 = std::make_unique<TH1F>("angle_t23","",100,0,M_PI);
    m_h_angle_t23->GetXaxis()->SetTitle("#theta [rad]");
    m_h_angle_t23->GetYaxis()->SetTitle("Number of Entries");

    m_h_sizeL1 = std::make_unique<TH1F>("sizeL1","",200,0,250);
    m_h_sizeL1->GetXaxis()->SetTitle("Size of L1 Spacepoints");
    m_h_sizeL1->GetYaxis()->SetTitle("Number of Entries");
    m_h_sizeL2 = std::make_unique<TH1F>("sizeL2","",200,0,250);
    m_h_sizeL2->GetXaxis()->SetTitle("Size of L2 Spacepoints");
    m_h_sizeL2->GetYaxis()->SetTitle("Number of Entries");
    m_h_sizeL3 = std::make_unique<TH1F>("sizeL3","",200,0,250);
    m_h_sizeL3->GetXaxis()->SetTitle("Size of L3 Spacepoints");
    m_h_sizeL3->GetYaxis()->SetTitle("Number of Entries");

    m_h_sizeL1L3 = std::make_unique<TH1F>("sizeL1L3","",10000,0,10000);
    m_h_sizeL1L2L3 = std::make_unique<TH1F>("sizeL1L2L3","",20000,0,20000); 

    m_h_XY121 = std::make_unique<TH1F>("XY121","",100,0,100);
    m_h_XY122 = std::make_unique<TH1F>("XY122","",100,0,100);
    m_h_XY121_t = std::make_unique<TH1F>("XY121_t","",100,0,100);
    m_h_XY122_t = std::make_unique<TH1F>("XY122_t","",100,0,100);

    m_h_XY131 = std::make_unique<TH1F>("XY131","",100,0,100);
    m_h_XY133 = std::make_unique<TH1F>("XY133","",100,0,100);
    m_h_XY131_t = std::make_unique<TH1F>("XY131_t","",100,0,100);
    m_h_XY133_t = std::make_unique<TH1F>("XY133_t","",100,0,100);

    m_h_XY232 = std::make_unique<TH1F>("XY232","",100,0,100);
    m_h_XY233 = std::make_unique<TH1F>("XY233","",100,0,100);
    m_h_XY232_t = std::make_unique<TH1F>("XY232_t","",100,0,100);
    m_h_XY233_t = std::make_unique<TH1F>("XY233_t","",100,0,100);

    std::vector<SpacePoint> thePoints;

    Long64_t nentries = m_tree->GetEntries();
    m_ncalls = 0;
    for( Long64_t evt=0;evt<nentries;++evt ){
      m_tree->LoadTree(evt);
      m_tree->GetEntry(evt);
    

      if (log.level()<=MSG::DEBUG && evt % 100 == 0) log << MSG::DEBUG << "event " << evt << "/" << nentries << endmsg;
  
      std::vector<Cluster*> clust;
      m_ntuple.read(clust);
 

      if (clust.at(0)->techIndex() == Muon::MuonStationIndex::TechnologyIndex::TGC){
        std::sort (clust.begin(),clust.end(), sortfunctionTGC);
        thePoints = createSpacePoints(clust);
    
        if(thePoints.empty()) continue;
        std::vector<std::vector<SpacePoint>> seeds = createTGCSeeds(thePoints);
        m_h_numseeds->Fill(seeds.size());
      } else {
        std::sort (clust.begin(),clust.end(), sortfunctionRPC);
        thePoints = createSpacePoints(clust);

        if(thePoints.empty()) continue;
        std::vector<std::vector<SpacePoint>> seeds = createRPCSeeds(thePoints);
        m_h_numseeds->Fill(seeds.size());
      }

      for(auto &it: thePoints){
        m_h_rz->Fill(fabs(it.z()),it.rCyl());
        m_h_xy->Fill(it.x(),it.y());
      }
      ++m_ncalls;
      m_ntuple.clean(clust);
    }
  }

  std::vector<std::vector<SpacePoint>> ClusterAnalysis::analyse(const std::vector<Cluster*>& clust){
    std::vector<std::vector<SpacePoint>> sPoints;
    std::vector<SpacePoint> thePoints = createSpacePoints(clust);
    if(thePoints.empty()) return sPoints;
    std::vector<std::vector<SpacePoint>> seeds = (thePoints.at(0).techIndex() == Muon::MuonStationIndex::TechnologyIndex::TGC) ? 
                                                 createTGCSeeds(thePoints) : createRPCSeeds(thePoints);
    return seeds;  
  }

  std::vector<std::vector<SpacePoint>> ClusterAnalysis::createTGCSeeds( const std::vector<SpacePoint>& points){

    std::vector<SpacePoint> layer1Points;
    std::vector<SpacePoint> layer2Points;
    std::vector<SpacePoint> layer3Points;
    std::vector<std::vector<SpacePoint>> seeds;

    for(auto &it: points){
      if (it.phiIndex() == Muon::MuonStationIndex::PhiIndex::T1) layer1Points.push_back(it);
      else if(it.phiIndex() == Muon::MuonStationIndex::PhiIndex::T2) layer2Points.push_back(it);
      else if(it.phiIndex() == Muon::MuonStationIndex::PhiIndex::T3) layer3Points.push_back(it);
    }

    if(m_writeOut){
      int L1size = layer1Points.size();
      int L2size = layer2Points.size();
      int L3size = layer3Points.size(); 
    
      m_h_sizeL1->Fill(L1size);
      m_h_sizeL2->Fill(L2size);
      m_h_sizeL3->Fill(L3size);
      m_h_sizeL1L3->Fill(L1size*L3size);
      m_h_sizeL1L2L3->Fill(L1size*L2size*L3size);
    }
    
    if(m_writeOut && (layer1Points.empty() || layer2Points.empty() || layer3Points.empty())){
      if(!layer1Points.empty()){
        for(auto &it1: layer1Points){if(it1.isMatch()) m_h_miss_RZ->Fill(fabs(it1.z()),it1.rCyl()); m_h_miss_XY->Fill(it1.x(),it1.y());} 
      }
      if(!layer2Points.empty()){
        for(auto &it1: layer2Points){if(it1.isMatch()) m_h_miss_RZ->Fill(fabs(it1.z()),it1.rCyl()); m_h_miss_XY->Fill(it1.x(),it1.y());}
      }
      if(!layer3Points.empty()){
        for(auto &it1: layer3Points){if(it1.isMatch()) m_h_miss_RZ->Fill(fabs(it1.z()),it1.rCyl()); m_h_miss_XY->Fill(it1.x(),it1.y());}
      }
    }
 
    createSeedsAllLayers(layer1Points,layer2Points,layer3Points,seeds);
    if(seeds.size() < 25){
      if(!layer1Points.empty() && !layer2Points.empty() && layer3Points.empty()) createSeedsTwoLayers(layer1Points,layer2Points,seeds);
      if(!layer3Points.empty() && !layer1Points.empty() && layer2Points.empty()) createSeedsTwoLayers(layer1Points,layer3Points,seeds);
      if(!layer2Points.empty() && !layer3Points.empty() && layer1Points.empty()) createSeedsTwoLayers(layer2Points,layer3Points,seeds);
    }
    return seeds;
  }

  std::vector<std::vector<SpacePoint>> ClusterAnalysis::createRPCSeeds( const std::vector<SpacePoint>& points){

    std::vector<SpacePoint> layer1Points;
    std::vector<SpacePoint> layer2Points;
    std::vector<std::vector<SpacePoint>> seeds;

    for(auto &it: points){
      if (it.phiIndex() == Muon::MuonStationIndex::PhiIndex::BM1) layer1Points.push_back(it);
      else if (it.phiIndex() == Muon::MuonStationIndex::PhiIndex::BM2) layer2Points.push_back(it);
    }
    if(m_writeOut){
      int L1size = layer1Points.size();
      int L2size = layer2Points.size();

      m_h_sizeL1->Fill(L1size);
      m_h_sizeL2->Fill(L2size);
    }
  
   
    if(m_writeOut && (layer1Points.empty() || layer2Points.empty())){
      if(!layer1Points.empty()){
        for(auto &it1: layer1Points){if(it1.isMatch()) m_h_miss_RZ->Fill(fabs(it1.z()),it1.rCyl()); m_h_miss_XY->Fill(it1.x(),it1.y());}
      }
      if(!layer2Points.empty()){
        for(auto &it1: layer2Points){if(it1.isMatch()) m_h_miss_RZ->Fill(fabs(it1.z()),it1.rCyl()); m_h_miss_XY->Fill(it1.x(),it1.y());}
      }
    }

    if(seeds.size() < 25 && !layer1Points.empty() && !layer2Points.empty()) createSeedsTwoLayers(layer1Points,layer2Points,seeds);
    
    return seeds;
  }


  void ClusterAnalysis::createSeedsAllLayers(const std::vector<SpacePoint>& layer1Points,
                                             const std::vector<SpacePoint>& layer2Points,
                                             const std::vector<SpacePoint>& layer3Points,
                                             std::vector<std::vector<SpacePoint>>& seeds){

    if(!layer1Points.empty() && !layer3Points.empty()){
      for(auto &it1: layer1Points){
      for(auto &it3: layer3Points){
      if (TMath::Sign(1.,it1.z()) != TMath::Sign(1.,it3.z())) continue;
        std::vector<SpacePoint> theSeed;
        Cluster point = Cluster(it3.x()-it1.x(),it3.y()-it1.y(),it3.z()-it1.z(),false,0,0,false,0);
        double angle = acos((point.x()*it1.x()+point.y()*it1.y()+point.z()*it1.z())/(point.rSph()*it1.rSph()));
        if(angle > M_PI) angle = 2*M_PI - angle;
        double dphi = fabs(point.phi()-it1.phi());
        if(dphi > M_PI_2 && dphi < 3*M_PI_2) dphi -= M_PI;
        if(dphi > 3*M_PI_2) dphi -= 2*M_PI;
        double dtheta = fabs(point.theta()-it1.theta());
        if (m_writeOut) {
          m_h_phi->Fill(fabs(dphi));
          m_h_theta->Fill(dtheta);
          m_h_corr->Fill(dphi,dtheta);
          m_h_angle->Fill(angle);
        }
        if(m_writeOut && it3.isMatch() && it1.isMatch() && it3.barcode() == it1.barcode() && it1.barcode() != 0 ) {
          m_h_barcodes->Fill(it1.barcode(),it3.barcode());
          m_h_phi_t->Fill(fabs(dphi));
          m_h_theta_t->Fill(dtheta);
          m_h_angle_t->Fill(angle);
        }

    if(angle < m_ang_cut) {
          if(!layer2Points.empty()){
            for(auto &it2: layer2Points){
              std::vector<SpacePoint> theSeed;
              if (TMath::Sign(1.,it2.z()) != TMath::Sign(1.,it1.z())) continue;
              double seedR = (it2.z()-it1.z())*tan(point.theta()) + it1.rCyl();
              double spR = it2.rCyl();
              double t = (it2.z() - it1.z())/point.z();
              double seedX = it1.x() + t*point.x();
              double seedY = it1.y() + t*point.y();
              double XY = sqrt(pow(it2.x()-seedX,2)+pow(it2.y()-seedY,2));
              if (m_writeOut) {
                m_h_R->Fill(fabs(spR-seedR));
                m_h_XY->Fill(XY);
              }

              if(m_writeOut && it1.isMatch() && it3.isMatch() && it2.isMatch() && it1.barcode() == it3.barcode() && it1.barcode() == it2.barcode() && it1.barcode() != 0) {
                m_h_R_t->Fill(fabs(spR-seedR));
                m_h_XY_t->Fill(XY);

              }

              if( XY < m_dist_cut){
                 theSeed.push_back(it1);
                 theSeed.push_back(it2);
                 theSeed.push_back(it3);
                 seeds.push_back(theSeed);
              }
            } //end loop of layer2points
          }
        } // check that seed passes IP pointing constraint   
      } //end loop over layer3points
      } //end loop over layer1points
    }//end if statement
  }

  void ClusterAnalysis::createSeedsTwoLayers(const std::vector<SpacePoint>& layer1Points,const std::vector<SpacePoint>& layer2Points,
                                             std::vector<std::vector<SpacePoint>>& seeds){
    for(auto &it1: layer1Points){
          for(auto &it3: layer2Points){
            if (TMath::Sign(1.,it1.z()) != TMath::Sign(1.,it3.z())) continue;
            std::vector<SpacePoint> theSeed;
            Cluster point = Cluster(it3.x()-it1.x(),it3.y()-it1.y(),it3.z()-it1.z(),false,0,0,false,0);
            double angle = acos((point.x()*it1.x()+point.y()*it1.y()+point.z()*it1.z())/(point.rSph()*it1.rSph()));
            if(angle > M_PI) angle = 2*M_PI - angle;
            double dphi = fabs(point.phi()-it1.phi());
            if(dphi > M_PI_2 && dphi < 3*M_PI_2) dphi -= M_PI;
            if(dphi > 3*M_PI_2) dphi -= 2*M_PI;
            double dtheta = fabs(point.theta()-it1.theta());
            if (m_writeOut) {
              m_h_phi12->Fill(fabs(dphi));
              m_h_theta12->Fill(dtheta);
              m_h_angle12->Fill(angle);
            }
            if(m_writeOut && it3.isMatch() && it1.isMatch() && it3.barcode() == it1.barcode() && it1.barcode() != 0 ) {
              m_h_phi_t12->Fill(fabs(dphi));
              m_h_theta_t12->Fill(dtheta);
              m_h_angle_t12->Fill(angle);
            }
            if (angle < m_ang2_cut){
              theSeed.push_back(it1);
              theSeed.push_back(it3);
              seeds.push_back(theSeed);
            }
          }//layer2Points
        } //layer1Points
  }

  std::vector<SpacePoint> ClusterAnalysis::createSpacePoints( const std::vector<Cluster*>& clust) {

    std::vector<std::pair<Cluster*,int>> phiClusters;
    std::vector<std::pair<Cluster*,int>> etaClusters;
    std::vector<SpacePoint> spacePoints;

    int citer(0);
    for(auto it: clust){
      if(it->isPhi()) phiClusters.push_back(std::make_pair(it,citer));
      else etaClusters.push_back(std::make_pair(it,citer));
      citer++;
    }  
    for(auto &pit: phiClusters){
    for(auto &eit: etaClusters){
      bool tbool = false;
      if (eit.first->isMatch() && pit.first->isMatch() && eit.first->barcode() == pit.first->barcode()) tbool = true;
        if(pit.first->phiIndex() == eit.first->phiIndex() && eit.first->isSideA() == pit.first->isSideA()) {
          SpacePoint thePoint = SpacePoint(eit.first->eta(),pit.first->phi(),eit.first->z(),eit.first->techIndex(),eit.first->phiIndex(),tbool,eit.first->barcode(),eit.second,pit.second);
          spacePoints.push_back(thePoint);
        }
    }
    }
    return spacePoints;
  }
}

