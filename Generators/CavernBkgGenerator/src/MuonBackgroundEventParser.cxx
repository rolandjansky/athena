/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: MuonBackgroundConverter/MuonBackgroundEventParser.cxx
// Description:
//
//    Helper class to read cavern-background events from an ASCII file
// 
// Author: A. Nairz, December 28, 2004
//         Based on work by W. Seligman, M. Shapiro, I. Hinchliffe;
//         S. Bentvelsen, N. van Eldik 
//
// Modifications:
// 
#include "CavernBkgGenerator/MuonBackgroundEventParser.h"

#include <iostream>
#include <iomanip>
#include <string>
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "HepPDT/ParticleData.hh"


std::istream& operator >> ( std::istream& is, MuonBackgroundEventParser& evt ) 
{
  return evt.read( is );
}


std::ostream& operator << ( std::ostream& os, const MuonBackgroundEventParser& evt ) 
{
  return evt.write( os );
}

MuonBackgroundEventParser::MuonBackgroundEventParser ( void ) {
  m_eventNumber = 0; m_pdgId=0; m_string="";
  for( int i=0 ;i< 50 ; i++)
    m_pdgIdTable[i] =  0 ;
  m_pdgIdTable[1]  =    22;  // photon 
  m_pdgIdTable[2]  =   -11;  // positron
  m_pdgIdTable[3]  =    11;  // electron
  m_pdgIdTable[5]  =   -13;  // mu+
  m_pdgIdTable[6]  =    13;  // mu-
  m_pdgIdTable[7]  =   111;  // pi0
  m_pdgIdTable[8]  =   211;  // pi+
  m_pdgIdTable[9]  =  -211;  // pi-
  m_pdgIdTable[10] =   130;  // K0_L
  m_pdgIdTable[13] =  2112;  // neutron
  m_pdgIdTable[14] =  2212;  // proton
  m_pdgIdTable[25] = -2112;  // anti-neutron


  for( int i=0 ;i< 50 ; i++)
    m_mass[i]  = 0.0;
  m_mass[1]  = 0.0;             // photon 
  m_mass[2]  = 5.10998902e-01;  // positron
  m_mass[3]  = 5.10998902e-01;  // electron
  m_mass[5]  = 1.05658357e+02;  // mu+
  m_mass[6]  = 1.05658357e+02;  // mu-
  m_mass[7]  = 1.349766e+02;    // pi0
  m_mass[8]  = 1.3957018e+02;   // pi+
  m_mass[9]  = 1.3957018e+02;   // pi-
  m_mass[10] = 4.97672e+02;     // K0_L
  m_mass[13] = 9.3956533e+02;   // neutron
  m_mass[14] = 9.3827200e+02;   // proton
  m_mass[25] = 9.3956533e+02;   // anti-neutron

}

std::istream& MuonBackgroundEventParser::read( std::istream& is )
{
  int id;
  CLHEP::Hep3Vector vert;
  double time, px1, py1, pz1, p;
  
  is >> m_string;
  if ( m_string == "ievnt,ITYPE,TOF,VECT:" ) 
  {
    is >> m_eventNumber >> id >> time >> vert >> px1 >> py1 >> pz1 >> p;

    //
    // Convert G3 particle ID to PDG one; determine particle m_mass
    //
    m_pdgId     = convertG3ToPdg( id );
    double mass = getMass( id );

    //
    // Fill vertex 4-vector 
    //
    m_vertex.setVect( vert * CLHEP::cm );
    m_vertex.setT( time );

    //
    // Calculate and fill momentum 4-vector
    //
    m_momentum.setPx( p * px1 * CLHEP::GeV );
    m_momentum.setPy( p * py1 * CLHEP::GeV );
    m_momentum.setPz( p * pz1 * CLHEP::GeV );
  
    CLHEP::Hep3Vector mom = m_momentum.vect();
    m_momentum.setE( std::sqrt( mass*mass + mom.mag2() ) );
  }
  
  return is;
}


std::ostream& MuonBackgroundEventParser::write( std::ostream& os ) const
{
  if ( m_string == "ievnt,ITYPE,TOF,VECT:" ) 
  {
    os << std::setw(6) << m_eventNumber << std::setw(6) << m_pdgId  
       << std::setw(16) << std::setprecision(6) << std::setiosflags( std::ios::scientific | std::ios::showpoint ) 
       << m_vertex.t() 
       << std::resetiosflags( std::ios::scientific | std::ios::showpoint ) 
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_vertex.x() 
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_vertex.y()  
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_vertex.z() 
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_momentum.x() 
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_momentum.y() 
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_momentum.z()
       << std::setiosflags( std::ios::fixed | std::ios::showpoint ) << std::setw(13) << std::setprecision(4) 
       << m_momentum.t();
  }
  else 
  {
    // std::cout << "MuonBackgroundEventParser:  m_string = " << m_string << std::endl;
    os << "MuonBackgroundEventParser:  End of Event";
  }
   
  return os;
}


int MuonBackgroundEventParser::convertG3ToPdg( int g3Id ) const
{
  if( g3Id < 0 || g3Id > 49)
    return 0;
  
  return m_pdgIdTable[ g3Id ];
}  


double MuonBackgroundEventParser::getMass( int g3Id ) const
{
  if( g3Id < 0 || g3Id > 49)
    return 0.0;

  
  return m_mass[ g3Id ];
}  
