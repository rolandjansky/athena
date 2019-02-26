/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This class implements an Athena algorithm for evaluating the BDT score for tau jets.
/*!
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef TAUJETBDT_H
#define TAUJETBDT_H

#include "tauRecTools/TauRecToolBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/MethodTransform.h"
#include <string>
#include "xAODTau/TauJet.h"


class TauJetBDT: virtual public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauJetBDT, TauRecToolBase, ITauToolBase)
    public:

  
        //!< Constructor
        TauJetBDT(const std::string& name = "TauJetBDT");

        //!< Destructor
        virtual ~TauJetBDT() {}

        /**
         * @brief The boosted decision trees are built.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode initialize();

        /**
         * @brief Values of the tau parameters are extracted and a boosted decision tree score is retrieved.
         * @param tauJet a @c xAOD::TauJet pointer to a tau jet candidate.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode execute(xAOD::TauJet&);

        /**
         * @brief Allocated memory is freed.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode finalize();

	virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
	virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS;}
	virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    private:

        float m_jetScore;                     //!< Holds the current jet score which is used by a MethodCuts instance to determine if it passes loose, medium, or tight.

	std::string m_jetBDTFile;                  //!< The @c string name of the bdt file for jet rejection.
	std::string m_jetSigBitsFile;              //!< The @c string name of the file used to define the loose, medium, and tight cuts on the signal taus for jet rejection.
	std::string m_jetSigTransFile;

	TauID::MethodBDT* m_jetBDT;                  //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for jet discrimination.
	TauID::MethodCuts* m_jetSigBits;             //!< A pointer to the @c MethodCuts used to determine whether the current jet BDT score passes loose, medium, or tight signal cut.
	TauID::MethodTransform* m_jetSigTrans;

};

#endif
