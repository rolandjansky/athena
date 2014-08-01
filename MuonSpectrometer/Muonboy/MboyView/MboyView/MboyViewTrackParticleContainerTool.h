/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewTrackParticleContainerTool_H
#define MboyViewTrackParticleContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"
#include "MboyView/MboyViewPointerDumperTool.h"
#include "MboyView/MboyViewTrTrackDumpTool.h"
#include "MboyView/MboyViewTSOSDumpTool.h"

  /**
   @class MboyViewTrackParticleContainerTool

   This class 
    is a tool to view TrackParticle Container

  @author samusog@cern.ch
  
  */

class MboyViewTrackParticleContainerTool:public AthAlgTool{
public:
    MboyViewTrackParticleContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewTrackParticleContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewTrackParticleContainer(
               std::string TrackParticleContainerLocation,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

   /**Get Kine */
   void GetKinePerigee(
         const Trk::Perigee* pPerigee,
         double& Charge  ,
         double& PosX    ,
         double& PosY    ,
         double& PosZ    ,
         double& PX      ,
         double& PY      ,
         double& PZ      ,
         double& Pt      
        );

   int m_DoCom    ; //!< Message Control 
   int m_DoTSO    ; //!< Message Control 

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

   ToolHandle< MboyViewPointerDumperTool > p_MboyViewPointerDumperTool;
   ToolHandle< MboyViewTrTrackDumpTool > p_MboyViewTrTrackDumpTool;
   ToolHandle< MboyViewTSOSDumpTool > p_MboyViewTSOSDumpTool;

};

#endif

