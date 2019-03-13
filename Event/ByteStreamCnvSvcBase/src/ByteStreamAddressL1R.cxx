/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	ByteStreamAddressL1R implementation
//--------------------------------------------------------------------
//
//	Package    : ByteStreamCnvSvcBase
//
//  Description    : Special BSCnvSvc L1Result Address implementation
//
//	Author     : Hong Ma
//      created    : Dec 2, 2002
//  History    :
//====================================================================

#include "ByteStreamCnvSvcBase/ByteStreamAddressL1R.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"


/// Standard Constructor
ByteStreamAddressL1R::ByteStreamAddressL1R(const CLID& clid,
		const std::string& fname , const std::string& cname, int p1, int p2)
    : GenericAddress(ByteStreamAddress::storageType(), clid, fname, cname, p1, p2) {
}

ByteStreamAddressL1R::ByteStreamAddressL1R(const CLID& clid)
    : GenericAddress(ByteStreamAddress::storageType(), clid, "", "") {
}

/** Add pointer
 */
void ByteStreamAddressL1R::addPointer(const ROBFragment& p) {
   m_ptr.push_back(p);
}

/** access the fragment pointer
*/
const std::vector<ByteStreamAddressL1R::ROBFragment>& ByteStreamAddressL1R::getPointers() const {
   return(m_ptr);
}
