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
#include "TauDiscriminant/TauDetailsManager.h"
#include <string>
#include <PathResolver/PathResolver.h>
#include "xAODTau/TauJet.h"

using namespace std;
using namespace TauID;

class TauJetBDT: public TauDiscriToolBase
{
    public:

        //!< Constructor
        TauJetBDT(const string& type,
                const string& name,
                const IInterface* parent):
            TauDiscriToolBase(type, name, parent),
            jetBDTFile(""),
            jetSigBitsFile(""),
            jetBkgBitsFile(""),
            jetSigTransFile(""),
            jetBkgTransFile(""),
            jetBDT(NULL),
            jetSigBits(NULL),
            jetBkgBits(NULL),
            jetSigTrans(NULL),
            jetBkgTrans(NULL)
    {
        declareInterface<TauDiscriToolBase>(this);

        declareProperty("jetBDT", this->jetBDTFile);
        declareProperty("jetSigBits",this->jetSigBitsFile);
        declareProperty("jetBkgBits",this->jetBkgBitsFile);
        declareProperty("jetSigTrans",this->jetSigTransFile);
        declareProperty("jetBkgTrans",this->jetBkgTransFile);
    }

        //!< Destructor
        virtual ~TauJetBDT() {}

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

        float jetScore;                     //!< Holds the current jet score which is used by a MethodCuts instance to determine if it passes loose, medium, or tight.

        string jetBDTFile;                  //!< The @c string name of the bdt file for jet rejection.
        string jetSigBitsFile;              //!< The @c string name of the file used to define the loose, medium, and tight cuts on the signal taus for jet rejection.
        string jetBkgBitsFile;              //!< The @c string name of the file used to define the loose, medium, and tight cuts on the background jets for jet rejection.
        string jetSigTransFile;
        string jetBkgTransFile;

        MethodBDT* jetBDT;                  //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for jet discrimination.
        MethodCuts* jetSigBits;             //!< A pointer to the @c MethodCuts used to determine whether the current jet BDT score passes loose, medium, or tight signal cut.
        MethodCuts* jetBkgBits;             //!< A pointer to the @c MethodCuts used to determine whether the current jet BDT score passes loose, medium, or tight background cut.
        MethodTransform* jetSigTrans;
        MethodTransform* jetBkgTrans;
};

#endif
