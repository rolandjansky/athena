/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @author apolici
#ifndef MboyViewMETContainerTool_H
#define MboyViewMETContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewMETContainerTool

   This class 
    is a tool to view met Container

  @author antonio.policicchio@cern.ch (apolici)
  
  */

class MboyViewMETContainerTool:public AthAlgTool{
public:
    MboyViewMETContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewMETContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewMETContainer(
               std::string METContainerLocation,
               std::ofstream* pOut,
               float& EtMin
              );

private:
///////////////////////////////////

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

