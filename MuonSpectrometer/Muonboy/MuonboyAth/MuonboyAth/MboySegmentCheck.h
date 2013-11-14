/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboySegmentCheck_H
#define MboySegmentCheck_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MuonboyAth/MuonboyAthMisc.h"

class IMboyDumpNativeSegmentTool;

  /**
   @class MboySegmentCheck

   This class is performing dumps of the collections of 
   MboyCoreEvt object for segments

  @author samusog@cern.ch

  */

class MboySegmentCheck:public AthAlgorithm {
public:
    MboySegmentCheck(const std::string& name, ISvcLocator* pSvcLocator);
    ~MboySegmentCheck();

public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////

   std::string m_SegmentStoreLocation ;//!< Location of the MB_SegmentStoreTDS

   std::string    m_NameOfMboySegmentCheckOutputFile ;//!< file to dump the tracks store
   std::ofstream  m_OutMboySegmentCheck              ;//!< file to dump the tracks store

   int   m_LetCSCGo                     ; //!< Control production of CSC segment
   
   int   m_ApplyQualityFactorCut        ; //!< Control cut of quality factor
   float m_QualityFactorCut             ; //!< Value of cut of quality factor
   
   int   m_ApplyNberofDigitsCut         ; //!< Control cut of nber of digits
   int   m_NberofDigitsCut              ; //!< Value of cut of nber of digits
   
   int   m_ApplyNberofPrecisonDigitsCut ; //!< Control cut of nber of digits
   int   m_NberofPrecisonDigitsCut      ; //!< Value of cut of nber of digits

   ToolHandle< IMboyDumpNativeSegmentTool >      p_IMboyDumpNativeSegmentTool      ; //!< Pointer On IMboyDumpNativeSegmentTool

};

#endif

