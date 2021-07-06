/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETQGTAGGER_H
#define JETQGTAGGER_H

#include "JetAnalysisInterfaces/IJetQGTagger.h"
#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "PATInterfaces/SystematicsTool.h"

#include <TH2D.h>

namespace InDet { class IInDetTrackSelectionTool; }
namespace InDet { class IInDetTrackTruthFilterTool; }
namespace InDet { class IJetTrackFilterTool; }
namespace InDet { class IInDetTrackTruthOriginTool; }

namespace CP {

  enum QGSystApplied {
    QG_NONE,
    QG_TRACKEFFICIENCY,
    QG_TRACKFAKES,
    QG_NCHARGEDTOPO,
    QG_NCHARGEDEXP_UP,
    QG_NCHARGEDME_UP,
    QG_NCHARGEDPDF_UP,
    QG_NCHARGEDEXP_DOWN,
    QG_NCHARGEDME_DOWN,
    QG_NCHARGEDPDF_DOWN
  };


  class JetQGTagger: public IJetQGTagger, public JSSTaggerBase, public SystematicsTool{
    ASG_TOOL_CLASS( JetQGTagger, IJetQGTagger )

    public:

      JetQGTagger( const std::string& name); 
      virtual ~JetQGTagger(); // destructor

      virtual StatusCode initialize() override;

      // Implement IBoostedJetTagger interface
      virtual StatusCode tag(const xAOD::Jet& jet, const xAOD::Vertex *pv) const override;
      virtual StatusCode tag(const xAOD::Jet& jet) const override { return tag(jet, nullptr); }

      // functions for systematic variations
      virtual bool isAffectedBySystematic(const SystematicVariation& var) const override {return SystematicsTool::isAffectedBySystematic(var);}
      virtual SystematicSet affectingSystematics() const override {return SystematicsTool::affectingSystematics();}
      virtual SystematicSet recommendedSystematics() const override {return SystematicsTool::recommendedSystematics();}
      virtual StatusCode applySystematicVariation(const SystematicSet& set) override {return SystematicsTool::applySystematicVariation(set);}
      virtual StatusCode sysApplySystematicVariation(const SystematicSet&) override;

    private:
      JetQGTagger();
      StatusCode getNTrack(const xAOD::Jet * jet, /*const xAOD::Vertex * pv,*/ int &ntracks) const ;
      StatusCode getNTrackWeight(const xAOD::Jet * jet, double &weight) const ;
      StatusCode simplegetNTrackWeight(const xAOD::Jet * jet, double &weight) const ;

      QGSystApplied m_appliedSystEnum;

      TH2D* m_hquark;
      TH2D* m_hgluon;

      TH2D* m_topo_hquark;

      TH2D* m_exp_hquark_up;
      TH2D* m_exp_hquark_down;
      TH2D* m_exp_hgluon_up;
      TH2D* m_exp_hgluon_down;

      TH2D* m_me_hquark_up;
      TH2D* m_me_hquark_down;
      TH2D* m_me_hgluon_up;
      TH2D* m_me_hgluon_down;

      TH2D* m_pdf_hquark_up;
      TH2D* m_pdf_hquark_down;
      TH2D* m_pdf_hgluon_up;
      TH2D* m_pdf_hgluon_down;

      TH2D* m_trackeff_hquark;
      TH2D* m_trackeff_hgluon;
      TH2D* m_fake_hquark;
      TH2D* m_fake_hgluon;

      StatusCode loadHist(TH2D *&hist,std::string filename,std::string histname);

      std::string m_taggername;
      std::string m_topofile;
      std::string m_expfile;
      std::string m_mefile;
      std::string m_pdffile;
      std::string m_trackefffile;
      std::string m_fakefile;
      std::string m_weight_decoration_name;
      std::string m_tagger_decoration_name;

      int m_NTrackCut;
      double m_slope;
      double m_intercept;
      std::string m_cuttype;
      int m_mode;

      asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;
      asg::AnaToolHandle<InDet::IInDetTrackTruthFilterTool> m_trkTruthFilterTool;
      asg::AnaToolHandle<InDet::IInDetTrackTruthFilterTool> m_trkFakeTool;
      asg::AnaToolHandle<InDet::IJetTrackFilterTool> m_jetTrackFilterTool;
      asg::AnaToolHandle<InDet::IInDetTrackTruthOriginTool> m_originTool;

      /// ReadDecorHandle keys
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_readNumTrkPt500PVKey{this, "NumTrkPt500PVName", "NumTrkPt500PV", "SG key for NumTrkPt500PV"};
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_readNtrkKey{this, "NtrkName", "DFCommonJets_QGTagger_NTracks", "SG key for Ntrk"};

      /// WriteDecorHandle keys
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decTagKey{this, "TagName", "Tag", "SG key for Tag"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_decWeightKey{this, "WeightName", "Weight", "SG key for Weight"};
  
  };

} /* namespace CP */

#endif /* JETQGTAGGER_H */
