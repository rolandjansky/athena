/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/BunchGroupUtil.h"
//#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include <sstream>
#include <iostream>


BunchGroupUtil::BunchGroupUtil() {
  // Clear vectors
  this->clear();
  error.clear();
}

void
BunchGroupUtil::clear() {
  m_bunchGroup.clear();
}


// Access functions
unsigned int
BunchGroupUtil::nBunchGroup() const {
  return m_bunchGroup.size();
}

const std::vector<unsigned int> &
BunchGroupUtil::bunchGroup() const {
  return m_bunchGroup;
}

// Fill value from cool::Record (ie. python)
bool
BunchGroupUtil::setValue(const cool::Record& rec) {
  return setValue(rec.attributeList());
}

// Fill value from AttributeList 
// Returns false on error
bool 
BunchGroupUtil::setValue(const coral::AttributeList& attrList) {

  // First, clear old values
  this->clear();
  error.clear();

  // Check if there is any data
  if (attrList["BunchCode"].isNull()) {
    error = "BunchCode is NULL!";
    return false;
  }

  const coral::Blob &blobBC = attrList["BunchCode"].data<coral::Blob>(); 
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
  unsigned int c0 = 0;
  unsigned int c1 = 0;
  unsigned int c3 =0;
  unsigned int c5 = 0;
  unsigned int c9 = 0;
  unsigned int c17 = 0;
  unsigned int c33 = 0;
  unsigned int c65 = 0;
  unsigned int c131 = 0;
  unsigned int other = 0;
  // Decode beam1 list
  unsigned int tmp = *p;
  std::cout << "HER" << tmp << std::endl; 
  for (unsigned int i = 0; i < blobBC.size(); i++, p++) {
    tmp = *p;
    // unsigned int tmp = *p;
    //   std::cout << "The Data of BunchGroupContent is   " << tmp << std::endl ;
      std::cout << "BGC    " << tmp  << std::endl;
    if (tmp==131) {
      c131++;
      //      std::cout << " 131   " << i << std::endl;
    }
    else if (tmp==65) {
      c65++;
      //      std::cout <<" 65   " <<  i << std::endl;
    }
    else if (tmp==33) {
      c33++;
      //      std::cout << "33   " << i << std::endl;
    }
    else if (tmp==17) {
      c17++;
      //      std::cout << "17   " << i << std::endl;
    }
    else if (tmp==9) {
      c9++;
      //      std::cout << "9   " << i << std::endl;
    }
    else if (tmp==5) {
      c5++;
      //      std::cout << "5   " << i << std::endl;
    }
    else if (tmp==3) {
      //      std::cout << "3      " << i << std::endl;
      c3++;
    }
    else if (tmp==1) {
      c1++;
      //      std::cout << "1   " << i << std::endl;
    }
    else if (tmp==0) {
      c0++;
    }
    else {
      other++;
    }
    m_bunchGroup.push_back(*p);
  }
  std::cout << "COME ON 1368      " << c131+c3 << std::endl;
  std::cout << "C131 i.e. group 8     " << c131 << std::endl;
  std::cout << "C65 i.e. group 7     " << c65 << std::endl;
  std::cout << "C33 i.e. group 6     " << c33 << std::endl;
  std::cout << "C17 i.e. group 5     " << c17 << std::endl;
  std::cout << "C9 i.e. group 4     " << c9 << std::endl;
  std::cout << "C5 i.e. group 3     " << c5 << std::endl;
  std::cout << "C3 i.e. group 2     " << c3 << std::endl;
  std::cout << "C1 i.e. group 1     " << c1 << std::endl;
  std::cout << "Other i.e. group 0          " << other << std::endl;
  // Success!
  return true;
}


