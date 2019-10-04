/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/OHLockedHist.h"

std::mutex* oh_lock_histogram_mutex::m_mutex = nullptr;
