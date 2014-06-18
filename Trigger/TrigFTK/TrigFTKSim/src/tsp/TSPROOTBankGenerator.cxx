/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/tsp/TSPROOTBankGenerator.h"

#include "TrigFTKSim/tsp/TSPMap.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKPattern.h"

#include <TTree.h>
#include <TBranch.h>

#include <boost/lexical_cast.hpp>

#include <cstdio>
#include <iostream>

using namespace TSPROOT;
using namespace boost;
using namespace std;


TSPROOTBankGenerator::TSPROOTBankGenerator(const char *fname)
{
  m_bankfile = TFile::Open(fname);
}


TSPROOTBankGenerator::TSPROOTBankGenerator(FTKSetup* setup, const std::vector<FTKSSMap*>& ssMaps, const std::string& inputBank, const std::string& outBank, unsigned planes, int maxPatterns, int mincoverage) :
  m_setup(setup), m_ssMaps(ssMaps), m_maxPatterns(maxPatterns), m_planes(planes),  m_mincoverage(mincoverage), m_patternID(0l){

  m_setup = &FTKSetup::getFTKSetup();
	if (!ftk_dcap::open_for_read(inputBank, m_inputBank)) {
		FTKSetup::PrintMessageFmt(ftk::sevr, "Error opening the inputbank file: %s\n", inputBank.c_str());
		throw;
	}

	m_bankfile = TFile::Open(outBank.c_str(),"recreate");
	FTKSetup::PrintMessageFmt(ftk::info,"Output bank file created: %s\n",outBank.c_str());
}

TSPROOTBankGenerator::~TSPROOTBankGenerator(){

	 m_bankfile->Close();
	 delete m_bankfile;
}


/** produce the banks and the relations */
void TSPROOTBankGenerator::generate() throw (TSPPatternReadException){
  // read the head of the oroginal pattern bank
  int npatterns, nplanes;
  m_inputBank >> npatterns >> nplanes;
  FTKSetup::PrintMessageFmt(ftk::info, "Leaf bank db copy started\nInput bank: %d Patterns, %d Planes\n", npatterns, nplanes);
  
  // prepare the first bank
  FTKPattern *inputpattern = new FTKPattern(nplanes);
  TTree *bank0 = new TTree("Bank0","Level 0 pattern");
  bank0->Branch("Pattern",&inputpattern);
  npatterns = m_maxPatterns >= 0 ? m_maxPatterns : npatterns;
  
  // the first step is just the conversion of the original ASCII bank a TTree
  for (int ipatt = 0; ipatt < npatterns; ++ipatt) {
    showstats(ipatt, npatterns);
    readNextPattern(*inputpattern, ipatt);    
    // verify the pattern has the minimum requried coverage
    if (inputpattern->getCoverage()<m_mincoverage) {
      FTKSetup::PrintMessageFmt(ftk::info, "Minimum coverage cutoff at %d reached at pattern %d\n",m_mincoverage,ipatt);
      break; // don't add
    }
    bank0->Fill();
    m_patternID+=1; // increment the global pattern ID
  }
  bank0->Write();

  // Loop for each AM SS map to generate AM maps grouping the previous loop, i=0 is the TSP level
  for(unsigned i = 1; i < m_ssMaps.size(); i++){
    FTKSetup::PrintMessageFmt(ftk::info, "Bank conversion for %d patterns\n",m_patternID);
    m_patternMap.clear();
    m_patternID = 0; // reset the pattern counter
    generateChildren(i, nplanes);
  }

  if (m_RemoveTSP>0) {
    m_bankfile->Delete(Form("%s;*",bank0->GetName()));
  }
  m_setup->usageStat();
}

