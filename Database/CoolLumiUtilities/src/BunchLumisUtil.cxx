/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/BunchLumisUtil.h"
//#Include "CoolKernel/IObject.h"                                                           
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>

BunchLumisUtil::BunchLumisUtil() {
  // Clear vectors                                                                          
  this->clear();
  error.clear();
}

// The number 3564 is the number of BCID's
void
BunchLumisUtil::clear() {
  m_bunchLumis.clear();
}

// Access functions                                                                         
unsigned int
BunchLumisUtil::nBunchLumis() const {
  return m_bunchLumis.size();
}

int
BunchLumisUtil::nxStorage(const cool::Record& rec) {
  return nxStorage(rec.attributeList());
}

int
BunchLumisUtil::nyStorage(const cool::Record& rec) {
  return nyStorage(rec.attributeList());
}

int
BunchLumisUtil::nxStorage(const coral::AttributeList& attrList1) const {
  return m_blobUtil.xvalue(attrList1);
}

int
BunchLumisUtil::nyStorage (const coral::AttributeList& attrList1) const {
  return m_blobUtil.yvalue(attrList1);
}

const std::vector<double> &
BunchLumisUtil::bunchLumis() const {
  return m_bunchLumis;
}

// Fill value from cool::Record (ie. python)                                                     
bool
BunchLumisUtil::setValue(const cool::Record& rec) {
  return setValue(rec.attributeList());
}

bool 
BunchLumisUtil::setCollidingBCIDs(std::vector<unsigned int> & p) {
  m_collidingBCIDs = p;
  return true;
}

