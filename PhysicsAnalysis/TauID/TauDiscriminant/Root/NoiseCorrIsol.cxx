/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*******************************************************//
// Name: NoiseCorrIsol.cxx                               //
// Author: Michel Trottier-McDonald <mtm@cern.ch>        //
// Description: A class to provide isolation based       //
// pileup/underlying event correction to the clusters    //
// of tau decays                                         //
//*******************************************************//

#include "TauDiscriminant/NoiseCorrIsol.h"
#include "math.h"
#include <iostream>

//---------------------------------------------------------
// Default Constructor
//---------------------------------------------------------
NoiseCorrIsol::NoiseCorrIsol() : m_innerDR(0), m_outerDR(0), m_areaRatio(0), m_eff(0)
{}


//---------------------------------------------------------
// Main constructor 1
//---------------------------------------------------------
NoiseCorrIsol::NoiseCorrIsol(const TLorentzVector& tau,
			     const std::vector<TLorentzVector>& clusters,
			     double innerDR,
			     double outerDR,
			     bool eff)
    : m_clusters(clusters),
      m_tau(tau),
      m_innerDR(innerDR),
      m_outerDR(outerDR),
      m_eff(eff)
{
    m_areaRatio = areaRatio();
}


//---------------------------------------------------------
// Main constructor 2
//---------------------------------------------------------
NoiseCorrIsol::NoiseCorrIsol(const TLorentzVector& tau,
			     const std::vector<EMFCluster>& clusters,
			     double innerDR,
			     double outerDR,
			     bool eff)
  : m_EMFClusters(clusters),
    m_tau(tau),
    m_innerDR(innerDR),
    m_outerDR(outerDR),
    m_eff(eff)
{
    m_areaRatio = areaRatio();
}


//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
NoiseCorrIsol::~NoiseCorrIsol()
{}


//---------------------------------------------------------
// Ratio of areas calculation
//---------------------------------------------------------
double NoiseCorrIsol::areaRatio()
{
    double delta = m_outerDR*m_outerDR - m_innerDR*m_innerDR;
    if(delta != 0)	
    	return (m_innerDR*m_innerDR)/(delta);

    else return 99999999.0;
}


//---------------------------------------------------------
// Correcting clusters (pure TLorentzVectors)
//---------------------------------------------------------
std::vector<TLorentzVector> NoiseCorrIsol::correctedClustersTLV()
{

    // Calculate correction
    std::vector<TLorentzVector> innerClusters;
    int nCl = (int)m_clusters.size();
    double outerPt = 0;

    //Obtain isolation ring energy (outerE) and inner clusters
    for(int i = 0; i < nCl; ++i)
    {
	double dR = m_tau.DeltaR(m_clusters.at(i));

	if( (dR < m_outerDR)&&(dR > m_innerDR) )
	    outerPt += m_clusters.at(i).Pt();

	if( dR < m_innerDR )
	    innerClusters.push_back(m_clusters.at(i));
    }

    int nInCl = (int)innerClusters.size();
    
    double correction = 0.0;
    if(nInCl != 0) correction = (outerPt * m_areaRatio)  / nInCl;
    std::vector<TLorentzVector> finalClusters;

    //Apply correction
    for(int i = 0; i < nInCl; ++i)
    {
	double correctedPt = innerClusters.at(i).Pt() - correction;
	double Eta         = innerClusters.at(i).Eta();
	double Phi         = innerClusters.at(i).Phi();

	if(correctedPt > 0) //Reject clusters with smaller E than the correction E
	{
	    TLorentzVector corrCluster;
	    corrCluster.SetPtEtaPhiM(correctedPt, Eta, Phi, 0);
	    finalClusters.push_back(corrCluster);
	}
    }

    return finalClusters;
}



//---------------------------------------------------------
// Correcting clusters (EMFClusters)
//---------------------------------------------------------
std::vector<EMFCluster> NoiseCorrIsol::correctedClusters()
{

    // Calculate correction
    std::vector<EMFCluster> innerClusters;
    int nCl = (int)m_EMFClusters.size();
    double outerPt = 0;

    //Obtain isolation ring energy (outerE) and inner clusters
    for(int i = 0; i < nCl; ++i)
    {
	double dR = m_tau.DeltaR(m_EMFClusters.at(i).TLV());

	if( (dR < m_outerDR)&&(dR > m_innerDR) )
	    outerPt += m_EMFClusters.at(i).TLV().Pt();

	if( dR < m_innerDR )
	    innerClusters.push_back(m_EMFClusters.at(i));
    }

    int nInCl = (int)innerClusters.size();
    double correction = 0.0;
    if(nInCl>0) correction = (outerPt * m_areaRatio)  / nInCl;
    std::vector<EMFCluster> finalClusters;

    //Apply correction
    for(int i = 0; i < nInCl; ++i)
    {
	double correctedPt = innerClusters.at(i).TLV().Pt() - correction;
	double Eta         = innerClusters.at(i).TLV().Eta();
	double Phi         = innerClusters.at(i).TLV().Phi();

	if(correctedPt > 0) //Reject clusters with smaller E than the correction E
	{
	    TLorentzVector corrCluster;
	    corrCluster.SetPtEtaPhiM(correctedPt, Eta, Phi, 0);
	    EMFCluster corrEMFCluster(corrCluster, innerClusters.at(i).PSSF(), innerClusters.at(i).EM2F(), innerClusters.at(i).EM3F());
	    finalClusters.push_back(corrEMFCluster);
	}
    }

    if(m_eff) return effClusters(finalClusters);
    return finalClusters;
}


//---------------------------------------------------------
// Calculating effective clusters (post-correction)
//---------------------------------------------------------
bool EComp(const EMFCluster& iCluster, const EMFCluster& jCluster)
{ return (iCluster.TLV().E() > jCluster.TLV().E()); }

std::vector<EMFCluster> NoiseCorrIsol::effClusters(const std::vector<EMFCluster>& inputClusters)
{
    int nInputClusters = (int)inputClusters.size();
    std::vector<EMFCluster> inClusters = inputClusters;

    double SumSquare = 0;
    double SquareSum = 0;

    for(int i = 0; i < nInputClusters; ++i)
    {
	double E = inClusters.at(i).TLV().E();
	SquareSum += E;
	SumSquare += E*E;
    }

    double NEff = 0;
    if(SumSquare != 0.0) NEff = ceil((SquareSum*SquareSum)/SumSquare);

    std::sort(inClusters.begin(), inClusters.end(), EComp);

    std::vector<EMFCluster> outputClusters;

    for(int i = 0; i < NEff; ++i)
	outputClusters.push_back(inClusters.at(i));

    return outputClusters;
}
