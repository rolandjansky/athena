/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// Description: Filter for ttbar as function of the pT of the hadronic top
//
// Authors:
//         I Hinchliffe:  December 2001
//         A Buckley:     April 2009

#ifndef DerivationFrameworkTop_BoostedHadTopAndTopPairFilterTool_H
#define DerivationFrameworkTop_BoostedHadTopAndTopPairFilterTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTruth/TruthEventContainer.h"
#include <iostream>
#include <cmath>

namespace DerivationFramework{

static const InterfaceID IID_BoostedHadTopAndTopPairFilterTool("BoostedHadTopAndTopPairFilterTool", 1, 0);

class BoostedHadTopAndTopPairFilterTool: public AthAlgTool {
public:
  BoostedHadTopAndTopPairFilterTool(const std::string& t, const std::string& n, const IInterface* p);
  virtual ~BoostedHadTopAndTopPairFilterTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  int filterFlag(double, double) const;

  static const InterfaceID& interfaceID() { return IID_BoostedHadTopAndTopPairFilterTool; }

private:

  std::string m_mcName;

  // properties
  // use pt cuts locally for now
  //  double m_tHadPtCut;
  //  double m_tPairPtCut;
  int m_cutPtOf;
 
  const xAOD::TruthParticle* findInitial(const xAOD::TruthParticle* part) const;
  bool  isFromTop(const xAOD::TruthParticle* part) const;
  bool  isHadronic(const xAOD::TruthParticle* part) const;
  bool  isFinalParticle(const xAOD::TruthParticle* part) const;
  double PxBofW(const xAOD::TruthParticle* part) const;
  double PyBofW(const xAOD::TruthParticle* part) const;

};

} /// namespace


#endif
