/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SVXSUMMARY_H
#define JETTAGINFO_SVXSUMMARY_H
#include "JetTagInfo/BaseTagInfo.h"
#include "VxVertex/RecVertex.h"
#include <vector>
#include <iostream>

namespace Rec { class TrackParticle; }
namespace Trk { class RecVertex; }

namespace Analysis
{

class SvxSummary : public BaseTagInfo {
 public:
  /** default constructor */
  SvxSummary();
  
  /** constructor with info type */
  SvxSummary(TagInfoType x);
  
  /** copy destructor */
  SvxSummary(const SvxSummary& svSsummary);
  
  /** assigenment operator */
  SvxSummary &operator= (const SvxSummary& rhs);
  
  /** default destructor */
  virtual ~SvxSummary() {};
  virtual SvxSummary* clone() const;

  /** Set/Get method */
  void Svx(Trk::RecVertex svx) {m_Svx = svx;};
  void Results(std::vector<double> res) {m_Results = res;};
  void TrkInSvx(std::vector<const Rec::TrackParticle*>& vtl) {m_TrkInSvx = vtl;};
  void TrkFromV0(std::vector<const Rec::TrackParticle*>& v0t) {m_TrkFromV0 = v0t;};
  Trk::RecVertex Svx() const  {return m_Svx;};
  std::vector<double> Results() const {return m_Results;};
  std::vector<const Rec::TrackParticle*> TrkInSvx() const {return m_TrkInSvx;};
  std::vector<const Rec::TrackParticle*> TrkFromV0() const {return m_TrkFromV0;};

 private:
  Trk::RecVertex m_Svx;
  std::vector<double> m_Results;
  std::vector<const Rec::TrackParticle*> m_TrkInSvx; 
  std::vector<const Rec::TrackParticle*> m_TrkFromV0;
 
}; // End class
  
inline Analysis::SvxSummary* Analysis::SvxSummary::clone() const
{
    return new SvxSummary(*this);
}

} // End namespace
#endif
