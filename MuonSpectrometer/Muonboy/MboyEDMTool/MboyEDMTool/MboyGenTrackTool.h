/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyGenTrackTool_H
#define MboyGenTrackTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "MboyEDMToolInterfaces/IMboyGenTrackTool.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

class MboyGenTrackStore;

  /**
   @class MboyGenTrackTool

   This class is extracting generated track collection

  @author samusog@cern.ch
  
  */

class MboyGenTrackTool:public AthAlgTool, virtual public IMboyGenTrackTool{
public:
    MboyGenTrackTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyGenTrackTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Get generated tracks collections */
   virtual StatusCode GetMboyGenTrackStore(
               MboyGenTrackStore*& pMboyGenTrackStore,
               std::string TrackRecordCollectionLocation
              );

private:
///////////////////////////////////

   MboyGenTrackStore* p_MboyGenTrackStore ; //!< Pointer On MboyGenTrackStore

   int m_KounterErrorMessage    ; //!< Control error messages prints
   int m_KounterErrorMessageMax ; //!< Control error messages prints
};

#endif

