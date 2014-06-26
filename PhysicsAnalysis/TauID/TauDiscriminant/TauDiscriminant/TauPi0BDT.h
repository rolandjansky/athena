/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This class implements an Athena algorithm for evaluating the BDT score for pi0s.
/*!
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef TAUPI0BDT_H
#define TAUPI0BDT_H

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include <string>
#include <PathResolver/PathResolver.h>
#include "xAODTau/TauJet.h"

using namespace std;
using namespace TauID;

class TauPi0BDT: public TauDiscriToolBase
{
    public:

        //!< Constructor
        TauPi0BDT(const string& type,
                  const string& name,
                  const IInterface* parent):
            TauDiscriToolBase(type, name, parent),
            pi0BDTPrimaryFile(""),
            pi0BDTSecondaryFile(""),
            pi0BDTPrimary(NULL),
            pi0BDTSecondary(NULL)
    {
        declareInterface<TauDiscriToolBase>(this);

        declareProperty("pi0BDTPrimary", this->pi0BDTPrimaryFile);
        declareProperty("pi0BDTSecondary", this->pi0BDTSecondaryFile);
    }

        //!< Destructor
        virtual ~TauPi0BDT() {}

        /**
         * @brief The boosted decision trees are built.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode prepare(const TauDetailsManager&);

        /**
         * @brief Values of the tau parameters are extracted and a boosted decision tree score is retrieved.
         * @param tauJet a @c xAOD::TauJet pointer to a tau jet candidate.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode execute(xAOD::TauJet*, FakeTauBits*, FakeTauScores*);

        /**
         * @brief Allocated memory is freed.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode finalize();

    private:

        string pi0BDTPrimaryFile;           //!< The @c string name of the bdt file for establishing the presence of pi0s
        string pi0BDTSecondaryFile;         //!< The @c string name of the bdt file for differentiating 2 from 1 pi0s

        MethodBDT* pi0BDTPrimary;           //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for establishing the presence of pi0s
        MethodBDT* pi0BDTSecondary;         //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for differentiating 2 from 1 pi0s
};

#endif
