// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterBadChannelData.h 589286 2014-03-25 14:39:52Z wlampl $
#ifndef XAODCALOEVENT_CALOCLUSTERBADCHANNELDATA_H
#define XAODCALOEVENT_CALOCLUSTERBADCHANNELDATA_H

// Local include(s):
#include "xAODCaloEvent/versions/CaloClusterBadChannelData_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

  typedef CaloClusterBadChannelData_v1 CaloClusterBadChannelData;

  typedef std::vector<CaloClusterBadChannelData>  CaloClusterBadChannelList;
}

#endif // XAODCALOEVENT_CALOCLUSTER_H
