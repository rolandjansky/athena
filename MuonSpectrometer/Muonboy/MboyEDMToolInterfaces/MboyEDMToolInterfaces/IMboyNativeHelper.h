/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyNativeHelper_H
#define IMboyNativeHelper_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MB_SegmentAth;
class MbROTMDT;
class MbROTRPC;
class MbROTTGC;
class MbROTCSC;
class MbROTCSCclu;
class MbHoleMDT;
class MbHoleRPC;
class MbHoleTGC;
class MbHoleCSC;
class MbHoleCSCclu;

  /**
   @class IMboyNativeHelper

   This class is collecting number of utility methods
   on MboyCoreEvt objects

  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyNativeHelper("IMboyNativeHelper", 1, 0);

class IMboyNativeHelper:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Dumps MB_SegmentAth */
   virtual StatusCode Dump_MB_SegmentAth(
              MB_SegmentAth* pTheSegment,
              std::ofstream* pOutCurrent) = 0;

   
   /** Dumps Elements */
   virtual StatusCode Dump_MB_SegmentAthtElements(
              MB_SegmentAth* pSegment,
              std::ofstream* pOutCurrent) = 0;

   /** Dump Elements */
   virtual StatusCode dumpMbROTMDT    (const MbROTMDT*    pMbROTMDT    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbROTRPC    (const MbROTRPC*    pMbROTRPC    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbROTTGC    (const MbROTTGC*    pMbROTTGC    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbROTCSC    (const MbROTCSC*    pMbROTCSC    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbROTCSCclu (const MbROTCSCclu* pMbROTCSCclu ,std::ofstream* pOutCurrent) = 0;

   virtual StatusCode dumpMbHolemdt    (const MbHoleMDT*    pMbHoleMDT    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbHolerpc    (const MbHoleRPC*    pMbHoleRPC    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbHoletgc    (const MbHoleTGC*    pMbHoleTGC    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbHolecsc    (const MbHoleCSC*    pMbHoleCSC    ,std::ofstream* pOutCurrent) = 0;
   virtual StatusCode dumpMbHolecscclu (const MbHoleCSCclu* pMbHoleCSCclu ,std::ofstream* pOutCurrent) = 0;

   /** Segment Associated Station Name */
   virtual std::string SegmentAssociatedStationName(MB_SegmentAth* pSegment) = 0;

   /** Reconstructed Objects Validity */
   virtual int IsValid(MB_SegmentAth* pSegment) = 0;

   /** Segments Sharing State */
   virtual int SharingState(MB_SegmentAth* pSegment) = 0;

   /** Get Segment Point*/
   virtual void GetPoint(
                 MB_SegmentAth* pSegment,
                 double& Xpt,
                 double& Ypt,
                 double& Zpt
		 ) = 0;

   /** Get Segment direction */
   virtual void GetDirection(
                 MB_SegmentAth* pSegment,
                 double& Xdir,
                 double& Ydir,
                 double& Zdir
                 ) = 0;

   /** Get Segment quality */
   virtual double GetfitQuality( MB_SegmentAth* pSegment ) = 0;

   /** Get Nber of typed elements */
   virtual int GetNberOfDigiMDT        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfDigiRPC        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfDigiTGC        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfDigiCSC        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfDigiCSCclu     (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfDigiCSCPrecise (MB_SegmentAth* pSegment) = 0;
   
   virtual int GetNberOfHoleMDT        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfHoleRPC        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfHoleTGC        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfHoleCSC        (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfHoleCSCclu     (MB_SegmentAth* pSegment) = 0;

   /** Get Nber of elements */
   virtual int GetNberOfElements       (MB_SegmentAth* pSegment) = 0;
   virtual int GetNberOfDigi           (MB_SegmentAth* pSegment) = 0;

};

inline const InterfaceID& IMboyNativeHelper::interfaceID() { return IID_IIMboyNativeHelper; }

#endif

