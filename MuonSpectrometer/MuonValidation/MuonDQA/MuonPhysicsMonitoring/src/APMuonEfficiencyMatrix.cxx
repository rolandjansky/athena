/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPhysicsMonitoring/APMuonEfficiencyMatrix.h"

using namespace Insitu;


// Constructors
APMuonEfficiencyMatrix::APMuonEfficiencyMatrix():
  APEfficiencyMatrix()
{
  APEfficiencyMatrix::clear();
 
  std::vector<APMatrixAxis> axes;
  std::vector<double> bins_Eta;
  std::vector<double> bins_Phi;
  std::vector<double> bins_Pt;
	
  bins_Eta.clear();
  bins_Phi.clear();
  bins_Pt.clear();
  bins_Eta.push_back(-2.50);
  bins_Eta.push_back(-2.00);
  bins_Eta.push_back(-1.60);
  bins_Eta.push_back(-1.30);
  bins_Eta.push_back(-1.05);
  bins_Eta.push_back(-0.90);
  bins_Eta.push_back(-0.75);
  bins_Eta.push_back(-0.60);
  bins_Eta.push_back(-0.45);
  bins_Eta.push_back(-0.25);
  bins_Eta.push_back(-0.10);
  bins_Eta.push_back(0.10);
  bins_Eta.push_back(0.25);
  bins_Eta.push_back(0.45);
  bins_Eta.push_back(0.60);
  bins_Eta.push_back(0.75);
  bins_Eta.push_back(0.90);
  bins_Eta.push_back(1.05);
  bins_Eta.push_back(1.30);
  bins_Eta.push_back(1.60);
  bins_Eta.push_back(2.00);
  bins_Eta.push_back(2.50);
	
  axes.push_back(APMatrixAxis("Eta",bins_Eta));  

  bins_Phi.push_back(-180.00  * ap_Degree);
  bins_Phi.push_back(-165.00  * ap_Degree);
  bins_Phi.push_back(-150.00  * ap_Degree);
  bins_Phi.push_back(-135.00  * ap_Degree);
  bins_Phi.push_back(-120.00  * ap_Degree);
  bins_Phi.push_back(-105.00  * ap_Degree);
  bins_Phi.push_back(-90.000  * ap_Degree);
  bins_Phi.push_back(-75.000  * ap_Degree);
  bins_Phi.push_back(-60.000  * ap_Degree);
  bins_Phi.push_back(-45.000  * ap_Degree);
  bins_Phi.push_back(-30.000  * ap_Degree);
  bins_Phi.push_back(-15.000  * ap_Degree);
  bins_Phi.push_back(-0.0000  * ap_Degree);
  bins_Phi.push_back(+15.000  * ap_Degree);
  bins_Phi.push_back(+30.000  * ap_Degree);
  bins_Phi.push_back(+45.000  * ap_Degree);
  bins_Phi.push_back(+60.000  * ap_Degree);
  bins_Phi.push_back(+75.000  * ap_Degree);
  bins_Phi.push_back(+90.000  * ap_Degree);
  bins_Phi.push_back(+105.00  * ap_Degree);
  bins_Phi.push_back(+120.00  * ap_Degree);
  bins_Phi.push_back(+135.00  * ap_Degree);
  bins_Phi.push_back(+150.00  * ap_Degree);
  bins_Phi.push_back(+165.00  * ap_Degree);
  bins_Phi.push_back(+180.00  * ap_Degree);

  axes.push_back(APMatrixAxis("Phi",bins_Phi));  

  bins_Pt.push_back(20. * ap_GeV);
  bins_Pt.push_back(25. * ap_GeV);
  bins_Pt.push_back(30. * ap_GeV);
  bins_Pt.push_back(35. * ap_GeV);
  bins_Pt.push_back(40. * ap_GeV);
  bins_Pt.push_back(45. * ap_GeV);
  bins_Pt.push_back(50. * ap_GeV);
  bins_Pt.push_back(55. * ap_GeV);
  bins_Pt.push_back(60. * ap_GeV);
  bins_Pt.push_back(65. * ap_GeV);
  bins_Pt.push_back(70. * ap_GeV);
  bins_Pt.push_back(75. * ap_GeV);

  axes.push_back(APMatrixAxis("Pt",bins_Pt));  

  setupMatrix(axes);
}

// Copy constructor
APMuonEfficiencyMatrix::APMuonEfficiencyMatrix(const APMuonEfficiencyMatrix &ob) : APEfficiencyMatrix(ob)
{
  this->APEfficiencyMatrix::operator=(ob);
}

APMuonEfficiencyMatrix::~APMuonEfficiencyMatrix()
{
  APEfficiencyMatrix::clear();
}

// Overload =
void APMuonEfficiencyMatrix::operator = (const APMuonEfficiencyMatrix& ob)
{
  this->APEfficiencyMatrix::operator=(ob);
}

void APMuonEfficiencyMatrix::operator += (APMuonEfficiencyMatrix& ob)
{
  this->APEfficiencyMatrix::operator+=(ob);
}

bool APMuonEfficiencyMatrix::addTrial(TLorentzVector *vec, bool outcome)
{
  std::vector<double> v;
  v.push_back(vec->Eta());
  v.push_back(vec->Phi());
  v.push_back(vec->Pt());
  m_StatusCode = APEfficiencyMatrix::addTrial(v, outcome);
  return m_StatusCode;
}

double	APMuonEfficiencyMatrix::getEfficiency(TLorentzVector *vec)
{
  return getEfficiencyInformation(vec).efficiency();
}


double	APMuonEfficiencyMatrix::getEfficiencyUncertainty(TLorentzVector *vec)
{
  return getEfficiencyInformation(vec).uncertainty();
}


APEfficiencyEntry	APMuonEfficiencyMatrix::getEfficiencyInformation(TLorentzVector *vec)
{
  std::vector<double> v;
  v.push_back(vec->Eta());
  v.push_back(vec->Phi());
  v.push_back(vec->Pt());
  return APEfficiencyMatrix::getEfficiency(v);
}


APEfficiencyEntry	APMuonEfficiencyMatrix::getEfficiencyProjectionToEta(double eta)
{
  return projectTo("Eta").getBinContent(eta);
}


APEfficiencyEntry	APMuonEfficiencyMatrix::getEfficiencyProjectionToPhi(double phi)
{
  return projectTo("Phi").getBinContent(phi);
}


APEfficiencyEntry	APMuonEfficiencyMatrix::getEfficiencyProjectionToPt(double pt)
{
  return projectTo("Pt").getBinContent(pt);
}



bool	APMuonEfficiencyMatrix::plotEfficiencyProjectionToEta(TH1F *hist)
{
  plotEfficiencyProjectionTo("Eta", hist);
  return getStatusCode();
}


bool	APMuonEfficiencyMatrix::plotEfficiencyProjectionToPhi(TH1F *hist)
{
  plotEfficiencyProjectionTo("Phi", hist);
  return getStatusCode();
}


bool	APMuonEfficiencyMatrix::plotEfficiencyProjectionToPt(TH1F *hist)
{
  plotEfficiencyProjectionTo("Pt", hist);
  return getStatusCode();
}

