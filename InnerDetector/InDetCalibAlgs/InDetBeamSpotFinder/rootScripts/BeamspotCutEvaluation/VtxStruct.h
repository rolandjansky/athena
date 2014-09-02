/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PDVTX_HPP
#define D3PDVTX_HPP

#include <vector>
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TObject.h"

#include <map>
#include <iostream>
#include <algorithm>

struct SEvent {
  UInt_t          ei_RunNumber;
  UInt_t          ei_EventNumber;
  UInt_t          ei_timestamp;
  UInt_t          ei_timestamp_ns;
  UInt_t          ei_lbn;
  UInt_t          ei_bcid;
  UInt_t          ei_detmask0;
  UInt_t          ei_detmask1;
};

struct SMCVertex{
  Int_t           mcVx_n;
  std::vector<float>   *mcVx_x;
  std::vector<float>   *mcVx_y;
  std::vector<float>   *mcVx_z;  
};

struct SVertex{
  Int_t           vx_n;
  std::vector<float>   *vx_x;
  std::vector<float>   *vx_y;
  std::vector<float>   *vx_z;
  std::vector<float>   *vx_chi2;
  std::vector<int>     *vx_ndof;
  std::vector<float>   *vx_err_x;
  std::vector<float>   *vx_err_y;
  std::vector<float>   *vx_err_z;
  std::vector<float>   *vx_cov_x;
  std::vector<float>   *vx_cov_y;
  std::vector<float>   *vx_cov_z;
  std::vector<float>   *vx_cov_xy;
  std::vector<float>   *vx_cov_xz;
  std::vector<float>   *vx_cov_yz;
  std::vector<float>   *vx_px;
  std::vector<float>   *vx_py;
  std::vector<float>   *vx_pz;
  std::vector<float>   *vx_E;
  std::vector<float>   *vx_m;
  std::vector<int>     *vx_nTracks;
  std::vector<float>   *vx_sumPt;
  std::vector<int>     *vx_type;
  std::vector<int>     *vx_trk_n;
  std::vector<std::vector<float> > *vx_trk_chi2;
  std::vector<std::vector<float> > *vx_trk_d0;
  std::vector<std::vector<float> > *vx_trk_z0;
  std::vector<std::vector<float> > *vx_trk_unbiased_d0;
  std::vector<std::vector<float> > *vx_trk_unbiased_z0;
  std::vector<std::vector<float> > *vx_trk_err_unbiased_d0;
  std::vector<std::vector<float> > *vx_trk_err_unbiased_z0;
  std::vector<std::vector<float> > *vx_trk_phi;
  std::vector<std::vector<float> > *vx_trk_theta;
  std::vector<std::vector<float> > *vx_trk_weight;
  //  std::vector<std::vector<int> > *vx_trk_index; // not allowed
  
  
};
/*
class MyVertex : public TObject {
  ClassDef(MyVertex, 1);
*/
class MyVertex {
 public:
  MyVertex():cov(3){}
  virtual ~MyVertex(){}
  TVector3 pos;
  TMatrixDSym cov;
  
  float chi2;
  int ndf;
  float sumPt;
  int vType;
  int nTracks;
  int trk_n;
  std::vector<float> trk_chi2;
  std::vector<float> trk_d0;
  std::vector<float> trk_z0;
  std::vector<float> trk_unbiased_d0;
  std::vector<float> trk_unbiased_z0;
  std::vector<float> trk_err_unbiased_d0;
  std::vector<float> trk_err_unbiased_z0;
  std::vector<float> trk_phi;
  std::vector<float> trk_theta;
  std::vector<float> trk_weight;
  std::vector<int>   trk_index;
  
  void print() const {
    pos.Print();
    cov.Print();
    std::cout  << chi2 << "  " << ndf << "  " << sumPt << "  " << nTracks << std::endl;
    
  }
  
};

