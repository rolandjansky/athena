///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMaker.h 
// Header file for class METMaker
// Author: T.J.Khoo<khoo@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef METUTILITIES_MET_METMAKER_H
#define METUTILITIES_MET_METMAKER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METInterface includes
#include "METInterface/IMETMaker.h"

// EDM includes
#include "xAODJet/JetContainer.h"

// Forward declaration

namespace met {

  // typedefs
  typedef ElementLink<xAOD::IParticleContainer> obj_link_t;

  class METMaker
    : virtual public asg::AsgTool,
      virtual public IMETMaker
            
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METMaker, IMETMaker)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    METMaker(const std::string& name);

    /// Destructor: 
    virtual ~METMaker(); 

    // Athena algtool's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  finalize();
    //    virtual StatusCode  execute();

    virtual StatusCode rebuildMET(xAOD::MissingET* met,
				  const xAOD::IParticleContainer* collection,
				  const xAOD::MissingETAssociationMap* map,
				  std::vector<const xAOD::IParticle*>& uniques);
    virtual StatusCode rebuildMET(xAOD::MissingET* met,
				  const xAOD::IParticleContainer* collection,
				  const xAOD::MissingETAssociationMap* map,
				  std::vector<const xAOD::IParticle*>& uniques,
				  MissingETBase::UsageHandler::Policy p,
				  bool removeOverlap);

    inline virtual StatusCode rebuildMET(xAOD::MissingET* met,
					 const xAOD::IParticleContainer* collection,
					 const xAOD::MissingETAssociationMap* map)
    {
      std::vector<const xAOD::IParticle*> uniques;
      return rebuildMET(met, collection, map, uniques);
    }

    virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
				     const xAOD::JetContainer* jets,
				     const xAOD::MissingETAssociationMap* map,
				     std::vector<const xAOD::IParticle*>& uniques,
				     xAOD::MissingET* metSoftClus,
				     const xAOD::MissingET* coreSoftClus,
				     xAOD::MissingET* metSoftTrk,
				     const xAOD::MissingET* coreSoftTrk);

    inline virtual StatusCode rebuildJetMET(xAOD::MissingET* metJet,
					    const xAOD::JetContainer* jets,
					    const xAOD::MissingETAssociationMap* map,
					    xAOD::MissingET* metSoftClus,
					    const xAOD::MissingET* coreSoftClus,
					    xAOD::MissingET* metSoftTrk,
					    const xAOD::MissingET* coreSoftTrk)
    {
      std::vector<const xAOD::IParticle*> uniques;
      return rebuildJetMET(metJet, jets, map, uniques,
			   metSoftClus, coreSoftClus,
			   metSoftTrk, coreSoftTrk);
    }


    virtual StatusCode buildMETSum(const std::string& totalName,
				   xAOD::MissingETContainer* metCont,
				   MissingETBase::Types::bitmask_t softTermsSource=0);

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

    bool m_doJetJvf;
    bool m_jetCorrectPhi;

    double m_jetMinPt;
    double m_jetMinAbsJvf;
    double m_jetMinEfrac;
    double m_jetMinWeightedPt;

    bool m_doPFlow;

    // Set up accessors to original object links in case of corrected copy containers
    SG::AuxElement::ConstAccessor<obj_link_t>  m_objLinkAcc;

    /// Default constructor: 
    METMaker();

  }; 

} //> end namespace met
#endif //> !METUTILITIES_MET_METMAKER_H
