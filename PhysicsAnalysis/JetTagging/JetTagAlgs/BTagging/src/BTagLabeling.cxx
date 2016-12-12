/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagLabeling.h"
#include "BTagging/IBTagLabeling.h"

//#include "JetEvent/Jet.h"
#include "ParticleJetTools/IJetTruthMatching.h"
#include "ParticleJetTools/JetQuarkLabel.h"
//#include "ParticleJetTools/JetQGPartonLabel.h"

#include "JetTagInfo/TruthInfo.h"
//#include "JetEvent/JetTagInfoBase.h"

//#include "ParticleJetTools/FindLeptonTruth.h"
//#include "JetTagInfo/SoftLeptonTruthInfo.h"
//#include "JetTagInfo/QGPartonTruthInfo.h"
//#include "JetTagInfo/SLTrueInfo.h"

//#include "ParticleJetTools/FindLeptonTruth.h"
namespace Analysis {

  BTagLabeling::BTagLabeling(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_jetTruthMatchTool("Analysis::JetQuarkLabel")//,
    //m_LeptonTruthTool("Analysis::LeptonTruthTool")
  {
      declareInterface<IBTagLabeling>(this);
      declareProperty("JetTruthMatchTool",         m_jetTruthMatchTool);
      //declareProperty("JetQGPartonTruthMatchTool", m_jetQGPartonTruthMatchTool);
      //declareProperty("LeptonTruthTool",           m_LeptonTruthTool);
  }

  BTagLabeling::~BTagLabeling() {
  }

  StatusCode BTagLabeling::initialize()
  {
    //std::cout << "AARON initializing BTagLabeling" << std::endl;
     /// retrieve the Jet and lepton truth match tools
     if ( !m_jetTruthMatchTool.empty() ) {
       if ( m_jetTruthMatchTool.retrieve().isFailure() ) {
         ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_jetTruthMatchTool);
         return StatusCode::FAILURE;
       } else {
         ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_jetTruthMatchTool);
       }
     }
     // if ( !m_jetQGPartonTruthMatchTool.empty() ) {
     //   if ( m_jetQGPartonTruthMatchTool.retrieve().isFailure() ) {
     // 	 ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_jetQGPartonTruthMatchTool);
     // 	 return StatusCode::FAILURE;
     //   } else {
     // 	 ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_jetQGPartonTruthMatchTool);
     //   }
     // }

     // if ( !m_LeptonTruthTool.empty() ) {
     //   if ( m_LeptonTruthTool.retrieve().isFailure() ) {
     //     ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_LeptonTruthTool);
     //     return StatusCode::FAILURE;
     //   } else {
     //     ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_LeptonTruthTool);
     //   }
     // }

