/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAdjustableT0Tool_H
#define IAdjustableT0Tool_H

#include "MuonRecToolInterfaces/IMuonTofTool.h"

  /**
   @class IAdjustableT0Tool

   This class is used to adjust the T0 shift. 
   It is highly inspired from MuGirlStau code
   
  @author samusog@cern.ch
  
  */

namespace AdjT0
{

class IAdjustableT0Tool : public Muon::IMuonTofTool
{
public:
   virtual void SetVerboset0shift() = 0;
   
   virtual void DumpT0Shift() = 0;
   
   virtual void SetDefaultTShift( double Tshift )= 0;
   virtual void ResetSetTShift( double Tshift = 0. ) = 0;
   virtual void SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift ) = 0 ;

   virtual double GetDefaultTShift() const = 0 ;
   virtual int    NberOfData() const = 0 ;
   virtual void   GetData(int Irank, int& stationName, int& stationEta, int& stationPhi, double& TShift ) const = 0 ;

   virtual void SetEnable() = 0 ;
   virtual void SetDisable() = 0 ;
};

}
#endif // IAdjustableT0Tool_H
