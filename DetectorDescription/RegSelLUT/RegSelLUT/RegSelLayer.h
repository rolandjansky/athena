// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelLayer.h  
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Wed Apr  4 17:13:53 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef REGSELLAYER_H
#define REGSELLAYER_H


#include <vector>
#include <iostream>



#include "RegSelLUT/ZRObject.h"
#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/RegSelModule.h"


class RegSelLayer : public ZRObject {

public:
  
  RegSelLayer() : 
    ZRObject(0,0,0,0), 
    m_set(false), m_ID(0),
    m_modules(0),   m_disabled(0), m_Nphi(0),
    m_ideltaphi(0), m_phiMin(0), m_phiMax(0), m_phimaps(0) { }   
  
  RegSelLayer(double rmin, double rmax, double zmin, double zmax) :
    ZRObject(rmin, rmax, zmin, zmax), 
    m_set(true), m_ID(0), m_modules(0), m_disabled(0), m_Nphi(0),
    m_ideltaphi(0), m_phiMin(0), m_phiMax(0), m_phimaps(0) { }    
  
  void reset(); 

  void addModule(const RegSelModule& module);
  void setupDisabledModuleList(); 

  void getModules(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const;
  void getModules(std::vector<const RegSelModule*>& modules) const;

  void getDisabledModules(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const;

  int ID() const { return m_ID; } 
  int ID(int id) { m_ID = id; return m_ID; } 

  int size() const { return m_modules.size(); } 

  void createMaps();

  double phiMin() const { return m_phiMin; } 
  double phiMax() const { return m_phiMax; } 

  double phiMin(double phimin) { return m_phiMin=phimin; } 
  double phiMax(double phimax) { return m_phiMax=phimax; } 


private:

  // code to cluster the module positions to calculate 
  // the appropriate phi segemntation for the layer 

  class PhiCluster { 
  public:
    // add and cluster module 
    void addModule(const RegSelModule& m);
    // how many phi clusters are there?
    unsigned size() const { return m_N.size(); }
  private:
    std::vector<int>    m_N; 
    std::vector<double> m_positions;
  };

  // driver routine to pick which modules to cluster
  int  clusterModules() const;

private:

  bool m_set;
  int  m_ID;

  std::vector<const RegSelModule*> m_modules;
  std::vector<const RegSelModule*> m_disabled;

  //  std::vector<const RegSelModule*> m_enabledmodules;


  int m_Nphi;

  double m_ideltaphi;

  double m_phiMin;
  double m_phiMax; 

  std::vector<std::vector<const RegSelModule*> >          m_phimaps;
    
};


std::ostream& operator<<(std::ostream& s, const RegSelLayer& layer);


#endif  /* REGSELLAYER_H */
