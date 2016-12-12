/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagJetPtScaling.h"
#include "xAODJet/Jet.h"

namespace Analysis {

  BTagJetPtScaling::BTagJetPtScaling(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p)
  {
      declareInterface<IBTagJetPtScaling>(this);
//      declareProperty( "doPtScaling",        m_doPtScaling = false,      "decide whether to carry out rescaling or not");
      declareProperty( "PtScalingConstPar",  m_PtScalingConstPar = 8000, "offset for track jet to calorimeter jet pt scaling");
      declareProperty( "PtScalingLinearPar", m_PtScalingLinearPar = 1.4, "track jet to calorimeter jet pt scale");
  }

  BTagJetPtScaling::~BTagJetPtScaling() {
  }

  StatusCode BTagJetPtScaling::initialize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode BTagJetPtScaling::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode BTagJetPtScaling::BTagJetPtScaling_exec(xAOD::JetContainer& jets) const {

    /* ----------------------------------------------------------------------------------- */
    /*               Rescale jet pt values                                                 */
    /* ----------------------------------------------------------------------------------- */

//    if (! m_doPtScaling) return StatusCode::SUCCESS;

    for (auto jetit = jets.begin(); jetit != jets.end(); ++jetit) {
      xAOD::Jet* jet = *jetit;
      // retrieve jet four-momentum
      double pt = jet->pt(), eta = jet->eta(), phi = jet->phi(), m = jet->m();
      // rescale
      double ptRescaled = m_PtScalingConstPar + m_PtScalingLinearPar*pt;
      // and modify the jet with the new pt
      jet->setJetP4(xAOD::JetFourMom_t(ptRescaled, eta, phi, m));

      ATH_MSG_VERBOSE("Jet pt before rescaling: " << pt << ", afterwards: " << jet->pt());

    }

    return StatusCode::SUCCESS;
   }

} // namespace
