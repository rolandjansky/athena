/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RefineMonitor_H
#define RefineMonitor_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyAthToolInterfaces/IRefineMonitor.h"

/////////////////////////////////////////////////////////////////////////////
#ifdef HAVE_NEW_IOSTREAMS
//gcc3.2
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#else
//gcc 295
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#endif
#if HAVE_CXX_STDC_HEADERS
# include <cstdio>
# include <cmath>
#else
# include <stdio.h>
# include <math.h>
#endif
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <utility>

class IMboyAthToolHelper;

namespace Muon {

  class MuPatCandidateBase;
  class MuPatSegment;
  class MuPatTrack;  
  class MuonSegment;  

  /**
   @class RefineMonitor

  @author samusog@cern.ch
  
  */


class RefineMonitor:public AthAlgTool, virtual public IRefineMonitor{
public:
    RefineMonitor(const std::string&,const std::string&,const IInterface*);
    virtual ~RefineMonitor 	 ();

public:

 virtual StatusCode initialize        ();
 virtual StatusCode finalize          ();

 virtual void Reset() ;
 virtual void PrintSegConfig ( Muon::MuPatTrack& MuPatTrackToBeRefined ) ;
 virtual bool AlreadySuccessfullyTried (  Muon::MuPatTrack& MuPatTrackToBeRefined ) ;
 virtual void RegisterIfSuccessful     (  Muon::MuPatTrack& MuPatTrackToBeRefined , const Muon::MuPatTrack* pMuPatTrack ) ;
   
private:
///////////////////////////////////

   std::string m_ToolName ;

   std::vector< std::vector<const Muon::MuonSegment*> > m_CalledConfig  ;

   ToolHandle< IMboyAthToolHelper >   p_OwnEDMHelper          ; //!< Pointer On IMboyAthToolHelper

};

}

#endif

