/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * IpHistos.cxx 
 *
 * Stephan.Sandvoss@cern.ch
 */

/**
 * intra-package includes
 */
#include "ImpactParameterTuningUtils/IpHistos.h"
//#include "TrackSelector/eventChannel.h"

/**
 * extra-package includes
 */
#include "GaudiKernel/MsgStream.h"


/**
 * system includes
 */
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TROOT.h"
#include "TStyle.h"
#include <cassert>
#include <sstream> 


/**
 * Constructor
 * z0 relative to 0,0,0 goes from -100..+100, relative to PV from -1..+1
 */
IpHistos::IpHistos(MsgStream &msg, TFile *file) :
		m_nJetAlgos(8), m_nIpSig(2), m_nTransLong(2),
		m_separationChar("_"), m_plus("+"),
		m_histoBins(100), m_lowerIpTrans(-1.0), m_upperIpTrans(+1.0),
		m_lowerIpLong(-1.0), m_upperIpLong(+1.0),
		m_lowerIpSig(-40.0), m_upperIpSig(+40.0),
		m_zero(0),
		m_titleIpLong("Longitudinal Impact Parameters"),
		m_titleIpSigLong(
		"Longitudinal Impact Parameter Significance"),
		m_titleIpTrans("Transversal Impact Parameters"),
		m_titleIpSigTrans(
		"Transversal Impact Parameter Significance"),
		m_xAxisTitleIp("IP [mm]"),
		m_xAxisTitleIpSig("IPSig=IP/sigma(IP) [1]"),
		m_yAxisTitleAbsolute("number of entries [1]"),
		m_yAxisTitleNormalized("normalized frequency [1]"),
		m_absolute("Absolute"), m_normalized("Normalized"),
		m_lin("Lin"), m_loga("Log"), m_ps(".ps"), m_eps(".eps"),
		m_histoLineColor(1), m_lineStyle(1), m_lineWidth(3)
		// the number of eta and pt bins is set in m_initNumbersStrings
		//m_nEta(5), m_nPt(5)
{
	m_msg = &msg;
	// if only root without athena/histservice is used:
	if(file) {
		m_created = false;
	} else {
		m_created = false;
	}
	m_initNumbersStrings();
	m_initFunctions();
	m_histos = new MultiArray(boost::extents
			[(Index)m_nJetAlgos][(Index)m_nIpSig]
			[(Index)m_nTransLong][(Index)m_nEta][(Index)m_nPt]);
}

/**
 * Destructor
 */
IpHistos::~IpHistos()
{
	delete m_negative2GaussIpTrans;
	m_negative2GaussIpTrans = 0;
	delete m_negative2GaussIpLong;
	m_negative2GaussIpLong = 0;
	delete m_negative2GaussIpSig;
	m_negative2GaussIpSig = 0;
	delete m_positive1Gauss1ExpIpTrans;
	m_positive1Gauss1ExpIpTrans = 0;
	delete m_positive1Gauss1ExpIpLong;
	m_positive1Gauss1ExpIpLong = 0;
	delete m_positive1Gauss1ExpIpSig;
	m_positive1Gauss1ExpIpSig = 0;
	// bug: THistSvc saves the histo AFTER destruction of
	// the algorithm -> do not delete m_histos here ! otherwise 
	// nothing is saved to output file and athena crashes at 
	// THistSvc::finalize
	// delete only histograms if they were created, not when they were
	// loaded, e.g. from cooldb
	//if(m_created) {
	//	m_deleteHistos();
	//}
}

/**
 * get file name for ITHistService
 */
std::string IpHistos::getHistServiceFile()
{
	return m_histServiceFile;
}

/**
 * get directory name for ITHistService
 */
std::string IpHistos::getHistServiceDirectory()
{
	return m_histServiceDirectory;
}

/**
 * get file and directory name for ITHistService
 */
std::string IpHistos::getHistServiceFullString()
{
	return (std::string)
			("/"+m_histServiceFile+"/"+m_histServiceDirectory);
}

/**
 * set file name for ITHistService
 */
void IpHistos::setHistServiceFile(std::string s)
{
	m_histServiceFile = s;
	return;
}

/**
 * set directory name for ITHistService
 */
void IpHistos::setHistServiceDirectory(std::string s)
{
	m_histServiceDirectory = s;
	return;
}

/**
 * get number of histograms for jet algorithms
 */
unsigned int IpHistos::getNJetAlgos()
{
	return m_nJetAlgos;
}

/**
 * get number of histogram of default jet algorithm Cone4Tower
 */
unsigned int IpHistos::getDefaultJetAlgo()
{
	return m_defaultJetAlgo;
}

/**
 * get number of histograms for ip/ipsig (==2)
 */
