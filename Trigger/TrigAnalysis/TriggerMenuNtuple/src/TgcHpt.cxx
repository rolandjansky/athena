/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TgcHpt.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*/
#include "TriggerMenuNtuple/TgcHpt.h"

TgcHpt::TgcHpt() :
  isAside(false),
  isForward(false),
  phi(-1),
  trackletId(-1),
  sub(-1),
  delta(-1)
{
}

TgcHpt::~TgcHpt()
{
}

void
TgcHpt::clear ()
{

  isAside    = false;
  isForward  = false;
  phi        = -1;
  trackletId = -1;
  sub        = -1;
  delta      = -1;

}
