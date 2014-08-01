/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewIDRecDigiTool_H
#define MboyViewIDRecDigiTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

//TRT
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"

/////////////////////////////////////////////////////////////////////////////

#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewCombinedMuonContainerTool

   This class 
   is a tool to view Muon Container

  @author Oleg.Fedin@cern.ch
  
  */
  
class MboyViewIDRecDigiTool:public AthAlgTool{
public:
    MboyViewIDRecDigiTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewIDRecDigiTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode TRTRDORetrive(std::string TRTRDOContainerName,int,int,std::ofstream* pOut);
   StatusCode SCTRDORetrive(std::string SCTRDOContainerName,std::ofstream* pOut);
   StatusCode PixelRDORetrive(std::string PixelRDOContainerName,std::ofstream* pOut);

private:
///////////////////////////////////

   std::string m_DataObjectName;
   const InDetDD::TRT_DetectorManager  *m_trtmgr;
   const InDetDD::SCT_DetectorManager  *m_sctmgr;
   const InDetDD::PixelDetectorManager *m_pixelmgr;

   const TRT_ID*  p_TRT_ID  ;
   const SCT_ID*  p_SCT_ID  ;
   const PixelID* p_PixelID ;

   ToolHandle< ITRT_DriftFunctionTool > p_ITRT_DriftFunctionTool;

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif
