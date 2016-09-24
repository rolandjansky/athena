/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCTriggerEfficiency.h"
#include <vector>

using namespace std;
float ZDCTriggerEfficiency::GetEfficiency(int side, float ADCSum){
  double alpha = _currentParams[side][0];
	double  beta = _currentParams[side][1];
	double  theta = _currentParams[side][2];
	if(alpha<10e-5||beta<10e-5||theta<10e-5)
	{
		return -1;
	}
	float m = exp(-ADCSum/theta);
	float p = exp(-(ADCSum - alpha) / beta);
	float efficiency = (1 - m) / (1 + p);
  return efficiency;
}

std::pair<float, float> ZDCTriggerEfficiency::GetEfficiencyAndError(int side, float ADCSum){
  double alpha = _currentParams[side][0];
	double  beta = _currentParams[side][1];
	double  theta = _currentParams[side][2];
	if(alpha<10e-5||beta<10e-5||theta<10e-5)
	{
		return std::make_pair(-1,-1);
	}

	double alphaErr = _currentParamErrors[side][0];
	double betaErr = _currentParamErrors[side][1];
	double thetaErr = _currentParamErrors[side][2];

	double cov_alpha_beta = _currentCorrCoefff[side][0]; 
	double cov_alpha_theta = _currentCorrCoefff[side][1];
	double cov_beta_theta = _currentCorrCoefff[side][2]; 

	float m = exp(-ADCSum/theta);
	float n = exp(alpha/beta);
	float p = exp(-(ADCSum - alpha) / beta);
	float efficiency = (1 - m) / (1 + p);
	float dda = (1 - m) * -n / beta / (1 + p) / (1 + p);
	float ddb = (1 - m) * -p / pow(1 + p, 2.0) * ADCSum / beta / beta;
	float ddt = -ADCSum * m / theta / theta / (1 + p);

	float efficiencyErr = sqrt(
			alphaErr * alphaErr * dda * dda +
			betaErr * betaErr * ddb * ddb + 
			thetaErr * thetaErr * ddt * ddt + 
			2 * cov_alpha_beta * dda * ddb + 
			2 * cov_alpha_theta * dda * ddt + 
			2 * cov_beta_theta * ddb * ddt); 
	if(alphaErr * alphaErr * dda * dda +
			betaErr * betaErr * ddb * ddb + 
			thetaErr * thetaErr * ddt * ddt + 
			2 * cov_alpha_beta * dda * ddb + 
			2 * cov_alpha_theta * dda * ddt + 
			2 * cov_beta_theta * ddb * ddt<0) efficiencyErr = 0; 

	return std::make_pair(efficiency, efficiencyErr);
}
