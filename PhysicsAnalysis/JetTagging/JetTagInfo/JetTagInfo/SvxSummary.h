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
  void Svx(const Trk::RecVertex& svx) {m_Svx = svx;};
  void Results(const std::vector<double>& res) {m_Results = res;};
  void Results(std::vector<double>&& res) {m_Results = std::move(res);};

  void TrkInSvx(const std::vector<const Rec::TrackParticle*>& vtl) {m_TrkInSvx = vtl;};
  void TrkInSvx(std::vector<const Rec::TrackParticle*>&& vtl) {m_TrkInSvx = std::move(vtl);};

  void TrkFromV0(const std::vector<const Rec::TrackParticle*>& v0t) {m_TrkFromV0 = v0t;};
  void TrkFromV0(std::vector<const Rec::TrackParticle*>&& v0t) {m_TrkFromV0 = std::move(v0t);};

  const Trk::RecVertex& Svx() const  {return m_Svx;};
  const std::vector<double>& Results() const {return m_Results;};
  const std::vector<const Rec::TrackParticle*>& TrkInSvx() const {return m_TrkInSvx;};
  const std::vector<const Rec::TrackParticle*>& TrkFromV0() const {return m_TrkFromV0;};

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
