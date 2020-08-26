// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoCoreSim/InputConnector.h"
#include "L1TopoCommon/Exception.h"

using namespace std;
using namespace TCS;

InputConnector::InputConnector(const string & name) :
    Connector(name, INPUT)
{
   m_inputTOBType = inputType(name);
}

InputConnector::~InputConnector()
{}


TCS::StatusCode
TCS::InputConnector::clearOutput() {
   m_outputData = 0;
   return StatusCode::SUCCESS;
}


// attaches the input data to the connector (does not take ownership)
void
TCS::InputConnector::attachOutputData(const TCS::InputTOBArray* data) {
   if(m_outputData != 0) {
      TCS_EXCEPTION("Trying to attach data to input connector " << name() << " which has already data attached")
   }
   m_outputData = data;
}


namespace TCS {


ostream &
operator<<(ostream &o, const TCS::InputConnector & conn) {
   o << conn.name();
   return o;
}


}
