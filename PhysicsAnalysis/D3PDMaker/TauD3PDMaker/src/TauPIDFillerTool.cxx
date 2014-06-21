/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauPIDFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date Sept, 2009
 * @brief Block filler tool for tau PID.
 */


#include "TauPIDFillerTool.h"
#include "tauEvent/TauJet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauPIDFillerTool::TauPIDFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<Analysis::TauJet> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode TauPIDFillerTool::book()
{
    CHECK( addVariable ("BDTEleScore",   m_BDTEleScore)  );
    CHECK( addVariable ("BDTJetScore",   m_BDTJetScore)  );
    CHECK( addVariable ("likelihood",       m_likelihood)  );
    CHECK( addVariable ("SafeLikelihood",   m_SafeLikelihood)  );
    /// now the bits
    CHECK( addVariable ("electronVetoLoose",   m_electronVetoLoose)  );
    CHECK( addVariable ("electronVetoMedium",   m_electronVetoMedium)  );
    CHECK( addVariable ("electronVetoTight",   m_electronVetoTight)  );
    CHECK( addVariable ("muonVeto",   m_muonVeto)  );
    // CHECK( addVariable ("tauCutLoose",   m_tauCutLoose)  );
    // CHECK( addVariable ("tauCutMedium",   m_tauCutMedium)  );
    // CHECK( addVariable ("tauCutTight",   m_tauCutTight)  );
    CHECK( addVariable ("tauLlhLoose",   m_tauLlhLoose)  );
    CHECK( addVariable ("tauLlhMedium",   m_tauLlhMedium)  );
    CHECK( addVariable ("tauLlhTight",   m_tauLlhTight)  );
    CHECK( addVariable ("JetBDTSigLoose",   m_JetBDTSigLoose)  );
    CHECK( addVariable ("JetBDTSigMedium",   m_JetBDTSigMedium)  );
    CHECK( addVariable ("JetBDTSigTight",   m_JetBDTSigTight)  );
    CHECK( addVariable ("EleBDTLoose",   m_EleBDTLoose)  );
    CHECK( addVariable ("EleBDTMedium",   m_EleBDTMedium)  );
    CHECK( addVariable ("EleBDTTight",   m_EleBDTTight)  );


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
StatusCode TauPIDFillerTool::fill (const Analysis::TauJet& p)
{
    *m_BDTEleScore =         p.tauID()->discriminant(TauJetParameters::BDTEleScore);
    *m_BDTJetScore =         p.tauID()->discriminant(TauJetParameters::BDTJetScore);
    *m_likelihood =          p.tauID()->discriminant(TauJetParameters::Likelihood);
    *m_SafeLikelihood =      p.tauID()->discriminant(TauJetParameters::SafeLikelihood);

    /// Now the bits
    *m_electronVetoLoose =     p.tauID()->isTau(TauJetParameters::ElectronVetoLoose);
    *m_electronVetoMedium =    p.tauID()->isTau(TauJetParameters::ElectronVetoMedium);
    *m_electronVetoTight =     p.tauID()->isTau(TauJetParameters::ElectronVetoTight);
    *m_muonVeto =              p.tauID()->isTau(TauJetParameters::MuonVeto);
    // *m_tauCutLoose =           p.tauID()->isTau(TauJetParameters::TauCutLoose);
    // *m_tauCutMedium =          p.tauID()->isTau(TauJetParameters::TauCutMedium);
    // *m_tauCutTight =           p.tauID()->isTau(TauJetParameters::TauCutTight);
    *m_tauLlhLoose =           p.tauID()->isTau(TauJetParameters::TauLlhLoose);
    *m_tauLlhMedium =          p.tauID()->isTau(TauJetParameters::TauLlhMedium);
    *m_tauLlhTight =           p.tauID()->isTau(TauJetParameters::TauLlhTight);
    *m_JetBDTSigLoose =           p.tauID()->isTau(TauJetParameters::JetBDTSigLoose);
    *m_JetBDTSigMedium =          p.tauID()->isTau(TauJetParameters::JetBDTSigMedium);
    *m_JetBDTSigTight =           p.tauID()->isTau(TauJetParameters::JetBDTSigTight);
    *m_EleBDTLoose =           p.tauID()->isTau(TauJetParameters::EleBDTLoose);
    *m_EleBDTMedium =          p.tauID()->isTau(TauJetParameters::EleBDTMedium);
    *m_EleBDTTight =           p.tauID()->isTau(TauJetParameters::EleBDTTight);    

  return StatusCode::SUCCESS;
}


} // namespace D3PD
