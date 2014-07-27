/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// emacs: this is -*- c++ -*-
//
//   ShifterTool.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: ShifterTool.h,v 1.6 2009-04-09 22:27:26 eozcan Exp $


#ifndef __SHIFTERTOOL_H
#define __SHIFTERTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "TrigIDSCAN/IShifterTool.h"


#include <iostream>
#include <string>
#include <vector>
#include <map>


class StoreGateSvc;



class ShifterTool : public AlgTool, virtual public IShifterTool {

public:

  ShifterTool( const std::string&, const std::string&, const IInterface* );
  virtual ~ShifterTool(){};
  
  virtual StatusCode initialize();
  StatusCode finalize();


  // interface
  StatusCode execute(ToolHandle<ISpacePointProvider>  spacePointProvider,
		     double etaMinus, double etaPlus,
		     double phiMin,   double phiMax, double& shift);

  void clear();

  // accessors
  double get_xshift()    const { return m_xShift; } 
  double get_yshift()    const { return m_yShift; } 
  bool   translateOnly() const { return m_translateOnly; }

  double get_shifterz()  const { return m_shifterz; }

  // set automatic shift status 
  double xShift(double x) { return m_xShift = x; } 
  double yShift(double y) { return m_yShift = y; } 
  bool   translateOnly(bool b) { return m_translateOnly = b; }

  // get the corresponding original spacepoint 
  // from the shifted ptr 
  TrigSiSpacePoint* mapPoint(TrigSiSpacePoint* sp) const;

  std::vector<TrigSiSpacePoint*> spVec() const { return m_spVec; }

  int HitCount() const { return m_spVec.size(); }

private:  // data members

  void translate(std::vector<TrigSiSpacePoint*>& sp);
  double getShift(std::vector<TrigSiSpacePoint*>& sp);

private:

  mutable MsgStream m_athenaLog;

  bool m_translateOnly;

  double m_xShift;
  double m_yShift;

  double m_shifterz;

  bool m_boundary;

  std::vector<TrigSiSpacePoint*>  m_spVec;

  std::map<TrigSiSpacePoint*, TrigSiSpacePoint* > m_spMap;
  std::string m_pixelSpName,m_sctSpName;
  StoreGateSvc*        m_StoreGate;
  bool m_usePixels,m_useSCT;
  int m_maxPIXOccupancy, m_maxSCTOccupancy;
  double m_shiftRoadRadius;
};


#endif  // __SHIFTERTOOL_H 










