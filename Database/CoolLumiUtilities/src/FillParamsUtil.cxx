/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/FillParamsUtil.h"
//#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include <sstream>
#include <iostream>

FillParamsUtil::FillParamsUtil() {
  // Clear vectors
  this->clear();
  error.clear();
}

void
FillParamsUtil::clear() {
  m_beam1Bunches.clear();
  m_beam2Bunches.clear();
  m_luminousBunches.clear();
}


// Access functions
unsigned int
FillParamsUtil::nBeam1Bunches() const {
  return m_beam1Bunches.size();
}

unsigned int
FillParamsUtil::nBeam2Bunches() const {
  return m_beam2Bunches.size();
}

unsigned int
FillParamsUtil::nLuminousBunches() const {
  return m_luminousBunches.size();
}

/////////////////////////////

const std::vector<unsigned int> &
FillParamsUtil::beam1Bunches() const {
  return m_beam1Bunches;
}

const std::vector<unsigned int> &
FillParamsUtil::beam2Bunches() const {
  return m_beam2Bunches;
}

const std::vector<unsigned int> &
FillParamsUtil::luminousBunches() const {
  return m_luminousBunches;
}

/*
const std::vector<unsigned int> &
FillParamsUtil::bcidmasks() const{
  return m_bcidmasks;
}
*/

// Fill value from cool::Record (ie. python)
bool
FillParamsUtil::setValue(const cool::Record& rec) {
  return setValue(rec.attributeList());
}

// Fill value from AttributeList 
// Returns false on error
bool 
FillParamsUtil::setValue(const coral::AttributeList& attrList) {

  // First, clear old values
  this->clear();
  error.clear();

  // Check if there is any data
  if (attrList["BCIDmasks"].isNull()) {
    error = "BCIDmasks is NULL!";
    return false;
  }

  // Extract raw data from COOL object
  cool::UInt32 nb1 = attrList["Beam1Bunches"].data<cool::UInt32>();
  cool::UInt32 nb2 = attrList["Beam2Bunches"].data<cool::UInt32>();
  cool::UInt32 ncol = attrList["LuminousBunches"].data<cool::UInt32>();

  const coral::Blob& blob = attrList["BCIDmasks"].data<coral::Blob>();

  // Verify length
  if ( static_cast<cool::UInt32>( blob.size() ) != 2 * (nb1 + nb2 + ncol)) {
    std::stringstream ss;
    ss << "BCIDmasks length " << blob.size() << " != 2 * " << (nb1+nb2+ncol) << "!";
    error = ss.str();
    return false;
  }
  //  std::cout << "  Size of BCIDmask: " << blob.size() << std::endl;

  // Pointer to blob to allow unpacking from binary
  const uint16_t* p=static_cast<const uint16_t*>(blob.startingAddress());

  // Decode beam1 list
  for (unsigned int i = 0; i < nb1; i++, p++) {
    m_beam1Bunches.push_back(*p);
    // std::cout << "nBeam1 is  " << *p << std::endl;
    if (i==nb1) {
      std::cout << "####################" << std::endl;
	}
  }

  // Decode beam2 list
  for (unsigned int i = 0; i < nb2; i++, p++) {
    // unsigned int tmp = *p;
    //    std::cout << "#2 is " << tmp << std::endl;
    m_beam2Bunches.push_back(*p);
    if (i==nb2){
      std::cout << "####################" << std::endl;
    }
  }

  
  // Decode luminous list
  for (unsigned int i = 0; i < ncol; i++, p++) {
    //unsigned int tmp = *p;
    //    std::cout << "Luminous bunch is   " << tmp << std::endl;
    m_luminousBunches.push_back(*p);
    if (i==ncol){
      std::cout << "####################" << std::endl;
    }
  }


  for (unsigned int i = (nb1+nb2+ncol); i < blob.size(); i++, p++) {
    //unsigned int tmp = *p;
    //  std::cout << "BCIDmask is    " << tmp  << std::endl;
  }
  // Success!
  return true;
}
