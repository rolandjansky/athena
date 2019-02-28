/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    @class SVTag
 ***************************************************************************/

#ifndef JETTAGTOOLS_SVTAG_H
#define JETTAGTOOLS_SVTAG_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <map>


//namespace xAOD { class TrackParticle; class TrackParticleContainer; }
//namespace Trk { class RecVertex;}
//class Jet;
class StoreGateSvc;

namespace Analysis
{  
  class SVInfo;
  class NewLikelihoodTool;
  class HistoHelperRoot;
  
  class SVTag : public AthAlgTool , virtual public ITagTool
    {
    public:
      SVTag(const std::string&,const std::string&,const IInterface*);
      virtual ~SVTag();
      StatusCode initialize();
      StatusCode finalize();
      
      StatusCode tagJet(const xAOD::Jet* jetToTag, xAOD::BTagging * BTag);    
      void setOrigin(const xAOD::Vertex* priVtx);
      void finalizeHistos();
      
    private:      
      
      //GP: calculate the 3d significance on the fly
      double get3DSignificance(const xAOD::Vertex* priVertex,
                               std::vector<const xAOD::Vertex*>& secVertex,
                               const Amg::Vector3D jetDirection);
      // double get3DSignificance(const Trk::RecVertex & priVertex,
      //                          std::vector<const Trk::RecVertex*> & secVertex,
      //                          const Amg::Vector3D jetDirection);
      

      ToolHandle< NewLikelihoodTool > m_likelihoodTool;
      HistoHelperRoot* m_histoHelper;
      
      double m_c_mom;
      float  m_expos;
      
      std::string m_runModus; 
      std::string m_refType;
      std::string m_SVmode;
      float m_pTjetmin;
      bool m_checkOverflows;
      double m_purificationDeltaR;
      bool m_UseBinInterpol;

      /** information to persistify: */
      // std::string m_originalTPCollectionName;
      // const xAOD::TrackParticleContainer* m_originalTPCollection;
     
      const xAOD::Vertex* m_priVtx = 0;
      
      /** just print some info at the beginning */
      void printParameterSettings();

      // for debugging:
      int m_nbjet;
      int m_ncjet;
      int m_nljet;

      bool m_useCHypo;
      bool m_usePtSV2;

      std::vector<std::string> m_jetCollectionList;
      std::vector<std::string> m_hypotheses;

      bool m_doForcedCalib;
      std::string m_ForcedCalibName;
      //name of the VKalVrt secondary vertex in Jet instance
      //(needed to retrieve the ISvxConstituent written 
      //out by the finder and attached to the ParticleJet)
      std::string m_secVxFinderName;
      std::string m_xAODBaseName;

      // flag to turn ON/OFF the use of \DeltaR(jet,PV-SV) in the likelihood
      bool m_useDRJPVSV;    
      bool m_isFlipped; // true if tagger is configured in flipped mode. in that case adjusts DRJPVSV computation

    }; // End class

  inline void SVTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }

} // End namespace 

#endif
