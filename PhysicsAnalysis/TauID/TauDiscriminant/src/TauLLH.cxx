/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**                                       
 * file: TauLLH.cxx                    
 *                                        
 * Author: Martin Flechl (mflechl@cern.ch)
 *
 */

#include "TauDiscriminant/TauLLH.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
// #include "tauEvent/TauCommonDetails.h"
// #include "tauEvent/TauPID.h"
// #include "tauEvent/TauJetParameters.h"
#include "xAODTau/TauDefs.h"
#include <PathResolver/PathResolver.h>

using namespace xAOD;

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauLLH::prepare(const TauDetailsManager& manager)
{
    //defllh cannot be used yet
    //  m_defllh = new MethodLLH("llhdef");
    m_safellh = new MethodLLH("llhsafe");
    //m_safellh = new MethodLLH("llhsafe",true);

    //  m_defllh->setDetails(this->manager);
    m_safellh->setDetails(manager);

    std::string jetPDFPath = PathResolver::find_file(m_fileNameJetPDF, "DATAPATH");
    std::string tauPDFPath = PathResolver::find_file(m_fileNameTauPDF, "DATAPATH");
    std::string LMTCutsPath = PathResolver::find_file(m_fileNameLMTCuts, "DATAPATH");


    std::string fileNames=tauPDFPath+","+jetPDFPath+","+LMTCutsPath;
    if ( !m_safellh->build(fileNames) ) {
        ATH_MSG_FATAL("unable to build safe likelihood");
        return StatusCode::FAILURE;
    }

	// set trigger flag
	m_safellh->setDoTrigger(manager);
    
	return StatusCode :: SUCCESS;
}


//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauLLH :: finalize()
{
    //  delete m_defllh;
    delete m_safellh;
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauLLH::execute(xAOD::TauJet* tau, FakeTauBits* /*bits*/, FakeTauScores* /*scores*/)
{
//     Analysis::TauPID *p_tauid = tau->tauID();

    bool loose = m_safellh->response(0);
    bool medium = m_safellh->response(1);
    bool tight = m_safellh->response(2);
    float value = m_safellh->response(3); 

    //default llh value
    tau->setIsTau(TauJetParameters::TauLlhLoose, loose);
    tau->setIsTau(TauJetParameters::TauLlhMedium, medium);
    tau->setIsTau(TauJetParameters::TauLlhTight, tight);
    tau->setDiscriminant(TauJetParameters::Likelihood, value);
    tau->setDiscriminant(TauJetParameters::SafeLikelihood, value);

    return StatusCode :: SUCCESS;
} 
