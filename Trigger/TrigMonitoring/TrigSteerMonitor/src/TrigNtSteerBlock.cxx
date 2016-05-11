/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigNtSteerBlock.cxx
//
// DESCRIPTION: Fills an ntuple with Steering Monitoring info.
//
// AUTHOR:   Manuel Diaz-Gomez
//
//
// ********************************************************************

#include <algorithm>
#include <iostream>
//#include <strstream>
#include "TrigSteerMonitor/TrigNtSteerBlock.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/INTupleSvc.h"

/*---------------------------------------------------------------*/
StatusCode TrigNtSteerBlock::book( NTuple::Tuple* nt1)
/*---------------------------------------------------------------*/
{
  StatusCode sc;

  sc = nt1->addItem ("NSeq",m_nSeq, 1, 21);
  sc = nt1->addItem ("TEinput", m_nSeq, m_TEinput);
  sc = nt1->addItem ("TEoutput", m_nSeq, m_TEoutput);

  m_nSeq = 0;



  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------------*/
StatusCode TrigNtSteerBlock::fill(unsigned int TEinput, unsigned int TEoutput )
/*---------------------------------------------------------------*/
{

  if (m_nSeq < 21) {

    m_TEinput[m_nSeq] = TEinput;
    m_TEoutput[m_nSeq] = TEoutput;

    m_nSeq++;
  }
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------------*/
void TrigNtSteerBlock::reset()
/*---------------------------------------------------------------*/
{
  m_nSeq = 0;
}
