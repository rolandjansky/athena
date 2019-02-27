/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_SOFTMUONTAG_H
#define JETTAGTOOLS_SOFTMUONTAG_H
/********************************************************************
    @class SoftMuonTag
    PACKAGE : offline/PhysicsAnalysis/JetTagging/JetTagTools

   PURPOSE:  b-tagging based on soft muon identification
            In reference mode the significance is filled into a histogram.
   @author Henri.Bachacou@cern.ch
********************************************************************/
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <map>
#include <string>
#include <list>
#include "JetTagTools/ITagTool.h"
#include "xAODTracking/TrackParticle.h"
#include "MVAUtils/BDT.h"
#include "StoreGate/ReadHandleKey.h"
#include "JetTagCalibration/JetTagCalibCondData.h"

#include "JetTagTools/IMultivariateJetTagger.h"

#include "InDetSVWithMuonTool/InDetSVWithMuonTool.h"

namespace Trk  { class VxCandidate; }
namespace Reco { class ITrackToVertex; }
namespace Trk  { class ITrackToVertexIPEstimator; }
namespace Analysis { class MuonContainer; }
class AnalysisTools;

namespace CP { class IMuonSelectionTool; }

namespace Analysis
{
  class SoftMuonInfo;
  class NewLikelihoodTool;
  class HistoHelperRoot;

  class SoftMuonTag : public AthAlgTool, virtual public ITagTool
  {
  public:
    SoftMuonTag(const std::string&,const std::string&,const IInterface*);

    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~SoftMuonTag();
    StatusCode initialize();
    StatusCode finalize();
      
    /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
	be part of the JetTag IParticle interface implementation. The trouble with
	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const xAOD::Vertex* priVtx);
      
    StatusCode tagJet(const xAOD::Jet* jetToTag, xAOD::BTagging * BTag);  
      
    void finalizeHistos();
      
  private:
    /** ANDREA **/
    std::string m_softmuon_infosource;
    std::string m_taggerName;
    std::string m_taggerNameBase; // unique name for regular and flip versions
    std::string m_treeName;
    std::string m_varStrName;

    /** Key of calibration data: */
    SG::ReadCondHandleKey<JetTagCalibCondData> m_readKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of input (derived) JetTag calibration data"};
    bool m_forceMV2CalibrationAlias;
    bool m_useEgammaMethodMV2;
    //bool m_decorateBTaggingObj;
    std::string m_decTagName;
    std::string m_MV2CalibAlias;
    std::string m_MV2cXX;
    std::string m_xAODBaseName;

    std::map<std::string, MVAUtils::BDT*> m_egammaBDTs;
    std::list<std::string> m_undefinedReaders; // keep track of undefined readers to prevent too many warnings.

    float deltaR(float eta1, float eta2, float phi1, float phi2); 

    /** ANDREA **/
      
    const unsigned m_nClasses=3;//b,u,c probabilities. It might be better to read from calib file for future
    const bool m_writeRootFile=false;//Developper option
    bool m_disableAlgo;
    int  m_warnCounter;

    /** GP: Tool for the estimation of the IPs to the Vertex */
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
    
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectorTool;
    int m_muonQualityCut;

    /** Likelihood tool */
    ToolHandle< NewLikelihoodTool > m_likelihoodTool;

    /** Muon-seeded SV finder */      
    ToolHandle< InDet::ISVWithMuonFinder > m_SVmuonFinder;

    // Helpers...
    HistoHelperRoot* m_histoHelper;
      
    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/
    std::string    m_runModus;          // "analysis: read reference histos; reference: make reference histos
    std::string    m_refType;           // type jets (B, C, or L)
    std::string    m_algMode;           // type of algorithm (cuts, likelihood pt vs ptrel, combined, etc...)

    float m_pTjetmin;
    float m_etajetmin;

    bool m_checkOverflows;
    double m_purificationDeltaR;
    double m_muonIsolDeltaR;
    bool m_UseBinInterpol;

    /** Muon selection cuts */
    int m_alg;
    double m_d0cut;
    double m_pTcut;
    double m_DRcut;
    double m_MatchChi2cut;

    //SMT vars
    float  m_pt    			;
    float  m_absEta			;
    float  m_sm_mu_pt          	;
    float  m_sm_dR             	;
    float  m_sm_qOverPratio    	;
    float  m_sm_mombalsignif   	;
    float  m_sm_scatneighsignif	;
    float  m_sm_pTrel          	;
    float  m_sm_mu_d0          	;
    float  m_sm_mu_z0          	;
    float  m_sm_ID_qOverP      	;
    float  m_sm_mu_sv_mass     	;
    float  m_sm_mu_sv_efrc     	;
    float  m_sm_mu_sv_ntrk     	;
    float  m_sm_mu_sv_VtxnormDist	;
    float  m_sm_mu_sv_ntrkVtx		;
    float  m_sm_mu_sv_dmaxPt 		;
    float  m_sm_mu_sv_Lxy 		;  
    float  m_sm_mu_sv_L3d 		;  
    float  m_sm_mu_sv_dR  		;  

    float m_my_smt;

    SG::ReadHandleKey<xAOD::TrackParticleContainer > m_TrackParticles {this, "TrackParticlesName", "InDetTrackParticles", "Input track particle container to find SV with a muon"};

    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx = 0;

    /** just print some info at the beginning */
    void printParameterSettings();

    std::vector<std::string> m_jetCollectionList;
    std::vector<std::string> m_hypothese; // "b" or "c" or "l"
    std::vector<std::string> m_histoname; // list of histogram names

    bool m_doForcedCalib;
    std::string m_ForcedCalibName;

    bool m_writeInfoPlus;
    std::string m_originalMuCollectionName;
    std::string m_muonAssociationName;
      
    void ReplaceNaN_andAssign(std::map<std::string, double> var_map);
    void SetVariableRefs(const std::vector<std::string> inputVars,
			 unsigned &nConfgVar, bool &badVariableFound, std::vector<float*> &inputPointers);
    void ClearInputs();
    void PrintInputs();

    std::vector<float> GetMulticlassResponse(MVAUtils::BDT* bdt) const {
      std::vector<float> v(m_nClasses,-1);
      return (bdt->GetPointers().size() ? bdt->GetMultiResponse(bdt->GetPointers(),m_nClasses) : v);
    }
    double GetClassResponse (MVAUtils::BDT* bdt) const { return (bdt->GetPointers().size() ? bdt->GetGradBoostMVA(bdt->GetPointers()) : -9.); }
   
  }; // End class
 
  inline void SoftMuonTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }
} // End namespace

#endif
