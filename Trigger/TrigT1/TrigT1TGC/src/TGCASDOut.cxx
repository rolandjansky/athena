/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCASDOut.cc
                                      QCJP, 1999
*/
// ====================================================================

#include <iostream>
#include <iomanip>

#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigT1TGC/TGCASDOut.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
//      constants and globals
//
// ====================================================================

static const char* strsig[3]= { "N/A", "WireGroup", "Strip" };

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////
TGCASDOut::TGCASDOut()
  : m_tgcReadoutIndex(),
    m_signalType(WIRE), m_hitID(0), m_channel(0), m_hitToF(0.0)
//////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(TGCIndex tgcindex, int ilyr,
		     TGCSignalType sigtype, int id, double tof)
  : m_tgcReadoutIndex(tgcindex, ilyr),
    m_signalType(sigtype), m_hitID(id), m_channel(0), m_hitToF(tof)
///////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(TGCReadoutIndex tgcrindex, 
		     TGCSignalType sigtype, int id, double tof)
  : m_tgcReadoutIndex(tgcrindex),
    m_signalType(sigtype), m_hitID(id), m_channel(0), m_hitToF(tof)
///////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(const TGCASDOut& right)
/////////////////////////////////////////////////////////////
{
  *this= right;
}


/////////////////////////////////////////////////////////////
TGCASDOut& TGCASDOut::operator=(const TGCASDOut& right)
/////////////////////////////////////////////////////////////
{
  if (this != &right) {
    m_tgcReadoutIndex= right.m_tgcReadoutIndex;
    m_signalType= right.m_signalType;
    m_hitID= right.m_hitID;
    m_channel= right.m_channel;
    m_hitToF= right.m_hitToF;
  }
  return *this;
}



////////////////////////////////////////////////////////////////////////
void TGCASDOut::SetParams(TGCSignalType signal_type, int id, double tof)
////////////////////////////////////////////////////////////////////////
{
  m_signalType= signal_type;
  m_hitID= id;
  m_hitToF= tof;
}


/////////////////////////////
void TGCASDOut::Print() const
/////////////////////////////
{
  m_tgcReadoutIndex.Print(); 
  std::cout << "::" << std::setw(9) << strsig[m_signalType] 
            << ":: ID=" << std::setw(3) << m_hitID
            << ", tof=" << std::setw(5) << std::setprecision(1) << m_hitToF/CLHEP::ns << "ns"
            << std::setprecision(6) << std::endl;
}


} //end of namespace bracket
