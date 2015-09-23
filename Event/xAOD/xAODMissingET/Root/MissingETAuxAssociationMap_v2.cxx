/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingETAuxAssociationMap_v2.h"

using namespace xAOD;

MissingETAuxAssociationMap_v2::MissingETAuxAssociationMap_v2()
  : AuxContainerBase()
{
  AUX_VARIABLE( jetLink );
  AUX_VARIABLE( objectLinks );
  AUX_VARIABLE( calpx );
  AUX_VARIABLE( calpy );
  AUX_VARIABLE( calpz );
  AUX_VARIABLE( cale  );
  AUX_VARIABLE( calsumpt  );
  AUX_VARIABLE( calkey  );
  AUX_VARIABLE( trkpx );
  AUX_VARIABLE( trkpy );
  AUX_VARIABLE( trkpz );
  AUX_VARIABLE( trke  );
  AUX_VARIABLE( trksumpt  );
  AUX_VARIABLE( trkkey  );
  AUX_VARIABLE( jettrkpx );
  AUX_VARIABLE( jettrkpy );
  AUX_VARIABLE( jettrkpz );
  AUX_VARIABLE( jettrke  );
  AUX_VARIABLE( jettrksumpt  );
  AUX_VARIABLE( overlapIndices );
  AUX_VARIABLE( overlapTypes );
  AUX_VARIABLE( isMisc );
}

MissingETAuxAssociationMap_v2::~MissingETAuxAssociationMap_v2()
{ }

