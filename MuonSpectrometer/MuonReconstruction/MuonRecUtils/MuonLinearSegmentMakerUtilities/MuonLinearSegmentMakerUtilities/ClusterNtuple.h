/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
//// ClusterNtuple.h (c) ATLAS Detector software
//// Author: N.Bernard <nathan.rogers.bernard@cern.ch>
/////////////////////////////////////////////////////////////////////////
#ifndef CLUSTERNTUPLE_H
#define CLUSTERNTUPLE_H

#include "MuonLinearSegmentMakerUtilities/Cluster.h"
#include <vector>
#include <algorithm>

class TTree;

namespace ClusterSeg {

  enum FillType {chi2,chi2T,residual,residualT};

  struct ClusterNtuple {

    void initForWrite(TTree& tree);
    void initForRead(TTree& tree);

    void fill(const double& fillVal, FillType theFillType );
    void fill( const Cluster& cluster );
    void fill( const std::vector<Cluster*>& clusters );

    bool read( std::vector<Cluster*>& clusters);
    void clean( std::vector<Cluster*>& clusters);   

    void reset();
    void init();

    static const int CLUSTERSIZE = 50000;

    double residualT[CLUSTERSIZE];
    double residual[CLUSTERSIZE];
    int nMDT;
    int nMDTT;
    double chi2T[CLUSTERSIZE];
    double chi2[CLUSTERSIZE];
    int ntracksT;
    int ntracks;
    int nclusters;
    double x[CLUSTERSIZE];
    double y[CLUSTERSIZE];
    double z[CLUSTERSIZE];
    int tIndex[CLUSTERSIZE];
    int pIndex[CLUSTERSIZE];
    bool isPhi[CLUSTERSIZE]; 
    bool isMatch[CLUSTERSIZE];
    int barcode[CLUSTERSIZE];
  };

}


#endif
