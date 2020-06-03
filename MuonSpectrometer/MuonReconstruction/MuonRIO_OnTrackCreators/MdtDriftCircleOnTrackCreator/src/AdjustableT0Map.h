/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AdjustableT0Map_H
#define AdjustableT0Map_H

#include "MuonIdHelpers/MdtIdHelper.h"
#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////////////

  /**
   @class AdjustableT0Map

   This class is used to store the T0 shift by station

  @author samusog@cern.ch
  
  */

class AdjustableT0Map {
public:
    AdjustableT0Map(const MdtIdHelper* idHelp);
    ~AdjustableT0Map()=default;

public:
///////////////////////////////////

  void SetDefaultTShift( double TShift ) ;
  void ResetSetTShift( double Tshift = 0. ) ;
  
  
  void DumpT0Shift() ;
  
  void DumpT0Shift( const Identifier& id ) ;
  void DumpT0Shift( std::string stationNameString, int stationEta, int stationPhi ) ;
  void DumpT0Shift( int stationName, int stationEta, int stationPhi ) ;
  void DumpT0Shift( int IdentSta ) ;
  
  
  void SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift )  ;
  void SetStatTShift( int stationName, int stationEta, int stationPhi, double TShift );
  void SetStatTShift( int IdentSta , double TShift );


  double GetT0Shift( const Identifier& id ) const;
  double GetT0Shift( std::string stationNameString, int stationEta, int stationPhi ) const;
  double GetT0Shift( int stationName, int stationEta, int stationPhi ) const;
  double GetT0Shift( int IdentSta ) const;

  double GetDefaultTShift() const;
  int    NberOfData() const;
  void   GetData(int Irank, int& stationName, int& stationEta, int& stationPhi, double& TShift ) const;

  void   SetEnable();
  void   SetDisable();


private:
///////////////////////////////////
  
  int    GetIdentSta( const Identifier& id ) const;
  int    GetIdentSta( std::string stationNameString, int stationEta, int stationPhi ) const;
  int    GetIdentSta( int stationName, int stationEta, int stationPhi) const;

  void   DecodeIdentSta( int IdentSta, int& stationName, int& stationEta, int& stationPhi) const;

  double m_DefaultTShift;

  const MdtIdHelper* m_mdtIdHelper;
  
  int m_stationEtaMIN    ;
  int m_stationPhiMIN    ;
  int m_stationNameMIN   ;
  int m_stationEtaMAX    ;
  int m_stationPhiMAX    ;
  int m_stationNameMAX   ;
  int m_stationEtaRANGE  ;
  int m_stationPhiRANGE  ;
  int m_stationNameRANGE ;

  std::map<int,double>    m_IdentStaTimeShiftMap      ; //!< Table Stations Ids Time shifts

  int m_Disabled  ;

};

#endif

