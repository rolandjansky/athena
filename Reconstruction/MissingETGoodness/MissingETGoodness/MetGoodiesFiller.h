// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_MetGoodiesFiller_H
#define MissingETGoodness_MetGoodiesFiller_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MissingETGoodness/Goodies.h"

#include <string>

/////////////////////////////////////////////////////////////////////////////
static const InterfaceID IID_MetGoodiesFiller("MetGoodiesFiller", 1, 0);

class MetGoodiesFiller : public AthAlgorithm 
{
public:
   MetGoodiesFiller(const std::string& name, ISvcLocator* pSvcLocator );
   virtual ~MetGoodiesFiller();
  
   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();
  
private:
   void initGoodiesMap();
   static MET::Goodies& s_goodies;
};

#endif // MissingETGoodness_MetGoodiesFiller_H