// Fill value from AttributeList        
// Returns false on error                                                                       
bool
BunchLumisUtil::setValue(const coral::AttributeList& attrList1) {  
  // First, clear old values                                                                     
  this->clear();
  error.clear();
  // Check if there is any data                                                                          
  if (attrList1["BunchRawInstLum"].isNull()) {
    error = "BunchCode is NULL!";
    return false;
  }

  // Defines the blob and integer from COOL
  cool::Float ARI = attrList1["AverageRawInstLum"].data<cool::Float>();  
  const coral::Blob &blobBC = attrList1["BunchRawInstLum"].data<coral::Blob>();
  
  //  m_blobUtil.setValue(attrList1);

  m_blobUtil.unpack(ARI,blobBC,m_collidingBCIDs);

  return true;
}
  //  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  /*
  if (blobBC.size() == 0) {
    std::cout << "No Blob" << std::endl;
    return false;
  }
  
  else {     

    unsigned int fact = 0;
    unsigned int x = 1;
    unsigned int y = 1;
    unsigned int BCID = 0;
    unsigned int BCIDold = 0;
    float BV = 0;
    double element = 0;
    unsigned int tmk = *k;
    
    x=tmk/10;
    y=tmk-10*x;
    fact=int(pow(100,x));

    std::cout << "value of x: " << x << std::endl;
    std::cout << "value of y: " << y << std::endl;
    std::cout << "BlobBC.size() " << blobBC.size() << std::endl;
    std::cout << "m_collidingBCIDs.size() " << m_collidingBCIDs.size() << std::endl;
    k++;
    if (x==1) {
      if (y==0) {
	for (unsigned int i =0; i<m_collidingBCIDs.size(); i++) {
	  BCID = m_collidingBCIDs[i];
	  
	  const uint8_t* y10 = (const uint8_t*) k;
	  for (unsigned int j = BCIDold; j <= BCID; j++, y10++) {
	    if (j==BCID) {
	      tmk = *y10;
	      BV = ARI*tmk/fact;
	      m_bunchLumis.push_back(BV);
	    }
	    else {
	      m_bunchLumis.push_back(element);
	    }
	  }
	  BCIDold=BCID+1;
	}
	for (unsigned int i=BCIDold; i<3564; i++) {
	  m_bunchLumis.push_back(element);
	}
      }      //This ends y=0

      if (y==1) {
	const uint8_t* y11 = (const uint8_t*) k;
	for (unsigned int i = 0; i < ((blobBC.size()-1)/x); i++, y11++) {
	  tmk = *y11;
	  BV = ARI*tmk/fact;
	  m_bunchLumis.push_back(BV);
	}
      }
      //This ends y=1
      if (y==2) {
	const uint16_t* k1 = (const uint16_t*) k;
	unsigned int len = *k1;
	k1++;
	const uint8_t* y12 = (const uint8_t*) (k+2*(len+1));

	for (unsigned int i = 0; i<len; i++, k1++) {
	  BCID = *k1;
	  for (unsigned int j=BCIDold; j<=(BCID); j++, y12++) {
	    if (j==BCID) {
	      tmk=*y12;
	      BV = ARI*tmk/fact;
	      m_bunchLumis.push_back(BV);
	    }
	    else {
	    m_bunchLumis.push_back(element);
	    }
	  }
	  BCIDold=BCID+1;
	}
	for (unsigned int i=(BCIDold); i<3564; i++) {
	  m_bunchLumis.push_back(element);
	}
	BCIDold=0;
      }  //This ends y=2
    }    //This ends x=1
    
    if (x==2) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCIDs.size(); i++) {
	  BCID = m_collidingBCIDs[i];

          const uint16_t* y20 = (const uint16_t*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y20++) {
            if (j==BCID) {
              tmk = *y20;
              BV = ARI*tmk/fact;
              m_bunchLumis.push_back(BV);
            }
            else {
              m_bunchLumis.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunchLumis.push_back(element);
        }
      } //This ends y=0
      if (y==1) {
	const uint16_t* y21 = (const uint16_t*) k;
	for (unsigned int i = 0; i < ((blobBC.size()-1)/x); i++, y21++) {
	  tmk=*y21;
	  BV = ARI*tmk/fact;
	  m_bunchLumis.push_back(BV);
	}
      }          //This ends y=1
      
      if (y==2) {
	const uint16_t* k2 = (const uint16_t*) k;
	unsigned int len = *k2;
	k2++;
	
        const uint16_t* y22 = (const uint16_t*) (k+2*(len+1));

        for (unsigned int i = 0; i<len; i++, k2++) {
          BCID = *k2;
	  for (unsigned int j=BCIDold; j<=(BCID); j++, y22++) {
            if (j==BCID) {
	      tmk=*y22;
	      BV = ARI*tmk/fact;
	      m_bunchLumis.push_back(BV);
	    }
	    else {
	    m_bunchLumis.push_back(element);
	    }
	  }
          BCIDold=BCID+1;
        }
        for (unsigned int i=(BCIDold); i<3564; i++) {
          m_bunchLumis.push_back(element);
        }
      }
      //This ends y=2
    }
    //This ends x=2
    if (x==4) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCIDs.size(); i++) {
          BCID = m_collidingBCIDs[i];

          const float * y40 = (const float*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y40++) {
            if (j==BCID) {
              tmk = *y40;
              BV = ARI*tmk/fact;
              m_bunchLumis.push_back(BV);
            }
            else {
              m_bunchLumis.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunchLumis.push_back(element);
        }
      }    //This ends y=0
      if (y==1) {
	const float* y41 = (const float*) k;
	for (unsigned int i = 0; i < ((blobBC.size()-1)/x); i++, y41++) {
	  BV = *y41;
	  m_bunchLumis.push_back(BV);
	}
      }    //This ends y=1      
      if (y==2) {
	const uint16_t* k4 = (const uint16_t*) k;
	unsigned int len = *k4;
	k4++;
	const float* y42 = (const float*) (k+2*(1+len));

	for (unsigned int i = 0; i<len; i++, k4++) {
          BCID = *k4;
          for (unsigned int j=BCIDold; j<=(BCID); j++, y42++) {
            if (j==BCID) {
              BV = *y42;
              m_bunchLumis.push_back(BV);
            }
            else {
	      m_bunchLumis.push_back(element);
            }  
	  }
	  BCIDold=BCID+1;
	}
	for (unsigned int i=(BCIDold); i<3564; i++) {
	  m_bunchLumis.push_back(element);
	}
      }      //This ends y=2
    }        //This ends x=4

  
    return true;
}

  */
