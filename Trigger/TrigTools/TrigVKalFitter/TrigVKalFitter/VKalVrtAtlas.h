/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrtAtlas.h
//
#ifndef _TrigVKalAthena_TrigVKalAtlas_H
#define _TrigVKalAthena_TrigVKalAtlas_H

// Mag field service
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include  "TrkVKalVrtCore/VKalVrtBMag.h"
// External propagator
#include "TrkVKalVrtCore/Propagator.h"
//#include "TrkExInterfaces/IExtrapolator.h"

//  ATLAS magnetic field access for TrkVKalVrtCore
//-------------------------------------------------

  class TrigAtlasMagFld : public Trk::baseMagFld  {
    public:

       TrigAtlasMagFld();
      ~TrigAtlasMagFld();
       void getMagFld(const double,const double,const double,double&,double&,double&);
       void setAtlasMag( MagField::IMagFieldSvc * );
       void setAtlasMag(const double );

    private:
    
       //MagFieldAthena* m_VKalAthenaField;                 //VK Old field
       MagField::IMagFieldSvc*  m_VKalAthenaField;
       double m_FIXED_ATLAS_FIELD;
   
   };


#endif
