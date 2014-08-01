/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewTSOSDumpTool_H
#define MboyViewTSOSDumpTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"


/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewTSOSDumpTool

  @author samusog@cern.ch
  
  */

class MboyViewTSOSDumpTool:public AthAlgTool{
public:
    MboyViewTSOSDumpTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewTSOSDumpTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   void DumpTSOSs(
               const Trk::Track* pTrack ,
               std::ofstream* pOut
              );

private:
///////////////////////////////////


   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

