/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCASDOut.cc
                                      QCJP, 1999
*/
// ====================================================================

#include <iostream>
#include <iomanip>

#include "GaudiKernel/SystemOfUnits.h"
#include "TrigT1TGC/TGCASDOut.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
//      constants and globals
//
// ====================================================================

const char* const strsig[3]= { "N/A", "WireGroup", "Strip" };

// ====================================================================
//
// class description
//
// ====================================================================


///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(TGCIndex tgcindex, int ilyr,
		     TGCSignalType sigtype, int id, double tof)
  : m_tgcReadoutIndex(tgcindex, ilyr),
    m_signalType(sigtype), m_hitID(id), m_hitToF(tof)
///////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(TGCReadoutIndex tgcrindex, 
		     TGCSignalType sigtype, int id, double tof)
  : m_tgcReadoutIndex(tgcrindex),
    m_signalType(sigtype), m_hitID(id), m_hitToF(tof)
///////////////////////////////////////////////////////////////
{
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
            << ", tof=" << std::setw(5) << std::setprecision(1) << m_hitToF/Gaudi::Units::ns << "ns"
            << std::setprecision(6) << std::endl;
}


} //end of namespace bracket
