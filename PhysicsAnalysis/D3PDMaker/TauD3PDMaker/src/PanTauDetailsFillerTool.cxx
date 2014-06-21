/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PanTauDetailsFillerTool.cxx 528845 2012-12-04 15:07:38Z tibristo $
/**
 * @file D3PDMaker/TauD3PDMaker/src/PanTauDetailsFillerTool.cxx
 * @author Sebastian Fleischmann
 * @date Jul, 2010
 * @brief Block filler tool for PanTau details.
 */


#include "PanTauDetailsFillerTool.h"
#include "tauEvent/TauJet.h"
#include "PanTauEvent/PanTauDetails.h"
#include "PanTauEvent/TauFeature.h"
#include "PanTauEvent/TauSeed.h"
#include "AthenaKernel/errorcheck.h"
#include "TRandom2.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PanTauDetailsFillerTool::PanTauDetailsFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<Analysis::TauJet> (type, name, parent),
    m_randomNumbers(0),
    m_smearDefaultValues(false)
{
    declareProperty("FeatureNames",                         m_featureNames,                         "vector of names of features to write out");
    declareProperty("FeatureDefaultValues",                 m_featureDefaultValues,                 "vector of default values of features to write out");
    declareProperty("FeatureDefaultValueSmearingSigma",     m_featureDefaultValueSmearingSigma,     "vector of standard deviations to smear default values of features");
    declareProperty("SmearDefaultValuesOfFeatures",         m_smearDefaultValues,                   "smear the default values of features?");
}


/**
 * @brief Book variables for this block.
 */
StatusCode PanTauDetailsFillerTool::book()
{
    //do not attempt to book any variables if there are no features to be dumped
    if(m_featureNames.size() == 0) {
        ATH_MSG_WARNING("The list of features to extracted is zero -> quit extraction of PanTauFetaures");
        return StatusCode::SUCCESS;
    }
    
    if (m_featureNames.size() != m_featureDefaultValues.size()) {
        ATH_MSG_ERROR( "job option problem: vector sizes of feature names and default values do not match.");
        return StatusCode::FAILURE;
    }
    m_seedFeatureBranches.resize(m_featureNames.size(), 0);
    
    for (unsigned int featureIndex = 0; featureIndex<m_featureNames.size(); ++featureIndex) {
        std::string VarToAdd = "pantauFeature_" + m_featureNames[featureIndex];
        CHECK( addVariable (VarToAdd,   m_seedFeatureBranches[featureIndex])  );
    }

    m_randomNumbers = new TRandom2();
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
StatusCode PanTauDetailsFillerTool::fill (const Analysis::TauJet& p)
{
    ATH_MSG_DEBUG( "PanTauDetailsFillerTool running!");
    
    const PanTau::PanTauDetails*  p_taudetails(0);
    p_taudetails = p.details<const PanTau::PanTauDetails>();
    if (!p_taudetails){
        ATH_MSG_DEBUG( "TauJet has no associated PanTauDetails!!");
        return StatusCode::SUCCESS;
    }
    
    //do not attempt to book any variables if there are no features to be dumped
    if(m_featureNames.size() == 0) {
        ATH_MSG_DEBUG("The list of features to be extracted is empty -> quit extraction of PanTauFetaures");
        return StatusCode::SUCCESS;
    }
    
    /// add 
    const PanTau::TauFeature* seedFeatures = p_taudetails->features();
    if (!seedFeatures) {
        ATH_MSG_DEBUG(  "could not get features from PanTauDetails");// of type(s): "<< seed->dumpType() ;
        for (unsigned int featureIndex = 0; featureIndex<m_featureNames.size(); featureIndex++) {
            if (m_smearDefaultValues && (m_featureDefaultValueSmearingSigma[featureIndex] > 0.)) {
                *(m_seedFeatureBranches[featureIndex]) = m_randomNumbers->Gaus(m_featureDefaultValues[featureIndex], m_featureDefaultValueSmearingSigma[featureIndex]);
            } else {
                *(m_seedFeatureBranches[featureIndex]) = m_featureDefaultValues[featureIndex];
            }
        }
    } else {
        ATH_MSG_DEBUG( "   found features for current tau -- good!");
        bool isValid=false;
        ATH_MSG_DEBUG( "   will loop over m_featureNames.size() = " << m_featureNames.size() << " features");
        ATH_MSG_DEBUG( "   compare to seedFeatures.size() = " << seedFeatures->nValues());
        for (unsigned int featureIndex = 0; featureIndex<m_featureNames.size(); featureIndex++) {
            float value = seedFeatures->value(m_featureNames[featureIndex], isValid);
            if (std::isnan(value)) {
                ATH_MSG_WARNING( "Got NaN for " << m_featureNames[featureIndex] << ": " << seedFeatures->value(m_featureNames[featureIndex], isValid));
                isValid = false;
            }
            if (std::isinf(value)) {
                ATH_MSG_WARNING( "Got inf for " << m_featureNames[featureIndex] << ": " << seedFeatures->value(m_featureNames[featureIndex], isValid));
                isValid = false;
            }
            if (!isValid) {
                if (m_smearDefaultValues && (m_featureDefaultValueSmearingSigma[featureIndex] > 0.)) {
                    value = m_randomNumbers->Gaus(m_featureDefaultValues[featureIndex], m_featureDefaultValueSmearingSigma[featureIndex]);
                } else {
                    value = m_featureDefaultValues[featureIndex];
                }
            } // end if !isValid
            *(m_seedFeatureBranches[featureIndex]) = value;
        } // end for loop over featureIndex
    }

    return StatusCode::SUCCESS;
}

StatusCode PanTauDetailsFillerTool::finalize() {
    delete m_randomNumbers;
    return StatusCode::SUCCESS;
}

} // namespace D3PD