void TSPROOTBankGenerator::generateChildren(int bankID, int planes) throw (TSPPatternReadException){
	// get the TTree pointer for the TSP bank from the outfile
	TTree *TSPbank = dynamic_cast<TTree*>(m_bankfile->Get(Form("Bank%d",bankID-1)));
	FTKPattern *TSPpattern = 0x0;
	TSPbank->SetBranchAddress("Pattern",&TSPpattern);

	// create the TTree for the related AM bank
	TTree *AMbank = new TTree(Form("Bank%d",bankID),Form("AM bank generated groping Bank%d",bankID-1));
	FTKPattern *AMpattern = new FTKPattern(planes);
	AMbank->Branch("Pattern",&AMpattern);

	// create the TTree storing the relation for the TSP pattern and the link to the AM pattern
	TTree *TSPrelations = new TTree(Form("TSPRelation%d",bankID-1),"Relation from TSP to AM");
	int AMid;
	TSPrelations->Branch("AMID",&AMid,"AMID/I",100);
	unsigned int halfplanes = 0;
	TSPrelations->Branch("HalfPlaneMask",&halfplanes,"HalfPlaneMask/i",100);


	// create the object that stores all the information needed for the TSP
	TSPMap tspmap(m_ssMaps[bankID],m_ssMaps[bankID-1]);
	if (tspmap.getTotBits()>sizeof(halfplanes)*8) {
	  FTKSetup::PrintMessageFmt(ftk::sevr, "Total number of DC-bits exceeds the maximum currently supported: %u>%u\n",tspmap.getTotBits(),sizeof(halfplanes)*8);	
	}

	int nTSPpatterns = TSPbank->GetEntries();
	for (int iTSP=0;iTSP!=nTSPpatterns;++iTSP) {
	  TSPbank->GetEntry(iTSP);
	  AMpattern->setSectorID(TSPpattern->getSectorID());
	  AMpattern->setPatternID(m_patternID);
	  AMpattern->setPatternDBID(m_patternID);

	  showstats(iTSP, nTSPpatterns);
	  m_patternStringID.str("");
	  m_ssPattern.clear();
	  halfplanes = 0;

	  for(int i = 0; i < planes; i++){
	    // Get SS id
	    int ssid = TSPpattern->getSSID(i);

	    // Separate the phi and eta part TOFIX
	    int ssoff = tspmap.getDim(i)==2 ? m_ssMaps[bankID - 1]->getPhiOffset(false) : m_ssMaps[bankID - 1]->getPhiOffset(true);
	    int ssid_eta = ssid % ssoff;
	    
	    int newssid;
	    if (tspmap.getDim(i)==2) { // PXL case
	      // retrieve the number of bits used to codify the internal positions
	      const int &nbitsX = tspmap.getNBits(i,0);
	      const int &nbitsY = tspmap.getNBits(i,1);

	      int phioff;
	      int phimod;
	      int localX;
	      int etaoff;
	      int etamod;
	      int localY;
	      m_ssMaps[bankID - 1]->decodeSS(ssid, i, 0, phioff, phimod, localX, etaoff, etamod, localY);
	      
	      int phiwidth = m_ssMaps[bankID]->getPhiWidthRounded(i);
	      int phiss = m_ssMaps[bankID]->getMap(i, 0, 0).m_phiss;
	      int etawidth = m_ssMaps[bankID]->getEtaWidthRounded(i);
	      int etass = m_ssMaps[bankID]->getMap(i, 0, 0).m_etass;
	      
	      newssid = (phiwidth * phimod + localX) / phiss * ssoff + (etawidth * etamod + localY) / etass;
	      
	      // Half plane bit
	      unsigned int bitlayerX = (localX % phiss)/(m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiss); // between 0 and 2^(nbitsX)-1
	      unsigned int bitlayerY = (localY % etass)/(m_ssMaps[bankID-1]->getMap(i, 0, 0).m_etass); // between 0 and 2^(nbitsY)-1

	      // verify that there are not bits set after nbitsX, then nbitsY
	      if (bitlayerX&(~0<<nbitsX)) {
		FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerX[%d]=%d\n",iTSP,i,bitlayerX);
	      }
	      if (bitlayerY&(~0<<nbitsY)) {
		FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerY[%d]=%d\n",iTSP,i,bitlayerY);
	      }
	      
	      // convert the standard binary representation in Grey code
	      bitlayerX = tspmap.IEEE2GC(nbitsX,bitlayerX);
	      bitlayerY = tspmap.IEEE2GC(nbitsY,bitlayerY);
	      // encode the positions in the HB mask
	      halfplanes |= (bitlayerX | (bitlayerY<<tspmap.getInternalBitOffset(i,0))) << tspmap.getBitOffset(i);
	    } else if ( tspmap.getDim(i)==1) { // SCT case
	      // get the number of bits used to codify the internal position
	      const int &nbitsX = tspmap.getNBits(i,0);

	      int phioff;
	      int phimod;
	      int localX;
	      int etaoff;
	      m_ssMaps[bankID - 1]->decodeSS(ssid, i, 0, phioff, phimod, localX, etaoff);
	      
	      int phiwidth = m_ssMaps[bankID]->getPhiWidthRounded(i);
	      int phiss = m_ssMaps[bankID]->getMap(i, 0, 0).m_phiss;
	      newssid = (phiwidth * phimod + localX) / phiss * ssoff + ssid_eta;
	      
	      // Half plane bit
	      unsigned int bitlayerX = (localX % phiss)/(m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiss); // is 0 to (2^nbits-1)
	      // verify that there are not bits set after nbitsX
	      if (bitlayerX&(~0<<nbitsX)) {
		FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerX[%d]=%d\n",iTSP,i,bitlayerX);
	      }


	      // use the Grey code for the position
	      bitlayerX = tspmap.IEEE2GC(nbitsX,bitlayerX);

	      halfplanes |= bitlayerX << tspmap.getBitOffset(i);
	    }

	    AMpattern->setSSID(i,newssid);
	    m_patternStringID << "," << newssid;
	    m_ssPattern.push_back(newssid);
	  }

	  AMid = addAMPattern(bankID + 1, iTSP, m_patternStringID.str(), AMbank);

	  // store the TSP relation with the AM pattern
	  TSPrelations->Fill();
	}

	AMbank->Write();
	TSPrelations->Write();

	FTKSetup::PrintMessageFmt(ftk::info, "Generated %d AM patterns at level %d\n",m_patternID,bankID+1);	
}