struct Event {
  UInt_t          runNumber;
  UInt_t          eventNumber;
  UInt_t          timestamp;
  UInt_t          timestamp_ns;
  UInt_t          lbn;
  UInt_t          bcid;
  UInt_t          detmask0;
  UInt_t          detmask1;
  void clear() {
    runNumber=-1;
    eventNumber=-1;
    timestamp=-1;
    timestamp_ns=-1;
    lbn=-1;
    bcid=-1;
    detmask0=-1;
    detmask1=-1;
  }
  void fill( SEvent * evt) {
    clear();
    if (!evt) return;
    runNumber=evt->ei_RunNumber;
    eventNumber=evt->ei_EventNumber;
    timestamp=evt->ei_timestamp;
    timestamp_ns=evt->ei_timestamp_ns;
    lbn=evt->ei_lbn;
    bcid=evt->ei_bcid;
    detmask0=evt->ei_detmask0;
    detmask1=evt->ei_detmask1;
    
  }
};

struct MyMCVertex {
  TVector3 pos;
  void print() const {pos.Print();}
};

struct MCVertices {
  int nVx;
  std::vector<MyMCVertex> vertices;
  void clear() {    
    nVx = -1;
    vertices.clear();
  }
  void fill( SMCVertex * vx) {
    clear();
    if (!vx) return;
    nVx = vx->mcVx_n;
    for ( int i =0; i < vx->mcVx_n; ++i ) {
      MyMCVertex temp;
      temp.pos.SetXYZ( vx->mcVx_x->at(i), vx->mcVx_y->at(i), vx->mcVx_z->at(i) );
      vertices.push_back(temp);
    }// for
  }    
};

struct Vertices {
  int nVx;
  std::vector<MyVertex> vertices;
  void clear() {    
    nVx = -1;
    vertices.clear();
  }
  void fill( SVertex * vx) {
    clear();
    if (!vx) return;
    nVx = vx->vx_n;
    for ( int i =0; i < vx->vx_n; ++i ) {
      //cout << "XX " << vx->vx_type->at(i) << endl;
      if ( vx->vx_type->at(i) == 0) continue; // don't allow dummy vertex
      
      MyVertex temp;
      temp.cov.Zero();
      temp.pos.SetXYZ( vx->vx_x->at(i), vx->vx_y->at(i), vx->vx_z->at(i) );
      temp.chi2  = vx->vx_chi2->at(i);
      temp.ndf   = vx->vx_ndof->at(i);
      temp.sumPt = vx->vx_sumPt->at(i);
      temp.vType = vx->vx_type->at(i);
      temp.nTracks = vx->vx_nTracks->at(i);
      temp.trk_n   = vx->vx_trk_n->at(i);
      
      temp.cov[0][0] = vx->vx_cov_x->at(i);
      temp.cov[1][1] = vx->vx_cov_y->at(i);
      temp.cov[2][2] = vx->vx_cov_z->at(i);
      temp.cov[0][1] = vx->vx_cov_xy->at(i);
      temp.cov[0][2] = vx->vx_cov_xz->at(i);
      temp.cov[1][2] = vx->vx_cov_yz->at(i);
      
      // tracks
      std::copy(vx->vx_trk_chi2->at(i).begin() ,vx->vx_trk_chi2->at(i).end() ,std::back_inserter(temp.trk_chi2) );
      std::copy(vx->vx_trk_d0->at(i).begin() ,vx->vx_trk_d0->at(i).end() ,std::back_inserter(temp.trk_d0) );
      std::copy(vx->vx_trk_z0->at(i).begin() ,vx->vx_trk_z0->at(i).end() ,std::back_inserter(temp.trk_z0) );
      std::copy(vx->vx_trk_unbiased_d0->at(i).begin() ,vx->vx_trk_unbiased_d0->at(i).end() ,std::back_inserter(temp.trk_unbiased_d0) );
      std::copy(vx->vx_trk_unbiased_z0->at(i).begin() ,vx->vx_trk_unbiased_z0->at(i).end() ,std::back_inserter(temp.trk_unbiased_z0) );
      std::copy(vx->vx_trk_err_unbiased_d0->at(i).begin() ,vx->vx_trk_err_unbiased_d0->at(i).end() ,std::back_inserter(temp.trk_err_unbiased_d0) );
      std::copy(vx->vx_trk_err_unbiased_z0->at(i).begin() ,vx->vx_trk_err_unbiased_z0->at(i).end() ,std::back_inserter(temp.trk_err_unbiased_z0) );
      std::copy(vx->vx_trk_phi->at(i).begin() ,vx->vx_trk_phi->at(i).end() ,std::back_inserter(temp.trk_phi) );
      std::copy(vx->vx_trk_theta->at(i).begin() ,vx->vx_trk_theta->at(i).end() ,std::back_inserter(temp.trk_theta) );
      std::copy(vx->vx_trk_weight->at(i).begin() ,vx->vx_trk_weight->at(i).end() ,std::back_inserter(temp.trk_weight) );
      //      std::copy(vx->vx_trk_index->at(i).begin() ,vx->vx_trk_index->at(i).end() ,std::back_inserter(temp.trk_index) );

      vertices.push_back(temp);
    } // for
    nVx = (int) vertices.size(); // may be different to vx_n if includes dummy
         
  } // fill
};


