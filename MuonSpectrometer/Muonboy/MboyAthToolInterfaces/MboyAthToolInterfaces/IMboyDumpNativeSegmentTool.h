/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyDumpNativeSegmentTool_H
#define IMboyDumpNativeSegmentTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthToolInterfaces/IMboyAthToolMisc.h"

  /**
   @class IMboyDumpNativeSegmentTool
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyDumpNativeSegmentTool("IMboyDumpNativeSegmentTool", 1, 0);

class IMboyDumpNativeSegmentTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /**Dump Native Segments  */
   virtual StatusCode dumpNativeSegments(
                                     int   LetCSCGo                     ,
                                     int   ApplyQualityFactorCut        , 
                                     float QualityFactorCut             ,
                                     int   ApplyNberofDigitsCut         ,
                                     int   NberofDigitsCut              ,
                                     int   ApplyNberofPrecisonDigitsCut ,
                                     int   NberofPrecisonDigitsCut      ,
                                     std::string SegmentStoreLocation   ,
                                     std::ofstream*      pOutFile
                                    ) = 0 ;
                                    
};

inline const InterfaceID& IMboyDumpNativeSegmentTool::interfaceID() { return IID_IIMboyDumpNativeSegmentTool; }

#endif

