/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * IpHistos.h
 *
 * Stephan.Sandvoss@cern.ch
 */
#ifndef IPHISTOS_H
#define IPHISTOS_H

/**
 * intra-package includes
 */
#ifndef INCLUDED_IMPACTPARAMETERTUNINGTOOLS_ITRACKIPADJUST
#include "ImpactParameterTuningUtils/TrackJetEventTree.h"
#define INCLUDED_IMPACTPARAMETERTUNINGTOOLS_ITRACKIPADJUST
#endif

/**
 * extra-package includes
 */
/**
 * system includes
 */
#ifndef INCLUDED_TH1
#include "TH1.h"
#define INCLUDED_TH1
#endif

#ifndef INCLUDED_BOOST_MULTIARRAY
#include <boost/multi_array.hpp>
#define INCLUDED_BOOST_MULTIARRAY
#endif

class MsgStream;

class TCanvas;
class TF1;
class TFile;
class TStyle;

/**
 * class IpHistos for plotting/fitting the ip distributions
 */
class IpHistos {
	public:
		// track impact parameter or its significance
		enum IpSig { IP, IPSIG };

		// x-y or z plane
		enum Orientation { transverse, longitudinal };

		// constructor
		IpHistos(MsgStream &msg, TFile *file);

		// destructor
		~IpHistos();

		// provide the file name for ITHistService
		std::string getHistServiceFile();

		// provide the directory name for ITHistService
		std::string getHistServiceDirectory();

		// provide file and directory name for ITHistService
		std::string getHistServiceFullString();

		// store the ITHistService file name
		void setHistServiceFile(std::string s);

		// store the ITHistService directory name
		void setHistServiceDirectory(std::string s);

		// provide the number of jet algorithms
		unsigned int getNJetAlgos();

		// provide the number of the default jet algorithm
		unsigned int getDefaultJetAlgo();

		// provide the number of IP/IPSig == 2
		unsigned int getNIpSig();

		// provide the number of orientations trans/long == 2
		unsigned int getNTransLong();

		// provide the number of track eta bins
		unsigned int getNEta();

		// provide the number track pt bins
		unsigned int getNPt();

		// provide the bin for a certain eta
		int getBinEta(double eta);

		// provide the bin for a certain pt
		int getBinPt(double pt);

		// ITHistService/root wants to have pointers
		void setHisto(TH1 *histo, unsigned int jetAlgo,
				unsigned int ipSig, unsigned int transLong,
				unsigned int eta, unsigned int pt);

		// provide the histo name for ITHistService
		std::string getHistoName(unsigned int jetAlgo,
				unsigned int ipSig, unsigned int transLong,
				unsigned int eta, unsigned int pt);

		// ITHistService/root wants to have pointers
		TH1 *getHisto(unsigned int jetAlgo, unsigned int ipSig,
				unsigned int transLong, unsigned int eta,
				unsigned int pt);

		// provide number of bins of the TH1
		unsigned int getNHistoBins();

		// provide lower border of the TH1 for transversal ip
		double getLowerBorderIpTransHisto();

		// provide upper border of the TH1 for transversal ip
		double getUpperBorderIpTransHisto();

		// provide lower border of the TH1 for longitudinal ip
		double getLowerBorderIpLongHisto();

		// provide upper border of the TH1 for longitudinal ip
		double getUpperBorderIpLongHisto();

		// provide lower border of the TH1
		double getLowerBorderIpSigHisto();

		// provide upper border of the TH1
		double getUpperBorderIpSigHisto();

		// create all histograms
		void createHistos();

		// load histograms from root file
		void loadHistos();

		// fill all histograms
		void fill(double eventWeight, std::string jetAlgo,
				double ipTrans, double ipLong,
				double ipSigTrans, double ipSigLong,
				double eta, double pt);

		// fit all histograms with TF1
		void fit();

		// save histograms to .ps/.eps files
		void save(std::string prefix);

		// delete positive side, mirror negative side to positive side,
		// scale to the number of entries
		void mirrorAndScale();

		// true when all histograms were mirrored and scaled
		bool areMirroredAndScaled();
	private:
		// printf/cout is forbidden
		MsgStream *m_msg;

		// number of jet algoritms
		const unsigned int m_nJetAlgos;

		// number of the default jet algorithm
		unsigned int m_defaultJetAlgo;

		// 0 means IP, 1 means IP Significance
		const unsigned int m_nIpSig;

		// 0 means transversal, 1 means longitudinal
		const unsigned int m_nTransLong;

		// number of track eta bins
		unsigned int m_nEta;

		// number of track pt bins
		unsigned int m_nPt;

		// saves if create() was called
		bool m_created;

		// define an array of histos for each combination of
		// jetalgorithm
		// ip, ipsignificance
		// transversal, longitudinal
		// track eta
		// track pt
		typedef boost::multi_array<TH1*,5> MultiArray;

		// index for the array
		typedef MultiArray::index Index;

		// the array itself
		MultiArray *m_histos;

		// the track eta bin borders
		std::vector<double> m_etaBorders;

		// the track pt bin borders
		std::vector<double> m_ptBorders;

		// it separates the observables in the histogram name
		const std::string m_separationChar;

		// +
		const std::string m_plus;

		// the jet algorithm names
		std::vector<std::string> m_jetAlgoNames;