// functions
double getChi2Val( const TVector3 & a, const TVector3 & b, const TMatrixDSym & cov ) {
  TMatrixDSym covInv(cov);
  covInv.Invert();
  //  TMatrixD d = b-a; 
  TVectorT<double> temp(3);
  temp[0] = b[0]-a[0];
  temp[1] = b[1]-a[1];
  temp[2] = b[2]-a[2];
  double c= covInv.Similarity( temp);
//  c.Print();
  // cout <<  c[0][0] << endl;
  return c;
}


std::map<const MyMCVertex*,const MyVertex*> matchVertices( const MCVertices & mcVertices, const Vertices & vertices ) {
  
  std::multimap<double, std::pair<const MyVertex*,const MyMCVertex*> >  best;
  
  // 
  for (std::vector<MyVertex>::const_iterator it = vertices.vertices.begin(); it != vertices.vertices.end(); ++it) {
    for (std::vector<MyMCVertex>::const_iterator mit = mcVertices.vertices.begin(); mit != mcVertices.vertices.end(); ++mit) {
      double c = getChi2Val( it->pos, mit->pos, it->cov);
      best.insert( std::make_pair( c, std::make_pair( &(*it), &(*mit) ) ) ); 
    }
  }
  
  // 
  std::vector<const MyVertex*>   usedVtx;
  std::vector<const MyMCVertex*> usedMCVtx;

  std::map<const MyMCVertex*,const MyVertex* > matches;
  
  for ( std::multimap<double, std::pair<const MyVertex*,const MyMCVertex*> >::iterator it = best.begin(); it != best.end(); ++it ) {
    
    const MyVertex* vx = it->second.first;
    const MyMCVertex* mcvx = it->second.second;
    
    if ( find(usedVtx.begin(), usedVtx.end(), vx) != usedVtx.end()) continue;
    if ( find(usedMCVtx.begin(), usedMCVtx.end(), mcvx) != usedMCVtx.end()) continue;

    matches.insert( std::make_pair( mcvx, vx) );
    usedVtx.push_back(vx);
    usedMCVtx.push_back(mcvx);
  }
  
  return matches;
}


std::map<const MyVertex*,const MyVertex*> matchVertices( const Vertices & mcVertices, const Vertices & vertices ) {
  
  std::multimap<double, std::pair<const MyVertex*,const MyVertex*> >  best;
  
  // 
  for (std::vector<MyVertex>::const_iterator it = vertices.vertices.begin(); it != vertices.vertices.end(); ++it) {
    for (std::vector<MyVertex>::const_iterator mit = mcVertices.vertices.begin(); mit != mcVertices.vertices.end(); ++mit) {
      double c = getChi2Val( it->pos, mit->pos, it->cov + mit->cov);
      best.insert( std::make_pair( c, std::make_pair( &(*it), &(*mit) ) ) ); 
    }
  }
  
  // 
  std::vector<const MyVertex*>   usedVtx;
  std::vector<const MyVertex*> usedMCVtx;
  
  std::map<const MyVertex*,const MyVertex* > matches;
  
  for ( std::multimap<double, std::pair<const MyVertex*,const MyVertex*> >::iterator it = best.begin(); it != best.end(); ++it ) {
    
    const MyVertex* vx = it->second.first;
    const MyVertex* mcvx = it->second.second;
    
    if ( find(usedVtx.begin(), usedVtx.end(), vx) != usedVtx.end()) continue;
    if ( find(usedMCVtx.begin(), usedMCVtx.end(), mcvx) != usedMCVtx.end()) continue;
    
    matches.insert( std::make_pair( mcvx, vx) );
    usedVtx.push_back(vx);
    usedMCVtx.push_back(mcvx);
  }
  
  return matches;
}


#endif

