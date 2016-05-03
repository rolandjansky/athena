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

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/MethodTransform.h"
#include <string>
#include "xAODTau/TauJet.h"


class TauJetBDT: virtual public TauDiscriToolBase
{
  ASG_TOOL_CLASS2(TauJetBDT, TauDiscriToolBase, ITauToolBase)
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

    private:

        float jetScore;                     //!< Holds the current jet score which is used by a MethodCuts instance to determine if it passes loose, medium, or tight.

	std::string jetBDTFile;                  //!< The @c string name of the bdt file for jet rejection.
	std::string jetSigBitsFile;              //!< The @c string name of the file used to define the loose, medium, and tight cuts on the signal taus for jet rejection.
	std::string jetSigTransFile;

	TauID::MethodBDT* jetBDT;                  //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for jet discrimination.
	TauID::MethodCuts* jetSigBits;             //!< A pointer to the @c MethodCuts used to determine whether the current jet BDT score passes loose, medium, or tight signal cut.
	TauID::MethodTransform* jetSigTrans;

};

#endif
