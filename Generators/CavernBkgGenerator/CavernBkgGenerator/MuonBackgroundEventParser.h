/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBACKGROUNDEVENTPARSER_H
#define MUONBACKGROUNDEVENTPARSER_H

#include <iostream>
#include <iomanip>
#include <string>
#include "CLHEP/Vector/LorentzVector.h"

class MuonBackgroundEventParser 
{
  public:
    
  MuonBackgroundEventParser ( void );
    ~MuonBackgroundEventParser( void ) {};
    
    const CLHEP::HepLorentzVector&  getVertex  ( void ) { return m_vertex; }
    const CLHEP::HepLorentzVector&  getMomentum( void ) { return m_momentum; }    
    
    int getPdgID      ( void ) { return m_pdgId; }
    int getEventNumber( void ) { return m_eventNumber; }

    const std::string getString( void ) { return m_string; }
  
  
  private:
    
    CLHEP::HepLorentzVector m_vertex, m_momentum;
    int m_eventNumber, m_pdgId;
    std::string m_string;

    int    convertG3ToPdg( int g3Id ) const;
    double getMass( int g3Id )        const;
    
    std::istream& read ( std::istream& is );
    std::ostream& write( std::ostream& os ) const;
    
    friend std::istream& operator >> ( std::istream& is, MuonBackgroundEventParser& evt );
    friend std::ostream& operator << ( std::ostream& os, const MuonBackgroundEventParser& evt );


    int m_pdgIdTable[50];
    double m_mass[50];

};


#endif
