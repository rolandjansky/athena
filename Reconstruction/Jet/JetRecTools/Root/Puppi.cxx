#include "Math/ProbFunc.h"
#include "TMath.h"
#include "fastjet/Selector.hh"

#include "JetRecTools/Puppi.h"

using namespace std;
using namespace fastjet;

PuppiUserInfo::PuppiUserInfo(): otherChi2Vec(std::vector<double>()) {}
PuppiUserInfo::PuppiUserInfo(std::vector<double> v): otherChi2Vec(v) {}
PuppiUserInfo::PuppiUserInfo(double v): otherChi2Vec(std::vector<double>({v})) {}

//=================================================================================================================================

Puppi::Puppi(double R0, double Rmin, double beta, double centralPTCutOffset, double centralPTCutSlope, double forwardPTCutOffset, double forwardPTCutSlope, double etaBoundary, bool PUPenalty): 
	m_R0(R0), m_Rmin(Rmin), m_beta(beta), m_centralPTCutOffset(centralPTCutOffset), m_centralPTCutSlope(centralPTCutSlope), m_forwardPTCutOffset(forwardPTCutOffset), m_forwardPTCutSlope(forwardPTCutSlope), m_etaBoundary(etaBoundary), m_PUPenalty(PUPenalty)
{}

//---------------------------------------------------------------------------------------------------------------------------------

//Sets the particles used for calculations, and calculates the median and RMS for the PU distribution
void Puppi::setParticles(const std::vector<fastjet::PseudoJet> chargedHS, const std::vector<fastjet::PseudoJet> chargedPU, const std::vector<fastjet::PseudoJet> neutral, const std::vector<fastjet::PseudoJet> forward, int nPU){

	m_chargedHS=chargedHS;
	m_chargedPU=chargedPU;
	m_neutral=neutral;
	m_forward=forward;
	
	m_nPU=nPU;

	m_allParticles = {&m_chargedHS, &m_chargedPU, &m_neutral, &m_forward};
	
	findAlphaMedianAndRMS();
}

//---------------------------------------------------------------------------------------------------------------------------------

double Puppi::getChi2(const PseudoJet& pfo){
	double chi=(getAlpha(pfo)-m_median)/m_rms;
	return chi*fabs(chi);
}

//---------------------------------------------------------------------------------------------------------------------------------

double Puppi::getWeight(PseudoJet pfo){

	double chi2Total=getChi2(pfo);
	int nDF=1;

	if(pfo.has_user_info<PuppiUserInfo>()){
	  //This is untested!!!!
		//Need to think about how to handle sign of chi2 - for now just throw it away
		chi2Total=fabs(chi2Total);
		for(auto v: pfo.user_info<PuppiUserInfo>().otherChi2Vec) {
			chi2Total+=v;
			nDF+=1;
		}
	}

	double w =  ROOT::Math::chisquared_cdf(chi2Total, nDF);

	double offset,slope;
	if( fabs(pfo.eta()) < m_etaBoundary) {
	offset=m_centralPTCutOffset;
	slope=m_centralPTCutSlope;
	}
	else {
	offset=m_forwardPTCutOffset;
	slope=m_forwardPTCutSlope;
	}

	w*=(pfo.pt()>offset+m_nPU*slope);

	return w;
}

//---------------------------------------------------------------------------------------------------------------------------------

double Puppi::getAlpha(const PseudoJet pfo){
	fastjet::Selector sel = fastjet::SelectorCircle(m_R0);
	sel.set_reference(pfo);

	double sum=0;
	int nNeighbors=0;

	if (fabs(pfo.eta())<m_etaBoundary+m_R0){
		vector<PseudoJet> chargedHSNeighbors = sel(m_chargedHS);
		for (auto p: chargedHSNeighbors){
			float dR=pfo.delta_R(p);
			if (dR>m_Rmin){
			  sum+=p.pt()/pow(dR, m_beta);
				nNeighbors+=1;
			}
		}
	}

	if (m_includeCentralNeutralsInAlpha){
		if (fabs(pfo.eta())<m_etaBoundary+m_R0){
			vector<PseudoJet> neutralNeighbors = sel(m_neutral);
			for (auto p: neutralNeighbors){
				float dR=pfo.delta_R(p);
				if (dR>m_Rmin){
					sum+=pow(p.pt()/dR, m_beta);
					nNeighbors+=1;
				}
			}
		}
	}

	if (m_PUPenalty){
		if (fabs(pfo.eta())<m_etaBoundary+m_R0){
			vector<PseudoJet> chargedPUNeighbors = sel(m_chargedPU);
			for (auto p: chargedPUNeighbors){
				float dR=pfo.delta_R(p);
				if (dR>m_Rmin){
					sum-=pow(p.pt()/dR, m_beta);  //subtract from alpha rather than add
					nNeighbors+=1;
				}
			}
		}
	}

	if (fabs(pfo.eta())>m_etaBoundary-m_R0){
		vector<PseudoJet> forwardNeighbors = sel(m_forward);
		for (auto p: forwardNeighbors){
			float dR=pfo.delta_R(p);
			if (dR>m_Rmin){
				sum+=pow(p.pt()/dR, m_beta);
				nNeighbors+=1;
			}
		}
	}

	if (sum<=FLT_MIN) return -99999;
	if (nNeighbors!=0) return log(sum);
	return -9999;
}

//---------------------------------------------------------------------------------------------------------------------------------

//Finds the median and LHS RMS for the charged PU distribution (done each event)
void Puppi::findAlphaMedianAndRMS(){
	vector<double> values;

	for(auto p: m_chargedPU){

		// Don't want to include particles on the boundary
		if( fabs(p.eta()) > m_etaBoundary-m_R0) continue;

		double value = getAlpha(p);
		if(value > -999) values.push_back(value);  //-9999 is the value assigned to pfo with zero neighbors within cone
	}

	std::sort(values.begin(),values.end());
			
	if(values.size()>0) m_median=values[int(values.size()*0.5)];
	else m_median=-9999;

	// now compute the LHS RMS
	double sum=0;
	int n = 0;
	for(auto value: values){
		if (value - m_median > 0) continue;
		sum += (value - m_median)*(value - m_median);
		++n;
	}

	if(n > 0) m_rms = TMath::Sqrt(sum/n);
	else m_rms=-9999;
}

double Puppi::getMedian(){
	return m_median;
}
double Puppi::getRMS(){
	return m_rms;
}
