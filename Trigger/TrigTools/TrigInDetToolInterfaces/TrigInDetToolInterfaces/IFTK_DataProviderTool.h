/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IFTK_DATAPROVIDERTOOL_H__
#define __IFTK_DATAPROVIDERTOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrigInDetPattRecoEvent/TrigFTK_TrackSeed.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include <vector>

static const InterfaceID IID_IFTK_DataProviderTool("IFTK_DataProviderTool",1,0);

class IFTK_DataProviderTool : virtual public IAlgTool {
 public:
  /** other standard AlgTool methods */
  
  static const InterfaceID& interfaceID () {  //!< the Tool's interface
    return IID_IFTK_DataProviderTool; 
  }  	
  
  /** virtual functions here */
  virtual StatusCode retrieveFTK_Data(TrackCollection&) = 0;
  virtual StatusCode retrieveFTK_Data(const IRoiDescriptor&, TrackCollection&) = 0;
  virtual StatusCode retrieveFTK_Data(const IRoiDescriptor&, std::vector<const Trk::Track*>&) = 0;
  virtual StatusCode retrieveFTK_Data(std::vector<const Trk::Track*>&) = 0;
  virtual StatusCode retrieveFTK_Data(const IRoiDescriptor&, std::vector<TrigFTK_TrackSeed>&) = 0;
  virtual StatusCode retrieveFTK_Data(std::vector<TrigFTK_TrackSeed>&) = 0;

};

#endif
