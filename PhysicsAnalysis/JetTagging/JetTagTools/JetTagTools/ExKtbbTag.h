/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_EXKTBBTAG_H
#define JETTAGTOOLS_EXKTBBTAG_H

/******************************************************
    @class ExKtbbTag
    Identification of double b-hadron jets
    @ author Qi Zeng
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"

// std
#include <string>

// xAOD EDM
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"

// ROOT
#include "TLorentzVector.h"

// TMVA
#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"
namespace TMVA { class Reader; }

// CalibrationBroker
namespace Analysis { class CalibrationBroker; }

typedef Eigen::Matrix<double, 3, 3, 0, 3, 3> AmgSymMatrix3D;

namespace Analysis { 

  class ExKtbbTag : public AthAlgTool , virtual public ITagTool {
   
  public:
    ExKtbbTag(const std::string&,const std::string&,const IInterface*);
      
    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~ExKtbbTag();
    StatusCode initialize();
    StatusCode finalize();
      
    /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
  	be part of the JetTag IParticle interface implementation. The trouble with 
  	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const xAOD::Vertex* priVtx);
      
    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const std::string jetName);

    void finalizeHistos() {};

  private:      

    // global variables //

    std::string m_runModus;            // "reference" means to produce variables for performance study; "analysis" means output weight with given reference histogram
    std::string m_tagMode;             // "Hbb", or "gbb"
    std::string m_BDTMode;             // "MV2Only", "MV2andJFDRSig", or "MV2andTopos"
    std::string m_taggerNameBase;      // prefix in BTaggingContainer -- mostly used in code
    std::string m_xAODBaseName;        // something required by base class
    std::string m_ForceCalibChannel;   // force to use this jet channel in calibration broker. Empty means default jet author. "Default" means "AntiKt10LCTopoTrimmedPtFrac5SmallR20" in "Hbb" tagMode and "AntiKt4LCTopo" in "gbb" tagMode
    bool        m_debug;

    // variables for tagger //

    std::string m_SubJetLabel;         // label to access subjet information
    bool        m_JFOnlyVtx;           // whether we insist JF should have at least one vtx with at least 2 tracks
      
    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx = 0;

    ///////////////////////////////////////////////
    // Functions related to variable calculation //
    ///////////////////////////////////////////////

    // high-level functionality //

    bool CalculateInputVariables(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);

    bool JetTagAnalysisMode(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);
    bool JetTagReferenceMode(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);

    // medium-level functionality//

    double GetDoubleMV2c20(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2);
    double GetSingleMV2c20(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2);

    int GetSV1NtrksDiff(const xAOD::BTagging* bjet, const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool doValidDualSV1);
    int GetJFNtrksDiff(const xAOD::BTagging* bjet, const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool doValidDualJF, bool onlyVtx);

    double GetSV1CombMass(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool doValidDualSV1);
    double GetJFCombMass(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool doValidDualJF, bool onlyVtx, bool doNeutralCorrection);

    double GetSV1DRSignificance(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, double& DR, double& DR_error);
    double GetJFDRSignificance(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool onlyVtx, double& DR, double& DR_error);

    double GetSV1DR3DSignificance(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, double& dR3D, double& dR3D_error);

    double GetSV1DLSignificance(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, double& dL, double& dL_error);
    double GetJFDLSignificance(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool onlyVtx, double& dL, double& dL_error);

    // low-level functionality //

    static bool SortPt(const xAOD::Jet* j1, const xAOD::Jet* j2) {return j1->pt() > j2->pt();}

    std::vector<const xAOD::Vertex*> GetSV1vertices(const xAOD::BTagging* bjet);
    bool IsValidSV1(std::vector<const xAOD::Vertex*> input);
    bool SV1DualValid(std::vector<const xAOD::Vertex*> vtx1, std::vector<const xAOD::Vertex*> vtx2);

    std::vector<const xAOD::BTagVertex*> GetJFVertices(const xAOD::BTagging* bjet);
    std::vector<float> GetJFfittedPosition(const xAOD::BTagging* bjet, std::vector<float>& JFfittedCov);
    bool IsValidJF(const xAOD::BTagging* bjet, bool onlyVtx);
    bool JFDualValid(const xAOD::BTagging* bjet1, const xAOD::BTagging* bjet2, bool onlyVtx);
    int JFntrk(const xAOD::BTagging* bjet, bool onlyVtx);
    TLorentzVector JFMomentum(const xAOD::BTagging* bjet, bool onlyVtx, bool doNeutralCorrection);
    double JFEffectiveDecayLength(const xAOD::BTagging* bjet, bool onlyVtx, double& error);

    double LimitUpperBound(double input, double start, double limit){
      if(input > start) return start + (limit-start)/TMath::Pi()*2.*TMath::ATan(TMath::Pi()/2./(limit-start)*(input-start));
      else              return input;
    }

    std::vector<Amg::Vector3D> GetVtxEigenVariation(Amg::Vector3D vtxPosition, AmgSymMatrix3D vtxCovMatrix);
    double GeneralDRSignificance3D(Amg::Vector3D vtx1, AmgSymMatrix3D vtx1_CovMatrix, Amg::Vector3D vtx2, AmgSymMatrix3D vtx2_CovMatrix, double& dR3D, double& dR3D_error);

    ////////////////////////////////////
    // Functions related to final BDT //
    ////////////////////////////////////

    std::string m_taggerName;          // actual taggerName

    ToolHandle<CalibrationBroker> m_calibrationTool;

    std::map<std::string, TMVA::Reader*>     m_tmvaReaders;       // key should be alias + m_taggerCalibName
    std::map<std::string, TMVA::MethodBase*> m_tmvaMethod;        // key should be alias + m_taggerCalibName
    std::vector<std::string>                 m_undefinedReaders;

    std::string GetPtBin(const xAOD::Jet* jetToTag);
    
    std::map<std::string, float*>    m_tmvaVariables;             // key should be the same as those used in BDT 
    bool                             InitTMVAVariables(std::string BDTMode);
    bool                             FillTMVAVariables(std::string BDTMode, const xAOD::Jet* jetToTag, const xAOD::BTagging* BTag);
    bool                             BundleTMVAVariables(std::string BDTMode, TMVA::Reader* tmvaReader);

    std::vector<double>  m_ptbinlist;


  }; // End class

  inline void ExKtbbTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }

} // End namespace 

#endif
