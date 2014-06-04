/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingETAuxComponentMap_v1.h"

using namespace xAOD;

MissingETAuxComponentMap_v1::MissingETAuxComponentMap_v1()
  : AuxContainerBase()
{
  AUX_VARIABLE( metLink );
  AUX_VARIABLE( objectLinks );
  AUX_VARIABLE( wpx );
  AUX_VARIABLE( wpy );
  AUX_VARIABLE( wet );
  AUX_VARIABLE( statusWord );
}

MissingETAuxComponentMap_v1::~MissingETAuxComponentMap_v1()
{ }

