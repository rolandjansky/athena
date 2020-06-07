/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AdjustableT0Map.h"

AdjustableT0Map::AdjustableT0Map(const MdtIdHelper* idHelp)
{
  m_mdtIdHelper = idHelp;
  
  m_DefaultTShift = 0. ;

  m_IdentStaTimeShiftMap.clear();

  m_stationPhiMIN    =  -10 ;
  m_stationPhiMAX    =   10 ;
  m_stationEtaMIN    =  -30 ;
  m_stationEtaMAX    =   30 ;
  m_stationNameMIN   = -100 ;
  m_stationNameMAX   =  100 ;
  m_stationEtaRANGE  = m_stationEtaMAX  - m_stationEtaMIN  + 1 ;
  m_stationPhiRANGE  = m_stationPhiMAX  - m_stationPhiMIN  + 1 ;
  m_stationNameRANGE = m_stationNameMAX - m_stationNameMIN + 1 ;
  
  m_Disabled = 0 ;
}

void AdjustableT0Map::SetDefaultTShift( double TShift ){ m_DefaultTShift = TShift ; }

void AdjustableT0Map::ResetSetTShift(double Tshift){
  SetDefaultTShift(Tshift) ;
  m_IdentStaTimeShiftMap.clear();  
}

void AdjustableT0Map::DumpT0Shift(){
  std::cout << "m_DefaultTShift " << m_DefaultTShift <<  std::endl;

  std::map<int,double>::const_iterator it = m_IdentStaTimeShiftMap.begin();
  for(; it!=m_IdentStaTimeShiftMap.end(); it++) {
    std::cout << "DumpT0Shift "  ;
    DumpT0Shift( it->first );
  }
}

void AdjustableT0Map::DumpT0Shift( const Identifier& id ) {
  DumpT0Shift( GetIdentSta(id) );
}
void AdjustableT0Map::DumpT0Shift( std::string stationNameString, int stationEta, int stationPhi ) {
  DumpT0Shift( GetIdentSta(stationNameString,stationEta,stationPhi) );
}
void AdjustableT0Map::DumpT0Shift( int stationName, int stationEta, int stationPhi ) {
  DumpT0Shift( GetIdentSta(stationName,stationEta,stationPhi) );
}
void AdjustableT0Map::DumpT0Shift( int IdentSta ) {
  int stationName ; 
  int stationEta  ; 
  int stationPhi  ;  
  DecodeIdentSta( IdentSta, stationName, stationEta, stationPhi);

  std::cout 
       << " IdentSta    " << IdentSta
       << " stationName " << stationName
       << " " << m_mdtIdHelper->stationNameString(stationName)
       << " stationPhi  " << stationPhi 
       << " stationEta  " << stationEta 
       << " time " << GetT0Shift(IdentSta)
       <<  std::endl;
}



void AdjustableT0Map::SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift ){
  SetStatTShift( GetIdentSta(stationNameString,stationEta,stationPhi) , TShift ) ;
}
void AdjustableT0Map::SetStatTShift( int stationName, int stationEta, int stationPhi, double TShift ){
  SetStatTShift(  GetIdentSta(stationName,stationEta,stationPhi) , TShift );
}
void AdjustableT0Map::SetStatTShift( int IdentSta , double TShift ){
  m_IdentStaTimeShiftMap[IdentSta] = TShift ;
}


double AdjustableT0Map::GetT0Shift( const Identifier& id) const{
  return GetT0Shift( GetIdentSta(id) ) ;
}
double AdjustableT0Map::GetT0Shift( std::string stationNameString, int stationEta, int stationPhi ) const{
  return GetT0Shift( GetIdentSta(stationNameString,stationEta,stationPhi) );
}
double AdjustableT0Map::GetT0Shift( int stationName, int stationEta, int stationPhi ) const{
  return GetT0Shift( GetIdentSta(stationName,stationEta,stationPhi) ) ;
}
double AdjustableT0Map::GetT0Shift( int IdentSta ) const{
  if ( m_Disabled == 1 ) return 0.;
  std::map<int,double>::const_iterator it = m_IdentStaTimeShiftMap.find(IdentSta);
  if (it != m_IdentStaTimeShiftMap.end()) return it->second;
  return m_DefaultTShift ;
}



double AdjustableT0Map::GetDefaultTShift() const{
  return m_DefaultTShift ;
}
int AdjustableT0Map::NberOfData() const{
  return m_IdentStaTimeShiftMap.size() ;
}
void AdjustableT0Map::GetData(int Irank, int& stationName, int& stationEta, int& stationPhi, double& TShift ) const{
  stationName = -9999  ; 
  stationEta  = -9999  ; 
  stationPhi  = -9999  ; 
  TShift      = -9999. ; 
  
  int Kounter = -1 ;
  std::map<int,double>::const_iterator it = m_IdentStaTimeShiftMap.begin();
  for(; it!=m_IdentStaTimeShiftMap.end(); it++) {
    Kounter = Kounter + 1 ;
    if ( Kounter == Irank ){
      int IdentSta = it->first ;
      DecodeIdentSta( IdentSta, stationName, stationEta, stationPhi);
      TShift = GetT0Shift(IdentSta) ;
      break ;
    }
  }
}




int AdjustableT0Map::GetIdentSta( const Identifier& id) const{
  return GetIdentSta( m_mdtIdHelper->stationName(id) , m_mdtIdHelper->stationEta(id) , m_mdtIdHelper->stationPhi(id) ) ;
}
int AdjustableT0Map::GetIdentSta( std::string stationNameString, int stationEta, int stationPhi ) const{
  return GetIdentSta( m_mdtIdHelper->stationNameIndex(stationNameString), stationEta, stationPhi ) ;
}
int AdjustableT0Map::GetIdentSta( int stationName, int stationEta, int stationPhi) const{
  int ToBeReturned = 
   + ( stationEta  - m_stationEtaMIN  )  
   + ( stationPhi  - m_stationPhiMIN  ) * m_stationEtaRANGE
   + ( stationName - m_stationNameMIN ) * m_stationEtaRANGE * m_stationPhiRANGE ;
  return ToBeReturned;
}

void AdjustableT0Map::DecodeIdentSta( int IdentSta, int& stationName, int& stationEta, int& stationPhi) const{
  stationName = m_stationNameMIN + IdentSta / ( m_stationEtaRANGE * m_stationPhiRANGE ) ;
  
  stationPhi = IdentSta - ( stationName - m_stationNameMIN ) * m_stationEtaRANGE * m_stationPhiRANGE  ;
  stationPhi = m_stationPhiMIN + stationPhi / m_stationEtaRANGE   ;
  
  stationEta = IdentSta - ( stationName - m_stationNameMIN ) * m_stationEtaRANGE * m_stationPhiRANGE ;
  stationEta = stationEta - ( stationPhi  - m_stationPhiMIN  ) * m_stationEtaRANGE ;
  stationEta = m_stationEtaMIN + stationEta ;
}

void   AdjustableT0Map::SetEnable() { m_Disabled = 0 ; }
void   AdjustableT0Map::SetDisable(){ m_Disabled = 1 ; }

