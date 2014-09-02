/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETTOOLS_FINDLEPTONTRUTH_H
#define PARTICLEJETTOOLS_FINDLEPTONTRUTH_H

#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

#include "JetTagInfo/SoftLeptonTruthInfo.h"

class McEventCollection;
class Jet;

namespace Analysis
{

class FindLeptonTruth : public AthAlgTool {

 public:
  FindLeptonTruth(const std::string&,const std::string&,const IInterface*);
  virtual ~FindLeptonTruth();
  static const InterfaceID& interfaceID();
  StatusCode initialize();
  StatusCode finalize();

  std::vector<SLTrueInfo> FindTruth(const Jet& myJet);

 private:
  std::string m_mcEventCollection; 
  double m_deltaRCutMu, m_deltaRCutEl;
  double m_ptCutMu,     m_ptCutEl;
  double m_pCutMu,      m_pCutEl;
  double m_rProdmax;
  unsigned int m_NSLTmax;

};

class SLTrueInfoSortCriterion {
 public:
  bool operator() (const SLTrueInfo& slt1, const SLTrueInfo& slt2) {
    return slt2.momentum().perp() < slt1.momentum().perp();
  }
};

}
#endif
