/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewTrackRecordCollectionTool_H
#define MboyViewTrackRecordCollectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrackRecord/TrackRecord.h"
#include "HepPDT/ParticleDataTable.hh"

#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"
#include "MboyView/MboyViewPointerDumperTool.h"

  /**
   @class MboyViewTrackRecordCollectionTool

   This class 
    is a tool to view TrackRecord Container

  @author samusog@cern.ch
  
  */

class MboyViewTrackRecordCollectionTool:public AthAlgTool{
public:
    MboyViewTrackRecordCollectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewTrackRecordCollectionTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewTrackRecordCollection(
               std::string TrackRecordCollectionLocation,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

   /**Get Kine */
   void GetKineTrackRecord(
         TrackRecord* pTrackRecord,
         int&    PDGCode ,
         double& PosX    ,
         double& PosY    ,
         double& PosZ    ,
         double& PX      ,
         double& PY      ,
         double& PZ      ,      
         double& Pt      
        );

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

   const HepPDT::ParticleDataTable	*m_particleDataTable;

   ToolHandle< MboyViewPointerDumperTool > p_MboyViewPointerDumperTool;

};

#endif

