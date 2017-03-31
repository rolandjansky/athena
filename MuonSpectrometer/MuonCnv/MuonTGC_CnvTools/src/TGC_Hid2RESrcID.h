/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGC_HID2RESRCID
#define MUONTGC_CNVTOOLS_TGC_HID2RESRCID

#include <vector>
#include <inttypes.h> 

class TgcRdo;
class Identifier;
class ITGCcablingSvc;

namespace Muon 
{

  /** @class TGC_Hid2RESrcID
   *  This class provides conversion between TGC RDO Id and RESrcID.
   *  RESrcID is used for identifying each ROD. 
   *
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   *
   *  This class was developed by Tadashi Maeno based on 
   *  MDT_Hid2RESrcID written by Naples. 
   */

  class TGC_Hid2RESrcID 
    {
    public:

      /** Constrcutor */ 
      TGC_Hid2RESrcID () : m_cabling(0) {}

      /** Destrcutor */ 
      ~TGC_Hid2RESrcID () {} 

      /** Initialize the cabling service. */
      void set(const ITGCcablingSvc* p_cabling);

      /** Make a ROD Source ID for TGC RDO. */ 
      uint32_t getRodID(const TgcRdo *rdo);
      /** Make a ROD Source ID for SubDetector ID and ROD ID. */ 
      uint32_t getRodID(uint16_t subDetectorId, uint16_t rodId);
      /** Make a ROD Source ID for TgcDigitCollection. */ 
      uint32_t getRodID(const Identifier & offlineId);
      /** Make a ROB Source ID from a ROD source ID. */ 
      uint32_t getRobID  (uint32_t rod_id); 
      /** Make a ROS Source ID from a ROB source ID. */ 
      uint32_t getRosID  (uint32_t rob_id); 
      /** Make a SubDetector ID from ROS source ID. */
      uint32_t getDetID  (uint32_t ros_id); 

      /** Return all the ROB IDs. */
      const std::vector<uint32_t>& allRobIds() { return m_robIDs; } 

    private:
      /** Fill all the ROB IDs. */
      void fillAllRobIds();

    private:
      const ITGCcablingSvc * m_cabling;
      std::vector<uint32_t> m_robIDs;
    };

} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGC_HID2RESRCID
