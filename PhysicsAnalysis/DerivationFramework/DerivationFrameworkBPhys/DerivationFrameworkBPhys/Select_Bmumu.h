/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Select_Bmumu.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
// 
// Based on Select_onia2mumu.h.
// Original author: Daniel Scheirich <daniel.scheirich@cern.ch>
//
// Select B candidates for the B(s)mumu analysis including for
// the reference channels used.
//
// For an example see BPHY8.py .
//
// Job options provided by this class:
//                           
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_Select_Bmumu_H
#define DERIVATIONFRAMEWORK_Select_Bmumu_H

#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "xAODBPhys/BPhysHelper.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

namespace xAOD {
  class BPhysHypoHelper;
}

namespace SG {
  class AuxElement;
}

/** THE candidate selection tool
 */
namespace DerivationFramework {

  class Select_Bmumu : public CfAthAlgTool, public IAugmentationTool {
    public: 
      Select_Bmumu(const std::string& t, const std::string& n, const IInterface* p);

      /** inirialization and finalization
       */
      StatusCode initialize();
      StatusCode finalize();
      
      /** @brief: augmentation and selection
       *  Retrieved vertices are augmented with usual information. 
       *  Selection is performed and each candidate is decorated with the 
       *  Char_t flag named "passed_"+name() to indicate whether if the candidate
       *  passed the selection. This flag is then used by the event selection tool
       *  and by the vertex thinning tool.
       */
      virtual StatusCode addBranches() const;
      
    private:
      void ProcessVertex(xAOD::BPhysHypoHelper&, xAOD::BPhysHelper::pv_type) const;
      bool massCuts(float mass) const;

      bool pass(const SG::AuxElement& em, std::string hypo) const;
      bool setPass(SG::AuxElement& em, std::string hypo, bool passVal) const;
      bool setPassIfNotAvailable(SG::AuxElement& em, std::string hypo,
				 bool passVal) const;
      // std::vector<xAOD::Vertex*> getPrecedingVertices(const xAOD::Vertex* vtx);
      
      /** tools
       */
      ToolHandle<Trk::V0Tools> m_v0Tools;
      
      /** job options
       */
      std::string m_hypoName;               //!< name of the mass hypothesis. E.g. Jpis, Upsi, etc. Will be used as a prefix for decorations
      std::string m_inputVtxContainerName;  //!< name of the input container name
      std::vector<double> m_trkMasses;      //!< track mass hypotheses
      double m_massHypo;                    //!< vertex mass hypothesis
      double m_massMax;                     //!< invariant mass range
      double m_massMin;                     //!< invariant mass range
      double m_chi2Max;                     //!< max chi2 cut
      int m_DoVertexType;                   //!< Allows user to skip certain vertexes - bitwise test 7==all(111)
      double m_blindMassMin;                //!< blinding mass range
      double m_blindMassMax;                //!< blinding mass range
      bool   m_doBlinding;                  //!< enable blinding range
      bool   m_useMuCalcMass;               //!< also check against MUCALC mass

      std::vector<std::string> m_subDecVtxContNames; //!< names of sub-decay vertex containers
      std::vector<std::string> m_subDecVtxHypoCondNames; //!< hypo names for sub-decays to be considered
      std::vector<std::string> m_subDecVtxHypoFlagNames; //!< names of hypo flags set on sub-decays if passing
  }; 
}

#endif // DERIVATIONFRAMEWORK_Select_Bmumu_H
