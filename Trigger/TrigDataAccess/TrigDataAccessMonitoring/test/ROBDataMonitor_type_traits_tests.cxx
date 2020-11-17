/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <type_traits>

#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

int main() {

  static_assert(std::is_nothrow_move_constructible<robmonitor::ROBDataStruct>::value);

	return 0;
}