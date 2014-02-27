/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_BYTESTREAMADDRESSL1R_H
#define BYTESTREAMCNVSVCBASE_BYTESTREAMADDRESSL1R_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"

#include "ByteStreamData/RawEvent.h"

#include <stdint.h>
#include <vector>

/**
 *  @class  ByteStreamAddressL1R
 *  @brief  IOpaqueAddress for ByteStreamCnvSvc with ROB Fragments
 *
 *  Package     : ByteStreamCnvSvcBase
 *
 *  @author     : H. Ma
 *  description : Definition of RawEvent address class
 *                This class holds a vector ROBFragments Used by L1 Results
 *  Created     : Dec 2, 2002
 */

class ByteStreamAddressL1R : public GenericAddress {
public:
   typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBFragment;

   // @brief Destructor
   virtual ~ByteStreamAddressL1R() {}; //doesn't own event

   // @brief standard Constructor
   ByteStreamAddressL1R(const CLID& clid, const std::string& fname, const std::string& cname, int p1 = 0,int p2 = 0);

   // @brief constructor with only CLID
   ByteStreamAddressL1R(const CLID& clid);

   // @brief add ROB Fragment
   void addPointer(const ROBFragment& p);

   // @brief access the ROB fragments
   const std::vector<ROBFragment>& getPointers() const;

private:
   std::vector<ROBFragment> m_ptr;
};

#endif
