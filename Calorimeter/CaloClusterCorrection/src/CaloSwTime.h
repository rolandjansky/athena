/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  /// Inherit constructor.
  using CaloClusterCorrection::CaloClusterCorrection;

  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;


 private:


};

#endif

