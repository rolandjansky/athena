/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRTAbstract_H
#define MuonboyRTAbstract_H


#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MuonboyRTAbstract

   This class is the base class to connect to RT services

  @author samusog@cern.ch
  
  */

class MuonboyRTAbstract{
public:
   MuonboyRTAbstract();
   virtual ~MuonboyRTAbstract();

public:
///////////////////////////////////

   /**Get radius */
   virtual void RfT(int& TheRimRTSvc,
                    int& Ident, 
                    double& XXXDCA,double& YYYDCA,double& ZZZDCA,
                    double& XXXDCd,double& YYYDCd,double& ZZZDCd,
                    double& XWiCen,double& YWiCen,double& ZWiCen,
                    double& XWidir,double& YWidir,double& ZWidir,
                    double& RADIUS,
                    int& MboyMdtStatus) = 0 ;

   /**Get drift time */
   virtual void TfTDC(int&  Ident,int& TDCcount,double& TimeFromTDC) = 0 ;

   /**Get RS line */
   virtual std::string PrintRsLine(int Ident, int WireSag, double AdMaxima) = 0 ;

};
#endif

