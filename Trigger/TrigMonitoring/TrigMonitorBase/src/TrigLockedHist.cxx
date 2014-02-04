/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigLockedHist.cxx
 * @brief  Histogram lock implementation
 * @author Tomasz Bold, Frank Winklmeier
 *
 * $Id:$
 **/
#include "TrigMonitorBase/TrigLockedHist.h"

boost::mutex* lock_histogram_mutex::m_mutex(0);
