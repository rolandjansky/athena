// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINER_P2_H__
#define TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINER_P2_H__

#include "TrigMuonEventTPCnv/TrigMuonEFIsolation_p2.h"
#include <vector>
/**
 * @package: TrigMuonEventTPCnv
 * @class TrigMuonEFIsolationContainer_p2
 *
 * @brief Persistent class partner for TrigMuonEFIsolationContainer
 * @author Mark Owen <markowen@cern.ch>
 */


class TrigMuonEFIsolationContainer_p2 : public std::vector<TrigMuonEFIsolation_p2>
{

};//class TrigMuonEFIsolationContainer_p2

#endif //TRIGMUONEVENTTPCNV_TRIGMUONEFISOLATIONCONTAINER_P2_H__