int TSPROOTBankGenerator::addAMPattern(int /*bankID*/, int /*childID*/, const std::string& patternStringID, TTree *AMbank){
	pair<PatternMap::iterator, bool> res = m_patternMap.insert(make_pair(patternStringID, m_patternID));
	int fatherID = -1;

	if(res.second){ // the pattern doesn't exist
	  AMbank->Fill();
	  // fatherID is the position of this pattern in its own list
	  fatherID = m_patternID;
	  m_patternID++; // increment the counter for this level
	}else{
	  fatherID = res.first->second; // the AM pattern already exists
	}

	// TODO: add information on the AM --> TSP relation
	return fatherID;
}

void TSPROOTBankGenerator::showstats(int ipatt, int npatterns) const{
	static int statStep = std::max(npatterns / 10, 1);

	if(ipatt == 0 || ipatt % statStep == 0) {
		m_setup->usageStat();
		FTKSetup::PrintMessageFmt(ftk::info, "Converting pattern %d/%d\n", ipatt, npatterns);
	}
}

void TSPROOTBankGenerator::readNextPattern(FTKPattern& pattern, int patternID) throw (TSPPatternReadException) {
	m_inputBank >> pattern;
	if (!m_inputBank) throw TSPPatternReadException("Stream error reading pattern");
	if (pattern.getPatternID() != patternID) throw TSPPatternReadException("Synch error: read pattern id doesn't correspond to the expected id");
}

