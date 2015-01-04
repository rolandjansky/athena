/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/BunchDataUtil.h"
//#include "CoolKernel/IObject.h"                                              
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include <sstream>
#include <iostream>
#include <cmath>

BunchDataUtil::BunchDataUtil() {
  // Clear vectors                                                                     
  this->clear();
  error.clear();
}

void
BunchDataUtil::clear() {
  m_bunch1Data.clear();
  m_bunch2Data.clear();
}
// Access functions                                                                    
unsigned int
BunchDataUtil::nBunch1Data() const {
  return m_bunch1Data.size();
}
unsigned int
BunchDataUtil::nBunch2Data() const {
  return m_bunch2Data.size();
}
const std::vector<unsigned int> &
BunchDataUtil::bunch1Data() const {
  return m_bunch1Data;
}
const std::vector<unsigned int> &
BunchDataUtil::bunch2Data() const {
  return m_bunch2Data;
}

int
BunchDataUtil::nx1Storage(const cool::Record& rec) {
  return nx1Storage(rec.attributeList());
}

int
BunchDataUtil::ny1Storage(const cool::Record& rec) {
  return ny1Storage(rec.attributeList());
}

int
BunchDataUtil::nx2Storage(const cool::Record& rec) {
  return nx2Storage(rec.attributeList());
}

int
BunchDataUtil::ny2Storage(const cool::Record& rec) {
  return ny2Storage(rec.attributeList());
}

int
BunchDataUtil::nx1Storage(const coral::AttributeList& attrList1) const {

  const coral::Blob &blobBC = attrList1["B1BunchIntensities"].data<coral::Blob>();
  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  int x = 1;
  if (blobBC.size() == 0) {
    return -1;
  }
  unsigned int tmk = *k;
  x=tmk/10;
  return x;

}

int
BunchDataUtil::ny1Storage(const coral::AttributeList& attrList1) const {
  const coral::Blob &blobBC = attrList1["B1BunchIntensities"].data<coral::Blob>();
  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  int x = 1;
  int y = 1;
  if (blobBC.size() == 0) {
    return -1;
  }
  unsigned int tmk = *k;
  x=tmk/10;
  y=tmk-10*x;
  return y;
}

int
BunchDataUtil::nx2Storage(const coral::AttributeList& attrList1) const {
  const coral::Blob &blobBC = attrList1["B2BunchIntensities"].data<coral::Blob>();
  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  int x = 1;
  if (blobBC.size() == 0) {
    return -1;
  }
  unsigned int tmk = *k;
  x=tmk/10;
  return x;
}

int
BunchDataUtil::ny2Storage(const coral::AttributeList& attrList1) const {
  const coral::Blob &blobBC = attrList1["B2BunchIntensities"].data<coral::Blob>();
  const uint8_t *k=static_cast<const uint8_t*>(blobBC.startingAddress());
  int x = 1;
  int y = 1;
  if (blobBC.size() == 0) {
    return -1;
  }
  unsigned int tmk = *k;
  x=tmk/10;
  y=tmk-10*x;
  return y;
}

// Fill value from cool::Record (ie. python)                                           
bool
BunchDataUtil::setValue(const cool::Record& rec) {
  return setValue(rec.attributeList());
}

bool
BunchDataUtil::setCollidingBCID1s(std::vector<unsigned int> & p) {
  m_collidingBCID1s = p;
  return true;
}

bool
BunchDataUtil::setCollidingBCID2s(std::vector<unsigned int> & pp) {
  m_collidingBCID2s = pp;
  return true;
}

