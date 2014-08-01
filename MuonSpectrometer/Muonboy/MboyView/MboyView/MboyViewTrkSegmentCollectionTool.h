/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewTrkSegmentCollectionTool_H
#define MboyViewTrkSegmentCollectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonSegment/MuonSegment.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewTrkSegmentCollectionTool

   This class 
    is a tool to view TrkSegments Container

  @author samusog@cern.ch
  
  */

class MboyViewTrkSegmentCollectionTool:public AthAlgTool{
public:
    MboyViewTrkSegmentCollectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewTrkSegmentCollectionTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewTrkSegmentCollection(
               std::string TrkSegmentCollectionLocation,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

   /**Get Kine */
   void GetKineMuonSgement(
         const Muon::MuonSegment* pMuonSegment,
         double& PosX    ,
         double& PosY    ,
         double& PosZ    ,
         double& DirX    ,
         double& DirY    ,
         double& DirZ    
        );

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