     return StatusCode::SUCCESS;
  }

  StatusCode BTagLabeling::finalize()
  {
      return StatusCode::SUCCESS;
  }
 
  StatusCode BTagLabeling::BTagLabeling_exec(xAOD::Jet& jetToTag) {
    //JetTruthMatching and setting of the TruthInfo object 
    bool jetIsMatched( true );
    //const Analysis::TruthInfo* mcinfo = NULL;//jetToTag.tagInfo<Analysis::TruthInfo>("TruthInfo");// AA - need jet input
    
    int thisJetTruthLabel = 0;
    // bool doTruthInfo = !jetToTag.getAttribute("TruthLabelID",thisJetTruthLabel);
    // TEMPORARY FIX! This will happily rerun the tagging even if it was done already. A proper fix should be made
    bool doTruthInfo = true;
    // const Analysis::SoftLeptonTruthInfo* mcslinfo = jetToTag.tagInfo<Analysis::SoftLeptonTruthInfo>("SoftLeptonTruthInfo");
    // bool doSLTruthInfo = true;
    // if(mcslinfo) doSLTruthInfo = false;

    if (!m_jetTruthMatchTool.empty() && doTruthInfo) {
      //TruthInfo* truthInfo = new TruthInfo("TruthInfo");
      Analysis::IJetTruthMatching::MatchInfo info;
      jetIsMatched = m_jetTruthMatchTool->matchJet( jetToTag, &info );
      jetToTag.setAttribute("TruthLabelDeltaR_B",info.deltaRMinTo("B"));
      jetToTag.setAttribute("TruthLabelDeltaR_C",info.deltaRMinTo("C"));
      jetToTag.setAttribute("TruthLabelDeltaR_T",info.deltaRMinTo("T"));
      //
      if ( jetIsMatched ) {
        thisJetTruthLabel = info.jetLabel;
        //if(thisJetTruthLabel=="B") {
          //jetToTag.set_pdgId(PDG::b);// AA - need jet input
	  //if (jql) {
	  //  truthInfo->BDecayVertex(jql->BDecVtx());
	  //  truthInfo->BHadronPdg(jql->Bpdg());
	// }
        //} else if(thisJetTruthLabel=="C") {
          //jetToTag.set_pdgId(PDG::c);// AA - need jet input
        //} else if(thisJetTruthLabel=="T") {
          //jetToTag.set_pdgId(PDG::tau_minus);// AA - need jet input
        //}
      }
      else {
        thisJetTruthLabel = -1;
      }
      jetToTag.setAttribute("ConeTruthLabelID",thisJetTruthLabel);
      ATH_MSG_VERBOSE("#BTAG# Matching tool result: " << jetIsMatched << " " << thisJetTruthLabel);
      //std::cout << "AARON result: " << jetIsMatched << " " << thisJetTruthLabel << std::endl;
      //jetToTag.addInfo(truthInfo); AA - need jet input

      //
      // Quick and dirty : if the McEventCollection contains more than one event, 
      //// add a TruthInfo which labels via in time pileup

	/// need a fix: if TruthInfo is there but not TruthInfoPU
	/// and we want to compute TruthInfoPU. 
       /// This will not happen with the current setup. 
/*      if (jql) {
	//std::cout << "AARON in jql" << std::endl;
	int nMC = jql->NEventInCollection();
	if (nMC > 1 && jql->EventSelection() == -1) {
	  TruthInfo* truthInfoPU = new TruthInfo("TruthInfoPU");
	  jql->EventSelection(1);
	  bool jetIsMatchedPU = m_jetTruthMatchTool->matchJet( jetToTag );
	  truthInfoPU->deltaRMinTo("B", jql->deltaRMinTo("B"));
	  truthInfoPU->deltaRMinTo("C", jql->deltaRMinTo("C"));
	  truthInfoPU->deltaRMinTo("T", jql->deltaRMinTo("T"));
	  truthInfoPU->setJetTruthLabel(m_jetTruthMatchTool->jetLabel());
	  if (jetIsMatchedPU && m_jetTruthMatchTool->jetLabel() == "B") {
	    truthInfoPU->BDecayVertex(jql->BDecVtx());
	    truthInfoPU->BHadronPdg(jql->Bpdg());
	  }
	  //jetToTag.addInfo(truthInfoPU); AA - need jet input
	  //std::cout << "AARON calling jql->EventSelection(-1)" << std::endl;
	  jql->EventSelection(-1);
	}
	}*/
    }/*
    //JetQGPartonTruthMatching and setting of the TruthInfo object
    bool jetIsQGPartonMatched( true );
    std::string thisJetQGPartonTruthLabel( "N/A" ); /// by default no truth info available
    const Analysis::QGPartonTruthInfo* QGPartonmcinfo = jetToTag.tagInfo<Analysis::QGPartonTruthInfo>("QGPartonTruthInfo");
    bool doQGPartonTruthInfo = true;
    if(QGPartonmcinfo) doQGPartonTruthInfo = false;
  
    if (!m_jetQGPartonTruthMatchTool.empty() && doQGPartonTruthInfo) {
      QGPartonTruthInfo* QGTruthInfo = new QGPartonTruthInfo("QGPartonTruthInfo");
      ATH_MSG_VERBOSE("#QGTAG# Matching tool available, calling it...");
      jetIsQGPartonMatched = m_jetQGPartonTruthMatchTool->matchJet( jetToTag );
      JetQGPartonLabel* jqgpl = dynamic_cast<JetQGPartonLabel*>(&(*m_jetQGPartonTruthMatchTool)); // & gets the pointer out of the ToolHandle to cast it!
      //
      if ( jetIsQGPartonMatched ) {
        thisJetQGPartonTruthLabel = m_jetQGPartonTruthMatchTool->jetLabel();
        QGTruthInfo->setJetTruthLabel(thisJetQGPartonTruthLabel);
        if(jqgpl){
  	  QGTruthInfo->deltaRToParton( jqgpl->deltaRToParton()  );
  	}	
      }
      ATH_MSG_VERBOSE("#QGTAG# Matching tool result: " << jetIsQGPartonMatched << " " << thisJetQGPartonTruthLabel);
      jetToTag.addInfo(QGTruthInfo);
      //
      // Quick and dirty : if the McEventCollection contains more than one event, 
      //// add a TruthInfo which labels via in time pileup
 
      /// need a fix: if TruthInfo is there but not TruthInfoPU
      /// and we want to compute TruthInfoPU. 
      /// This will not happen with the current setup. 
      if (jqgpl) {
        int nMC = jqgpl->NEventInCollection();
  	if (nMC > 1 && jqgpl->EventSelection() == -1) {
    	  QGPartonTruthInfo* QGTruthInfoPU = new Analysis::QGPartonTruthInfo("QGPartonTruthInfoPU");
 	  jqgpl->EventSelection(1);
 	  bool jetIsQGPartonMatchedPU = m_jetQGPartonTruthMatchTool->matchJet( jetToTag );
 	  if(jetIsQGPartonMatchedPU){
 	    QGTruthInfoPU->deltaRToParton(jqgpl->deltaRToParton());
 	    QGTruthInfoPU->setJetTruthLabel(m_jetQGPartonTruthMatchTool->jetLabel());
 	    jetToTag.addInfo(QGTruthInfoPU);
 	    jqgpl->EventSelection(-1);
 	  }
 	}
      }
    }

      // Specific truth information for leptons:
      if (!m_LeptonTruthTool.empty() && doSLTruthInfo) {
        ATH_MSG_VERBOSE("#BTAG# Lepton truth tool available, calling it...");
        std::vector<SLTrueInfo> sltinfovec = m_LeptonTruthTool->FindTruth( jetToTag );
        uint numberOfTrueLeptons = sltinfovec.size();
        if (numberOfTrueLeptons > 0) {
	  SoftLeptonTruthInfo* sltinfo = new SoftLeptonTruthInfo("SoftLeptonTruthInfo");
	  for (uint i = 0; i < numberOfTrueLeptons; i++) {
	    ATH_MSG_VERBOSE("#BTAG# Adding lepton " << i << " " << sltinfovec[i]);
	    sltinfo->addSLTrueInfo(sltinfovec[i]);
	  }
	  jetToTag.addInfo(sltinfo);
        }
        ATH_MSG_VERBOSE("#BTAG# Lepton truth tool result: " << numberOfTrueLeptons);
      }
      return StatusCode::SUCCESS;*/

    return StatusCode::SUCCESS;
  }

} // namespace
