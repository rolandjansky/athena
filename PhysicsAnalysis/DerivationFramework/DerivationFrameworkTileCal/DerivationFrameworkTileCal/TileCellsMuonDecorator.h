///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCellsMuonDecorator.h 
// Header file for class TileCellsMuonDecorator
/////////////////////////////////////////////////////////////////// 
#ifndef DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_TILECELLSMUONDECORATOR_H
#define DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_TILECELLSMUONDECORATOR_H 1


#include "GaudiKernel/ToolHandle.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"



#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h" 


// PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkTileCal/TileCellsDecorator.h"
#include "../src/TrackTools.h"

// STL includes
#include <string>
#include <vector>


// Forward declaration
namespace Trk {
  //  class IParticleCaloExtensionTool;
}

namespace Rec {
  class IParticleCaloCellAssociationTool;
}

namespace DerivationFramework {
  //  class TileCellsDecorator;
}

namespace xAOD {
  class ITrackParticlesInConeTool;
}


class CaloCell;
class TileID;
class TileHWID;
class TileCellsDecorator;

namespace DerivationFramework {

  class TileCellsMuonDecorator: virtual public IAugmentationTool
                                , public AthAlgTool { 
    
    public: 
    
      /// Constructor with parameters: 
      TileCellsMuonDecorator( const std::string& type, const std::string& name, const IInterface* parent );
    
      virtual StatusCode addBranches() const;

      // Athena algtool's Hooks
      StatusCode initialize();

    private:

    //      ToolHandle <Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool;
      ToolHandle<ITrackTools> m_trackInCalo;
      ToolHandle<xAOD::ITrackParticlesInConeTool> m_tracksInCone;
      ToolHandle<DerivationFramework::TileCellsDecorator> m_cellsDecorator;
  

      std::string m_muonContainer;
      std::string m_cellContainer;

      std::string m_prefix;
      bool m_selectMuons;
      double m_minPt;
      double m_maxAbsEta;
      double m_isoCone;
      double m_maxRelEtrkInIsoCone;
      bool m_saveTileCellMuonInfo;

  }; 
  
}


#endif //> !DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_TILECELLSMUONDECORATOR_H
