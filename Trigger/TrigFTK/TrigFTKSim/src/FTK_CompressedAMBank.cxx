/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_CompressedAMBank.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/tsp/TSPMap.h"

#include <sys/time.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TString.h>
#include <TString.h>
#include <TRegexp.h>

/*
  class FTK_CompressedAMBank
  Author: S.Schmitt
  Date 01/2015

  Implements a memory and CPU efficient emulation of the FTK pattern finding

 */

// #define PRINT_SS 1
#define PRINT_ROADS_SECTOR 1
// #define USE_FTKSSMAP_METHODS_ONLY
// #define PRINT_MULTIPLICITY
#define SKIP_USELESS_SECTORS

int const FTK_CompressedAMBank::MAX_NROAD=100000;

template<class A> inline void FTK_CompressedAMBank::patternLoop
(A &a,uint8_t const __restrict *ptr,uint32_t firstPattern,int nPattern)
   const {
   // first pattern ID is known
   a.initialize(firstPattern);
   a.process();
   // loop over all compressed patterns
   for(--nPattern;nPattern;nPattern--) {
      uint8_t d=*(ptr++);
      if(d>=0xf0) {
         // process multiple patterns with delta=1
         int di=d-0xef;
         nPattern-=di;
         do {
            a.update(1);
            a.process();
            di--;
         } while(di>=0);
      } else {
         // process one pattern
         uint32_t delta=1;
         while(d & 0x80) {
            // pattern with more than 7 bits
            //  add non-zero high bits in groups of 4 bits
            int shift=((d>>2)&0x3c)-25;
            delta += ((uint32_t)(d&0xf))<<shift;
            d=*(ptr++);
         }
         delta += d;
         // process one pattern
         a.update(delta);
         a.process();
      }
   }
}

FTK_CompressedAMBank::FTK_CompressedAMBank
(int bankID,int subID,FTKSSMap *ssMap,FTKSSMap *ssMapTSP,char const *name)
   : FTKLogging(name),FTK_AMsimulation_base(bankID,subID),
     m_SSmapTSP(ssMapTSP) {
   setSSMap(ssMap);
   if(getSSMap()&& getSSMapTSP()) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   } else {
      m_TSPmap=0;
   }
}

void FTK_CompressedAMBank::setNPlanes(int nPlanes) {
   //
   // set number of planes
   Info("setNPlanes")<<"nPlanes="<<nPlanes<<"\n";
   FTK_AMsimulation_base::setNPlanes(nPlanes);
   if(!m_TSPmap) {
      if(getSSMap()&& getSSMapTSP()) {
         m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
      } else {
         Fatal("setNPlanes")<<"missing SS maps: SSMap="<<getSSMap()
                            <<" SSMapTSP="<<getSSMapTSP()<<"\n";
      }
   }
   //
   // lookup-tables to convert TSP <-> DC
   m_TSPtoDC.resize(0);
   m_DCtoTSP.resize(0);
   m_TSPtoDC.resize(getNPlanes());
   m_DCtoTSP.resize(getNPlanes());

   // set up lookup-tables to convert compressed DC bits to subSSmask,DC,HB
   m_subSSmask.resize(getNPlanes());
   m_dcMaskLookup.resize(getNPlanes());
   m_hbMaskLookup.resize(getNPlanes());
   for(int ipl=0;ipl<getNPlanes();ipl++) {
      int nBitsX=m_TSPmap->getNBits(ipl,0);
      int nBitsY=0;
      if(m_TSPmap->getDim(ipl)==2) {
         nBitsY=m_TSPmap->getNBits(ipl,1);
      }
      unsigned n2= 1<<(2*(nBitsX+nBitsY));
      m_subSSmask[ipl].resize(n2);
      m_dcMaskLookup[ipl].resize(n2);
      m_hbMaskLookup[ipl].resize(n2);
      unsigned nPosX=1<<nBitsX;
      unsigned nPosY=1<<nBitsY;
      int maskX=nPosX-1;
      int maskY=nPosY-1;
      for(unsigned xy=0;xy<n2;xy++) {
         int hbx=xy & maskX;
         int dcx=(~(xy>>nBitsX)) & maskX;
         int hby=(xy>>(2*nBitsX)) & maskY;
         int dcy=(~(xy>>(2*nBitsX+nBitsY))) & maskY;
         int subMask=0;
         int subMaskOr=1;
         for(unsigned iy=0;iy<nPosY;iy++) {
            int notGCy= nBitsY ? ~m_TSPmap->IEEE2GC(nBitsY,iy) : -1;
            for(unsigned ix=0;ix<nPosX;ix++) {
               int notGCx=nBitsX ? ~m_TSPmap->IEEE2GC(nBitsX,ix) : -1;
               if(((((notGCx^hbx)|dcx)&maskX)==maskX) &&
                  ((((notGCy^hby)|dcy)&maskY)==maskY) ) {
                  subMask |= subMaskOr;
               }
               subMaskOr <<=1;
            }
         }
         m_subSSmask[ipl][xy]=subMask;
         m_dcMaskLookup[ipl][xy]=dcx | (dcy<<nBitsX);
         m_hbMaskLookup[ipl][xy]=hbx | (hby<<nBitsY);
      }
   }

   // lookup-table to determine number of bits in a word
   m_nHit16.resize(0x10000);
   for(int hitmask=0;hitmask<0x10000;hitmask++) {
      int n=0;
      for(int k=0;k<getNPlanes();k++) {
         if(hitmask & (1<<k)) n++;
      }
      m_nHit16[hitmask]=n;
   }
}

/**
   convertCachedBankFiles()

   read cached bank, write out sector-ordered root file
   dc bits are stored as extra SSID words

   arguments:
    list of cached banks 
      (could be from several subregions with same SSID encoding)
    output file name

    Note: this method writes a new file but does not load the pattern data
          into memory

       Use the method readSectorOrderedFile()
          to read the resulting file into memory

       Then use the method writeRootFile() to write out the bank file
          in a faster format 
*/
int FTK_CompressedAMBank::convertCachedBankFiles
(std::vector<char const *> &cachedBanks,char const *sectorOrderedFile) {
  TDirectory *patternsbysector=FTKRootFile::Instance()->CreateRootFile
    (sectorOrderedFile);
  FTKPatternBySectorWriter *writer=
    new FTKPatternBySectorWriter(*patternsbysector);
  FTKPatternWithCoverage *outPattern=0;
  // look over input banks (for merging several subregions in this step)
  for(unsigned iInput=0;iInput<cachedBanks.size();iInput++) {
    TDirectory *inputFile=
      FTKRootFile::Instance()->OpenRootFileReadonly(cachedBanks[iInput]);
    if(inputFile) {
      TTree *tree;
      // "Bank" tree is a list of FTKPattern objects
      inputFile->GetObject("Bank",tree);
      if(tree) {
	int nPattern=tree->GetEntriesFast();
	Info("ConvertCachedBankFiles")
	  <<"\""<<cachedBanks[iInput]<<"\" number of patterns: "
	  <<nPattern<<"\n";
	FTKPattern *pattern=0;
	tree->SetBranchAddress("Pattern",&pattern);
	// read all patterns from the cached bank
	for(int iPattern=0;iPattern<nPattern;++iPattern) {
	  tree->GetEntry(iPattern);
	  if(!(iPattern %1000000))
	    Debug("ConvertCachedBankFiles")
	      <<iPattern<<" "<<pattern->getSectorID()<<"\n";
	  if(!outPattern) {
	    outPattern=
	      new FTKPatternWithCoverage(pattern->getNPlanes()+2);
	    // number of layers is increaed by two
	    // plane N holds the DCMask
	    // plane N+1 holds the HBMask
	    writer->SetNLayers(pattern->getNPlanes()+2);
	  }
	  for(int i=0;i<pattern->getNPlanes();i++) {
	    // SSID layer 0..N-1
	    outPattern->SetHit(i,pattern->getSSID(i));
	  }
	  // DC and HB
	  outPattern->SetHit(pattern->getNPlanes(),
			     pattern->getDCMask());
	  outPattern->SetHit(pattern->getNPlanes()+1,
			     pattern->getHBMask());
	  // coverage is not used
	  outPattern->SetCoverage(1);
	  writer->AppendPattern(pattern->getSectorID(),*outPattern);
	}
      } else {
	Fatal("ConvertCachedBankFiles")
	  <<"failed to read TTree \"Bank\" from \""
	  <<cachedBanks[iInput]<<"\"\n";
	return 2;
      }
    } else {
      Fatal("ConvertCachedBankFiles")<<"failed to open root file \""
					     <<cachedBanks[iInput]<<"\"\n";
      return 1;
    }
  }
  delete writer;
  delete patternsbysector;
  return 0;
}

/**
   writeCachedBankFiles()

   dump patterns into a file, using the "cached bank" format
     (to be used by class FTKTSPBank)

*/
int FTK_CompressedAMBank::writeCachedBankFile
(char const *cachedBankFile) const {
   TDirectory *outputFile=
      FTKRootFile::Instance()->CreateRootFile(cachedBankFile);
   TTree *amtree=new TTree("Bank","Pattern bank");
   FTKPattern *curpatt = new FTKPattern(getNPlanes());
   amtree->Branch("Pattern", &curpatt);
   // set the number of planes once for all
   curpatt->setNPlanes(getNPlanes());
   // the coverage in the cache file is arbitrary set to -1
   curpatt->setCoverage(-1);
   // loop over sectors
   for(MAP<int,std::pair<int,int> >::const_iterator isector=
          m_SectorFirstLastPattern.begin();
       isector!=m_SectorFirstLastPattern.end();isector++) {
      int sector=(*isector).first;
      if(!(sector%100)) {
         Info("writeCachedBankFile")<<"sector="<<sector<<"\n";
      }
      int firstPattern=(*isector).second.first;
      int lastPattern=(*isector).second.second;
      // loop over all patterns in the sector
      for(int ipattern=firstPattern;ipattern<=lastPattern;ipattern++) {
         curpatt->clearSubPatterns();
         curpatt->setPatternID(ipattern);
         curpatt->setPatternDBID(ipattern+1);

         // unpack pattern info: SSID and DC bits
#ifdef BIT_PACKED_BANK
         // FIXME:  this code is partially duplicated in simulateAM()
         // the base bit is the start bit of the next pattern
         // the planes are extracted in reverse order
         uint64_t baseBit=
            ((uint64_t)(ipattern+1))*m_bank.m_PatternBitsTotal;
#endif
         //
         // DC bits and HB mask are composed inside the loop 
         int dcMask=0,hbmask=0;
         //
         // loop over planes
         for (int ipl=getNPlanes()-1;ipl>=0;--ipl) {
            // number of DC bits in this plane
            int nDCbits=m_TSPmap->getNBits(ipl);
            // number of DC+HB bits in this plane
            int nDCHBbits=2*nDCbits;
            unsigned dcSSID_with_DCbits;
            if(m_bank.m_pattern8Data[ipl].size()) {
               dcSSID_with_DCbits=m_bank.m_pattern8Data[ipl][ipattern];
            } else {
               dcSSID_with_DCbits=m_bank.m_pattern16Data[ipl][ipattern];
            }
            // SSID index is encoded in teh upper bits
            int dcSSIDindex=dcSSID_with_DCbits >> nDCHBbits;
            // SSID index is converted to SSID using lookup-table
            int dcSSID=m_bank.m_PatternByLayer[ipl].m_dcSSIDbySectorIndex
               [sector][dcSSIDindex];
            // DC and HB bits are encoded in the lower bits
            int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
            // use looupb-table to translate and add these bits to the
            //  DC,HB words
            dcMask =(dcMask<<nDCbits) | m_dcMaskLookup[ipl][dcBits];
            hbmask =(hbmask<<nDCbits) | m_hbMaskLookup[ipl][dcBits];
            // set SSID
            curpatt->setSSID(ipl,dcSSID);
         }
         // specify the sector
         curpatt->setSectorID(sector);
         // set DC,HB words
         curpatt->setDCMask(dcMask);
         curpatt->setHBMask(hbmask);
         // store this pattern
         amtree->Fill();
      }
   }
   // write tree
   amtree->Write("", TObject::kOverwrite);
   // close file
   delete outputFile;
   delete curpatt;
   return 0;
}

/**
   method to convert tspSSID (fine geometry)
   to a dcSSID (coarse geometry) and a subSS index 

   return.first   is the dcSSID
   return.second  is the subSS index

   the SSIDs are buffered in a map for efficient retreival
 */
std::pair<int,int> const &FTK_CompressedAMBank::getDCssid
(int layer,int tspSSID) {
   MAP<int,std::pair<int,int> >::const_iterator
      i=m_TSPtoDC[layer].find(tspSSID);
   if(i==m_TSPtoDC[layer].end()) {
      insertSSID(layer,tspSSID,-1);
      i=m_TSPtoDC[layer].find(tspSSID);
   }   
   return (*i).second;
}

/**
   method to convert dcSSID (coarse geometry)
   to a list of tspSSID (fine geometry)

   the SSIDs are buffered in a map for efficient retreival
 */
