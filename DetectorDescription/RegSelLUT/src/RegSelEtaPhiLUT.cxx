// emacs: this is -*- c++ -*-
//
//   @file    RegSelEtaPhiLUT.h        
//
//            a look up table for eta-phi space divided up into 
//            a set of elements Neta x Nphi.
//
//            "Modules" are added to the lookup table, and are 
//            stored in each element that they overlap with
//
//            To accelerate the look up when there are a large 
//            number of modules, only elements that overlap with 
//            the roi are examined. 
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

#include "RegSelLUT/RegSelEtaPhiLUT.h"



/// constructor the number of elements in the internal eta-phi
/// element storage
RegSelEtaPhiLUT::RegSelEtaPhiLUT(int Neta, int Nphi) : 
  m_etamin(-3), m_etamax(3),      m_Neta(Neta),  
  m_phimin(0),  m_phimax(2*M_PI), m_Nphi(Nphi), 
  m_grandmap( m_etamin, m_etamax, m_phimin, m_phimax,  
	      std::vector< std::vector< map_element > >() )
{
  /// inverse element sizes  
  m_ideta = m_Neta/(m_etamax-m_etamin);    
  m_idphi = m_Nphi/(m_phimax-m_phimin);
    
  std::vector< std::vector< map_element > >& tmap = m_grandmap.payload();
  
  //  std::cout << "RegSelEtaPhiUT::RegSelEtaPhiLUT() Neta " << Neta << "\tNphi " << Nphi << std::endl;

  moduleset empty_set;
  
  for ( int i=0 ; i<m_Neta ; i++ ) { 
    
    double tetamin = m_etamin+i/m_ideta;
    double tetamax = m_etamin+(i+1)/m_ideta;
    
      std::vector< map_element > row;
      
      for ( int j=0 ; j<m_Nphi ; j++ ) { 
	double tphimin = m_phimin+j/m_idphi;
	double tphimax = m_phimin+(j+1)/m_idphi;
	
	map_element m( tetamin, tetamax, tphimin, tphimax, empty_set );
	
	row.push_back( m ); 
	
      }
      
      tmap.push_back( row ); 
      
  }
    
}



/// destructor 
RegSelEtaPhiLUT::~RegSelEtaPhiLUT() { } 




/// add an element, returns true if element added. Could return the number of 
/// eta-phi regions element was added to
bool RegSelEtaPhiLUT::addModule( EtaPhiModule& m ) { 

  //    std::cout << "adding... " << m << std::endl;

  static std::vector< std::vector< map_element > >& tmap = m_grandmap.payload();
  
  int first_eta = (m.etamin()-m_etamin)*m_ideta;
  int last_eta  = (m.etamax()-m_etamin)*m_ideta;
  
  int first_phi = (m.phimin()-m_phimin)*m_idphi;
  int last_phi  = (m.phimax()-m_phimin)*m_idphi;
  
  /// first check if it overlaps with look up table at all
  if ( !m_grandmap.overlap( m ) ) return false;
  
  /// only do overlapping bits in eta
  if ( first_eta<0 )      first_eta = 0;
  if ( last_eta>=m_Neta ) last_eta  = m_Neta-1;
  
  /// wrap in phi
  if ( first_phi<0 )       first_phi += m_Nphi;
  if ( first_phi>=m_Nphi ) first_phi -= m_Nphi;
  
  if ( last_phi<0 )       last_phi += m_Nphi;
  if ( last_phi>=m_Nphi ) last_phi -= m_Nphi;
  
  
  /// does it overlap phi boundary? 
  if ( first_phi<last_phi ) { 
    for ( int i=int(first_eta) ; i<=last_eta ; i++ ) { 
      for ( int j=int(first_phi) ; j<=last_phi ; j++ ) tmap[i][j].payload().insert( m );
    }
  }
  else { 
    for ( int i=int(first_eta) ; i<=last_eta ; i++ ) { 
      for ( int j=0 ; j<=int(last_phi) ; j++ ) tmap[i][j].payload().insert( m );
      for ( int j=first_phi ; j<m_Nphi ; j++ ) tmap[i][j].payload().insert( m );
    }
  }
  
  //    std::cout << "added" << std::endl;
  
  return true;
}




/// get back a vector of the payload for each of the eta-phi 
/// segments that overlap with this roi
void RegSelEtaPhiLUT::getModules( const RegSelRoI& roi, 
				  std::vector<IdentifierHash>& hashids, 
				  double r ) const {
    
  hashids.clear();
  
  std::set<EtaPhiModule> modules;
  
  getElements( roi, modules, r );
  
  hashids.reserve( modules.size() );
  
  std::set<EtaPhiModule>::iterator rpitr=modules.begin();
  std::set<EtaPhiModule>::iterator rpend=modules.end();
  
  while ( rpitr!=rpend ) { 
    hashids.push_back( rpitr->payload() );
    rpitr++;
  }
  
} 




  
/// add an element, returns the virtual eta-phi roi corresponding to the 
/// eta phi region of the roi 
/// NB: Could return the number of eta-phi modules found rather than the virtual roi

const EtaPhiBase RegSelEtaPhiLUT::getElements( const RegSelRoI& roi, 
					       std::set<EtaPhiModule>& modules, 
					       double idradius ) const { 

    modules.clear();

    //   struct timeval timer = simpletimer_start();

    ///  create a virtual roi for the input roi 
    const EtaPhiBase virtual_roi( roi.etaMinLimit(idradius), roi.etaMaxLimit(idradius), 
				  roi.getphiMin(),           roi.getphiMax() ); 

    //    double time = simpletimer_stop( timer );

    /// check roi overlaps with the lookup table 
    bool inmap = m_grandmap.overlap( virtual_roi );

    if ( !inmap ) return virtual_roi;

    int first_phi = (virtual_roi.phimin()-m_phimin)*m_idphi;
    int last_phi  = (virtual_roi.phimax()-m_phimin)*m_idphi;

    int first_eta = (virtual_roi.etamin()-m_etamin)*m_ideta;
    int last_eta  = (virtual_roi.etamax()-m_etamin)*m_ideta;

    /// don't have to cover the first_eta>=m_Neta case, etc as that is 
    /// dealt with by the overlap test earlier
    if ( first_eta<0 )      first_eta = 0;
    if ( last_eta>=m_Neta ) last_eta = m_Neta-1;

    /// wrap in phi
    if ( first_phi<0 )       first_phi += m_Nphi;
    if ( first_phi>=m_Nphi ) first_phi -= m_Nphi;

    if ( last_phi<0 )       last_phi += m_Nphi;
    if ( last_phi>=m_Nphi ) last_phi -= m_Nphi;

    static const std::vector< std::vector< map_element > >& tmap = map();

    /// does the roi overlap the phi boundary? 
    if ( first_phi<last_phi ) { 
      for ( int i=first_eta ; i<=last_eta ; i++ ) { 
	getRowElements( virtual_roi, tmap[i], first_phi, last_phi, modules );
      }
    }
    else { 
      for ( int i=first_eta ; i<=last_eta ; i++ ) { 
	getRowElements( virtual_roi, tmap[i], 0,        last_phi, modules );
	getRowElements( virtual_roi, tmap[i], first_phi, m_Nphi-1,  modules );
      }
    }
    
    return virtual_roi;
}
  







