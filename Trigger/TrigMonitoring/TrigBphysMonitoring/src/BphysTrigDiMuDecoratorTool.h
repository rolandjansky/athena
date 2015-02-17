///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BphysTrigDiMuDecoratorTool.h 
// Header file for class BphysTrigDiMuDecoratorTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGBPHYSMONITORING_BPHYSTRIGDIMUDECORATORTOOL_H
#define TRIGBPHYSMONITORING_BPHYSTRIGDIMUDECORATORTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

//#include "xAODTracking/VertexFwd.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrigBphysMonitoring/IBphysTrigDiMuDecoratorTool.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODTrigBphys/TrigBphysContainer.h"

namespace xAOD {
}

// Forward declaration
namespace Trk {
    //    class V0Tools;
}

//static const InterfaceID IID_BphysTrigDiMuDecoratorTool("BphysTrigDiMuDecoratorTool", 1, 0);

class BphysTrigDiMuDecoratorTool :
           virtual public ::IBphysTrigDiMuDecoratorTool,
           virtual public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  BphysTrigDiMuDecoratorTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~BphysTrigDiMuDecoratorTool(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
    StatusCode decorateVertex(const xAOD::Vertex* vtx, const xAOD::Vertex * pv=nullptr) const override;
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
//    static const InterfaceID& interfaceID()
//    {
//        return IID_BphysTrigDiMuDecoratorTool;
//    }
    
  ///////////////////////////////////////////////////////////////////
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  BphysTrigDiMuDecoratorTool();

    ToolHandle < Trk::V0Tools > m_V0Tools; /// For lxy etc
    
  // Containers
    std::string m_pvCandidatesKey; //m_JpsiCandidatesKey

    double m_trackMass;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TRIGBPHYSMONITORING_BPHYSTRIGDIMUDECORATORTOOL_H
