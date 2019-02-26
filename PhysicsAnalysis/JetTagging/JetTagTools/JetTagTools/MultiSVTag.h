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
  
  class MultiSVTag : public AthAlgTool , virtual public ITagTool
    {
    public:
      MultiSVTag(const std::string&,const std::string&,const IInterface*);
      virtual ~MultiSVTag();
      StatusCode initialize();
      StatusCode finalize();      

      StatusCode tagJet(const xAOD::Jet* jetToTag, xAOD::BTagging * BTag);
      void setOrigin(const xAOD::Vertex* priVtx);
      void finalizeHistos();
      
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
      const xAOD::Vertex* m_priVtx = 0;
   
      bool m_disableAlgo;
      int  m_warnCounter;
    
      std::vector<std::string> m_jetCollectionList;
      std::vector<std::string> m_hypotheses;
      bool m_doForcedCalib;
      std::string m_ForcedCalibName;
      std::string m_secVxFinderName;
      std::string m_xAODBaseName;
      //...
      //variables for bb tag
      float m_jetpt;
      float m_nvtx;
      float m_maxefrc;
      float m_summass;
      float m_totalntrk;
      float m_diffntrkSV0;
      float m_diffntrkSV1;
    
      float m_normDist;
      //properties of vertex with maximum (and 2nd max) mass:
      float m_mmax_mass ;
      float m_mmax_efrc ;
      
      float m_mmax_DRjet;
      float m_mmax_dist ;
      float m_mmx2_mass ; 
      float m_mmx2_efrc ;
     
      float m_mmx2_DRjet;
      float m_mmx2_dist ;
       // distances: max mass vertex to PV, and mx2 to max vertex
      float m_mx12_2d12 ; 
      float m_mx12_DR   ;
      float m_mx12_Angle;
      //...
      std::map<std::string, MVAUtils::BDT*> m_egammaBDTs;
      std::list<std::string> m_undefinedReaders;
      std::string m_sv0_infosource;
      std::string m_sv1_infosource;

      void SetVariableRefs(const std::vector<std::string> inputVars, 
			   unsigned &nConfgVar, bool &badVariableFound, std::vector<float*> &inputPointers);
      double GetClassResponse (MVAUtils::BDT* bdt) const { return (bdt->GetPointers().size() ? bdt->GetGradBoostMVA(bdt->GetPointers()) : -9.); }
    }; // End class


  inline void MultiSVTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }

} // End namespace 

#endif