std::vector<int> const &FTK_CompressedAMBank::getTSPssidVector
(int layer,int dcSSID) {
   MAP<int,std::vector<int> >::const_iterator
      i=m_DCtoTSP[layer].find(dcSSID);
   if(i==m_DCtoTSP[layer].end()) {
      insertSSID(layer,-1,dcSSID);
      i=m_DCtoTSP[layer].find(dcSSID);
   }   
   return (*i).second;
}

#ifdef USE_FTKSSMAP_METHODS_ONLY
/**
   method to convert tspSSID (fine geometry)
   to a dcSSID (coarse geometry)

   FIXME: implement other HWModes
 */

int FTK_CompressedAMBank::getDCssidSlow
(int layer,int tspSSID) {
   int dcSSID=-1;
   int nDim=m_TSPmap->getDim(layer);
   if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      dcSSID = tspSSID;
   } else {
      if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         // ********* GLOBAL SSID HW Format **********
         int phioff,phimod,localX,etaoff;
         if (nDim==2) {
            // PXL case
            int etamod,localY;
            getSSMapTSP()->decodeSS
               (tspSSID,layer,0,phioff,phimod,localX,etaoff,etamod,localY);
            dcSSID=getSSMap()->getSS
               (layer,0,phimod,etamod,localX,localY);
         } else {
            // SCT case
            getSSMapTSP()->decodeSS
               (tspSSID,layer,0,phioff,phimod,localX,etaoff);
            dcSSID=getSSMap()->getSS
               (layer,0,phimod,etaoff,localX);
         }
      } else {
         Fatal("getDCssidSlow")
            <<"hardware mode="<<FTKSetup::getFTKSetup().getHWModeSS()
            <<" not supported\n";
      }
   }
   if(dcSSID<0) {
      Fatal("getDCssidSlow")
         <<"bad SSID conversion tsp="<<tspSSID
         <<" -> dc="<<dcSSID<<"\n";
   }
   return dcSSID;
}

/**
   method to convert dcSSID (coarse geometry) plus subSS index
   to a tspSSID (fine geometry)

   FIXME: implement other HWModes
 */
int FTK_CompressedAMBank::getTSPssidSlow
(int layer,int dcSSID,int tspXY) {
   int tspSSID=-1;
   int nDim=m_TSPmap->getDim(layer);
   int phissTSP =  getSSMapTSP()->getMap(layer, 0, 0).m_phiss;
   int etassTSP=0;
   int nbitsX = m_TSPmap->getNBits(layer,0);
   int ix=tspXY;
   int iy=0;
   if(nDim==2) {
      etassTSP =  getSSMapTSP()->getMap(layer, 0, 0).m_etass;
      iy= ix>>nbitsX;
      ix= ix & ((1<<nbitsX)-1);
   }
   // convert dc-SSID and iTSP to tsp-SSID
   if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      tspSSID=dcSSID;
   } else {
      if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         // ********* GLOBAL SSID HW Format **********
         int phioff,phimod,localX,etaoff;
         if (nDim==2) {
            // PXL case
            int etamod,localY;
            getSSMap()->decodeSS
               (dcSSID,layer,0,phioff,phimod,localX,etaoff,etamod,localY);
            localX += ix*phissTSP;
            localY += iy*etassTSP;
            tspSSID=getSSMapTSP()->getSS(layer,0,phimod,etamod,localX,localY);
         } else {
            // SCT case
            getSSMap()->decodeSS
               (dcSSID,layer,0,phioff,phimod,localX,etaoff);
            localX += ix*phissTSP;
            tspSSID=getSSMapTSP()->getSS(layer,0,phimod,etaoff,localX);
         }
      } else {
         Fatal("getTSPssidSlow")
            <<"hardware mode="<<FTKSetup::getFTKSetup().getHWModeSS()
            <<" not supported\n";
      }
   }
   if(tspSSID<0) {
      Fatal("getTSPssidSlow")
         <<"bad SSID conversion dc="<<dcSSID<<" xy="<<tspXY
         <<" -> tsp="<<tspSSID<<"\n";
   }
   return tspSSID;
}
#else
/**
   method to convert tspSSID (fine geometry)
   to a dcSSID (coarse geometry)

   FIXME: implement other HWModes
 */

int FTK_CompressedAMBank::getDCssidSlow
(int layer,int tspSSID) {
   int dcSSID=-1;
   int nDim=m_TSPmap->getDim(layer);
   if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      dcSSID = tspSSID;
   } else {
      if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         // ********* GLOBAL SSID HW Format **********
         int phioff,phimod,localX,etaoff;
         int phiwidthDC=getSSMap()->getPhiWidthRounded(layer);
         int phissDC=getSSMap()->getMap(layer, 0, 0).m_phiss;
         //int ssoffTSP=getSSMapTSP()->getPhiOffset(nDim!=2);

         // from TSPROOTBankGenerator
         int ssoffDC=getSSMapTSP()->getPhiOffset(nDim!=2);
         // FIXME: should this offset be taken from the DC map?
         // int ssoffDC=getSSMap()->getPhiOffset(nDim!=2);
         if (nDim==2) {
            // PXL case
            int etamod,localY;
            int etawidthDC=getSSMap()->getEtaWidthRounded(layer);
            int etassDC=getSSMap()->getMap(layer, 0, 0).m_etass;
            getSSMapTSP()->decodeSS
               (tspSSID,layer,0,phioff,phimod,localX,etaoff,etamod,localY);
            // encoding taken from TSPROOTBankGenerator
            dcSSID = (phiwidthDC*phimod+localX)/phissDC*ssoffDC+
               (etawidthDC*etamod+localY)/etassDC;
         } else {
            // SCT case
            getSSMapTSP()->decodeSS
               (tspSSID,layer,0,phioff,phimod,localX,etaoff);
            // encoding taken from TSPROOTBankGenerator
            dcSSID = (phiwidthDC*phimod+localX)/phissDC*ssoffDC+
               // original code from TSPROOTBankGenerator
               //(tspSSID%ssoffTSP);
               // FIXME: this should be "etaoff"?
               etaoff;
         }
      } else {
         Fatal("getDCssidSlow")
            <<"hardware mode="<<FTKSetup::getFTKSetup().getHWModeSS()
            <<" not supported\n";
      }
   }
   return dcSSID;
}

/**
   method to convert dcSSID (coarse geometry) plus subSS index
   to a tspSSID (fine geometry)

   FIXME: implement other HWModes
 */
int FTK_CompressedAMBank::getTSPssidSlow
(int layer,int dcSSID,int tspXY) {
   int tspSSID=-1;
   int nDim=m_TSPmap->getDim(layer);
   int phissTSP =  getSSMapTSP()->getMap(layer, 0, 0).m_phiss;
   int etassTSP=0;
   int nbitsX = m_TSPmap->getNBits(layer,0);
   int ix=tspXY;
   int iy=0;
   if(nDim==2) {
      etassTSP =  getSSMapTSP()->getMap(layer, 0, 0).m_etass;
      iy= ix>>nbitsX;
      ix= ix & ((1<<nbitsX)-1);
   }
   // convert dc-SSID and iTSP to tsp-SSID
   if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      tspSSID=dcSSID;
   } else {
      if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         // ********* GLOBAL SSID HW Format **********
         int phimod,localX;
         // from TSPROOTBankGenerator
         int ssoffDC=getSSMapTSP()->getPhiOffset(nDim!=2);
         // FIXME: should this offset be taken from the DC map?
         // int ssoffDC=getSSMap()->getPhiOffset(nDim!=2);

         int phissDC=getSSMap()->getMap(layer, 0, 0).m_phiss;

         int ssidPartPhi=(dcSSID / ssoffDC) *phissDC;
         int ssidPartEta=dcSSID % ssoffDC;

         int phiwidthDC=getSSMap()->getPhiWidthRounded(layer);

         phimod=ssidPartPhi / phiwidthDC;
         localX=ssidPartPhi % phiwidthDC;

         if (nDim==2) {
            // PXL case
            int etamod,localY;
            int etassDC=getSSMap()->getMap(layer, 0, 0).m_etass;
            int etawidthDC=getSSMap()->getEtaWidthRounded(layer);

            // inverse of the encoding in TSPROOTBankGenerator
            ssidPartEta *= etassDC;
            etamod = ssidPartEta/etawidthDC;
            localY = ssidPartEta%etawidthDC;

            localX += ix*phissTSP;
            localY += iy*etassTSP;
            tspSSID=getSSMapTSP()->getSS(layer,0,phimod,etamod,localX,localY);
            if(tspSSID<0) {
               int ssoffTSP=getSSMapTSP()->getPhiOffset(false);
               int phiwidthTSP=getSSMapTSP()->getPhiWidthRounded(layer);
               int etawidthTSP=getSSMapTSP()->getEtaWidthRounded(layer);
               int SSwidthxTSP = (phiwidthTSP-phiwidthTSP%phissTSP+phissTSP-1)/
                  phissTSP;
               int SSwidthyTSP = (etawidthTSP-etawidthTSP%etassTSP+etassTSP-1)/
                  etassTSP;
               tspSSID = (phimod*SSwidthxTSP+localX/phissTSP) * ssoffTSP +
                  etamod*SSwidthyTSP+localY/etassTSP;
               
            }
         } else {
            // SCT case
            int etaoff=ssidPartEta;

            localX += ix*phissTSP;

            tspSSID=getSSMapTSP()->getSS(layer,0,phimod,etaoff,localX);
            if(tspSSID<0) {
               int ssoffTSP=getSSMapTSP()->getPhiOffset(true);
               int phiwidthTSP=getSSMapTSP()->getPhiWidthRounded(layer);
               int SSwidthTSP = (phiwidthTSP-phiwidthTSP%phissTSP+phissTSP-1)/
                  phissTSP;
               /* std::cout<<" localX="<<localX
                        <<" phimod="<<phimod
                        <<" SSwidthTSP="<<SSwidthTSP
                        <<" phissTSP="<<phissTSP
                        <<" ssoffTSP="<<ssoffTSP
                        <<" phiwidthTSP="<<phiwidthTSP
                        <<" etaoff="<<etaoff
                        <<" phissDC="<<phissDC
                        <<" phiwidthDC="<<phiwidthDC                  
                        <<"\n"; */
               tspSSID = (phimod*SSwidthTSP+localX/phissTSP) * ssoffTSP +
                  etaoff;
               /* int phioff;
                  getSSMapTSP()->decodeSS
                  (tspSSID,layer,0,phioff,phimod,localX,etaoff);
                  std::cout<<" localX="<<localX
                        <<" phimod="<<phimod
                        <<" etaoff="<<etaoff
                        <<" phioff="<<phioff
                        <<"\n"; */
            }
         }
      } else {
         Fatal("getTSPssidSlow")
            <<"hardware mode="<<FTKSetup::getFTKSetup().getHWModeSS()
            <<" not supported\n";
      }
   }
   if(tspSSID<0) {
      Fatal("getTSPssidSlow")
         <<"bad SSID returned from FTKSSMap::getSS()"
         <<" layer="<<layer
         <<" dcSSID="<<dcSSID
         <<" tspXY="<<tspXY
         <<" tspSSID="<<tspSSID
         <<"\n";
   }
   int dcSSIDtest=getDCssidSlow(layer,tspSSID);
   if(dcSSIDtest != dcSSID) {
       Fatal("getTSPssidSlow")
          <<"bad SSID conversion layer="<<layer
          <<" dc="<<dcSSID<<" -> tsp="<<tspSSID
          <<" -> dc="<<dcSSIDtest<<"\n";
   }
   return tspSSID;
}
#endif

/**
   auxillary method to convert dcSSID (coarse geometry)
   to tspSSID (fine geometry) and back

   results are buffered in lookup-tables
 */
void FTK_CompressedAMBank::insertSSID(int layer,int tspSSID,int dcSSID0) {
   if(!m_TSPmap) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   }
   // if there is no dcSSID, convert TSP to DC
   int dcSSID=dcSSID0;
   if(tspSSID>=0) {
      dcSSID=getDCssidSlow(layer,tspSSID);
   }
   // create vector to hold all TSP for the given DC
   int nPos=1;
   if(!FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      nPos=1<<m_TSPmap->getNBits(layer);
   }
   std::vector<int> tsp(nPos);
   int error=0;
   // loop over all subSS indexes, get TSP from DC 
   for(int iTSP=0;iTSP<nPos;iTSP++) {
      tsp[iTSP]=getTSPssidSlow(layer,dcSSID,iTSP);
      // insert (DC,ssindex) into table ordered by TSP
      m_TSPtoDC[layer].insert
         (std::make_pair(tsp[iTSP],std::make_pair(dcSSID,iTSP)));
   }
   // insert vector<TSP> into table ordered by DC
   m_DCtoTSP[layer].insert(make_pair(dcSSID,tsp));
   if(error) {
      std::cout<<"insertSSID: DC0="<<dcSSID0<<" TSP="<<tspSSID
               <<" DC="<<dcSSID
               <<" ->";
      for(unsigned i=0;i<tsp.size();i++) {
         std::cout<<" TSP["<<i<<"]="<<tsp[i];
      }
      std::cout<<"\n";
      Fatal("insertSSID")<<"problem while converting SSIDs\n";
   }
}

