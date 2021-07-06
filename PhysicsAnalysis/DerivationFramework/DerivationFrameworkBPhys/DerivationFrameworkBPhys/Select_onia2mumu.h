/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Select_onia2mumu.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Select_onia2mumu_H
#define DERIVATIONFRAMEWORK_Select_onia2mumu_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "xAODBPhys/BPhysHelper.h"
#include <string>

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

namespace xAOD {
  class BPhysHypoHelper;
}

/** THE candidate selection tool
 */
namespace DerivationFramework {

  class Select_onia2mumu : public AthAlgTool, public IAugmentationTool {
    public: 
      Select_onia2mumu(const std::string& t, const std::string& n, const IInterface* p);

      /** inirialization and finalization
       */
      StatusCode initialize() override;
      
      /** @brief: augmentation and selection
       *  Retrieved vertices are augmented with usual information. 
       *  Selection is performed and each candidate is decorated with the 
       *  Char_t flag named "passed_"+name() to indicate whether if the candidate
       *  passed the selection. This flag is then used by the event selection tool
       *  and by the vertex thinning tool.
       */
      virtual StatusCode addBranches() const override;

    private:
      void ProcessVertex(xAOD::BPhysHypoHelper&, xAOD::BPhysHelper::pv_type) const;
      /** tools
       */
      ToolHandle<Trk::V0Tools> m_v0Tools;
      
      /** job options
       */
      std::string m_hypoName;               //!< name of the mass hypothesis. E.g. Jpis, Upsi, etc. Will be used as a prefix for decorations
      SG::ReadHandleKey<xAOD::VertexContainer> m_inputVtxContainerName;  //!< name of the input container name
      std::vector<double> m_trkMasses;      //!< track mass hypotheses
      double m_massHypo;                    //!< vertex mass hypothesis
      double m_massMax;                     //!< invariant mass range
      double m_massMin;                     //!< invariant mass range
      double m_chi2Max;                     //!< max chi2 cut
      double m_lxyMin;                      //!< min lxy cut
      int m_DoVertexType;                   //!< Allows user to skip certain vertexes - bitwise test 7==all(111)
      bool m_do3d;
  }; 
}

#endif // DERIVATIONFRAMEWORK_Select_onia2mumu_H
