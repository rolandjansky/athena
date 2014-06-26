/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*******************************************************//
// Name: Pi0Finder.cxx                                   //
// Author: Michel Trottier-McDonald <mtm@cern.ch>        //
// Description: Main class to find which clusters are    //
// the pi0s in tau decays involving neutral pions        //
//*******************************************************//

#include "TauDiscriminant/Pi0Finder.h"
#include "TauDiscriminant/NoiseCorrIsol.h"
#include "math.h"
#include <iostream>


//---------------------------------------------------------
// Default Constructor
//---------------------------------------------------------
Pi0Finder::Pi0Finder()
{}



//---------------------------------------------------------
// Constructor 1
//---------------------------------------------------------
Pi0Finder::Pi0Finder(const std::vector<TLorentzVector>& tracks,
		     const std::vector<EMFCluster>& clusters,
		     bool twoPi0s,
		     double resImportance, 
		     double turnOnPoint,
		     double turnOnRate,
		     double PSSFactor,
		     double EM2Factor,
		     double twoPi0Strength,
		     bool usePseudoHADF,
		     bool effClusters  ) : m_tracks(tracks),
    m_usePseudoHADF(usePseudoHADF),
    m_twoPi0s(twoPi0s),
    m_resImportance(resImportance),
    m_turnOnPoint(turnOnPoint),
    m_turnOnRate(turnOnRate),
    m_PSSFactor(PSSFactor),
    m_EM2Factor(EM2Factor),
    m_twoPi0Strength(twoPi0Strength),
    m_caloE(0),
    m_caloHADE(0),
    m_trkE(0),
    m_trkHADF(0),
    m_doubleCountingE(0),
    m_applyCorrCluster1(false),
    m_applyCorrCluster2(false),
    m_keepCluster1(false),
    m_keepCluster2(false),
    m_noMatch(true),
    m_selEMFCluster1(0.000001),
    m_selEMFCluster2(0.000001),
    m_selEMFClusterNotCorr1(0.000001),
    m_selEMFClusterNotCorr2(0.000001)
{
    TLorentzVector trksys;
    for(unsigned int i = 0; i < m_tracks.size(); ++i)
	trksys += m_tracks[i];

    NoiseCorrIsol NCI(trksys, clusters, 0.3, 0.4, effClusters);
    m_EMFClusters = NCI.correctedClusters();

    execute();
}


//---------------------------------------------------------
// Constructor 2
//---------------------------------------------------------
Pi0Finder::Pi0Finder(const std::vector<TLorentzVector>& tracks,
		     const std::vector<TLorentzVector>& clusters,
		     const std::vector<float>& PSSFs,
		     const std::vector<float>& EM2Fs,
		     const std::vector<float>& EM3Fs,
		     bool twoPi0s,
		     double resImportance, 
		     double turnOnPoint,
		     double turnOnRate,
		     double PSSFactor,
		     double EM2Factor,
		     double twoPi0Strength,
		     bool usePseudoHADF,
		     bool effClusters  ) : m_tracks(tracks),
    m_usePseudoHADF(usePseudoHADF),
    m_twoPi0s(twoPi0s),
    m_resImportance(resImportance),
    m_turnOnPoint(turnOnPoint),
    m_turnOnRate(turnOnRate),
    m_PSSFactor(PSSFactor),
    m_EM2Factor(EM2Factor),
    m_twoPi0Strength(twoPi0Strength),
    m_caloE(0),
    m_caloHADE(0),
    m_trkE(0),
    m_trkHADF(0),
    m_doubleCountingE(0),
    m_applyCorrCluster1(false),
    m_applyCorrCluster2(false),
    m_keepCluster1(false),
    m_keepCluster2(false),
    m_noMatch(true),
    m_selEMFCluster1(0.000001),
    m_selEMFCluster2(0.000001),
    m_selEMFClusterNotCorr1(0.000001),
    m_selEMFClusterNotCorr2(0.000001)
{
    std::vector<EMFCluster> tmpEMFClusters = convertToEMFClusters(clusters, PSSFs, EM2Fs, EM3Fs);
    for(unsigned int i = 0; i < m_tracks.size(); ++i)
	m_trkSys += m_tracks[i];
    NoiseCorrIsol NCI(m_trkSys, tmpEMFClusters, 0.3, 0.4, effClusters);
    m_EMFClusters = NCI.correctedClusters();
    execute();
}


//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
Pi0Finder::~Pi0Finder()
{}