/**
   readSectorOrderedFile()

   read sector-ordered patterns with DC bits

   and fill bank structures
   argument: file-name

    Note: this method is very slow

      Use the method writeRootFile() to write out the bank file
          in a faster format, then read the bank from the new file
*/
int FTK_CompressedAMBank::readSectorOrderedFile
(char const *sectorOrderedFile) {
   //
   // clear old bank data
   erase();
   if(!m_TSPmap) {
      Fatal("ReadSectorOrderedFile")<<"TSPmap not defined\n";
   }
   //
   // use methods to read sector-ordered patterns
   //  (originally used for pattrenb merging)
   TDirectory *patternsbysector=
      FTKRootFile::Instance()->OpenRootFileReadonly(sectorOrderedFile);
   FTKPatternBySectorReader *reader=
      new FTKPatternBySectorReader(*patternsbysector);
   //
   // in the sector-ordered file tehre are two extra planes
   //   these do not hold SSIDs bu they hold the DC and HB words
   //   see FTKTSPBank for the meaning of these words
   int nLayer=reader->GetNLayers()-2;
   setNPlanes(nLayer);
   //
   // prepare information to decode DC bits
   //   the element xyFromHBDCandLayer[layer][dchb]
   //   returns the valid subSS indexes for the given DC,HB combination
   std::vector< std::vector< std::vector<int> > > xyFromHBDCandLayer(nLayer) ;
   for(int iLayer=0;iLayer<nLayer;iLayer++) {
      int ndim=m_TSPmap->getDim(iLayer);
      int nBitsX=m_TSPmap->getNBits(iLayer,0);
      int nBitsY=0;
      if(ndim!=1) {
         nBitsY=m_TSPmap->getNBits(iLayer,1);
      }
      int nPosX=(1<<nBitsX);
      int nPosY=(1<<nBitsY);
      uint32_t maskX=nPosX-1;
      uint32_t maskY=nPosY-1;
      int nDC=nBitsX+nBitsY;
      int nDCHB=2*nDC;
      xyFromHBDCandLayer[iLayer].resize(1<<nDCHB);
      for(unsigned dchb=0;dchb<xyFromHBDCandLayer[iLayer].size();dchb++) {
         uint32_t hbx=dchb & maskX;
         uint32_t dcx=(dchb>>nBitsX) & maskX;
         uint32_t hby=(dchb>>(2*nBitsX)) & maskY;
         uint32_t dcy=(dchb>>(2*nBitsX+nBitsY)) & maskY;
         int xy=0;
         for(int iy=0;iy<nPosY;iy++) {
            uint32_t yNotGC=nBitsY ? ~m_TSPmap->IEEE2GC(nBitsY,iy) : -1;
            for(int ix=0;ix<nPosX;ix++) {
               uint32_t xNotGC=nBitsX ? ~m_TSPmap->IEEE2GC(nBitsX,ix) : -1;
              if(( (((xNotGC^hbx)|dcx)&maskX)==maskX ) &&
                 ( (((yNotGC^hby)|dcy)&maskY)==maskY )) {
                 xyFromHBDCandLayer[iLayer][dchb].push_back(xy);
              }
              xy++;
            }
         }
      }
   }
   uint32_t ipattSector=0;
   int nSector=0;
   m_bank.m_PatternByLayer.resize(nLayer);
   int maxSector=0;
   // determine maximum sector number
   for(int sector=reader->GetFirstSector();sector>=0;
       sector=reader->GetNextSector(sector)) {
      if(sector>maxSector) maxSector=sector;
   }
   for(int iLayer=0;iLayer<nLayer;iLayer++) {
      m_bank.m_PatternByLayer[iLayer].
         m_CompressedDeltaBySector.resize(maxSector);
   }
   //
   // sector loop
   for(int sector=reader->GetFirstSector();sector>=0;
       sector=reader->GetNextSector(sector)) {
      // read all patterns of this sector into memory
      FTKPatternOneSector *data=reader->MergePatterns(sector);
      // 
      // loop two times over all patterns in this sector
      //  first look: count patterns
      //  second loop: allocate memory and store patterns
      uint32_t ipatt;
      //
      struct CompressionHelper {
         //
         // previous pattern ID (to determine delta)
         uint32_t m_lastPattern;
         //
         // current position when storing compressed data
         uint32_t m_position;
         //
         // for compression: delta and repetition count
         uint32_t m_repeatDelta1;
      };
      //
      // adress auxillary information by layer and SSID
      std::vector<MAP<int,CompressionHelper> > compressionHelper(nLayer);
      for(int iLoop=0;iLoop<2;iLoop++) {
         ipatt=ipattSector;
         if(iLoop) {
            // allocate memory
            for(unsigned iLayer=0;iLayer<compressionHelper.size();iLayer++) {
               MAP<int,CompressionHelper>
                  &helperThisLayer=compressionHelper[iLayer];
               uint32_t nByte=0;
               for(MAP<int,CompressionHelper>::const_iterator ssid=
                      helperThisLayer.begin();ssid!=helperThisLayer.end();
                   ssid++) {
                  PatternBySector_t &sectorList
                     (m_bank.m_PatternByLayer[iLayer].
                      m_SSidData[(*ssid).first]);
                  PatternBySector_t::iterator dataPtr=
                     sectorList.find(sector);
                  SectorData &data=(*dataPtr).second;
                  data.m_offset=nByte;
                  data.m_length=(*ssid).second.m_position;
                  nByte+=data.m_length; 
               }
               m_bank.m_PatternByLayer[iLayer].
                  m_CompressedDeltaBySector[sector].resize(nByte);
            }
         }
         // loop over all patterns in this sector
         for(FTKPatternOneSector::Ptr_t patternPointer=data->Begin();
             patternPointer!=data->End();patternPointer++) {
            // DC information for this pattern
            uint32_t dcMaskAll=(*patternPointer).first.GetHit(nLayer);
            uint32_t hbMaskAll=(*patternPointer).first.GetHit(nLayer+1);
            //
            // loop over all planes
            for(int iLayer=0;iLayer<nLayer;iLayer++) {
               MAP<int,CompressionHelper>
                  &helperThisLayer=compressionHelper[iLayer];
               uint8_t *compressedDelta=m_bank.m_PatternByLayer[iLayer].
                  m_CompressedDeltaBySector[sector].data();
               //  decode DC information
               int nBitsX=m_TSPmap->getNBits(iLayer,0);
               int nBitsY=0;
               int ndim=m_TSPmap->getDim(iLayer);
               if(ndim!=1) {
                  nBitsY=m_TSPmap->getNBits(iLayer,1);
               }
               int maskX=(1<<nBitsX)-1;
               int maskY=(1<<nBitsY)-1;
               int dcx=dcMaskAll & maskX;
               dcMaskAll>>=nBitsX;
               int dcy=dcMaskAll & maskY;
               dcMaskAll>>=nBitsY;
               int hbx=hbMaskAll & maskX;
               hbMaskAll>>=nBitsX;
               int hby=hbMaskAll & maskY;
               hbMaskAll>>=nBitsY;
               int dchb=
                  (((((dcy<<nBitsY)|hby)<<nBitsX)|dcx)<<nBitsX)|hbx;
               // expand DC bits to the list of valid subSS indexes
               std::vector<int> const &tspIndexes=
                  xyFromHBDCandLayer[iLayer][dchb];
               int ssid=(*patternPointer).first.GetHit(iLayer);
               // loop over subSS indexes within dcSSID
               for(unsigned iTSP=0;iTSP<tspIndexes.size();iTSP++) {
                  // calculate TSP pattern address
                  uint32_t tspid=getTSPssid(iLayer,ssid,tspIndexes[iTSP]);
                  // find auxillary information
                  CompressionHelper &helper=helperThisLayer[tspid];
                  // find target section of the pattern bank
                  PatternBySector_t &sectorList
                     (m_bank.m_PatternByLayer[iLayer].m_SSidData[tspid]);
                  PatternBySector_t::iterator dataPtr=
                     sectorList.find(sector);
                  if(dataPtr==sectorList.end()) {
                     // first look, first pattern
                     // initialize data structure
                     SectorData &data=sectorList[sector];
                     // first pattern in this bank section, reset members
                     // store pattern ID
                     data.m_FirstPattern=ipatt;
                     // reset pattern counter
                     data.m_NPattern=1;
                     // reset compressed data counter
                     helper.m_position=0;
                     helper.m_repeatDelta1=0;
                  } else {
                     SectorData &data=(*dataPtr).second;
                     if(ipatt==data.m_FirstPattern) {
                        // second loop, first pattern
                        // reset pattern counter
                        data.m_NPattern=1;
                        // reset position and count of repeated deltas
                        helper.m_position=0;
                        helper.m_repeatDelta1=0;
                     } else {
                        // calculate delta to previous pattern 
                        uint32_t delta=ipatt-helper.m_lastPattern-1;
                        // number of bytes needed to store this delta
                        uint32_t d=(iLoop==0) ? 1 : helper.m_position;
                        uint32_t mask4=0x780;
                        int shift=7;
                        // check whether some bits >=7 are nonzero
                        //  -> need extra bytes to store this delta
                        for(uint32_t mask=0xffffff80;mask;mask<<=4) {
                           if((delta & mask)==0) break;
                           uint32_t bits=delta & mask4;
                           if(bits) {
                              if(iLoop) {
                                 compressedDelta[data.m_offset+d]=
                                    (bits>>shift)|((shift+25)<<2);
                              }
                              d++;
                           }
                           mask4 <<=4;
                           shift+=4;
                        }
                        // count number of patterns
                        (*dataPtr).second.m_NPattern++;
                        // count number of data bytes needed
                        if(!delta) {
                           if((helper.m_repeatDelta1<1)||
                              (helper.m_repeatDelta1>=16)) {
                              helper.m_repeatDelta1=1;
                              if(!iLoop) {
                                 helper.m_position++;
                              } else {
                                 compressedDelta[data.m_offset+d]=0;
                                 d++;
                              }
                           } else {
                              helper.m_repeatDelta1++;
                              if(iLoop) {
                                 compressedDelta[data.m_offset+d-1]=
                                    helper.m_repeatDelta1 + 0xee;
                              }
                           }
                        } else {
                           if(!iLoop) {
                              helper.m_position += d;
                           } else {
                              compressedDelta[data.m_offset+d]=delta & 0x7f;
                              d++;
                           }
                        }
                        if(iLoop) {
                           helper.m_position=d;
                        }
                     }
                  }
                  helper.m_lastPattern=ipatt;
               }
            }
            ipatt++;
         }
      }
      ipattSector=ipatt;
      delete data;
      if((!(sector%100))||
         (sector<10)||
         ((sector<100)&&(!(sector%10)))) {
         Debug("ReadSectorOrderedFile")
            <<"sector "<<sector<<" nPatt="<<ipatt<<"\n";
      }
      nSector++;
   }
   Info("ReadSectorOrderedFile")
      <<"nLayer="<<m_bank.m_PatternByLayer.size()<<" nSector="<<nSector
      <<" nPattern(DC-encoded)="<<ipattSector<<"\n";
   readBankPostprocessing("ReadSectorOrderedFile");
   delete reader;
   delete patternsbysector;
   return 0;
}

