/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
////// ClusterNtuple.cxx (c) ATLAS Detector software
////// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
///////////////////////////////////////////////////////////////////////////
#include "MuonLinearSegmentMakerUtilities/ClusterNtuple.h"

#include <iostream>
#include <TTree.h>

namespace ClusterSeg {

  void ClusterNtuple::initForWrite(TTree& tree){
    init();
    tree.Branch("nclusters",&nclusters,"nclusters/I");
    tree.Branch("x",&x,"x[nclusters]/D");
    tree.Branch("y",&y,"y[nclusters]/D");
    tree.Branch("z",&z,"z[nclusters]/D");
    tree.Branch("isPhi",&isPhi,"isPhi[nclusters]/O");
    tree.Branch("techIndex",&tIndex,"techIndex[nclusters]/I");
    tree.Branch("phiIndex",&pIndex,"phiIndex[nclusters]/I");
    tree.Branch("isMatch",&isMatch,"isMatch[nclusters]/O");
    tree.Branch("barcode",&barcode,"barcode[nclusters]/I");
    tree.Branch("ntracks",&ntracks,"ntracks/I");
    tree.Branch("chi2",&chi2,"chi2[ntracks]/D");
    tree.Branch("ntracksT",&ntracksT,"ntracksT/I");
    tree.Branch("chi2T",&chi2T,"chi2T[ntracksT]/D");
    tree.Branch("nMDT",&nMDT,"nMDT/I");
    tree.Branch("residual",&residual,"residual[nMDT]/D");
    tree.Branch("nMDTT",&nMDTT,"nMDTT/I");
    tree.Branch("residualT",&residualT,"residualT[nMDTT]/D");
  }

  void ClusterNtuple::initForRead(TTree& tree) {
    tree.SetBranchAddress("nclusters",&nclusters);
    tree.SetBranchAddress("x",&x);
    tree.SetBranchAddress("y",&y);
    tree.SetBranchAddress("z",&z);
    tree.SetBranchAddress("isPhi",&isPhi);
    tree.SetBranchAddress("techIndex",&tIndex);
    tree.SetBranchAddress("phiIndex",&pIndex);
    tree.SetBranchAddress("isMatch",&isMatch);
    tree.SetBranchAddress("barcode",&barcode);
  }
 
  void ClusterNtuple::fill( const std::vector<Cluster*>& clusters) {
    for(auto& it: clusters) fill(*it);
  }

  void ClusterNtuple::fill( const Cluster& cluster ){
    if( nclusters >= CLUSTERSIZE ) return;
    x[nclusters] = cluster.m_x;
    y[nclusters] = cluster.m_y;
    z[nclusters] = cluster.m_z;
    isPhi[nclusters] = cluster.m_isPhi;
    tIndex[nclusters] = (int)cluster.m_tIndex;
    pIndex[nclusters] = (int)cluster.m_pIndex;
    isMatch[nclusters] = cluster.m_isMatch;
    barcode[nclusters] = cluster.m_barcode;
    ++nclusters;
  }

  void ClusterNtuple::fill(const double& theVal,FillType theFillType) {
    if (theFillType == FillType::chi2){
      if( ntracks >= CLUSTERSIZE ) return;
      chi2[ntracks] = theVal;
      ++ntracks;   
    }
 
    if (theFillType == FillType::chi2T){
      if( ntracksT >= CLUSTERSIZE ) return;
      chi2T[ntracksT] = theVal;
      ++ntracksT;
    }
 
    if (theFillType == FillType::residual){
      if( ntracks >= CLUSTERSIZE ) return;
      residual[nMDT] = theVal;
      ++nMDT;
    }

    if (theFillType == FillType::residualT){
      if( ntracksT >= CLUSTERSIZE ) return;
      residualT[nMDTT] = theVal;
      ++nMDTT;
    }
 
  }

  void ClusterNtuple::reset() {
    nclusters = 0;
    ntracks = 0;
    ntracksT = 0;
    nMDT = 0;
    nMDTT = 0;
  }

  bool ClusterNtuple::read( std::vector<Cluster*>& clusters) {
    if( nclusters == 0 ) {
      std::cout << " ntuple not initialized for reading " << std::endl;
      return false;
    }
    
    for( int i=0;i<nclusters;++i){
      Cluster* cluster = new Cluster(x[i],y[i],z[i],isPhi[i],tIndex[i],pIndex[i],isMatch[i],barcode[i]);
      clusters.push_back(cluster);
    }

    return true;
  }

  void ClusterNtuple::clean( std::vector<Cluster*>& clusters) {
    for(auto& it: clusters) delete it;
  }

  void ClusterNtuple::init() {
    nclusters = 0;
    ntracks = 0;
    ntracksT = 0;
    nMDT = 0;
    nMDTT = 0;
  }

}
