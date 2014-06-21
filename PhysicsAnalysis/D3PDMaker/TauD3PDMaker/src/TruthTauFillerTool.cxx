/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TruthTauFillerTool.cxx
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date December, 2009
 * @brief Block filler tool for taujet.
 */


#include "TruthTauFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "McParticleEvent/TruthParticle.h"
//#include "Vector/LorentzVector.h"


using CLHEP::HepLorentzVector;


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthTauFillerTool::TruthTauFillerTool (const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
  : Base(type, name, parent),
    m_truth_handler()
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode TruthTauFillerTool::initialize()
{
    m_truth_handler.SetProngCount(true);
    return Base::initialize();
}


/**
 * @brief Book variables for this block.
 */
StatusCode TruthTauFillerTool::book()
{
    CHECK( addVariable ("vis_m",  m_vis_m )  );
    CHECK( addVariable ("vis_Et",  m_vis_Et )  );
    CHECK( addVariable ("vis_eta",  m_vis_eta )  );
    CHECK( addVariable ("vis_phi",  m_vis_phi )  );
    CHECK( addVariable ("nProng",  m_nProng )  );
    CHECK( addVariable ("nPi0",  m_nPi0 )  );
    CHECK( addVariable ("charge",  m_charge )  );
    return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TruthTauFillerTool::fill (const TruthParticle& p)
{
    HepLorentzVector true_vis_tau = m_truth_handler.getTauVisibleSumTruth(&p);
    bool useKaons = true;
    *m_vis_m = true_vis_tau.m();
    *m_vis_Et = true_vis_tau.et();
    *m_vis_eta = true_vis_tau.eta();
    *m_vis_phi = true_vis_tau.phi();
    *m_nProng = m_truth_handler.nProngTruth(&p,useKaons);
    *m_nPi0 = m_truth_handler.numNeutPion(&p);
    *m_charge = p.charge();
    return StatusCode::SUCCESS;
}


} // namespace D3PD
