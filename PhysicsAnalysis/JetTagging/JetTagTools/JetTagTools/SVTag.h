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
  
  class SVTag : public extends<AthAlgTool, ITagTool>
    {
    public:
      SVTag(const std::string&,const std::string&,const IInterface*);
      virtual ~SVTag();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      
      virtual StatusCode tagJet(const xAOD::Vertex& priVtx,
                                const xAOD::Jet& jetToTag,
                                xAOD::BTagging& BTag,
                                const std::string &jetName) const override;
      virtual void finalizeHistos() override;
      
    private:      
      
      //GP: calculate the 3d significance on the fly
      double get3DSignificance(const xAOD::Vertex& priVertex,
                               std::vector<const xAOD::Vertex*>& secVertex,
                               const Amg::Vector3D jetDirection) const;
      // double get3DSignificance(const Trk::RecVertex & priVertex,
      //                          std::vector<const Trk::RecVertex*> & secVertex,
      //                          const Amg::Vector3D jetDirection);
      double get3DSignificanceCorr(const xAOD::Vertex& priVertex,
                               std::vector<const xAOD::Vertex*>& secVertex,
                               const Amg::Vector3D jetDirection) const;
      

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
     
      /** just print some info at the beginning */
      void printParameterSettings();

      // for debugging:
      mutable std::atomic<int> m_nbjet;
      mutable std::atomic<int> m_ncjet;
      mutable std::atomic<int> m_nljet;

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
      bool m_save_probabilities;

    }; // End class

} // End namespace 

#endif