//---------------------------------------------------------
// Execute the pi0 finding
//---------------------------------------------------------
void Pi0Finder::execute()
{
    preSelParameters();
    if(m_twoPi0s) select2();
    else select();

    postSelParameters();

    if(m_keepCluster1)
    {
	if(m_applyCorrCluster1) m_selEMFCluster1 = correct(m_selEMFClusterNotCorr1);
	else m_selEMFCluster1 = m_selEMFClusterNotCorr1;
    }

    if(m_keepCluster2)
    {
	if(m_applyCorrCluster2) m_selEMFCluster2 = correct(m_selEMFClusterNotCorr2);
	else m_selEMFCluster2 = m_selEMFClusterNotCorr2;
    }
}


//---------------------------------------------------------
// Parameters to be used in the selection
//---------------------------------------------------------
void Pi0Finder::preSelParameters()
{
    //Build the track system
    int ntracks = (int)m_tracks.size();

    m_trkE = 0;

    for(int i = 0; i < ntracks; ++i)
	m_trkE += m_tracks.at(i).E();

    //Calculate calo quantities
    int nclusters = (int)m_EMFClusters.size();
    m_caloE = 0;
    m_caloHADE = 0;

    for(int i = 0; i < nclusters; ++i)
    {
	double clE    = m_EMFClusters.at(i).TLV().E();
	double clHADE = m_EMFClusters.at(i).pseudoHADE();
	if(!m_usePseudoHADF)
	    clHADE = m_EMFClusters.at(i).HADE();

	m_caloE    += clE;
	m_caloHADE += clHADE;
    }
    if(m_trkE != 0) m_trkHADF = m_caloHADE/m_trkE;
}


//---------------------------------------------------------
// Calculate scores and select cluster with highest score
//---------------------------------------------------------
void Pi0Finder::select()
{
    double pi0ScoreMin  = 0.0;
    EMFCluster cl1;
    int nclusters = (int)m_EMFClusters.size();

    for(int i = 0; i < nclusters; ++i)
    {
	double clE      = m_EMFClusters.at(i).TLV().E();
	double HADF     = m_EMFClusters.at(i).HADF();
	double rawRecoE = m_caloE - m_trkE;
	double rawERes  = 999999.0;
	if(rawRecoE != 0) rawERes = sqrt(fabs(clE/rawRecoE - 1.0));
	double PSSE     = m_EMFClusters.at(i).PSSE();
	double pi0Score = PSSE/(HADF + m_resImportance*rawERes + 0.0000001);
	if(pi0Score > pi0ScoreMin)
	{
	    pi0ScoreMin = pi0Score;
	    cl1 = m_EMFClusters.at(i);
	    m_noMatch = false;
	}
    }
    if(!m_noMatch)
	m_selEMFClusterNotCorr1 = cl1;
}



//---------------------------------------------------------
// Calculate scores and select pair of clusters with
// highest score
//---------------------------------------------------------
void Pi0Finder::select2()
{
    double pi0ScoreMin  = 0.0;
    EMFCluster cl1;
    EMFCluster cl2;

    int nclusters = (int)m_EMFClusters.size();

    for(int i = 0; i < nclusters; ++i)
    {

	double clE1      = m_EMFClusters.at(i).TLV().E();
	double clEta1    = m_EMFClusters.at(i).TLV().Eta();
	double HADF1     = m_EMFClusters.at(i).HADF();
	double rawRecoE1 = m_caloE - m_trkE;
	double rawERes1 = 9999999.0;
	if(rawRecoE1 != 0) rawERes1  = sqrt(fabs(clE1/rawRecoE1 - 1.0));
	double PSSE1     = m_EMFClusters.at(i).PSSE();

	double pi0Score1 = PSSE1/(HADF1 + m_resImportance*rawERes1 + 0.0000001);

	if((pi0Score1 > pi0ScoreMin) && (fabs(clEta1) < 5.0))
	{
	    pi0ScoreMin = pi0Score1;
	    cl1 = m_EMFClusters.at(i);
	    TLorentzVector empty;
	    EMFCluster EMFempty(empty, 0, 0, 0);
	    cl2 = EMFempty;
	    m_noMatch = false;
	    m_keepCluster1 = true;
	    m_keepCluster2 = false;
	}

	for(int j = 0; j < nclusters; ++j)
	{
	    if(j > i) // Do not pair a cluster with itself
	    {
		double clE_i    = m_EMFClusters.at(i).TLV().E();
		double clE_j    = m_EMFClusters.at(j).TLV().E();
		double clEta_i  = m_EMFClusters.at(i).TLV().Eta();
		double clEta_j  = m_EMFClusters.at(j).TLV().Eta();
		double HADE_i   = m_EMFClusters.at(i).HADE();
		double HADE_j   = m_EMFClusters.at(j).HADE();
		double clE      = clE_i + clE_j;
		double HADF     = 0.0;
		if(clE != 0) HADF = (HADE_i + HADE_j)/clE;
		double rawRecoE = m_caloE - m_trkE;
		double rawERes  = 999999.0;
		if(rawRecoE != 0) rawERes = sqrt(fabs(clE/rawRecoE - 1.0));
		double PSSE_i   = m_EMFClusters.at(i).PSSE();
		double PSSE_j   = m_EMFClusters.at(j).PSSE();
		double PSSE     = PSSE_i + PSSE_j;
		
		double pi0Score = m_twoPi0Strength*PSSE/(HADF + m_resImportance*rawERes + 0.0000001);

		if((pi0Score > pi0ScoreMin) &&(fabs(clEta_i) < 5.0 && fabs(clEta_j) < 5.0))
		{
		    pi0ScoreMin = pi0Score;
		    cl1 = m_EMFClusters.at(i);
		    cl2 = m_EMFClusters.at(j);
		    m_noMatch = false;
		    m_keepCluster1 = true;
		    m_keepCluster2 = true;
		}
	    }
	}
    }
    if(!m_noMatch)
    {
	m_selEMFClusterNotCorr1 = cl1;
	m_selEMFClusterNotCorr2 = cl2;
    }
}




