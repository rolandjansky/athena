/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqutilsStatusFlagCOOL_h
#define dqutilsStatusFlagCOOL_h

// Protect CINT from some system definitions that cause problems
#ifndef __CINT__
  //COOL API include files (CoolKernel)
  #include "CoolKernel/pointers.h"
  #include "CoolKernel/ValidityKey.h"
#else
  namespace cool {
    class IDatabasePtr;
    class IFolderPtr;
  }
#endif


#include <iostream>
#include <string>
#include <cstdlib>

#include <TObject.h>

//CORAL API include files
#include "CoralBase/AttributeList.h"

//COOL API include files (CoolApplication)
#include "CoolApplication/Application.h"
// --> limits.h is needed for CoolKernel/types.h
#include <limits.h>
#include "CoolKernel/types.h"
#include "CoolKernel/ChannelId.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/ChannelSelection.h"

#include "DataQualityUtils/StatusFlagCOOLBase.h"

namespace dqutils {

class StatusFlagCOOL : public StatusFlagCOOLBase {

public:
    // Methods needed to come up to COOL framework.
    cool::RecordSpecification createSpec();
    coral::AttributeList createPayload(int colourCode, float dfrac, float thrust, const 
                                       cool::RecordSpecification& spec);
   
    // Constructors and Destructors.
    StatusFlagCOOL(std::string dbStr, std::string folderStr, int runS, int lumiS, int runU, int lumiU);
    StatusFlagCOOL(int runS, int lumiS, int runU, int lumiU);
    StatusFlagCOOL();
    virtual ~StatusFlagCOOL () {};

    // Methods of "interest". To insert a status flag you can use either ChannelId or ChannelName.
    void insert(cool::ChannelId channelId, int code, float dfrac, float thrust, std::string tag_name);
    void insert(std::string channelName, int code, float dfrac, float thrust, std::string tag_name);

    // Needed for the ROOT interface.
    ClassDef( StatusFlagCOOL, 0 ) // A class for modifying DQ info in the COOL database
};

}

#endif
