///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaTool.h 
// Header file for class METEgammaTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METEGAMMATOOL_H
#define METRECONSTRUCTION_METEGAMMATOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

// Egamma EDM
#include "xAODEgamma/EgammaFwd.h"

// Calo EDM
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace met{

  class METEgammaTool
    : virtual public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METEgammaTool, IMETToolBase)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METEgammaTool(const std::string& name);
    virtual ~METEgammaTool();

    // AsgTool Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  finalize();

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
    virtual StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const = 0;
    virtual bool accept(const xAOD::IParticle* object) const;
    virtual bool resolveOverlap(const xAOD::IParticle* object,
				xAOD::MissingETComponentMap* metMap,
				std::vector<const xAOD::IParticle*>& acceptedSignals,
				MissingETBase::Types::weight_t& objWeight) const = 0;

    // Identify topoclusters that share energy with the egamma SW cluster
    //
    virtual void matchTopoClusters(const xAOD::Egamma* eg, std::vector<const xAOD::IParticle*>& tclist,
				   const xAOD::CaloClusterContainer* tccont) const;
    virtual void matchExtraTracks(const xAOD::Egamma* eg, std::vector<const xAOD::IParticle*>& tclist) const;

    // PID quality criteria and cuts
    std::string m_eg_pid;
    uint16_t m_eg_author;
    double m_eg_minPt;
    double m_eg_maxEta;
    //
    bool m_eg_rejectCrack;
    double m_eg_crkEtaLo;
    double m_eg_crkEtaHi;
    // don't currently have the interface for isEM (or equivalent)
    // allow for the possibility to select on the cluster quality
    bool m_eg_testClusOQ;
    unsigned int m_eg_clusOQ;

    // SW-topocluster overlap removal
    std::string m_tcCont_key;
    const xAOD::CaloClusterContainer* m_tcCont;
    double m_tcMatch_dR;
    double m_tcMatch_maxRat;
    double m_tcMatch_tolerance;
    unsigned short m_tcMatch_method;

    // track overlap removal
    bool m_eg_doTracks;

  private:
    /// Default constructor: 
    METEgammaTool();
  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> !METRECONSTRUCTION_METEGAMMATOOL_H
