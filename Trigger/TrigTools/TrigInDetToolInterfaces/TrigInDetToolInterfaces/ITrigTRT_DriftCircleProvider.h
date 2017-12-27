/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_IDriftCircleProviderTool_H
#define TRT_IDriftCircleProviderTool_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include <string>
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "ByteStreamData/RawEvent.h"
#include "IRegionSelector/IRoiDescriptor.h"


typedef std::vector< InDet::TRT_DriftCircle* >   driftCircleVec;
typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ;

static const InterfaceID IID_ITrigTRT_DriftCircleProviderTool("ITrigTRT_DriftCircleProviderTool", 1, 0);

class ITrigTRT_DriftCircleProviderTool : virtual public IAlgTool {
  
 public:
  /** Destructor */
  virtual ~ITrigTRT_DriftCircleProviderTool(){};
  
  /** Return method for the InterfaceID */
  static const InterfaceID& interfaceID( ) { return IID_ITrigTRT_DriftCircleProviderTool; };
  
  /** Interface method */
  virtual StatusCode  fillCollections( const IRoiDescriptor &roi) = 0;
  virtual StatusCode  fillCollections( std::vector<IdentifierHash> ,std::vector<const ROBF*>) = 0;
  virtual std::string trtContainerName() = 0;
  virtual double getTRTTotalTime() const = 0;
  virtual double getTRTRegSelTime() const = 0;
  virtual double getTRTTrtCnvTime() const = 0;
  virtual double getTRTRobProvTime() const = 0;

  virtual const std::vector<int>* fillTRT_DataErrors() = 0;
};

#endif
 
