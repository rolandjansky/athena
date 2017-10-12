/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of MdtROD_Encoder class 

#include "MdtAmtReadOut.h"
#include "MdtCsmReadOut.h"
#include "MdtRODReadOut.h"
#include "MdtROD_Encoder.h" 

#include "MDT_Hid2RESrcID.h"

#include <algorithm> 
#include <math.h> 
#include <map>

/** constructor 
*/ 

MdtROD_Encoder::MdtROD_Encoder() {

}

/** destructor 
*/ 
MdtROD_Encoder::~MdtROD_Encoder() {

}


// Add a CSM 
void MdtROD_Encoder::add(const MdtCsm* csm) 
{
  m_vMdtCsm.push_back(csm);
  return;
}

// Clear the vector of CSMs
void MdtROD_Encoder::clear()
{
  m_vMdtCsm.erase(m_vMdtCsm.begin(),m_vMdtCsm.end());
  return;
}

/** convert all MDT CSMs in the current list to 
  a vector of 32bit words
*/ 

void MdtROD_Encoder::fillROD(std::vector<uint32_t>& v)
{
  
  //  IMessageSvc* msgSvc = 0;
  //ISvcLocator* svcLocator = Gaudi::svcLocator();
  //StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
  //if (sc==StatusCode::FAILURE)  
  //   throw std::runtime_error( "Unable to get MessageSvc");
  //MsgStream log(msgSvc, "MdtROD_Encoder::fillROD");   
  
  MdtAmtReadOut * amtReadOut = new MdtAmtReadOut(); 
  MdtCsmReadOut * csmReadOut = new MdtCsmReadOut(); 
  MdtRODReadOut * rodReadOut = new MdtRODReadOut(); 
  
  uint32_t mrod_wcnt = 0;
  
  std::vector<const MdtCsm*>::const_iterator it     = m_vMdtCsm.begin();
  std::vector<const MdtCsm*>::const_iterator it_end = m_vMdtCsm.end();
  
  // Get the rodId from the first csm
  //uint16_t subdetId = (*it)->SubDetId();
  //uint16_t mrodId = (*it)->MrodId();
  
  // Make the ROD Header
  //v.push_back(rodReadOut->makeHeaderMarker());
  //v.push_back(rodReadOut->makeHeaderSize());
  //v.push_back(rodReadOut->makeFormatVersion());
  
  //v.push_back(rodReadOut->makeRODId(subdetId, mrodId));   // Source Id
  
  //v.push_back(0);   // Level1ID
  //v.push_back(0);   // Bunch crossing ID
  //v.push_back(0);   // Level1 Trigger Type
  //v.push_back(0);   // Detector event type
  
  //mrod_wcnt += rodReadOut->makeHeaderSize();
  
  typedef std::vector<const MdtAmtHit*>        hit_vector;
  typedef std::map<uint16_t, hit_vector> tdc_map;
  typedef std::map<uint16_t, uint32_t>   masked_map;

  tdc_map    tdcMap;
  masked_map maskedMap;
  
  // Insert the Beginning of Block Word (Lvl1ID not set yet)
  uint32_t lvl1Id = 0;
  v.push_back(csmReadOut->makeBOB(lvl1Id));
  
  // make the body of the ROD
  // Loop on the CSMs
  for ( ; it != it_end ; ++it) {
    const MdtCsm * csm = (*it);
    
    uint16_t ctwc = 0;     // Trailer word count initialized
    
    v.push_back(csmReadOut->makeLWC(0));   // Link Word Count 
    v.push_back(csmReadOut->makeBOL(csm->MrodId(), csm->CsmId())); // Beginning of link
    
    v.push_back(csmReadOut->makeTLP(0));
    ++ctwc;    // Trailer word count starts from TLP

    // Group the hits of this CSM according to the TDC number    
    tdcMap.clear();   
    maskedMap.clear();
    MdtCsm::const_iterator it_amt = csm->begin();
    for ( ; it_amt != csm->end() ; ++it_amt) {
      const MdtAmtHit * amt = (*it_amt);
      uint16_t tdcNum = amt->tdcId();

      if (!amt->isMasked()) {
	tdcMap[tdcNum].push_back(amt);
      }
      else {
	maskedMap[tdcNum] = maskedMap[tdcNum] | (int) pow(2,amt->channelId());
      }
    }

    // Loop on the TDCs and build the corresponding MROD blocks
    uint16_t jt = 0;
    tdc_map::const_iterator it_tdc = tdcMap.begin();
    for ( ; it_tdc != tdcMap.end() ; ++it_tdc ) {
      
      // Beginning of TDC header word
      // Need to fix event counter and bcid
      v.push_back(amtReadOut->makeBOT((*it_tdc).first , 0 , 0 )); 
      ++ctwc;

      uint32_t maskedFlags = maskedMap[(*it_tdc).first];
      uint16_t wcnt = 0;     // TDC word count

      // Masked channels flags
      if (maskedFlags != 0) {
	v.push_back(amtReadOut->makeTMC(jt, maskedFlags));
	++ctwc;
	++wcnt;
      }

      // Loop on the hits for this TDC
      hit_vector::const_iterator it_amtvec = (*it_tdc).second.begin();
      for ( ; it_amtvec != (*it_tdc).second.end() ; ++it_amtvec) {

	uint16_t chan = (*it_amtvec)->channelId();
	// bool leading = (*it_amtvec)->leading();
	//bool leading = true;
	//bool errflag = false;

	uint16_t coarse = (*it_amtvec)->coarse();
	uint16_t fine   = (*it_amtvec)->fine();
	uint16_t width  = (*it_amtvec)->width();

	// Add a "Single Measurement" word
	// v.push_back(amtReadOut->makeTSM(jt, chan, leading, errflag, coarse, fine)); 

	// Add a combined measurement data word
	v.push_back(amtReadOut->makeTCM(jt, chan, width, coarse, fine)); 

	++ctwc;  // CSM word count
	++wcnt;  // TDC word count
      }
      
      uint16_t jt   = 0;
      uint16_t ecnt = 0;    // Event counter
      // End of TDC trailer
      v.push_back(amtReadOut->makeEOT(jt,ecnt,wcnt+2));
      ++ctwc;
    }    
    
    // Last word of each csm: TWC trailer word count
    uint16_t ecnt = 0;
    v.push_back(csmReadOut->makeTWC(ecnt, ctwc));

    // Update the MROD word count
    mrod_wcnt += ctwc;
    mrod_wcnt += 2;
    
  }
  
  // Close the MROD: MROD word count in EOB
  v.push_back(csmReadOut->makeEOB(mrod_wcnt));
  
  // Status words not fully defined yet ... wait
  
  //  log << MSG::DEBUG << "************** Encoder dumping the words ************" << endmsg;
  //if (v.size() > 0) {
  //  log << MSG::DEBUG << "The size of the ROD-Write is: " << v.size() << endmsg;
  //  for (unsigned int i=0 ; i<v.size() ; ++i) {
  //    log << MSG::DEBUG << "word " << i << " = " << MSG::hex << v[i] << MSG::dec << endmsg;
  //  }
  //}
  
  // cleanup
  delete amtReadOut;
  delete csmReadOut;
  delete rodReadOut;
  
  return; 
}

