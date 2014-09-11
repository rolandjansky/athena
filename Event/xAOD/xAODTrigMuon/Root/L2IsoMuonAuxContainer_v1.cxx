/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2IsoMuonAuxContainer_v1.cxx 605591 2014-07-09 12:07:31Z krasznaa $

// Local include(s):
#include "xAODTrigMuon/versions/L2IsoMuonAuxContainer_v1.h"

namespace xAOD {

   L2IsoMuonAuxContainer_v1::L2IsoMuonAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( pt );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( eta );

      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( charge );
      AUX_VARIABLE( errorFlag );
      AUX_VARIABLE( sumPt01 );
      AUX_VARIABLE( sumPt02 );
      AUX_VARIABLE( sumPt03 );
      AUX_VARIABLE( sumPt04 );
      AUX_VARIABLE( sumEt01 );
      AUX_VARIABLE( sumEt02 );
      AUX_VARIABLE( sumEt03 );
      AUX_VARIABLE( sumEt04 );
   }

} // namespace xAOD
