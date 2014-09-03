/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBEvent/TBMWPC.h"

/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

TBMWPC::TBMWPC()
  : TBBeamDetector(),
    m_isX(false)
{
}

TBMWPC::TBMWPC(const std::string& thisBPCName)
  : TBBeamDetector(thisBPCName),
    m_isX(false)
{

}

TBMWPC::~TBMWPC()
{ }

