/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_MULTISVINFOPLUS_H
#define JETTAGINFO_MULTISVINFOPLUS_H
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/MSVVtxInfo.h"
#include <vector>
#include <iostream>

namespace Analysis
{

class MultiSVInfoPlus : public BaseTagInfo
{
 public:
  /** default constructor */
  MultiSVInfoPlus();
  
  /** constructor with info type */
  MultiSVInfoPlus(TagInfoType x);
  
  /** copy destructor */
  MultiSVInfoPlus(const MultiSVInfoPlus& MultiSVInfoPlus);
  
  /** assigenment operator */
  MultiSVInfoPlus &operator= (const MultiSVInfoPlus& rhs);
  MultiSVInfoPlus &operator= (MultiSVInfoPlus&& rhs);
  
  /** default destructor */
  virtual ~MultiSVInfoPlus();
  virtual MultiSVInfoPlus* clone() const;

  /** Set methods */
  void setNGTrackInJet(long int);
  void setNGTrackInSvx(long int);
  void setN2T(long int);
  void setNormDist(float);
  void addVtxInfo(MSVVtxInfo*); 
  void addVtxInfo(std::unique_ptr<MSVVtxInfo>); 

  /** Get methods */
  long int              getNGTrackInJet() const;
  long int              getNGTrackInSvx() const;
  long int              getN2T() const;
  float                 getNormDist() const;
  int                   numVtxInfo() const;
  const MSVVtxInfo*     getVtxInfo(unsigned int i) const;

 private:
  long int       m_NGTinJet;
  long int       m_NGTinSvx;
  long int       m_N2Tpair;
  float          m_normdist;
  std::vector<MSVVtxInfo*> m_vtxInfo; /// need a vector of pointer to be able to link to in the d3pd
                                      /// even if it makes the copy constructor non trivial
  
}; // End class
  
 inline void MultiSVInfoPlus::setNGTrackInJet(long int li) {m_NGTinJet = li;}
 inline void MultiSVInfoPlus::setNGTrackInSvx(long int li) {m_NGTinSvx = li;}
 inline void MultiSVInfoPlus::setN2T(long int li)          {m_N2Tpair  = li;}
 inline void MultiSVInfoPlus::setNormDist(float nd)        {m_normdist = nd;}
 inline void MultiSVInfoPlus::addVtxInfo(MSVVtxInfo* i) { m_vtxInfo.push_back(i); }
 inline void MultiSVInfoPlus::addVtxInfo(std::unique_ptr<MSVVtxInfo> i) { m_vtxInfo.push_back(i.release()); }
 //
 inline long int MultiSVInfoPlus::getNGTrackInJet() const              {return m_NGTinJet;}
 inline long int MultiSVInfoPlus::getNGTrackInSvx() const              {return m_NGTinSvx;}
 inline long int MultiSVInfoPlus::getN2T() const                       {return m_N2Tpair;}
 inline float    MultiSVInfoPlus::getNormDist() const                  {return m_normdist;}
 inline int      MultiSVInfoPlus::numVtxInfo() const                 {return m_vtxInfo.size();}
 inline const MSVVtxInfo* MultiSVInfoPlus::getVtxInfo(uint i) const {return m_vtxInfo.at(i);}

inline Analysis::MultiSVInfoPlus* Analysis::MultiSVInfoPlus::clone() const
{
  return new MultiSVInfoPlus(*this);
}

} // End namespace
#endif
