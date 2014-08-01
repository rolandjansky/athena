/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewGenEventSelectionTool_H
#define MboyViewGenEventSelectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewGenEventSelectionTool

   This class 
    is a tool to select gene evt

  @author samusog@cern.ch
  
  */

class MboyViewGenEventSelectionTool:public AthAlgTool{
public:
    MboyViewGenEventSelectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewGenEventSelectionTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode EventSelection(
               std::vector<std::string> TrackRecordCollectionLocationList,
               int GenEvtSelectionType,
               float GenEvtSelectionEtaMin,
               float GenEvtSelectionEtaMax
              );

};

#endif

