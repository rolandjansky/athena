/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h>

int main()
{
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
#endif // XAOD_STANDALONE
  CP::EgammaCalibrationAndSmearingTool tool("EgammaCalibrationAndSmearingTool");
  tool.msg().setLevel(MSG::DEBUG);
  tool.setProperty("ESModel", "es2015cPRE").ignore();
  tool.setProperty("decorrelationModel", "FULL_v1").ignore();
  tool.initialize().ignore();
  return 0;
}
