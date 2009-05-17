/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthExJobOptions/KeyMixin.h"

#include "AthenaBaseComps/AthAlgorithm.h"


/////////////////////////////////////////////////////////////////////////////

KeyMixin::KeyMixin( AthAlgorithm* alg )
{
  
// declare key properties for the algorithm

   alg->declareProperty( "InputKey",  m_inputKey  = "",       "key to locate input data" );
   alg->declareProperty( "OutputKey", m_outputKey = "outkey", "key for writing output data" );
}
