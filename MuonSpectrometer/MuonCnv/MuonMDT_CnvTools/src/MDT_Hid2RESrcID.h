/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MDT_HID2RESRCID__
#define __MDT_HID2RESRCID__

#include <stdint.h>

#include <map>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

// this class provides conversion between Rpc pad Id and RESrcID
// RESrcID is used for identifying each ROD.
// Ketevi A. Assamagan
// Jan-16-2003, BNL
// -------------------------
// Adapted for MDT
// Michela Biglietti
// March 2003, Naples

class MDT_Hid2RESrcID {
public:
    /** constrcutor
     */
    MDT_Hid2RESrcID();

    /** intialize the cabling service
     */
    StatusCode set(const MdtIdHelper* mdtId);

    /** TMP method for sector 13 data */
    void setSpecialROBNumber(int specialROBNumber) { m_specialROBNumber = specialROBNumber; }
    /**
     */
    uint32_t getRodID(const Identifier& offlineId) const;

    /** Make a ROB Source ID from a ROD source ID
     */
    uint32_t getRobID(uint32_t rod_id) const;

    /** Make a ROS Source ID from a ROB source ID
     */
    uint32_t getRosID(uint32_t rob_id) const;

    /** Make a SubDetector ID from ROS source ID
     */
    uint32_t getDetID(uint32_t ros_id) const;

private:
    const MdtIdHelper* m_mdtIdHelper;

    int m_specialROBNumber;

    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey;
};

#endif
