/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! This class implements an Athena algorithm for evaluating the BDT score for tau jets.
/*!
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef TAUELEBDT_H
#define TAUELEBDT_H

#include "TauDiscriminant/TauDiscriToolBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/MethodTransform.h"
#include <string>
#include <PathResolver/PathResolver.h>
#include "xAODTau/TauJet.h"
#include "TFile.h"
#include "TH2F.h"

using namespace std;
using namespace TauID;

class TauEleBDT: virtual public TauDiscriToolBase
{
  ASG_TOOL_CLASS2(TauEleBDT, TauDiscriToolBase, ITauToolBase)
    public:

        //!< Constructor
        TauEleBDT(const string& name = "TauEleBDT"):
            TauDiscriToolBase(name),
	    eleScore(-1.),
            eleBDTFile(""),
            eleBitsFile(""),
            eleBitsRootFile(""),
	    cutsFile(NULL),
	    hloose(NULL), hmedium(NULL), htight(NULL),
            eleBDT(NULL),
            eleBits(NULL)
    {
        declareProperty("eleBDT", this->eleBDTFile);
        declareProperty("eleBits", this->eleBitsFile);
        declareProperty("eleBitsRoot", this->eleBitsRootFile);
    }

        //!< Destructor
        virtual ~TauEleBDT() {}

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

        float eleScore;                     //!< Holds the current electron score which is used by a MethodCuts instance to determine if it passes loose, medium, or tight.

        string eleBDTFile;                  //!< The @c string name of the bdt file for electron rejection.
        string eleBitsFile;                 //!< The @c string name of the file used to define the loose, medium, and tight cuts for electron rejection.
        string eleBitsRootFile;             //!< The @c string name of the ROOT file used to define the loose, medium, and tight cuts for electron rejection.
	TFile *cutsFile;
	TH2F *hloose, *hmedium, *htight;    //!< Histograms storing eta/pt for loose/medium/tight cuts
        MethodBDT* eleBDT;                  //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for electron discrimination.
        MethodCuts* eleBits;                //!< A pointer to the @c MethodCuts used to determine whether the current electron BDT score passes loose, medium, or tight cut.
};

#endif
