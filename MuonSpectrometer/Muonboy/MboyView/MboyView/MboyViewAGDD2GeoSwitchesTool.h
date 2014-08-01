/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewAGDD2GeoSwitchesTool_H
#define MboyViewAGDD2GeoSwitchesTool_H

#include "AthenaBaseComps/AthAlgTool.h"


class IGeoModelSvc;
#include "AthenaKernel/IOVSvcDefs.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewAGDD2GeoSwitchesTool

   This class 
    is a tool to view AGDD2GeoSwitches data

  @author samusog@cern.ch
  
  */

class MboyViewAGDD2GeoSwitchesTool:public AthAlgTool{
public:
    MboyViewAGDD2GeoSwitchesTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewAGDD2GeoSwitchesTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewAGDD2GeoSwitches(
               std::ofstream* pOut
              );

private:
///////////////////////////////////
   StatusCode regFcninitializeFromGeomodel();
   StatusCode initializeFromGeomodelCallback(IOVSVC_CALLBACK_ARGS);
   StatusCode initializeFromGeomodel();
   StatusCode initializeFromOracle();

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

   const IGeoModelSvc* p_IGeoModelSvc ; //!< Pointer On GeoModel

   std::string m_detectorKey  ; //!< Oracle node 
   std::string m_detectorNode ; //!< Oracle node

   std::vector<std::string> m_AGDD2GeoSwitches  ; //!< Store infos from AGDD2GeoSwitches
 
};

#endif

