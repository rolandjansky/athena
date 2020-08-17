/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCTriggerEfficiency.h"
#include <AsgTools/MessageCheck.h>
#include <vector>

double ZDCTriggerEfficiency::GetEfficiency(int side, float ADCSum){
  double alpha = currentParams[side][0];
	double  beta = currentParams[side][1];
	double  theta = currentParams[side][2];
	if(alpha<10e-5||beta<10e-5||theta<10e-5)
	{
		return -1;
	}
	double m = exp(-ADCSum/theta);
  double p = exp(-(ADCSum - alpha) / beta);
	double efficiency = (1 - m) / (1 + p);
  return efficiency;
}

std::pair<double, double> ZDCTriggerEfficiency::GetEfficiencyAndError(MsgStream& msg, int side, float ADCSum){
  double alpha = currentParams[side][0];
	double  beta = currentParams[side][1];
	double  theta = currentParams[side][2];
	if(alpha<10e-5||beta<10e-5||theta<10e-5)
	{
		return std::make_pair(-1,-1);
	}

	double alphaErr = currentParamErrors[side][0];
	double betaErr = currentParamErrors[side][1];
	double thetaErr = currentParamErrors[side][2];

	double corrAlphaBeta = currentCorrCoefff[side][0];
	double corrAlphaTheta = currentCorrCoefff[side][1];
	double corrBetaTheta = currentCorrCoefff[side][2];

	double m = exp(-ADCSum/theta);
	double p = exp(-(ADCSum - alpha) / beta);
	double efficiency = (1 - m) / (1 + p);
	double dda = (1 - m) * -p / beta / (1 + p) / (1 + p);
	double ddb = (1 - m) * -p / pow(1 + p, 2.0) * ADCSum / beta / beta;
	double ddt = -ADCSum * m / theta / theta / (1 + p);

	double efficiencyErr = std::sqrt(
			std::pow(alphaErr * dda, 2) +
			std::pow(betaErr * ddb, 2) +
			std::pow(thetaErr * ddt, 2) +
			2 * corrAlphaBeta*alphaErr*betaErr * dda * ddb +
			2 * corrAlphaTheta*alphaErr*thetaErr * dda * ddt +
			2 * corrBetaTheta*betaErr*thetaErr * ddb * ddt);

	if (efficiencyErr<0)
	  {
	    efficiencyErr = 0;
            msg << MSG::INFO << "Efficiency Uncertainty calculation abnormal." << endmsg;
	  }

	return std::make_pair(efficiency, efficiencyErr);
}
