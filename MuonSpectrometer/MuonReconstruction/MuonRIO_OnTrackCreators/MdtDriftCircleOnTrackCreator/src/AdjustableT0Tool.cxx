/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "MuonIdHelpers/MdtIdHelper.h"

#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////
#include "AdjustableT0Tool.h"

//----------------------------------------------------------------//
#include "AdjustableT0Map.h"

namespace AdjT0
{

AdjustableT0Tool::AdjustableT0Tool(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t,n,p)
{

  declareInterface< IMuonTofTool >(this) ;
  declareInterface< IAdjustableT0Tool >(this) ;
 
//Declare the properties

  declareProperty( "DoTof" , m_DoTof = 1 ) ;

  m_inverseSpeedOfLight = 1./299.792458;

  m_Verbose = 0 ;

  p_AdjustableT0Map = 0 ;

}

StatusCode AdjustableT0Tool::initialize()
{

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  ATH_MSG_INFO(  "================================" ) ;
  ATH_MSG_INFO(  "=Proprieties are     " ) ;
  ATH_MSG_INFO(  "= DoTof                         " << m_DoTof           ) ;
  ATH_MSG_INFO(  "================================" ) ;

  ATH_CHECK( m_muonIdHelperTool.retrieve() );
  p_AdjustableT0Map = new AdjustableT0Map(m_muonIdHelperTool.get()) ;

  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;
}

AdjustableT0Tool::~AdjustableT0Tool(){ delete p_AdjustableT0Map ;}

StatusCode AdjustableT0Tool::finalize(){return StatusCode::SUCCESS;}

void AdjustableT0Tool::SetVerboset0shift(){ 
 m_Verbose = 1 ; 
}
  
void AdjustableT0Tool::SetDefaultTShift( double TShift ){ 
 p_AdjustableT0Map->SetDefaultTShift(TShift) ; 
}

void AdjustableT0Tool::DumpT0Shift(){  
 p_AdjustableT0Map->DumpT0Shift() ; 
}

void AdjustableT0Tool::ResetSetTShift(double Tshift){
  m_Verbose = 0 ;
  p_AdjustableT0Map->ResetSetTShift(Tshift);
}

void AdjustableT0Tool::SetStatTShift( std::string stationNameString, int stationEta, int stationPhi, double TShift ){
 p_AdjustableT0Map->SetStatTShift( stationNameString, stationEta, stationPhi, TShift ) ;
}


double AdjustableT0Tool::GetDefaultTShift() const{
  return p_AdjustableT0Map->GetDefaultTShift();
}
int AdjustableT0Tool::NberOfData() const{
  return p_AdjustableT0Map->NberOfData();
}
void AdjustableT0Tool::GetData(int Irank, int& stationName, int& stationEta, int& stationPhi, double& TShift ) const{
  return p_AdjustableT0Map->GetData(Irank,stationName,stationEta,stationPhi,TShift );
}

double AdjustableT0Tool::timeOfFlight( const Identifier& id, const Amg::Vector3D& pos ) const{

//double tof = - p_AdjustableT0Map->GetT0Shift(id)  ;
  double tof =   p_AdjustableT0Map->GetT0Shift(id)  ;
  if ( m_DoTof == 1 ) tof += pos.mag()*m_inverseSpeedOfLight ;
  
  if ( m_Verbose == 1 ) {
    std::cout << "timeOfFlight " ;
    p_AdjustableT0Map->DumpT0Shift(id) ;
  }

  return tof;
}

void AdjustableT0Tool::SetEnable(){
  return p_AdjustableT0Map->SetEnable();
}
void AdjustableT0Tool::SetDisable(){
  return p_AdjustableT0Map->SetDisable();
}


}
