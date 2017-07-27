/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_SELECTALLOBJECT_H
#define EVENTCONTAINERS_SELECTALLOBJECT_H

#include "SelectAllObjectMT.h"

template < class DCC, typename OBJECT = typename DCC::IDENTIFIABLE::DIGIT >
using SelectAllObject = SelectAllObjectMT<DCC, OBJECT>;

#endif 
