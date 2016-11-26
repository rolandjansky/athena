///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonTool.h 
// Header file for class METMuonTool
//
// This is a scheduler for the MET Reconstruction, and sets up
// the sequence in which the individual terms are constructed.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METMUONTOOL_H
#define METRECONSTRUCTION_METMUONTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METBuilderTool.h"

// Forward declaration

namespace met{

  class METMuonTool
    : public METBuilderTool
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METMuonTool, IMETToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name (does this have to be a non-const
    // std::string and not a const reference?)
    METMuonTool(const std::string& name);
    ~METMuonTool();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();

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
    StatusCode executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const;
    bool accept(const xAOD::IParticle* object) const;
    bool resolveOverlap(const xAOD::IParticle* object,
			xAOD::MissingETComponentMap* metMap,
			std::vector<const xAOD::IParticle*>& acceptedSignals,
			MissingETBase::Types::weight_t& objWeight) const;

  private:
    double m_mu_minP;
    double m_mu_minPt;
    double m_mu_maxEta;
    unsigned short m_mu_qualcut; // loose, medium, tight
    int m_mu_nPixHits;
    int m_mu_nSiHits;

    bool m_mu_doStandAlone;
    double m_mu_minEtaSA;
    int m_mu_nPrecisionHits;
    int m_mu_nSegments;

    bool m_mu_doTracks;

    /// Default constructor: 
    METMuonTool();

  }; 

}

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !METRECONSTRUCTION_METMUONTOOL_H
