/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    @class MultiSVTag
 ***************************************************************************/

#ifndef JETTAGTOOLS_MULTISVTAG_H
#define JETTAGTOOLS_MULTISVTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include "JetTagCalibration/JetTagCalibCondData.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <string>
#include <vector>
#include <map>
#include <list>
#include "MVAUtils/BDT.h"

namespace Analysis
{  
  
  class MultiSVTag : public extends<AthAlgTool, ITagTool>
    {
    public:
      MultiSVTag(const std::string&,const std::string&,const IInterface*);
      virtual ~MultiSVTag();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;

      virtual StatusCode tagJet(const xAOD::Vertex& priVtx,
                                const xAOD::Jet& jetToTag,
                                xAOD::BTagging& BTag,
                                const std::string &jetName) const override;

      virtual void finalizeHistos() override;
      
    private:      
      std::string m_taggerName; 
      std::string m_taggerNameBase;      
      //
      std::string m_treeName;
      std::string m_varStrName;

      /** Key of calibration data: */
      SG::ReadCondHandleKey<JetTagCalibCondData> m_readKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of input (derived) JetTag calibration data"};
      std::string m_MultiSV;
      //
      std::string m_runModus; 
      std::string m_refType;
   
      int  m_warnCounter;
    
      std::vector<std::string> m_jetCollectionList;
      std::vector<std::string> m_hypotheses;
      bool m_doForcedCalib;
      std::string m_ForcedCalibName;
      std::string m_secVxFinderName;
      std::string m_xAODBaseName;
      //...
      //...
      std::string m_sv0_infosource;
      std::string m_sv1_infosource;

      struct Vars
      {
        //variables for bb tag
        float m_jetpt = 0;
        float m_nvtx = 0;
        float m_maxefrc  = 0;
        float m_summass  = 0;
        float m_totalntrk = 0;
        float m_diffntrkSV0 = -999;
        float m_diffntrkSV1 = -999;
    
        float m_normDist = 0;
        //properties of vertex with maximum (and 2nd max) mass:
        float m_mmax_mass = -9;
        float m_mmax_efrc = -9;
      
        float m_mmax_DRjet = -9;
        float m_mmax_dist  = -9;
        float m_mmx2_mass  = -9; 
        float m_mmx2_efrc  = -9;
     
        float m_mmx2_DRjet = -9;
        float m_mmx2_dist  = -9;
        // distances: max mass vertex to PV, and mx2 to max vertex
        float m_mx12_2d12  = -9; 
        float m_mx12_DR    = -9;
        float m_mx12_Angle = -9;

        void SetVariableRefs(MsgStream& msg,
                             const std::vector<std::string>& inputVars, 
                             unsigned &nConfgVar,
                             bool &badVariableFound,
                             std::vector<float*> &inputPointers);
      };

      double GetClassResponse (MVAUtils::BDT* bdt) const { return (bdt->GetPointers().size() ? bdt->GetGradBoostMVA(bdt->GetPointers()) : -9.); }
    }; // End class


} // End namespace 

#endif
