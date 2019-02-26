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

#include "tauRecTools/TauRecToolBase.h"
#include "TauDiscriminant/MethodBDT.h"
#include "TauDiscriminant/MethodCuts.h"
#include "TauDiscriminant/MethodTransform.h"
#include <string>
#include <PathResolver/PathResolver.h>
#include "xAODTau/TauJet.h"
#include "TFile.h"
#include "TH2F.h"

//using namespace std;
//using namespace TauID;

class TauEleBDT: virtual public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauEleBDT, TauRecToolBase, ITauToolBase)
    public:

        //!< Constructor
        TauEleBDT(const std::string& name = "TauEleBDT"):
            TauRecToolBase(name),
	    m_eleScore(-1.),
            m_eleBDTFile(""),
            m_eleBitsFile(""),
            m_eleBitsRootFile(""),
	    m_cutsFile(NULL),
	    m_hloose(NULL), m_hmedium(NULL), m_htight(NULL),
            m_eleBDT(NULL),
            m_eleBits(NULL)
    {
        declareProperty("eleBDT", this->m_eleBDTFile);
        declareProperty("eleBits", this->m_eleBitsFile);
        declareProperty("eleBitsRoot", this->m_eleBitsRootFile);
        declareProperty("onlyDecorateScore", this->m_onlyDecorateScore=false);
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
        virtual StatusCode decorate(xAOD::TauJet&);
        virtual StatusCode execute(xAOD::TauJet&);

        /**
         * @brief Allocated memory is freed.
         * @return @c StatusCode StatusCode::FAILURE if there were problems and StatusCode::SUCCESS otherwise.
         */
        virtual StatusCode finalize();

	virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
	virtual	StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual	StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
	virtual	StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS;}
	virtual	StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual	StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    private:

        float m_eleScore;                     //!< Holds the current electron score which is used by a MethodCuts instance to determine if it passes loose, medium, or tight.
        bool m_onlyDecorateScore;           //!< Only decorate the score

        std::string m_eleBDTFile;                  //!< The @c string name of the bdt file for electron rejection.
        std::string m_eleBitsFile;                 //!< The @c string name of the file used to define the loose, medium, and tight cuts for electron rejection.
        std::string m_eleBitsRootFile;             //!< The @c string name of the ROOT file used to define the loose, medium, and tight cuts for electron rejection.
	TFile *m_cutsFile;
	TH2F *m_hloose, *m_hmedium, *m_htight;    //!< Histograms storing eta/pt for loose/medium/tight cuts
        TauID::MethodBDT* m_eleBDT;                  //!< A pointer to the @c MethodBDT used to construct and evaluate BDTs used for electron discrimination.
        TauID::MethodCuts* m_eleBits;                //!< A pointer to the @c MethodCuts used to determine whether the current electron BDT score passes loose, medium, or tight cut.
};

#endif