/**
   writeBinaryFile()

   write out pattern bank as binary file

   Fast but machine dependent. Use only for debugging.
*/
int FTK_CompressedAMBank::writeBinaryFile
(char const *binaryFile) const {
  int error=0;
  int32_t sectorCountTotal=0;
  int32_t ssidCountTotal=0;
  std::ofstream ssidBinary(binaryFile);
  if(!ssidBinary.is_open()) {
    Fatal("WriteBinaryFile")
      <<"failed to create file \""<<binaryFile<<"\"\n";
    return 1;
  }
  // number of layers
  int32_t layerCount=m_bank.m_PatternByLayer.size();
  ssidBinary.write((const char *)&layerCount,sizeof(int32_t));
  // loop over all layers
  for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
     LayerData const &layerData=m_bank.m_PatternByLayer[layer];
     int32_t ssidCount=layerData.m_SSidData.size();
     Debug("WriteBinaryFile")
        <<"layer="<<layer
        <<" number of SSIDs="<<ssidCount
        <<" file="<<binaryFile
        <<"\n";
     ssidBinary.write((const char *)&ssidCount,sizeof(int32_t));
     if(ssidBinary.fail()) {
        Error("WriteBinaryFile")
           <<"problem to write header layer="<<layer<<"\n";
        error++;
        break;
     }
     ssidCountTotal += ssidCount;
     // loop over all SSIDs in this layer
     for(PatternBySectorSSidMap_t::const_ptr ssid=
           layerData.m_SSidData.beginPtr();
	ssid!=layerData.m_SSidData.endPtr();++ssid) {
      int32_t ssidID=(*ssid).first;
      int32_t sectorCount = (*ssid).second.size();
      ssidBinary.write((const char *)&ssidID,sizeof(int32_t));
      if(ssidBinary.write((const char *)&sectorCount,sizeof(int32_t)).
	 fail()) {
         error++;
        Error("WriteBinaryFile")
           <<"problem to write sector count layer="<<layer
           <<" ssid="<<ssidID<<" count="<<sectorCount<<"\n";
      }
      sectorCountTotal += sectorCount;
      // loop over sectors with several patterns
      for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
          sector!=(*ssid).second.endPtr();++sector) {
	int32_t sectorID=(*sector).first;
	ssidBinary.write((const char *)&sectorID,sizeof(int32_t));
	SectorData const &data=(*sector).second;
        int32_t firstPattern;
        firstPattern=data.m_FirstPattern;
	ssidBinary.write((const char *)&firstPattern,sizeof(int32_t));
	ssidBinary.write((const char *)&data.m_NPattern,sizeof(int32_t));
	uint32_t nData=data.m_length;
	ssidBinary.write((const char *)&nData,sizeof(uint32_t));
	if(nData>0)
           // here the bulk of the data is written
           //  dump all pattern data for this layer/ssid/sector
           ssidBinary.write((const char *)
                            m_bank.m_PatternByLayer[layer].
                            m_CompressedDeltaBySector[sectorID].data(),
                            nData*sizeof(uint8_t));
        if(ssidBinary.fail()) {
           Error("WriteBinaryFile")
              <<"problem to write sector data layer="<<layer
              <<" ssid="<<ssidID<<" sector="<<sectorID
              <<" nPattern="<<data.m_NPattern
              <<" nData="<<nData<<"\n";
           error++;
           if(error) break;
        }
      }
      if(error) break;
    }
    if(error) break;
  }
  if(error) {
    Error("WriteBinaryFile")
      <<"problem with writing binary file \""
      <<binaryFile
      <<"\"\n";
  }
  Info("WriteBinaryFile")
    <<"number of SSIDs="<<ssidCountTotal
    <<" total number of sectors="<<sectorCountTotal<<"\n"
    <<" average number of sectors per SSID="
    <<sectorCountTotal/(double)ssidCountTotal<<"\n";
  return error;
}

void FTK_CompressedAMBank::erase(void) {
   m_bank.m_PatternByLayer.resize(0);
   m_bank.m_pattern8Data.resize(0);
   m_bank.m_pattern16Data.resize(0);
   setNPlanes(0);
}

/**
   readBinaryFile()

   read bank data from a binary file

   Fast but machine dependent. Use only for debugging.
*/
int FTK_CompressedAMBank::readBinaryFile
(char const *binaryFile) {
   erase();
   ifstream ssidBinary(binaryFile);
   if(!ssidBinary.is_open()) {
      Fatal("ReadBinaryFile")
         <<"failed to open file \""<<binaryFile<<"\"\n";
      return 1;
   }
   // number of layers
   uint32_t nLayer=0;
   ssidBinary.read((char *)&nLayer,sizeof(int32_t));
   int error=0;
   if(ssidBinary.fail()) {
      error++;
      nLayer=0;
   }
   Debug("ReadBinaryFile")<<"Number of layers: "<<nLayer<<"\n";
   m_bank.m_PatternByLayer.resize(nLayer);
   setNPlanes(nLayer);
   for(uint32_t layer=0;layer<nLayer;layer++) {
      int32_t ssidCount;
      ssidBinary.read((char *)&ssidCount,sizeof(int32_t));
      if(ssidBinary.fail()) {
         if(!error) {
            Error("ReadBinaryFile")
               <<"problem to read layer #"<<layer
               <<" nSSid="<<ssidCount<<"\n";
         }
         error++;
         break;
      }
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      Debug("ReadBinaryFile")
         <<"layer="<<layer<<" number of SSIDs="<<ssidCount<<"\n";
    //PatternBySectorSSidMap_t &layerMap=ssidMap[layerId];
      for(int iSSid=0;iSSid<ssidCount;iSSid++) {
         int32_t ssidID=-1;
         int32_t sectorCount=-1;
         ssidBinary.read((char *)&ssidID,sizeof(int32_t));
         ssidBinary.read((char *)&sectorCount,sizeof(int32_t));
         if(ssidBinary.fail()) {
            if(!error) {
               Error("ReadBinaryFile")
                  <<"problem to read ssid #"<<iSSid
                  <<" (layer="<<layer
                  <<") ID="<<ssidID<<" nSector="<<sectorCount<<"\n";
            }
            error++;
            break;
         }
         for(int iSector=0;iSector<sectorCount;iSector++) {
            int32_t sectorID=-1;
            int32_t firstPattern=-1;
            int32_t nPattern=-1;
            uint32_t nData=-1;
            ssidBinary.read((char *)&sectorID,sizeof(int32_t));
            ssidBinary.read((char *)&firstPattern,sizeof(int32_t));
            ssidBinary.read((char *)&nPattern,sizeof(int32_t));
            ssidBinary.read((char *)&nData,sizeof(int32_t));
            SectorData &sectorData=layerData.m_SSidData[ssidID][sectorID];
            sectorData.m_FirstPattern=firstPattern;
            sectorData.m_NPattern=nPattern;
            if((int)m_bank.m_PatternByLayer[layer].
               m_CompressedDeltaBySector.size()<sectorID+1) {
               m_bank.m_PatternByLayer[layer].
                  m_CompressedDeltaBySector.resize(sectorID+1);
            }
            sectorData.m_offset=
               m_bank.m_PatternByLayer[layer].
               m_CompressedDeltaBySector[sectorID].size();
            sectorData.m_length=nData;
            if(nData) {
               m_bank.m_PatternByLayer[layer].
                  m_CompressedDeltaBySector[sectorID].resize
                  (sectorData.m_offset+nData);
               ssidBinary.read((char*)m_bank.m_PatternByLayer[layer].
                               m_CompressedDeltaBySector
                               [sectorID].data()+sectorData.m_offset,
                               nData*sizeof(uint8_t));
            }
            if(ssidBinary.fail()) {
               if(!error) {
                  Error("ReadBinaryFile")
                     <<"problem to read sector data #"<<iSector
                     <<" (layer="<<layer<<" ssid="<<ssidID
                     <<") ID="<<sectorID
                     <<" firstPattern="<<firstPattern
                     <<" nPattern="<<nPattern
                     <<" nData="<<nData
                     <<"\n";
               }
               error++;
               break;
            }
         }
      }
   }
   if(error) {
      Fatal("ReadBinaryFile")
         <<"nError="<<error<<" reading SSID banks from "<<binaryFile<<"\n";
      return error;
   }
   readBankPostprocessing("ReadBinaryFile");
   return 0;
}

/**
   readBankPostprocessing()

   unpack patern data and create a "classical" pattern bank
   indexed by pattern ID
   also fill other auxillary tables and print some information
*/
void FTK_CompressedAMBank::readBankPostprocessing(char const *where) {
   // total number of distinct (plane,ssid)
   uint32_t ssidCountTotal=0;
   // total number of sectors
   uint32_t sectorCountTotal=0;
   // count of TSP-SSIDs used to form the patterns (by plane)
   std::vector<uint32_t> patternTSPcount(getNPlanes());
   // total number of patterns
   int32_t patternCountTotal=-1;
   // memory consumption (estimate)
   uint32_t memorySSIDlookup=0;
   uint32_t memoryPatternIDlookup=0;

   m_SectorFirstLastPattern.clear();

   if(!m_TSPmap) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   }

   // this s a temporary lookup-table
   //  it converts a layer,sector,ssid
   //  to an index
   //
   // the inverse table (for unpacking the bank data) is stored in
   //  a data member
   //    for packing the bank data (local variable in this method):
   //      index=dcSSIDBySectorLayer[layer][sector][ssid]
   //    for unpacking bank data (data member):
   //      ssid=m_bank.m_PatternByLayer[layer].m_SSidData[sector][index
   std::vector<MAP<int,MAP<int,int> > >
      dcSSIDBySectorLayer(m_bank.m_PatternByLayer.size());

   //
   // pack data into VECTORMAP structures
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      for(PatternBySectorSSidMap_t::iterator iSSID=
             layerData.m_SSidData.begin();
          iSSID!=layerData.m_SSidData.end();iSSID++) {
         (*iSSID).second.pack();
      }
      layerData.m_SSidData.pack();
   }

   //
   // table of delta data
   std::vector<uint32_t> deltaMultiplicity(0x100);

   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData const &layerData=m_bank.m_PatternByLayer[layer];
      ssidCountTotal += layerData.m_SSidData.size();
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         sectorCountTotal += (*ssid).second.size();
         // get DC-level SSID
         int dcSSID=getDCssid(layer,(*ssid).first).first;
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            int sectorID=(*sector).first;
            // remember DC-level SSID for this sector
            // set index to -1 (undefined)
            dcSSIDBySectorLayer[layer][sectorID][dcSSID]=-1;
            // unpack bank data to determine min/max pattern ID in this
            //  sector
            SectorData const &sectordata=(*sector).second;
            patternTSPcount[layer]+=sectordata.m_NPattern;
            class GetMinMax {
            public:
               inline void initialize(int firstPattern) {
                  minPatternId=firstPattern;
                  maxPatternId=firstPattern;
               }
               inline void process(void) { }
               inline void update(int delta) {
                  maxPatternId+=delta;
               }
               int minPatternId,maxPatternId;
            } getMinMax;
            patternLoop(getMinMax,
                        layerData.m_CompressedDeltaBySector[sectorID].data()+
                        sectordata.m_offset,
                        sectordata.m_FirstPattern,
                        sectordata.m_NPattern);
            int minPatternId=getMinMax.minPatternId;
            int maxPatternId=getMinMax.maxPatternId;
            if(patternCountTotal<maxPatternId)
               patternCountTotal=maxPatternId;
            MAP<int,std::pair<int,int> >::iterator is=
               m_SectorFirstLastPattern.find(sectorID);
            if(is==m_SectorFirstLastPattern.end()) {
               m_SectorFirstLastPattern.insert
                  (std::make_pair
                   (sectorID,std::make_pair(minPatternId,maxPatternId)));
            } else {
               if((*is).second.first>minPatternId) 
                  (*is).second.first=minPatternId;
               if((*is).second.second<maxPatternId)
                  (*is).second.second=maxPatternId;
            }
            // estimate memory consumption
            memorySSIDlookup+=sizeof(uint8_t)*sectordata.m_length;
         }
         // memory for this SSID map
         //   [parent, previous,next,child] plus vector [ptr,size]
         memorySSIDlookup+= (*ssid).second.getMemoryEstimate();

      }
      memorySSIDlookup += sizeof(LayerData)
         +layerData.m_SSidData.getMemoryEstimate();
   }
   // number of patterns = maximum patternID+1
   patternCountTotal++;
   m_hitPatterns.resize(patternCountTotal);
   m_roadCand.resize(MAX_NROAD);
   //
   // get sector number from pattern ID
   //   m_PatternToSector.lower_bound(pattern) returns pointer to sector
#ifdef CANDIDATES_WITHOUT_SECTORID
   for(MAP<int,std::pair<int,int> >::const_iterator
          iSector=m_SectorFirstLastPattern.begin();
       iSector!=m_SectorFirstLastPattern.end();iSector++) {
      m_patternToSector[(*iSector).second.second]=(*iSector).first;
   }
#endif
#ifdef PRINT_MULTIPLICITY
   MAP<uint64_t,int> histogram;
   for(int i=0;i<0x100;i++) {
      histogram[(((uint64_t)deltaMultiplicity[i])<<8)|i]=i;
   }
   uint32_t sum=0;
   for(MAP<uint64_t,int>::const_iterator i=histogram.begin();
       i!=histogram.end();i++) {
      sum += ((*i).first>>8);
      std::cout<<std::setbase(16)<<std::setw(3)<<(*i).second
               <<std::setbase(10)<<std::setw(10)<<((*i).first>>8)
               <<std::setw(10)<<sum
               <<"\n";
   }
