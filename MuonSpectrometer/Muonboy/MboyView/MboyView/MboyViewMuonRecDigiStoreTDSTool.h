/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewMuonRecDigiStoreTDSTool_H
#define MboyViewMuonRecDigiStoreTDSTool_H

#include "AthenaBaseComps/AthAlgTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewMuonRecDigiStoreTDSTool

   This class 
    is a tool to view digits Container

  @author samusog@cern.ch
  
  */

class MboyViewMuonRecDigiStoreTDSTool:public AthAlgTool{
public:
    MboyViewMuonRecDigiStoreTDSTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewMuonRecDigiStoreTDSTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewMuonRecDigiStoreTDS(
               std::string MuonRecDigiStoreTDSLocation,
               int ViewAthIds,
               int PrintRSLine,
               std::ofstream* pOut
              );

private:
///////////////////////////////////

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

