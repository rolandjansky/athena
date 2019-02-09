/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARROD_ENCODER_H
#define LARBYTESTREAM_LARROD_ENCODER_H

/** This class provides conversion from LArRawChannel and LArDigit to ROD format. 
   * @author H. Ma
   * @version  0-0-1 , Oct 7, 2002

   * Modified, Jan 02, 2003
       Split from LArROD_Decoder. 

   * Modified, Jul 08 2003 Walter Lampl
       Read ByteStream for LArDigits and LArRawChannel

   * Modified, Jul 17 2003 Remi Lafaye
       Split with LArRODBlockStructure
       - LArROD_Encoder       = Athena world only
       - LArRODBlockStructure = ROD format only
       It might be less CPU efficient
       Althought it will be easier to update

   * Modified, Nov 10 2005 D.O. Damazio
	New LArRodBlockPhysics
		(Bertrand Laforge format).
   */

#include <stdint.h>

#include "GaudiKernel/MsgStream.h"

#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
//#include "LArRawEvent/LArRawChannelCollection.h"

#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "LArRawEvent/LArCalibDigit.h"
#include "LArRawEvent/LArCalibDigitContainer.h"

#include "LArCabling/LArCablingLegacyService.h"
#include "LArByteStream/LArRodBlockStructure.h"
//#include "LArByteStream/LArRodBlockStructure_0.h"
//#include "LArByteStream/LArRodBlockStructure_1.h"
//#include "LArByteStream/LArRodBlockStructure_3.h"
#include "ByteStreamData/RawEvent.h" 
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArIdentifier/LArOnlineID.h"

class ICaloNoiseTool;
class LArRodEncoder
{
public: 

  // constructor 
  LArRodEncoder(); 
  // destructor 
  ~LArRodEncoder(); 

  // add LArRawChannels to the current list 
  void add(const LArRawChannel* rc);
  
  // add LArDigits to the current list
  void add(const LArDigit* digit);
  
  // add LArDigit for fixed gain
  void add(const LArDigit* dg, const int gain);

  //add LArCalibDigit (always fixed gain)
  void add(const LArCalibDigit* dg, const int gain);

  // clear the current  LArRawChannels list
  void clear(); 

  // convert all LArRawChannels in the current list to a vector of 32bit words
//  void fillROD(std::vector<uint32_t>& v, MsgStream& logstr) ; 
  void fillROD(std::vector<uint32_t>& v, MsgStream& logstr, ICaloNoiseTool* calonoisetool, double nsigma) ;

  static void setRodBlockStructure(LArRodBlockStructure* BlStructPtr);

private:
  /*
  int m_NFeb;
  uint32_t *m_FEB_ID;
  std::vector<const LArRawChannel*> **m_vLArRC;
  std::vector<const LArDigit*> **m_vLArDigit;
  
  
  std::vector<uint32_t> m_FEB_ID;
  std::vector<std::vector<const LArRawChannel*>> m_vLArRC;
  std::vector<std::vector<const LArRawChannel*>> m_vLArDigit;
  std::vector<std::vector<const LArRawChannel*>> m_vLArDigitFixed[3];

  //std::vector<std::vector<const LArRawChannel*>> m_vLArDigit2;
  //std::vector<std::vector<const LArRawChannel*>> m_vLArDigit3;
  */

  struct FebData_t {
    //uint32_t FEB_ID;
    std::vector<const LArRawChannel*> vLArRC;
    std::vector<const LArDigit*> vLArDigit;           //Free gain
    std::vector<const LArDigit*> vLArDigitFixed[3];   //Three gains
    std::vector<const LArCalibDigit*> vLArCalibDigit[3];  //Three gains
    //std::vector<const LArAverageDigits* > vLArAverageDigits[3];  //Three gains
  };

  //std::vector<FebData> m_vFEB;
  std::map<uint32_t,FebData_t> m_mFEB;

  //Static members;
  static LArRodBlockStructure* m_BlStruct;
  static LArCablingLegacyService*    m_cablingSvc; 
  static const CaloDetDescrManager* m_CaloDetDescrManager;
  static const LArOnlineID*    m_onlineHelper; 
  /*
 public:
  static int m_digitcounter; //for debug purpose only
  static int m_fixeddigitcounter[4];
  */
} ; 

#endif
