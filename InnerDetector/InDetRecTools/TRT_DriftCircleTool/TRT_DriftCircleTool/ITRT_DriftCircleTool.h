/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ITRT_DriftCircleTool.h
//   Header file for class ITRT_DriftCircleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Andreas.Salzburger@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRT_IDriftCircleTool_H
#define TRT_IDriftCircleTool_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"


namespace InDet {
   
  /** Interface ID for TRT_DriftCircleTool */  
  static const InterfaceID IID_ITRT_DriftCircleTool("ITRT_DriftCircleTool", 1, 0); 
     
  /** @class ITRT_DriftCircleTool
      Interface class for TRT_DriftCircleTool
    
      @author Andreas.Salzburger@cern.ch 
     
    */
  class ITRT_DriftCircleTool : virtual public IAlgTool {
      
      public: 
       /** Destructor */
       virtual ~ITRT_DriftCircleTool(){};
       
       /** Return method for the InterfaceID */
       static const InterfaceID& interfaceID( ) { return IID_ITRT_DriftCircleTool; };
       
       /** Interface method */
       virtual InDet::TRT_DriftCircleCollection* convert(int,const InDetRawDataCollection<TRT_RDORawData>*, const EventContext& ctx, const bool CTBBadChannels) const = 0;               
       /** test validity gate for corrected drift times */
       virtual bool passValidityGate(unsigned int word, float lowGate, float highGate, float t0) const =0;

       
  };
      
}
  
  
#endif // TRT_IDriftCircleTool_H
