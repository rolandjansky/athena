/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  : tgcReadoutIndex(),
    signalType(WIRE), hitID(0), channel(0), hitToF(0.0)
//////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(TGCIndex tgcindex, int ilyr,
		     TGCSignalType sigtype, int id, double tof)
  : tgcReadoutIndex(tgcindex, ilyr),
    signalType(sigtype), hitID(id), channel(0), hitToF(tof)
///////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////
TGCASDOut::TGCASDOut(TGCReadoutIndex tgcrindex, 
		     TGCSignalType sigtype, int id, double tof)
  : tgcReadoutIndex(tgcrindex),
    signalType(sigtype), hitID(id), channel(0), hitToF(tof)
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
    tgcReadoutIndex= right.tgcReadoutIndex;
    signalType= right.signalType;
    hitID= right.hitID;
    channel= right.channel;
    hitToF= right.hitToF;
  }
  return *this;
}



////////////////////////////////////////////////////////////////////////
void TGCASDOut::SetParams(TGCSignalType signal_type, int id, double tof)
////////////////////////////////////////////////////////////////////////
{
  signalType= signal_type;
  hitID= id;
  hitToF= tof;
}


/////////////////////////////
void TGCASDOut::Print() const
/////////////////////////////
{
  tgcReadoutIndex.Print(); 
  std::cout << "::" << std::setw(9) << strsig[signalType] 
            << ":: ID=" << std::setw(3) << hitID
            << ", tof=" << std::setw(5) << std::setprecision(1) << hitToF/CLHEP::ns << "ns"
            << std::setprecision(6) << std::endl;
}


} //end of namespace bracket