#endif
   // determine minimum and maximum number of patterns per sector
   int minPattern=-1,maxPattern=-1;
   for(MAP<int,std::pair<int,int> >::const_iterator
          is=m_SectorFirstLastPattern.begin();
       is!=m_SectorFirstLastPattern.end();is++) {
      int nPattern=(*is).second.second-(*is).second.first+1;
      if(maxPattern==-1) {
         minPattern=nPattern;
         maxPattern=nPattern;
      } else {
         if(minPattern>nPattern) minPattern=nPattern;
         if(maxPattern<nPattern) maxPattern=nPattern;
      }
   }
   int maxSector=(*m_SectorFirstLastPattern.rbegin()).first+1;

   // determine number of dc-SSIDs per layer/sector
   //   and set up translation tables
   m_bank.m_pattern8Data.resize(getNPlanes());
   m_bank.m_pattern16Data.resize(getNPlanes());
   for(unsigned layer=0;layer<dcSSIDBySectorLayer.size();layer++) {
      unsigned iMax=0;
      // layer loop
      m_bank.m_PatternByLayer[layer].m_dcSSIDbySectorIndex.resize(maxSector);
      for(MAP<int,MAP<int,int> >::iterator sector=
             dcSSIDBySectorLayer[layer].begin();
          sector!=dcSSIDBySectorLayer[layer].end();sector++) {
         // sector loop
         //
         VECTOR<int> &dcSSIDtable=
            m_bank.m_PatternByLayer[layer].m_dcSSIDbySectorIndex
            [(*sector).first];
         //
         // size of the table is the number of distinct SSIDs in this
         //     layer,sector
         dcSSIDtable.resize((*sector).second.size());
         memoryPatternIDlookup += sizeof(int)*dcSSIDtable.size();
         //
         // assign indexes for each SSID given (layer,sector)
         int i=0;
         for(MAP<int,int>::iterator ssid=(*sector).second.begin();
             ssid!=(*sector).second.end();ssid++) {
            // set up table to encode SSID->index
            (*ssid).second=i;
            // set up table to decode index->SSID
            dcSSIDtable[i++]= (*ssid).first;
         }
         // maximum index in this layer
         if(dcSSIDtable.size()>iMax) iMax=dcSSIDtable.size();
      }
      // estimate memory consumption for vector 
      memoryPatternIDlookup += sizeof(std::vector<int>)*maxSector*getNPlanes();
      // determine nunber of bits neede to fit the maximum index
      int nbit;
      for(nbit=0;iMax>(1U<<nbit);nbit++);
      //
      // add space for ternary bits
      nbit+= 2*m_TSPmap->getNBits(layer);
      Info(where)<<"layer="<<layer<<" #dcSSID="<<iMax
                 <<" DC bits="<< m_TSPmap->getNBits(layer)
                 <<" nbit="<<nbit<<"\n";
      // pattern bank data is stored in 8-bit or 16-bit words per layer
      if(nbit<=8) {
         m_bank.m_pattern8Data[layer].resize(patternCountTotal);
         memoryPatternIDlookup += sizeof(uint8_t)*patternCountTotal;
      } else {
         m_bank.m_pattern16Data[layer].resize(patternCountTotal);
         memoryPatternIDlookup +=  sizeof(uint16_t)*patternCountTotal;
      }
   }

   //
   // fill bank indexed by patternID

   // layer loop
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData const &layerData=m_bank.m_PatternByLayer[layer];
      //
      // encoding table dcIndexTable[sector][SSID] for the given layer
      MAP<int,MAP<int,int> >
         const &dcIndexTable(dcSSIDBySectorLayer[layer]);
      //
      // number of DC bits in X,Y
      int nBitsX=m_TSPmap->getNBits(layer,0);
      int nBitsY=0;
      if(m_TSPmap->getDim(layer)==2) {
         nBitsY=m_TSPmap->getNBits(layer,1);
      }
      int nPosX=1<<nBitsX;
      int nPosY=1<<nBitsY;
      // total number of ternary bits in this layer
      int hbBitsLayer=nBitsX+nBitsY;
      // total number of DC+HB bits in this layer
      int dcHbBitsLayer=2*hbBitsLayer;
      //
      // lookup-tables to encode DC bits
      std::vector<uint64_t> dcBitsLookup1(1<<hbBitsLayer);
      std::vector<uint64_t> dcBitsLookup2(1<<(4*dcHbBitsLayer));
      //
      // fill:   dcBitsLookup1[xy]
      //
      //   given the xy coordinates in the TSP map relative to the DC map
      //   this returns the encoded HB bits for this layer
      //   initial setting of DC bits
      unsigned xy1=0;
      for(int iy=0;iy<nPosY;iy++) {
         unsigned hbY=nBitsY ? m_TSPmap->IEEE2GC(nBitsY,iy) : 0;
         for(int ix=0;ix<nPosX;ix++) {
            unsigned hbX=nBitsX ? m_TSPmap->IEEE2GC(nBitsX,ix) : 0;
            // dc bits for first pattern
            // upper bits: ~DC  (all on: no ternary bits)
            unsigned notDcX=nPosX-1;
            unsigned notDcY=nPosY-1;
            dcBitsLookup1[xy1++]=
               (((((notDcY<<nBitsY)|hbY)<<nBitsX)|notDcX)<<nBitsX)|hbX;
         }
      }
      //
      // fill:        dcBitsLookup2[][]
      //
      //   this table serves for ORing a new TSP-space pattern to an existing
      //      (encoded) DC-space pattern
      //   it returns the logical OR of 
      //   two encoded DCHB patterns
      //     dchbNew = dcBitsLookup2[dchb1][dchb2]
      for(unsigned xy1xy2=0;xy1xy2<dcBitsLookup2.size();xy1xy2++) {
         unsigned xy[2],hbx[2],hby[2],dcx[2],dcy[2];
         for(int i=0;i<2;i++) {
            xy[i]=(xy1xy2>>(2*i*hbBitsLayer)) &
               ((1<<(2*hbBitsLayer))-1);
            hbx[i]=xy[i] & (nPosX-1);
            dcx[i]=(xy[i]>>nBitsX) & (nPosX-1);
            hby[i]=(xy[i]>>(2*nBitsX)) & (nPosY-1);
            dcy[i]=(xy[i]>>(2*nBitsX+nBitsY)) & (nPosY-1);
            if(nBitsX) hbx[i]=m_TSPmap->IEEE2GC(nBitsX,hbx[i]);
            if(nBitsY) hby[i]=m_TSPmap->IEEE2GC(nBitsY,hby[i]);
         }
         if(!xy[0]) {
            // xy[0]=0 means that xy[1] is the first pattern entered 
            dcBitsLookup2[xy1xy2]=xy[1];
         } else if(!xy[1]) {
            // xy[1]=0 means that xy[0] is the first pattern entered 
            dcBitsLookup2[xy1xy2]=xy[0];
         } else {
            // update notDC bits
            // if hb bits are different, clear the bits
            unsigned notDcX= dcx[0] & dcx[1] & ~(hbx[0]^hbx[1]);
            unsigned notDcY= dcy[0] & dcy[1] & ~(hby[0]^hby[1]);
            // determine new hb bits
            //  if the notDC bits are non-zero copy from xy[0]
            //  if all notDC bits are zero, the hb bits do not matter
            //    but notDC=0 and hb=0 indicates that the
            //      pattern is not valid -> set HB=1 if notDC=0
            unsigned hbX = notDcX ? hbx[0] : (nPosX-1);
            unsigned hbY = notDcY ? hby[0] : (nPosY-1);
            dcBitsLookup2[xy1xy2]=
               (((((notDcY<<nBitsY)|hbY)<<nBitsX)|notDcX)<<nBitsX)|hbX;
         }
      }
      //
      // loop over all SSIDs in this layer (in TSP space)
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         //
         // get DC-level SSID
         //    dcSSIDxy.first  : SSID in DC-space
         //    dcSSIDxy.second : xy coordinate of TSP relative to DC
         //
         std::pair<int,int> dcSSIDxy=getDCssid(layer,(*ssid).first);

         //
         // class to loop over all sectors
         class DCBitExtractor {
         public:
            inline DCBitExtractor
            (int _layer,std::pair<int,int> const &dcSSIDxy,int sector,
             MAP<int,MAP<int,int> > const &dcIndexTable,int dcHbBitsLayer,
             std::vector<uint64_t> const &dcBitsLookup1,
             std::vector<uint64_t> const &dcBitsLookup2,
             PatternBank &bank) : layer(_layer),m_bank(bank) {
               //
               // get encoded SSID information
               ssidIndex=-1;
               MAP<int,MAP<int,int> >::const_iterator dcIndexThisSector=
                  dcIndexTable.find(sector);
               if(dcIndexThisSector!=dcIndexTable.end()) {
                  MAP<int,int>::const_iterator ssidIndexPtr=
                     (*dcIndexThisSector).second.find(dcSSIDxy.first);
                  if(ssidIndexPtr!=(*dcIndexThisSector).second.end()) {
                     //
                     // ssidIndex is a (small) number which identifies
                     //  the (TSP-)SSID, given (layer,sector)
                     //
                     // it is stored together with the ternary bit information
                     // ssidIndex is shifted by dcHbBitsLayer
                     //  to leave space for the ternary bit inflrmation
                     ssidIndex=(*ssidIndexPtr).second << dcHbBitsLayer;
                  }
               }
               dcHbInitial=dcBitsLookup1[dcSSIDxy.second];
               dcHbMask=(1<<dcHbBitsLayer)-1;
               dcBitsLookup=dcBitsLookup2.data()
                  +dcHbInitial*(1<<dcHbBitsLayer);
               
            }
            inline int getSSIDindex(void) const { return ssidIndex; }
            inline void initialize(int firstPattern) {
               pattern=firstPattern;
            }
            inline void process(void) {
               //
               // transform xy coordinate of TSP relative to DC
               //   to DC+HB bits
               // Here, the ternary information is encoded as follows:
               //
               //   MSB ................LSB
               //  ------+-----+------+-----
               //   ~DCY | HBY | ~DCX | HBX
               //
               // ~DCY : logical-not of don't-care bits, Y-coordinate
               //  HBY : y-coordinate of TSP relative to DC
               // ~DCX : logical-not of don't-care bits, Y-coordinate
               //  HBX : y-coordinate of TSP relative to DC
               //
               // there is a special case:
               //  if both ~DC and HB are zero, the pattern is "invalid"
               //  (it has not been filled yet
               //
               // In the original "cached" Bank (Class FTKTSPBank),
               // if DC=1 (corresponds to ~DC=0) then
               //   HB could be zero or one
               // The setting of HB is not relevant if DC is set,
               // but it is stored anyway
               //
               // in the encoding used here, if ~DC=0 then HB=1 to indicate
               // a valid pattern
               //
               // here: calculate pattern address
               //  dcOld : ternary bits of presently stored pattern
               //  ssidWithDC : SSID index
               uint32_t dcOld;
               uint32_t ssidWithDC=ssidIndex;
               if(m_bank.m_pattern8Data[layer].size()) {
                  dcOld=m_bank.m_pattern8Data[layer][pattern];
               } else {
                  dcOld=m_bank.m_pattern16Data[layer][pattern];
               }
               // mask out non-DC bits
               dcOld &= dcHbMask;
               if(!dcOld) {
                  //
                  // first-time this patttern is set
                  //    use initial DC information 
                  ssidWithDC |= dcHbInitial;
               } else {
                  //
                  // store OR of DC information from the new TSP-SSID
                  // and the old DC bits
                  ssidWithDC |= dcBitsLookup[dcOld];
               }
               // 
               // store updated pattern information
               if(m_bank.m_pattern8Data[layer].size()) {
                  m_bank.m_pattern8Data[layer][pattern]=ssidWithDC;
               } else {
                  m_bank.m_pattern16Data[layer][pattern]=ssidWithDC;
               }
            }
            inline void update(int delta) {
               pattern+=delta;
            }
         protected:
            uint32_t ssidIndex;
            uint32_t dcHbInitial;
            uint32_t dcHbMask;
            int layer;
            const uint64_t *dcBitsLookup;
            int pattern;
            PatternBank &m_bank;
         };
         //
         // loop over all sectors with multiple patterns
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            DCBitExtractor extractor
               (layer,dcSSIDxy,(*sector).first,
                dcIndexTable,dcHbBitsLayer,dcBitsLookup1,dcBitsLookup2,m_bank);
            if(extractor.getSSIDindex()<0) {
               Fatal(where)
                  <<"problem with sector/SSID indexing (multi-pattern)\n";
            }
            //
            // pattern data for this (layer,ssid,sector)
            SectorData const &sectordata=(*sector).second;
            patternLoop(extractor,
                        layerData.m_CompressedDeltaBySector
                        [(*sector).first].data()+sectordata.m_offset,
                        sectordata.m_FirstPattern,sectordata.m_NPattern);
         }
      }
#ifdef BIT_PACKED_BANK
      layerBaseBit += nBitLayer;
