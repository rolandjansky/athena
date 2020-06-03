#include "ZdcAnalysis/ZDCTriggerEfficiency.h"
#include <vector>

using namespace std;
double ZDCTriggerEfficiency::GetEfficiency(int side, float ADCSum){
  double alpha = _currentParams[side][0];
	double  beta = _currentParams[side][1];
	double  theta = _currentParams[side][2];
	if(alpha<10e-5||beta<10e-5||theta<10e-5)
	{
		return -1;
	}
	double m = exp(-ADCSum/theta);
  double p = exp(-(ADCSum - alpha) / beta);
	double efficiency = (1 - m) / (1 + p);
  return efficiency;
}

std::pair<double, double> ZDCTriggerEfficiency::GetEfficiencyAndError(int side, float ADCSum){
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

	double corr_alpha_beta = _currentCorrCoefff[side][0]; 
	double corr_alpha_theta = _currentCorrCoefff[side][1];
	double corr_beta_theta = _currentCorrCoefff[side][2]; 

	double m = exp(-ADCSum/theta);
	double p = exp(-(ADCSum - alpha) / beta);
	double efficiency = (1 - m) / (1 + p);
	double dda = (1 - m) * -p / beta / (1 + p) / (1 + p);
	double ddb = (1 - m) * -p / pow(1 + p, 2.0) * ADCSum / beta / beta;
	double ddt = -ADCSum * m / theta / theta / (1 + p);

	double efficiencyErr = sqrt(
			alphaErr * alphaErr * dda * dda +
			betaErr * betaErr * ddb * ddb + 
			thetaErr * thetaErr * ddt * ddt + 
			2 * corr_alpha_beta*alphaErr*betaErr * dda * ddb + 
			2 * corr_alpha_theta*alphaErr*thetaErr * dda * ddt + 
			2 * corr_beta_theta*betaErr*thetaErr * ddb * ddt); 
       
	if (efficiencyErr<0)
	  {
	    efficiencyErr = 0;
	    std::cout<<"Efficiency Uncertainty calculation abnormal.";
	  }

	return std::make_pair(efficiency, efficiencyErr);
}
