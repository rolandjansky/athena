/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/BunchDescription.h"
//#include "CoolKernel/IObject.h"                                                                                   
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include <sstream>
#include <iostream>


BunchDescription::BunchDescription() {
  // Clear vectors                                                                                                  
  this->clear();
  error.clear();
}

void
BunchDescription::clear() {
  m_bunchD.clear();
}


// Access functions                                                                                                 
unsigned int
BunchDescription::nBunchD1() const {
  return m_bunchD.size();
}

const std::vector<unsigned int> &
BunchDescription::bunchD() const {
  return m_bunchD;
}

// Fill value from cool::Record (ie. python)                                                                        
bool
BunchDescription::setValue(const cool::Record& rec) {
  return setValue(rec.attributeList());
}

// Fill value from AttributeList                                                                                    
// Returns false on error                                                                                           
bool
BunchDescription::setValue(const coral::AttributeList& attrList) {

  // First, clear old values                                                                                        
  this->clear();
  error.clear();

  // Check if there is any data                                                                                     
  if (attrList["ItemToBunchGroupMap"].isNull()) {
    error = "BunchCode is NULL!";
    return false;
  }

  const coral::Blob &blobBC = attrList["ItemToBunchGroupMap"].data<coral::Blob>();
  //  std::cout << "size of blob:   " << blobBC.size() << std::endl;                                                

  // Verify length                                                                                                  
  /*  if ( static_cast<cool::UInt32>( blobBC.size() ) != 2 * (nb1 + nb2 + ncol)) {                                  
    std::stringstream ss;                                                                                           
    ss << "BCIDmasks length " << blobBC.size() << " != 2 * " << (nb1+nb2+ncol) << "!";                              
    error = ss.str();                                                                                               
    return false;                                                                                                   
  }                                                                                                                 
  */
  //  for (int i =0; i < ( blobBC.size() ); i++) {                                                                  
  //     std::cout << blobBC.startingAddress();                                                                     
  //   }                                                                                                            



  // Pointer to blob to allow unpacking from binary                                                                 
  const uint8_t *p=static_cast<const uint8_t*>(blobBC.startingAddress());

  // Decode beam1 list                                                                                              
  for (unsigned int i = 0; i < blobBC.size(); i++, p++) {
    //unsigned int tmp = *p;
    //   std::cout << "The Data of BunchGroupContent is   " << tmp << std::endl ;                                   
    // std::cout << "BGC    " << tmp  << std::endl;
    m_bunchD.push_back(*p);
  }

  /*
  std::cout << "COME ON 1368      " << c131+c33 << std::endl;
  std::cout << "C131      " << c131 << std::endl;
  std::cout << "C65      " << c65 << std::endl;
  std::cout << "C33      " << c33 << std::endl;
  std::cout << "C17     " << c17 << std::endl;
  std::cout << "C9      " << c9 << std::endl;
  std::cout << "C5      " << c5 << std::endl;
  std::cout << "C3      " << c3 << std::endl;
  std::cout << "C1      " << c1 << std::endl;
  std::cout << "Other           " << other << std::endl;
  // Success!                                                                                                       */
  return true;
}