#endif
   }

   double patternTSPcountTotal=0.;
   for(unsigned i=0;i<patternTSPcount.size();i++)
      patternTSPcountTotal+= patternTSPcount[i];

   Info(where)
      <<"number of distinct (layer,SSIDs)="<<ssidCountTotal
      <<" number of sectors="<<sectorCountTotal
      <<" numbner of patterns="<<patternCountTotal
      <<" (0x"<<std::setbase(16)<<patternCountTotal<<std::setbase(10)<<")\n";
   Info(where)
      <<"number of TSP-SSIDs per pattern="
      <<patternTSPcountTotal/patternCountTotal
      <<" average number of patterns per sector="
      <<patternCountTotal/(double)sectorCountTotal
      <<" min="<<minPattern<<" max="<<maxPattern<<"\n";
   for(unsigned i=0;i<patternTSPcount.size();i++) {
      Info(where)
         <<"plane="<<i<<" number of TSP/pattern="
         <<patternTSPcount[i]/(double)patternCountTotal<<"\n";
   }
   int memoryBuffers=2*sizeof(VECTOR<HitPattern_t>)+
      (m_sectorUsage.size()+m_hitPatterns.size())*sizeof(HitPattern_t)+
      sizeof(VECTOR<uint32_t>)+m_roadCand.size()*sizeof(uint32_t);
   Info(where)
      <<"memory: SSID="<<memorySSIDlookup/1024/1024
      <<" MB, patternID="<<memoryPatternIDlookup/1024/1024
      <<" MB, buffers="<<(memoryBuffers)/1024/1024
      <<" MB, total="<<(memorySSIDlookup+memoryPatternIDlookup+
                        memoryBuffers)/1024/1024
      <<" MB\n";
   Info(where)
      <<"bytes per pattern and layer: SSID="<<memorySSIDlookup/
      (double)(patternCountTotal*getNPlanes())
      <<" patternID="<<memoryPatternIDlookup/(double)
      (patternCountTotal*getNPlanes())
      <<" total="<<(memorySSIDlookup+memoryPatternIDlookup)/
      (double)(patternCountTotal*getNPlanes())<<"\n";
}

/**
   compare()

   compare two banks
   returns zero if the banks are identical
*/
int FTK_CompressedAMBank::compare(FTK_CompressedAMBank const *bank) const {
   int error=0;
   for(unsigned layer=0;(layer<m_bank.m_PatternByLayer.size())&&
          (layer<bank->m_bank.m_PatternByLayer.size());
       layer++) {
      LayerData const &thisLayerData=m_bank.m_PatternByLayer[layer];
      LayerData const &otherLayerData=bank->m_bank.m_PatternByLayer[layer];
      PatternBySectorSSidMap_t::const_ptr ssid1,ssid2;
      for(ssid1=thisLayerData.m_SSidData.beginPtr(),
             ssid2=otherLayerData.m_SSidData.beginPtr();
          (ssid1!=thisLayerData.m_SSidData.endPtr())&&
             (ssid2!=otherLayerData.m_SSidData.endPtr());
          ++ssid1,++ssid2) {
         if((*ssid1).first != (*ssid2).first) {
            if((error & 0x0010)==0) {
               Info("Compare")
                  <<"ssid number mismatch (layer="<<layer<<") "
                  <<(*ssid1).first<<"!="<<(*ssid2).first<<"\n";
            }
            error |= 0x0010;
            continue;
         }
         
         MAP<int,VECTOR<uint32_t> > patternData1,patternData2;

         class PatternExtractor {
         public:
            inline PatternExtractor(VECTOR<uint32_t> &data) : m_data(data) { }
            inline void initialize(uint32_t first) { pattern=first; }
            inline void process(void) {
               m_data.push_back(pattern);
            }
            inline void update(uint32_t delta) {
               pattern+=delta;
            }
         protected:
            VECTOR<uint32_t> &m_data;
            uint32_t pattern;
         };
         for(PatternBySector_t::const_ptr sector1=(*ssid1).second.beginPtr();
              sector1!=(*ssid1).second.endPtr();++sector1) {
            PatternExtractor extractor(patternData1[(*sector1).first]);
            patternLoop(extractor,
                        thisLayerData.m_CompressedDeltaBySector
                        [(*sector1).first].data()+(*sector1).second.m_offset,
                        (*sector1).second.m_FirstPattern,
                        (*sector1).second.m_NPattern);
         }
         for(PatternBySector_t::const_ptr sector2=(*ssid2).second.beginPtr();
              sector2!=(*ssid2).second.endPtr();++sector2) {
            PatternExtractor extractor(patternData2[(*sector2).first]);
            patternLoop(extractor,
                        otherLayerData.m_CompressedDeltaBySector
                        [(*sector2).first].data()+(*sector2).second.m_offset,
                        (*sector2).second.m_FirstPattern,
                        (*sector2).second.m_NPattern);
         }
         MAP<int,VECTOR<uint32_t> >::const_iterator sector1,sector2;
         for(sector1=patternData1.begin(),sector2=patternData2.begin();
             (sector1!=patternData1.end())&&(sector2!=patternData2.end());
             sector1++,sector2++) {
            if((*sector1).first != (*sector2).first) {
               if((error & 0x0100)==0) {
                  Info("Compare")
                     <<"sector number mismatch (layer="<<layer
                     <<" ssid="<<(*ssid1).first<<") "
                     <<(*sector1).first<<"!="<<(*sector2).first<<"\n";
               }
               error |= 0x0100;
               continue;
            }
            VECTOR<uint32_t> const &sectordata1=(*sector1).second;
            VECTOR<uint32_t> const &sectordata2=(*sector2).second;
            int errOld=error;
            if(sectordata1.size()!=sectordata2.size()) error |= 0x2000;
            for(unsigned i=0;i<sectordata1.size();i++) {
               if(sectordata1[i]!=sectordata2[i]) {
                  error |= 0x8000;
                  break;
               }
            }
            if(errOld != error) {
               Info("Compare")
                  <<"bank data mismatch (layer="<<layer
                  <<" ssid="<<(*ssid1).first<<" sector="<<(*sector1).first
                  <<") error=0x"<<std::setbase(16)<<(error^errOld)
                  <<std::setbase(10)<<"\n";
            }
         }
         if(((error & 0x0200)==0) && ((sector1!=patternData1.end())||
                                      (sector2!=patternData2.end()))) {
            Info("Compare")
               <<"different number of sectors (layer="<<layer
               <<" ssid="<<(*ssid1).first
               <<" )"<<thisLayerData.m_SSidData.size()
               <<"!="<<otherLayerData.m_SSidData.size()
               <<"\n";
            error |= 0x0200;
         }
      }
      if(((error & 0x0020)==0)&&((ssid1!=thisLayerData.m_SSidData.endPtr())||
                                 (ssid2!=otherLayerData.m_SSidData.endPtr())))
         {
            Info("Compare")
               <<"different number of ssids (layer="<<layer
               <<") "<<thisLayerData.m_SSidData.size()
               <<"!="<<otherLayerData.m_SSidData.size()
               <<"\n";
            error |= 0x0020;
         }
   }
   if((m_bank.m_PatternByLayer.size()!=bank->m_bank.m_PatternByLayer.size())) {
      Info("Compare")
         <<"different number of layers "<<m_bank.m_PatternByLayer.size()
         <<"!="<<bank->m_bank.m_PatternByLayer.size()<<"\n";
      error |= 0x0001;
   }
   if(error) {
      Warning("Compare")<<"banks do not agree error="<<std::setbase(16)
                        <<error<<std::setbase(10)<<"\n";
   } else {
      Debug("Compare")<<"bank comparison OK\n";
   }
   return error;
}

/**
   readRootFile()

   read bank data from root file
   the root format is the default file format to store the compressed bank

   The data are stored in TTree objects with primitive data types (Int_t)
   in all branches

   see writeRootFile() for further details
*/
int FTK_CompressedAMBank::readRootFile(char const *filename) {
   erase();
   int error=0;
   TDirectory *in=FTKRootFile::Instance()->OpenRootFileReadonly(filename);
   TIter next(in->GetListOfKeys());
   TObject *o;
   int maxLayer=0;
   while((o=next())) {
      TString name=((TObjString *)o)->GetString();
      int layer=TString(name(TRegexp("[0-9]+"))).Atoi();
      if(name.BeginsWith("Layer")) {
         if(layer>maxLayer) maxLayer=layer;
      }
   }
   m_bank.m_PatternByLayer.resize(maxLayer+1);
   setNPlanes(maxLayer+1);
   next=in->GetListOfKeys();
   while((o=next())) {
      TString name=((TObjString *)o)->GetString();
      int layer=TString(name(TRegexp("[0-9]+"))).Atoi();
      if(layer<0) {
         continue;
      }
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      if(name.BeginsWith("Layer")) {
         Info("readRootFile")
            <<"file="<<filename<<" read layer="<<layer<<" data\n";
         TTree *tree;
         in->GetObject(name,tree);
         if(tree) {
            int nSector=tree->GetEntries();
            //std::cout<<" nSector="<<nSector<<"\n";
            Int_t ssidVar,sectorVar,firstPatternVar,nPatternVar;
            Int_t nDataVar;
            tree->SetBranchAddress("ssid",&ssidVar);
            tree->SetBranchAddress("sector",&sectorVar);
            tree->SetBranchAddress("firstPattern",&firstPatternVar);
            tree->SetBranchAddress("nPattern",&nPatternVar);
               tree->SetBranchAddress("nData",&nDataVar);
            int dataSize=0;
            tree->SetBranchStatus("sector");
            tree->SetBranchStatus("nData");
            MAP<int,int> sectorSize;
            for(int i=0;i<nSector;i++) {
               //std::cout<<" "<<i;
               tree->GetEntry(i);
               //std::cout<<" "<<nDataVar<<" "<<sectorVar<<"\n";
               if(nDataVar>dataSize) dataSize=nDataVar;
               sectorSize[sectorVar]+=nDataVar;
            }
            int maxSector=(*sectorSize.rbegin()).first+1;
            //std::cout<<" sectorVar="<<dataSize
            //         <<" maxSector="<<maxSector<<"\n";
            VECTOR<int> sectorOffset(maxSector);
            std::vector<Int_t> data(dataSize);
            layerData.m_CompressedDeltaBySector.resize(maxSector);
            for(MAP<int,int>::const_iterator isector=sectorSize.begin();
                isector!=sectorSize.end();isector++) {
               layerData.m_CompressedDeltaBySector[(*isector).first].resize
                  ((*isector).second);
            }
            tree->SetBranchAddress("data",data.data());
            tree->SetBranchStatus("data");
            tree->SetBranchStatus("ssid");
            tree->SetBranchStatus("firstPattern");
            tree->SetBranchStatus("nPattern");
            Info("readRootFile")<<"nSector="<<nSector
                                  <<" buffer size="<<data.size()<<"\n";
            for(int i=0;i<nSector;i++) {
               tree->GetEntry(i);
               SectorData &sectordata=layerData.m_SSidData[ssidVar][sectorVar];
               sectordata.m_FirstPattern=firstPatternVar;
               sectordata.m_NPattern=nPatternVar;
               sectordata.m_offset=sectorOffset[sectorVar];
               sectordata.m_length=nDataVar;
               uint8_t __restrict *dest=
                  layerData.m_CompressedDeltaBySector[sectorVar].data()+
                  sectordata.m_offset;
               for(int i=0;i<nDataVar;i++) {
                  (*dest++)=data[i];
               }
               sectorOffset[sectorVar]+=nDataVar;
            }
         } else {
            Error("readRootFile")<<"TObject "<<name<<" is not a TTree\n";
            error++;
         }
      }
   }
   readBankPostprocessing("readRootFile");
   return error;
}

/**
   writeRootFile()

   write bank data to root file
   the root format is the default file format to store the compressed bank

   The data are stored in TTree objects with primitive data types (Int_t)
   in all branches

   There is one TTree for each plane, named "Layer0", "Layer1", ...

   each enry in the TTree coresponds to one
     (ssid,sector) combination
   the branches are:
           ssid : TSP-level SSID
         sector : sector
   firstPattern : first pattern ID for this (ssid,sector) combination
       nPattern : number of patterns for this (ssid,sector) combination
          nData : size of compressed data
    data[nData] : compressed pattern data
*/
int FTK_CompressedAMBank::writeRootFile(char const *filename) const {
   int error=0;
   TDirectory *out=FTKRootFile::Instance()->CreateRootFile(filename);
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData const &layerData=m_bank.m_PatternByLayer[layer];
      TString treeName(TString::Format("Layer%d",layer));
      TTree *ssidTree=new TTree(treeName,treeName);
      int maxDataSize=0;
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            int nword=(*sector).second.m_length;
            if(nword>maxDataSize) maxDataSize=nword;
         }
      }
      Info("WriteRootFile")
         <<"layer="<<layer<<" file="<<filename
         <<" maxDeltaSize="<<maxDataSize<<"\n";
      Int_t ssidVar,sectorVar,firstPatternVar,nPatternVar,nDataVar;
      std::vector<Int_t> data(maxDataSize);
      ssidTree->Branch("ssid",&ssidVar,"ssid/I");
      ssidTree->Branch("sector",&sectorVar,"sector/I");
      ssidTree->Branch("firstPattern",&firstPatternVar,"firstPattern/I");
      ssidTree->Branch("nPattern",&nPatternVar,"numPattern/I");
      if(maxDataSize) {
         ssidTree->Branch("nData",&nDataVar,"nData/I");
         ssidTree->Branch("data",data.data(),"data[nData]/I");
      }
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         ssidVar=(*ssid).first;
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            sectorVar=(*sector).first;
            firstPatternVar=(*sector).second.m_FirstPattern;
            nPatternVar=(*sector).second.m_NPattern;
            nDataVar=(*sector).second.m_length;
            uint8_t __restrict const *compressedDeltaData=layerData.
               m_CompressedDeltaBySector[sectorVar].data()+
               (*sector).second.m_offset;
            for(int i=0;i<nDataVar;i++)
               data[i]=compressedDeltaData[i];
            ssidTree->Fill();
         }
      }
      ssidTree->Write("", TObject::kOverwrite);      
      Info("WriteRootFile")
         <<"layer="<<layer<<" file="<<filename
         <<" nSector="<<ssidTree->GetEntriesFast()<<" written\n";
   }
   delete out;
   return error;
}

