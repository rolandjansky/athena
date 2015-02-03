// This file's extension implies that it's C, but it's really -*- C++ -*-.

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


#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TrackRecord/TrackRecordCollection.h"


#ifndef MUOND3PDMAKER_TRACKRECORDCOLLECTIONGETTERTOOL_H
#define MUOND3PDMAKER_TRACKRECORDCOLLECTIONGETTERTOOL_H


namespace D3PD {


class TrackRecordCollectionGetterTool
  : public SGCollectionGetterTool<TrackRecordCollection>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrackRecordCollectionGetterTool (const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);
};


} // namespace D3PD


#endif // not MUOND3PDMAKER_TRACKRECORDCOLLECTIONGETTERTOOL_H
