/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//                                                              //
//  Implementation of class VP12DViewRPhiFishEyeTransformation  //
//                                                              //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>       //
//                                                              //
//  Initial version: April 2007                                 //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "VP1Base/VP12DViewRPhiFishEyeTransformation.h"

//____________________________________________________________________
VP12DViewRPhiFishEyeTransformation::VP12DViewRPhiFishEyeTransformation()
  : IVP12DViewTransformation(), m_fisheyepar(0.1)
{
}