/**
   readROOTBank()

   interface provided by base class
*/
int FTK_CompressedAMBank::readROOTBank(const char *name, int /*maxpatts*/) {
   return readRootFile(name);
}

/**
   init()

   initialize the pattern finding
*/
void FTK_CompressedAMBank::init() {
   // set up hard-coded wildcards
   //   these accounts for a cut make on certain eta modules
   //   in the outermost plane
   //    (see FTK_AMbank::am_output)
   FTK_AMsimulation_base::init();
   PatternBySectorSSidMap_t const &outerLayer=
      m_bank.m_PatternByLayer[getNPlanes()-1].m_SSidData;
   MAP<int,int> sectorFlag;
   for(PatternBySectorSSidMap_t::const_ptr iSSid=outerLayer.beginPtr();
       iSSid!=outerLayer.endPtr();++iSSid) {
      int ieta1 = abs((*iSSid).first) %100;
      int mask;
      if((ieta1==0 ) || (ieta1==12) || (ieta1==21) ||(ieta1==31)) {
         mask=2;
      } else {
         mask=1;
      }
      for(PatternBySector_t::const_ptr sector=(*iSSid).second.beginPtr();
          sector!=(*iSSid).second.endPtr();++sector) {
         sectorFlag[(*sector).first] |= mask;
      }
   }
   int n2=0,n3=0;
   for(MAP<int,int>::const_iterator sector=sectorFlag.begin();
       sector!=sectorFlag.end();sector++) {
      if((*sector).second==2) {
         // in this sector, all SSid fulfil the specific eta condition
         m_SectorWildcard[(*sector).first]= 1<<(getNPlanes()-1);
         n2++;
      } else if((*sector).second==3) {
         n3++;
      }
   }
   if(n2) {
      Info("init")<<"special eta wildcard applied: n2="<<n2<<"\n";
   }
   if(n3) {
      Fatal("init")<<"special eta wildcard can not be applied: n3="<<n3<<"\n";
   }
   //
   // FIXME: read more wildcards from files
}

/**
   clear()

   clear data structures representing the
   data-organizer and AM output
 */
void FTK_CompressedAMBank::clear(void) {
   naoClear();
   clearNRoads();
   // data organizer
   m_FiredSSmap.resize(0);
   m_FiredSSmap.resize(getNPlanes());
   m_UsedSSmap.resize(0);
   m_UsedSSmap.resize(getNPlanes());
   // AM output
   m_roads.clear();

   m_MAX_MISSING_PLANES = FTKSetup::getFTKSetup().getMaxMissingPlanes();
   m_MAX_MISSING_SCT_PAIRS = FTKSetup::getFTKSetup().getMaxMissingSctPairs();
   m_nhWCmin=getNPlanes()-m_MAX_MISSING_PLANES-m_MAX_MISSING_SCT_PAIRS;
}

/**
   sort_hits()

   read vector<FKTHit>
   produce a list of (layer,TSPssid) stored as std::vector<std::list<int> >

   in addition, create a list of all superstrips (DC-space with subSS)
   store in m_FiredSSmap
*/
void FTK_CompressedAMBank::sort_hits
(const std::vector<FTKHit> &hits) {
   // list of all hits, ordered by layer and TSP-SSID
   typedef std::list<FTKHit const *> SSinfo_t;
   typedef MAP<int,SSinfo_t> SSinfoBySSID_t;
   typedef std::vector<SSinfoBySSID_t> SSinfoByLayerSSID_t;
   SSinfoByLayerSSID_t hitList(getNPlanes());

   // layers where all hits shall be skipped
   int layerMask=FTKSetup::getFTKSetup().getMaskLayers();

   std::vector<int> nHit(getNPlanes());
   for(std::vector<FTKHit>::const_iterator ihit = hits.begin();
       ihit!=hits.end();++ihit) {
      // pointer to the hit
      const FTKHit *hit = & (*ihit);
      // plane
      int iplane=hit->getPlane();
      // verify the hit according the mask that masks particular logical laeyrs
      if (layerMask&(1<<iplane)) continue; // skip this hit because the layer
      if (!getSSMap()->getRegionMap()->isHitInRegion(*hit,getBankID()))
         continue; // skip if the is not in this regione
      ++nHit[iplane];
      // pointer to the copy
      // get SSID
      int tsp_ssid;
      if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
         // Using a dummy pattern bank representing just the number of sectors, the IDs are the module IDs, for historical reason called sector.
         tsp_ssid = hit->getSector();
         //dc_ssid = hit->getSector();
      } else if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         //SS calculated assuming a global SS id
         tsp_ssid =getSSMapTSP()->getSSGlobal(*hit);
         //dc_ssid = getSSMap()->getSSGlobal(*hit);
      } else if (FTKSetup::getFTKSetup().getHWModeSS()==2) {
         // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
         tsp_ssid = getSSMapTSP()->getSSTower(*hit,getBankID());
         //dc_ssid = getSSMap()->getSSTower(*hit,getBankID());
      } else {
         Fatal("passHits")
            <<"hardware mode="<<FTKSetup::getFTKSetup().getHWModeSS()
            <<" not supported\n";
      }
      // check whether this TSP-SSID is new or not
      SSinfoBySSID_t::iterator iSSID=hitList[iplane].find(tsp_ssid);
      if(iSSID==hitList[iplane].end()) {
         // this SSID has not been entered yet
         iSSID=hitList[iplane].insert
            (std::make_pair(tsp_ssid,std::list<FTKHit const *>())).first;
         m_tspSSID[iplane].push_back(tsp_ssid);
      }
      // add hit
      (*iSSID).second.push_back(hit);
   }

   // prepare list of all superstrips
   // indexed by DC-level SSID
   // but with substrips according to TSP-level SSID
   for (int ipl=0;ipl<getNPlanes();++ipl) {
      SSinfoBySSID_t &hitsInLayer=hitList[ipl];
      MAP<int,std::vector<FTKSS> > firedTSPbyDC;
      for(SSinfoBySSID_t::const_iterator iSSID=hitsInLayer.begin();
          iSSID!=hitsInLayer.end();iSSID++) {
         int tspSSID=(*iSSID).first;
         //int dcSSID=(*iSSID).second.fSSIDdc;
         std::pair<int,int> const &dcSSID=getDCssid(ipl,tspSSID);
         /* std::cout<<"layer="<<ipl
             <<" tspSSID="<<tspSSID
             <<" dcSSID="<<dcSSID.first<<" xy="<<dcSSID.second
             <<"\n"; */
         std::vector<FTKSS> &ssBYxy=firedTSPbyDC[dcSSID.first];
         ssBYxy.resize(m_TSPmap->getNPositions(ipl));
         FTKSS &tspLevelSS=ssBYxy[dcSSID.second];
         for(std::list<FTKHit const *>::const_iterator ihit=
                (*iSSID).second.begin();ihit!=(*iSSID).second.end();ihit++) {
            tspLevelSS.addHit(**ihit);
         }
      }
      for(MAP<int,std::vector<FTKSS> >::const_iterator
             i=firedTSPbyDC.begin();i!=firedTSPbyDC.end();i++) {
         m_FiredSSmap[ipl][(*i).first].addSubStrips((*i).second);
      }
      naoSSPushBack(m_FiredSSmap[ipl].size());
   }

   // statistics
   for (int iplane=0;iplane!=getNPlanes();++iplane) {
      naoClusPushBack(nHit[iplane]);
   }
}

void FTK_CompressedAMBank::data_organizer(void) {
   data_organizer_r(m_tspSSID);
}

void FTK_CompressedAMBank::data_organizer_r
(std::vector<std::list<int> > const &tspSSIDfired) {
   int maxSector=(*m_SectorFirstLastPattern.rbegin()).first+1;
   m_sectorUsage.resize(maxSector);
   memset(m_sectorUsage.data(),0,sizeof(HitPattern_t)*m_sectorUsage.size());
   //
   // loop over all input (layer,ssid)
   // locate the (layer,ssid) in the bank
   // store pointer to the pattern data in the sector-ordered structure
   //    patternDataBySectorLayerSSID[sector][layer][ssid]
   for(int iplane=0;iplane<getNPlanes();iplane++) {
      LayerData const &bankDataThisLayer=m_bank.m_PatternByLayer[iplane];
      HitPattern_t mask=1<<iplane;
      for(std::list<int>::const_iterator iSSID=tspSSIDfired[iplane].begin();
          iSSID!=tspSSIDfired[iplane].end();iSSID++) {
         int tsp_ssid=(*iSSID);
         // locate the SSID in the pattern bank
         PatternBySectorSSidMap_t::const_iterator ssidInBank=
            bankDataThisLayer.m_SSidData.find(tsp_ssid);
         if(ssidInBank!=bankDataThisLayer.m_SSidData.end()) {
            // tsp_ssid is present in the pattern bank
            //  loop over all its sectors and set mask
            for(PatternBySector_t::const_ptr sector=
                   (*ssidInBank).second.beginPtr();
                sector!=(*ssidInBank).second.endPtr();++sector) {
               m_sectorUsage[(*sector).first]|=mask;
            }
         }
      }
   }
   //
   // loop over all sectors
   // discard sectors with insufficient numnber of hits candidates
   for(MAP<int,std::pair<int,int> >::const_iterator isector=
          m_SectorFirstLastPattern.begin();
       isector!=m_SectorFirstLastPattern.end();isector++) {
      int sector=(*isector).first;
      // determine wildcard for this sector
      HitPattern_t sectorWildcard=0;
      MAP<int,HitPattern_t>::const_iterator
         iWC=m_SectorWildcard.find(sector);
      if(iWC!=m_SectorWildcard.end()) sectorWildcard=(*iWC).second;
      int hitPattern=m_sectorUsage[sector] | sectorWildcard;
      int nhit=m_nHit16[hitPattern];
      if(nhit>=m_nhWCmin) {
         //
         // reset hit pattern for this sector
         int firstPattern=(*isector).second.first;
         int lastPattern=(*isector).second.second;
         if(firstPattern!=lastPattern) {
            memset(m_hitPatterns.data()+firstPattern,0,
                   lastPattern+1-firstPattern);
         }
      } else {
         // skip this sector
         m_sectorUsage[sector]=0;
      }
   }
}

/**
   am_in()
   am_in_r()

   put hits into assiciated memory
 */

void FTK_CompressedAMBank::am_in(void) {
   am_in_r(m_tspSSID);
}

