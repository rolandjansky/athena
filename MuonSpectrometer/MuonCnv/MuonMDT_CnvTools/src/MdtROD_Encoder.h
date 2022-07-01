/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTROD_ENCODER_H
#define MUONBYTESTREAM_MDTROD_ENCODER_H

#include <stdint.h>

#include <MuonIdHelpers/IMuonIdHelperSvc.h>
#include "ByteStreamData/RawEvent.h"
#include "MuonRDO/MdtCsm.h"
#include "GaudiKernel/ServiceHandle.h"


class MdtROD_Encoder {
public:
    /** constructor
     */
    MdtROD_Encoder();

    /** destructor
     */
    ~MdtROD_Encoder() = default;

    /** initialize the map
     */

    /** add Mdt Csms to the current list
     */
    void add(const MdtCsm* csm);

    /** clear the current csm list
     */
    void clear();

    /** convert all Csm in the current list to
      a vector of 32bit words
    */

    void fillROD(std::vector<uint32_t>& v);

private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_mdtIdHelper{"Muon::MuonIdHelperSvc/MuonIdHelperSvc", "MdtRDO_Encoder"};
    std::vector<const MdtCsm*> m_vMdtCsm;   
};

#endif
