/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONALIGNHELPERTOOL_H
#define MUONALIGNGENTOOLS_MUONALIGNHELPERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"

#include <math.h>

/**
   @file MuonAlignHelperTool.h
   @class MuonAlignHelperTool

   @brief Tool containing miscellaneous methods useful for muon alignment 

   @author Robert Harrington <roberth@bu.edu>
   @date 8/27/2010
*/

class IMessageSvc;

namespace Muon {
  class MuonIdHelperTool;

  namespace MuonAlign {
    float RadsToDegrees(float Radians);
    double FixRad(double Radians);
  }

  enum StationLayer { Inner, Middle, Outer, Other };  

  static const InterfaceID ID_MuonAlignHelperTool ( "Muon::MuonAlignHelperTool",1,0);

  class MuonAlignHelperTool : public AthAlgTool {
    
  public:
    MuonAlignHelperTool(const std::string& type,
			const std::string& name,
			const IInterface* parent);
    ~MuonAlignHelperTool();

    StatusCode initialize();
    StatusCode finalize();

    bool isEndcap(Identifier id) const;
    bool isBIS8(Identifier id) const;
    StationLayer getMdtStatLayer(Identifier id) const;

    static const InterfaceID& interfaceID() { return ID_MuonAlignHelperTool; }

  private:
    ToolHandle<MuonIdHelperTool> m_muonIdHelper;

    
  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MUONALIGNHELPERTOOL_H
