/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyROA.h"
#include <cmath>
MboyROA::MboyROA( 
           const double& ROAThe ,
           const double& ROAPhi ,
           const double& ROAR3d 
){
  m_ROAThe  = ROAThe ;
  m_ROAPhi  = ROAPhi ;
  m_ROAR3d  = ROAR3d ;
  m_DeltPhi = 1.      ;
  m_DeltEta = 1.      ;
  SetRoadVariables() ;
}
MboyROA::MboyROA( 
           const double& Point_x ,
           const double& Point_y ,
           const double& Point_z ,
           const double& Direc_x ,
           const double& Direc_y ,
           const double& Direc_z  
){
  m_Point_x = Point_x ;
  m_Point_y = Point_y ;
  m_Point_z = Point_z ;
  m_Direc_x = Direc_x ;
  m_Direc_y = Direc_y ;
  m_Direc_z = Direc_z ;
  m_DeltPhi = 1.      ;
  m_DeltEta = 1.      ;
  SetROAVariables() ;
}
MboyROA::MboyROA( 
           const double& Point_x ,
           const double& Point_y ,
           const double& Point_z ,
           const double& Direc_x ,
           const double& Direc_y ,
           const double& Direc_z ,
           const double& DeltPhi ,
           const double& DeltEta  
){
  m_Point_x = Point_x ;
  m_Point_y = Point_y ;
  m_Point_z = Point_z ;
  m_Direc_x = Direc_x ;
  m_Direc_y = Direc_y ;
  m_Direc_z = Direc_z ;
  m_DeltPhi = DeltPhi ;
  m_DeltEta = DeltEta ;
  SetROAVariables() ;
}
MboyROA::~MboyROA(){}

double MboyROA::Get_Point_x() const { return m_Point_x ; } 
double MboyROA::Get_Point_y() const { return m_Point_y ; } 
double MboyROA::Get_Point_z() const { return m_Point_z ; } 
double MboyROA::Get_Direc_x() const { return m_Direc_x ; } 
double MboyROA::Get_Direc_y() const { return m_Direc_y ; } 
double MboyROA::Get_Direc_z() const { return m_Direc_z ; } 
double MboyROA::Get_DeltPhi() const { return m_DeltPhi ; } 
double MboyROA::Get_DeltEta() const { return m_DeltEta ; } 

double MboyROA::Get_ROAThe() const { return m_ROAThe ; } 
double MboyROA::Get_ROAPhi() const { return m_ROAPhi ; } 
double MboyROA::Get_ROAR3d() const { return m_ROAR3d ; } 

void MboyROA::SetROAVariables(){

  m_ROAR3d = sqrt( std::pow(m_Point_x,2) + std::pow(m_Point_y,2) + std::pow(m_Point_z,2) )  ;
  m_ROAPhi = atan2(m_Point_y,m_Point_x) ;
  m_ROAThe = acos(m_Point_z/m_ROAR3d);

}

void MboyROA::SetRoadVariables(){

  m_Direc_x = sin(m_ROAThe) * cos(m_ROAPhi) ;
  m_Direc_y = sin(m_ROAThe) * sin(m_ROAPhi) ;
  m_Direc_z = cos(m_ROAThe)                 ;

  m_Point_x = m_ROAR3d * m_Direc_x ;
  m_Point_y = m_ROAR3d * m_Direc_y ;
  m_Point_z = m_ROAR3d * m_Direc_z ;

}
