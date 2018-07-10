///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAlgTool.cxx 
// Implementation file for class AthAlgTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// AthenaBaseComps includes
#include "AthenaBaseComps/AthAlgTool.h"

AthAlgTool::AthAlgTool( const std::string& type, 
			const std::string& name, 
			const IInterface* parent ) : 
  AthCommonDataStore<AthCommonMsg<AlgTool>> ( type, name, parent )
{
}

AthAlgTool::~AthAlgTool()
{ 
}



