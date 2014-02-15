/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelSubDetector.cxx        
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Wed Apr  4 16:20:07 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#include "RegSelLUT/RegSelSubDetector.h"

#include <iostream>
#include <set>




void RegSelSubDetector::getModules(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const { 
  for ( unsigned i=m_layer.size() ; i-- ;  ) {
    if ( m_layer[i].inRoI(roi) )   m_layer[i].getModules(roi, modules);
  } 
}



void RegSelSubDetector::getModules(const RegSelRoI& roi, unsigned layer, std::vector<const RegSelModule*>& modules) const { 

  // if mapping between logical and physical layers is needed... 
  if ( m_mapped ) { 
    std::map<unsigned, unsigned>::const_iterator litr = m_layermap.find(layer);
    if ( litr==m_layermap.end() ) return;
    else layer = litr->second;
  }

  if ( layer<m_layer.size() ) {    
    if ( m_layer[layer].inRoI(roi) )  m_layer[layer].getModules(roi, modules);
  } 

}




void RegSelSubDetector::getModules( unsigned layer, std::vector<const RegSelModule*>& modules) const { 

  // if mapping between logical and physical layers is needed... 
  if ( m_mapped ) { 
    std::map<unsigned, unsigned>::const_iterator litr = m_layermap.find(layer);
    if ( litr==m_layermap.end() ) return;
    else layer = litr->second;
  }

  if ( layer<m_layer.size() ) {    
    m_layer[layer].getModules(modules);
  } 

}



// setup the disabled modules list for this subdetector

void RegSelSubDetector::setupDisabledModules() {
  std::vector<RegSelLayer>::iterator litr(m_layer.begin());
  std::vector<RegSelLayer>::iterator lend(m_layer.end());    
  while ( litr!=lend ) { 
    (*litr++).setupDisabledModuleList();    
  }
} 


void RegSelSubDetector::reset() { 
  for ( unsigned i=0 ; i<m_Nlayers ; i++ ) m_layer[i].reset();
}


void RegSelSubDetector::addModules(std::vector<RegSelModule>& modules)
{ 
  // barrel
  // make the layer vector have the correct number of layers
  
  // calculate the number of layers, and get the map of logical
  // layer number to physical layer number
  
  std::set<unsigned> layerset;

  std::vector<RegSelModule>::iterator mptr(modules.begin()); 
  
  for ( ; mptr!=modules.end() ; mptr++ ) { 
    int layer    = mptr->layer();
    int detector = mptr->detector();
    
    if ( !mptr->enabled() ) continue; 
    
    if ( detector == ID() ) layerset.insert(layer);
  }

  // if no modules, don't need to do anything...

  m_Nlayers = layerset.size();

  if ( m_Nlayers==0 ) return;

  // set up the layer map, the variable m_mapped will be used
  // to know whether to use logical, or physical layer numbers 
  // in subsequent lookups

  std::set<unsigned>::iterator litr = layerset.begin();
  std::set<unsigned>::iterator lend = layerset.end();

  m_mapped = false;
  for ( unsigned i=0 ; litr!=lend ; litr++, i++ ) { 
    //    std::cout << "\t\tRegSelSubDetector::addModules() layer " << *litr << " -> " << i << std::endl;
    if ( *litr!=i ) m_mapped = true; 
    m_layermap.insert( std::map<unsigned, unsigned>::value_type( *litr, i ) );
  }
  
  // now actually create and set up the layers for this subdetector

  //  std::cout << "\t\tRegSelSubDetector::addModules() created with  " << m_Nlayers << " layers\tmapped " << m_mapped << std::endl; 

  m_layer.resize(m_Nlayers);

  for ( mptr=modules.begin() ; mptr!=modules.end() ; mptr++ ) { 
    
    int detector = mptr->detector();
    
    if ( detector == ID() ) { 
      
      if ( !mptr->enabled() ) continue; 

      if ( !m_mapped )  m_layer[mptr->layer()].addModule(*mptr);
      else {
	std::map<unsigned, unsigned>::iterator litr = m_layermap.find(mptr->layer());
	// if ( litr!=m_layermap.end() ) 
	m_layer[litr->second].addModule(*mptr);
	// else std::cerr << "module - layer mismatch " << *mptr << std::endl;
      }
            
    }
  }

  //  std::cout << "RegSelSubDetector::addModules() detector extent" << std::endl;
  
  for ( unsigned i=0 ; i<m_Nlayers ; i++ ) { 
    m_layer[i].ID(i);
    
    if ( i==0 ) { 
      rMin(m_layer[i].rMin());
      rMax(m_layer[i].rMax());
      
      zMin(m_layer[i].zMin());
      zMax(m_layer[i].zMax());
    }
    else { 
      if ( m_layer[i].rMin()<rMin() ) rMin(m_layer[i].rMin());
      if ( m_layer[i].rMax()>rMax() ) rMax(m_layer[i].rMax());
      
      if ( m_layer[i].zMin()<zMin() ) zMin(m_layer[i].zMin());
      if ( m_layer[i].zMax()>zMax() ) zMax(m_layer[i].zMax());
    }

    //    std::cout << m_layer[i] << std::endl;
    m_layer[i].createMaps();

  }

  //  std::cout << "Detector " << getName()  
  //       << "\tr=" << rMin() << " - " << rMax()  
  //       << ",\tz=" << zMin() << " - " << zMax()  
  //       << std::endl;

} 



// disabling and enabling an entire layer

void RegSelSubDetector::disableLayer(unsigned layer) { 
  if ( !m_mapped ) { 
    if ( layer<m_Nlayers ) m_layer[layer].disable();
  }
  else { 
    std::map<unsigned, unsigned>::iterator litr = m_layermap.find(layer);
    if ( litr!=m_layermap.end() ) m_layer[litr->second].disable();
  }
}

void RegSelSubDetector::enableLayer(unsigned layer) { 
  if ( !m_mapped ) { 
    if ( layer<m_Nlayers ) m_layer[layer].enable();
  }
  else { 
    std::map<unsigned, unsigned>::iterator litr = m_layermap.find(layer);
    if ( litr!=m_layermap.end() ) m_layer[litr->second].enable();
  }
}

