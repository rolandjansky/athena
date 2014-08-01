/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewEventSelectionTool_H
#define MboyViewEventSelectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewEventSelectionTool

   This class 
    is a tool to select event

  @author samusog@cern.ch
  
  */

class MboyViewEventSelectionTool:public AthAlgTool{
public:
    MboyViewEventSelectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewEventSelectionTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode EventSelection(
               std::string MuonRecDigiStoreTDSLocation,
               std::string TrkTrackCollectionLocation,
               int EvtSelectionType,
               int EvtSelectionThreshold
              );

private:
///////////////////////////////////

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

