/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyMaterialAGDD2GeoSwitches_H
#define MuonboyMaterialAGDD2GeoSwitches_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyMaterialAGDD2GeoSwitches

   this class manages update of Muonboy material

  @author samusog@cern.ch
  
  */

class MuonboyMaterialAGDD2GeoSwitches{
public:
    MuonboyMaterialAGDD2GeoSwitches();
   ~MuonboyMaterialAGDD2GeoSwitches();

public:
///////////////////////////////////

   /*Load material according to AGDD2GeoSwitches Data */
   void   SetMatiereAGDD2GeoSwitches(std::vector<std::string> AGDD2GeoSwitches);

};

#endif
