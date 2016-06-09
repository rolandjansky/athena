/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWTIME_H
#define CALOCLUSTERCORRECTION_CALOSWTIME_H
/********************************************************************

NAME:     CaloSwTime.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  G.Unal      
CREATED:  December 2008   

PURPOSE:  Compute time of SW clusters
          
********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrection.h"


class CaloSwTime : public  CaloClusterCorrection
{
public:
  // constructor 
  CaloSwTime (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloSwTime() override;

  virtual void makeCorrection (const EventContext& ctx,
                               xAOD::CaloCluster* cluster) const override;


 private:


};

#endif