void FTK_CompressedAMBank::am_in_r
(std::vector<std::list<int> > const &tspSSIDfired) {
   // first part of the AM emulation
   //   corresponds to am_in()
   //   the hits are fed into the assiciated memory
   //   and road candidates are defined

   class MaskUpdaterFast {
      // fast update of hitpattern only
   public:
      inline MaskUpdaterFast(HitPattern_t *hits,HitPattern_t msk)
         : patternPtr(hits), mask(msk) {
      }
      inline void process(void) {
         *patternPtr |= mask;
      }
      inline void update(int delta) {
         patternPtr += delta;
      }
      inline void initialize(int firstPattern) {
         update(firstPattern);
      }
   protected:
      HitPattern_t __restrict *patternPtr;
      HitPattern_t mask;
   };
   class MaskUpdaterSlow {
      // slow update:
      // update hit pattern and
      // determine road candidates
      // a road candidate is store if:
      //   (1) the pattern has hits in other layers
      //   (2) the pattern has no hit in the present layer
      //   (3) the updated mask has exactly the minimum number of hits
      // condition (1) is to skip empty patterns (not needed?)
      // condition (2) is to ensure a valid pattern is stored only once
      // condition (3) is there to ensure the pattern is there and stored only
      //   as the threshold is passed
   public:
      inline MaskUpdaterSlow(HitPattern_t *hits,HitPattern_t msk,
#ifndef CANDIDATES_WITHOUT_SECTORID
                             uint32_t sectorID,
#endif
                             uint8_t nhmin,uint8_t const *nh16,
                             VECTOR<
#ifdef CANDIDATES_WITHOUT_SECTORID
                             uint32_t
#else
                             std::pair<uint32_t,uint32_t>
#endif
                             > &rc,unsigned &nrc)
         : patternPtr(hits), mask(msk),notMask(~msk),roadCandidates(rc),
           nRC(nrc),nhMin(nhmin),nhit16(nh16)
#ifndef CANDIDATES_WITHOUT_SECTORID
         ,sector(sectorID)
#endif
      {
         base=patternPtr;
      }
      inline void process(void) {
         HitPattern_t h0=*patternPtr;
         HitPattern_t h1=h0|mask;
         if((h0 & notMask)&&(!(h0 & mask))&&(nhit16[h1]==nhMin)) {
            if(nRC<roadCandidates.size()) {
               roadCandidates[nRC++] =
#ifdef CANDIDATES_WITHOUT_SECTORID
                  patternPtr-base
#else
                  std::make_pair(patternPtr-base,sector)
#endif
                  ;
            }
         }
         *patternPtr =h1;
      }
      inline void update(int delta) {
         patternPtr += delta;
      }
      inline void initialize(int firstPattern) {
         update(firstPattern);
      }
   protected:
      HitPattern_t __restrict *patternPtr,*base;
      HitPattern_t mask;
      HitPattern_t notMask;
      VECTOR<
#ifdef CANDIDATES_WITHOUT_SECTORID
                             uint32_t
#else
                             std::pair<uint32_t,uint32_t>
#endif
         > &roadCandidates;
      unsigned &nRC;
      uint8_t nhMin;
      uint8_t const *nhit16;
#ifndef CANDIDATES_WITHOUT_SECTORID
      uint32_t sector;
#endif
   };
   m_nRoadCand=0;
   for(int iplane=0;iplane<getNPlanes();++iplane) {
      LayerData const &bankDataThisLayer=m_bank.m_PatternByLayer[iplane];
      HitPattern_t mask= 1<<iplane;
      for(std::list<int>::const_iterator iSSID=tspSSIDfired[iplane].begin();
          iSSID!=tspSSIDfired[iplane].end();iSSID++) {
         int tsp_ssid=(*iSSID);
         // locate the SSID in the pattern bank
         PatternBySectorSSidMap_t::const_iterator ssidInBank=
            bankDataThisLayer.m_SSidData.find(tsp_ssid);
         if(ssidInBank!=bankDataThisLayer.m_SSidData.end()) {
            // tsp_ssid is present in the pattern bank
            //  loop over all its sectors and set mask
            for(PatternBySector_t::const_ptr isector=
                   (*ssidInBank).second.beginPtr();
                isector!=(*ssidInBank).second.endPtr();++isector) {
               int sector=(*isector).first;
               if(!m_sectorUsage[sector]) continue;
               SectorData const &sectorData((*isector).second);
               if(iplane<m_nhWCmin-1) {
                  // first planes: no chance to form a road candidate
                  // simply update bit mask
                  MaskUpdaterFast updater(m_hitPatterns.data(),mask);
                  patternLoop(updater,
                              m_bank.m_PatternByLayer[iplane].
                              m_CompressedDeltaBySector[sector].data()+
                              sectorData.m_offset,
                              sectorData.m_FirstPattern,sectorData.m_NPattern);
               } else {
                  // last planes, update bit mask and remember road candidates
                  MaskUpdaterSlow updater
                     (m_hitPatterns.data(),mask,
#ifndef CANDIDATES_WITHOUT_SECTORID
                      sector,
#endif
                      m_nhWCmin,m_nHit16.data(),
                      m_roadCand,m_nRoadCand);
                  patternLoop(updater,
                              m_bank.m_PatternByLayer[iplane].
                              m_CompressedDeltaBySector[sector].data()+
                              sectorData.m_offset,
                              sectorData.m_FirstPattern,sectorData.m_NPattern);
               }
            }
         }
      }
   }
}

/**
   am_output()

   Output: populate m_roads
 */

void FTK_CompressedAMBank::am_output() {
   for(unsigned iCand=0;iCand<m_nRoadCand;iCand++) {
#ifdef CANDIDATES_WITHOUT_SECTORID
      int patternID=m_roadCand[iCand];
      MAP<int,int>::const_iterator sectorPtr=
         m_patternToSector.lower_bound(patternID);
      int sector=(*sectorPtr).second;
#else
      int patternID=m_roadCand[iCand].first;
      int sector=m_roadCand[iCand].second;
#endif
      HitPattern_t hitmask=m_hitPatterns[patternID];
      HitPattern_t sectorWildcard=0;
      MAP<int,HitPattern_t>::const_iterator
         iWC=m_SectorWildcard.find(sector);
      if(iWC!=m_SectorWildcard.end()) sectorWildcard=(*iWC).second;
      HitPattern_t hitmaskWithWC=hitmask|sectorWildcard;
      uint8_t nhWC=m_nHit16[hitmaskWithWC];
      //
      // determine FTKRoads
      if(nhWC>=m_nhWCmin) {
         //
         // extra checks
         // required hits in first or last plane 
         uint32_t lastPlaneMask=1<<(getNPlanes()-1);
         bool first_check = !getRequireFirst() || (hitmaskWithWC & 1);
         bool missinglast = ! (hitmaskWithWC & lastPlaneMask);
         bool last_check = !getRequireLast() || !missinglast;
         if( first_check && last_check &&
             (nhWC>= getNPlanes()-m_MAX_MISSING_PLANES)) {
            //
            // statistics
            countNRoads();
            //
            // number of hits
            uint8_t nh=m_nHit16[hitmask];
            //
            // construct FTKRoad object
            //   see: FTK_AMBank::am_output
            m_roads.push_front
               (FTKRoad
                (getNRoads()-1,
                 getBankID()+100*getSubID(),
                 // ID is set to patternID+1, so ID=0 is not used
                 patternID+1,getNPlanes(),
                 nh,     // 
                 hitmask // FTK_TSPBANK::attach_SS
                 ));
            FTKRoad &road=* m_roads.begin();
            //
            // add missing information
            //   sector ID 
            road.setSectorID(sector);  //attachSS[ 655 -> 662]
            //   DBID is set identical to ID
            road.setPatternDBID(patternID+1);
            //
            // unpack DC bits and DC-SSID fron pattern bank
            int dcMask=0,hbmask=0;
            //
            // loop over all planes
            for (int ipl=getNPlanes()-1;ipl>=0;--ipl) {
               int nDCbits=m_TSPmap->getNBits(ipl);
               int nDCHBbits=2*nDCbits;
               //
               // dcSSID_with_DCbits contains
               //   the encoded SSID-index and the encoded DC bits
               // nbit=nDCHBbits
               //              <-- nbit -->
               //  MSB..................LSB
               // ------------+-------------
               //   SSIDindex |   DCbits
               unsigned dcSSID_with_DCbits;
               if(m_bank.m_pattern8Data[ipl].size()) {
                  dcSSID_with_DCbits=m_bank.m_pattern8Data[ipl][patternID];
               } else {
                  dcSSID_with_DCbits=m_bank.m_pattern16Data[ipl][patternID];
               }
               //
               // decode SSID using index table:
               //   m_bank.m_PatternByLayer[ipl].m_dcSSIDtable[sector]
               int dcSSIDindex=dcSSID_with_DCbits >> nDCHBbits;
               int dcSSID=m_bank.m_PatternByLayer[ipl].m_dcSSIDbySectorIndex
                  [sector][dcSSIDindex];
               //
               // store SSID
               road.setSSID(ipl,dcSSID);  //attachSS[ 655 -> 670]
               int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
               //
               // store subss mask
               road.setSubSSMask(ipl,m_subSSmask[ipl][dcBits]);
               //
               // set HLID (?)  - see FTKTSPBank::attachSS()
               road.setHLID(ipl,m_TSPmap->getNBits(ipl));
               //
               // decode DC and HB bits
               dcMask =(dcMask<<nDCbits) | m_dcMaskLookup[ipl][dcBits];
               hbmask =(hbmask<<nDCbits) | m_hbMaskLookup[ipl][dcBits];
            }
            //
            // store ternary bits
            road.setDCBitmask(dcMask);
            road.setHLBitmask(hbmask);
         }
      }
   }
   naoSetNroadsAM(getNRoads());
}

/*******************************************
 * Function: informationMatch()
 * Purpose: applies road warrior between two patterns (indexed by patt1 and patt2)
 * Arguments: two road ID's that index patterns
 * Returns: 0, if two roads are considered independent
 *                 so that both will be kept
 *           1, if one road is the ghost of another
 * Comments: 
 *  original code was using the index in the pattern bank
 *  here the SSID information is taken from the FTKRoad objects
*******************************************/
int FTK_CompressedAMBank::informationMatch(FTKRoad *r1,FTKRoad *r2) {
  const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const FTKPlaneMap *pmap = getSSMap()->getPlaneMap();

  //int nhits1 = hitbit[ibank][NPLANES][patt1];
  //int nhits2 = hitbit[ibank][NPLANES][patt2];
  int nsame = getNPlanes(); // count the number of shared SS

  if(r1->getSectorID()!=r2->getSectorID()) {
     if (ftkset.getRoadWarrior()<=1)
        return 0; // If ROADWARRIOR=0 this means always different patterns
  }

  for(int i = 0; i < getNPlanes(); ++i) { // loop to count the shared SS
     if(!pmap->isSCT(i)) { // Pixel plane
      /* pattern[][][] has the strips used by each pattern, if there is a
	 difference the two roads could be different.
	 The different is meaningful only if the two roads are empty in both
	 patterns or if the patt2 is empty while patt1 is a complete pattern*/
        if ( (r1->getSSID(i)!=r2->getSSID(i)) &&
             ( r1->hasHitOnLayer(i) && r2->hasHitOnLayer(i) )) {
           /* The hitbit requirement is sufficient because because if patt1
              has 1 empty SS patt2 has at least a different empty SS,
              for this reason an empty SS in patt2 can be suspected to be
              a ghost road while an empty SS in patt1 doesn-t */

           nsame -= 1;
           return 0;
        }
     } else { // SCT plane
        int ipair = pmap->getPlanePair(i);
        bool condition1 = (r1->getSSID(i)!=r2->getSSID(i)) &&
           r2->hasHitOnLayer(i);
        // if SCT plane pairing is not used both conditions are the same
        bool condition2 (condition1);
        if (i!=ipair) { // pairing is used
           condition2 = (r1->getSSID(ipair)!=r2->getSSID(ipair)) &&
              r2->hasHitOnLayer(i);
           if (ipair==i+1) {
              // the 1st SCT plane can be paired to the next,
              // if it is the case the next step can be skipped
              i += 1; // skip one step
           }
        }
        if ( condition1 && condition2 ) {
           // here if the SS of the two patterns in this plane differs
           nsame -= 1;
           return 0;
        }
     }
  } // end loop to count the shared SS
  if (nsame==getNPlanes())
    return 1;
  else 
    return 0;
}

/**
   getRoads()

   return list of roads found in this event
 */
const std::list<FTKRoad>& FTK_CompressedAMBank::getRoads() {
   // return list of roads
   // construct list of superstrips really used for the roads found
   for(std::list<FTKRoad>::iterator iroad = m_roads.begin();
       iroad!=m_roads.end();++iroad) {
      // prepare list of all superstrips attached to any of the roads
      for (int ipl=0;(ipl<getNPlanes())&&(!getStoreAllSS());++ipl) {
         int dc_ssid=(*iroad).getSSID(ipl);
         std::unordered_map<int,FTKSS>::iterator item0 =
            m_FiredSSmap[ipl].find(dc_ssid);
         if (item0==m_FiredSSmap[ipl].end()) 
            continue; // no hits
         // map for this plane
         std::unordered_map<int,FTKSS> &imap = m_UsedSSmap[ipl];
         // find this ss is in the bank
         std::unordered_map<int,FTKSS>::iterator item = imap.find(dc_ssid);
         if (item==imap.end()) { // not found
            imap[dc_ssid] = (*item0).second;
         } else {
            continue;
         }
      } // end loop over the layers
   }
#ifdef PRINT_ROADS_SECTOR
  static int print=1;
  if(print) {
     Info("getRoads")<<"number of roads="<<m_roads.size()<<"\n";
     printRoads(m_roads,PRINT_ROADS_SECTOR);
     print--;
  }
#endif
  return m_roads;
}

/**
   getStrips()

   return all superstrips or the superstrips associated with a road
   for a given plane
 */
const std::unordered_map<int,FTKSS>& 
FTK_CompressedAMBank::getStrips(int plane) {
#ifdef PRINT_SS
  static int printSS=PRINT_SS;
  if(printSS) {
     Info("getStrips")<<"plane="<<plane<<" fired="<< m_FiredSSmap[plane].size()
                      <<" used="<<m_UsedSSmap[plane].size()<<"\n";
#ifdef DEBUG_SS
     MAP<int,FTKSS const *> sorted;
     for(std::unordered_map<int,FTKSS>::const_iterator i=m_UsedSSmap[plane].begin();
         i!=m_UsedSSmap[plane].end();i++) {
        sorted[(*i).first]=& (*i).second;
     }
     for(MAP<int,FTKSS const *>::const_iterator i=sorted.begin();i!=sorted.end();i++) {
        std::cout<<" "<<(*i).first;
     }
     std::cout<<"\n";
#endif
     printSS--;
  }
#endif
  if(getStoreAllSS()) {
     return m_FiredSSmap[plane];
  }
  return m_UsedSSmap[plane];
}
