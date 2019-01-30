/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_MSVVTXINFO_H
#define JETTAGINFO_MSVVTXINFO_H

#include "JetTagInfo/SVTrackInfo.h"
#include "VxVertex/RecVertex.h"
#include <vector>
#include <iostream>

namespace Analysis
{

  class MSVVtxInfo
  {
  public:

    MSVVtxInfo();

    /** Set methods */
    void setRecSvx(const Trk::RecVertex&); 
    void setMass(float);   
    void setPt(float);
    void setEta(float);
    void setPhi(float);
    void setEnergyFraction(float);
    void setNormDist(float);
    void addTrackInfo(const SVTrackInfo& i);
    /** Get methods */
    const Trk::RecVertex& getRecSvx() const;
    float                 getMass() const;
    float                 getPt() const;
    float                 getEta() const;
    float                 getPhi() const;
    float                 getEnergyFraction() const;
    float                 getNormDist() const;
    int                   numTrackInfo() const;
    const SVTrackInfo&    getTrackInfo(unsigned int i) const;

  private:
    Trk::RecVertex m_recsvx;
    float          m_masssvx;
    float          m_ptsvx;
    float          m_etasvx;
    float          m_phisvx;
    float          m_efracsvx;
    float          m_normdist;
    std::vector<SVTrackInfo> m_trackinfo;
  
  }; // End class
  
  inline void MSVVtxInfo::setRecSvx(const Trk::RecVertex& gl) {m_recsvx   = gl;}
  inline void MSVVtxInfo::setMass(float mass)          {m_masssvx  = mass;}
  inline void MSVVtxInfo::setPt(float pt)          {m_ptsvx  = pt;}
  inline void MSVVtxInfo::setEta(float eta)          {m_etasvx  = eta;}
  inline void MSVVtxInfo::setPhi(float phi)          {m_phisvx  = phi;}
  inline void MSVVtxInfo::setEnergyFraction(float ef)  {m_efracsvx = ef;}
  inline void MSVVtxInfo::setNormDist(float nd)        {m_normdist = nd;}
  inline void MSVVtxInfo::addTrackInfo(const SVTrackInfo& i) { m_trackinfo.push_back(i); }
  //
  inline const Trk::RecVertex& MSVVtxInfo::getRecSvx() const       {return m_recsvx;}
  inline float    MSVVtxInfo::getMass() const                      {return m_masssvx;}
  inline float    MSVVtxInfo::getPt() const                      {return m_ptsvx;}
  inline float    MSVVtxInfo::getEta() const                      {return m_etasvx;}
  inline float    MSVVtxInfo::getPhi() const                      {return m_phisvx;}
  inline float    MSVVtxInfo::getEnergyFraction() const            {return m_efracsvx;}
  inline float    MSVVtxInfo::getNormDist() const                  {return m_normdist;}
  inline int      MSVVtxInfo::numTrackInfo() const                 {return m_trackinfo.size();}
  inline const SVTrackInfo& MSVVtxInfo::getTrackInfo(unsigned int i) const {return m_trackinfo.at(i);}



} // End namespace
#endif
