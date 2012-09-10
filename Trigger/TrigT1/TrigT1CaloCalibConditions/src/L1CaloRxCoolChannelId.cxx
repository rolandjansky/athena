/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

#include <sstream>
#include <iostream>

/*!
 *  \class L1CaloRxCoolChannelId
 *  Encapsulates the ID of one Rx channel of conditions data in COOL,
 *  ie the ID of a row in a table.
 *
 */

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Create a L1CaloRxCoolChannelId from the integer ID field.
 */
L1CaloRxCoolChannelId::L1CaloRxCoolChannelId()
  : m_id(0),
  m_bOutputIsSet(false),
  m_outputConn(0),
  m_outputPair(0)
{
}

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Create a L1CaloRxCoolChannelId from its component fields.
 */
L1CaloRxCoolChannelId::L1CaloRxCoolChannelId(unsigned int crate,
                             unsigned int module,
                             unsigned int inputConn,
                             unsigned int inputPair)
  : m_id(0),
  m_bOutputIsSet(false),
  m_outputConn(0),
  m_outputPair(0)
{
  this->setId(crate, module, inputConn, inputPair);
}

L1CaloRxCoolChannelId::L1CaloRxCoolChannelId(unsigned int crate, unsigned int module, unsigned int inputConn, unsigned int inputPair, unsigned int outputConn, unsigned int outputPair): m_id(0),
  m_bOutputIsSet(false),
  m_outputConn(0),
  m_outputPair(0)
{
  this->setId(crate, module, inputConn, inputPair);
  this->outputAttributes(outputConn, outputPair);
}

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Create a L1CaloRxCoolChannelId from the integer ID field.
 */
L1CaloRxCoolChannelId::L1CaloRxCoolChannelId(unsigned int id)
  : m_id(id),
  m_bOutputIsSet(false),
  m_outputConn(0),
  m_outputPair(0)
{
}

//==============================================================================
// copy constructor
//==============================================================================
/*!
 *  Copy a L1CaloRxCoolChannelId.
 */
L1CaloRxCoolChannelId::L1CaloRxCoolChannelId(const L1CaloRxCoolChannelId& id)
  : m_id(id.m_id),
  m_bOutputIsSet(id.m_bOutputIsSet),
  m_outputConn(id.m_outputConn),
  m_outputPair(id.m_outputPair)
{
}

//==============================================================================
// setId
//==============================================================================
/*!
 *  Set the CoolChannelId value from its component fields.
 */
void
L1CaloRxCoolChannelId::setId(unsigned int crate,
                     unsigned int module,
                     unsigned int inputConn,
                     unsigned int inputPair)
{

  m_id = inputPair + inputConn*16 + module * 64 + crate*1024;
}


//==============================================================================
// operator=
//==============================================================================
/*!
 *  Copy a L1CaloRxCoolChannelId.
 */
L1CaloRxCoolChannelId&
L1CaloRxCoolChannelId::operator=(const L1CaloRxCoolChannelId& id)
{
  if (this != &id) m_id = id.m_id;
  return *this;
}

bool L1CaloRxCoolChannelId::operator<(const L1CaloRxCoolChannelId& id) const {
	return m_id<id.m_id;
}

bool L1CaloRxCoolChannelId::operator==(const L1CaloRxCoolChannelId& id) const {
	return m_id==id.m_id;
}


void L1CaloRxCoolChannelId::outputAttributes(unsigned int conn, unsigned int pair) {
	if(m_bOutputIsSet) {
		std::cout<<"Output attributes are already set for this channel: "<<this->id()<<". They cannot be modified"<<std::endl;
		return;
	}
	m_outputConn = conn;
	m_outputPair = pair;
	m_bOutputIsSet = true;
}

unsigned int L1CaloRxCoolChannelId::outputConn() const {
	if(!m_bOutputIsSet) {
		std::cout<<"outputConn has not been set for channel "<< this->id() <<". You should not call this method."<<std::endl;
	}
	return m_outputConn;
}

unsigned int L1CaloRxCoolChannelId::outputPair() const {
	if(!m_bOutputIsSet) {
		std::cout<<"outputPair has not been set for channel "<< this->id() <<". You should not call this method."<<std::endl;
	}
	return m_outputPair;
}

std::ostream& operator<<(std::ostream& output, const  L1CaloRxCoolChannelId& r) {
	output << "rxChannelId: 0x" << std::hex <<r.id() << std::dec <<", crate: "<<r.crate()<<", module: "<<r.module()<<", inputConn: "<<r.inputConn()<<", inputPair: "<<r.inputPair()<<" ( outputConn: "<< r.outputConn()<<", outputPair: "<<r.outputPair()<<")";
    return output;
}