//---------------------------------------------------------
// Parameters to be used in the correction
//---------------------------------------------------------
void Pi0Finder::postSelParameters()
{
    double clE1 = 0.0;
    double clEta1 = 10.0;
    double clE2 = 0.0;
    double clEta2 = 10.0;

    if(m_keepCluster1)
    {
	clE1 = m_selEMFClusterNotCorr1.TLV().E();
	clEta1 = m_selEMFClusterNotCorr1.TLV().Eta();
    }

    if(m_keepCluster2)
    {
	clE2 = m_selEMFClusterNotCorr2.TLV().E();
	clEta2 = m_selEMFClusterNotCorr2.TLV().Eta();
    }


    double DCTwoClusters = 1.0;
    double DCCluster1 = 1.0;
    double DCCluster2 = 1.0;
       
    if(m_caloE > 0.0)
    {
        DCTwoClusters = (clE1 + clE2 + m_trkE)/m_caloE;
	DCCluster1    = (clE1 + m_trkE)/m_caloE;
	DCCluster2    = (clE2 + m_trkE)/m_caloE;
	
    }

    bool TwoClusterCorrection = (DCTwoClusters > m_turnOnPoint);
    bool Cluster1Correction = (DCCluster1 > m_turnOnPoint);
    bool Cluster2Correction = (DCCluster1 > m_turnOnPoint);

    if((clE1 > 0.0 && clEta1 < 5.0) && (clE2 > 0.0 && clEta2 < 5.0))
    {
	m_doubleCountingE = DCTwoClusters;
	
	m_keepCluster1 = true;
	m_keepCluster2 = true;

	if(TwoClusterCorrection)
	{
	    if(Cluster1Correction) m_applyCorrCluster1 = true;
	    if(Cluster2Correction) m_applyCorrCluster2 = true;

	    if(!Cluster1Correction && !Cluster2Correction)
	    {
		if(DCCluster1 > DCCluster2) m_applyCorrCluster1 = true;
		else m_applyCorrCluster2 = true;
	    }
	}
    }
 
    else if((clE1 == 0.0 || clEta1 > 5.0) && (clE2 > 0.0 && clEta2 < 5.0))
    {
	m_doubleCountingE = DCCluster2;
	m_keepCluster1 = false;
	m_keepCluster2 = true;
	if(Cluster2Correction) m_applyCorrCluster2 = true;
    }

    else if((clE2 == 0.0 || clEta2 > 5.0) && (clE1 > 0.0 && clEta1 < 5.0))
    {
	m_doubleCountingE = DCCluster1;
	m_keepCluster1 = true;
	m_keepCluster2 = false;
	if(Cluster1Correction) m_applyCorrCluster1 = true;
    }

    else
    {
	m_doubleCountingE = 0;
	m_applyCorrCluster1 = false;
	m_applyCorrCluster2 = false;
	m_keepCluster1 = false;
	m_keepCluster2 = false;
	m_noMatch = true;
	
    }
}


