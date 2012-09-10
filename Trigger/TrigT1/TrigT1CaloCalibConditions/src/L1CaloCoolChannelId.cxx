/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <sstream>
#include <iostream>

/*!
 *  \class L1CaloCoolChannelId
 *  Encapsulates the ID of one channel of conditions data in COOL,
 *  ie the ID of a row in a table.
 *
 *  At present, COOL can only handle integer IDs, so we encode
 *  crate, module type, module in crate, submodule, and channel
 *  with module or submodule into a 32 bit quantity as follows:
 *  - bits 31-24 crate number (0-7=PP, 8-11=CP, 12-13=JEP, 14-15=ROD)
 *  - bits 23-20 module type (1=PPM, 2=CPM, 3=JEM, 4=CMM, 5=TCM,
 *                            6=ROD, 7=CAM)
 *  - bits 19-16 module number in crate
 *  - bits 15-08 submodule number in module (if appropriate)
 *  - bits 08-00 channel number in (sub)module (if appropriate)
 *
 *  This class is a very close adaptation of the online coolL1Calo/L1CaloCoolChannelId class
 */

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Create a L1CaloCoolChannelId from the integer ID field.
 */
L1CaloCoolChannelId::L1CaloCoolChannelId()
  : m_id(0)
{
}

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Create a L1CaloCoolChannelId from its component fields.
 */
L1CaloCoolChannelId::L1CaloCoolChannelId(unsigned int crate,
                             L1CaloModuleType mType,
                             unsigned int module,
                             unsigned int submodule,
                             unsigned int channel,
                             bool logical)
  : m_id(0)
{
	if(logical) this->setId(crate, mType, module, submodule, channel);
	else {
		unsigned int moduleIndex = this->physicalToLogicalModule(mType, module);
		this->setId(crate, mType, moduleIndex, submodule, channel);
	}
}

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Create a L1CaloCoolChannelId from the integer ID field.
 */
L1CaloCoolChannelId::L1CaloCoolChannelId(unsigned int id)
  : m_id(id)
{
}

//==============================================================================
// copy constructor
//==============================================================================
/*!
 *  Copy a L1CaloCoolChannelId.
 */
L1CaloCoolChannelId::L1CaloCoolChannelId(const L1CaloCoolChannelId& id)
  : m_id(id.m_id)
{
}

//==============================================================================
// setId
//==============================================================================
/*!
 *  Set the CoolChannelId value from its component fields.
 */
void
L1CaloCoolChannelId::setId(unsigned int crate,
                     L1CaloModuleType mType,
                     unsigned int module,
                     unsigned int submodule,
                     unsigned int channel)
{
  m_id = (crate & 0xff)     << 24 | (module & 0x0f) << 16 |
         (submodule & 0xff) <<  8 | (channel & 0xff);
  // We only have a limited number of module types that will
  // provide conditions data, so convert module types here.
  // Ignore any other module type.

  m_id |= ((unsigned int)mType & 0x0f) << 20;

//  if      (mType == L1CaloModuleType::Ppm)       { m_id |= 0x00100000; }
//  else if (mType == L1CaloModuleType::Cpm)       { m_id |= 0x00200000; }
//  else if (mType == L1CaloModuleType::Jem)       { m_id |= 0x00300000; }
//  else if (mType == L1CaloModuleType::Cmm)       { m_id |= 0x00400000; }
//  else if (mType == L1CaloModuleType::Tcm)       { m_id |= 0x00500000; }
//  else if (mType == L1CaloModuleType::L1CaloRod) { m_id |= 0x00600000; }
//  else if (mType == L1CaloModuleType::Cam)       { m_id |= 0x00700000; }
}

//==============================================================================
// getModuleType
//==============================================================================
/*!
 *  Return module type of this channel ID.
 *  \return L1CaloModuleType
 */
L1CaloModuleType
L1CaloCoolChannelId::moduleType() const
{
  unsigned int mTypeId = (m_id & 0x00f00000) >> 20;
  return L1CaloModuleType((L1CaloModuleType::L1CaloModuleEnum)mTypeId);
}

