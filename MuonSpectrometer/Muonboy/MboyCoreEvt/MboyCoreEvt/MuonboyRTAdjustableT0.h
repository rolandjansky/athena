/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRTAdjustableT0_H
#define MuonboyRTAdjustableT0_H


#include "MboyCoreEvt/MboyCoreEvtMisc.h"

#include "MboyCoreEvt/MuonboyRTAbstract.h"
#include "MboyCoreEvt/MuonRecDigiStore.h"

  /**
   @class MuonboyRTAdjustableT0

   This class is the base class to connect to RT services
   with T shift possible
   It derives from MuonboyRTAbstract

  @author samusog@cern.ch
  
  */

class MuonboyRTAdjustableT0: public MuonboyRTAbstract{
public:
   MuonboyRTAdjustableT0();
   virtual ~MuonboyRTAdjustableT0();

public:
///////////////////////////////////

   /**Get drift time */
   virtual void TfTDC(int&  Ident,int& TDCcount,double& TimeFromTDC) ;

   /**Get RS line */
   virtual std::string PrintRsLine(int Ident, int WireSag, double AdMaxima) ;

public:
///////////////////////////////////
   virtual void DumpT0Shift() = 0;
   virtual void SetVerboset0shift() = 0;
   virtual void SetDefaultTShift( double Tshift )= 0;
   virtual void ResetSetTShift( double Tshift = 0. ) = 0;
   virtual void SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift ) = 0 ;

   virtual void SetEnable()= 0;
   virtual void SetDisable()= 0;


public:
 ///////////////////////////////////
  MuonboyRTAbstract*      p_MuonboyRTAbstract ; //!< The pointer on MuonboyRTAbstract

};
#endif

