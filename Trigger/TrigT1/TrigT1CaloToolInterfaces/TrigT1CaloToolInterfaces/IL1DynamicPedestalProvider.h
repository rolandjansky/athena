// -*- C++ -*-
///////////////////////////////////////////////////////////////////
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// IL1DynamicPedestalProvider.h,
///////////////////////////////////////////////////////////////////

#ifndef TRIGT1CALOTOOLINTERFACES_IL1DYNAMICPEDESTALPROVIDER_H
#define TRIGT1CALOTOOLINTERFACES_IL1DYNAMICPEDESTALPROVIDER_H

#include "GaudiKernel/IAlgTool.h"

namespace LVL1
{

/**
 * Interface for L1CBIDCORRECTIONPROVIDER_H
*/  
static const InterfaceID IID_IL1DynamicPedestalProvider("LVL1::IL1DynamicPedestalProvider",1,0);
  
class IL1DynamicPedestalProvider : virtual public IAlgTool  {
public:
  static const InterfaceID& interfaceID( );

  // return the value of the dynamic pedestal
  virtual int dynamicPedestal(int iEta, int layer, int pedestal, int iBCID, float mu) = 0;
};

inline const InterfaceID& IL1DynamicPedestalProvider::interfaceID()
{
  return IID_IL1DynamicPedestalProvider;
}
  
} // end of namespace

#endif 
