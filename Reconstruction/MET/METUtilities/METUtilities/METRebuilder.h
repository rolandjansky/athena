///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRebuilder.h 
// Header file for class METRebuilder
// Author: T.J.Khoo<khoo@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef METUTILITIES_MET_METREBUILDER_H
#define METUTILITIES_MET_METREBUILDER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METInterface includes
#include "METInterface/IMETRebuilder.h"

// EDM includes
#include "xAODJet/JetContainer.h"

#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

// Forward declaration

namespace met {

  // typedefs
  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  class METRebuilder
    : virtual public asg::AsgTool,
      virtual public IMETRebuilder
            
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METRebuilder, IMETRebuilder)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    METRebuilder(const std::string& name);

    /// Destructor: 
    virtual ~METRebuilder(); 

    // Athena algtool's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  finalize();
    virtual StatusCode  execute();

    virtual StatusCode rebuildMET(const std::string& metKey,
				  xAOD::MissingETContainer* metCont,
				  const xAOD::IParticleContainer* collection,
				  const xAOD::MissingETComponentMap* metMap,
				  bool doTracks=true);
    virtual StatusCode rebuildMET(xAOD::MissingET* met,
				  const xAOD::IParticleContainer* collection,
				  const xAOD::MissingETComponent* component,
				  bool doTracks);

    virtual StatusCode rebuildJetMET(const std::string& jetKey,
				     const std::string& softKey,
				     xAOD::MissingETContainer* metCont,
				     const xAOD::JetContainer* jets,
				     const xAOD::MissingETComponentMap* metMap,
				     bool doTracks=true);
    virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
				     xAOD::MissingET* metSoft,
				     const xAOD::MissingET* oldSoft,
				     const xAOD::JetContainer* jets,
				     const xAOD::MissingETComponent* component,
				     bool doTracks);
    virtual StatusCode buildMETSum(const std::string& totalName,
				   xAOD::MissingETContainer* metCont);

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  private: 
    bool isPVTrack(const xAOD::TrackParticle* trk,
		   const xAOD::Vertex* pv) const;
    void associateTracks(const xAOD::IParticle* obj);

    /// Default constructor: 
    METRebuilder();

    std::string m_eleColl;
    std::string m_gammaColl;
    std::string m_tauColl;
    std::string m_jetColl;
    std::string m_muonColl;
    //
    std::string m_eleTerm;
    std::string m_gammaTerm;
    std::string m_tauTerm;
    std::string m_jetTerm;
    std::string m_muonTerm;
    std::string m_softTerm;
    //
    std::string m_inputMap;  
    std::string m_outMETCont;
    std::string m_outMETTerm;
    bool m_warnOfDupes;

    bool m_doEle;
    bool m_doGamma;
    bool m_doTau;
    bool m_doMuon;

    bool m_rebuildEle;
    bool m_rebuildGamma;
    bool m_rebuildTau;
    bool m_rebuildMuon;

    // Set up accessors to original object links in case of corrected copy containers
    SG::AuxElement::Accessor<obj_link_t>  m_objLinkAcc;
    SG::AuxElement::Decorator<char>  m_trkUsedDec;

    // For jet/soft term -- eventually break off into a separate tool
    double m_jetPtCut;
    bool m_jetDoJvf;
    double m_jetJvfCut;
    std::string m_softJetScale;
    bool m_doTracks;
    bool m_pureTrkSoft;

    // Should be in a track selector
    bool m_trk_doPVsel;
    std::string m_vtxColl;
    std::string m_clusColl;
    double m_trk_d0Max;
    double m_trk_z0Max;
  }; 

} //> end namespace met
#endif //> !METUTILITIES_MET_METREBUILDER_H
