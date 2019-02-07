/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSC_HID2RESRCID
#define MUONCSC_CNVTOOLS_CSC_HID2RESRCID

#include <inttypes.h> 
#include "CSCcabling/CSCcablingSvc.h"

class Identifier;
class CscIdHelper;
class CscRawDataCollection;

/* this class provides conversion between CSC RDO Id and RESrcID.
   RESrcID is used for identifying each ROD. 
   
   Author: Ketevi A. Assamagan
   BNL December 27 2003
   Following Tadashi's example for TGC
   
*/
class CSC_Hid2RESrcID 
{
public:

  /** default constrcutor
      you must then use the set method to set the cabling service and the id helper 
  */ 
  CSC_Hid2RESrcID () { m_cabling = 0;  m_cscIdHelper = 0; m_isCosmic=false; m_isOldCosmic = false; m_robIDs.clear(); } 

  /** the full constructor
  */
  CSC_Hid2RESrcID (CSCcablingSvc * p_cabling, const CscIdHelper* cscHelper) { this->set(p_cabling, cscHelper); }

  /** destructor 
  */ 
  ~CSC_Hid2RESrcID () = default;

  /** initialize the identifier helper
  */

  void set(CSCcablingSvc * p_cabling, const CscIdHelper* cscHelper) { 
    m_cabling = p_cabling; 
    m_cscIdHelper = cscHelper; 
    m_isCosmic=false; 
    m_isOldCosmic = false; 
    this->fillAllRobIds();
  }

  /** this is cosmic data */
  void set_isCosmic () { m_isCosmic = true; }

  /** to read the old cosmic data before the ROB id = ROD id fix */
  void set_isOldCosmic () { m_isOldCosmic = true; }

  /** Make a ROD Source ID for CSC RDO
   */ 

  uint32_t getRodID(const CscRawDataCollection *rdo) const;
  uint32_t getRodID(uint16_t subId, uint16_t rodId) const;

  /** Make a ROD Source ID for CscDigitCollection
   */ 
  uint32_t getRodID(const Identifier & offlineId) const;
  
  /** Make a ROB Source ID from a ROD source ID
   */ 
  uint32_t getRobID  (uint32_t rod_id) const;
  
  /** Make a ROS Source ID from a ROB source ID
   */ 
  uint32_t getRosID  (uint32_t rob_id) const;
  
  /** Make a SubDetector ID from ROS source ID 
   */
  uint32_t getDetID  (uint32_t ros_id) const; 

  /** return all the ROB IDs */
  const std::vector<uint32_t>& allRobIds() const { return m_robIDs; }

private:

  /** fill all the ROB ids */
  void fillAllRobIds();

private:

  const CscIdHelper * m_cscIdHelper; 
  bool m_isCosmic;
  bool m_isOldCosmic;
  CSCcablingSvc * m_cabling;
  std::vector<uint32_t> m_robIDs;
};

#endif 


