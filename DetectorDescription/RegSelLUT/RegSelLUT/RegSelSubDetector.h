// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelSubDetector.h  
 **
 **   Description:  Basic name class for named objects, just to 
 **                 keep everything tidy with standard calls  
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Wed Apr  4 15:50:48 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __REGSELSUBDETECTOR_H
#define __REGSELSUBDETECTOR_H


#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/RegSelModule.h"
#include "RegSelLUT/RegSelLayer.h"

#include "RegSelLUT/RegSelName.h"
#include "RegSelLUT/ZRObject.h"

#include <vector>
#include <map>






class RegSelSubDetector : public ZRObject, public RegSelName { 
  
public:
  
  RegSelSubDetector() : ZRObject(0,0,0,0), m_mapped(false) { } 

  RegSelSubDetector(int ID, int Nlayers, const std::string& s) : 
    ZRObject(0,0,0,0), RegSelName(s), m_ID(ID), m_Nlayers(Nlayers), m_mapped(false)   
  {  } // m_layer.resize(m_Nlayers); } 

  RegSelSubDetector(int ID, const std::string& s) : 
    ZRObject(0,0,0,0), RegSelName(s), m_ID(ID), m_Nlayers(0), m_mapped(false)   
  {  } 

  RegSelSubDetector(int ID) :
    ZRObject(0,0,0,0), RegSelName(""), m_ID(ID), m_Nlayers(0), m_mapped(false)   
  {  } 
  

  
  RegSelSubDetector(double rmin, double rmax, 
		    double zmin, double zmax) :
    ZRObject(rmin, rmax, zmin, zmax), m_Nlayers(0), m_mapped(false)   
  {  } // m_layer.resize(m_Nlayers); } 
  

  RegSelSubDetector(double rmin, double rmax, 
		    double zmin, double zmax, 
		    const std::string& s) :
    ZRObject(rmin, rmax, zmin, zmax), RegSelName(s), m_Nlayers(0), m_mapped(false) 
  { } 
  

  void reset();

  void addModules(std::vector<RegSelModule>& modules);

  void setupDisabledModules();

  void getModules(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const;
  void getModules(const RegSelRoI& roi, unsigned layer, std::vector<const RegSelModule*>& modules) const;
  void getModules(unsigned layer, std::vector<const RegSelModule*>& modules) const;


  int ID() const      { return m_ID; } 
  int Nlayers() const { return m_Nlayers; }  


  void drawlayers(std::vector<const ZRObject*>& c) const { 
    for ( unsigned i=0 ; i<m_Nlayers ; i++ )   c.push_back(&m_layer[i]);
  }

  const std::vector<RegSelLayer>& getLayers() const { return m_layer; }

  void disableLayer(unsigned layer);
  void enableLayer(unsigned layer);

protected:

  int   m_ID; 

  unsigned                     m_Nlayers;
  std::vector<RegSelLayer>     m_layer;

  std::map<unsigned, unsigned> m_layermap;
  bool                         m_mapped;
};





#endif  /* __REGSELSUBDETECTOR_H */