		// impact parameter or significance
		std::vector<std::string> m_ipSigNames;

		// transversal or longitudinal
		std::vector<std::string> m_transLongNames;

		// track eta names
		std::vector<std::string> m_etaNames;

		// track pt names
		std::vector<std::string> m_ptNames;

		// initialize the vectors above
		void m_initNumbersStrings();

		// initialize the TF1 functions
		void m_initFunctions();

		// the number of bins for the TH1
		const unsigned int m_histoBins;

		// the lower IP border of the TH1 for transversal ip
		const double m_lowerIpTrans;

		// the upper IP border of the TH1 for transversal ip
		const double m_upperIpTrans;

		// the lower IP border of the TH1 for longitudinal ip
		const double m_lowerIpLong;

		// the upper IP border of the TH1 for longitudinal ip
		const double m_upperIpLong;

		// the lower IPSig border of the TH1
		const double m_lowerIpSig;

		// the upper IPSig border of the TH1
		const double m_upperIpSig;

		// the middle of the TH1 is always 0
		const double m_zero;

		// the ITHistService file name
		std::string m_histServiceFile;

		// the ITHistService directory name
		std::string m_histServiceDirectory;

		// create all histograms
		void m_createHistos();

		// create a single histogram
		void m_allocHisto(unsigned int jetAlgo, unsigned int ipSig,
				unsigned int transLong, unsigned int eta,
				unsigned int pt);

		// delete a single histogram
		void m_freeHisto(unsigned int jetAlgo, unsigned int ipSig,
				unsigned int transLong, unsigned int eta,
				unsigned int pt);

		// load all histograms
		void m_loadHistos();

		// delete all histograms
		void m_deleteHistos();

		// name of the TF1 for the fit of the negative IP 
		std::string m_negativeFuncIp;

		// name of the TF1 for the fit of the positive IP
		std::string m_positiveFuncIp;

		// name of the TF1 for the fit of the negative IPSig
		std::string m_negativeFuncIpSig;

		// name of the TF1 for the fit of the positive IPSig
		std::string m_positiveFuncIpSig;

		// the TF1 for the fit of the negative transversal IP 
		TF1 *m_negative2GaussIpTrans;

		// the TF1 for the fit of the positive transversal IP
		TF1 *m_positive1Gauss1ExpIpTrans;

		// the TF1 for the fit of the negative longitudinal IP 
		TF1 *m_negative2GaussIpLong;

		// the TF1 for the fit of the positive longitudinal IP
		TF1 *m_positive1Gauss1ExpIpLong;

		// the TF1 for the fit of the negative IPSig
		TF1 *m_negative2GaussIpSig;

		// the TF1 for the fit of the positive IPSig
		TF1 *m_positive1Gauss1ExpIpSig;

		// fit a single histogram
		void m_fitHisto(unsigned int jetAlgo, unsigned int ipSig,
				unsigned int transLong, unsigned int eta,
				unsigned int pt);

		// the style of the histogram according to atlas convention
		void m_atlasRootStyle(TStyle *atlasStyle);

		// the histogram title for longitudinal IP
		const std::string m_titleIpLong;

		// the histogram title for longitudinal IPSig
		const std::string m_titleIpSigLong;

		// the histogram title for transversal IP
		const std::string m_titleIpTrans;

		// the histogram title for transversal IPSig
		const std::string m_titleIpSigTrans;

		// the title of the x-axis of the histogram for IP
		const std::string m_xAxisTitleIp;

		// the title of the x-axis of the histogram for IPSig
		const std::string m_xAxisTitleIpSig;

		// the title of the y-axis of the histogram when abs. entries
		const std::string m_yAxisTitleAbsolute;

		// the title of the x-axis of the histogram when normalized
		const std::string m_yAxisTitleNormalized;

		// for saving to a eps/ps file
		const std::string m_absolute;

		// for saving to a eps/ps file
		const std::string m_normalized;

		// for saving to a eps/ps file
		const std::string m_lin;

		// for saving to a eps/ps file
		const std::string m_loga;

		// for saving to a eps/ps file
		const std::string m_ps;

		// for saving to a eps/ps file
		const std::string m_eps;

		// the line color of the histogram
		const unsigned int m_histoLineColor;

		// the line style of the histogram
		const unsigned int m_lineStyle;

		// the line width of the histogram
		const unsigned int m_lineWidth;

		// set the histogram to absolute number of entries
		void m_setHistoAbsolute(TH1 *histo);

		// set the histogram to normalized number of entries
		void m_setHistoNormalized(TH1 *histo);

		// save a single histogram to an eps/ps file
		// commented out due to
		// https://hypernews.cern.ch/HyperNews/Atlas/get/offlineSWHelp/683.html
		void m_saveHisto(std::string prefix, TCanvas *c,
				unsigned int jetAlgo, unsigned int ipSig,
				unsigned int transLong, unsigned int eta,
				unsigned int pt);

		// keep track if histos were mirrored
		bool m_areMirroredAndScaled;

		// throw away positive side, mirror negative side to positive
		// and scale histogram
		void m_mirrorScaleHisto(
				unsigned int jetAlgo, unsigned int ipSig,
				unsigned int transLong, unsigned int eta,
				unsigned int pt);
};
#endif
