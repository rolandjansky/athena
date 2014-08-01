/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewPointerDumperTool_H
#define MboyViewPointerDumperTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkTrack/Track.h"
#include "TrackRecord/TrackRecord.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewPointerDumperTool

   This class 
    is a tool to view digits Container

  @author samusog@cern.ch
  
  */

class MboyViewPointerDumperTool:public AthAlgTool{
public:
    MboyViewPointerDumperTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewPointerDumperTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**Dumper */
   void DumpPointer(
               std::ofstream* pOut ,
               const Trk::Track* pTrack
              );
   void DumpPointer(
               std::ofstream* pOut ,
               TrackRecord* pTrackRecord
              );

private:
///////////////////////////////////

   int m_DoDump    ; //!< Dump Control 

};

#endif