unsigned int IpHistos::getNIpSig()
{
	return m_nIpSig;
}

/**
 * get number of histograms for direction (transverse/longitudinal; ==2)
 */
unsigned int IpHistos::getNTransLong()
{
	return m_nTransLong;
}

/**
 * get number of histograms for track eta bins
 */
unsigned int IpHistos::getNEta()
{
	return m_nEta;
}

/**
 * get number of histograms for track pt bins
 */
unsigned int IpHistos::getNPt()
{
	return m_nPt;
}

/**
 * provide the bin for a certain eta
 */
int IpHistos::getBinEta(double eta)
{
	if(eta < m_etaBorders[0] ||
			eta > m_etaBorders[m_etaBorders.size() - 1 ] ) {
		return -1;
	}
	for(unsigned int i = 0; i < m_etaBorders.size(); i++) {
		if(eta > m_etaBorders[i] ) {
			continue;
		} else {
			return i-1;
		}
	}
	return m_etaBorders.size() - 1;
}

/**
 * provide the bin for a certain pt
 */
int IpHistos::getBinPt(double pt)
{
	if(pt < m_ptBorders[0] ||
			pt > m_ptBorders[m_ptBorders.size() - 1 ] ) {
		return -1;
	}
	for(unsigned int i = 0; i < m_ptBorders.size(); i++) {
		if(pt > m_ptBorders[i] ) {
			continue;
		} else {
			return i-1;
		}
	}
	return m_ptBorders.size() - 1;
}

/**
 * if reading from root file, set histogram pointers
 */
