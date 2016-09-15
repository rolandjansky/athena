/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_CALOMUONTAG_H
#define CALOTRKMUIDTOOLS_CALOMUONTAG_H

#include "ICaloTrkMuIdTools/ICaloMuonTag.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

class TFile;
class TH2F;

class CaloMuonTag : public AthAlgTool, virtual public ICaloMuonTag {
	/** @class CaloMuonTag

	Muon Tagger in the ATLAS calorimeter.

	This tagger is used in the CaloTrkMuId framework. After some basic preselection CaloTrkMuIdAlg
	feeds a track to the TrackDepositInCaloTool. TrackDepositInCaloTool gathers the deposits that are found near that
	track. This tool can tag a muon track from its deposits.

	Applies energy deposition veto cuts in all samplings and noise cuts in the last three layers of the hadronic calorimeter
	(Tile and HEC). The following scale factors are applied to correct for the pathlength traversed in the calorimeter cells.

	\f$E_{\rm th} = \frac{E^{\mathrm{barrel}}_{0}}{\sin^2 \theta}\f$  for \f$|\eta|<1.7\f$
	\f$E_{\rm th} = \frac{E^{\rm end-cap}_{0}}{\ (1 - \sin \theta )^2}\f$ for \f$|\eta|>1.7\f$

	There are three tags, determined by the acceptance:

	0  not a muon,

	1  muon in Last Calo Layer,

	2  muon in Last-1 Calo Layer,

	3  muon in Last-2 Calo Layer,

	@author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
	Marcel Raas <marcelra@cern.ch> (contact person)
	*/

public:

	CaloMuonTag(const std::string& type, const std::string& name, const IInterface* pInterface);
	virtual ~CaloMuonTag();
	virtual StatusCode initialize();
	virtual StatusCode finalize();
	int caloMuonTag(const std::vector<DepositInCalo>& deposits, double eta, double pt) const; //!< Returns the Tag

private:

	double interpolate(std::string cutName, double pt) const;
	int getPtBinLow(double pt) const;
	int getCutBin(std::string cutName) const;


	TH2F* m_hist;
	std::string m_tagMode;
	StatusCode getHist(TFile* file, const char* histName, TH2F*& hist);

	std::vector < std::string > m_vecCutNames;
	std::vector < int > m_vecPtBins;
	int m_numCuts;
	int m_numPtBins;

	mutable int m_numTagged;
	mutable int m_numRejected;

};

#endif

