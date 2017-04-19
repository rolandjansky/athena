/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*- mode: c++ -*-
#ifndef EVENT_LOOP_GRID_POOL_H
#define EVENT_LOOP_GRID_POOL_H

#include <functional>
#include <vector>

using WorkUnit = std::function<void(void)>;
using WorkList = std::vector<WorkUnit>;

void process(const WorkList& workList, const size_t nThreads);

#endif //EVENT_LOOP_GRID_POOL_H
