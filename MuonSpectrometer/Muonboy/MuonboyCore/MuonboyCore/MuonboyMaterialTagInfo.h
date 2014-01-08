/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyMaterialTagInfo_H
#define MuonboyMaterialTagInfo_H

#include "MuonboyCore/MuonboyCoreMisc.h"

  /**
   @class MuonboyMaterialTagInfo

   this class manages update of Muonboy material

  @author samusog@cern.ch
  
  */

class MuonboyMaterialTagInfo{
public:
    MuonboyMaterialTagInfo();
   ~MuonboyMaterialTagInfo();

public:
///////////////////////////////////

   /*Load material according to TagInfoXML*/
   void   SetMatiereTagInfo(std::vector< std::pair<std::string,std::string> > TagInfoXML);

};

#endif