//---------------------------------------------------------
// Contamination Correction
//---------------------------------------------------------
EMFCluster Pi0Finder::correct(const EMFCluster& cl)
{
    //Start by extracting the EMFCluster since a new one will have to be built

    double E    = cl.TLV().E();
    double Eta  = cl.TLV().Eta();
    double Phi  = cl.TLV().Phi();
    double M    = cl.TLV().M();
    double PSSF = cl.PSSF();
    double EM2F = cl.EM2F();
    double EM3F = cl.EM3F();
    double HADE = cl.HADE();
//    double HADF = cl.HADF();

    if(m_usePseudoHADF)
    {
	HADE = cl.pseudoHADE();
//	HADF = cl.pseudoHADF();
    }
    double PSSE = cl.PSSE();
    double EM2E = cl.EM2E();

    double ChPiContaminationE = m_trkE;
    if(m_trkHADF > 0.0) ChPiContaminationE = HADE/m_trkHADF;
    double corrE  = (E - ChPiContaminationE);
    double corrE2 = 0.0;
    if(m_doubleCountingE > 0.0) corrE2 =  (m_PSSFactor*PSSE + m_EM2Factor*EM2E)/m_doubleCountingE;

    double corrScale = -exp(m_turnOnRate*(m_turnOnPoint - m_doubleCountingE)) + 1;

    //If there is substantial hadronic energy, use trkHADF
    TLorentzVector newCl;
    double effE = E;
    if(m_caloHADE > 0.0) effE = (1 - corrScale)*E + corrScale*((1-HADE/m_caloHADE)*corrE2 + (HADE/m_caloHADE)*corrE);

    //Correct direction
    double corrPt = effE/cosh(Eta);

    newCl.SetPtEtaPhiM(corrPt, Eta, Phi, M);

    EMFCluster newEMFCl(newCl, PSSF, EM2F, EM3F);

    return newEMFCl;
}


//---------------------------------------------------------
// give pi0 Mass
//---------------------------------------------------------
EMFCluster Pi0Finder::giveMass(const EMFCluster& cl)
{
    double Pt  = cl.TLV().Pt();
    double Eta  = cl.TLV().Eta();
    double Phi  = cl.TLV().Phi();
    double PSSF = cl.PSSF();
    double EM2F = cl.EM2F();
    double EM3F = cl.EM3F();
    double mass = 135;

    TLorentzVector newTLV;
    newTLV.SetPtEtaPhiM(Pt, Eta, Phi, mass);
    return EMFCluster(newTLV, PSSF, EM2F, EM3F);
}


//---------------------------------------------------------
// Return reconstructed visible tau
//---------------------------------------------------------
TLorentzVector Pi0Finder::visTauTLV() const
{
    int ntracks = (int)m_tracks.size();
    TLorentzVector trksys;

    for(int i = 0; i < ntracks; ++i)
    {
	double Pt  = m_tracks.at(i).Pt();
	double Eta = m_tracks.at(i).Eta();
	double Phi = m_tracks.at(i).Phi();

	TLorentzVector trk;
	trk.SetPtEtaPhiM(Pt, Eta, Phi, 140);
	trksys += trk;
    }	    

    TLorentzVector visTau = trksys;
    if(!m_noMatch)
    {
	TLorentzVector pi0s;

	if(m_twoPi0s)
	{
	    double pi0sPt =  m_selEMFCluster1.TLV().Pt() +  m_selEMFCluster2.TLV().Pt();
	    TLorentzVector pi0sdirection = m_selEMFCluster1.TLV() +  m_selEMFCluster2.TLV();
	    double pi0sEta = pi0sdirection.Eta();
	    double pi0sPhi = pi0sdirection.Phi();


	    pi0s.SetPtEtaPhiM(pi0sPt, pi0sEta, pi0sPhi, 135);
	}
	else
	    pi0s.SetPtEtaPhiM(m_selEMFCluster1.TLV().Pt(), m_selEMFCluster1.TLV().Eta(), m_selEMFCluster1.TLV().Phi(), 135);
	    
	visTau += pi0s;

    }

    return visTau;
}


//---------------------------------------------------------
// Converts the 4 std::vectors to 1 EMFCluster std::vector
//---------------------------------------------------------

std::vector<EMFCluster> Pi0Finder::convertToEMFClusters(const std::vector<TLorentzVector>& clusters,
							const std::vector<float>& PSSFs,
							const std::vector<float>& EM2Fs,
							const std::vector<float>& EM3Fs)
{
    int clsize   = (int)clusters.size();
    int pssfsize = (int)PSSFs.size();
    int em2fsize = (int)EM2Fs.size();
    int em3fsize = (int)EM3Fs.size();

    std::vector<EMFCluster> emfclusters;

    if(clsize == pssfsize && pssfsize == em2fsize && em2fsize == em3fsize)
    {
	for(int i = 0; i < clsize; ++i)
	{
	    EMFCluster emfcl(clusters.at(i), PSSFs.at(i), EM2Fs.at(i), EM3Fs.at(i));
	    emfclusters.push_back(emfcl);
	}
    }

    return emfclusters;
}
