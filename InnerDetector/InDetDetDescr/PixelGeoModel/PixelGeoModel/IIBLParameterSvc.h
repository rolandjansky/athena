// Hey emacs, this is -*- C++ -*-.
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IBLParameterSvc.h
//   Header file for class IBLParameterSvc
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//

#ifndef PIXELGEOMODEL_IIBLPARAMETERSVC_H
#define PIXELGEOMODEL_IIBLPARAMETERSVC_H


#include "GaudiKernel/IInterface.h"
#include <string>


class IIBLParameterSvc
  : virtual public IInterface
{
public:
  DeclareInterfaceID (IIBLParameterSvc, 1, 0);

  virtual void setBoolParameters(bool& param, const std::string& paramName) = 0;
};


#endif // not PIXELGEOMODEL_IIBLPARAMETERSVC_H

