/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARDIGITOSCILLATIONCORRTOOL_H
#define LARELECCALIB_ILARDIGITOSCILLATIONCORRTOOL_H

#include "GaudiKernel/IAlgTool.h"
class LArDigitContainer;

class ILArDigitOscillationCorrTool: virtual public IAlgTool {
  /**
   * AlgoTool to correct for oscillating noise
   *
   * @author T. Barillari
   * @version  \$Id: ILArDigitOscillationCorrTool.h,v 1.2 2008-09-27 10:23:37 wlampl Exp $*/

 public: 
  virtual ~ILArDigitOscillationCorrTool() {};
  
  virtual StatusCode calculateEventPhase(const LArDigitContainer &theDC) = 0;
  virtual StatusCode correctLArDigits(LArDigitContainer &theDC) = 0;

};

#endif 





