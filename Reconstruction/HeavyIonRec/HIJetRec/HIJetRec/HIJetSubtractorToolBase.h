/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetSubtractorToolBase.h

#ifndef __HIJETREC_HIJETSUBTRACTORTOOLBASE_H__
#define __HIJETREC_HIJETSUBTRACTORTOOLBASE_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetSubtractorToolBase
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Abstract base for JetModifiers applying constituent based subtraction.
///
////////////////////////////////////////////////////////////////////////////////

#include "HIJetRec/IHISubtractorTool.h"
#include "AsgTools/AsgTool.h"

class HIJetSubtractorToolBase : public asg::AsgTool, virtual public IHISubtractorTool
{
  ASG_TOOL_CLASS(HIJetSubtractorToolBase, IHISubtractorTool)
public:

  HIJetSubtractorToolBase(const std::string& myname);
  virtual StatusCode initialize();

private:

  /// %% FIX needs some cleanup
  float m_E_min_moment;
  bool m_update_clusters;
  std::vector<unsigned int> m_harmonics_vec;
  std::set<unsigned int> m_harmonics;

protected:
  inline void SetMinEnergyForMoment(float min_E) {m_E_min_moment=min_E;};
  inline void SetUpdateClusters(bool up) {m_update_clusters=up;};

  inline float MinEnergyForMoments() const {return m_E_min_moment;};
  inline bool UpdateClusters() const {return m_update_clusters;};

  inline const std::set<unsigned int>& Harmonics() const {return m_harmonics;};


};

#endif
