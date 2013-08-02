/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILARFEBTEMPTOOL_H
#define ILARFEBTEMPTOOL_H

/**
 @class ILArFEBTempTool
 @brief Interface for Tool to provide FEB TEMP DCS information.

 Environment:
      Software developed for the ATLAS Detector at the CERN LHC

 @author 
      Kirill Skovpen <kskovpen@cern.ch>
*/

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <string>

class HWIdentifier;

static const InterfaceID IID_ILArFEBTempTool("ILArFEBTempTool", 1 ,0);

typedef std::vector<std::pair<float,float> > FEBTemp;

class ILArFEBTempTool: virtual public IAlgTool
{
 public: 
   virtual ~ILArFEBTempTool () { } 
   
   static const InterfaceID& interfaceID() { return IID_ILArFEBTempTool; }

   virtual FEBTemp getFebTemp( const HWIdentifier& id ) = 0 ;   
};

#endif
