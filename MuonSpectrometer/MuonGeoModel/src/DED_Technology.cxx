/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 thick support panel on one side of the RPC
 ------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DED_Technology.cxx,v 1.1 2008-07-31 10:57:55 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonGeoModel/DED_Technology.h"

namespace MuonGM {

    DED::DED(std::string s) : Technology(s), AlThickness(0.), HoneyCombThickness(0.) {}

} // namespace MuonGM
