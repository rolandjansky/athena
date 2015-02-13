/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define ProgressBar_cxx
#include "MuonCalibrationFit/ProgressBar.h"

ProgressBar::ProgressBar( TString title, Long_t events, Int_t color, Int_t totalblocks ) {
  m_title = title;
  m_events = events;
  m_percentage = -1;
  m_blocks = 0;
  m_totalblocks = totalblocks;
  InitColor( color );
}

void ProgressBar::Update( Long_t entry ) {
  Int_t new_percentage = int( entry * 100 / m_events );
  if( new_percentage > m_percentage ) {
    m_percentage = new_percentage;
    if( NeedNewBlock( entry ) ) m_blocks++; 
    UpdatePrintOut();
  }
  if( entry == ( m_events - 1 ) ) {
    m_blocks++;
    m_percentage = 100;
    UpdatePrintOut();
    cout << endl;
  }
  return;
}

void ProgressBar::UpdatePrintOut( Bool_t colorPer ) {
  TString block_string = m_startCol; 
  block_string += string( m_blocks, '|' );
  block_string += m_endCol;
  block_string += string( m_totalblocks - m_blocks, '|' ); 
  cout << m_title << " :  " << setw( m_totalblocks ) << block_string << "  " << ( ( colorPer ) ? m_startCol : "" ) << m_percentage << " %\r" << ( ( colorPer ) ? m_endCol : "" ) << flush; 
}

Bool_t ProgressBar::NeedNewBlock( Long_t entry ) {
  Float_t next_block = Float_t( m_blocks + 1 ) / Float_t( m_totalblocks );  
  Float_t where = Float_t( entry + 1 ) / Float_t( m_events );  
  if( where < next_block ) return false;
  else return true; 
}

void ProgressBar::InitColor( Int_t color ) {
  if( color == Red ) {
    m_startCol = "\033[1;31m";
  }
  else if( color == Blue ) {
    m_startCol = "\033[1;34m";
  }
  else if( color == Green ) {
    m_startCol = "\033[1;32m";
  }
  else if( color == Yellow ) {
    m_startCol = "\033[1;33m";
  }
  else cout << "Error! Failed color initialization!" << endl;
  m_endCol = "\033[0m";
}
