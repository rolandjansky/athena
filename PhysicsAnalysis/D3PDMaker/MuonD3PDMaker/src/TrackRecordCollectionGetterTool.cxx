/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackRecordCollectionGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2015
 * @brief 
 */


#include "TrackRecordCollectionGetterTool.h"


namespace D3PD {


TrackRecordCollectionGetterTool::TrackRecordCollectionGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : SGCollectionGetterTool<TrackRecordCollection> (type, name, parent)
{
}


} // namespace D3PD
