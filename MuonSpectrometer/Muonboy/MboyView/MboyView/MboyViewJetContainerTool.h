/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @author apolici
#ifndef MboyViewJetContainerTool_H
#define MboyViewJetContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewJetContainerTool

   This class 
    is a tool to view jet Container

  @author antonio.policicchio@cern.ch (apolici)
  
  */

class MboyViewJetContainerTool:public AthAlgTool{
public:
    MboyViewJetContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewJetContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewJetContainer(
               std::string JetContainerLocation,
               std::ofstream* pOut,
               float& EtMin,
               float& EtaMin
              );

private:
///////////////////////////////////

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

