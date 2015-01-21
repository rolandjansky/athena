/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/LumiBlobUtil.h"
//#Include "CoolKernel/IObject.h"                                                                         
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>

LumiBlobUtil::LumiBlobUtil() {
  // Clear vectors                                                                                       
  this->clear();
  error.clear();
}
// The number 3564 is the number of BCID's                                                                
void
LumiBlobUtil::clear() {
  m_bunchLumis.clear();
}

// Access functions                                                                                       
int
LumiBlobUtil::xvalue(const cool::Record& rec) {
  return xvalue(rec.attributeList());
}

int
LumiBlobUtil::yvalue(const cool::Record& rec) {
  return yvalue(rec.attributeList());
}

int
LumiBlobUtil::xvalue(const coral::AttributeList& attrList1) const {
  const coral::Blob &blobBC = attrList1["BunchRawInstLum"].data<coral::Blob>();
  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  int x = 1;
  if (blobBC.size() == 0) {
    std::cout << "No Blob" << std::endl;
    return -1;
  }
  unsigned int tmk = *k;
  x=tmk/10;
  return x;
}

int
LumiBlobUtil::yvalue(const coral::AttributeList& attrList1) const {

  const coral::Blob &blobBC = attrList1["BunchRawInstLum"].data<coral::Blob>();

  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  int y = 1;
  int x = 1;
  if (blobBC.size() == 0) {
    std::cout << "No Blob" << std::endl;
    return -1;
  }
  unsigned int tmk = *k;
  x=tmk/10;
  y=tmk-10*x;
  return y;
}


const std::vector<double> &
LumiBlobUtil::bunchLumis() const {
  return m_bunchLumis;
}

/*// Fill value from cool::Record (ie. python)                                                             

bool
LumiBlobUtil::setValue(const cool::Record& rec) {
  return setValue(rec.attributeList());
}

bool
LumiBlobUtil::setCollidingBCIDs(std::vector<unsigned int> & p) {
  m_collidingBCIDs = p;
  return true;
}
*/

bool 
LumiBlobUtil::unpack(const cool::Float &ARI, const coral::Blob &blobBC, const std::vector<unsigned int> &PV) {

  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());

  if (blobBC.size() == 0) {
    std::cout << "No Blob" << std::endl;
    return false;
  }

  else {
    unsigned int tmk = *k; //Used to make sure that the value from the blob is a number
    unsigned int fact = 0; //Used in calculation of Stored value when x=1 or x=2
    unsigned int x = 1; //Size of blob values
    unsigned int y = 1; // Storage modes
    unsigned int BCID = 0;//Newest BCID number - used for storing the data when y=0 or y=2
    unsigned int BCIDold = 0; //Last BCID number - used for storing data when y=0 or y=2
    unsigned int len = 0; //calculating the number of luminous bunches when y=2
    float BV = 0; //The actual stored value
    float AB1 = 0; // The avarage value for one LB/timestep
    double element = 0; // This is allways 0, but makes sure it is a double

    x=tmk/10;
    y=tmk-x*10;
    fact=int(pow(100,x));
    /*    
    std::cout << "From unpack " << std::endl;
    std::cout << "value of x: " << x << std::endl;
    std::cout << "value of y: " << y << std::endl;
    std::cout << "BlobBC.size() " << blobBC.size() << std::endl;
    std::cout << "Vector.size " << PV.size() << std::endl;
    */
    k++;

    ////////////////////////////////////////////////////////////                                   
    if (x==1) {
      if (y==0) {
        for (unsigned int i =0; i<PV.size(); i++) {
	  //	  std::cout << i<< std::endl;
          BCID = PV[i];

	  //	  std::cout << "BCID " << BCID << std::endl;
          const uint8_t* y10 = (const uint8_t*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y10++) {
            if (j==BCID) {
              tmk = *y10;
              BV = ARI*tmk/fact;
              m_bunchLumis.push_back(BV);
	      AB1 += BV;
	      //  std::cout << "BV " << BV << std::endl;
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
	AB1 = AB1/PV.size();
	//	std::cout << AB1 << std::endl;
      }      //This ends y=0      
      if (y==1) {
        const uint8_t* y11 = (const uint8_t*) k;
        for (unsigned int i = 0; i < ((blobBC.size()-1)/x); i++, y11++) {
          tmk = *y11;
          BV = ARI*tmk/fact;
          m_bunchLumis.push_back(BV);
          AB1 +=  BV;
        }
	AB1 = AB1/m_bunchLumis.size();
	//	std::cout << AB1 << std::endl;
      } //This ends y=1   
      if (y==2) {
	const uint16_t* k1 = (const uint16_t*) k;
	len = *k1;
	k1++;
	const uint8_t* y12 = (const uint8_t*) (k+2*(len+1));
	for (unsigned int i = 0; i<len; i++, k1++) {
	  BCID = *k1;
	  for (unsigned int j=BCIDold; j<=(BCID); j++, y12++) {
	    if (j==BCID) {
	      tmk=*y12;
	      BV = ARI*tmk/fact;
	      m_bunchLumis.push_back(BV);
	      AB1 +=  BV;
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
      } //This ends y=2
      if (len > 0) AB1 = AB1/len;
      // std::cout << AB1 << std::endl;
    } //This ends x=1    
    if (x==2) {
      if (y==0) {
        for (unsigned int i =0; i<PV.size(); i++) {
          BCID = PV[i];
          const uint16_t* y20 = (const uint16_t*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y20++) {
            if (j==BCID) {
              tmk = *y20;
              BV = ARI*tmk/fact;
              m_bunchLumis.push_back(BV);
	      AB1 += BV;
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
	AB1 = AB1/PV.size();
	//	std::cout << AB1 << std::endl;
      }// This ends y=0  
      if (y==1) {
        const uint16_t* y21 = (const uint16_t*) k;
        for (unsigned int i = 0; i < ((blobBC.size()-1)/x); i++, y21++) {
          tmk=*y21;
          BV = ARI*tmk/fact;
          m_bunchLumis.push_back(BV);
          AB1 +=  BV;
        }
        AB1 = AB1/((blobBC.size()-1)/x);
	//	std::cout << AB1 << std::endl;
      }      //This ends y=1
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
	      AB1 += BV;
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
      } // This ends y=2               
      AB1 = AB1/len;
      //  std::cout << AB1 << std::endl;
    } // This ends x=2     
    if (x==4) {
      if (y==0) {
        for (unsigned int i =0; i<PV.size(); i++) {
          BCID = PV[i];
          const float * y40 = (const float*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y40++) {
            if (j==BCID) {
              tmk = *y40;
              BV = ARI*tmk/fact;
              m_bunchLumis.push_back(BV);
	      AB1 += BV;
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
	AB1 = AB1/PV.size();
	//	std::cout << AB1 << std::endl;
      }        //This ends y=0   
      if (y==1) {
        const float* y41 = (const float*) k;
        for (unsigned int i = 0; i < ((blobBC.size()-1)/x); i++, y41++) {
          BV = *y41;
          m_bunchLumis.push_back(BV);
          AB1 +=  BV;
        }
        AB1 = AB1/((blobBC.size()-1)/x);
	//	std::cout << AB1 << std::endl;
      } //This ends y=1      
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
              AB1 +=  BV;
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
      }//This ends y=2
      if (len > 0) AB1 = AB1/len;
      //  std::cout << AB1 << std::endl;
    } //This ends x=4
                                                                                                   
  } //This ends else       
  return true;
} // This ends setValue