void IpHistos::setHisto(TH1 *histo, unsigned int jetAlgo,
		unsigned int ipSig, unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	if( (*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
			[(Index)eta][(Index)pt] != 0) {
		delete (*m_histos)[(Index)jetAlgo][(Index)ipSig]
				[(Index)transLong][(Index)eta][(Index)pt];
		(*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt] = 0;
		(*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt] = histo;
	} else {
		(*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt] = histo;
	}
	m_created = false;
	return;
}

/**
 * for saving via ITHistService get histogram pointers
 */
TH1* IpHistos::getHisto(unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta, unsigned int pt)
{
	return (*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
			[(Index)eta][(Index)pt];
}

/**
 * provide number of bins of the TH1
 */
unsigned int IpHistos::getNHistoBins()
{
	return m_histoBins;
}

/**
 * provide lower border of the Ip TH1 for transverse ip
 */
double IpHistos::getLowerBorderIpTransHisto()
{
	return m_lowerIpTrans;
}

/**
 * provide upper border of the Ip TH1 for transverse ip
 */
double IpHistos::getUpperBorderIpTransHisto()
{
	return m_upperIpTrans;
}

/**
 * provide lower border of the Ip TH1 for longitudinal ip
 */
double IpHistos::getLowerBorderIpLongHisto()
{
	return m_lowerIpLong;
}

/**
 * provide upper border of the Ip TH1 for longitudinal ip
 */
double IpHistos::getUpperBorderIpLongHisto()
{
	return m_upperIpLong;
}

/**
 * provide lower border of the IpSig TH1
 */
double IpHistos::getLowerBorderIpSigHisto()
{
	return m_lowerIpSig;
}

/**
 * provide upper border of the Ip TH1
 */
double IpHistos::getUpperBorderIpSigHisto()
{
	return m_upperIpSig;
}

/**
 * initialize some strings and numbers
 */
void IpHistos::m_initNumbersStrings()
{
	m_areMirroredAndScaled = false;
	m_etaBorders.clear();
	m_etaBorders.push_back(-2.5);
	m_etaBorders.push_back(-1.9);
	m_etaBorders.push_back(-0.1);
	m_etaBorders.push_back(+0.1);
	m_etaBorders.push_back(+1.9);
	m_etaBorders.push_back(+2.5);
	m_nEta = m_etaBorders.size() - 1;
	m_ptBorders.clear();
	m_ptBorders.push_back(0);
	m_ptBorders.push_back(+1000.0);
	m_ptBorders.push_back(+2000.0);
	m_ptBorders.push_back(+3000.0);
	m_ptBorders.push_back(+4000.0);
	m_ptBorders.push_back(+14000000.0);
	m_nPt = m_ptBorders.size() - 1;
	m_jetAlgoNames.clear();
	m_jetAlgoNames.push_back("Cone4H1TowerJets");
	m_defaultJetAlgo = 0;
	m_jetAlgoNames.push_back("Cone4H1TopoJets");
	m_jetAlgoNames.push_back("Cone7H1TopoJets");
	m_jetAlgoNames.push_back("Cone7H1TowerJets");
	m_jetAlgoNames.push_back("Kt4H1TopoJets");
	m_jetAlgoNames.push_back("Kt4H1TowerJets");
	m_jetAlgoNames.push_back("Kt6H1TopoJets");
	m_jetAlgoNames.push_back("Kt6H1TowerJets");
	/* for files created with athena13:
	m_jetAlgoNames.push_back("Cone4H1TowerParticleJets");
	m_defaultJetAlgo = 0;
	m_jetAlgoNames.push_back("Cone4H1TopoParticleJets");
	m_jetAlgoNames.push_back("Cone7H1TopoParticleJets");
	m_jetAlgoNames.push_back("Cone7H1TowerParticleJets");
	m_jetAlgoNames.push_back("Kt4H1TopoParticleJets");
	m_jetAlgoNames.push_back("Kt4H1TowerParticleJets");
	m_jetAlgoNames.push_back("Kt6H1TopoParticleJets");
	m_jetAlgoNames.push_back("Kt6H1TowerParticleJets");
	*/
	m_ipSigNames.clear();
	m_ipSigNames.push_back("Ip");
	m_ipSigNames.push_back("Sig");
	m_transLongNames.clear();
	m_transLongNames.push_back("Trans");
	m_transLongNames.push_back("Long");
	m_etaNames.clear();
	for(unsigned int i = 0; i < m_nEta; i++) {
		std::ostringstream first;
		first << std::showpos << m_etaBorders[i];
		std::ostringstream second;
		second << std::showpos << m_etaBorders[i+1];
		m_etaNames.push_back("Eta" + first.str() + second.str() );
	}
	m_ptNames.clear();
	for(unsigned int i = 0; i < m_nPt; i++) {
		std::ostringstream first;
		first << std::showpos << m_ptBorders[i];
		std::ostringstream second;
		second << std::showpos << m_ptBorders[i+1];
		m_ptNames.push_back("Pt" + first.str() + second.str() );
	}
	return;
}

/**
 * initialize the functions that should be fitted to histograms
 */
void IpHistos::m_initFunctions()
{
	std::string gauss1 = "[0]*exp(-(x)*(x)/(2*[1]*[1]))";
	std::string gauss2 = "[2]*exp(-(x)*(x)/(2*[3]*[3]))";
	std::string exp2 = "[2]*exp(-x/[3])";
	std::string exp3 = "[4]*exp(-x/[5])";
	std::string gauss = "gauss";
	std::string expo = "exp";
	std::string ip = "Ip";
	std::string ipsig = "IpSig";
	m_negativeFuncIp = gauss + ip;
	//m_negativeFuncIp = gauss + gauss + ip;
	m_positiveFuncIp = gauss + expo + ip;
	m_negativeFuncIpSig = gauss + ipsig;
	//m_negativeFuncIpSig = gauss + gauss + ipsig;
	m_positiveFuncIpSig = gauss + expo + ipsig;
	m_negative2GaussIpTrans = new TF1( m_negativeFuncIp.c_str(),
			(gauss1).c_str(),
			m_lowerIpTrans, m_zero);
	//m_negative2GaussIp = new TF1( m_negativeFuncIp.c_str(),
	//		(gauss1 + m_plus + gauss2).c_str(),
	//		m_lowerIpTrans, m_zero);
	m_positive1Gauss1ExpIpTrans = new TF1( m_positiveFuncIp.c_str(),
			(gauss1 + m_plus + exp2).c_str(),
			m_zero, m_upperIpTrans);
	m_negative2GaussIpLong = new TF1( m_negativeFuncIp.c_str(),
			(gauss1).c_str(),
			m_lowerIpLong, m_zero);
	//m_negative2GaussIp = new TF1( m_negativeFuncIp.c_str(),
	//		(gauss1 + m_plus + gauss2).c_str(),
	//		m_lowerIpLong, m_zero);
	m_positive1Gauss1ExpIpLong = new TF1( m_positiveFuncIp.c_str(),
			(gauss1 + m_plus + exp2).c_str(),
			m_zero, m_upperIpLong);
	m_negative2GaussIpSig = new TF1( m_negativeFuncIpSig.c_str(),
			(gauss1).c_str(),
			m_lowerIpSig, m_zero);
	//m_negative2GaussIpSig = new TF1( m_negativeFuncIpSig.c_str(),
	//		(gauss1 + m_plus + gauss2).c_str(),
	//		m_lowerIpSig, m_zero);
	m_positive1Gauss1ExpIpSig = new TF1( m_positiveFuncIpSig.c_str(),
			(gauss1 + m_plus + exp2).c_str(),
			m_zero, m_upperIpSig);
	return;
}

/**
 * create all histograms
 */
void IpHistos::createHistos()
{
	if(m_created) {
		return;
	}
	for(unsigned int i = 0; i < m_nJetAlgos; i++)
		for(unsigned int j = 0; j < m_nIpSig; j++)
			for(unsigned int k = 0; k < m_nTransLong; k++)
				for(unsigned int l = 0; l < m_nEta; l++)
					for(unsigned int m = 0; m < m_nPt; m++)
						m_allocHisto(i, j, k, l, m);
	m_created = true;
	return;
}

/**
 * provide the histogram name for ITHistService
 */
std::string IpHistos::getHistoName(unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	return m_jetAlgoNames[jetAlgo] + m_separationChar +
			m_ipSigNames[ipSig] + m_separationChar +
			m_transLongNames[transLong] + m_separationChar +
			m_etaNames[eta] + m_separationChar +
			m_ptNames[pt];
}

/**
 * create current histogram
 */
void IpHistos::m_allocHisto(unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	std::string name = getHistoName(jetAlgo, ipSig, transLong, eta, pt);
	if(ipSig == IP) {
		TH1D *tmp = 0;
		if(transLong == transverse) {
			tmp = new TH1D(name.c_str(), name.c_str(),
					m_histoBins,
					m_lowerIpTrans, m_upperIpTrans);
			tmp->SetTitle(m_titleIpTrans.c_str() );
		} else if(transLong == longitudinal) {
			tmp = new TH1D(name.c_str(), name.c_str(),
					m_histoBins,
					m_lowerIpLong, m_upperIpLong);
			tmp->SetTitle(m_titleIpLong.c_str() );
		}
		tmp->GetXaxis()->SetTitle(m_xAxisTitleIp.c_str() );
		tmp->GetYaxis()->SetTitle(m_yAxisTitleAbsolute.c_str() );
		tmp->SetLineColor( (Color_t)m_histoLineColor);
		tmp->SetLineStyle( (Style_t)m_lineStyle);
		tmp->SetLineWidth( (Width_t)m_lineWidth);
		(*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt] = tmp;
		tmp->Sumw2();
	} else if(ipSig == IPSIG) {
		TH1D *tmp = new TH1D(
				name.c_str(), name.c_str(), m_histoBins,
				m_lowerIpSig, m_upperIpSig);
		if(transLong == transverse) {
			tmp->SetTitle(m_titleIpSigTrans.c_str() );
		} else if(transLong == longitudinal) {
			tmp->SetTitle(m_titleIpSigLong.c_str() );
		}
		tmp->GetXaxis()->SetTitle(m_xAxisTitleIpSig.c_str() );
		tmp->GetYaxis()->SetTitle(m_yAxisTitleAbsolute.c_str() );
		tmp->SetLineColor( (Color_t)m_histoLineColor);
		tmp->SetLineStyle( (Style_t)m_lineStyle);
		tmp->SetLineWidth( (Width_t)m_lineWidth);
		(*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt] = tmp;
		tmp->Sumw2();
	}
	return;
}

/**
 * delete current histogram
 */
void IpHistos::m_freeHisto(unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	delete (*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt];
	(*m_histos)[(Index)jetAlgo][(Index)ipSig][(Index)transLong]
				[(Index)eta][(Index)pt] = 0;
	return;
}

/**
 * load histograms
 */
void IpHistos::loadHistos()
{
	m_created = false;
	return;
}

/**
 * delete all histograms
 */
void IpHistos::m_deleteHistos()
{
	for(unsigned int i = 0; i < m_nJetAlgos; i++)
		for(unsigned int j = 0; j < m_nIpSig; j++)
			for(unsigned int k = 0; k < m_nTransLong; k++)
				for(unsigned int l = 0; l < m_nEta; l++)
					for(unsigned int m = 0; m < m_nPt; m++)
						m_freeHisto(i, j, k, l, m);
	return;
}

/**
 * fill current track information into all according histograms 
 */
void IpHistos::fill(double eventWeight,
		std::string jetAlgo, double ipTrans, double ipLong,
		double ipSigTrans, double ipSigLong, double eta, double pt)
{
	std::vector<std::string>::iterator p = find(m_jetAlgoNames.begin(),
			m_jetAlgoNames.end(), jetAlgo);
	unsigned int pos = 0;
	if(p != m_jetAlgoNames.end() ) {
		pos = p - m_jetAlgoNames.begin();
	} else {
		*m_msg << MSG::DEBUG << "no matching jet algo found -> no fill"
				<< endreq;
		return;
	}
	// find the matching eta bin and call it then in the arrary as i-1
	unsigned int i = 0;
	while( (eta > m_etaBorders[i]) && (i < m_etaBorders.size()) ) {
		i++;
	}
	if( (i == 0) || (i == m_etaBorders.size()) ) {
		*m_msg << MSG::DEBUG << "no matching eta bin found -> no fill"
				<< endreq;
		return;
	}
	// find the matching pt bin and call it then in the arrary as j-1
	unsigned int j = 0;
	while( (pt > m_ptBorders[j]) && (j < m_ptBorders.size()) ) {
		j++;
	}
	if( (j == 0) || (j == m_ptBorders.size()) ) {
		*m_msg << MSG::DEBUG << "no matching pt bin found -> no fill"
				<< endreq;
		return;
	}
/*
	*m_msg << MSG::DEBUG << "jetAlgo = " << jetAlgo << endreq;
	*m_msg << MSG::DEBUG << "ipTrans = " << ipTrans << endreq;
	*m_msg << MSG::DEBUG << "ipLong = " << ipLong << endreq;
	*m_msg << MSG::DEBUG << "ipSigTrans = " << ipSigTrans << endreq;
	*m_msg << MSG::DEBUG << "ipSigLong = " << ipSigLong << endreq;
	*m_msg << MSG::DEBUG << "eta = " << eta << endreq;
	*m_msg << MSG::DEBUG << "pt = " << pt << endreq;
	double entries = 0;
	entries = (*m_histos) [(Index)pos] [(Index)IP] [(Index)transverse]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
	*m_msg << MSG::DEBUG << "entries before fill = " << entries << endreq;
*/
	(*m_histos) [(Index)pos] [(Index)IP] [(Index)transverse]
			[(Index)(i-1)] [(Index)(j-1)]
			->Fill(ipTrans, eventWeight);
/*
	entries = (*m_histos) [(Index)pos] [(Index)IP] [(Index)transverse]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
	*m_msg << MSG::DEBUG << "entries after fill = " << entries << endreq;

	entries = (*m_histos) [(Index)pos] [(Index)IP] [(Index)longitudinal]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
	*m_msg << MSG::DEBUG << "entries before fill = " << entries << endreq;
*/
	(*m_histos) [(Index)pos] [(Index)IP] [(Index)longitudinal]
			[(Index)(i-1)] [(Index)(j-1)]
			->Fill(ipLong, eventWeight);
/*
	entries = (*m_histos) [(Index)pos] [(Index)IP] [(Index)longitudinal]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
	*m_msg << MSG::DEBUG << "entries after fill = " << entries << endreq;

	*m_msg << MSG::DEBUG << "entries before fill = " << entries << endreq;
	entries = (*m_histos) [(Index)pos] [(Index)IPSIG] [(Index)transverse]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
*/
	(*m_histos) [(Index)pos] [(Index)IPSIG] [(Index)transverse]
			[(Index)(i-1)] [(Index)(j-1)]
			->Fill(ipSigTrans, eventWeight);
/*
	entries = (*m_histos) [(Index)pos] [(Index)IPSIG] [(Index)transverse]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
	*m_msg << MSG::DEBUG << "entries after fill = " << entries << endreq;

	*m_msg << MSG::DEBUG << "entries before fill = " << entries << endreq;
	entries = (*m_histos) [(Index)pos] [(Index)IPSIG] [(Index)longitudinal]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
*/
	(*m_histos) [(Index)pos] [(Index)IPSIG] [(Index)longitudinal]
			[(Index)(i-1)] [(Index)(j-1)]
			->Fill(ipSigLong, eventWeight);
/*
	entries = (*m_histos) [(Index)pos] [(Index)IPSIG] [(Index)longitudinal]
			[(Index)(i-1)] [(Index)(j-1)] ->GetEntries();
	*m_msg << MSG::DEBUG << "entries after fill = " << entries << endreq;
*/
	*m_msg << MSG::DEBUG << "histograms filled" << endreq;
	return;
}

/**
 * fit all histograms
 */
void IpHistos::fit()
{
	for(unsigned int i = 0; i < m_nJetAlgos; i++)
		for(unsigned int j = 0; j < m_nIpSig; j++)
			for(unsigned int k = 0; k < m_nTransLong; k++)
				for(unsigned int l = 0; l < m_nEta; l++)
					for(unsigned int m = 0; m < m_nPt; m++)
						m_fitHisto(i, j, k, l, m);
	return;
}

/**
 * fit current histogram
 */
void IpHistos::m_fitHisto(unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	if(ipSig == IP) {
		if(transLong == transverse) {
			(*m_histos) [(Index)jetAlgo][(Index)ipSig]
        		                [(Index)transLong]
					[(Index)eta][(Index)pt] ->
					Fit(m_negativeFuncIp.c_str(),
					"Q", "",
					m_lowerIpTrans, 0.0);
		} else if(transLong == longitudinal) {
			(*m_histos) [(Index)jetAlgo][(Index)ipSig]
        		                [(Index)transLong]
					[(Index)eta][(Index)pt] ->
					Fit(m_negativeFuncIp.c_str(),
					"Q", "",
					m_lowerIpLong, 0.0);
		}
	} else if(ipSig == IPSIG) {
		(*m_histos) [(Index)jetAlgo][(Index)ipSig]
        	                [(Index)transLong][(Index)eta][(Index)pt] ->
				Fit(m_negativeFuncIpSig.c_str(), "Q", "",
				m_lowerIpSig, 0.0);
	}
	return;
}

/**
 * save all histograms to .ps/.eps files
 */
void IpHistos::save(std::string prefix)
{
	//commented out due to
	//https://hypernews.cern.ch/HyperNews/Atlas/get/offlineSWHelp/683.html
	*m_msg << MSG::DEBUG << "Start of save" << endreq;
	const std::string atlas = "ATLAS";
	const std::string style = "Atlas style";
	TStyle *atlasStyle = new TStyle(atlas.c_str(), style.c_str());
	*m_msg << MSG::DEBUG << "TStyle created" << endreq;
	// crash in following function 20080422:
	//m_atlasRootStyle(atlasStyle);
	*m_msg << MSG::DEBUG << "TStyle set" << endreq;
	gROOT->SetStyle(atlas.c_str() );
	gROOT->ForceStyle();
	*m_msg << MSG::DEBUG << "TStyle forced" << endreq;
	// always annoying debate about resolution and ratio:
	// see also  http://de.wikipedia.org/wiki/Bild:Video_Standards.svg
	// with 1024/480=2.13 :
	// latex puts on 1 slide: centeredlarge title
	// left=normal graphics totalheight=5.6cm, nearly no white space 
	// left and right
	// 3 standard lines separated by "\\"
	// 1 centered line for author in \tiny :
	//TCanvas *c = new TCanvas("c", "canvas", 0, 0, 1024, 480);
	// 1024/640=1.67 :
	// latex same config as above: 50% of the screen width are white
	//TCanvas *c = new TCanvas("c", "canvas", 0, 0, 1024, 640);
	// 16:9 = 1.78 HD :
	// latex same config as above: 40% of the screen width are white
	//TCanvas *c = new TCanvas("c", "canvas", 0, 0, 1920, 1080);
	// set a ratio of 1.3 = 4/3 : for thesis useful: middle resolution :
	//TCanvas *c = new TCanvas("c", "canvas", 0, 0, 1024, 768);
	//  set a ratio of 1.3 = 4/3 : for thesis useful: high resolution :
	//TCanvas *c = new TCanvas("c", "canvas", 0, 0, , 2048, 1536);
	const unsigned int upperx = 0;
	const unsigned int uppery = 0;
	const unsigned int width = 1920;
	const unsigned int height = 1080;
	const std::string canvasName = "c";
	const std::string canvasTitle = "";
	*m_msg << MSG::DEBUG << "create TCanvas now ..." << endreq;
	TCanvas *c = new TCanvas(canvasName.c_str(),
			canvasTitle.c_str(),
			(Int_t)upperx, (Int_t)uppery,
			(Int_t)width, (Int_t)height);
	*m_msg << MSG::DEBUG << "canvas created" << endreq;
	gPad->SetGrid();
	c->ToggleEventStatus();
	*m_msg << MSG::DEBUG << "loop over all histos now..." << endreq;
	for(unsigned int i = 0; i < m_nJetAlgos; i++)
		for(unsigned int j = 0; j < m_nIpSig; j++)
			for(unsigned int k = 0; k < m_nTransLong; k++)
				for(unsigned int l = 0; l < m_nEta; l++)
					for(unsigned int m = 0; m < m_nPt; m++)
						m_saveHisto(prefix, c, i, j,
								k, l, m);
	*m_msg << MSG::DEBUG << "all histos saved" << endreq;
	delete c;
	c = 0;
	delete atlasStyle;
	atlasStyle = 0;
	return;
}

/**
 * save a single histogram to a eps/ps file
 */
void IpHistos::m_saveHisto(std::string prefix, TCanvas *c,
		unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	*m_msg << MSG::DEBUG << "start of m_saveHisto" << endreq;
	TH1 *tmpPointer = (*m_histos) [(Index)jetAlgo][(Index)ipSig]
        	                [(Index)transLong][(Index)eta][(Index)pt];
	std::string name = prefix + (std::string) (tmpPointer->GetName() );
	// bug: copy constructor does not work -> segfault:
	//TH1 *tmp = new TH1(*tmpPointer);
	//TH1 tmp = *tmpPointer;
	// bug: Clone takes much more time than creating a new histogram
	// and copying over the bin contents manually
	std::string clone = "Clone";
	TH1 *tmp = (TH1*)tmpPointer->Clone( (name + clone).c_str() );
	*m_msg << MSG::DEBUG << "saving histo " << name << " ..." << endreq;
	m_setHistoAbsolute(tmp);
	c->SetLogy(0);
	tmp->Draw();
	c->SaveAs( (name + m_separationChar + m_absolute + m_lin + m_eps)
			.c_str() );
	c->SaveAs( (name + m_separationChar + m_absolute + m_lin + m_ps)
			.c_str() );
	c->SetLogy();
	c->Update();
	c->SaveAs( (name + m_separationChar + m_absolute + m_loga + m_eps)
			.c_str() );
	c->SaveAs( (name + m_separationChar + m_absolute + m_loga + m_ps)
			.c_str() );
	tmp->Scale(1.0 / tmp->Integral() );
	m_setHistoNormalized(tmp);
	c->SetLogy(0);
	c->Update();
	c->SaveAs( (name + m_separationChar + m_normalized + m_lin + m_eps)
			.c_str() );
	c->SaveAs( (name + m_separationChar + m_normalized + m_lin + m_ps)
			.c_str() );
	c->SetLogy();
	c->Update();
	c->SaveAs( (name + m_separationChar + m_normalized + m_loga + m_eps)
			.c_str() );
	c->SaveAs( (name + m_separationChar + m_normalized + m_loga + m_ps)
			.c_str() );
	*m_msg << MSG::DEBUG << "histo " << name << " saved successfuly"
			<< endreq;
	delete tmp;
	tmp = 0;
	return;
}

/**
 * delete positive side, mirror negative side to positive side,
 * scale to the number of entries
 */
void IpHistos::mirrorAndScale()
{
	if(m_areMirroredAndScaled) {
		*m_msg << MSG::INFO << "Histograms are already mirrored and "
				"scaled -> did nothing" << endreq;
	} else {
		for(unsigned int i = 0; i < m_nJetAlgos; i++) {
			for(unsigned int j = 0; j < m_nIpSig; j++) {
				for(unsigned int k = 0;
						k < m_nTransLong; k++) {
					for(unsigned int l = 0;
							l < m_nEta; l++) {
						for(unsigned int m = 0;
								m < m_nPt;
								m++) {
							m_mirrorScaleHisto(
								i, j, k, l, m);
						}
					}
				}
			}
		}
		m_areMirroredAndScaled = true;
		*m_msg << MSG::INFO << "Histograms mirrored and scaled"
				<< endreq;
	}
	return;
}

/**
 * mirror and scale 1 histogram
 */
void IpHistos::m_mirrorScaleHisto(
		unsigned int jetAlgo, unsigned int ipSig,
		unsigned int transLong, unsigned int eta,
		unsigned int pt)
{
	TH1 *tmp = (*m_histos) [(Index)jetAlgo][(Index)ipSig]
			[(Index)transLong][(Index)eta][(Index)pt];
	*m_msg << MSG::INFO << "try to mirror/scale histogram " <<
			tmp->GetName() << endreq;
	int binMax = tmp->GetNbinsX();
	if(binMax % 2 == 0) {
		;
	} else {
		*m_msg << MSG::WARNING <<
				"Histogram has odd number of bins "
				"-> probably no bin border at 0"
				<< endreq;
	}
	// copy negative side (resolution) to positive side:
	for(unsigned int i = 1; i <= m_histoBins / 2; i++) {
		// TH1 ends at bin number GetNbinsX
		tmp->SetBinContent(m_histoBins - i + 1,
				tmp->GetBinContent(i) );
	}
	*m_msg << MSG::DEBUG << "before scaling bin content of 48 = "
			<< tmp->GetBinContent(48) << endreq;
	// now normalize the histogram:
	// after changing the bin content GetEntries() could give
	// the wrong number, so calculate it:
	double integral = tmp->Integral(1,m_histoBins,"");
	if(integral) {
		tmp->Scale(1.0 / integral);
	} else {
		*m_msg << MSG::WARNING <<
				"Integral of histogram is 0 -> not scaled"
				<< endreq;
	}
	*m_msg << MSG::DEBUG << "after scaling bin content of 48 = "
			<< tmp->GetBinContent(48) << endreq;
	*m_msg << MSG::DEBUG <<
			"current histogram mirrored and scaled"
			<< endreq;
	return;
}

/**
 * true when all histograms were mirrored and scaled
 */
bool IpHistos::areMirroredAndScaled()
{
	return m_areMirroredAndScaled;
}

/**
 * set title of y axis of histogram to absolute number of entries
 */
void IpHistos::m_setHistoAbsolute(TH1 *histo)
{
	histo->GetYaxis()->SetTitle(m_yAxisTitleAbsolute.c_str() );
}

/**
 * set title of y axis of histogram to absolute number of entries
 */
void IpHistos::m_setHistoNormalized(TH1 *histo)
{
	histo->GetYaxis()->SetTitle(m_yAxisTitleNormalized.c_str() );
}

/* 
 * the official ATLAS root style from
 * http://atlas.web.cern.ch/
 * Atlas/GROUPS/GENERAL/SCINOTES/templates/root/rootlogon.C
 */
void IpHistos::m_atlasRootStyle(TStyle *atlasStyle)
{
	//set white
	Int_t icol = 0;
	atlasStyle->SetFrameBorderMode(icol);
	atlasStyle->SetCanvasBorderMode(icol);
	atlasStyle->SetPadBorderMode(icol);
	atlasStyle->SetPadColor(icol);
	atlasStyle->SetCanvasColor(icol);
	atlasStyle->SetStatColor(icol);
	//atlasStyle->SetFillColor(icol);

	atlasStyle->SetPaperSize(20,26);
	atlasStyle->SetPadTopMargin(0.05);
	atlasStyle->SetPadRightMargin(0.05);
	atlasStyle->SetPadBottomMargin(0.16);
	atlasStyle->SetPadLeftMargin(0.12);

	// does not affect histo title
	// use large fonts
	//Int_t font=72;
	Int_t font = 42;//62 is root default
	Double_t tsize = 0.05;//1 is root default
	atlasStyle->SetTextFont(font);
	atlasStyle->SetTextSize(tsize);
	atlasStyle->SetTextSizePixels(100);
	atlasStyle->SetLabelFont(font,"x");
	atlasStyle->SetTitleFont(font,"x");
	atlasStyle->SetLabelFont(font,"y");
	atlasStyle->SetTitleFont(font,"y");
	atlasStyle->SetLabelFont(font,"z");
	atlasStyle->SetTitleFont(font,"z");
	atlasStyle->SetLabelSize(tsize,"x");
	atlasStyle->SetTitleSize(tsize,"x");
	atlasStyle->SetLabelSize(tsize,"y");
	atlasStyle->SetTitleSize(tsize,"y");
	atlasStyle->SetLabelSize(tsize,"z");
	atlasStyle->SetTitleSize(tsize,"z");

	// use bold lines and markers
	atlasStyle->SetMarkerStyle(20);
	atlasStyle->SetMarkerSize(1.2);
	// following changed from 2 to 3 :
	atlasStyle->SetHistLineWidth(3);
	atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

	// get rid of X error bars and y error bar caps
	//atlasStyle->SetErrorX(0.001);

	// do not (0) display (1) any of the standard histogram decorations
	atlasStyle->SetOptTitle(1);
	// 1111
	std::string statistics = "nemr";
	atlasStyle->SetOptStat(statistics.c_str());
	//atlasStyle->SetOptStat(0);
	atlasStyle->SetOptFit(0111);
	//atlasStyle->SetOptFit(0);

	gROOT->SetStyle("Plain");

	// put tick marks on top and RHS of plots
	// does not work -> so SetTicks(1,1) below
	atlasStyle->SetPadTickX(1);
	atlasStyle->SetPadTickY(1);

	// following lines introduced :
	// put date
	atlasStyle->SetOptDate(1);
	// set distance of y axis label to y axis (no overlap with number) :
	atlasStyle->SetTitleOffset(1.3,"y");//1.3
	//atlasStyle->GetAttDate()->SetTextFont(font=62);
	//atlasStyle->GetAttDate()->SetTextSize(size=0.025);
	//atlasStyle->GetAttDate()->SetTextAngle(angle=0);
	//atlasStyle->GetAttDate()->SetTextAlign(align=11);
	//atlasStyle->GetAttDate()->SetTextColor(color=1);
	gPad->SetGrid();
	gPad->SetTicks(1,1);
	// print title, center it and set font size :
	// labelsize 0.07 and titlesize 0.09 is shit because 
	// box too large, font too small
	// makes only the size of the box larger :
	atlasStyle->SetLabelFont(font,"t");
	atlasStyle->SetTitleFont(font,"t");
	atlasStyle->SetTitleSize(0.08,"t");
	atlasStyle->SetLabelSize(0.08,"t");
	// scales the box WITH font :
	atlasStyle->SetTitleX(0.19);
	atlasStyle->SetTitleY(0.995);
	// scales font only :
	atlasStyle->SetTitleW(0.58);
	atlasStyle->SetTitleXSize(0.05);
	atlasStyle->SetTitleYSize(0.05);
	// title is perfect with th1d->SetLabelSize(0.09,"t"); and 
	// no style on
	return;
}
