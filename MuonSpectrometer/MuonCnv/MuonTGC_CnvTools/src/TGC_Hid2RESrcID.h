/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
      TGC_Hid2RESrcID () {}

      /** Destrcutor */ 
      ~TGC_Hid2RESrcID () {} 

      /** Make a ROD Source ID for TGC RDO. */ 
      uint32_t getRodID(const TgcRdo *rdo) const;
      /** Make a ROD Source ID for SubDetector ID and ROD ID. */ 
      uint32_t getRodID(uint16_t subDetectorId, uint16_t rodId) const;
      /** Make a ROD Source ID for TgcDigitCollection. */ 
      uint32_t getRodID(const Identifier & offlineId,
                        const ITGCcablingSvc* cabling) const;
      /** Make a ROB Source ID from a ROD source ID. */ 
      uint32_t getRobID  (uint32_t rod_id) const; 
      /** Make a ROS Source ID from a ROB source ID. */ 
      uint32_t getRosID  (uint32_t rob_id) const;
      /** Make a SubDetector ID from ROS source ID. */
      uint32_t getDetID  (uint32_t ros_id) const;

      /** Return all the ROB IDs. */
      const std::vector<uint32_t>& allRobIds() const { return m_robIDs; } 

      /** Fill all the ROB IDs. */
      void fillAllRobIds();

    private:
      std::vector<uint32_t> m_robIDs;
    };

} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGC_HID2RESRCID
