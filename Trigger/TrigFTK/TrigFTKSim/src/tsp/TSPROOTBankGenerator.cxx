/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/tsp/TSPROOTBankGenerator.h"

#include "TrigFTKSim/tsp/TSPMap.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKPatternBySector.h"

#include <TTree.h>
#include <TBranch.h>

#include <boost/lexical_cast.hpp>

#include <cstdio>
#include <iostream>

#include "TrigFTKSim/FTK_AMBank.h"

using namespace TSPROOT;
using namespace boost;
using namespace std;


TSPROOTBankGenerator::TSPROOTBankGenerator(const char *fname) :
   m_RemoveTSP(0),
   m_setup(0),
   m_maxPatterns(0),
   m_planes(0),
   m_mincoverage(0),
   m_patternID(0),
   m_npatterns(0),
   m_iSub(0),
   m_nSub(0),
   m_nplanes(0),
   m_preader(NULL)
{
  m_bankfile = TFile::Open(fname);
}


TSPROOTBankGenerator::TSPROOTBankGenerator(FTKSetup* setup, const std::vector<FTKSSMap*>& ssMaps, const std::string& inputBank, const std::string& outBank, unsigned planes, int maxPatterns, int mincoverage) :

   m_RemoveTSP(0), m_setup(setup), m_ssMaps(ssMaps), m_maxPatterns(maxPatterns), m_planes(planes),  m_mincoverage(mincoverage), m_patternID(0l),
   m_iSub(0), m_nSub(0),m_preader(NULL)
{
 
  
   m_setup = &FTKSetup::getFTKSetup(); // ???
   // init input stream:
   // either root-formated (sector-ordered) patterns
   // or ascii file
   if ( FTKRootFile::Instance()->IsRootFile(inputBank.c_str()) ){
      FTKSetup::PrintMessageFmt(ftk::info,"TSPROOTBankGenerator. Reading inputBank (in root-format): %s.\n",inputBank.c_str());
      // input bank is in .root format
      // open rootfile
      TFile* file = (TFile*)FTKRootFile::Instance()->OpenRootFileReadonly(inputBank.c_str());
      if ( !file ) {
         FTKSetup::PrintMessageFmt(ftk::sevr, "Cannot read root-file from: %s\n", inputBank.c_str());
         throw;
      }
      m_preader = new FTKPatternBySectorForestReader(*file);
      if(!m_preader) {
         FTKSetup::PrintMessageFmt(ftk::sevr, "Cannot read root-file pattern from: %s\n", inputBank.c_str());
         throw;
      }
      m_npatterns = m_preader->GetNPatternsTotal();
      m_nplanes = m_preader->GetNLayers();
   }
   else {
      FTKSetup::PrintMessageFmt(ftk::info,"TSPROOTBankGenerator. Reading inputBank (in ascii-format): %s.\n",inputBank.c_str());
      // input bank is in ascii format
      if (!ftk_dcap::open_for_read(inputBank, m_inputBank)) {
	 FTKSetup::PrintMessageFmt(ftk::sevr, "Error opening the inputbank file: %s\n", inputBank.c_str());
	 throw;
      }
      m_inputBank >> m_npatterns >> m_nplanes;
   }
   m_npatterns = m_maxPatterns >= 0 ? m_maxPatterns : m_npatterns;

   // open outbank file 
   //m_bankfile = FTKRootFile::Instance()->CreateRootFile(outBank.c_str());
   m_bankfile = TFile::Open(outBank.c_str(),"recreate");
   FTKSetup::PrintMessageFmt(ftk::info,"Output bank file created: %s\n",outBank.c_str());
}

TSPROOTBankGenerator::~TSPROOTBankGenerator(){
   //FTKSetup::PrintMessageFmt(ftk::info,"~TSPROOTBankGenerator. Closing output bank file %s.\n",m_bankfile->GetName());
	 m_bankfile->Close();
	 delete m_bankfile;
}


/** write ssmaps, region map, plane map to file */
void TSPROOTBankGenerator::writeSSMapsToFile() {
   m_ssMaps[0]->WriteMapToRootFile(m_bankfile,"TSP");
   m_ssMaps[1]->WriteMapToRootFile(m_bankfile,"DC");
}


