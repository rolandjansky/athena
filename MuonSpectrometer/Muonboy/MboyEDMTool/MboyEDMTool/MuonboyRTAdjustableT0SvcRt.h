/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRTAdjustableT0SvcRt_H
#define MuonboyRTAdjustableT0SvcRt_H


#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonboyRTAdjustableT0.h"

#include "MuonRecToolInterfaces/IAdjustableT0Tool.h"

  /**
   @class MuonboyRTAdjustableT0SvcRt

   This class is the base class to connect to RT services
   with T shift possible 
   One use the RT svc
   It derives from MuonboyRTAdjustableT0

  @author samusog@cern.ch
  
  */

class MuonboyRTAdjustableT0SvcRt: public MuonboyRTAdjustableT0{
public:
   MuonboyRTAdjustableT0SvcRt();
   virtual ~MuonboyRTAdjustableT0SvcRt();

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
                    int& MboyMdtStatus) ;

public:
///////////////////////////////////
   virtual void DumpT0Shift() ;
   virtual void SetVerboset0shift() ;
   virtual void SetDefaultTShift( double Tshift ) ;
   virtual void ResetSetTShift( double Tshift = 0. ) ;
   virtual void SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift )  ;

   virtual void SetEnable() ;
   virtual void SetDisable() ;

public:
///////////////////////////////////
// Data

   AdjT0::IAdjustableT0Tool* p_IAdjustableT0Tool ; //!< Pointer On IAdjustableT0Tool
  

};
#endif

