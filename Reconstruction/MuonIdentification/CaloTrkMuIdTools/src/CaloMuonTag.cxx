/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTrkMuIdTools/CaloMuonTag.h"
#include "StoreGate/StoreGateSvc.h"
#include "TFile.h"
#include "TH2F.h"
#include "PathResolver/PathResolver.h"
///////////////////////////////////////////////////////////////////////////////
// Constructor 
///////////////////////////////////////////////////////////////////////////////
CaloMuonTag::CaloMuonTag(const std::string& type, const std::string& name, const IInterface*  pInterface) :
AthAlgTool(type, name, pInterface),
m_hist(0),
m_numCuts(0),
m_numPtBins(0),
m_numTagged(0),
m_numRejected(0)
{
	declareInterface<ICaloMuonTag>(this);
	declareProperty("TagMode", m_tagMode = "Tight");
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////
CaloMuonTag::~CaloMuonTag()
{
  delete m_hist; m_hist = nullptr; // TFile::Get new's so this needs to be deleted
}

///////////////////////////////////////////////////////////////////////////////
// initialize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonTag::initialize()
{
	/*  if (AlgTool::initialize().isFailure()) {
		msg(MSG::ERROR) << "AlgTool couldn't be initialized!" << endmsg;
		return StatusCode::FAILURE;
		} */

	// Retrieve histogram
	std::string rootFilePath = PathResolver::find_file("CaloTag.CutConfig.root","DATAPATH");
	TFile* rootFile = TFile::Open(rootFilePath.c_str(), "READ");
	ATH_CHECK(getHist(rootFile, m_tagMode.c_str(), m_hist));
  rootFile->Close(); delete rootFile; rootFile = nullptr; // don't need the file anymore
	// Read cut names from histogram
	m_numCuts = m_hist->GetYaxis()->GetNbins();
	if (m_numCuts == 0) {
		ATH_MSG_FATAL("Histogram number of cuts invalid");
		return StatusCode::FAILURE;
	}
	for (int i = 0; i <= m_numCuts; i++){
		m_vecCutNames.push_back(m_hist->GetYaxis()->GetBinLabel(i));
	}
	// Read pt bins from histogram
	m_numPtBins = m_hist->GetXaxis()->GetNbins();
	if (m_numPtBins == 0) {
		ATH_MSG_FATAL("Histogram number of pt bins invalid");
		return StatusCode::FAILURE;
	}
	for (int i = 0; i <= m_numPtBins; i++){
		m_vecPtBins.push_back(atoi(m_hist->GetXaxis()->GetBinLabel(i)));
	}
	ATH_MSG_DEBUG("initialize() successful in " << name());
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// getHist
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonTag::getHist(TFile* file, const char* histName, TH2F*& hist){
	if (!file) {
		ATH_MSG_ERROR("NULL TFile");
		return StatusCode::FAILURE;
	}
	hist = dynamic_cast<TH2F*>(file->Get(histName));
	if (!hist){
		ATH_MSG_ERROR("Cannot retrieve histogram: " << histName);
		return StatusCode::FAILURE;
	}
	hist->SetDirectory(0);
	ATH_MSG_INFO("Successfully retrieved histogram: " << histName);
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// finalize
///////////////////////////////////////////////////////////////////////////////
StatusCode CaloMuonTag::finalize()
{
	ATH_MSG_INFO("Number of tracks tagged   : " << m_numTagged);
	ATH_MSG_INFO("Number of tracks rejected : " << m_numRejected);
	ATH_MSG_DEBUG("finalize() successful in " << name());
	return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// caloMuonTag
///////////////////////////////////////////////////////////////////////////////
int CaloMuonTag::caloMuonTag(const std::vector<DepositInCalo>& deposits, double /*eta*/, double pt) const
{
	std::vector<DepositInCalo>::const_iterator it = deposits.begin();
	int numVetoesTriggered = 0;
	int tag = 4;
	for (; it != deposits.end(); it++) {
		CaloCell_ID::CaloSample sample = it->subCaloId();
		double dep = it->energyDeposited();
		double eLoss = it->muonEnergyLoss();
		double diff = dep - eLoss;

		if (sample == CaloCell_ID::EMB1) {
			if (dep > interpolate("emb1Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::EMB2) {
			if (dep > interpolate("emb2Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::EMB3) {
			if (dep > interpolate("emb3Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::EME1) {
			if (dep > interpolate("eme1Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::EME2) {
			if (dep > interpolate("eme2Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::EME3) {
			if (dep > interpolate("eme3Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::TileBar0) {
			if (tag > 3) tag = 3;
			if (dep < interpolate("tileBar0Sig", pt)) {
				numVetoesTriggered++;
			}
			if (dep > interpolate("tileBar0Veto", pt)) {
				numVetoesTriggered++;
			}
			if (diff < interpolate("tileBar0DiffLow", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::TileBar1) {
			if (tag > 2) tag = 2;
			if (dep < interpolate("tileBar1Sig", pt)) {
				numVetoesTriggered++;
			}
			if (dep > interpolate("tileBar1Veto", pt)) {
				numVetoesTriggered++;
			}
			if (diff < interpolate("tileBar1DiffLow", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::TileBar2) {
			if (tag > 1) tag = 1;
			if (dep < interpolate("tileBar2Sig", pt)) {
				numVetoesTriggered++;
			}
			if (dep > interpolate("tileBar2Veto", pt)) {
				numVetoesTriggered++;
			}
			if (diff < interpolate("tileBar2DiffLow", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::TileExt0) {
			if (tag > 3) tag = 3;
			if (dep < interpolate("tileExt0Sig", pt)) {
				numVetoesTriggered++;
			}
			if (diff < interpolate("tileExt0DiffLow", pt)) {
				numVetoesTriggered++;
			}
			if (dep > interpolate("tileExt0Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::TileExt1) {
			if (tag > 2) tag = 2;
			if (dep < interpolate("tileExt1Sig", pt)) {
				numVetoesTriggered++;
			}
			if (diff < interpolate("tileExt1DiffLow", pt)) {
				numVetoesTriggered++;
			}
			if (dep > interpolate("tileExt1Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::TileExt2) {
			if (tag > 1) tag = 1;
			if (dep < interpolate("tileExt2Sig", pt)) {
				numVetoesTriggered++;
			}
			if (diff < interpolate("tileExt2DiffLow", pt)) {
				numVetoesTriggered++;
			}
			if (dep > interpolate("tileExt2Veto", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::HEC0) {
			if (dep > interpolate("hec0Veto", pt)) {
				numVetoesTriggered++;
			}
			if (dep < interpolate("hec0Sig", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::HEC1) {
			if (tag > 3) tag = 3;
			if (dep > interpolate("hec1Veto", pt)) {
				numVetoesTriggered++;
			}
			if (dep < interpolate("hec1Sig", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::HEC2) {
			if (tag > 2) tag = 2;
			if (dep > interpolate("hec2Veto", pt)) {
				numVetoesTriggered++;
			}
			if (dep < interpolate("hec2Sig", pt)) {
				numVetoesTriggered++;
			}
		}
		if (sample == CaloCell_ID::HEC3) {
			if (tag > 1) tag = 1;
			if (dep > interpolate("hec3Veto", pt)) {
				numVetoesTriggered++;
			}
			if (dep < interpolate("hec3Sig", pt)) {
				numVetoesTriggered++;
			}
		}

		if (numVetoesTriggered >= 1) {
			m_numRejected++;
			return 0;
		}
	}
	if (tag < 3) m_numTagged++;
	else if (tag == 4) return 0;
	return tag;
}

///////////////////////////////////////////////////////////////////////////////
// interpolate
///////////////////////////////////////////////////////////////////////////////
double  CaloMuonTag::interpolate(std::string cutName, double pt) const {
	double ptLow = 0, ptUp = 0, cutLow = 0, cutUp = 0, cutReturn = 0;
	// MeV-->GeV
	pt = pt / 1000;
	int ptBin = getPtBinLow(pt);
	int cutBin = getCutBin(cutName);
	if (ptBin < 0 || ptBin > m_numPtBins || cutBin == 0 || cutBin > m_numCuts)
		ATH_MSG_WARNING("Bin out of bound");

	// Get lowest pt bin
	if (ptBin < 1) cutReturn = m_hist->GetBinContent(1, cutBin);
	// Get highest pt bin
	else if (ptBin == m_numPtBins) cutReturn = m_hist->GetBinContent(m_numPtBins, cutBin);
	// Interpolate between bins
	else {
		ptUp = m_vecPtBins[ptBin + 1];
		ptLow = m_vecPtBins[ptBin];
		cutUp = m_hist->GetBinContent(ptBin + 1, cutBin);
		cutLow = m_hist->GetBinContent(ptBin, cutBin);
		cutReturn = cutLow + (cutUp - cutLow) * (pt - ptLow) / (ptUp - ptLow);
	}

	return cutReturn;
}

int CaloMuonTag::getPtBinLow(double pt) const {
	// Return the bin of lower pt
	for (int i = 0; i <= m_numPtBins; i++){
		if (pt < m_vecPtBins[i]) return (i - 1);
	}
	return m_numPtBins;
}

int CaloMuonTag::getCutBin(std::string cutName) const {
	for (int i = 0; i <= m_numCuts; i++){
		if (cutName == m_vecCutNames[i]) return i;
	}
	return 0;
}



