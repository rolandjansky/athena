/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewEgammaContainerTool_H
#define MboyViewEgammaContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkParameters/TrackParameters.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewEgammaContainerTool

   This class 
    is a tool to view E gamma Container

  @author samusog@cern.ch
  
  */

class MboyViewEgammaContainerTool:public AthAlgTool{
public:
    MboyViewEgammaContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewEgammaContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewEgammaContainer(
               std::string EgammaContainerLocation,
               std::string EgammaEMTrackMatchContainerName,
               std::ofstream* pOut,
               float& EtMin,
               float& EtaMin,
	       float& CaloMinimumE
              );

private:
///////////////////////////////////
   size_t get_id_value(Identifier const & id) const;

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

