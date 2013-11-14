/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDumpNativeSegmentTool_H
#define MboyDumpNativeSegmentTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyAthToolInterfaces/IMboyDumpNativeSegmentTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class IMboyNativeHelper;
#include "MboyAthEvt/MB_SegmentAth.h"

  /**
   @class MboyDumpNativeSegmentTool
   
  @author samusog@cern.ch
  
  */

class MboyDumpNativeSegmentTool:public AthAlgTool, virtual public IMboyDumpNativeSegmentTool{
public:
    MboyDumpNativeSegmentTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyDumpNativeSegmentTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /**Dump collection */
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
                                    ) ;

private:
///////////////////////////////////

   /**Validity of a segment*/
   int  RecSeg_IsOk(
                    int   LetCSCGo                     ,
                    int   ApplyQualityFactorCut        , 
                    float QualityFactorCut             ,
                    int   ApplyNberofDigitsCut         ,
                    int   NberofDigitsCut              ,
                    int   ApplyNberofPrecisonDigitsCut ,
                    int   NberofPrecisonDigitsCut      ,
                    MB_SegmentAth& TheSegment
                   );

   ToolHandle< IMboyNativeHelper > p_OwnEDMHelper ; //!< Pointer On IMboyNativeHelper

};

#endif

