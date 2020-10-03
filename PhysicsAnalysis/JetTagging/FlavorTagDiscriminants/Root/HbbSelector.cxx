#include "FlavorTagDiscriminants/HbbSelector.h"

HbbSelector::HbbSelector(const std::string& prefix, double fTop):
  m_pHiggs(prefix + "_Higgs"),
  m_pQCD(prefix + "_QCD"),
  m_pTop(prefix + "_Top"),
  m_fTop(fTop)
{
}

double HbbSelector::getHbbScore(const xAOD::Jet& j) {
  return m_pHiggs(j) / ( m_fTop * m_pTop(j) + (1 - m_fTop) * m_pQCD(j));
}