// Fill value from AttributeList                                                       
// Returns false on error                                                              
bool
BunchDataUtil::setValue(const coral::AttributeList& attrList1) {
  // First, clear old values                                                           
  this->clear();
  error.clear();

  // Check if there is any data                                                        
  if (attrList1["B1BunchIntensities"].isNull()) {
    error = "BunchCode is NULL!";
    return false;
  }

  cool::Float AI1 = attrList1["B1BunchAverage"].data<cool::Float>();
  cool::Float AI2 = attrList1["B2BunchAverage"].data<cool::Float>();

  const coral::Blob &blobBC1 = attrList1["B1BunchIntensities"].data<coral::Blob>();
  const coral::Blob &blobBC2 = attrList1["B2BunchIntensities"].data<coral::Blob>();


  std::cout << "Dette skulle vaere beam1 " << std::endl;
  m_blobUtil.unpack(AI1,blobBC1,m_collidingBCID1s);

  std::cout << "This should be beam 2 " << std::endl;
  m_blobUtil.unpack(AI2,blobBC2,m_collidingBCID2s);

  return true;
}
  /*
  if (blobBC2.size() == 0 && blobBC1.size() == 0) {
    std::cout << "0" << std::endl;
    return false;
  }
  
  // Pointer to blob to allow unpacking from binary                                    
  const uint8_t *k=static_cast<const uint8_t*>(blobBC1.startingAddress());

  unsigned int fact = 0;
  unsigned int x = 1;
  unsigned int y = 1;
  unsigned int BCID=0;
  unsigned int BCIDold=0;
  float BV = 0;
  float element = 0;
  float AB1 = 0;
  float AB2 = 0;

  if (blobBC1.size() == 0) {
    std::cout << "0" << std::endl;
    return false;
  }
  
  else {

    unsigned int tmk = *k;
    x=tmk/10;
    y=tmk-10*x;
    fact=int(pow(100,x));

    k++;
    //    std::cout << "tmk= " << tmk << std::endl;
    // std::cout << "x= " << x << std::endl;
    // std::cout << "y= " << y << std::endl;
    // std::cout << "Size " << blobBC1.size() << std::endl; 
    if (x==1) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCID1s.size(); i++) {
          BCID = m_collidingBCID1s[i];

          const uint8_t* y10 = (const uint8_t*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y10++) {
            if (j==BCID) {
              tmk = *y10;
              BV = AI1*tmk/fact;
              m_bunch1Data.push_back(BV);
	      AB1 += BV;
	    }
            else {
              m_bunch1Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch1Data.push_back(element);
        }
	AB1 = AB1/m_collidingBCID1s.size();
	std::cout << AB1 << std::endl;
      }      //This ends y=0         
      if (y==1) {
	for (unsigned int i = 0; i < ((blobBC1.size()-1)/x); i++, k++) {
	  tmk = *k;
	  BV = AI1*tmk/fact;                                                             
	  m_bunch1Data.push_back(BV);
	  AB1 +=  BV;
	}
	AB1 = AB1/m_bunch1Data.size();
	std::cout << AB1 << std::endl;
      }  // this ends y=1 
   
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
              BV = AI1*tmk/fact;
              m_bunch1Data.push_back(BV);
              AB1 +=  BV;
            }
            else {
	      m_bunch1Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=(BCIDold); i<3564; i++) {
          m_bunch1Data.push_back(element);
        }
        BCIDold=0;
	AB1 = AB1/m_bunch1Data.size();
	std::cout << AB1 << std::endl;
      } // This ends y=2
    } //This ends x=1

    if (x==2) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCID1s.size(); i++) {
          BCID = m_collidingBCID1s[i];

          const uint16_t* y20 = (const uint16_t*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y20++) {
            if (j==BCID) {
              tmk = *y20;
              BV = AI1*tmk/fact;
	      AB1 += BV;
              m_bunch1Data.push_back(BV);
            }
            else {
              m_bunch1Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch1Data.push_back(element);
        }
	AB1 = AB1/m_collidingBCID1s.size();
	std::cout << AB1 << std::endl;
      }  //This ends y=0                                                                                

      if (y==1) {
        const uint16_t* y21 = (const uint16_t*) k;
	for (unsigned int i=0; i < (blobBC1.size()-1); i++, y21++) {
	  tmk = *y21;
	  BV = AI1*tmk/fact;
	  m_bunch1Data.push_back(BV);
	  AB1 += BV;
	}
	AB1 = AB1/m_bunch1Data.size();
	std::cout << AB1 << std::endl;
      }
      if (y==2) {
	const uint16_t* k2 = (const uint16_t*) k;
	unsigned int len = *k2;
	k2++;
	const uint16_t* y22 = (const uint16_t*) k;

	for (unsigned int i = 0; i<len; i++, k2++) {
	  BCID = *k2;
	  for (unsigned int j = BCIDold; j <= BCID; j++, y22++) {
	    if (j==BCID) {
	      tmk=*y22;
	      BV=AI1*tmk/fact;
	      m_bunch1Data.push_back(BV);
	      AB1 += BV;
	    }
	    else {
	      m_bunch1Data.push_back(element);
	    }
	  }
	  BCIDold = BCID+1;
	}
	for (unsigned int i=BCIDold; i<3564; i++) {
	  m_bunch1Data.push_back(element);
	}
	AB1 = AB1/m_bunch1Data.size();
	std::cout << AB1 << std::endl;
      } //This ends y=2
    } //This ends x=2
    
    if (x==4) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCID1s.size(); i++) {
          BCID = m_collidingBCID1s[i];

          const float * y40 = (const float*) k;
          for (unsigned int j = BCIDold; j <= BCID; j++, y40++) {
            if (j==BCID) {
              BV = *y40;
              m_bunch1Data.push_back(BV);
	      AB1 += BV;
            }
            else {
              m_bunch1Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch1Data.push_back(element);
        }
	AB1 = AB1/m_bunch1Data.size();
	std::cout << AB1 << std::endl;
      }    //This ends y=0    
      if (y==1) {
	const float* y41 = (const float*) k;
        for (unsigned int i = 0; i < ((blobBC1.size()-1)/x); i++, y41++) {
          BV = *y41;
          m_bunch1Data.push_back(BV);
	  AB1 += BV;
        }
	AB1 = AB1/m_collidingBCID1s.size();
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
              m_bunch1Data.push_back(BV);
	      AB1 += BV;
            }
            else {
              m_bunch1Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=(BCIDold); i<3564; i++) {
          m_bunch1Data.push_back(element);
        }
	AB1 = AB1/m_bunch1Data.size();
	std::cout << AB1 << std::endl;
      }      //This ends y=2 
    } //This ends x=4
}	       


 // Decode Beam2 list //////////////////////////

  const uint8_t *q = static_cast<const uint8_t*>(blobBC2.startingAddress());
  
  if (blobBC2.size() == 0) {
    std::cout << "0" << std::endl;
    return false;
  }
  else {
    unsigned int tmk = *q;
    x=tmk/10;
    y=tmk-10*x;
    fact=int(pow(100,x));
    q++;
    BCID = 0;
    BCIDold = 0;
    if (x==1) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCID2s.size(); i++) {
          BCID = m_collidingBCID2s[i];

          const uint8_t* u10 = (const uint8_t*) q;
          for (unsigned int j = BCIDold; j <= BCID; j++, u10++) {
            if (j==BCID) {
              tmk = *u10;
              BV = AI2*tmk/fact;
              m_bunch2Data.push_back(BV);
              AB2 += BV;
            }
            else {
              m_bunch2Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch2Data.push_back(element);
        }
        AB2 = AB2/m_collidingBCID2s.size();
      }      //This ends y=0                        

      if (y==1) {
	for (unsigned int i = 0; i < (blobBC2.size()-1)/x; i++, q++) {
	  tmk = *q;
	  BV = AI2*tmk/fact;
	  m_bunch2Data.push_back(BV);
	  AB2 +=  BV;
	}
	AB2 = AB2/((blobBC2.size()-1)/x);
      } // This ends y=1

      if (y==2) {
        const uint16_t* k1 = (const uint16_t*) k;
        unsigned int len = *k1;
        k1++;
        const uint8_t* u12 = (const uint8_t*) (k+2*(len+1));
        for (unsigned int i = 0; i<len; i++, k1++) {
          BCID = *k1;
          for (unsigned int j=BCIDold; j<=(BCID); j++, u12++) {
            if (j==BCID) {
              tmk=*u12;
              BV = AI2*tmk/fact;
              m_bunch2Data.push_back(BV);
              AB2 +=  BV;
            }
            else {
              m_bunch2Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=(BCIDold); i<3564; i++) {
          m_bunch2Data.push_back(element);
        }
        BCIDold=0;
        AB2 = AB2/len;
      } // This ends y=2  

    } //This ends x=1
    
    if (x==2) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCID2s.size(); i++) {
          BCID = m_collidingBCID2s[i];

          const uint16_t* u20 = (const uint16_t*) q;
          for (unsigned int j = BCIDold; j <= BCID; j++, u20++) {
            if (j==BCID) {
              tmk = *u20;
              BV = AI2*tmk/fact;
              m_bunch2Data.push_back(BV);
	      AB2 += BV;
            }
            else {
              m_bunch2Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch2Data.push_back(element);
        }
	AB2 = AB2/m_collidingBCID2s.size();
      }  //This ends y=0  
      
      if (y==1) {
	const uint16_t* u21 = (const uint16_t*) q;
     
	for (unsigned int i = 0; i < ((blobBC2.size()-1)/x); i++, u21++) {
	  tmk = *u21;
	  BV = AI2*tmk/fact;
	  m_bunch2Data.push_back(BV);
	  AB2 +=  BV;
	}
	AB2 = AB2/((blobBC2.size()-1)/x);
	std::cout << AB2 << std::endl;
      }
      if (y==2) {
        const uint16_t* k2 = (const uint16_t*) q;
        unsigned int len = *k2;
        k2++;
        const uint16_t* u22 = (const uint16_t*) q;

        for (unsigned int i = 0; i<len; i++, k2++) {
          BCID = *k2;
	  std::cout << "BCIDnew " << BCID << std::endl;
	  std::cout << "BCIDold " << BCIDold << std::endl;
          for (unsigned int j = BCIDold; j <= BCID; j++, u22++) {
            if (j==BCID) {
              tmk=*u22;
              BV=AI2*tmk/fact;
              m_bunch2Data.push_back(BV);
              AB2 += BV;
            }
            else {
              m_bunch2Data.push_back(element);
            }
          }
          BCIDold = BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch2Data.push_back(element);
        }
        AB2 = AB2/len;
      } //This ends y=2                    
    } // This ends x=2

    if (x==4) {
      if (y==0) {
        for (unsigned int i =0; i<m_collidingBCID2s.size(); i++) {
          BCID = m_collidingBCID2s[i];

          const float * u40 = (const float*) q;
          for (unsigned int j = BCIDold; j <= BCID; j++, u40++) {
            if (j==BCID) {
              BV = *u40;
              m_bunch2Data.push_back(BV);
              AB2 += BV;
            }
            else {
              m_bunch2Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=BCIDold; i<3564; i++) {
          m_bunch2Data.push_back(element);
        }
        AB2 = AB2/m_collidingBCID2s.size();
      }    //This ends y=0
      if (y==1) {
	const float* q4 = (const float*) q;
	for (unsigned int i = 0; i < ((blobBC2.size()-1)/x); i++, q4++) {
	  BV = *q4;
	  m_bunch2Data.push_back(BV);
	  AB2 += BV;
	}
	AB2 = AB2/m_bunch2Data.size();
      } //This ends y=1
      if (y==2) {
        const uint16_t* k4 = (const uint16_t*) q;
        unsigned int len = *k4;
        k4++;
        const float* u42 = (const float*) (k+2*(1+len));

        for (unsigned int i = 0; i<len; i++, k4++) {
          BCID = *k4;
          for (unsigned int j=BCIDold; j<=(BCID); j++, u42++) {
            if (j==BCID) {
              BV = *u42;
	      AB2 += BV;
	      m_bunch2Data.push_back(BV);
            }
            else {
              m_bunch2Data.push_back(element);
            }
          }
          BCIDold=BCID+1;
        }
        for (unsigned int i=(BCIDold); i<3564; i++) {
          m_bunch2Data.push_back(element);
        }
	AB2 = AB2/len;
      }//This ends y=2 
    }//This ends x=4
  } // This ends the beam 2 decoding

return true;
}
  */
