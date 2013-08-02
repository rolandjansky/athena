/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILARFECLVTEMPDCSTOOL_H
#define ILARFECLVTEMPDCSTOOL_H

/**
 @class ILArFecLvTempDcsTool
 @brief Interface for Tool to provide FEC LV TEMP DCS information.

 Environment:
      Software developed for the ATLAS Detector at the CERN LHC

 @author 
      Denis Oliveira Damazio <damazio@bnl.gov>
*/

// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <string>

class LArFecLvTempDcs;
class HWIdentifier;

static const InterfaceID IID_ILArFecLvTempDcsTool("ILArFecLvTempDcsTool", 1 ,0);


class ILArFecLvTempDcsTool: virtual public IAlgTool
{
 
  public: 
  virtual ~ILArFecLvTempDcsTool () { } 

  static const InterfaceID& interfaceID() { return IID_ILArFecLvTempDcsTool; }

  virtual StatusCode getV1( const std::string& cratename, float&  volt  ) = 0 ; 
  virtual StatusCode getFec( const std::string& cratename
                        , LArFecLvTempDcs&  volt  ) = 0 ;
  virtual StatusCode getFec( const HWIdentifier& crateID
                        , LArFecLvTempDcs&  volt  ) = 0 ;

};

#endif