///*!
// *  Convert a typeId to a L1CaloModuleType
// *  \return L1CaloModuleType
// */
//L1CaloModuleType
//L1CaloCoolChannelId::moduleType(unsigned int mTypeId) const
//{
//  L1CaloModuleType mType = L1CaloModuleType::None;
//  if      (mTypeId == 1) { mType = L1CaloModuleType::Ppm; }
//  else if (mTypeId == 2) { mType = L1CaloModuleType::Cpm; }
//  else if (mTypeId == 3) { mType = L1CaloModuleType::Jem; }
//  else if (mTypeId == 4) { mType = L1CaloModuleType::Cmm; }
//  else if (mTypeId == 5) { mType = L1CaloModuleType::Tcm; }
//  else if (mTypeId == 6) { mType = L1CaloModuleType::L1CaloRod; }
//  else if (mTypeId == 7) { mType = L1CaloModuleType::Cam; }
//  return mType;
//}
//
///*!
// *  Convert a L1CaloModuleType to a typeId
// *  \return unsigned int
// */
//unsigned int
//L1CaloCoolChannelId::moduleType(L1CaloModuleType mType) const
//{
//  unsigned int mTypeId = 0;
//  if      (mType == L1CaloModuleType::Ppm) { mTypeId = 1; }
//  else if (mType == L1CaloModuleType::Cpm) { mTypeId = 2; }
//  else if (mType == L1CaloModuleType::Jem) { mTypeId = 3; }
//  else if (mType == L1CaloModuleType::Cmm) { mTypeId = 4; }
//  else if (mType == L1CaloModuleType::Tcm) { mTypeId = 5; }
//  else if (mType == L1CaloModuleType::L1CaloRod) { mTypeId = 6; }
//  else if (mType == L1CaloModuleType::Cam) { mTypeId = 7; }
//  return mTypeId;
//}

unsigned int L1CaloCoolChannelId::module(bool logical) const {
	unsigned int logicalIndex = (m_id & 0x000f0000) >> 16;
	if(logical) {
		return logicalIndex;
	} else {
		return this->logicalToPhysicalModule(this->moduleType(), logicalIndex);
	}
}

unsigned int L1CaloCoolChannelId::logicalToPhysicalModule(L1CaloModuleType mType, unsigned int module) const {
	if      (mType == L1CaloModuleType::Ppm) { return module + 5; } //Ppm
	else if (mType == L1CaloModuleType::Cpm) { return module + 5; } //Cpm
	else if (mType == L1CaloModuleType::Jem) { return module + 4; } //Jem
	else {
		std::cout<<"The physical position of module type: "<<mType.toString()<<" is not known. Returning the logical index instead !"<<std::endl;
		return module;
	}
}

unsigned int L1CaloCoolChannelId::physicalToLogicalModule(L1CaloModuleType mType, unsigned int module) const {
	if      (mType == L1CaloModuleType::Ppm) { return module - 5; } //Ppm
	else if (mType == L1CaloModuleType::Cpm) { return module - 5; } //Cpm
	else if (mType == L1CaloModuleType::Jem) { return module - 4; } //Jem
	else {
		std::cout<<"The physical position of module type: "<<mType.toString()<<" is not known. Returning the logical index instead !"<<std::endl;
		return module;
	}
}


//==============================================================================
// operator=
//==============================================================================
/*!
 *  Copy a L1CaloCoolChannelId.
 */
L1CaloCoolChannelId&
L1CaloCoolChannelId::operator=(const L1CaloCoolChannelId& id)
{
  if (this != &id) m_id = id.m_id;
  return *this;
}

bool L1CaloCoolChannelId::operator<(const L1CaloCoolChannelId& id) const {
	return m_id<id.m_id;
}

bool L1CaloCoolChannelId::operator==(const L1CaloCoolChannelId& id) const {
	return m_id==id.m_id;
}

std::ostream& operator<<(std::ostream& output, const  L1CaloCoolChannelId& r) {
	output << "coolChannelId: 0x" << std::hex <<r.id() << std::dec <<"("<<r.id()<<"), crate: "<<r.crate()<<", module: "<<r.module()<<", submodule: "<<r.subModule()<<", channel: "<<r.channel();
    return output;
}
