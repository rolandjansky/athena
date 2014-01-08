/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyROA_H
#define MboyROA_H

#include "MboyCoreEvt/MboyCoreEvtMisc.h"

  /**
   @class MboyROA

  @author samusog@cern.ch
  
  */

class MboyROA{
public:
   MboyROA(
           const double& ROAThe ,
           const double& ROAPhi ,
           const double& ROAR3d 
          );
   MboyROA(
           const double& Point_x ,
           const double& Point_y ,
           const double& Point_z ,
           const double& Direc_x ,
           const double& Direc_y ,
           const double& Direc_z  
          );
   MboyROA(
           const double& Point_x ,
           const double& Point_y ,
           const double& Point_z ,
           const double& Direc_x ,
           const double& Direc_y ,
           const double& Direc_z ,
           const double& DeltPhi ,
           const double& DeltEta  
          );
   virtual ~MboyROA();

public:
///////////////////////////////////
  
   double Get_Point_x() const ;
   double Get_Point_y() const ;
   double Get_Point_z() const ;
   double Get_Direc_x() const ;
   double Get_Direc_y() const ;
   double Get_Direc_z() const ;
   double Get_DeltPhi() const ;
   double Get_DeltEta() const ;
 
   double Get_ROAThe() const ;
   double Get_ROAPhi() const ;
   double Get_ROAR3d() const ;

private:
///////////////////////////////////

  double m_Point_x ; //!< Point coordinates
  double m_Point_y ; //!< Point coordinates
  double m_Point_z ; //!< Point coordinates
  double m_Direc_x ; //!< Direc coordinates
  double m_Direc_y ; //!< Direc coordinates
  double m_Direc_z ; //!< Direc coordinates
  double m_DeltPhi ; //!< Delta Phi
  double m_DeltEta ; //!< Delta Eta

  double m_ROAThe ; //!< ROA The
  double m_ROAPhi ; //!< ROA Phi
  double m_ROAR3d ; //!< ROA R3d

  void SetROAVariables();
  void SetRoadVariables();
  
};
#endif
