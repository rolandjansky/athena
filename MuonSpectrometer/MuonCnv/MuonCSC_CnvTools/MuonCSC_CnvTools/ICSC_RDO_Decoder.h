/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOL_ICSC_RDO_Decoder_H
#define MUONCSC_CNVTOOL_ICSC_RDO_Decoder_H

#include "GaudiKernel/IAlgTool.h"

class Identifier;
class CscRawData;

namespace Muon {

/** This class provides conversion from RDO data to CSC RDO
   * Author: Ketevi A. Assamagan
   * BNL December 27 2003
   */

class ICSC_RDO_Decoder : virtual public IAlgTool {

public:
  DeclareInterfaceID(ICSC_RDO_Decoder, 1, 0);

  virtual void getDigit(const CscRawData * rawData, Identifier& moduleId,
                Identifier& channelId, double& adc, double& time) const=0;
  virtual Identifier stationIdentifier(const CscRawData* rawData) const=0;
  virtual Identifier channelIdentifier(const CscRawData * rawData, int j) const=0;
};

}

#endif
