/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_ZFINDER_H__
#define __TRIG_ZFINDER_H__
#include <vector>
#include <algorithm>
#include <iostream>
#include "TrigInDetPattRecoTools/ZHist.h"

class TrigRoiDescriptor;
//class TrigInDetTracklet;
//
//struct ZFilter
//{
//    isNearZ(const std::vector<float&> zVertices, const float tol) : 
//      m_vertices(zVertices),
//      m_tol(tol)
//    {}
//
//    bool operator() ()
//    {
//      return ();
//    }
//
//    std::vector<float> m_vertices;
//    float m_tol;
//};

class TrigZFinder {
  public:
    TrigZFinder();
    ~TrigZFinder();
    void initialize(const TrigRoiDescriptor*);
    void finalize();

    //void fill(const std::vector<TrigInDetTracklet*>&);
    template <class Tracklet>
      void fill(const std::vector<Tracklet*>& tVec) {

        for (const auto tracklet : tVec) {
          //std::cout << "SMH: z0: " << tracklet->z0() << std::endl;
          m_zHist.fill(tracklet->z0());
        }
      }
    template <class Tracklet>
      void fill(const Tracklet* t) {
        m_zHist.fill(t->z0());
      }
    template <class Tracklet>
      void filterByZ(std::vector<Tracklet*>& tVec, const std::vector<float>& zVertices, const float tol) {
        //std::cout << "tVec.size(): " << tVec.size() << std::endl;
        tVec.erase(std::remove_if(tVec.begin(),tVec.end(),
        //Lambda function to check if tracklet z0 is within vertex +/- m_tol
        [&](const Tracklet* t) { 
          for (const auto vertex : zVertices) {
            if (fabs(t->z0() - vertex) < tol) {
              return false;
            }
          }
          return true;
        }
        ), tVec.end());
      }

    float findZ() const;
    const std::vector<float>& findMultipleZ(const unsigned int nVertices = 1);

    const ZHist& zHist() const {return m_zHist;}
    void setMinZBinSize(float minZBinSize) {m_minZBinSize = minZBinSize;}

  private:
    ZHist m_zHist;
    void setBinEdges();
    void setBinEdges(const TrigRoiDescriptor*);
    float m_minZBinSize      = 1.0;
    //const float m_zBinSizeEtaCoeff = 0.1;
    float m_zBinSizeEtaCoeff = 0.5;
    float m_ZFinder_MaxZ     = 200.0;
    float m_tol     = 3.0;
    const std::vector<float> m_emptyVector;
};

#endif
