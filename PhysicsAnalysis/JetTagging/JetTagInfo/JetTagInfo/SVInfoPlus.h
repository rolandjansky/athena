/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SVINFOPLUS_H
#define JETTAGINFO_SVINFOPLUS_H
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/SVTrackInfo.h"
#include "VxVertex/RecVertex.h"
#include <vector>
#include <iostream>

namespace Analysis
{

class SVInfoPlus : public BaseTagInfo
{
 public:
  /** default constructor */
  SVInfoPlus();
  
  /** constructor with info type */
  SVInfoPlus(TagInfoType x);
  
  /** copy destructor */
  SVInfoPlus(const SVInfoPlus& SVInfoPlus);
  
  /** assigenment operator */
  SVInfoPlus &operator= (const SVInfoPlus& rhs);
  
  /** default destructor */
  virtual ~SVInfoPlus();
  virtual SVInfoPlus* clone() const;

  /** Set methods */
  void setRecSvx(const Trk::RecVertex&); 
  void setNGTrackInJet(long int);
  void setNGTrackInSvx(long int);
  void setN2T(long int);
  void setMass(float);
  void setEnergyFraction(float);
  void setNormDist(float);
  void addTrackInfo(const SVTrackInfo& i);
  /** Get methods */
  const Trk::RecVertex& getRecSvx() const;
  long int              getNGTrackInJet() const;
  long int              getNGTrackInSvx() const;
  long int              getN2T() const;
  float                 getMass() const;
  float                 getEnergyFraction() const;
  float                 getNormDist() const;
  float                 getChi2Compatibility(const Trk::RecVertex priVertex, const Trk::RecVertex secVertex, const Amg::Vector3D jetDirection) const;
  int                   numTrackInfo() const;
  const SVTrackInfo&    getTrackInfo(uint i) const;

 private:
  Trk::RecVertex m_recsvx;
  long int       m_NGTinJet;
  long int       m_NGTinSvx;
  long int       m_N2Tpair;
  float          m_masssvx;
  float          m_efracsvx;
  float          m_normdist;
  std::vector<SVTrackInfo> m_trackinfo;
  
}; // End class
  
 inline void SVInfoPlus::setRecSvx(const Trk::RecVertex& gl) {m_recsvx   = gl;}
 inline void SVInfoPlus::setNGTrackInJet(long int li) {m_NGTinJet = li;}
 inline void SVInfoPlus::setNGTrackInSvx(long int li) {m_NGTinSvx = li;}
 inline void SVInfoPlus::setN2T(long int li)          {m_N2Tpair  = li;}
 inline void SVInfoPlus::setMass(float mass)          {m_masssvx  = mass;}
 inline void SVInfoPlus::setEnergyFraction(float ef)  {m_efracsvx = ef;}
 inline void SVInfoPlus::setNormDist(float nd)        {m_normdist = nd;}
 inline void SVInfoPlus::addTrackInfo(const SVTrackInfo& i) { m_trackinfo.push_back(i); }
 //
 inline const Trk::RecVertex& SVInfoPlus::getRecSvx() const       {return m_recsvx;}
 inline long int SVInfoPlus::getNGTrackInJet() const              {return m_NGTinJet;}
 inline long int SVInfoPlus::getNGTrackInSvx() const              {return m_NGTinSvx;}
 inline long int SVInfoPlus::getN2T() const                       {return m_N2Tpair;}
 inline float    SVInfoPlus::getMass() const                      {return m_masssvx;}
 inline float    SVInfoPlus::getEnergyFraction() const            {return m_efracsvx;}
 inline float    SVInfoPlus::getNormDist() const                  {return m_normdist;}
 inline int      SVInfoPlus::numTrackInfo() const                 {return m_trackinfo.size();}
 inline const SVTrackInfo& SVInfoPlus::getTrackInfo(uint i) const {return m_trackinfo.at(i);}

inline Analysis::SVInfoPlus* Analysis::SVInfoPlus::clone() const
{
  return new SVInfoPlus(*this);
}

} // End namespace
#endif
