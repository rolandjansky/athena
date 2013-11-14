/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyDumpMboyStoreTool_H
#define MboyDumpMboyStoreTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyAthToolInterfaces/IMboyDumpMboyStoreTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthTool/MboyAthToolMisc.h"

class MB_SegmentAth;
class MB_TrackStoreAth;
class MB_SegmentStoreAth;

  /**
   @class MboyDumpMboyStoreTool
   
  @author samusog@cern.ch
  
  */

class MboyDumpMboyStoreTool:public AthAlgTool, virtual public IMboyDumpMboyStoreTool{
public:
    MboyDumpMboyStoreTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyDumpMboyStoreTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /**Dump Mboy Store */
   virtual StatusCode dumpMboyStore(
                                    std::string SegmentStoreLocation              , 
                                    std::string TrackStoreLocation                ,
                                    std::ofstream*  pOutMuonboySegmentStore       ,
                                    std::ofstream*  pOutMuonboySegmentStoreChecks ,
                                    std::ofstream*  pOutMuonboyTrackStore         ,
                                    std::ofstream*  pOutMuonboyTrackStoreChecks  
                                   ) ;

private:
///////////////////////////////////

   /**Dump pMB_TrackStoreAth*/
   void PrintTracks(const MB_TrackStoreAth* pMB_TrackStoreAth,
                    std::ostream* out,int EventNber) const;

   /**Dump pMB_SegmentStoreAth*/
   void PrintSegments(const MB_SegmentStoreAth* pMB_SegmentStoreAth,
                      std::ostream* out,int EventNber) const;

};

#endif

