/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TString.h"
#include <map>
#include "LArCafJobs/Definitions.h"
#include "LArCafJobs/ClassCounts.h"
#include "LArCafJobs/CaloId.h"
#include "LArCafJobs/Geometry.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/RunData.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/HistoryContainer.h"

#include "LArCafJobs/PersistentAccessor.h"
#include "LArCafJobs/DataStore.h"

#include "LArCafJobs/AbsShape.h"
#include "LArCafJobs/SimpleShape.h"
#include "CaloIdentifier/CaloGain.h"

template class std::vector<const LArSamples::PersistentAccessor*>;
