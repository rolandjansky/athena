/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TString.h"
#include <map>
 
#include "LArSamplesMon/Residual.h"

#include "LArSamplesMon/IndexRange.h"
#include "LArSamplesMon/Averager.h"
#include "LArSamplesMon/ScaledShiftedShape.h"
#include "LArSamplesMon/ShapeErrorData.h"
#include "LArSamplesMon/ScaledErrorData.h"
#include "LArSamplesMon/AbsShapeErrorGetter.h"
#include "LArSamplesMon/CombinedShapeErrorGetter.h"
#include "LArSamplesMon/UniformShapeErrorGetter.h"
#include "LArSamplesMon/TreeShapeErrorGetter.h"
#include "LArSamplesMon/OFC.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/HistoryIterator.h"
#include "LArSamplesMon/AbsLArCells.h"
#include "LArSamplesMon/Accessor.h"
#include "LArSamplesMon/TreeAccessor.h"
#include "LArSamplesMon/MultiTreeAccessor.h"
#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/FilterParams.h"
#include "LArSamplesMon/FilterList.h"
#include "LArSamplesMon/DataTweaker.h"
#include "LArSamplesMon/Chi2Calc.h"
#include "LArSamplesMon/ShapeFitter.h"
#include "LArSamplesMon/ShapeDrawer.h"
#include "LArSamplesMon/MonitorBase.h"
#include "LArSamplesMon/OccupancyMonitor.h"
#include "LArSamplesMon/DigitMonitor.h"
#include "LArSamplesMon/FitMonitor.h"
#include "LArSamplesMon/TimingMonitor.h"
//#include "LArSamplesMon/LudovicaTiming.h"
#include "LArSamplesMon/TimingClass.h"
#include "LArSamplesMon/Splitter.h"

#include "CaloIdentifier/CaloGain.h"

#include "LArSamplesMon/LArCellsEmptyMonitoring.h"
#include "LArSamplesMon/LArIdTranslatorHelper.h"

#include "LArSamplesMon/CaloD3PDClass.h"
#include "LArSamplesMon/D3PDConverter.h"

