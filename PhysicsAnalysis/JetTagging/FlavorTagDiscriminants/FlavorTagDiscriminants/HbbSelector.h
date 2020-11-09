#ifndef HBB_SELECTOR_H
#define HBB_SELECTOR_H

#include "xAODJet/Jet.h"

class HbbSelector {
public:
  // Constructor
  //
  // the prefix is used to access variables in the jet: it's used to
  // build accessors like `prefix + "_Higgs"`, `prefix + "_QCD"` etc.
  //
  // fTop is the 'anti-topness', 1 discriminates completely against
  // top, 0 is completely anti-QCD.
  HbbSelector(const std::string& prefix = "Xbb202006", double fTop = 0.25);
  double getHbbScore(const xAOD::Jet&);
private:
  SG::AuxElement::ConstAccessor<float> m_pHiggs;
  SG::AuxElement::ConstAccessor<float> m_pQCD;
  SG::AuxElement::ConstAccessor<float> m_pTop;
  double m_fTop;
};

#endif
