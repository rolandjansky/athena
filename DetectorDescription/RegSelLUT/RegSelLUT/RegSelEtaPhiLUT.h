// emacs: this is -*- c++ -*-
//
//   @file    RegSelEtaPhiLUT.h        
//
//            a look up table for eta-phi space divided up into 
//            a set of elements Neta x Nphi.
//
//            "Modules" are added to the lookup table, and are 
//            stored in each element that they overlap with, so 
//            that all the modules overlapping an internal element 
//            for a tower of modules (actually stored as a set)
//            as the payload for the element.  
//
//            Because RoIs are defined in terms of a z range, 
//            without these z positions, eta coordinates are 
//            not well defined.
//            As a result, when passed an RoI, this class calculates
//            a virtual RoI, just in eta-phi space, where the eta
//            range corresponds to the maxuimum and minimum eta
//            possible from any z position with the RoI where the
//            object can be completely contained within a user 
//            specified radius. The default value is 600mm, chosen
//            to completely contain the outer radius of the SCT       
//  
//                
//   Copyright (C) 2011 M.Sutton (sutt@cern.ch)    
//
//   $Id: RegSelEtaPhiLUT.h, v0.0   Sat 25 Jun 2011 18:47:25 BST sutt $


#ifndef REGSELETAPHILUT_H
#define REGSELETAPHILUT_H

#include <iostream>
#include <vector>
#include <set>

#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/RegSelEtaPhiModule.h"
#include "Identifier/IdentifierHash.h"



class RegSelEtaPhiLUT {

public:

  /// Concrete data type
  typedef TRegSelEtaPhiModule<IdentifierHash>  EtaPhiModule;
  
  /// tower of the actual modules assigned to each element of the 
  /// lookup table
  typedef std::set<EtaPhiModule>               moduleset;

  /// element type from which the look up table is built
  typedef TRegSelEtaPhiModule<moduleset>       map_element;  
    
public:

  /// defines the number of elements in the internal eta-phi
  /// element storage
  /// NB: these numbers of internal elements, 3, 32, have been optimised for 0.2 x 0.2 rois 
  ///     with 32 x 32 modules
  RegSelEtaPhiLUT(int Neta=3, int Nphi=32);
  
  /// destructor 
  virtual ~RegSelEtaPhiLUT();


  /// add an element, returns true if element added. Could return the number of 
  /// eta-phi regions element was added to
  bool addModule( EtaPhiModule& m );


  /// get back a vector of the payload for each of the eta-phi 
  /// segments that overlap with this roi
  void getModules( const RegSelRoI& roi, 
		   std::vector<IdentifierHash>& hashids, 
		   double r=600 ) const;


  /// access the actual complete map - shouldn't be public really,
  /// is only so for the ostream streamer  
  const std::vector< std::vector< map_element > >& map() const { return m_grandmap.payload(); };

  
  /// get the modules from the elements, returns the virtual eta-phi roi corresponding to the 
  /// virtual eta - phi region of the roi 
  /// NB: Could return the number of eta-phi modules found rather than the virtual roi
  const EtaPhiBase getElements( const RegSelRoI& roi, std::set<EtaPhiModule>& modules, double idradius=600 ) const;

private:
  
  /// get the modules from the elements in the phi direction, called for each eta slice 
  /// processed
  void getRowElements( const EtaPhiBase& virtual_roi, 
		       const std::vector< map_element >& row, 
		       int first_phi, int last_phi, 
		       std::set<EtaPhiModule>& modules ) const { 
    
    for ( int j=first_phi ; j<=last_phi ; j++ ) { 
      
      /// here, need to iterate over all the modules in the region and test whether they overlap with the roi
      const map_element& tower = row[j];
      
      moduleset::const_iterator rpitr=tower.payload().begin();
      moduleset::const_iterator rpend=tower.payload().end();

      while ( rpitr!=rpend ) { 
	if ( virtual_roi.overlap( *rpitr ) ) modules.insert( *rpitr );
	rpitr++;
      }      
    }
  }


private:

  /// eta and phi ranges and number of internal book keeping 
  /// elements in eta and phi directions
  double m_etamin;
  double m_etamax;
  
  int    m_Neta;

  /// inverse eta element size  
  double m_ideta;
  
  double m_phimin;
  double m_phimax;
  
  int    m_Nphi;
  
  /// inverse phi element size
  double m_idphi;

  /// overall map, can get the limits from the object, can have different numbers and 
  /// different ranges for different phi ranges - should swap eta and phi round probably
  TRegSelEtaPhiModule< std::vector< std::vector< map_element > >  > m_grandmap;  

};



inline std::ostream& operator<<( std::ostream& s, const RegSelEtaPhiLUT& f ) { 

  for ( unsigned i=0 ; i<f.map().size() ; i++ ) { 

    const std::vector< RegSelEtaPhiLUT::map_element >& row = f.map()[i];

    s << "row " << i << std::endl;
    
    for ( unsigned j=0 ; j<row.size() ; j++ ) { 
      
      const RegSelEtaPhiLUT::map_element& tower = row[j];
      
      s << "\t" << j << "\t" << (EtaPhiBase)tower << " \t:";

      RegSelEtaPhiLUT::moduleset::const_iterator rpitr=tower.payload().begin();
      RegSelEtaPhiLUT::moduleset::const_iterator rpend=tower.payload().end();

      while ( rpitr!=rpend ) { 
	s << "\t" << (EtaPhiBase)(*rpitr); 
       	rpitr++;
      }
      
      s << std::endl;
    }

    s << std::endl;

  }

  return s;
}

#endif  // __REGSELETAPHILUT_H 