/** produce the banks and the relations */
void TSPROOTBankGenerator::generate() {
  // read the head of the oroginal pattern bank
   FTKSetup::PrintMessageFmt(ftk::info, "Leaf bank db copy started\nInput bank: %d Patterns, %d Planes, %d MaxPatterns\n", m_npatterns, m_nplanes, m_maxPatterns);

  // prepare the first bank
  FTKPattern* inputpattern = new FTKPattern(m_nplanes);
  TTree* bank0 = new TTree("Bank0","Level 0 pattern");
  bank0->Branch("Pattern",&inputpattern);

  // the first step is just the conversion of the original (ASCII or root) bank a TTree
  if ( m_preader ) {
     long npatt = m_preader->GetNPatternsTotal();

     // update number of patterns (for the current subregion)
     m_npatterns = m_maxPatterns >= 0 ? m_maxPatterns : npatt;
  
     // loop over sectors recursively, convert to 'FTKPattern' and write into root-tree
   // coverage loop
   for(int coverage=m_preader->GetMaxCoverage();coverage;
       coverage=m_preader->GetNextCoverage(coverage)) {
      if(m_patternID>=m_npatterns) break;
      // sector loop
      for(int sector=m_preader->GetCoverageFirstSector(coverage);sector>=0;
          sector=m_preader->GetCoverageNextSector(coverage,sector)) {
      if(m_patternID>=m_npatterns) break;
         if ( coverage < m_mincoverage ) break;
         FTKPatternOneSector *data=m_preader->Read(sector,coverage);
         // pattern loop
         for(FTKPatternOneSector::Ptr_t i=data->Begin();i!=data->End();i++) {
            if(m_patternID>=m_npatterns) break;
	   showstats(m_patternID, m_npatterns);
	   inputpattern->setPatternID(m_patternID);
	   inputpattern->setSectorID(sector);
	   inputpattern->setCoverage(coverage);
	   const FTKHitPattern& patternData=data->GetHitPattern(i);
	   for(unsigned int iLayer=0;iLayer<patternData.GetSize();iLayer++) {
	      inputpattern->setSSID(iLayer,patternData.GetHit(iLayer));
	   }

	   // fill tree
	   bank0->Fill();
	   m_patternID++; // increment the global pattern ID

	   // if(!tree->ReadNextPattern()) {
	   //    isEmpty=true;
	   //    break;
	   // }
         } // end loop over patterns 
         delete data;
      } // end loop over sectors	
   } // end loop over coverage
   
   delete m_preader;
   m_preader = NULL;
   m_npatterns = m_patternID;
  }
  else {
     for (int ipatt = 0; ipatt < m_npatterns; ++ipatt) {
	showstats(ipatt, m_npatterns);
	readNextPattern(*inputpattern, ipatt);    
	// verify the pattern has the minimum requried coverage
	if (inputpattern->getCoverage()<m_mincoverage) {
	   FTKSetup::PrintMessageFmt(ftk::info, "Minimum coverage cutoff at %d reached at pattern %d\n",m_mincoverage,ipatt);
	   break; // don't add
	}
	bank0->Fill();
	m_patternID+=1; // increment the global pattern ID
     }
  }

  // Write patterns to disk
  bank0->Write();

  // Loop for each AM SS map to generate AM maps grouping the previous loop, i=0 is the TSP level
  for(unsigned i = 1; i < m_ssMaps.size(); i++){
    FTKSetup::PrintMessageFmt(ftk::info, "Bank conversion for %d patterns\n",m_patternID);
    m_patternMap.clear();
    m_patternID = 0; // reset the pattern counter
    generateChildren(i, m_nplanes);
  }

  if (m_RemoveTSP>0) {
     FTKSetup::PrintMessageFmt(ftk::info, "Deleteing bank %s from file %s\n",bank0->GetName(),m_bankfile->GetName());
     m_bankfile->Delete(Form("%s;*",bank0->GetName()));
  }
  m_setup->usageStat();
}

