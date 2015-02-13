/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ProgressBar_h
#define ProgressBar_h

#include <iostream>
#include <iomanip>

#include <TString.h>

using namespace std;

class ProgressBar {
protected:
  TString m_title;
  Long_t  m_events;
  Int_t   m_percentage;
  Int_t   m_blocks;
  Int_t   m_totalblocks;
  TString m_startCol;
  TString m_endCol;
public:
  enum { Red, Blue, Green, Yellow };
  ProgressBar( TString title, Long_t events, Int_t color = Red, Int_t totalblocks = 20 );
  ~ProgressBar() { ; }
  TString GetTitle() { return m_title; }
  Long_t  GetEvents() { return m_events; }
  Bool_t  NeedNewBlock( Long_t entry );
  void    InitColor( Int_t color );
  void    Update( Long_t entry );
  void    UpdatePrintOut( Bool_t colorPer = false );

};

#endif
