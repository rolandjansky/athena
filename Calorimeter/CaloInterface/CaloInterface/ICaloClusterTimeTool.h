/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICALOCLUSTERTIMETOOL_H
#define ICALOCLUSTERTIMETOOL_H
/********************************************************************

NAME:     ICaloClusterTimeTool.h
PACKAGE:  offline/Calorimeter/CaloInterface

AUTHORS:  D. Hu      
CREATED:  Jan 2012   

PURPOSE: interface of CaloClusterTimeTool 
          
********************************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class CaloCell;

static const InterfaceID IID_ICaloClusterTimeTool("ICaloClusterTimeTool",1,0); 

class ICaloClusterTimeTool : virtual public IAlgTool 
{
public:
  
  static const InterfaceID& interfaceID();

  virtual void makeClusterTimeCorrection(float pv_z,const xAOD::CaloCluster* cluster, float& time_corr, float& error, unsigned int& flag) = 0;
  virtual void makeCellTimeCorrection(float pv_z,const CaloCell* cell, float& time_corr, float& error, unsigned int& flag) = 0;

};

inline const InterfaceID& ICaloClusterTimeTool::interfaceID()
{
  return IID_ICaloClusterTimeTool;
}

#endif

