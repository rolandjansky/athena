
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "AsgMessaging/AsgMessagingForward.h"

namespace asg
{
  bool AsgMessagingForward::msgLvl( const MSG::Level lvl ) const
  {
    MsgStream& msg = m_msg();
    if (msg.level() <= lvl)
    {
      msg << lvl;
      return true;
    } else
    {
      return false;
    }
  }

  MsgStream& AsgMessagingForward::msg() const
  {
    return m_msg();
  }

  MsgStream& AsgMessagingForward::msg( const MSG::Level lvl ) const
  {
    MsgStream& msg = m_msg ();
    msg << lvl;
    return msg;
  }
}