void TSPROOTBankGenerator::generateChildren(int bankID, int planes) {
  // check which HW format should be used (0: global SSID, 1: not supported, 2: local compressed SSID
  int hwmode =  FTKSetup::getFTKSetup().getHWModeSS();
  TTree *TSPbank = dynamic_cast<TTree*>(m_bankfile->Get(Form("Bank%d",bankID-1)));
  if (TSPbank) {
     FTKPattern *TSPpattern = 0x0;
     TSPbank->SetBranchAddress("Pattern",&TSPpattern);
     // create the TTree for the related AM bank
     TTree *AMbank = new TTree(Form("Bank%d",bankID),Form("AM bank generated grouping Bank%d",bankID-1));
     if (AMbank) {
        FTKPattern *AMpattern = new FTKPattern(planes);
        AMbank->Branch("Pattern",&AMpattern);
        
        // create the TTree storing the relation for the TSP pattern and the link to the AM pattern
        // here, the HBmask Tree is created. This is the link between the above two trees
        TTree *TSPrelations = new TTree(Form("TSPRelation%d",bankID-1),"Relation from TSP to AM");
        int AMid;
        TSPrelations->Branch("AMID",&AMid,"AMID/I",100);
        unsigned int halfplanes = 0;
        TSPrelations->Branch("HalfPlaneMask",&halfplanes,"HalfPlaneMask/i",100);
        // there are two branches: AMID and the actual HBMask
        FTKSetup::PrintMessageFmt(ftk::info, "All trees are set up now\n");
        
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
           
           if (hwmode == 0) {
              // ********* GLOBAL SSID HW Format **********
              for(int i = 0; i < planes; i++) {
                 // Get SS id
                 int ssid = TSPpattern->getSSID(i);
                 
                 // Separate the phi and eta part TOFIX
                 int ssoff = tspmap.getDim(i)==2 ? m_ssMaps[bankID - 1]->getPhiOffset(false,FTKSetup::getFTKSetup().getITkMode()) : m_ssMaps[bankID - 1]->getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode());
                 int ssid_eta = ssid % ssoff;
                 
                 int newssid(0);
                 if (tspmap.getDim(i)==2) { // PXL case
                    // retrieve the number of bits used to codify the internal positions
                    const int &nbitsX = tspmap.getNBits(i,0);
                    const int &nbitsY = tspmap.getNBits(i,1);
                    
                    int phimod;
                    int localX;
                    int etamod;
                    int localY;
                    int section;
                    m_ssMaps[bankID - 1]->decodeSSxy(ssid, i, section, phimod, localX,etamod, localY);
                    
                    int phiwidth = m_ssMaps[bankID]->getPhiWidthRounded(i);
                    int phiss = m_ssMaps[bankID]->getMap(i, section, phimod).m_phiss;
                    int etawidth = m_ssMaps[bankID]->getEtaWidthRounded(i);
                    int etass = m_ssMaps[bankID]->getMap(i, section, etamod).m_etass;
                    
                    newssid = (phiwidth * phimod + localX) / phiss * ssoff + (etawidth * etamod + localY) / etass;
                    
                    // Half plane bit
                    unsigned int bitlayerX = (localX % phiss)/(m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiss); // between 0 and 2^(nbitsX)-1
                    unsigned int bitlayerY = (localY % etass)/(m_ssMaps[bankID-1]->getMap(i, 0, 0).m_etass); // between 0 and 2^(nbitsY)-1
                    
                    // verify that there are not bits set after nbitsX, then nbitsY
                    if (bitlayerX&(~0u<<nbitsX)) {
                       FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerX[%d]=%d\n",iTSP,i,bitlayerX);
                    }
                    if (bitlayerY&(~0u<<nbitsY)) {
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
                    int section;
                    int phimod;
                    int localX;
                    int etaoff;
                    m_ssMaps[bankID - 1]->decodeSSx(ssid, i, section, phimod, localX, etaoff);	      
                    int phiwidth = m_ssMaps[bankID]->getPhiWidthRounded(i);
                    int phiss = m_ssMaps[bankID]->getMap(i, section, phimod).m_phiss;
                    newssid = (phiwidth * phimod + localX) / phiss * ssoff + ssid_eta;	      
                    // Half plane bit
                    unsigned int bitlayerX = (localX % phiss)/(m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiss); // is 0 to (2^nbits-1)
                    // verify that there are not bits set after nbitsX
                    if (bitlayerX&(~0u<<nbitsX)) {
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
           }
           
           else if (hwmode == 2) {
              // int debug = 1;
              // ******** LOCAL COMPRESSED SSID HW format *********
              for(int i = 0; i < planes; i++){
                 // if (debug) printf ("we are in plane %d \n",i);
                 // Get SS id
                 int ssid = TSPpattern->getSSID(i);
	// if (debug) printf ("ssid: %d \n",ssid);
                 
	// ssid needs to be decoded
	// the ssid's are in the following format:
	// module ID*Etamax*Phimax | Eta part*Phimax | Phi part
	// in the end, at some point, they should be in the format:
	// module ID*modoff | MSB Eta part*etaoff | MSB Phi part*phioff | LSB Eta DC* PhiDCmax | LSB Phi DC
	// Here, it is sufficient to have them like this:
                 // module ID*modoff | MSB Eta part*etaoff | MSB Phi part*phioff
                 // meaning, to cut away the LSB's, since the information is present in the HBMask
                 
                 int newssid(0);
                 if (tspmap.getDim(i)==2) { // PXL case
                    // if (debug) printf ("Pixel case!!! ");
                    // get width of SS (phiss/etass) and width of module (phiwidth/etawidth)
                    int phiss = m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiss;
                    int phiwidth = m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiwidth;
                    int etass = m_ssMaps[bankID-1]->getMap(i, 0, 0).m_etass;
                    int etawidth = m_ssMaps[bankID-1]->getMap(i, 0, 0).m_etawidth;
                    // if (debug) printf ("phiss: %d, phiwidth: %d, etass: %d, etawidth: %d \n", phiss, phiwidth, etass, etawidth);
                    // how many SS are in a module?
                    int phimax = phiwidth/phiss;
                    if (phiwidth%phiss!=0) phimax += 1; // add 1 to keep not complete SS (round up)
                    int etamax = etawidth/etass;
                    if (etawidth%etass!=0) etamax += 1;
                    // if (debug) printf ("phimax: %d, etamax: %d \n", phimax,etamax);
                    
                    // Get number of DC bits that should be used in the end
                    // (retrieve the number of bits used to codify the internal positions)
                    const int &nbitsX = tspmap.getNBits(i,0);
                    // if (debug) printf("nDCbitsx: %d ",nbitsX);
                    const int &nbitsY = tspmap.getNBits(i,1);
	  // if (debug) printf("nDCbitsy: %d\n",nbitsY);
                    
   	  // Maximum of DC bits part - assume it should be bitwise!
	  // Not needed, if DC part is not added in the end
	  // int maxdcX = pow(2,nbitsX);
	  // int maxdcY = pow(2,nbitsY);
	  // if (debug) printf ("maxdcX: %d, maxdcY: %d \n",maxdcX,maxdcY);
                    
	  // Input SSID needs to be decoded
	  // For now assume no DC bits were set up to now
	  // Format is: module ID*Etamax*Phimax | Eta part*Phimax | Phi part
                    int ssid_phi = ssid % phimax;
                    int ssid_eta = (ssid / phimax) % etamax;
                    int ssid_mod = ssid / (phimax*etamax);
                    // if (debug) printf ("ssid: %d, ssid_phi: %d, ssid_eta: %d, ssid_mod: %d\n",ssid,ssid_phi,ssid_eta,ssid_mod);
                    
                    // Now, split eta and phi part into DC part (goes into LSB) and non-DC part
                    int eta_nondc = ssid_eta >> nbitsY;
                    int phi_nondc = ssid_phi >> nbitsX;
                    int eta_dc = ((1 << nbitsY)-1) & ssid_eta;
                    int phi_dc = ((1 << nbitsX)-1) & ssid_phi;
                    // if (debug) printf ("eta_nondc: %d, phi_nondc: %d, eta_dc: %d, phi_dc: %d\n",eta_nondc,phi_nondc,eta_dc,phi_dc);
                    // Do the gray-coding for the DC part
                    eta_dc = m_ssMaps[bankID-1]->gray_code(eta_dc,1);
                    phi_dc = m_ssMaps[bankID-1]->gray_code(phi_dc,1);
                    // if (debug) printf ("GRAY CODE! eta_dc: %d, phi_dc: %d\n",eta_dc,phi_dc);
                    
                    // Offsets for the non-dc eta and phi parts need to be adjusted
                    int etamax_nondc = etamax >> nbitsY;
                    int phimax_nondc = phimax >> nbitsX;
                    // if (debug) printf ("phimax: %d, etamax: %d, phimax_nondc: %d, etamax_nondc: %d\n", phimax,etamax,phimax_nondc,etamax_nondc);
                    
                    // The new SSID should be in format: module ID*modoff | non-DC Eta part*etaoff |  non-DC Phi part*phioff | LSB Eta DC* PhiDCmax | LSB Phi DC
                    // newssid = ssid_mod*(etamax_nondc*phimax_nondc*maxdcY*maxdcX) + eta_nondc*(phimax_nondc*maxdcY*maxdcX) + phi_nondc*(maxdcY*maxdcX) + eta_dc*maxdcX + phi_dc;
                    // if (debug) printf ("newssid: %d\n", newssid);
                    
                    // No, in fact it should be like: module ID*modoff | non-DC Eta part*etaoff |  non-DC Phi part*phioff
                    newssid = ssid_mod*(etamax_nondc*phimax_nondc) + eta_nondc*(phimax_nondc) + phi_nondc;
                    // if (debug) printf ("newssid: %d\n", newssid);
                    
                    // Half plane bit
                    // This should just be the original value of the LSB DC part - in Gray code!
                    unsigned int bitlayerX = phi_dc; // between 0 and 2^(nbitsX)-1
                    unsigned int bitlayerY = eta_dc; // between 0 and 2^(nbitsY)-1
                    // if (debug) printf ("bitlayerX: %d, bitlayerY: %d\n", bitlayerX,bitlayerY);
                    
                    // verify that there are not bits set after nbitsX, then nbitsY
                    if (bitlayerX&(~0u<<nbitsX)) {
                       FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerX[%d]=%d\n",iTSP,i,bitlayerX);
                    }
                    if (bitlayerY&(~0u<<nbitsY)) {
                       FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerY[%d]=%d\n",iTSP,i,bitlayerY);
                    }
                    
                    // encode the positions in the HB mask
                    // the encoding is: plane z | plane y | plane x with x<y<z
                    halfplanes |= (bitlayerX | (bitlayerY<<tspmap.getInternalBitOffset(i,0))) << tspmap.getBitOffset(i);
                    // if (debug) printf ("halfplanes: %d\n", halfplanes);
                 }
                 else if (tspmap.getDim(i)==1) { // SCT case
                    
                    // get width of SS (phiss/etass) and width of module (phiwidth/etawidth)
                    int phiss = m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiss;
                    int phiwidth = m_ssMaps[bankID-1]->getMap(i, 0, 0).m_phiwidth;
                    // how many SS are in a module?
                    int phimax = phiwidth/phiss;
                    if (phiwidth%phiss!=0) phimax += 1; // add 1 to keep not complete SS (round up)
                    
                    // Get number of DC bits that should be used in the end
                    // (retrieve the number of bits used to codify the internal positions)
                    const int &nbitsX = tspmap.getNBits(i,0);
                    
                    // Maximum of DC bits part - assume it should be bitwise! -> Not needed, LSB part cut away
                    // int maxdcX = pow(2,nbitsX);
                    
                    // Input SSID needs to be decoded
                    // For now assume no DC bits were set up to now
                    // Format is: module ID*Phimax | Phi part
                    int ssid_phi = ssid % phimax;
                    int ssid_mod = ssid / phimax;
                    
                    // Now, split phi part into DC part (goes into LSB) and non-DC part
                    int phi_nondc = ssid_phi >> nbitsX;
                    int phi_dc = ((1 << nbitsX)-1) & ssid_phi;
                    // Do the gray-coding for the DC part
                    phi_dc = m_ssMaps[bankID-1]->gray_code(phi_dc,1);
                    
                    // Offsets for the non-dc phi parts need to be adjusted
                    int phimax_nondc = phimax >> nbitsX;
                    
                    // The new SSID should be in format: module ID*modoff |  non-DC Phi part*phioff | LSB Phi DC
                    // newssid = ssid_mod*(phimax_nondc*maxdcX) + phi_nondc*maxdcX + phi_dc;
                    
                    // No, in fact it should be like this: module ID*modoff |  non-DC Phi part
                    newssid = ssid_mod*phimax_nondc + phi_nondc;
                    
                    // Half plane bit
                    // This should just be the original value of the LSB DC part - in Gray code!
                    unsigned int bitlayerX = phi_dc; // between 0 and 2^(nbitsX)-1
                    
                    // verify that there are not bits set after nbitsX, then nbitsY
                    if (bitlayerX&(~0u<<nbitsX)) {
                       FTKSetup::PrintMessageFmt(ftk::sevr,"Error creating the local-SS representation for pattern %d, bitlayerX[%d]=%d\n",iTSP,i,bitlayerX);
                    }
                    
                    // encode the positions in the HB mask
                    halfplanes |= bitlayerX << tspmap.getBitOffset(i);
                 }
                 AMpattern->setSSID(i,newssid);
                 m_patternStringID << "," << newssid;
                 m_ssPattern.push_back(newssid);
              }
           }
           else {  FTKSetup::PrintMessageFmt(ftk::sevr, "HWMODEID %d not supported. Please set HWMODEID to 0 ('old' HW format) or 2 (compressed SSID format).\n",hwmode); }
           
           AMid = addAMPattern(bankID + 1, iTSP, m_patternStringID.str(), AMbank);
           
           // store the TSP relation with the AM pattern
           TSPrelations->Fill();
        }
        
	FTKSetup::PrintMessageFmt(ftk::info, "Writing AMbank to root-file.\n");
        AMbank->Write();
	FTKSetup::PrintMessageFmt(ftk::info, "Writing TSPrelations to root-file.\n");
        TSPrelations->Write();
     }
  }
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

void TSPROOTBankGenerator::readNextPattern(FTKPattern& pattern, int patternID) {
	m_inputBank >> pattern;
	if (!m_inputBank) throw TSPPatternReadException("Stream error reading pattern");
	if (pattern.getPatternID() != patternID) throw TSPPatternReadException("Synch error: read pattern id doesn't correspond to the expected id");
}

