/**
 * @file DebugTutorial/src/TestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief debugging tutorial
 */


#include "TestAlg.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TH1F.h"
#include <vector>
#include <string>


using CLHEP::GeV;


namespace Tutorial {


/// Standard Gaudi @c initialize method.
StatusCode TestAlg::initialize()
{
  ATH_CHECK( m_jetsKey.initialize() );

  // Build the pair index lookup table.
  for (int i=0; i<6; i++) {
    for (int j=0; j<=i; j++) {
      m_pairtable.push_back(i);
      m_pairtable.push_back(j);
    }
    m_end.push_back (m_pairtable.size());
  }
  return StatusCode::SUCCESS;
}


/// Standard Gaudi @c finalize method.
StatusCode TestAlg::finalize()
{
  m_hist->Print("ALL");
  return StatusCode::SUCCESS;
}


// Generate a label for jet I.
std::string jetlab (int i)
{
  char* buf = new char[16];
  // 15 chars +  null byte
  sprintf (buf, "jet %1d xverse pt", i);
  std::string s(buf);
  delete [] buf;
  return s;
}


/// Standard Gaudi @c execute method.
StatusCode TestAlg::execute()
{
  // First dump jets.
  int i = 0;
  SG::ReadHandle<xAOD::JetContainer> jets (m_jetsKey);
  for (const xAOD::Jet* jet : *jets) {
    msg(MSG::VERBOSE) << jetlab(i) << " " << jet->auxdata<float> ("Pt") << endmsg;
    ++i;
  }

  // Fill the histogram with invariant masses of jet pairs from the table.
  unsigned int njet = jets->size();
  if (njet > 1) {
    if (njet > m_end.size())
      njet = m_end.size();
    for (int i=0; i < m_end[njet]; i += 2) {
      const xAOD::Jet* j1 = (*jets)[m_pairtable[i]];
      const xAOD::Jet* j2 = (*jets)[m_pairtable[i+1]];
      float minv = (j1->p4() + j2->p4()).M();
      m_hist->Fill (minv / GeV);
    }
  }
  return StatusCode::SUCCESS;
}


} // namespace TestAlg
