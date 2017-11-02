/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_BYTESTREAMADDRESS_H
#define BYTESTREAMCNVSVCBASE_BYTESTREAMADDRESS_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/EventContext.h"

#include <stdint.h>
#include <vector>

/**
 *  @class  ByteStreamAddress
 *  @brief  IOpaqueAddress for ByteStreamCnvSvc, with ROB ids
 *
 *  Package     : ByteStreamCnvSvcBase
 *
 *  description : Definition of RawEvent address class
 *                This class holds a vector of ROBIDs
 *  @author     : H. Ma
 *  Revision    : July 10, 2002, Use new eformat package
 *  Revision    : Sept 19, 2002, Store ROB IDs, to be used with ROBDataProviderSvc
 */

class ByteStreamAddress : public GenericAddress {
public:
   // @brief Constructor
   ByteStreamAddress(const CLID& clid, const std::string& fname, const std::string& cname, int p1 = 0, int p2 = 0);
   // @brief Constructor
   ByteStreamAddress(const CLID& clid);

   // @brief Destructor
   virtual ~ByteStreamAddress() {}; //doesn't own event

   // @brief Add a rob id
   void add(uint32_t robid);

   // @brief Add event id
   void setEventContext(const EventContext& eid);

   // @brief access the ROB fragment IDs
   const std::vector<uint32_t>& getRobIDs() const;

   // @brief get event id
   const EventContext& getEventContext() const;

private:
   // @brief it holds a vector of rob ids
   std::vector<uint32_t> m_robIDs;
   EventContext m_eid;
};

inline void ByteStreamAddress::setEventContext(const EventContext& eid) 
{ m_eid = eid; }

inline const EventContext& ByteStreamAddress::getEventContext() const
{ return m_eid; }

#endif
