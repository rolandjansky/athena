/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * $Id: MboyViewL1RoIContainerTool.h 482567 2012-02-14 08:02:17Z sizun $
 * @author apolici
 */
#ifndef MboyViewL1RoIContainerTool_H
#define MboyViewL1RoIContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewL1RoIContainerTool

   This class 
    is a tool to view L1 RoI Containers

  @author antonio.policicchio@cern.ch (apolici)
  
  */

class MboyViewL1RoIContainerTool:public AthAlgTool{
public:
    MboyViewL1RoIContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewL1RoIContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewL1RoIContainer(
               std::string L1RoIContainerLocation,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

   //LVL1 RoIs
   const LVL1_ROI* lvl1Roi;

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

