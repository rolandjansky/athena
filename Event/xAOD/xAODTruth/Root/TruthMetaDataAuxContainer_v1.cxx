/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Local include(s):
#include "xAODTruth/versions/TruthMetaDataAuxContainer_v1.h"

namespace xAOD {

   TruthMetaDataAuxContainer_v1::TruthMetaDataAuxContainer_v1()
   : AuxContainerBase() {

      AUX_VARIABLE( weightNames );
      AUX_VARIABLE( mcChannelNumber );
      AUX_VARIABLE( lhefGenerator );
      AUX_VARIABLE( generators );
      AUX_VARIABLE( evgenProcess );
      AUX_VARIABLE( evgenTune );
      AUX_VARIABLE( hardPDF );
      AUX_VARIABLE( softPDF );
   }

} // namespace xAOD
