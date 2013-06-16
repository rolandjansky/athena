// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgType.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MSGTYPE_H
#define TRIGT1MUCTPI_MSGTYPE_H

namespace LVL1MUCTPI {

  enum MsgType { VERBOSE = 1, DEBUG = 2, INFO = 3, WARNING = 4,
                 ERROR = 5, FATAL = 6, ALWAYS = 7 };

}

#endif // TRIGT1MUCTPI_MSGTYPE_H
