///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCellsDecorator.h 
// Header file for class TileCellsDecorator
/////////////////////////////////////////////////////////////////// 
#ifndef DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_TILECELLSDECORATOR_H
#define DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_TILECELLSDECORATOR_H 1


#include "GaudiKernel/ToolHandle.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"

// STL includes
#include <string>
#include <vector>

class CaloCell;
class TileID;
class TileHWID;

namespace xAOD {
  class IParticle;
}


namespace DerivationFramework {

  class TileCellsDecorator: public AthAlgTool { 
    
    public: 
    
      /// Constructor with parameters: 
      TileCellsDecorator( const std::string& type, const std::string& name, const IInterface* parent );
    
      // Athena algtool's Hooks
      StatusCode initialize();
      StatusCode decorate(const xAOD::IParticle* particle, std::vector<const CaloCell*>& cells) const;

    static const InterfaceID& interfaceID();

    private:

      std::string m_prefix;

      bool m_saveTileCellPmtInfo;
      bool m_saveTileCellPositionAndDimention;

      mutable const TileID* m_tileID;  
      mutable const TileHWID* m_tileHWID;  
  }; 
  
}


#endif //> !DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_TILECELLSDECORATOR_H
