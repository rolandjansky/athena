/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "AsgTools/AsgMessagingForward.h"

namespace asg
{
  bool AsgMessagingForward::msgLvl( const MSG::Level lvl ) const
  {
    if (m_msg->level() <= lvl)
    {
      (*m_msg) << lvl;
      return true;
    } else
    {
      return false;
    }
  }

  MsgStream& AsgMessagingForward::msg() const
  {
    return *m_msg;
  }

  MsgStream& AsgMessagingForward::msg( const MSG::Level lvl ) const
  {
    (*m_msg) << lvl;
    return *m_msg;
  }
}
