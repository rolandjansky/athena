/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingETAuxContainer_v1.h"

using namespace xAOD;

MissingETAuxContainer_v1::MissingETAuxContainer_v1()
  : AuxContainerBase()
{
  AUX_VARIABLE( mpx );
  AUX_VARIABLE( mpy );
  AUX_VARIABLE( sumet );
  AUX_VARIABLE( name );
  AUX_VARIABLE( source );
}

MissingETAuxContainer_v1::~MissingETAuxContainer_v1()
{ }

