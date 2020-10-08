///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METBuilderTool.h 
// Header file for class METBuilderTool
//
// This is the base class for tools that construct MET terms
// from other object collections.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METBUILDERTOOL_H
#define METRECONSTRUCTION_METBUILDERTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

// METRecoInterface includes
#include "METRecoInterface/IMETToolBase.h"

// Forward declaration
namespace xAOD {
  class IParticle;
}

namespace met {
  class METBuilderTool
    : virtual public asg::AsgTool,
      virtual public IMETToolBase
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METBuilderTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METBuilderTool(const std::string& name);
    virtual ~METBuilderTool();

    // AsgTool Hooks
    virtual StatusCode initialize();
    virtual StatusCode execute(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;
    virtual StatusCode finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
  protected:

    std::string m_input_data_key;
    std::string m_output_met_key;
    std::string m_mod_clus_key;
    bool m_useRapidity; // by default, use pseudorapidity for matching
    bool m_useModClus;  // use modified e.g. origin-corrected clusters

    // reconstruction process to be defined in the individual tools
    // pure virtual -- we have no default
    virtual StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const = 0;

    // decision about whether an input object should be considered for inclusion in the MET sum
    virtual bool accept(const xAOD::IParticle* object) const = 0;

    // returns true if no overlapping clusters found, false otherwise
    // acceptedSignals will be filled with the constituents that are not already in the composition map
    // an object weight can also optionally be set
    //
    // make this a pure virtual for now -- needs specialisation for all classes
    // could create a default implementation if we are provided with a common accessor for constituents
    virtual bool resolveOverlap(const xAOD::IParticle* object,
				xAOD::MissingETComponentMap* metMap,
				std::vector<const xAOD::IParticle*>& acceptedSignals,
				MissingETBase::Types::weight_t& objWeight) const = 0;

    // after overlap resolution, add the object with its list of constituents to the composition map
    // weight and status can be set if they need to be different from the default
    virtual bool addToMET(const xAOD::IParticle* object,
			  const std::vector<const xAOD::IParticle*>& acceptedSignals,
			  xAOD::MissingET* metTerm,
			  xAOD::MissingETComponentMap* metMap,
			  MissingETBase::Types::weight_t& objWeight,
			  MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::TrackCluster) const;
    
    // uses metMap->insert(metTerm, particle, weight, status, acceptedSignals)
    //
    // Usage example:
    //
    // METJetTool::executeTool(xAOD::MissingET* pmetObj,xAOD::MissingETComponentMap* pMap)
    // {
    //    // get input from StoreGate
    //    const xAOD::xAOD::IParticleContainer* pCont =  0;
    //    if ( /* get input container */ )...
    //
    //    // loop on input container
    //    xAOD::xAOD::IParticleContainer::const_iterator fCont(pCont->begin());
    //    xAOD::xAOD::IParticleContainer::const_iterator lCont(pCont->end());
    //    std::vector<const xAOD::IParticle*> signalList;
    //    for ( ; fCont != lCont; ++fCont )
    //    { if ( this->accept(*fCont) ) {
    //        signalList.clear();
    //        weight_t   weight;
    //        if ( this->resolveOverlap(*fCont,pMap,signalList,weight) ) {
    //          if ( weight.wet() > m_minimumWeight ) this->addMET(*fCont,signalList,pmetObj,weight,statusWord); 
    //          }
    //       }
    //    }   
    // }
    //
    // bool METElectronTool::resolveOverlap(....)
    // {
    //    // measure overlap
    //    return ( this->getOverlapClusters(particle,clusterList) ) ... // this function needs to return the list of TopoClusters associated with the electron
    // }
    //
    // 

  private:
    /// Default constructor:
    METBuilderTool();

  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> !METRECONSTRUCTION_METBUILDERTOOL_H
