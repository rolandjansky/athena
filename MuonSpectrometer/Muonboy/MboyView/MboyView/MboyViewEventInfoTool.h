/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewEventInfoTool_H
#define MboyViewEventInfoTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "MboyView/MboyViewAGDD2GeoSwitchesTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewEventInfoTool

   This class 
    is a tool to get evt info

  @author samusog@cern.ch
  
  */

class MboyViewEventInfoTool:public AthAlgTool {
public:
    MboyViewEventInfoTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewEventInfoTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewEventInfo(
               std::string MuonRecDigiStoreTDSLocation,
               int ViewAthIds,
               int ATCOSM,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

  /**First evt actions Control */
  int m_IsFirstEvent ;
  std::string GetGeoModelAtlasVersion();

  void putConditionsTags(std::ostream& out) const;

  int m_KounterErrorMessage    ; //!< Message Control 
  int m_KounterErrorMessageMax ; //!< Message Control 

  /**AGDD2GeoSwitches Viewer */
  int m_ViewAGDD2GeoSwitches ;
  ToolHandle< MboyViewAGDD2GeoSwitchesTool > p_MboyViewAGDD2GeoSwitchesTool;

  int m_SIM ; //!< Sim Control 
};

#endif

