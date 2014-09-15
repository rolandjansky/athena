/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyNativeHelper_H
#define MboyNativeHelper_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MboyEDMToolInterfaces/IMboyNativeHelper.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

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
   @class MboyNativeHelper

   This class is collecting number of utility methods
   on MboyCoreEvt objects

  @author samusog@cern.ch
  
  */

class MboyNativeHelper:public AthAlgTool, virtual public IMboyNativeHelper{
public:
    MboyNativeHelper(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyNativeHelper 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /** Dumps MB_SegmentAth */
   virtual StatusCode Dump_MB_SegmentAth(
              MB_SegmentAth* pTheSegment,
              std::ofstream* pOutCurrent);

   
   /** Dumps Elements */
   virtual StatusCode Dump_MB_SegmentAthtElements(
              MB_SegmentAth* pSegment,
              std::ofstream* pOutCurrent);

   /** Dump Elements */
   virtual StatusCode dumpMbROTMDT    (const MbROTMDT*    pMbROTMDT    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbROTRPC    (const MbROTRPC*    pMbROTRPC    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbROTTGC    (const MbROTTGC*    pMbROTTGC    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbROTCSC    (const MbROTCSC*    pMbROTCSC    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbROTCSCclu (const MbROTCSCclu* pMbROTCSCclu ,std::ofstream* pOutCurrent) ;

   virtual StatusCode dumpMbHolemdt    (const MbHoleMDT*    pMbHoleMDT    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbHolerpc    (const MbHoleRPC*    pMbHoleRPC    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbHoletgc    (const MbHoleTGC*    pMbHoleTGC    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbHolecsc    (const MbHoleCSC*    pMbHoleCSC    ,std::ofstream* pOutCurrent) ;
   virtual StatusCode dumpMbHolecscclu (const MbHoleCSCclu* pMbHoleCSCclu ,std::ofstream* pOutCurrent) ;

   /** Segment Associated Station Name */
   virtual std::string SegmentAssociatedStationName(MB_SegmentAth* pSegment);

   /** Reconstructed Objects Validity */
   virtual int IsValid(MB_SegmentAth* pSegment);

   /** Segments Sharing State */
   virtual int SharingState(MB_SegmentAth* pSegment);

   /** Get Segment Point*/
   virtual void GetPoint(
                 MB_SegmentAth* pSegment,
                 double& Xpt,
                 double& Ypt,
                 double& Zpt
		 );

   /** Get Segment direction */
   virtual void GetDirection(
                 MB_SegmentAth* pSegment,
                 double& Xdir,
                 double& Ydir,
                 double& Zdir
                 );

   /** Get Segment quality */
   virtual double GetfitQuality( MB_SegmentAth* pSegment );

   /** Get Nber of typed elements */
   virtual int GetNberOfDigiMDT        (MB_SegmentAth* pSegment);
   virtual int GetNberOfDigiRPC        (MB_SegmentAth* pSegment);
   virtual int GetNberOfDigiTGC        (MB_SegmentAth* pSegment);
   virtual int GetNberOfDigiCSC        (MB_SegmentAth* pSegment);
   virtual int GetNberOfDigiCSCclu     (MB_SegmentAth* pSegment);
   virtual int GetNberOfDigiCSCPrecise (MB_SegmentAth* pSegment);
   
   virtual int GetNberOfHoleMDT        (MB_SegmentAth* pSegment);
   virtual int GetNberOfHoleRPC        (MB_SegmentAth* pSegment);
   virtual int GetNberOfHoleTGC        (MB_SegmentAth* pSegment);
   virtual int GetNberOfHoleCSC        (MB_SegmentAth* pSegment);
   virtual int GetNberOfHoleCSCclu     (MB_SegmentAth* pSegment);

   /** Get Nber of elements */
   virtual int GetNberOfElements       (MB_SegmentAth* pSegment);
   virtual int GetNberOfDigi           (MB_SegmentAth* pSegment);

};

#endif

