/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementBoundaryLinks_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementBoundaryLinks_xk_H
#define SiDetElementBoundaryLinks_xk_H

#include "SiSPSeededTrackFinderData/SiDetElementBoundaryLink_xk.h"
#include <vector>

namespace InDet {
  typedef std::vector<SiDetElementBoundaryLink_xk> SiDetElementBoundaryLinks_xk;
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDet::SiDetElementBoundaryLinks_xk , 235877993 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDet::SiDetElementBoundaryLinks_xk , 112316929 );

#endif // SiDetElementBoundaryLinks_xk
