/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <IsolationCorrections/IsolationCorrectionTool.h>

int main()
{
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
#endif // XAOD_STANDALONE

  //TString FileName = "isolation_ptcorrections_rel20_2_v7.root";
  //bool useLogLog = false;
  //int OutputLevel = 2;
  CP::IsolationCorrectionTool tool("isoTest");
  //tool.setProperty("CorrFile",FileName).ignore();
  //tool.setProperty("OutputLevel",OutputLevel).ignore();
  //tool.setProperty("ForcePartType",true).ignore();
  //tool.setProperty("LogLogFitForLekage",useLogLog).ignore();
  tool.msg().setLevel(MSG::DEBUG);
  tool.initialize().ignore();
  
  return 0;
}
