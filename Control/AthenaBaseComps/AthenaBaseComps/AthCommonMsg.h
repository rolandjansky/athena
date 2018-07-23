/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @class  AthCommonMsg
 *  @file   AthenaBaseComps/AthCommonMsg.h
 *  @author Charles Leggett
 *  @date   June 2018
 *  @brief  Templated class that provides backwards compatibility for
 *          legacy MsgService manipulation methods
 */

#ifndef ATHENABASECOMPS_ATHCOMMONMSG_H
#define ATHENABASECOMPS_ATHCOMMONMSG_H

#include "GaudiKernel/MsgStream.h"

template <class PBASE>
class AthCommonMsg : public PBASE {
public:
  using PBASE::PBASE;

  // forward to CommonMessaging
  inline MsgStream& msg() const {
    return this->msgStream();
  }
  inline MsgStream& msg(const MSG::Level lvl) const {
    return this->msgStream(lvl);
  }
  inline bool msgLvl(const MSG::Level lvl) const {
    return this->msgLevel(lvl);
  }

};


#endif
