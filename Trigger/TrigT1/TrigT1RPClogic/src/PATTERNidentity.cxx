/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/PATTERNidentity.h" 

using namespace std;

const char PATTERNidentity::s_inputTag[4][9] = 
    { {"Low Pt"},{"Pivot"},{"High Pt"},{"No input"} };


PATTERNidentity::PATTERNidentity(CMAinput io,int layer) : 
    m_input_id(io),m_layer_id(layer) {}
 
PATTERNidentity::PATTERNidentity(const PATTERNidentity& id)
{
    m_input_id = id.input_id();
    m_layer_id = id.layer_id();
}

PATTERNidentity
PATTERNidentity::operator=(const PATTERNidentity& id)
{
    m_input_id = id.input_id();
    m_layer_id = id.layer_id();
    return *this;
}

bool 
PATTERNidentity::operator ==(const PATTERNidentity& id) const
{
    if(m_input_id == id.input_id() && m_layer_id == id.layer_id()) return true;
    return false;
}
    
bool 
PATTERNidentity::operator !=(const PATTERNidentity& id) const
{
    return !(*this == id);
}
    
bool 
PATTERNidentity::operator < (const PATTERNidentity& id) const
{
    if( m_input_id < id.input_id() && m_layer_id < id.layer_id() ) return true;
    return false;
}

void PATTERNidentity::Print(ostream& stream,bool detail) const
{
    detail = true;
    if(detail)
    {
        stream << s_inputTag[input_id()] << ",  layer " << layer_id();
    }
}
