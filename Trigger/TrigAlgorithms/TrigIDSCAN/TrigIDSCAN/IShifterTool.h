// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   IShifterTool.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: IShifterTool.h,v 1.5 2009-01-21 13:41:53 eozcan Exp $


#ifndef __ISHIFTERTOOL_H
#define __ISHIFTERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigSiSpacePointTool/ISpacePointProvider.h"

#include <iostream>


static const InterfaceID IID_IShifterTool("IShifterTool", 1 , 0); 

class IShifterTool : virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_IShifterTool;
  }
  
  virtual StatusCode execute(ToolHandle<ISpacePointProvider>  spacePointProvider,
			     double etaMinus, double etaPlus,
			     double phiMin,   double phiMax, double&) = 0;

  virtual void clear() = 0;


  virtual double get_xshift() const = 0; 
  virtual double get_yshift() const = 0; 
  virtual bool   translateOnly() const = 0;

  virtual double get_shifterz() const = 0;

  virtual double xShift(double x) = 0;
  virtual double yShift(double y) = 0;
  virtual bool   translateOnly(bool b) = 0;

  virtual TrigSiSpacePoint* mapPoint(TrigSiSpacePoint* sp) const = 0;
       
  virtual std::vector<TrigSiSpacePoint*> spVec() const = 0;

  virtual int HitCount() const = 0;
};


#endif  // __ISHIFTERTOOL_H 










