/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_DIGITIZATION_H
#define AFP_DIGITIZATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

//class IPileUpTool; //Temporary for back-compatibility with 17.3.X.Y
class AFP_PileUpTool; //Temporary for back-compatibility with 17.3.X.Y

class AFP_DigiTop: public AthAlgorithm
{
 
 public:

  AFP_DigiTop(const std::string& name,
              ISvcLocator* pSvcLocator);
 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
 private:
  
  //ToolHandle<IPileUpTool> m_digTool; //Temporary for back-compatibility with 17.3.X.Y
  ToolHandle<AFP_PileUpTool> m_digTool; //Temporary for back-compatibility with 17.3.X.Y

};
  
#endif
