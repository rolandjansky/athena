/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_CompressedAMBank.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/tsp/TSPMap.h"
#include "TrigFTKSim/ftk_dcap.h"

#include <sys/time.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
//#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TString.h>
#include <TString.h>
#include <TRegexp.h>


#ifdef SEARCH_MEMORY_LEAK
#include <sys/time.h>
#include <sys/resource.h>

static void printVmemUsage(char const *text) {
   struct rusage usage;
   getrusage(RUSAGE_SELF,&usage);
   std::cout<<"VMEM usage "<<text
            <<" "<<usage.ru_maxrss/1024./1024.<<"G"
      <<"\n";
}
#endif

#define CONSISTENCY_CHECK

/*
  class FTK_CompressedAMBank
  Author: S.Schmitt
  Date 01/2015

  Implements a memory and CPU efficient emulation of the FTK pattern finding

 */

#define QSORT_FOR_DC_IMPORT

#define PRINT_ROADS_SECTORID -1
#define PRINT_DETAILS_NEVENT 0
//#define PRINT_DETAILS_NEVENT 100
#define PRINT_SS 0
//#define PRINT_SS PRINT_DETAILS_NEVENT
#define PRINT_SS_NOROAD 0
//#define PRINT_SS_NOROAD PRINT_DETAILS_NEVENT
// #define PRINT_MULTIPLICITY
// #define WRITE_DEADMODULE_TEMPLATE
// #define TESTDCSSID 19
//#define DEBUG_EVENT 88

#define HW2_USE_TSPMAP

int const FTK_CompressedAMBank::MAX_NROAD=300000;

int const FTK_CompressedAMBank::s_WILDCARDid=-1;
int const FTK_CompressedAMBank::s_INVALIDid=-2;

#ifdef DEBUG_EVENT
static int g_event=0;
#endif

//using namespace std;

template<class A> inline void FTK_CompressedAMBank::patternLoop
(A &a,uint8_t const * __restrict ptr,uint32_t firstPattern,int nPattern)
   const {
   // first pattern ID is known
   a.initialize(firstPattern);
   a.process();
   // loop over all compressed patterns
   for(--nPattern;nPattern>0;nPattern--) {
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
   // if(nPattern<0) { std::cout<<"bug\n"; exit(0); }
}

FTK_CompressedAMBank::~FTK_CompressedAMBank() {
   if (m_TSPmap) delete m_TSPmap;
}

FTK_CompressedAMBank::FTK_CompressedAMBank
(int bankID,int subID,FTKSSMap *ssMap,FTKSSMap *ssMapTSP,
 int hwmodeid_TSP,int hwmodeid_DC,char const *name)
   : FTKLogging(name),FTK_AMsimulation_base(bankID,subID),
     m_nRoadCand(0),
     m_nhWCmin(0),
     m_MAX_MISSING_PLANES(0),
     m_SSmapTSP(ssMapTSP),
     m_hwmodeIDtsp(hwmodeid_TSP),
     m_hwmodeIDdc(hwmodeid_DC),
     m_nDCmax(-1),m_wildcardPenalty(0) {
   setSSMap(ssMap);
   if(getSSMap()&& getSSMapTSP()) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   } else {
      m_TSPmap=0;
   }
   Info("CompressedAMBank")
      <<"hwmodeIDtsp="<<m_hwmodeIDtsp
      <<" hwmodeIDdc="<<m_hwmodeIDdc<<"\n";
}

void FTK_CompressedAMBank::setNDCmax(int ndc) {
   m_nDCmax=ndc;
   Info("setNDCmax")<<"maximum number of DC bits set to "<<getNDCmax()<<"\n";   
}

void FTK_CompressedAMBank::setNDCmaxPlane(size_t plane,int ndc) {
   if(plane>=m_nDCmaxPlane.size()) m_nDCmaxPlane.resize(plane+1);
   m_nDCmaxPlane[plane]=ndc;
   Info("setNDCmax")<<"maximum number of DC bits for plane "<<plane
                    <<" set to "<<getNDCmaxPlane(plane)<<"\n";
}

void FTK_CompressedAMBank::setWildcardPenalty(int penalty) {
   m_wildcardPenalty=penalty;
   Info("setWildcardPenalty")
      <<"wildcard Penalty (extra DC bit count) set to: "
      <<getWildcardPenalty()<<"\n";   
}

void FTK_CompressedAMBank::setWildcardPenaltyPlane(size_t plane,int penalty) {
   if(plane>=m_wildcardPenaltyPlane.size())
      m_wildcardPenaltyPlane.resize(plane+1);
   m_wildcardPenaltyPlane[plane]=penalty;
   Info("setWildcardPenaltyPlane")
      <<"wildcard penalty for plane "<<plane
      <<" set to: "<<getWildcardPenaltyPlane(plane)<<"\n";
}

void FTK_CompressedAMBank::invalidateSSIDtables(void) {
   m_TSPtoDC.resize(0);
   m_DCtoTSP.resize(0);
   if(getNPlanes()) {
      m_TSPtoDC.resize(getNPlanes());
      m_DCtoTSP.resize(getNPlanes());   
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

   // lookup-table to determine number of bits in a word
   m_nHit16.resize(0x10000);
   for(int hitmask=0;hitmask<0x10000;hitmask++) {
      int n=0;
      for(int k=0;k<getNPlanes();k++) {
         if(hitmask & (1<<k)) n++;
      }
      m_nHit16[hitmask]=n;
   }

   //
   // lookup-tables to convert TSP <-> DC
   invalidateSSIDtables();
   m_badModules.resize(getNPlanes());

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
   // set up lookup-tables to merge pairs of encoded (DC,HB) bits
   m_dcBitsLookup1.resize(getNPlanes());
   m_dcBitsLookup2.resize(getNPlanes());
   m_ndcBitsLookup.resize(getNPlanes());
   for(int layer=0;layer<getNPlanes();layer++) {
      int nBitsX=m_TSPmap->getNBits(layer,0);
      int nBitsY=0;
      if(m_TSPmap->getDim(layer)==2) {
         nBitsY=m_TSPmap->getNBits(layer,1);
      }
      int nPosX=1<<nBitsX;
      int nPosY=1<<nBitsY;
      int hbBitsLayer=nBitsX+nBitsY;
      int dcHbBitsLayer=2*hbBitsLayer;
      //
      // fill:   dcBitsLookup1[layer][xy]
      //
      //   given the xy coordinates in the TSP map relative to the DC map
      //   this returns the encoded HB bits for this layer
      //   initial setting of DC bits
      VECTOR<uint64_t> &dcBitsLookup1=m_dcBitsLookup1[layer];
      dcBitsLookup1.resize(1<<hbBitsLayer);
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
      // fill:   m_ndcBitsLookup[layer][xy]
      //
      //   given the encoded DCHB bits for this layer
      //   this returns the number of DC bits
      VECTOR<uint8_t> &ndcBitsLookup=m_ndcBitsLookup[layer];
      ndcBitsLookup.resize(1<<dcHbBitsLayer);
      for(xy1=0;xy1<ndcBitsLookup.size();xy1++) {
         unsigned dcx,dcy;
         dcx=(xy1>>nBitsX) & (nPosX-1);
         dcy=(xy1>>(2*nBitsX+nBitsY)) & (nPosY-1);
         ndcBitsLookup[xy1]=nBitsX+nBitsY-m_nHit16[dcx]-m_nHit16[dcy];
      }
      //
      //
      // fill:        dcBitsLookup2[][]
      //
      //   this table serves for ORing a new TSP-space pattern to an existing
      //      (encoded) DC-space pattern
      //   it returns the logical OR of 
      //   two encoded DCHB patterns
      //     dchbNew = dcBitsLookup2[dchb1][dchb2]
      VECTOR<uint64_t> &dcBitsLookup2=m_dcBitsLookup2[layer];
      dcBitsLookup2.resize(1<<(2* dcHbBitsLayer)); // was 4*  STS 25.10.2016
      for(unsigned xy1xy2=0;xy1xy2<dcBitsLookup2.size();xy1xy2++) {
         unsigned xy[2],hbx[2],hby[2],dcx[2],dcy[2];
         for(int i=0;i<2;i++) {
            xy[i]=(xy1xy2>>(i*dcHbBitsLayer)) &
               ((1<<(dcHbBitsLayer))-1);
            hbx[i]=xy[i] & (nPosX-1);
            dcx[i]=(xy[i]>>nBitsX) & (nPosX-1);
            hby[i]=(xy[i]>>(2*nBitsX)) & (nPosY-1);
            dcy[i]=(xy[i]>>(2*nBitsX+nBitsY)) & (nPosY-1);
            // not for lookup table 2, because indices are already encoded
            //if(nBitsX) hbx[i]=m_TSPmap->IEEE2GC(nBitsX,hbx[i]);
            //if(nBitsY) hby[i]=m_TSPmap->IEEE2GC(nBitsY,hby[i]);
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
   }
}

void FTK_CompressedAMBank::readSectorDefinition
(const char *sectorFileHWMODEID0,const char *sectorFileHWMODEID2) {
   char const *name=sectorFileHWMODEID0;
   VECTOR<VECTOR<int> > *moduleBySector=&m_moduleIdHW0;
   VECTOR<int> ntrack;
   int error=0;
   for(int i=0;i<2;i++) {
      ftk_dcap::istream *sectorFile=ftk_dcap::open_for_read(name);
      if(!sectorFile) {
	 Warning("readSectorDefinition")<<"failed to open file \""<<name
				      <<"\" for reading, skipping\n";
	 error=-10;
	 continue;
      }
      int nsectorRead,nplane;
      (*sectorFile)>>nsectorRead>>nplane;
      if(sectorFile->fail()||
         ((getNPlanes()>0)&&(nplane!=getNPlanes()))||
         (nplane<=0)||
         (nsectorRead<(int)ntrack.size())||
         ((int)ntrack.size()&&((int)ntrack.size()!=nsectorRead))) {
         Error("readSectorDefinition")
            <<"read error file="<<name
            <<" fail="<<sectorFile->fail()
            <<" nplane="<<nplane<<" (expect "<<getNPlanes()<<")"
            <<" nsector="<<nsectorRead<<" (expect "<<ntrack.size()<<")\n";
      }
      if(!getNPlanes()) setNPlanes(nplane);
      moduleBySector->resize(nplane);
      for(int p=0;p<nplane;p++) {
         (*moduleBySector)[p].resize(nsectorRead);
      }
      ntrack.resize(nsectorRead);
      for(int sector=0;sector<(int)ntrack.size();sector++) {
         int ii,dummy;
         (*sectorFile)>>ii;
         if(ii!=sector) {
            error++;
            break;
         }
         for(int n=0;n<nplane;n++) {
            (*sectorFile)>>(*moduleBySector)[n][sector];
         }
         int ntk=ntrack[sector];
         (*sectorFile)>>dummy>>ntrack[sector];
         if(sectorFile->fail() || ((ntk>0)&&(ntk!=ntrack[sector]))) {
            error++;
            break;
         }
      }
      delete sectorFile;
      name=sectorFileHWMODEID2;
      moduleBySector=&m_moduleIdHW2;
   }
   if(error>0) {
      Error("readSectorDefinition")
         <<"sector files \""<<sectorFileHWMODEID0<<"\" and \""
         <<sectorFileHWMODEID2<<"\"are not consistent\n";
   }
   else if ( error<0 ) {
      Warning("readSectorDefinition")
         <<"sector files not available: HWMODE=2 not usable.\n";
   }
#ifdef WRITE_DEADMODULE_TEMPLATE
   // write dead module template file
   std::ostringstream allmoduleFileName;
   allmoduleFileName<<"allmodules.reg"<<getBankID()<<".txt";
   std::ofstream allmodules(allmoduleFileName.str().c_str());
   for(int iplane=0;iplane<getNPlanes();iplane++) {
      FTKHit hit;
      MAP<int,int> moduleid;
      for(unsigned isector=0;(isector<m_moduleIdHW0[iplane].size())&&
             (isector<m_moduleIdHW2[iplane].size());isector++) {
         moduleid[m_moduleIdHW2[iplane][isector]]=
            m_moduleIdHW0[iplane][isector];
      }
      for(MAP<int,int>::const_iterator im=moduleid.begin();im!=moduleid.end();
          im++) {
         hit.setSector((*im).second);
         allmodules<<" "<<(m_TSPmap->getDim(iplane)-1)
                   <<" "<<(hit.getIsBarrel() ?0 : 1)
                   <<" "<<(*im).second
                   <<" "<<iplane
                   <<" "<<hit.getEtaModule()
                   <<" "<<hit.getPhiModule()
                   <<" "<<hit.getSection()
                   <<" "<<(*im).first<<"\n";
      }
   }
#endif
}

/**
   writeCachedBankFiles()

   dump patterns into a file, using the "cached bank" format
     (to be used by class FTKTSPBank)

*/
int FTK_CompressedAMBank::writePCachedBankFile
(char const *cachedBankFile,int flatFormat,int nsub,int nlamb) const {
   TDirectory *outputFile=
      FTKRootFile::Instance()->CreateRootFile(cachedBankFile);
   TTree *amtree;
   // loop over subregions
   //  if(nsub==0) the loop is executed only once and all patterns are
   //    stored in one TTree
   //  if(nsub>0) the loop is executed multiple times and
   //    subregions are stored in different TTrees
   amtree=new TTree("Bank","Pattern bank");
   FTKPattern *curpatt =0;
   //int patternID; // pattern identification
   int nplanes; // number of planes
   int ssid[8]; //[m_nplanes] SS id on each plane
   int sectorID; // sector containing this pattern
   //int coverage; // number of events that created this pattern
   int dcmask; // DC mask for this pattern
   int hbmask; // half-bin mask
   int nTSP;
   int coverage;
   if(flatFormat) {
      //amtree->Branch("patternID",&patternID,"patternID/I");
      amtree->Branch("nplanes",&nplanes,"nplanes/I");
      amtree->Branch("ssid",ssid,"ssid[nplanes]/I");
      amtree->Branch("sectorID",&sectorID,"sectorID/I");
      //amtree->Branch("coverage",&coverage,"coverage/I");
      amtree->Branch("dcmask",&dcmask,"dcmask/I");
      amtree->Branch("hbmask",&hbmask,"hbmask/I");
      if( m_bank.m_numTSP.size()) {
         amtree->Branch("nTSP",&nTSP,"nTSP/I"); 
         amtree->Branch("coverage",&coverage,"coverage/I"); 
      }
   } else {
      curpatt=new FTKPattern(getNPlanes());
      amtree->Branch("Pattern", &curpatt);
   }
   // set the number of planes once for all
   if(flatFormat) {
      nplanes=getNPlanes();
   } else {
      curpatt->setNPlanes(getNPlanes());
   }
   if(!flatFormat) {
      curpatt->setCoverage(-1);
   }
   // patternID counter, required for writing special patterns
   int patternID=0;
   // loop over sectors
   int isub=getSubID();
   MAP<int,int> sectorByPatternID;
   VECTOR<int> sectorWritten;
   for(MAP<int,std::pair<int,int> >::const_iterator isector=
          m_SectorFirstLastPattern.begin();
       isector!=m_SectorFirstLastPattern.end();isector++) {
      sectorByPatternID[(*isector).second.first]=(*isector).first;
   }
   for(MAP<int,int>::const_iterator iSector=sectorByPatternID.begin();
       iSector!=sectorByPatternID.end();iSector++) {
      int sector=(*iSector).second;
      if(nsub && (!flatFormat) &&((sector %nsub)!=isub)) continue;
      if(!((sector-isub)%100)) {
         Info("writePCachedBankFile")<<"sector="<<sector<<"\n";
      }
      sectorWritten.push_back(sector);
      MAP<int,std::pair<int,int>>::const_iterator isector=
         m_SectorFirstLastPattern.find(sector);
      int firstPattern=(*isector).second.first;
      int lastPattern=(*isector).second.second;
      // loop over all patterns in the sector
      sectorID=sector;
      for(int ipattern=firstPattern;ipattern<=lastPattern;ipattern++) {
         // skip over reserved patterns (HW compatibility)
         if(isReservedPatternId(ipattern)) continue;
         // write special pattern if required
         while(patternID<ipattern) {
            if(!flatFormat) {
               curpatt->clearSubPatterns();
               curpatt->setPatternID(patternID);
               curpatt->setPatternDBID(patternID);
               for (int ipl=getNPlanes()-1;ipl>=0;--ipl) {
                  curpatt->setSSID(ipl,s_WILDCARDid);
               }
               curpatt->setSectorID(sector);
               curpatt->setDCMask(0);
               curpatt->setHBMask(0);
            } else {
               dcmask =0;
               hbmask =0;
               coverage=0;
               nTSP=0;
               for (int ipl=getNPlanes()-1;ipl>=0;--ipl) {
                  ssid[ipl]=s_WILDCARDid;
               }
            }
            amtree->Fill();
            patternID++;
         }

         if(!flatFormat) {
            curpatt->clearSubPatterns();
            curpatt->setPatternID(patternID);
            curpatt->setPatternDBID(patternID);
         }
         // DC bits and HB mask are composed inside the loop 
         dcmask=0;
         hbmask=0;
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
            // SSID index is encoded in the upper bits
            int dcSSIDindex=dcSSID_with_DCbits >> nDCHBbits;
            // SSID index is converted to SSID using lookup-table
            int dcSSID=m_bank.m_PatternByLayer[ipl].m_dcSSIDbySectorIndex
               [sector][dcSSIDindex];
            // DC and HB bits are encoded in the lower bits
            int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
            // use lookup-table to translate and add these bits to the
            //  DC,HB words
            dcmask =(dcmask<<nDCbits) | m_dcMaskLookup[ipl][dcBits];
            hbmask =(hbmask<<nDCbits) | m_hbMaskLookup[ipl][dcBits];
            // set SSID
            if(flatFormat) {
               ssid[ipl]=dcSSID;
            } else {
               curpatt->setSSID(ipl,dcSSID);
            }
         }
         if(m_bank.m_numTSP.size()>(size_t)ipattern) {
            coverage=m_bank.m_coverage[ipattern];
            nTSP=m_bank.m_numTSP[ipattern];
         } else {
            coverage=0;
            nTSP=0;
         }
         if(!flatFormat) {
            // specify the sector
            curpatt->setSectorID(sector);
            // set DC,HB words
            curpatt->setDCMask(dcmask);
            curpatt->setHBMask(hbmask);
         }
         // store this pattern
         amtree->Fill();
         patternID++;
      }
   }
   if(curpatt) delete curpatt;
   // write tree
   amtree->Write("", TObject::kOverwrite);

   if(flatFormat) {
      // sector information
      TTree *auxtree=new TTree("Sector","Sector-wise auxillary information");
      unsigned int ndc[8]; // number of DC bits per plane
      unsigned int wildcard;
      //unsigned lowestSSID;
      int sector,firstPattern,lastPattern,isub,lamb;
      auxtree->Branch("nplanes",&nplanes,"nplanes/I");
      auxtree->Branch("ndc",ndc,"ndc[nplanes]/I");
      auxtree->Branch("sector",&sector,"sector/I");
      auxtree->Branch("first",&firstPattern,"first/I");
      auxtree->Branch("last",&lastPattern,"last/I");
      auxtree->Branch("lamb",&lamb,"lamb/I");
      auxtree->Branch("isub",&isub,"isub/I");
      auxtree->Branch("wildcard",&wildcard,"wildcard/I");
      //auxtree->Branch("lowestSSID",&lowestSSID,"lowestSSID/I");
      // set number of DC bits per plane (constant)
      for(int i=0;i<getNPlanes();i++) {
         ndc[i]=m_TSPmap->getNBits(i  );
      }
      for(size_t i=0;i<sectorWritten.size();i++) {
         sector=sectorWritten[i];
         MAP<int,std::pair<int,int> >::const_iterator isector=
            m_SectorFirstLastPattern.find(sector);
         firstPattern=(*isector).second.first;
         lastPattern=(*isector).second.second;
         isub= nsub ? (sector % nsub) : 0;
         lamb= nlamb ? (sector % nlamb) : 0;
         wildcard=m_SectorWC[sector];
         //lowestSSID=;
         auxtree->Fill();
      }
      auxtree->Write("", TObject::kOverwrite);
      // plane information
      int plane,ndcx,ndcy,sswx,sswy,modwx,modwy;
      auxtree=new TTree("SSMap","Plane-wise auxillary information");
      auxtree->Branch("plane",&plane,"plane/I");
      auxtree->Branch("ndcx",&ndcx,"ndcx/I");
      auxtree->Branch("ndcy",&ndcy,"ndcy/I");
      auxtree->Branch("sswx",&sswx,"sswx/I");
      auxtree->Branch("sswy",&sswy,"sswy/I");
      auxtree->Branch("modwx",&modwx,"modwx/I");
      auxtree->Branch("modwy",&modwy,"modwy/I");
      for(plane=0;plane<getNPlanes();plane++) {
         ndcx=m_TSPmap->getNBits(plane,0);
         ndcy=m_TSPmap->getNBits(plane,1);
         sswx=m_ssmap->getMap(plane,0,0).m_phiss;
         sswy=m_ssmap->getMap(plane,0,0).m_etass;
         modwx=m_ssmap->getMap(plane,0,0).m_phiwidth;
         modwy=m_ssmap->getMap(plane,0,0).m_etawidth;
         auxtree->Fill();
      }
      auxtree->Write("", TObject::kOverwrite);
#ifdef UNUSED
      // SSID encoding information
      auxtree=new TTree("SSID","Superstrip encoding auxillary information");
      auxtree->Branch("plane",&plane,"plane/I");
      auxtree->Branch("ssid",&ssid,"ssid/I");
      auxtree->Branch("moduleLocal",&local,"moduleLocal/I");
      auxtree->Branch("moduleHash",&hash,"moduleHash/I");
      auxtree->Branch("localX",&localX,"localX/I");
      auxtree->Branch("localY",&localY,"localY/I");
      auxtree->Branch("dcpart",&dcpart,"dcpart/I");
      auxtree->Write("", TObject::kOverwrite);
#endif

   }
   //
   // silently add ccache format
   if(flatFormat>1) {
      writeCCachedBankFile(outputFile);
   }
   // close file
   delete outputFile;
   return 0;
}

/**
   methods to decide which SSID adressing mode is used for DC and for TSP

   getHWmodeSS_dc()
   getHWmodeSS_tsp()

*/

int FTK_CompressedAMBank::getHWModeSS_dc(void) const {
   if(m_hwmodeIDdc<0) return FTKSetup::getFTKSetup().getHWModeSS();
   return m_hwmodeIDdc;
}

int FTK_CompressedAMBank::getHWModeSS_tsp(void) const {
   if(m_hwmodeIDtsp<0) return FTKSetup::getFTKSetup().getHWModeSS();
   return m_hwmodeIDtsp;
}

/**
   method to convert tspSSID (fine geometry)
   to a dcSSID (coarse geometry) and a subSS index 

   return.first   is the dcSSID
   return.second  is the subSS index

   the SSIDs are buffered in a map for efficient retreival
 */
std::pair<int,int> const &FTK_CompressedAMBank::getDCssid
(int layer,int sector,int tspSSID) {
   MAP<int,std::pair<int,int> >::const_iterator
      i=m_TSPtoDC[layer].find(tspSSID);
   if(i==m_TSPtoDC[layer].end()) {
      insertSSID(layer,sector,tspSSID,s_WILDCARDid);
      i=m_TSPtoDC[layer].find(tspSSID);
   }   
   return (*i).second;
}

int FTK_CompressedAMBank::getDCssidConst
(int layer,int tspSSID) const {
   MAP<int,std::pair<int,int> >::const_iterator
      i=m_TSPtoDC[layer].find(tspSSID);
   if(i==m_TSPtoDC[layer].end()) {
      return s_INVALIDid;
   }
   return (*i).second.first;
}

/**
   method to convert dcSSID (coarse geometry)
   to a list of tspSSID (fine geometry)

   the SSIDs are buffered in a map for efficient retreival
 */
std::vector<int> const &FTK_CompressedAMBank::getTSPssidVector
(int layer,int sector,int dcSSID) {
   MAP<int,std::vector<int> >::const_iterator
      i=m_DCtoTSP[layer].find(dcSSID);
   if(i==m_DCtoTSP[layer].end()) {
      insertSSID(layer,sector,s_WILDCARDid,dcSSID);
      i=m_DCtoTSP[layer].find(dcSSID);
   }   
   return (*i).second;
}

/**
   method to convert tspSSID (fine geometry)
   to a dcSSID (coarse geometry)
 */

int FTK_CompressedAMBank::getDCssidSlow
(int layer,int sector,int tspSSID,int *moduleID) {
   int dcSSID=-1;
   int nDim=m_TSPmap->getDim(layer);
   if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      dcSSID = tspSSID;
   } else {
      int phimod=-1,etacode=-1,section=0;
      int localX=0,localY=0;
      int localModuleID=-1;
      if (getHWModeSS_tsp()==0) {
         // ********* GLOBAL SSID HW Format **********
         if (nDim==2) {
            // PXL case
            getSSMapTSP()->decodeSSxy
               (tspSSID,layer,section,phimod,localX,etacode,localY);
            if(moduleID) *moduleID=getSSMapTSP()->getSSxy
               (layer,section,phimod,etacode,0,0);
         } else {
            // SCT case
            getSSMapTSP()->decodeSSx
               (tspSSID,layer,section,phimod,localX,etacode);
            if(moduleID) *moduleID=getSSMapTSP()->getSSx
               (layer,section,phimod,etacode,0);
         }
      } else if (getHWModeSS_tsp()==2) {
         // here, assume section=0 for encoding/decoding the SSID
         // otherwise it gets ***REALLY** complicated
         if (nDim==2) {
            getSSMapTSP()->decodeSSTowerXY(tspSSID,getBankID(),layer,section,
                                           localModuleID,localX,localY);
         } else {
            getSSMapTSP()->decodeSSTowerX(tspSSID,getBankID(),layer,section,
                                          localModuleID,localX);
         }
         if(moduleID) *moduleID=localModuleID;
      } else {
         Fatal("getDCssidSlow")
            <<"hardware mode="<<getHWModeSS_tsp()
            <<" (TSP) not supported\n";
      }
      if((phimod<0)&&(localModuleID<0)) {
         Fatal("getDCssidSlow")
            <<"could not decode TSP-SSID="<<tspSSID
            <<" layer="<<layer
	    <<" ndim="<<nDim
            <<" localModuleID="<<localModuleID
	    <<" phimod=" << phimod
	    <<" localModuleID=" << localModuleID
	    <<" getHWModeSS_tsp()=" << getHWModeSS_tsp()
            <<"\n";
      }
      if (getHWModeSS_dc()==0) {
         if(phimod<0) {
            if(m_moduleIdHW0.size()) {
               static FTKHit hit;
               hit.setSector(m_moduleIdHW0[layer][sector]);
               phimod=hit.getPhiModule();
               section=hit.getSection();
               if((nDim==2)&& !hit.getIsBarrel()) {
                  etacode=14+3*(hit.getASide() ? 1 : 0)+section;
               } else {
                  etacode=hit.getEtaCode();
               }
            } else {
               Fatal("getDCssidSlow")<<"no lookup-table to convert HWMODEID=2 to HWMODEID=0\n";
            }
         }
         if (nDim==2) {
            dcSSID=getSSMap()->getSSxy
               (layer,section,phimod,etacode,localX,localY);
         } else {
            dcSSID=getSSMap()->getSSx
               (layer,section,phimod,etacode,localX);
         }
      } else if(getHWModeSS_dc()==2) {
         if(localModuleID<0) {
            if(m_moduleIdHW2.size()) {
               int moduleID=m_moduleIdHW2[layer][sector];
               localModuleID=getSSMapTSP()->getRegionMap()
                  ->getLocalId(getBankID(),layer,moduleID);
               if(localModuleID<0) {
                  Fatal("getDCssidSlow")
                     <<"plane="<<layer
                     <<" sector="<<sector
                     <<" phimod="<<phimod
                     <<" etacode="<<etacode
                     <<" section="<<section
                     <<" moduleID="<<moduleID
                     <<" localModuleID="<<localModuleID<<"\n";
               }
            } else {
               Fatal("getDCssidSlow")<<"no lookup-table to convert HWMODEID=0 to HWMODEID=2\n";
            }
         }
         if (nDim==2) {
#ifdef HW2_USE_TSPMAP
            getSSMapTSP()->setSSDCX(layer,m_TSPmap->getNBits(layer,0));
            getSSMapTSP()->setSSDCY(layer,m_TSPmap->getNBits(layer,1));
            dcSSID=getSSMapTSP()->compressed_ssid_word_pixel
               (localModuleID,layer,section,localX,localY)
               >>m_TSPmap->getNBits(layer);
            getSSMapTSP()->setSSDCX(layer,0);
            getSSMapTSP()->setSSDCY(layer,0);
#else
            dcSSID=getSSMap()->compressed_ssid_word_pixel
               (localModuleID,layer,section,localX,localY);
#endif
         } else {
#ifdef HW2_USE_TSPMAP
            getSSMapTSP()->setSSDCX(layer,m_TSPmap->getNBits(layer,0));
            dcSSID=getSSMapTSP()->compressed_ssid_word_strip
               (localModuleID,layer,section,localX)
               >>m_TSPmap->getNBits(layer);
            getSSMapTSP()->setSSDCX(layer,0);
#else
            dcSSID=getSSMap()->compressed_ssid_word_strip
               (localModuleID,layer,section,localX);
#endif
         }
      } else {
         Fatal("getDCssidSlow")
            <<"hardware mode="<<getHWModeSS_dc()
            <<" (DC) not supported\n";
      }
#ifdef TESTDCSSID
      if(dcSSID==TESTDCSSID) {
         std::cout<<" TSP->DC dc="<<dcSSID<<" tsp="<<tspSSID
                  <<" module(HW0)="<<m_moduleIdHW0[layer][sector]
                  <<" module(HW2)="<<m_moduleIdHW2[layer][sector]
                  <<" phimod="<<phimod<<" secion="<<section
                  <<" etacode="<<etacode<<"\n";
      }
#endif
      if(dcSSID<0) {
         Info("getDCssidSlow")
            <<"bad SSID conversion bank="<<getBankID()<<" tsp="<<tspSSID
            <<" -> dc="<<dcSSID
            <<" HWMODEID TSP="<<getHWModeSS_tsp()<<" DC="<<getHWModeSS_dc()
            <<" layer,section,phimod,etacode,moduleid,localX,localY "
            <<layer<<" "<<section<<" "<<phimod<<" "<<etacode
            <<" "<<localModuleID<<" "<<localX<<" "<<localY
            <<"\n";
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
 */
int FTK_CompressedAMBank::getTSPssidSlow
(int layer,int sector,int dcSSID,int tspXY /* ,int *moduleID */) {
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
      int phimod=-1,etacode=-1,section=0;
      int localX=0,localY=0;
      int localModuleID=-1;
      if (getHWModeSS_dc()==0) {
         // ********* GLOBAL SSID HW Format **********
         if (nDim==2) {
            // PXL case
            getSSMap()->decodeSSxy
               (dcSSID,layer,section,phimod,localX,etacode,localY);
            localY += iy*etassTSP;
            /* *moduleID=getSSMapTSP()->getSSxy
               (layer,section,phimod,etacode,0,0); */
         } else {
            // SCT case
            getSSMap()->decodeSSx
               (dcSSID,layer,section,phimod,localX,etacode);
            /* *moduleID=getSSMapTSP()->getSSx
               (layer,section,phimod,etacode,0); */
         }
         localX += ix*phissTSP;
      } else if (getHWModeSS_dc()==2) {
         // here, assume section=0 for encoding/decoding the SSID
         // otherwise it gets ***REALLY** complicated
         if (nDim==2) {
#ifdef HW2_USE_TSPMAP
            getSSMapTSP()->setSSDCX(layer,m_TSPmap->getNBits(layer,0));
            getSSMapTSP()->setSSDCY(layer,m_TSPmap->getNBits(layer,1));
            getSSMapTSP()->decodeSSTowerXY(dcSSID<<m_TSPmap->getNBits(layer),
                                           getBankID(),layer,section,
                                           localModuleID,localX,localY);
            getSSMapTSP()->setSSDCX(layer,0);
            getSSMapTSP()->setSSDCY(layer,0);
#else
            getSSMap()->decodeSSTowerXY(dcSSID,getBankID(),layer,section,
                                        localModuleID,localX,localY);
#endif
            localX += ix*phissTSP;
            localY += iy*etassTSP;
         } else {
#ifdef HW2_USE_TSPMAP
            getSSMapTSP()->setSSDCX(layer,m_TSPmap->getNBits(layer,0));
            getSSMapTSP()->decodeSSTowerX(dcSSID<<m_TSPmap->getNBits(layer),
                                          getBankID(),layer,section,
                                          localModuleID,localX);
            getSSMapTSP()->setSSDCX(layer,0);
#else
            getSSMap()->decodeSSTowerX(dcSSID,getBankID(),layer,section,
                                       localModuleID,localX);
#endif
            localX += ix*phissTSP;
         }
         // *moduleID=localModuleID;
      } else {
         Fatal("getTSPssidSlow")
            <<"hardware mode="<<getHWModeSS_dc()
            <<" (DC) not supported\n";
      }
      if((phimod<0)&&(localModuleID<0)) {
         Fatal("getTSPssidSlow")
            <<"could not decode DC-SSID="<<dcSSID
            <<" layer="<<layer<<" ndim="<<nDim
            <<" localModuleID="<<localModuleID
            <<"\n";
      }
      if (getHWModeSS_tsp()==0) {
         if(phimod<0) {
           if(m_moduleIdHW0.size()) {
              static FTKHit hit;
              hit.setSector(m_moduleIdHW0[layer][sector]);
              phimod=hit.getPhiModule();
              section=hit.getSection();
              if((nDim==2)&& !hit.getIsBarrel()) {
                 etacode=14+3*(hit.getASide() ? 1 : 0)+section;
              } else {
                 etacode=hit.getEtaCode();
              }
#ifdef TESTDCSSID
              if(dcSSID==TESTDCSSID) {
                 std::cout<<" DC->TSP dc="<<dcSSID<<" tsp="<<tspSSID
                          <<" module(HW0)="<<m_moduleIdHW0[layer][sector]
                          <<" module(HW2)="<<m_moduleIdHW2[layer][sector]
                          <<" phimod="<<phimod<<" secion="<<section
                          <<" etacode="<<etacode<<"\n";
              }
#endif
            } else {
               Fatal("getTSPssidSlow")<<"no lookup-table to convert HWMODEID=2 to HWMODEID=0\n";
            }
         }
         if (nDim==2) {
            tspSSID=getSSMapTSP()->getSSxy(layer,section,phimod,etacode,
                                         localX,localY);
         } else {
            tspSSID=getSSMapTSP()->getSSx
               (layer,section,phimod,etacode,localX);
         }
      } else if(getHWModeSS_tsp()==2) {
         if(localModuleID<0) {
            if(m_moduleIdHW2.size()) {
               int moduleID=m_moduleIdHW2[layer][sector];
               localModuleID=getSSMapTSP()->getRegionMap()
                  ->getLocalId(getBankID(),layer,moduleID);
               if(localModuleID<0) {
                  Fatal("getTSPssidSlow")
                     <<"plane="<<layer
                     <<" sector="<<sector
                     <<" phimod="<<phimod
                     <<" etacode="<<etacode
                     <<" section="<<section
                     <<" moduleID="<<moduleID
                     <<" localModuleID="<<localModuleID<<"\n";
               }
            } else {
               Fatal("getTSPssidSlow")<<"no lookup-table to convert HWMODEID=0 to HWMODEID=2\n";
            }
         }
         if (nDim==2) {
            tspSSID=getSSMapTSP()->
               compressed_ssid_word_pixel(localModuleID,layer,section,localX,localY);
         } else {
            tspSSID=getSSMapTSP()->
               compressed_ssid_word_strip(localModuleID,layer,section,localX);
         }
      } else {
         Fatal("getTSPssidSlow")
            <<"hardware mode="<<getHWModeSS_dc()
            <<" (TSP) not supported\n";
      }
   }
   if(tspSSID<0) {
      Fatal("getTSPssidSlow")
         <<"bad SSID conversion dc="<<dcSSID<<" xy="<<tspXY
         <<" -> tsp="<<tspSSID<<"\n";
   }
   return tspSSID;
}

/**
   auxillary method to convert dcSSID (coarse geometry)
   to tspSSID (fine geometry) and back

   results are buffered in lookup-tables
 */
void FTK_CompressedAMBank::insertSSID
(int layer,int sector,int tspSSID,int dcSSID0) {
   if(!m_TSPmap) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   }
   // here:
   //  (a) (tspSSID==s_INVALIDid)||(dcSSID0==s_INVALIDid)
   //   -> lookup requested for a bad module, no wildcard
   //  (b) (tspSSID==s_WILDCARDid)&&(dcSSID0==s_WILDCARDid)
   //   -> lookup requested for a bad module, with wildcard
   //  (c) normal lookup, either tspSSID or dcSSID0 are valid

   // if there is no dcSSID, convert TSP to DC
   int dcSSID=dcSSID0;
   // if TSP is valid, convert to DC
   if(tspSSID!=s_WILDCARDid) {
      // (a) or (c)
      if(tspSSID!=s_INVALIDid) {
         dcSSID=getDCssidSlow(layer,sector,tspSSID,0);
      } else {
         dcSSID=s_INVALIDid;
      }
   }
   // at this point:
   //  (a1) tspSSID==s_INVALIDid  && dcSSID==s_INVALIDid
   //  (a2) tspSSID==s_WILDCARDid  && dcSSID==s_INVALIDid
   //  (b)  tspSSID==s_WILDCARDid && dcSSID==s_WILDCARDid
   //  (c1) tspSSID=={valid} && dcSSID={valid}
   //  (c2) tspSSID==s_WILDCARDid && dcSSID={valid}

   // create vector to hold all TSP for the given DC
   int nPos=1;
   if(!FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
      nPos=1<<m_TSPmap->getNBits(layer);
   }
   std::vector<int> tsp(nPos);
   //int error=0;
   // loop over all subSS indexes, get TSP from DC 
   bool match=false;
   for(int iTSP=0;iTSP<nPos;iTSP++) {
      if((dcSSID!=s_INVALIDid)&&(dcSSID!=s_WILDCARDid)) {
         // (c1) or (c2)
         tsp[iTSP]=getTSPssidSlow(layer,sector,dcSSID,iTSP);
      } else {
         // (a1) or (a2) or (b)
         // copy DC-id which is set properly,
         //  either s_INVALIDid or s_WILDCARDid
         tsp[iTSP]=dcSSID;
      }
      if(tsp[iTSP]==tspSSID) match=true;
      // insert (DC,ssindex) into table ordered by TSP
      m_TSPtoDC[layer].insert
         (std::make_pair(tsp[iTSP],std::make_pair(dcSSID,iTSP)));
   }
   int moduleID=-1;
   if((tspSSID!=s_WILDCARDid)&&(tspSSID!=s_INVALIDid)) {
      int dcSSID1=getDCssidSlow(layer,sector,tspSSID,&moduleID);
      if((!match)||(dcSSID1!=dcSSID)) {
         std::ostream &fatal=Fatal("insertSSID");
         fatal<<"SSID inconsistency: layer="<<layer
              <<" sector="<<sector<<" TSP0="<<tspSSID
              <<" (DC="<<dcSSID1<<")"
              <<" DC="<<dcSSID<<" TSP=[";
         for(int i=0;i<nPos;i++) {
            fatal<<" "<<tsp[i]<<" (DC="<<getDCssidSlow(layer,sector,tsp[i],0)<<")";
         }
         fatal<<"]\n";
      }
   }
   // insert vector<TSP> into table ordered by DC
   m_DCtoTSP[layer].insert(make_pair(dcSSID,tsp));

   // handle bad modules
   MAP<int,int>::const_iterator im=m_badModules[layer].find(moduleID);
   if(im!=m_badModules[layer].end()) {
      int type=(*im).second;
      for(int iTSP=0;iTSP<nPos;iTSP++) {
         MAP<int,std::pair<int,int> >::iterator
            tspPtr= m_TSPtoDC[layer].find(tsp[iTSP]);
         (*tspPtr).second=std::make_pair(type,iTSP);
      }
      MAP<int,std::vector<int> >::iterator
         dcPtr=m_DCtoTSP[layer].find(dcSSID);
      for(int iTSP=0;iTSP<nPos;iTSP++) {
         (*dcPtr).second[iTSP]=type;
      }
   }

   // for now cannot enter here, does nothing!
   // if(error) {
   //    std::cout<<"insertSSID: DC0="<<dcSSID0<<" TSP="<<tspSSID
   //             <<" DC="<<dcSSID
   //             <<" ->";
   //    for(unsigned i=0;i<tsp.size();i++) {
   //       std::cout<<" TSP["<<i<<"]="<<tsp[i];
   //    }
   //    std::cout<<"\n";
   //    Fatal("insertSSID")<<"problem while converting SSIDs\n";
   // }
}

static int ssidCompare(const void *a,const void *b) {
   int const *ia=(int *)a;
   int const *ib=(int *)b;
   for(int i=0;i<8;i++) {
      if(ia[i]<ib[i]) return -1;
      if(ia[i]>ib[i]) return 1;
   }
   return 0;
}

/**
   readROOTBankCache()

   interface provided by base class, to read bank in pcache format
*/
int FTK_CompressedAMBank::readROOTBankCache(char const *cachedBank) {
   //typedef MAP<FTKHitPattern,std::pair<int,int>,FTKHitPatternCompare>
   //   FTKDCPatternOneSector_t;
   TDirectory *inputFile=
      FTKRootFile::Instance()->OpenRootFileReadonly(cachedBank);
   if(readCCachedBank(inputFile)) {
      Warning("readROOTBankCache")
         <<"could not read pattern bank (ccache format) from \""
         <<cachedBank<<"\", try as pcache\n";
      if(readPCachedBank(inputFile)) {
         Error("readROOTBankCache")
            <<"could not read pattern bank (pcache format) from \""
            <<cachedBank<<"\"\n";
         return -1;
      }
   }
   delete inputFile;
   return 0;
}

/**
   readPCachedBank()

   read bank in pcache format (the format written by FTKTSPBank)
*/

int FTK_CompressedAMBank::readPCachedBank(TDirectory *inputDir,int nlamb) {
   MAP<int,int> sectorcount;
   int error=0;
   int32_t *ssidData=0;
   // sectorPointer[sector].first : sector start
   // sectorPointer[sector].second : sector end
   VECTOR<std::pair<int,int> > sectorPointer;
   int nLayer=0;
   int maxSector=0;
   int nDCHB=2;
   int offsetSSID=nDCHB;
   TTree *tree;
   // "Bank" tree is a list of FTKPattern objects
   inputDir->GetObject("Bank",tree);
   if(tree) {
      int nPattern=tree->GetEntriesFast();
      Info("readPCachedBank")
         <<"\""<<inputDir->GetName()<<"\" number of patterns: "
         <<nPattern<<"\n";
      FTKPattern *pattern=new FTKPattern();
      if(tree->SetBranchAddress("Pattern",&pattern)!=0) {
         // no match, assume this is a flat file
         Info("readPCachedBank")<<"try to read flat format\n";
         delete pattern;
         pattern=0;
      }
      TBranch *branch_nplanes;
      TBranch *branch_sectorID;
      int nplanes,sectorID;
      if(!pattern) {
         // flat format
         // acces branches and set addresses
         branch_nplanes=tree->GetBranch("nplanes");
         branch_sectorID=tree->GetBranch("sectorID");
         branch_nplanes->SetAddress(&nplanes);
         branch_sectorID->SetAddress(&sectorID);
      } else {
         // access data members by their name
         // no need to set address, done by SetBranchAddress("Pattern",...)
         branch_nplanes=tree->GetBranch("m_nplanes");
         branch_sectorID=tree->GetBranch("m_sectorID");
      }
      // loop over all patterns, determine maximum sector number
      //   and number of planes
      //int isub=getSubID();
      for(int iPattern=0;iPattern<nPattern;++iPattern) {
         // only read the branches which are really required
         branch_sectorID->GetEntry(iPattern);
         if(pattern) {
            sectorID=pattern->getSectorID();
         }
         //if(nsub && (sectorID %nsub != isub)) continue;
         branch_nplanes->GetEntry(iPattern);
         if(pattern) {
            // non-flat format, extract data from class FTKPattern
            nplanes=pattern->getNPlanes();
         }
         if(nplanes>nLayer) {
            nLayer=nplanes;
         }
         sectorcount[sectorID]++;
      }
      if(pattern) pattern->setNPlanes(nLayer);
      offsetSSID+=nLayer;
      // store all patterns in a big int[] array
      // [0]..[nlayer-1] SSID data
      // [nlayer] dc bits
      // [nlayer+1] hb bits
      // the array is sector-ordered
      ssidData=new int32_t[offsetSSID*nPattern];
      maxSector=(*sectorcount.rbegin()).first+1;
      Info("readPCachedBank")
         <<"nLayer="<<nLayer<<" maxSector="<<maxSector<<"\n";
      sectorPointer.resize(maxSector);
      int pos=0;
      // in this loop, determine the pattern start address for each sector
      //  valid within the array "ssidData"
      for(MAP<int,int>::const_iterator isector=sectorcount.begin();
          isector!=sectorcount.end();isector++) {
         sectorPointer[(*isector).first]=std::make_pair(pos,pos);
         pos+=(*isector).second*offsetSSID;
      }
      // read all patterns from the cached bank
      TBranch *branch_ssid;
      TBranch *branch_dcmask;
      TBranch *branch_hbmask;
      if(!pattern) {
         // flat format: find branches and set address
         branch_ssid=tree->GetBranch("ssid");
         branch_dcmask=tree->GetBranch("dcmask");
         branch_hbmask=tree->GetBranch("hbmask");
      } else {
         // non-flat format, find branches using
         //   names of data-members in FTKPattern
         //   (no need to set addresses)
         branch_ssid=tree->GetBranch("m_ssid");
         branch_dcmask=tree->GetBranch("m_dcmask");
         branch_hbmask=tree->GetBranch("m_hbmask");
      }
      for(int iPattern=0;iPattern<nPattern;++iPattern) {
         // get sector number
         branch_sectorID->GetEntry(iPattern);
         if(!pattern) {
            // flat file
            // determine position in ssidData array
            int &pos=sectorPointer[sectorID].second;
            branch_ssid->SetAddress( ssidData+pos);
            pos+=nplanes;
            // here pos points to ssid address+nlayer -> DC bits
            branch_dcmask->SetAddress(ssidData+pos);
            pos++;
            // here pos points to ssid address+nlayer+1 -> HB bits
            branch_hbmask->SetAddress(ssidData+pos);
            pos++;
            // here pos points to ssid address+nlayer+2
            // !!!NOTE!!! pos is a reference, so this pointer is updated
            // for reading the next pattern in this sector
            if(!(iPattern %1000000))
               Debug("readPCachedBank")
                  <<iPattern<<" sector="<<sectorID<<"\n";
         }
         // read SSID data, DC, HB
         branch_ssid->GetEntry(iPattern);
         branch_dcmask->GetEntry(iPattern);
         branch_hbmask->GetEntry(iPattern);
         // at this point:
         //   if flat file: data has been stored in the ssidData array
         //                   -> done
         //   else: data is stored in "pattern"
         //                   -> copy to the desired location
         if(pattern) {
            if(!(iPattern %1000000))
               Debug("readPCachedBank")
                  <<iPattern<<" sector="<<pattern->getSectorID()<<"\n";
            // NOTE: pos is a reference, so increasing pos (below)
            //   automatically selects the proper position for reading
            //   the next pattern in this sector
            int &pos=sectorPointer[pattern->getSectorID()].second;
            for(int i=0;i<nLayer;i++) {
               // SSID layer 0..N-1
               ssidData[pos++]=pattern->getSSID(i);
            }
            ssidData[pos++]=pattern->getDCMask();
            ssidData[pos++]=pattern->getHBMask();
         }
      }
      if(pattern) delete pattern;
   } else {
      Fatal("readPCachedBank")
         <<"failed to read TTree \"Bank\" from \""
         <<inputDir->GetName()<<"\"\n";
      error=-2;
   }
   if(ssidData) {
      importDCpatterns(nLayer,offsetSSID,ssidData,sectorPointer,nlamb);
      delete [] ssidData;
   }
   readBankPostprocessing("readPCachedBank");
   return error;
}

//
// importDCpatterns()
//   nLayer: number of planes
//   offsetSSID: number of int per pattern
//   ssidData: pattern data
//   sectorPointer: first,last index of a sector in ssidData
//   nlamb: number of LAMB boards for grouping sectors

void FTK_CompressedAMBank::importDCpatterns
(int nLayer,int offsetSSID,int32_t *ssidData,
 VECTOR<std::pair<int,int> > const &sectorPointer,int nlamb) {
   //
   // clear old bank data
   erase();
   if(!m_TSPmap) {
      Fatal("importDCpatterns")<<"TSPmap not defined\n";
   }
   setNPlanes(nLayer);
   //
   // prepare information to decode DC bits
   //   the element xyFromHBDCandLayer[layer][dchb]
   //   returns the valid subSS indexes for the given DC,HB combination
   std::vector<std::vector<std::vector<int> > > xyFromHBDCandLayer(nLayer);
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
   // set number of layers
   m_bank.m_PatternByLayer.resize(nLayer);
   for(int iLayer=0;iLayer<nLayer;iLayer++) {
      // resize to the maximum sector ID
      m_bank.m_PatternByLayer[iLayer].
         m_CompressedDeltaBySector.resize(sectorPointer.size());
   }
   // subregion (LAMB) loop
   if(nlamb<=0) nlamb=1;
   for(int ilamb=0;ilamb<nlamb;ilamb++) {
      //
      // sector loop
      unsigned max_sectorByNlamb=(sectorPointer.size()-1)/nlamb;
      unsigned top_sectorByNlamb=0;
      while(top_sectorByNlamb+1 <= max_sectorByNlamb)
         top_sectorByNlamb=(top_sectorByNlamb+1)*2-1;
      // here: top_sectorByLamb has all bits set to one and is larger or equal the largest sector/nlamb
      unsigned leadingBitMask=(top_sectorByNlamb>>1)+1;
      unsigned sectorByLamb=0;
      while(true) {
         //for(unsigned sector=ilamb;sector<sectorPointer.size();sector+=nlamb) {
         unsigned sector=sectorByLamb*nlamb+ilamb;

         // count sectorByLamb from zero to top_sectorByNlamb
         // bits are counted in reverse order: MSB first, then next, etc:
         //  e.g. if top_sectorByNlamb has 12 bit:
         //   0x000, 0x800, 0x400, 0xc00, 0x200, 0xa00, 0x600, 0xe00, 0x100, ... 0xfff
         // This randomizes the sector ordering, for better load-balancing of patterns
         // in the hardware
         for(unsigned mask=leadingBitMask;mask;mask>>=1) {
            // count the bit indicated in "mask"
            sectorByLamb^=mask;
            // if the it is set, we are done
            if(sectorByLamb & mask) break;
            // if the bit is zero, have to count the next bit
         }
         if(!sectorByLamb) break; // all bits were counted, stop
         // when counting in reverse bit order, sectors are not produced in sequence
         // high (invalid) sector numbers are produced in random order and have
         // to be skipped
         if(sector>=sectorPointer.size()) continue; // skip sector numbers outside range
         int nPattern=(sectorPointer[sector].second-
                       sectorPointer[sector].first)/offsetSSID;
         if(!nPattern) continue;
         // sort patterns in each sector
#ifdef QSORT_FOR_DC_IMPORT
         int *patternDataSectorNC=ssidData+sectorPointer[sector].first;
         qsort(patternDataSectorNC,nPattern,sizeof(int)*offsetSSID,
               ssidCompare);
         int const *patternDataSector(patternDataSectorNC);
#else
         int const *patternDataSector(ssidData+sectorPointer[sector].first);
#endif
         // 
         // loop two times over all patterns in this sector
         //  first loop: count patterns
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
               // allocate memory (second loop only)
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
            for(int iPattern=0;iPattern<nPattern;iPattern++) {
               // reserve special pattern IDs as required by hardware
               // in the compressed bank, these IDs are simply not used
               if(isReservedPatternId(ipatt)) {
                  /* std::cout<<"Skipping reserved pattern "
                     <<std::setbase(16)<<ipatt<<std::setbase(10)<<"\n"; */
                  ipatt++;
               }
               int const *patternData=patternDataSector+iPattern*offsetSSID;
               // skip pattern if all IDs are invalid
               size_t invalid=0;
               for(unsigned iLayer=0;iLayer<compressionHelper.size();iLayer++) {
                  if((patternData[iLayer]==s_WILDCARDid)||
                     (patternData[iLayer]==s_INVALIDid)) invalid++;
               }
               if(invalid==compressionHelper.size()) continue;

               // DC information for this pattern
               uint32_t dcMaskAll=patternData[nLayer];
               uint32_t hbMaskAll=patternData[nLayer+1];
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
                  int dchb=(((((dcy<<nBitsY)|hby)<<nBitsX)|dcx)<<nBitsX)|hbx;
                  // expand DC bits to the list of valid subSS indexes
                  std::vector<int> const &tspIndexes=
                     xyFromHBDCandLayer[iLayer][dchb];
                  int ssid=patternData[iLayer];
                  // loop over subSS indexes within dcSSID
                  int numTSP=tspIndexes.size();
                  if((ssid==s_WILDCARDid)||(ssid==s_INVALIDid)) {
                     // wildcard/invalid pattern has no DC bits
                     // -> only one TSP pattern
                     numTSP=1;
                  }
                  for(int iTSP=0;iTSP<numTSP;iTSP++) {
                     // calculate TSP pattern address
                     uint32_t tspid=getTSPssid
                        (iLayer,sector,ssid,tspIndexes[iTSP]);
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
                           uint32_t &pos=helper.m_position;
                           uint32_t mask4=0x780;
                           int shift=7;
                           // check whether some bits >=7 are nonzero
                           //  -> need extra bytes to store this delta
                           for(uint32_t mask=0xffffff80;mask;mask<<=4) {
                              if((delta & mask)==0) break;
                              uint32_t bits=delta & mask4;
                              if(bits) {
                                 if(iLoop) {
                                    compressedDelta[data.m_offset+pos]=
                                       (bits>>shift)|((shift+25)<<2);
                                 }
                                 pos++;
                              }
                              mask4 <<=4;
                              shift+=4;
                           }
                           // count number of patterns
                           data.m_NPattern++;
                           // count number of data bytes needed
                           if(!delta) {
                              if((helper.m_repeatDelta1<1)||
                                 (helper.m_repeatDelta1>=16)) {
                                 helper.m_repeatDelta1=1;
                                 if(iLoop) {
                                    compressedDelta[data.m_offset+pos]=0;
                                 }
                                 pos++;
                              } else {
                                 helper.m_repeatDelta1++;
                                 if(iLoop) {
                                    compressedDelta[data.m_offset+pos-1]=
                                       helper.m_repeatDelta1 + 0xee;
                                 }
                              }
                           } else {
                              helper.m_repeatDelta1=0;
                              if(iLoop) {
                                 compressedDelta[data.m_offset+pos]=
                                    delta & 0x7f;
                              }
                              pos++;
                           }
                        }
                     }
                     helper.m_lastPattern=ipatt;
                  }
               }
               ipatt++;
            } // end loop over nPattern
            if(iLoop) {
               // extract auxillary information (coverage and #TSP patterns)
               if(offsetSSID>=nLayer+4) {
                  m_bank.m_numTSP.resize(ipatt);
                  m_bank.m_coverage.resize(ipatt);
                  ipatt=ipattSector;
                  for(int iPattern=0;iPattern<nPattern;iPattern++) {
                     if(isReservedPatternId(ipatt)) {
                        // skip over reserved IDs
                        ipatt++;
                     }
                     int const *patternData=
                        patternDataSector+iPattern*offsetSSID;
                     // Auxillary information for this pattern
                      m_bank.m_numTSP[ipatt]=
                         patternData[nLayer+2];
                      m_bank.m_coverage[ipatt]=
                         patternData[nLayer+3];
                      ipatt++;
                  }
               }
            }
         }
         ipattSector=ipatt;
         if((!(sector%100))||
            (sector<10)||((sector<100)&&(!(sector%10)))) {
            Debug("importDCpatterns")
               <<"sector "<<sector<<" nPatt="<<ipatt<<"\n";
         }
      } // end loop over sectors
   } // end loop over subregions
}

void FTK_CompressedAMBank::erase(void) {
   m_bank.m_PatternByLayer.resize(0);
   m_bank.m_pattern8Data.resize(0);
   m_bank.m_pattern16Data.resize(0);
   m_bank.m_numTSP.resize(0);
   m_bank.m_coverage.resize(0);
   setNPlanes(0);
   m_SectorWC.resize(0);
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
   m_npatterns=-1;
   // memory consumption (estimate)
   uint32_t memorySSIDlookup=0;
   uint32_t memoryPatternIDlookup=0;

   readWildcards();

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
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            int sectorID=(*sector).first;
            int dcSSID=getDCssid(layer,sectorID,(*ssid).first).first;
            // remember DC-level SSID for this sector
            // (insert a new entry)
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
            if(m_npatterns<maxPatternId)
               m_npatterns=maxPatternId;
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
   m_npatterns++;
   m_hitPatterns.resize(m_npatterns);
   m_roadCand.resize(MAX_NROAD);
   //
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
   // determine maximum sector ID
   int maxSector=0;
   if(m_SectorFirstLastPattern.rbegin()!=m_SectorFirstLastPattern.rend()) 
      maxSector=(*m_SectorFirstLastPattern.rbegin()).first+1;

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
         m_bank.m_pattern8Data[layer].resize(m_npatterns);
         memoryPatternIDlookup += sizeof(uint8_t)*m_npatterns;
      } else {
         m_bank.m_pattern16Data[layer].resize(m_npatterns);
         memoryPatternIDlookup +=  sizeof(uint16_t)*m_npatterns;
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
      // total number of ternary bits in this layer
      int hbBitsLayer=nBitsX+nBitsY;
      // total number of DC+HB bits in this layer
      int dcHbBitsLayer=2*hbBitsLayer;
      //
      // lookup-tables to encode DC bits
      std::vector<uint64_t> const &dcBitsLookup1(m_dcBitsLookup1[layer]);
      std::vector<uint64_t> const &dcBitsLookup2(m_dcBitsLookup2[layer]);
      //
      // loop over all SSIDs in this layer (in TSP space)
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         //
         // class to loop over all sectors
         class DCBitExtractor {
         public:
            inline DCBitExtractor
            (int layer,std::pair<int,int> const &dcSSIDxy,int sector,
             MAP<int,MAP<int,int> > const &dcIndexTable,int dcHbBitsLayer,
             std::vector<uint64_t> const &dcBitsLookup1,
             std::vector<uint64_t> const &dcBitsLookup2,
             PatternBank &bank) : m_layer(layer),m_bank(bank) {
               //
               // get encoded SSID information
               m_ssidIndex=-1;
               MAP<int,MAP<int,int> >::const_iterator dcIndexThisSector=
                  dcIndexTable.find(sector);
               if(dcIndexThisSector!=dcIndexTable.end()) {
                  MAP<int,int>::const_iterator ssidIndexPtr=
                     (*dcIndexThisSector).second.find(dcSSIDxy.first);
                  if(ssidIndexPtr!=(*dcIndexThisSector).second.end()) {
                     //
                     // m_ssidIndex is a (small) number which identifies
                     //  the (TSP-)SSID, given (layer,sector)
                     //
                     // it is stored together with the ternary bit information
                     // m_ssidIndex is shifted by dcHbBitsLayer
                     //  to leave space for the ternary bit inflrmation
                     m_ssidIndex=(*ssidIndexPtr).second << dcHbBitsLayer;
                  }
               }
               m_dcHbInitial=dcBitsLookup1[dcSSIDxy.second];
               m_dcHbMask=(1<<dcHbBitsLayer)-1;
               m_dcBitsLookup=dcBitsLookup2.data()
                  +m_dcHbInitial*(1<<dcHbBitsLayer);
               
            }
            inline int getSSIDindex(void) const { return m_ssidIndex; }
            inline void initialize(int firstPattern) {
               m_pattern=firstPattern;
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
               uint32_t ssidWithDC=m_ssidIndex;
               if(m_bank.m_pattern8Data[m_layer].size()) {
                 dcOld=m_bank.m_pattern8Data[m_layer][m_pattern];
               } else {
                 dcOld=m_bank.m_pattern16Data[m_layer][m_pattern];
               }
               // mask out non-DC bits
               dcOld &= m_dcHbMask;
               if(!dcOld) {
                  //
                  // first-time this patttern is set
                  //    use initial DC information 
                  ssidWithDC |= m_dcHbInitial;
               } else {
                  //
                  // store OR of DC information from the new TSP-SSID
                  // and the old DC bits
                  ssidWithDC |= m_dcBitsLookup[dcOld];
               }
               // 
               // store updated pattern information
               if(m_bank.m_pattern8Data[m_layer].size()) {
                  m_bank.m_pattern8Data[m_layer][m_pattern]=ssidWithDC;
               } else {
                  m_bank.m_pattern16Data[m_layer][m_pattern]=ssidWithDC;
               }
            }
            inline void update(int delta) {
               m_pattern+=delta;
            }
         protected:
            uint32_t m_ssidIndex;
            uint32_t m_dcHbInitial;
            uint32_t m_dcHbMask;
            int m_layer;
            const uint64_t *m_dcBitsLookup;
            int m_pattern;
            PatternBank &m_bank;
         };
         //
         // loop over all sectors with multiple patterns
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            //
            // get DC-level SSID
            //    dcSSIDxy.first  : SSID in DC-space
            //    dcSSIDxy.second : xy coordinate of TSP relative to DC
            //
            std::pair<int,int> dcSSIDxy=
               getDCssid(layer,(*sector).first,(*ssid).first);

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
   }
   // count number of encoded patterns
   uint64_t nTSPpatterns=0;
   std::vector<std::vector<int> > dcMask(dcSSIDBySectorLayer.size());
   for(unsigned layer=0;layer<dcSSIDBySectorLayer.size();layer++) {
      int nBitsX=m_TSPmap->getNBits(layer,0);
      int nBitsY=0;
      if(m_TSPmap->getDim(layer)==2) {
         nBitsY=m_TSPmap->getNBits(layer,1);
      }
      for(int i=0;i<nBitsX;i++) {
         dcMask[layer].push_back(1<<(i+nBitsX));
      }
      for(int i=0;i<nBitsY;i++) {
         dcMask[layer].push_back(1<<(i+nBitsY+2*nBitsX));
      }
   }
   for(int iPattern=0;iPattern<m_npatterns;iPattern++) {
      int nTSP=1;
      for(unsigned layer=0;layer<dcMask.size();layer++) {
         int ssidWithDC;
         if(m_bank.m_pattern8Data[layer].size()) {
            ssidWithDC=m_bank.m_pattern8Data[layer][iPattern];
         } else {
            ssidWithDC=m_bank.m_pattern16Data[layer][iPattern];
         }
         for(unsigned i=0;i<dcMask[layer].size();i++) {
            if(!(ssidWithDC & dcMask[layer][i])) nTSP<<=1;
         }
      }
      nTSPpatterns += nTSP;
   }

   double patternTSPcountTotal=0.;
   for(unsigned i=0;i<patternTSPcount.size();i++)
      patternTSPcountTotal+= patternTSPcount[i];

   Info(where)
      <<"number of distinct (layer,SSID)="<<ssidCountTotal
      <<" number of distinct (layer,SSID,sector)="<<sectorCountTotal
      <<" number of patterns="<<m_npatterns
      <<" (0x"<<std::setbase(16)<<m_npatterns<<std::setbase(10)<<")\n";
   Info(where)
      <<"number of TSP-SSIDs per pattern="
      <<(m_npatterns ? (patternTSPcountTotal/(double)m_npatterns) : 0)<<"\n";
   Info(where)
      <<"number of encoded TSP-patterns per DC-pattern="
      <<(m_npatterns ? (nTSPpatterns/(double)m_npatterns) : 0)<<"\n";
   Info(where)
      <<"average number of DC-patterns per sector="
      <<((sectorCountTotal > 0) ? m_npatterns/(double)sectorCountTotal : -1)
      <<" min="<<minPattern<<" max="<<maxPattern<<"\n";
   for(unsigned i=0;i<patternTSPcount.size();i++) {
      Info(where)
         <<"plane="<<i<<" number of TSP/pattern="
         << ((m_npatterns > 0 ) ? (patternTSPcount[i]/(double)m_npatterns)
             : - 1) <<"\n";

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
      (double)(m_npatterns*getNPlanes())
      <<" patternID="<<memoryPatternIDlookup/(double)
      (m_npatterns*getNPlanes())
      <<" total="<<(memorySSIDlookup+memoryPatternIDlookup)/
      (double)(m_npatterns*getNPlanes())<<"\n";


   int error=0;

#ifdef CONSISTENCY_CHECK
   // corruption test of SSIDs
   // loop over all planes and SSIDS
   // check whether the SSID correponds to a valid module number
   int nPrint=100;
   for(unsigned iPlane=0;iPlane<dcSSIDBySectorLayer.size();iPlane++) {
      PatternBySectorSSidMap_t const &planeData=
         m_bank.m_PatternByLayer[iPlane].m_SSidData;
      for(PatternBySectorSSidMap_t::const_iterator
             iSSid=planeData.begin();iSSid!=planeData.end();++iSSid) {
         if(((*iSSid).first!=s_WILDCARDid)&&((*iSSid).first!=s_INVALIDid)) {
            int moduleId=-1;
            getDCssidSlow(iPlane,
                          (*(*iSSid).second.begin()).first,
                          (*iSSid).first,&moduleId);
            int hash=-1;
            if(moduleId>=0) {
               if (getHWModeSS_tsp()==0) {
                  hash=moduleId;
               } else {
                  hash=getSSMapTSP()->getRegionMap()
                     ->getGlobalId(getBankID(),iPlane,moduleId);
               }
            }
            if(hash<0) {
               error++;
               if(nPrint) {
                  std::cout<<"invalid SSID="<<(*iSSid).first
                           <<" (bad module ID) plane="<<iPlane<<"\n";
                  nPrint--;
               }
            }
         }
      }
   }
   if(error) {
      Fatal(where)<<"invalid SSIDs ("<<error<<")in pattern bank\n";
   }

   // corruption test of sector structures
   // loop over all planes and all SSIDs
   // construct table of module IDs for each sector
   // verify that there is only one well-defined module per sector and plane
   error=0;
   for(unsigned iPlane=0;iPlane<dcSSIDBySectorLayer.size();iPlane++) {
      VECTOR<std::set<int> > modulesBySector(maxSector);
      PatternBySectorSSidMap_t const &planeData=
         m_bank.m_PatternByLayer[iPlane].m_SSidData;
      for(PatternBySectorSSidMap_t::const_iterator
             iSSid=planeData.begin();iSSid!=planeData.end();++iSSid) {
         for(PatternBySector_t::const_iterator
                iSector=(*iSSid).second.begin();
             iSector!=(*iSSid).second.end();++iSector) {
            int moduleId=-1;
            if(((*iSSid).first!=s_WILDCARDid)&&(*iSSid).first!=s_INVALIDid) {
               getDCssidSlow(iPlane,(*iSector).first,(*iSSid).first,&moduleId);
            }
            modulesBySector[(*iSector).first].insert(moduleId);
         }
      }
      std::map<int,int> multiplicity;
      for(size_t sector=0;sector<modulesBySector.size();sector++) {
         multiplicity[modulesBySector[sector].size()]++;
      }
      for(std::map<int,int>::const_iterator imult=multiplicity.begin();
          imult!=multiplicity.end();imult++) {
         if((*imult).first>1) {
            error++;
            std::cout<<"plane="<<iPlane<<" some sectors ("<<(*imult).second
                     <<") have "<<(*imult).first<<" modules assigned (error)\n";
         }
      }
      // print all bad sectors
      for(size_t sector=0;sector<modulesBySector.size();sector++) {
         if(modulesBySector[sector].size()>1) {
            std::cout<<"bad plane="<<iPlane<<" sector="<<sector<<" [";
            for(std::set<int>::const_iterator k=modulesBySector[sector].begin();
                k!=modulesBySector[sector].end();k++) {
               std::cout<<" "<<(*k);
            }
            std::cout<<"]\n";
         }
      }
   }
   if(error) {
      Fatal(where)
         <<"Multiple modules are assigned to a given plane and sector\n";
   }
#endif
   setupSectorWildcards();
}

/**
   readWildcards()

   read wildcards from predefined sources
*/
void FTK_CompressedAMBank::readWildcards(void) {
   importBadModuleASCII(getBadSSMapPath(),s_WILDCARDid);
   importBadModuleASCII(getBadSSMapPath2(),s_INVALIDid);
   invalidateSSIDtables();
}

/**
   importBadModuleASCII()

   read list of bad modules from an ACSII file

   type==s_WILDCARDid
      try to apply wildcards for these modules

   type==s_INVALIDid
      do not apply wildcards, simply drop the modules

   modifies:
      m_badModules
*/

void FTK_CompressedAMBank::importBadModuleASCII
(std::string const &name,int type) {
   int changed=0;
   Info("importBadModuleASCII")<<"reading "<<name<<" type="<<type<<"\n";
   if(name.empty()) return;
   std::ifstream wildcardFile(name.c_str());
   if(!wildcardFile.is_open()) {
      Warning("importBadModuleASCII")<<"failed to open file "<<name<<"\n";
      return;
   }
   std::string line;
   while( getline(wildcardFile,line) ) {
      std::istringstream lineRead(line);
      int tmpisPixel,tmpBEC,tmpSector,tmpPlane,tmpEtaModule,tmpPhiModule,tmpSection,tmpidhash;
      lineRead >>tmpisPixel >>tmpBEC >>tmpSector >>tmpPlane >>tmpEtaModule >>tmpPhiModule >>tmpSection;
      if(lineRead.fail()) {
         Error("importBadModuleASCII")
            <<"problem to read wildcard data from \""<<line<<"\"\n";
         continue;
      }
      lineRead>>tmpidhash;
      if(lineRead.fail() && (getHWModeSS_tsp()==2)) {
         Error("importBadModuleASCII")
            <<"HWMODEID==2 but module hash code missing \""<<line<<"\"\n";
         continue;
      }
      int id=-1;
      if(getHWModeSS_tsp()==0) {
         static FTKHit hit;
         hit.setSector(tmpSector);
         int phimod=hit.getPhiModule();
         int section=hit.getSection();
         int etacode;
         if((m_TSPmap->getDim(tmpPlane)==2)&& !hit.getIsBarrel()) {
            etacode=14+3*(hit.getASide() ? 1 : 0)+section;
         } else {
            etacode=hit.getEtaCode();
         }
         if(m_TSPmap->getDim(tmpPlane)==2) {
            id=getSSMapTSP()->getSSxy
               (tmpPlane,section,phimod,etacode,0,0);
         } else {
            id=getSSMapTSP()->getSSx
               (tmpPlane,section,phimod,etacode,0);
         }
      } else {
         id=getSSMapTSP()->getRegionMap()
            ->getLocalId(getBankID(),tmpPlane,tmpidhash);
      }
      if(id!=-1) {
         if(m_badModules.size()<=(size_t)tmpPlane)
            m_badModules.resize(tmpPlane+1);
         // old type
         // can be:
         //   0 (adding a new module) 
         //   s_WILDCARDid bad module already added and wildcard is requested
         //   s_INVALIDid bad module already added and wildcard is forbidden
         int &typePtr= m_badModules[tmpPlane][id];
         int oldId=typePtr;
         // if type is IGRNORE, it can not be changed
         // (assuming that the wildcard can not be set for good reasons)
         if(oldId!=s_INVALIDid) {
            typePtr=type;
         }
         if(typePtr != oldId) {
            changed++;
            if(typePtr==s_INVALIDid) {
               Info("importBadModuleASCII")
                  <<"bad module added (IGNORE) plane="<<tmpPlane
                  <<" tower="<<getBankID()
                  <<" id="<<id<<"\n";
            } else if(typePtr==s_WILDCARDid) {
               Info("importBadModuleASCII")
                  <<"bad module added (try WILDCARD) plane="<<tmpPlane
                  <<" tower="<<getBankID()
                  <<" id="<<id<<"\n";
            } else {
               Error("importBadModuleASCII")
                  <<"bad module added (unknown type) plane="<<tmpPlane
                  <<" tower="<<getBankID()
                  <<" id="<<id<<" type="<<typePtr<<"\n";
            }
         }
      }
   }
}

/**
    FTK_CompressedAMBank::importBadModuleTTree()
    read list of bad modules from root tree
    store them to:
      m_badModules
*/

void FTK_CompressedAMBank::importBadModuleTTree
(TDirectory *dir,char const *name) {
   TTree *badModuleTree;
   dir->GetObject(name,badModuleTree);
   m_badModules.resize(getNPlanes());
   if(badModuleTree) {
      int32_t plane,moduleID,type;
      TBranch *planeBranch=badModuleTree->GetBranch("plane");
      TBranch *idBranch=badModuleTree->GetBranch("id");
      TBranch *typeBranch=badModuleTree->GetBranch("type");
      if(planeBranch && idBranch && typeBranch) {
         planeBranch->SetAddress(&plane);
         idBranch->SetAddress(&moduleID);
         typeBranch->SetAddress(&type);
         for(int i=0;i<badModuleTree->GetEntriesFast();i++) {
            badModuleTree->GetEntry(i);
            if((plane>=0)&&(plane<getNPlanes())) {
               m_badModules[plane][moduleID]=type;
            }
         }
      } else {
         Error("importBadModuleTTree")
            <<"bad modules tree required branches: plane id type\n";
      }
   }
}

/**
    FTK_CompressedAMBank::exportBadModuleTTree()
    write list of bad modules to root tree
*/
void FTK_CompressedAMBank::exportBadModuleTTree
(TDirectory *dir,char const *name) const {
   TDirectory *back=gDirectory;
   dir->cd();
   TTree *badModuleTree=new TTree
      ("BadModules",TString::Format(name,getHWModeSS_tsp()));
   int32_t plane,moduleID,type;
   badModuleTree->Branch("plane",&plane,"plane/I");
   badModuleTree->Branch("id",&moduleID,"id/I");
   badModuleTree->Branch("type",&type,"type/I");
   for(plane=0;plane<getNPlanes();plane++) {
      for(MAP<int,int>::const_iterator m=m_badModules[plane].begin();
          m!=m_badModules[plane].end();m++) {
         moduleID=(*m).first;
         type=(*m).second;
         badModuleTree->Fill();
      }
   }
   badModuleTree->Write("", TObject::kOverwrite);
   back->cd();
}

/**
   FTK_CompressedAMBank::setupModuleWildcards()

   ensure that the bank data, the list of bad modules
   and the sector-wildcards are coherent

   input:
     m_bank
        the bank may contain SSIDs == s_WILDCARDid
        ->
     m_badModules

   output:
     m_sectorWC
        this array gives for each sector a bit-map of wildcards
        it can be used to initialize the hitlist of the sector
 */

void FTK_CompressedAMBank::setupSectorWildcards(void) {
   int maxSector=0;
   if(m_SectorFirstLastPattern.rbegin()!=m_SectorFirstLastPattern.rend()) 
      maxSector=(*m_SectorFirstLastPattern.rbegin()).first+1;
   m_SectorWC.resize(0);
   m_SectorWC.resize(maxSector);
   VECTOR<HitPattern_t> sectorBadList(maxSector);
   
   // store wildcards in sector table and in list of bad SSIDs
   for(int iPlane=0;iPlane<getNPlanes();iPlane++) {
      if(m_badModules[iPlane].size()) {
         Info("setupSectorWildcards")
            <<"plane="<<iPlane<<" has "
            <<m_badModules[iPlane].size()<<" dead modules\n";
      }
      // list of all sectors, with flags
      //   flag==1 : no bad module
      //   flag==2 : bad module, request wildcard
      //   flag==4 : bad module, no wildcard
      VECTOR<int> sectorFlag(maxSector);
      
      //for(std::set<int>::const_iterator wc=wildcards[iPlane].begin();
      //    wc!=wildcards[iPlane].end();wc++) {
      //}
      PatternBySectorSSidMap_t const &planeData=
         m_bank.m_PatternByLayer[iPlane].m_SSidData;
      for(PatternBySectorSSidMap_t::const_iterator iSSid=planeData.begin();
          iSSid!=planeData.end();++iSSid) {
         int mask=1;
         int moduleId=-1;
         if((*iSSid).first==s_WILDCARDid) {
            // wildcard inside bank
            mask=2;
         } else if((*iSSid).first==s_INVALIDid) {
            // ignore inside bank
            mask=4;
         } else {
            PatternBySector_t::const_iterator iSector=
               (*iSSid).second.begin();
            getDCssidSlow(iPlane,(*iSector).first,(*iSSid).first,&moduleId);
            MAP<int,int>::const_iterator im=
               m_badModules[iPlane].find(moduleId);
            if(im!=m_badModules[iPlane].end()) {
               if((*im).second==s_WILDCARDid) {
                  mask=2;
               } else if((*im).second==s_INVALIDid) {
                  mask=4;
               }
            }
         }
         for(PatternBySector_t::const_iterator
                iSector=(*iSSid).second.begin();
             iSector!=(*iSSid).second.end();++iSector) {
            sectorFlag[(*iSector).first]|=mask;
            /*if((*iSector).first==1273) {
               std::cout<<" SSID="<<(*iSSid).first
                        <<" plane="<<iPlane
                        <<" module="<<moduleId
                        <<" mask="<<mask<<"\n";
                        } */
         }
      }
      for(int sector=0;sector<maxSector;sector++) {
         int type=sectorFlag[sector];
         if(type==0) continue; // no data for this sector
         if(type==1) continue;  // no dead module
         if((type==2)||(type==4)) {
            sectorBadList[sector] |= 1<<iPlane;
            if(type==2) {
               m_SectorWC[sector] |= 1<<iPlane;
            }
         } else {
            bool firstPrint=true;
            for(PatternBySectorSSidMap_t::const_iterator
                   iSSid=planeData.begin();iSSid!=planeData.end();++iSSid) {
               int moduleId=-1;
               bool doPrint=false;
               for(PatternBySector_t::const_iterator
                      iSector=(*iSSid).second.begin();
                   iSector!=(*iSSid).second.end();++iSector) {
                  if((*iSector).first==sector) doPrint=true;
               }
               if(doPrint) {
                  if(firstPrint) {
                     for(size_t i=0;i<m_bank.m_PatternByLayer.size();i++) {
                        std::cout<<"plane="<<i<<" nSSID="
                                 <<m_bank.m_PatternByLayer[i].m_SSidData.size()
                                 <<"\n";
                     }
                     firstPrint=false;
                  }
                  if(((*iSSid).first!=s_WILDCARDid)&&
                     ((*iSSid).first!=s_INVALIDid)) {
                     getDCssidSlow(iPlane,(*(*iSSid).second.begin()).first,(*iSSid).first,&moduleId);
                  }
                  int hash=-2;
                  if(moduleId>=0) {
                     hash=getSSMapTSP()->getRegionMap()
                        ->getGlobalId(getBankID(),iPlane,moduleId);

                  }
                  std::cout<<" SSID="<<(*iSSid).first
                           <<" plane="<<iPlane
                           <<" module="<<moduleId
                           <<" hash="<<hash
                           <<" [";
                  for(PatternBySector_t::const_iterator
                         iSector=(*iSSid).second.begin();
                      iSector!=(*iSSid).second.end();++iSector) {
                     std::cout<<" "<<(*iSector).first;
                  }
                  std::cout<<"]\n";
               }
            }
            Error("setupSectorWildcards")
               <<"inconsistent wildcards sector="<<sector
               <<" plane="<<iPlane<<" type="<<type<<"\n";
         }
      }
   }

   // filter wildcards according to Alberto's prescription
   //   (email received 16/02/25)
   int n00=0,n10=0,n11=0,n21=0,nSilent=0;
   for(unsigned sectorID=0;sectorID<m_SectorWC.size();sectorID++) {
      int nDeadPlane=m_nHit16[sectorBadList[sectorID]];
      int nWC=m_nHit16[m_SectorWC[sectorID]];
      if(nDeadPlane<2) {
         // zero or one dead plane: keep wildcard
         //   -> 7/8 or 6/7 hits required
         if(nDeadPlane==1) {
            if(nWC==1) n11++;
            else n10++;
         } else n00++;
      } else if(nDeadPlane==2) {
         n21++;
         // two dead planes, possibly remove wildcard for one of them
         //  -> 6/6 hits required
         for(int mask=(1<<(getNPlanes()-1));mask;mask>>=1) {
            // if there are less than 2 wildcards, stop
            if(m_nHit16[m_SectorWC[sectorID]]<=1) break;
            m_SectorWC[sectorID] &= ~mask;
         }
      } else {
         nSilent++;
         // more than two dead planes, remove all wildcards
         //  -> sector is made silent
         m_SectorWC[sectorID]=0;
      }
   }

   // print a summary
   int nSectorWithDeadModule=0;
   VECTOR<int> nDeadPlanes(getNPlanes()+1);
   // count number of sectors with dead modules and with wildcard
   for(unsigned sectorID=0;sectorID<m_SectorWC.size();sectorID++) {
      if(sectorBadList[sectorID]) nSectorWithDeadModule++;
      nDeadPlanes[m_nHit16[sectorBadList[sectorID]]]++;
   }
   int np=getNPlanes();
   m_MAX_MISSING_PLANES = FTKSetup::getFTKSetup().getMaxMissingPlanes();
   Info("setupSectorWildcards")
      <<"number of sectors with dead module(s): "<<nSectorWithDeadModule<<"\n";
   Info("setupSectorWildcards")
      <<"sectors with ("<<(np-m_MAX_MISSING_PLANES)<<"/"<<np<<"): "<<n00<<"\n";
   if(n10) {
      Info("setupSectorWildcards")
         <<"sectors with ("<<(np-m_MAX_MISSING_PLANES)<<"/"<<(np-1)<<"): "<<n10<<"\n";
   }
   if(n11) {
      Info("setupSectorWildcards")
         <<"sectors with ("<<(np-1-m_MAX_MISSING_PLANES)<<"/"<<(np-1)<<"): "<<n11<<"\n";
   }
   if(n21) {
      Info("setupSectorWildcards")
         <<"sectors with ("<<(np-1-m_MAX_MISSING_PLANES)<<"/"<<(np-2)<<"): "<<n21<<"\n";
   }
   if(nSilent) {
      Info("setupSectorWildcards")
         <<"disabled sectors: "<<nSilent<<"\n";
   }
   for(int i=0;i<=getNPlanes();i++) {
      if(!nDeadPlanes[i]) continue;
      if(i<2) {
         Info("setupSectorWildcards")
            <<"sectors with "<<i<<" dead module(s): "<<nDeadPlanes[i]<<"\n";
      } else {
         Warning("setupSectorWildcards")
            <<"sectors with "<<i<<" dead module(s): "<<nDeadPlanes[i]<<"\n";
      }
   }
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
               Info("compare")
                  <<"ssid number mismatch (layer="<<layer<<") "
                  <<(*ssid1).first<<"!="<<(*ssid2).first<<"\n";
            }
            error |= 0x0010;
            continue;
         }
         
         MAP<int,VECTOR<uint32_t> > patternData1,patternData2;

         class PatternExtractor {
         public:
            inline PatternExtractor(VECTOR<uint32_t> &data) : m_data(data),m_pattern(0){ }
            inline void initialize(uint32_t first) { m_pattern=first; }
            inline void process(void) {
               m_data.push_back(m_pattern);
            }
            inline void update(uint32_t delta) {
               m_pattern+=delta;
            }
         protected:
            VECTOR<uint32_t> &m_data;
            uint32_t m_pattern;
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
                  Info("compare")
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
               Info("compare")
                  <<"bank data mismatch (layer="<<layer
                  <<" ssid="<<(*ssid1).first<<" sector="<<(*sector1).first
                  <<") error=0x"<<std::setbase(16)<<(error^errOld)
                  <<std::setbase(10)<<"\n";
            }
         }
         if(((error & 0x0200)==0) && ((sector1!=patternData1.end())||
                                      (sector2!=patternData2.end()))) {
            Info("compare")
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
            Info("compare")
               <<"different number of ssids (layer="<<layer
               <<") "<<thisLayerData.m_SSidData.size()
               <<"!="<<otherLayerData.m_SSidData.size()
               <<"\n";
            error |= 0x0020;
         }
   }
   if((m_bank.m_PatternByLayer.size()!=bank->m_bank.m_PatternByLayer.size())) {
      Info("compare")
         <<"different number of layers "<<m_bank.m_PatternByLayer.size()
         <<"!="<<bank->m_bank.m_PatternByLayer.size()<<"\n";
      error |= 0x0001;
   }
   if(error) {
      Warning("compare")<<"banks do not agree error="<<std::setbase(16)
                        <<error<<std::setbase(10)<<"\n";
   } else {
      Debug("compare")<<"bank comparison OK\n";
   }
   return error;
}

/**
   readCCachedBank()

   read bank data from root file
   the root format is the default file format to store the compressed bank

   The data are stored in TTree objects with primitive data types (Int_t)
   in all branches

   see writeRootFile() for further details
*/
int FTK_CompressedAMBank::readCCachedBank(TDirectory *inputDir) {
   Info("readCCachedBank")<<inputDir->GetName()<<"\n";
   erase();
   int error=0;
   TIter next(inputDir->GetListOfKeys());
   TObject *o;
   int maxLayer=-1;
   while((o=next())) {
      TString name=((TObjString *)o)->GetString();
      int layer=TString(name(TRegexp("[0-9]+"))).Atoi();
      if(name.BeginsWith("Layer")) {
         if(layer>maxLayer) maxLayer=layer;
      }
   }
   if(maxLayer<0) return -1;
   m_bank.m_PatternByLayer.resize(maxLayer+1);
   setNPlanes(maxLayer+1);
   next=inputDir->GetListOfKeys();
   while((o=next())) {
      TString name=((TObjString *)o)->GetString();
      int layer=TString(name(TRegexp("[0-9]+"))).Atoi();
      if(layer<0) {
         continue;
      }
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      if(name.BeginsWith("Layer")) {
         TTree *tree;
         inputDir->GetObject(name,tree);
         if(tree) {
            int nSector=tree->GetEntries();
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
               tree->GetEntry(i);
               if(nDataVar>dataSize) dataSize=nDataVar;
               sectorSize[sectorVar]+=nDataVar;
            }
            int maxSector=(*sectorSize.rbegin()).first+1;
            VECTOR<int> sectorOffset(maxSector);
            layerData.m_CompressedDeltaBySector.resize(maxSector);
            for(MAP<int,int>::const_iterator isector=sectorSize.begin();
                isector!=sectorSize.end();isector++) {
               layerData.m_CompressedDeltaBySector[(*isector).first].resize
                  ((*isector).second);
            }
            std::vector<Int_t> data(dataSize);
            tree->SetBranchAddress("data",data.data());
            tree->SetBranchStatus("data");
            tree->SetBranchStatus("ssid");
            tree->SetBranchStatus("firstPattern");
            tree->SetBranchStatus("nPattern");
            Info("readCCachedBank")<<"nSector="<<nSector
                                   <<" buffer size="<<data.size()<<"\n";
            for(int i=0;i<nSector;i++) {
               tree->GetEntry(i);
               SectorData &sectordata=layerData.m_SSidData[ssidVar][sectorVar];
               sectordata.m_FirstPattern=firstPatternVar;
               sectordata.m_NPattern=nPatternVar;
               sectordata.m_offset=sectorOffset[sectorVar];
               sectordata.m_length=nDataVar;
               uint8_t * __restrict dest=
                  layerData.m_CompressedDeltaBySector[sectorVar].data()+
                  sectordata.m_offset;
               for(int i=0;i<nDataVar;i++) {
                  (*dest++)=data[i];
               }
               sectorOffset[sectorVar]+=nDataVar;
            }
         } else {
            Error("readCCachedBank")<<"TObject "<<name<<" is not a TTree\n";
            error++;
         }
      }
   }
   importBadModuleTTree(inputDir);
   readBankPostprocessing("readCCachedBank");
   return error;
}

/**
   writeCCachedBankFile()

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
int FTK_CompressedAMBank::writeCCachedBankFile(char const *filename,int flatFormat) const {
   int error;
   if(flatFormat==0) {
      TDirectory *out=FTKRootFile::Instance()->CreateRootFile(filename);
      if(out) {
         error=writeCCachedBankFile(out);
         delete out;
      } else {
         error=1;
         Error("writeCCachedBankFile")
            <<" could not open \""<<filename<<"\" for writing\n";
      }
   } else {
      error=writeBANKjson(filename);
   }
   return error;
}

int FTK_CompressedAMBank::writeCCachedBankFile(TDirectory *out) const {
   int error=0;
   TDirectory *back=gDirectory;
   out->cd();
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
      /* Info("writeCCachedBankFile")
         <<"layer="<<layer
         <<" maxDeltaSize="<<maxDataSize<<"\n"; */
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
            uint8_t const *  __restrict compressedDeltaData=layerData.
               m_CompressedDeltaBySector[sectorVar].data()+
               (*sector).second.m_offset;
            for(int i=0;i<nDataVar;i++)
               data[i]=compressedDeltaData[i];
            ssidTree->Fill();
         }
      }
      ssidTree->Write("", TObject::kOverwrite);      
      Info("writeCCachedBankFile")
         <<"layer="<<layer
         <<" nSector="<<ssidTree->GetEntriesFast()<<" written\n";
   }
   // wildcards
   exportBadModuleTTree(out);
   back->cd();
   return error;
}

void  FTK_CompressedAMBank::SplitlistHUF
(uint64_t code,int *i2char,int *integral,int i0,int i1,
 VECTOR<int> &huftable,VECTOR<uint64_t> &hufcode) const {
   if(i0==i1) {
      huftable.push_back(0);
      huftable.push_back(i2char[i0]);
      hufcode[i2char[i0]]=code;
   } else {
      int thresh=(integral[i0]+integral[i1])/2;
      int j0=i0;
      int j1=i1;
      while(j0<j1) {
         int j2=(j0+j1)/2;
         if(integral[j2]<=thresh) {
            j0=j2+1;
         } else {
            j1=j2;
         }
      }
      int hufindex=huftable.size();
      huftable.push_back(0);
      SplitlistHUF((code<<1)+1,i2char,integral,j0,i1,huftable,hufcode);
      huftable[hufindex]=huftable.size()-hufindex;
      SplitlistHUF((code<<1),i2char,integral,i0,j0-1,huftable,hufcode);
   }
}

int FTK_CompressedAMBank::writeBANKjson(char const *jsonFile) const {
   int error=0;
   std::ofstream out(jsonFile);
   MAP< int,VECTOR< MAP<int,MAP<int,SectorData const *> > > > tspBySectorDC;

#ifdef JSON_HUF
   // set up tables for huf encoding
   VECTOR<VECTOR<int> > multiplicity(m_bank.m_PatternByLayer.size());
   VECTOR<VECTOR<int> > huftable(m_bank.m_PatternByLayer.size());
   VECTOR<VECTOR<int> > hufcodeSize(m_bank.m_PatternByLayer.size());
   VECTOR<VECTOR<uint64_t> > hufcode(m_bank.m_PatternByLayer.size());
#endif
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
#ifdef JSON_HUF
      multiplicity[layer].resize(256);
#endif
      LayerData const &layerData=m_bank.m_PatternByLayer[layer];
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         for(PatternBySector_t::const_ptr sector=(*ssid).second.beginPtr();
             sector!=(*ssid).second.endPtr();++sector) {
            VECTOR< MAP<int,MAP<int,SectorData const *> > >
               &layerData=tspBySectorDC[(*sector).first];
            if(!layerData.size()) layerData.resize(getNPlanes());
            int dc=getDCssidConst(layer,(*ssid).first);
            SectorData const *sd=&(*sector).second;
            layerData[layer][dc][(*ssid).first]=sd;
#ifdef JSON_HUF
            uint8_t const *ptr=m_bank.m_PatternByLayer[layer]
               .m_CompressedDeltaBySector[(*sector).first].data()+sd->m_offset;
            for(int i=sd->m_length;i;i--) {
               multiplicity[layer][*(ptr++)]++;
            }
#endif
         }
      }
#ifdef JSON_HUF
      MAP<int64_t,int> ordered;
      for(int i=0;i<256;i++) {
         if(multiplicity[layer][i]) {
            ordered[(multiplicity[layer][i]<<8)+i]=i;
         }
      }
      Info("writeBANKjson")<<"layer "<<layer
                           <<" HUF tree number of characters "
                           <<ordered.size()<<"\n";
      int sum=0,ii=0;
      int integral[256],i2char[256];
      for(MAP<int64_t,int>::const_iterator i=ordered.begin();
          i!=ordered.end();i++) {
         sum += (*i).first>>8;
         integral[ii]=sum;
         i2char[ii++]=(*i).second;
      }
      hufcode[layer].resize(256);
      hufcodeSize[layer].resize(256);
      SplitlistHUF(1,i2char,integral,0,ordered.size()-1,huftable[layer],
                   hufcode[layer]);
      for(int i=0;i<256;i++) {
         int b;
         for(b=31;b;b--) {
            if(hufcode[layer][i] & (1<<b)) break;
         }
         hufcodeSize[layer][i]=b;
      }
#endif
   }
   

   Info("writeBANKjson")<<"output file=\""<<jsonFile<<"\"\n";
#ifdef JSON_HUF
   out<<"{\"huftables\":[\n";
   for(int l=0;l<huftable.size();l++) {
      if(l)out<<",\n";
      out<<"{\"layer\":"<<l<<",\"huf\":[\n";
      for(int i=0;i<huftable[l].size();i++) {
         if(i) out<<",";
         out<<huftable[l][i];
      }
      out<<"\n]}\n";
   }
   out<<"],";
#else
   out<<"{";
#endif
   out<<"\"patterns\":[";
   for( MAP< int,VECTOR< MAP<int, MAP<int,SectorData const *> > > >
           ::const_iterator
           iSector=tspBySectorDC.begin();iSector!=tspBySectorDC.end();
        iSector++) {
      if(iSector!=tspBySectorDC.begin()) out<<",\n";
      int sector=(*iSector).first;
      std::pair<int,int> firstLast=
         (*m_SectorFirstLastPattern.find(sector)).second;
      out<<"{\"sector\":"<<sector
         <<",\"first\":"<<firstLast.first
         <<",\"last\":"<<firstLast.second
         <<",[\n";
      for(unsigned layer=0;layer<(*iSector).second.size();layer++) {
         MAP<int, MAP<int,SectorData const *> >
            const &ssidData((*iSector).second[layer]);
         if(layer) out<<",\n";
         out<<" {\"layer\":"<<layer<<",[\n";
         for(MAP<int, MAP<int,SectorData const *> >::const_iterator
                iDC=ssidData.begin();iDC!=ssidData.end();
             iDC++) {
            if(iDC!=ssidData.begin()) out<<",\n";
            out<<"  {\"DCssid\":"<<(*iDC).first<<",\"tspList\":[\n";
            for(MAP<int,SectorData const *>::const_iterator
                   iTSP=(*iDC).second.begin();
                iTSP!=(*iDC).second.end();iTSP++) {
               SectorData const *sd=(*iTSP).second;
               if(iTSP!=(*iDC).second.begin()) out<<",\n";
               out<<"   {\"TSPssid\":"<<(*iTSP).first
                  <<",\"first\":"<<sd->m_FirstPattern
                  <<",\"nPatt\":"<<sd->m_NPattern
                  <<",\"nData\":"<<sd->m_length
                  <<",\"data\":\"";
               uint8_t const *ptr=m_bank.m_PatternByLayer[layer]
                  .m_CompressedDeltaBySector[sector].data()+sd->m_offset;
#ifdef JSON_HUF
               char c=0;
               int ibit=5;
               for(int i=sd->m_length;i;i--) {
                  int data=*(ptr++);
                  uint64_t code=hufcode[layer][data];
                  for(int b=hufcodeSize[layer][data];b>=0;b--) {
                     if(code & (1LL<<b)) c|=(1<<ibit);
                     ibit--;
                     if(ibit<0) {
                        out<<(char)(c+'0');
                        c=0;
                        ibit=5;
                     }
                  }
               }
               if(ibit!=5) out<<(char)(c+'0');
#else
               for(int i=sd->m_length;i;i--) {
                  uint8_t data=*(ptr++)+0x33;
                  if((data>=23)&&(data<127)&&(data!='\\')) {
                     out<<(char)data;
                  } else {
                     out<<"\\h"<<std::setbase(16)<<std::setw(2)
                        <<(int)data<<std::setbase(10);
                  }
               }
#endif
               out<<"\"}";
            }
            out<<"]}";
         }
         out<<"\n ]}";
      }
      out<<"\n]}";
   }
   out<<"\n]\n}\n";
   return error;
}

void FTK_CompressedAMBank::insertPatterns
(int sector,FTKPatternOneSector const *patterns,
 int maxpatts,VECTOR<HitPatternMap_t> &dcPatterns,
 int &nDC,int &nTSP) {
   /* Info("insertPatterns") << "inserting patterns " << patterns
                          << " with Npatt=" << patterns->GetNumberOfPatterns()
                          << ", sector " << sector << " (" << patterns->GetSector() << ")"
                          << std::endl; */
   HitPatternMap_t &dcList=dcPatterns[sector];
   FTKPatternOneSectorOrdered *tspList=
      patterns->OrderPatterns(FTKPatternOrderByCoverage(0));
   unsigned nLayer=getNPlanes();
   // store a pattern in DC space
   FTKHitPattern dc(nLayer);
   for(FTKPatternOneSectorOrdered::Ptr_t itsp=tspList->Begin();
       itsp!=tspList->End();itsp++) {
      // original pattern in TSP space
      FTKHitPattern const &tsp(tspList->GetHitPattern(itsp));
      // this stores the coverage plus 2^32*number of TSP patterns
      uint64_t covCtr=tspList->GetCoverage(itsp) + 0x100000000LL;
      // this variables stores DC/HB bits of all layers
      uint64_t dchb0=0;
      //std::cout<<"Pattern\n";
      HitPattern_t badModules=0;
      HitPattern_t wildcardRequest=0;
      for(int ilayer=nLayer-1;ilayer>=0;ilayer--) {
         std::pair<int,int> const &dcSSID=
            getDCssid(ilayer,sector,tsp.GetHit(ilayer));
         int nDCbits=m_TSPmap->getNBits(ilayer);
         int nDCHBbits=nDCbits<<1; 
         /* std::cout<<" L"<<ilayer<<" tsp="<<tsp.GetHit(ilayer)
                  <<" DC="<<dcSSID.first<<":"<<dcSSID.second
                  <<" bits*2="<<nDCHBbits
                  <<"\n"; */
         dc.SetHit(ilayer,dcSSID.first);
         uint64_t dchbLayer;
         if(dcSSID.first==s_WILDCARDid) {
            badModules|=(1<<ilayer);
            wildcardRequest|=(1<<ilayer);
            dchbLayer=m_dcBitsLookup1[ilayer][0]; // fixed coordinate
         } else if(dcSSID.first==s_INVALIDid) {
            badModules|=(1<<ilayer);
            dchbLayer=m_dcBitsLookup1[ilayer][0]; // fixed coordinate
         } else {
            dchbLayer=m_dcBitsLookup1[ilayer][dcSSID.second];
         }
         dchb0 = (dchb0<<nDCHBbits) | dchbLayer;
      }
      // implement wildcard policy here
      if(badModules) {
         // if more than two bad modules, skip pattern
         if(m_nHit16[badModules]>2) continue; 
         // drop outermost wildcards
         for(int ilayer=nLayer-1;ilayer>=0;ilayer--) {
            if(m_nHit16[wildcardRequest]<=1) break;
            wildcardRequest &= ~ (1<<ilayer);
            dc.SetHit(ilayer,s_INVALIDid);
         }
      }

      // std::cout<<"dchb=0x"<<std::setbase(16)<<dchb<<std::setbase(10)<<"\n";
      std::pair<HitPatternMap_t::iterator,HitPatternMap_t::iterator> idc=
         dcList.equal_range(dc);
      HitPatternMap_t::iterator iPatt;
      for(iPatt=idc.first;iPatt!=idc.second;iPatt++) {
         uint64_t dchb1=dchb0; // this pattern's DCHB bits
         uint64_t dchb2=(*iPatt).second.first; // present DCHB bits
         uint64_t dchbMerged=0; // new DCHB bits
         int shift=0; // location of DCHB bits
         VECTOR<int> nDClayer(nLayer);
         for(unsigned ilayer=0;ilayer<nLayer;ilayer++) {
            int nDCbits=m_TSPmap->getNBits(ilayer);
            int nDCHBbits=nDCbits<<1;
            // bit mask to select HCHB bits
            int mask=(1<<nDCHBbits)-1;
            // xy1xy2 index to calculate OR of two DCHB values 
            //  in a given layer
            int xy1xy2=((dchb1 & mask)<<nDCHBbits)+(dchb2 & mask);
            uint64_t xyMerged=m_dcBitsLookup2[ilayer][xy1xy2];
            nDClayer[ilayer]+=m_ndcBitsLookup[ilayer][xyMerged];
            dchbMerged |= xyMerged<<shift;
            // shift to store merged DCHB bits of next layer
            shift += nDCHBbits;
            // remove data of this layer
            dchb1>>=nDCHBbits;
            dchb2>>=nDCHBbits;
         }
         bool accept=true;
         int nDCbits=0;
         for(unsigned ilayer=0;ilayer<nLayer;ilayer++) {
            if(dc.GetHit(ilayer)==s_WILDCARDid) {
               nDClayer[ilayer]+=m_wildcardPenalty;
               if(ilayer<m_wildcardPenaltyPlane.size()) {
                  nDClayer[ilayer]+=m_wildcardPenaltyPlane[ilayer];
               }
            }
            nDCbits+=nDClayer[ilayer];
            if((ilayer<m_nDCmaxPlane.size())&&
               (m_nDCmaxPlane[ilayer]>=0)&&
               (nDClayer[ilayer]>m_nDCmaxPlane[ilayer])) {
               accept=false;
            }
         }
         if((m_nDCmax>=0)&&(nDCbits>m_nDCmax)) accept=false;
         /* static int print=1000;
         if(print) {
            std::cout<<" "<<std::setbase(16)<<dchbMerged<<" :";
            for(unsigned ilayer=0;ilayer<nLayer;ilayer++) {
               std::cout<<" "<<nDClayer[ilayer];
            }
            std::cout<<" sum "<<nDCbits
                     <<" accept="<<accept
                     <<std::setbase(10)<<"\n";
            print--;
            } */
         if(accept) {
            // store this pattern with the existing DC pattern
            // by setting more bits to the ternary state X
            if(0 && badModules) {
               std::cout<<"Merging pattern: ";
               for(size_t iLayer=0;iLayer<nLayer;iLayer++) {
                  std::cout<<" "<<dc.GetHit(iLayer);
               }
               std::cout<<" DCHB "<<std::setbase(16)<<(*iPatt).second.first
                        <<" + "<<dchb0
                        <<" -> "<<dchbMerged
                        <<std::setbase(10)<<"\n";
            }
            (*iPatt).second.first=dchbMerged;
            (*iPatt).second.second += covCtr;
            break;
         }
      }
      if(iPatt==idc.second) {
         // pattern has not been stored., try to add new DC pattern
         if((maxpatts>0)&&(nDC>=maxpatts)) break;
         dcList.insert(std::make_pair(dc,std::make_pair(dchb0,covCtr))); // insert DC pattern with DCHB bits
         if(0 && badModules) {
         std::cout<<"Insert pattern: ";
         for(size_t iLayer=0;iLayer<nLayer;iLayer++) {
            std::cout<<" "<<dc.GetHit(iLayer);
         }
         std::cout<<" DCHB "<<std::setbase(16)<<dchb0<<std::setbase(10)<<"\n";
         }
         nDC++;
      }
      nTSP++;
      /* if(nTSP!=nDC) {
         std::cout<<"inconsistency nDC="<<nDC<<" nTSP="<<nTSP<<"\n";
         exit(0);
         } */
   }
   delete tspList;
}

/**
   readROOTBank()

   interface provided by base class
*/
int FTK_CompressedAMBank::readROOTBank(const char *name, int maxpatts) {
   return readSectorOrderedBank(name,maxpatts,1,0,1);
}



/**
   readSectorOrderedBank()

   read sector-ordered bank, convert to DC patterns
   only one subregion is selected, see
      FTKPatternBySectorReader::SelectSubregion()
*/
int FTK_CompressedAMBank::readSectorOrderedBank(const char *name, int maxpatts,
                                                int nSub,int numSectorMax,int nlamb) {
   // create partitions based on the number of subregions
   std::list<Partition> partitions;
   TDirectory *TSPfile=FTKRootFile::Instance()->OpenRootFileReadonly(name);
   if(TSPfile) {
      std::set<int> subregion;
      FTKPatternBySectorReader *TSPreader=
         FTKPatternBySectorReader::Factory(*TSPfile);
      TSPreader->CheckConsistency(getSSMapTSP(),getBankID(),getHWModeSS_tsp());
      for(int sector=TSPreader->GetFirstSector();sector>=0;
          sector=TSPreader->GetNextSector(sector)) {
         if((nSub>1)&&(sector%nSub!=getSubID())) continue;
         subregion.insert(sector);
      }
      delete TSPreader;
      int nSectorMax=-1;
      if(numSectorMax>0) {
         nSectorMax=nSub>0 ? numSectorMax/nSub : numSectorMax;
      }
      partitions.push_back(Partition(maxpatts,nSectorMax,subregion));
      delete TSPfile;
   } else {
      Error("readSectorOrderedBank")<<"failed to open \""<<name<<"\"\n";
   }
   return readPartitionedSectorOrderedBank(name,partitions,nlamb);
}

/**
   readSectorOrderedBank()

   read sector-ordered bank, convert to DC patterns
   the DC patterns are filled using the predefined partitions, each partition
   with a maximum number of patterns

   here, the partition list is read from an ASCII file

   if(nSub>0), split the list into subregions and read only one subregion
   otherwise, all partitions are read
*/

int FTK_CompressedAMBank::readSectorOrderedBank
   (const char *name,const char *partitionListName,int nSub,int numSectorMax,int nlamb) {
   std::list<Partition> partitionList;
   ftk_dcap::istream *partitionFile=ftk_dcap::open_for_read(partitionListName);
   if(!partitionFile) {
      Error("readSectorOrderedBank")
         <<"partition file \""<<partitionListName<<"\" can not be opened\n";
   }
   std::map<int,int> allSectors;
   std::vector<int> nPatternBySR(nSub>0 ? nSub : 1);
   for(int partition=0;!partitionFile->eof();partition++) {
      std::string id="";
      int iPart=-1,nPatt=-1,nSect=-1;
      (*partitionFile)>>id;
      if(partitionFile->fail()) break;
      (*partitionFile)>>iPart>>nPatt>>nSect;
      if((partitionFile->fail())||(id!="PARTITION")) {
         Error("readSectorOrderedBank")
            <<"failed to read partition header from file "
            <<partitionListName
            <<" ("<<id<<" "<<iPart<<" "<<nPatt<<" "<<nSect<<")\n";
         break;
      }
      if(iPart!=partition) {
         Warning("readSectorOrderedBank")
            <<"Partition numbers "<<iPart<<" not in sequence , expect "
            <<partition<<"\n";
      }
      if(nSect<=0) {
         Error("readSectorOrderedBank")
            <<"invalid number of sector for partition "<<iPart<<" nsect="
            <<nSect<<"\n";
         break;
      }
      std::set<int> sectors;
      int isub=-1;
      for(int i=0;i<nSect;i++) {
         int sector=-1;
         (*partitionFile)>>sector;
         if(partitionFile->fail()||(sector<0)) {
             Error("readSectorOrderedBank")
                <<"invalid sector number "<<sector<<" (position "<<i
                <<" in partition "<<iPart<<")\n";
             break;
         }
         if(nSub) {
            int jsub=sector%nSub;
            if((isub>=0)&&(jsub!=isub)) {
               Error("readSectorOrderedBank")
                  <<"subregion splitting requested, but partition "
                  <<iPart<<" includes sectors from several subregions"
                  <<" isub="<<isub<<" sector="<<sector<<"\n";
            }
            isub=jsub;
            if(isub!=getSubID()) continue;
         }
         if(!sectors.insert(sector).second) {
            Error("readSectorOrderedBank")
               <<"duplicate sector ID "<<sector
               <<" within partition "<<iPart<<"\n";
         } else {
            std::pair<std::map<int,int>::iterator,bool> r=
               allSectors.insert(std::make_pair(sector,iPart));
            if(!r.second) {
               Error("readSectorOrderedBank")
                  <<"duplicate sector ID "<<sector
                  <<" in partition "<<iPart<<" and "<<(*r.first).second<<"\n";
            }
         }
      }
      if(!sectors.size()) continue;
      if(sectors.size()!=(unsigned)nSect) break;
      partitionList.push_back(Partition(nPatt,-1,sectors));
      nPatternBySR[isub>=0 ? isub : 0]+=nPatt;
   }
   // implement limit on the number of sectors
   if(numSectorMax>0) {
      int numSectorTotal=0;
      for(std::list<Partition>::const_iterator ip=partitionList.begin();ip!=partitionList.end();ip++) {
         numSectorTotal+=(*ip).fSectorSet.size();
      }
      Info("readSectorOrderedBank")
         <<"region="<<getBankID()
         <<" numSectorMax="<<numSectorMax
         <<" numSectorTotal="<<numSectorTotal<<"\n";
      if(numSectorTotal>numSectorMax) {
         int numSectorImplemented=0;
         std::multimap<int,std::list<Partition>::iterator> multiplicity;
         for(std::list<Partition>::iterator ip=partitionList.begin();ip!=partitionList.end();ip++) {
            // limit
            (*ip).fNumSectorMax=(int)((numSectorMax*(double)(*ip).fSectorSet.size())/numSectorTotal+0.5);
            numSectorImplemented+=(*ip).fNumSectorMax;
            multiplicity.insert(std::make_pair((*ip).fSectorSet.size(),ip));
         }
         // adjust until number of sector precisely matches
         if(numSectorImplemented!=numSectorMax) {
            // if there is a sector with a limit of zero , increase it
            for(std::multimap<int,std::list<Partition>::iterator>::iterator im=multiplicity.begin();im!=multiplicity.end();im++) {
               std::list<Partition>::iterator ip=(*im).second;
               if((numSectorImplemented<numSectorMax)&&((*ip).fNumSectorMax==0)) {
                  (*ip).fNumSectorMax++;
                  numSectorImplemented++;
               } else if((numSectorImplemented>=numSectorMax)||((*ip).fNumSectorMax)) {
                  break;
               }
            }
            for(std::multimap<int,std::list<Partition>::iterator>::iterator im=multiplicity.begin();im!=multiplicity.end();im++) {
               std::list<Partition>::iterator ip=(*im).second;
               if(numSectorImplemented<numSectorMax) {
                  (*ip).fNumSectorMax++;
                  numSectorImplemented++;
               } else if(numSectorImplemented>numSectorMax) {
                  (*ip).fNumSectorMax--;
                  numSectorImplemented--;
               } else {
                  break;
               }
            }
         }
      }
   }
   for(int isub=0;(isub<nSub)||(!isub);isub++) {
      Info("readSectorOrderedBank")
         <<"Subregion "<<isub<<" number of patterns: "<<nPatternBySR[isub]
         <<std::setbase(16)<<" (0x"<<nPatternBySR[isub]
         <<std::setbase(10)<<")\n";
   }
   delete partitionFile;
   return readPartitionedSectorOrderedBank(name,partitionList,nlamb);
}

/**
   readPartitionedSectorOrderedBank()

   read sector-ordered bank, convert to DC patterns
   the DC patterns are filled using the predefined partitions, each partition
   with a maximum number of patterns
*/
int FTK_CompressedAMBank::readPartitionedSectorOrderedBank
(const char *name,std::list<FTK_CompressedAMBank::Partition> const &partitionList,
 int nlamb) {
   // read bad modules
   readWildcards();
   // open file
   int error=0;
   TDirectory *TSPfile=FTKRootFile::Instance()->OpenRootFileReadonly(name);
   // read sector-ordered root file (TSP patterns)
   if(TSPfile) {
      int partitionCount=0;
      int sectorCount=0;
      int nLayer=0;
      // determine largest sector number
      int largestSector=0;
      for(std::list<FTK_CompressedAMBank::Partition>::const_iterator
             iPartition=partitionList.begin();iPartition!=partitionList.end();
          iPartition++) {
         for(std::set<int>::const_iterator iSector=
                (*iPartition).fSectorSet.begin();
             iSector!=(*iPartition).fSectorSet.end();iSector++) {
            if((*iSector)>largestSector) largestSector=(*iSector);
         }
      }
      // store patterns here
      VECTOR<HitPatternMap_t> dcPatterns(largestSector+1);
      int nDC=0;
      int nTSP=0;
      int maxpatts=0;
      for(std::list<FTK_CompressedAMBank::Partition>::const_iterator
             iPartition=partitionList.begin();iPartition!=partitionList.end();
          iPartition++) {
         maxpatts+=(*iPartition).fNumPatternMax;
         // the FTKPatternBySectorBlockReader is reading the patterns
         // sector-by-sector, in a pre-defined coverage range
         //
         // if the number of subregions is specified, it will ignore all
         // sectors which do not belong to the active subregion
         FTKPatternBySectorReader *TSPreader=FTKPatternBySectorReader::Factory
            (*TSPfile,&(*iPartition).fSectorSet);
         if(TSPreader->GetNLayers()<=0) {
            Fatal("readSectorOrderedBank")
               <<"number of layers (reader) "<<TSPreader->GetNLayers()<<"\n";
         }
         TSPreader->CheckConsistency(getSSMapTSP(),getBankID(),getHWModeSS_tsp());
         if(!nLayer) {
            setNPlanes(TSPreader->GetNLayers());
            nLayer=TSPreader->GetNLayers();
         }
         Info("readSectorOrderedBank")
            <<"reading from "<<name<<" nLayer="<<TSPreader->GetNLayers()
            <<" partition "<<partitionCount<<" maxpattern="
            <<(*iPartition).fNumPatternMax<<" maxSector="<<(*iPartition).fNumSectorMax
            <<"\n";
         // the coverage map holds for each coverage the number of patterns
         // it is used in order to estimate down to which coverage the patterns
         // can be read
#ifdef SEARCH_MEMORY_LEAK
         printVmemUsage("before GetNPatternsByCoverage");
#endif
         std::map<int,int> coverageMap;
         TSPreader->GetNPatternsByCoverage(coverageMap);
#ifdef SEARCH_MEMORY_LEAK
         printVmemUsage("after GetNPatternsByCoverage");
#endif
         std::map<int,int>::const_reverse_iterator i=coverageMap.rbegin();
         TSPreader->Rewind();
#ifdef SEARCH_MEMORY_LEAK
         printVmemUsage("after Rewind");
#endif
         // print bank statistics
         // determine total number of patterns , patterns*coverage, <coverage>
         uint32_t totalPatterns=0;
         uint64_t totalPatternsCoverage=0;
         int nDC0=nDC;
         for(std::map<int,int>::const_iterator j=coverageMap.begin();
             j!=coverageMap.end();j++) {
            totalPatterns += (*j).second;
            totalPatternsCoverage += (*j).first*(uint64_t)(*j).second;
         }
         Info("readSectorOrderedBank")
            <<"input patterns (TSP): "<<totalPatterns
            <<" generated patterns (TSP): "<<totalPatternsCoverage
            <<" average coverage: "
            <<(totalPatterns ? (totalPatternsCoverage/(double)totalPatterns):0)
            <<"\n";
         int covEnd;
         std::set<int> loadedSectorList;
         int maxNumSector=(*iPartition).fNumSectorMax;
         if(maxNumSector<0) maxNumSector=(*iPartition).fSectorSet.size();
         do {
            std::map<int,int>::const_reverse_iterator j=i;
            int npattEstimate=nDC;
            covEnd=(*i).first;
            while((j!=coverageMap.rend())&&(npattEstimate<maxpatts)) {
               npattEstimate += (*j).second;
               covEnd=(*j).first;
               j++;
            }
            Info("readSectorOrderedBank")
               <<"adding coverage range "<<(*i).first<<"-"<<covEnd
               <<" extra patterns="<<npattEstimate-nDC<<"\n";
            // read all patterns in the given coverage range
            // (not reading the smallest coverage)
            // these patterns for sure will fit into the DC bank
            //for(int sector=TSPreader->GetFirstSector();sector>=0;
            //    sector=TSPreader->GetNextSector(sector)) {
            for(std::set<int>::const_iterator sectorPtr=
                   (*iPartition).fSectorSet.begin();
                sectorPtr!=(*iPartition).fSectorSet.end();sectorPtr++) {
               int sector=*sectorPtr;
               if(loadedSectorList.find(sector)==loadedSectorList.end()) {
                  if((int)loadedSectorList.size()>=maxNumSector) continue;
                  loadedSectorList.insert(sector);
               }
               FTKPatternOneSector *patterns=TSPreader->Read(sector,covEnd+1);
               if(patterns) {
                  insertPatterns(sector,patterns,maxpatts,dcPatterns,nDC,nTSP);
                  delete patterns;
               }
            }
            // read patterns for the smallest coverage
            // these patterns will partially fit into the bank
            for(std::set<int>::const_iterator sectorPtr=
                   (*iPartition).fSectorSet.begin();
                sectorPtr!=(*iPartition).fSectorSet.end();sectorPtr++) {
               int sector=*sectorPtr;
               if(loadedSectorList.find(sector)==loadedSectorList.end()) {
                  if((int)loadedSectorList.size()>=maxNumSector) continue;
                  loadedSectorList.insert(sector);
               }
               FTKPatternOneSector *patterns=TSPreader->Read(sector,covEnd);
               if(patterns) {
                  insertPatterns(sector,patterns,maxpatts,dcPatterns,nDC,nTSP);
                  delete patterns;
                  if(nDC>=maxpatts) break;
               }
            }
            i=j;
             // stop if all patterns have been read
            // or if the maximum number of patterns is reached

            // DEBUG - speed up parttern reading
            //   break;

         } while((i!=coverageMap.rend())&&(nDC<maxpatts));
         Info("readSectorOrderedBank")
            <<"partition "<<partitionCount<<" number of DC patterns: "<<nDC-nDC0
            <<" smallest coverage="<<covEnd
            <<" #sectors="<<loadedSectorList.size()
            <<" maxnsector="<<maxNumSector
            <<"\n";
         partitionCount++;
         sectorCount += loadedSectorList.size();
         delete TSPreader;
      }
      Info("readSectorOrderedBank")
         <<"total number of patterns read "<<nTSP<<" stored "<<nDC
         <<" packing "<<(nDC?(nTSP/(double)nDC):0.)
         <<" #sector="<<sectorCount<<"\n";
      // now all patterns are stored here
      //   VECTOR<HitPatternMap_t> dcPatterns(maxSector+1);
      // next, loop over all DC patterns and
      //   insert the corresponding TSP patterns
      int nDCHB=2;
      int nCOV=2;
      int offsetSSID=nLayer+nDCHB+nCOV;
      int pos=0;
      VECTOR<std::pair<int,int> > sectorPointer(dcPatterns.size());
      int numPattern=0;
      for(unsigned sector=0;sector<sectorPointer.size();sector++) {
         // initialize start pointer per sector
         // and end=start
         sectorPointer[sector]=std::make_pair(pos,pos);
         // start position of next sector
         pos += dcPatterns[sector].size()*offsetSSID;
         numPattern+=dcPatterns[sector].size();
      }
      int32_t *ssidData=new int32_t[pos];
      for(unsigned sector=0;sector<dcPatterns.size();sector++) {
         HitPatternMap_t const &hitMap=dcPatterns[sector];
         // pos is a reference, so increasing pos below increases
         //  the end position in the sectorPointer array
         int &pos=sectorPointer[sector].second;
         for(HitPatternMap_t::const_iterator iPattern=hitMap.begin();
             iPattern!=hitMap.end();iPattern++) {
            // dchb contains the encoded DC and HB bits
            //   DCHB7 | DCHB6 ... DCHB1 | DCHB0
            uint64_t dchb=(*iPattern).second.first;
            int dcMask=0,hbMask=0;
            int shift=0;
            for(int ipl=0;ipl<nLayer;ipl++) {
               int nDCbits=m_TSPmap->getNBits(ipl);
               int nDCHBbits=2*nDCbits;
               ssidData[pos++]=(*iPattern).first.GetHit(ipl);
               int dcBits=dchb & ((1<<nDCHBbits)-1);
               dchb>>=nDCHBbits;
               dcMask |= m_dcMaskLookup[ipl][dcBits]<<shift;
               hbMask |= m_hbMaskLookup[ipl][dcBits]<<shift;
               shift += nDCbits;
            }
            ssidData[pos++]=dcMask;
            ssidData[pos++]=hbMask;
            ssidData[pos++]=(*iPattern).second.second>>32; // number of TSP patterns
            ssidData[pos++]=(*iPattern).second.second; // summed coverage
         }
      }
      dcPatterns.resize(0);
      importDCpatterns(nLayer,offsetSSID,ssidData,sectorPointer,nlamb);
      delete [] ssidData;

      readBankPostprocessing("readSectorOrderedBank");
      delete TSPfile;
   } else {
      Error("readSectorOrderedBank")<<"failed to open \""<<name<<"\"\n";
      error++;
   }
   return error;
}

/**
   init()

   initialize the pattern finding
*/
void FTK_CompressedAMBank::init() {
   FTK_AMsimulation_base::init();
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
   // AM input
   m_tspSSID.resize(0);
   m_tspSSID.resize(getNPlanes());
   // AM output
   m_roads.clear();

   m_MAX_MISSING_PLANES = FTKSetup::getFTKSetup().getMaxMissingPlanes();
   m_nhWCmin=getNPlanes()-m_MAX_MISSING_PLANES;
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
#ifdef INTERNAL_LIST
   // list of hits
   typedef std::list<FTKHit const *> SSinfo_t;
   // first: TSP-SSID, second: pair<dc-ssid,hits>
   typedef MAP<int,SSinfo_t> SSinfoBySSID_t;
   // for each plane
   typedef std::vector<SSinfoBySSID_t> SSinfoByLayerSSID_t;
   SSinfoByLayerSSID_t hitList(getNPlanes());
#else
   std::vector< std::set<int> > tsp_ss_set(getNPlanes());
#endif
   // layers where all hits shall be skipped
   int layerMask=FTKSetup::getFTKSetup().getMaskLayers();

   std::vector<int> nHit(getNPlanes());
   int error=0;
   int expectedNHit=0;
   for(std::vector<FTKHit>::const_iterator ihit = hits.begin();
       ihit!=hits.end();++ihit) {
      // pointer to the hit
      const FTKHit *hit = & (*ihit);
      // plane
      int iplane=hit->getPlane();
      // verify the hit according the mask that masks particular logical laeyrs
      if (layerMask&(1<<iplane)) continue; // skip this hit because the layer
      if (!getSSMapTSP()->getRegionMap()->isHitInRegion(*hit,getBankID()))
         continue; // skip if the is not in this region
      ++nHit[iplane];
      // pointer to the copy
      // get SSID
      expectedNHit++;
      int tsp_ssid,coded_ssid;
      if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) {
         // Using a dummy pattern bank representing just the number of sectors, the IDs are the module IDs, for historical reason called sector.
         tsp_ssid = hit->getSector();
         coded_ssid = hit->getSector();
         // filter out bad modules
         if(m_badModules[iplane].find(coded_ssid)
            !=m_badModules[iplane].end()) {
            continue;
         }
      } else {
         try{
            if (getHWModeSS_tsp()==0) {
               //SS calculated assuming a global SS id
               tsp_ssid =getSSMapTSP()->getSSGlobal(*hit);
            } else if (getHWModeSS_tsp()==2) {
               // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
               tsp_ssid = getSSMapTSP()->getSSTower(*hit,getBankID());
            } else {
               Fatal("sort_hits")
                  <<"hardware mode "<<getHWModeSS_tsp()
                  <<" [TSP] not supported\n";
            }
            coded_ssid=getDCssidConst(iplane,tsp_ssid);
            // check whether the hit should be skipped (bad module)
            if((coded_ssid==s_INVALIDid)||
               (coded_ssid==s_WILDCARDid)) continue;
            if (getHWModeSS_dc()==0) {
               // SS calculated assuming a global SS id
               // large-size pattern ID in upper bits , fine resolution subpattern in lower bits
               coded_ssid = (getSSMap()->getSSGlobal(*hit)
                             <<m_TSPmap->getNBits(iplane))
                  | m_TSPmap->getHighResSSPart(*hit);
            } else if (getHWModeSS_dc()==2) {
               // get SSID directly from fine pattern map
               //   coded_ssid = getSSMapTSP()->getSSTower(*hit,getBankID());
#ifdef HW2_USE_TSPMAP
               getSSMapTSP()->setSSDCX(iplane,m_TSPmap->getNBits(iplane,0));
               getSSMapTSP()->setSSDCY(iplane,m_TSPmap->getNBits(iplane,1));
               coded_ssid = getSSMapTSP()->getSSTower(*hit,getBankID());
               getSSMapTSP()->setSSDCX(iplane,0);
               getSSMapTSP()->setSSDCY(iplane,0);
#else
               coded_ssid = (getSSMap()->getSSTower(*hit,getBankID())
                             <<m_TSPmap->getNBits(iplane))
                  |  m_TSPmap->getHighResSSPart(*hit);
#endif
            } else {
               Fatal("sort_hits")
                  <<"hardware mode "<<getHWModeSS_dc()
                  <<" [DC] not supported\n";
            }
         } catch(FTKException &e) {
            error++;
            // skip this hit
            continue;
         }
      }

#ifdef INTERNAL_LIST
      // check whether this TSP-SSID is new or not
      SSinfoBySSID_t::iterator iSSID=hitList[iplane].find(coded_ssid);
      if(iSSID==hitList[iplane].end()) {
         // this SSID has not been entered yet
         iSSID=hitList[iplane].insert
            (std::make_pair(coded_ssid,std::list<FTKHit const *>())).first;
         m_tspSSID[iplane].push_back(tsp_ssid);
      }
      // add hit
      (*iSSID).second.push_back(hit);
      //cout<<"P"<<iplane<<" SSID="<<coded_ssid<<" nhit="
      //    <<(*iSSID).second.size()<<"\n";
#else
      std::unordered_map<int,FTKSS>::iterator
         ssPtr=m_FiredSSmap[iplane].insert
         (std::make_pair(coded_ssid,FTKSS())).first;
      (*ssPtr).second.addHit(*hit);
      
      if(tsp_ss_set[iplane].insert(tsp_ssid).second) {
         m_tspSSID[iplane].push_back(tsp_ssid);
      }
      /* std::cout<<"P"<<iplane
               <<" codedSSID="<<coded_ssid
               <<" tspSSID="<<tsp_ssid
               <<" DC="<<(coded_ssid>>m_TSPmap->getNBits(iplane))
               <<" highres="<<(coded_ssid & ((1<<m_TSPmap->getNBits(iplane))-1))
               <<" nhit="
               <<(*ssPtr).second.getNHits()<<"\n"; */
#endif
   }
   if(error) {
      Warning("sort_hits")<<"failed to calculate SSID for "<<error<<"/"
                        <<expectedNHit<<" FTKHits\n";
   }
   for (int ipl=0;ipl<getNPlanes();++ipl) {
#ifdef INTERNAL_LIST
      SSinfoBySSID_t &hitsInLayer=hitList[ipl];
      MAP<int,std::vector<FTKSS> > firedTSPbyDC;
      for(SSinfoBySSID_t::const_iterator iSSID=hitsInLayer.begin();
          iSSID!=hitsInLayer.end();iSSID++) {
         int coded_ss=(*iSSID).first;
         std::list<FTKHit const *> const &hits=(*iSSID).second;
         std::unordered_map<int,FTKSS>::iterator
            ssPtr=m_FiredSSmap[ipl].insert
            (std::make_pair(coded_ss,FTKSS())).first;
         for(std::list<FTKHit const *>::const_iterator ihit=
                hits.begin();ihit!=hits.end();ihit++) {
            (*ssPtr).second.addHit(**ihit);
         }
      }
#endif
      naoSSPushBack(m_FiredSSmap[ipl].size());
   }
   // statistics
   for (int iplane=0;iplane!=getNPlanes();++iplane) {
      naoClusPushBack(nHit[iplane]);
   }
   //m_FiredSSmap[iplane].insert(std::make_pair(m_WCID,FTKSS()));
}

void FTK_CompressedAMBank::data_organizer(void) {
   data_organizer_r(m_tspSSID);
}

void FTK_CompressedAMBank::data_organizer_r
(std::vector<std::list<int> > const &tspSSIDfired) {
   int maxSector=0;
   if(m_SectorFirstLastPattern.rbegin()!=m_SectorFirstLastPattern.rend())
      maxSector=(*m_SectorFirstLastPattern.rbegin()).first+1;
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
   // discard sectors with insufficient number of hits candidates
   for(MAP<int,std::pair<int,int> >::const_iterator isector=
          m_SectorFirstLastPattern.begin();
       isector!=m_SectorFirstLastPattern.end();isector++) {
      int sector=(*isector).first;
      // determine wildcard for this sector
      HitPattern_t sectorWildcard=m_SectorWC[sector];
      int hitPattern=m_sectorUsage[sector] | sectorWildcard;
      int nhit=m_nHit16[hitPattern];
#ifdef DEBUG_EVENT
      if(g_event==DEBUG_EVENT-1) {
         if(sector<20) {
            std::cout<<"sector="<<sector<<std::setbase(16)
                     <<" wc=0x"<<(int)sectorWildcard
                     <<" hitPattern=0x"<<(int)hitPattern
                     <<std::setbase(10)
                     <<" nhit="<<nhit
                     <<" m_nhWCmin="<<(int)m_nhWCmin
                     <<"\n";
         }
      }
#endif
      if(nhit>=m_nhWCmin) {
         //
         // reset hit pattern for this sector
         int firstPattern=(*isector).second.first;
         int lastPattern=(*isector).second.second;
         if(firstPattern<=lastPattern) {
            if(sizeof(HitPattern_t)==1) {
               memset(m_hitPatterns.data()+firstPattern,sectorWildcard,
                      lastPattern+1-firstPattern);
            } else {
               for(int i=firstPattern;i<=lastPattern;i++) {
                  m_hitPatterns[i]=sectorWildcard;
               }
            }
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
   /* for(unsigned layer=0;layer<m_tspSSID.size();layer++) {
      Info("am_in")
         <<"number of hits layer"<<layer<<" is "<<m_tspSSID[layer].size()<<"\n";
         } */
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
         : m_patternPtr(hits), m_mask(msk) {
      }
      inline void process(void) {
         *m_patternPtr |= m_mask;
      }
      inline void update(int delta) {
         m_patternPtr += delta;
      }
      inline void initialize(int firstPattern) {
         update(firstPattern);
      }
   protected:
      HitPattern_t * __restrict m_patternPtr;
      HitPattern_t m_mask;
   };
   class MaskUpdaterSlow {
      // slow update:
      // update hit pattern and
      // determine road candidates
      // a road candidate is stored if:
      //   (1) the pattern has hits in other layers
      //   (2) the pattern has no hit in the present layer
      //   (3) the updated mask has exactly the minimum number of hits
      // condition (1) is to skip empty patterns (not needed?)
      // condition (2) is to ensure a valid pattern is stored only once
      // condition (3) is there to ensure the pattern is there and stored only
      //   as the threshold is passed
   public:
      inline MaskUpdaterSlow(HitPattern_t *hits,HitPattern_t msk,
                             uint32_t sectorID,
                             uint8_t nhmin,uint8_t const *nh16,
                             VECTOR<
                             std::pair<uint32_t,uint32_t>
                             > &rc,unsigned &nrc)
         : m_patternPtr(hits), m_mask(msk),m_notMask(~msk),m_roadCandidates(rc),
           m_nRC(nrc),m_nhMin(nhmin),m_nhit16(nh16),m_sector(sectorID)
      {
         m_base=m_patternPtr;
      }
      inline void process(void) {
         HitPattern_t h0=*m_patternPtr;
         HitPattern_t h1=h0|m_mask;
#ifdef DEBUG_EVENT
         if(g_event==DEBUG_EVENT) {
            std::cout<<m_patternPtr-m_base<<": 0x"<<std::setbase(16)
                     <<(int)h0<<" -> "<<(int)h1
                     <<std::setbase(10);
         }
#endif
         if( // if there are hits in other layers
            (h0 & m_notMask)&&
            // if this is the first hit in this layer
            (!(h0 & m_mask))&&
            // if the number of hits is exactly at the threshold
            (m_nhit16[h1]==m_nhMin)
            ) {
            if(m_nRC<m_roadCandidates.size()) {
               m_roadCandidates[m_nRC++] =
                  std::make_pair(m_patternPtr-m_base,m_sector)
                  ;
#ifdef DEBUG_EVENT
               if(g_event==DEBUG_EVENT) {
                  std::cout<<" >>> add roadCandidate";
               }
#endif
            }
         }
#ifdef DEBUG_EVENT
         if(g_event==DEBUG_EVENT) {
            std::cout<<"\n";
         }
#endif
         *m_patternPtr =h1;
      }
      inline void update(int delta) {
         m_patternPtr += delta;
      }
      inline void initialize(int firstPattern) {
         update(firstPattern);
      }
   protected:
      HitPattern_t * __restrict  m_patternPtr,* __restrict  m_base;
      HitPattern_t m_mask;
      HitPattern_t m_notMask;
      VECTOR<std::pair<uint32_t,uint32_t> > &m_roadCandidates;
      unsigned &m_nRC;
      uint8_t m_nhMin;
      uint8_t const *m_nhit16;
      uint32_t m_sector;
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
               // determine threshold for slow update
               int hitMin=m_nhWCmin-m_nHit16[m_SectorWC[sector]]-1;
               if(iplane<hitMin) {
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
                      sector,
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
      int patternID=m_roadCand[iCand].first;
      int sector=m_roadCand[iCand].second;

      const FTKPlaneMap *pmap = getSSMapTSP()->getPlaneMap();

      HitPattern_t hitmaskWithWC=m_hitPatterns[patternID];
      HitPattern_t hitmaskNoWC=hitmaskWithWC & ~m_SectorWC[sector];
      uint8_t nhWC=m_nHit16[hitmaskWithWC];
#ifdef DEBUG_EVENT
      if(g_event==DEBUG_EVENT) {
         std::cout<<"am_output "<<patternID
                  <<std::setbase(16)
                  <<" 0x"<<(int)hitmaskWithWC
                  <<" 0x"<<(int)hitmaskNoWC
                  <<std::setbase(10)
                  <<" nhWC="<<(int)nhWC
                  <<" nhWCmin="<<m_nhWCmin
                  <<"\n";
      }
#endif
      //
      // determine FTKRoads
      if(nhWC>=m_nhWCmin) {
         //
         // extra checks
         // required hits in first or last plane 
         uint32_t lastPlaneMask=1<<(getNPlanes()-1);
         bool first_check = (!getRequireFirst()) || (hitmaskWithWC & 1);
         bool missinglast = ! (hitmaskWithWC & lastPlaneMask);
         bool last_check = (!getRequireLast()) || !missinglast;

         if( first_check && last_check &&
             (nhWC>= getNPlanes()-m_MAX_MISSING_PLANES)) {
            //
            // statistics
            countNRoads();
            //
            // number of hits excluding wildcards
            uint8_t nh=m_nHit16[hitmaskNoWC];
            //
            // construct FTKRoad object
            //   see: FTK_AMBank::am_output
            m_roads.push_front
               (FTKRoad
                (getNRoads()-1,
                 getBankID()+100*getSubID(),
                 // ID is set to patternID
                 patternID,getNPlanes(),
                 nh,     // 
                 hitmaskNoWC // FTK_TSPBANK::attach_SS
                 ));
            FTKRoad &road=* m_roads.begin();

	    // Want to count if we're missing hits
	    bool misspix(false), misssct(false);
	    for(int ip = 0; ip < getNPlanes(); ++ip) {
	      if (!road.hasHitOnLayer(ip)) {
		if (pmap->isSCT(ip)) misssct = true;
		else misspix = true;
	      }
	    }
	    if (misspix) countNRoads_misspix();
	    if (misssct) countNRoads_misssct();
	    if (!misspix && !misssct) countNRoads_complete();

            //
            road.setDCMatchMode(1);
            //
            // add missing information
            //   sector ID 
            road.setSectorID(sector);  //attachSS[ 655 -> 662]
            //   DBID is set identical to ID
            road.setPatternDBID(patternID);
            //
            // unpack DC bits and DC-SSID from pattern bank
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
               //
               // decode DC and HB bits
               int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
               dcMask =(dcMask<<nDCbits) | m_dcMaskLookup[ipl][dcBits];
               hbmask =(hbmask<<nDCbits) | m_hbMaskLookup[ipl][dcBits];
               // store SSID
               //2016/01/15 Guido's new format (???)
               //if(sectorWildcard & (1<<ipl)) {
                  // wildcard
               // road.setSSID(ipl,m_WCID);
               //} else {
               int roadSSID=-1;
               if(dcSSID>=0) {
#ifdef HW2_USE_TSPMAP
                  roadSSID=(dcSSID<<nDCbits)|
                     (hbmask & (~dcMask) & ((1<<nDCbits)-1));
#else
                  roadSSID=(dcSSID<<nDCbits)|
                     (hbmask & (~dcMask) & ((1<<nDCbits)-1));
#endif
               } else {
                  roadSSID=dcSSID;
               }
               road.setSSID(ipl,roadSSID);  //attachSS[ 655 -> 670]
               // }
               //
               // store subss mask
               road.setSubSSMask(ipl,m_subSSmask[ipl][dcBits]);
               //
               // set HLID (?)  - see FTKTSPBank::attachSS()
               road.setHLID(ipl,m_TSPmap->getNBits(ipl));
            }
            //
            // store ternary bits
            road.setDCBitmask(dcMask);
            road.setHLBitmask(hbmask);
         }
      }
   }
   naoSetNroadsAM(getNRoads());
   naoSetNroadsAMComplete(getNRoads_complete());
   naoSetNroadsAMMissPix(getNRoads_misspix());
   naoSetNroadsAMMissSCT(getNRoads_misssct());
#ifdef DEBUG_EVENT
   if(g_event==DEBUG_EVENT) exit(0);
   g_event++;
#endif
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
  const FTKPlaneMap *pmap = getSSMapTSP()->getPlaneMap();

  //int nhits1 = hitbit[ibank][NPLANES][patt1];
  //int nhits2 = hitbit[ibank][NPLANES][patt2];
  int nsame = getNPlanes(); // count the number of shared SS

  if(r1->getSectorID()!=r2->getSectorID()) {
     if (ftkset.getRoadWarrior()<=1)
        return 0; // If ROADWARRIOR=0 this means always different patterns
  }

  for(int i = 0; i < getNPlanes(); ++i) { // loop to count the shared SS
     // STS: 2016/01/25 mask out DC bits for comparison 
     int nonDcMask=~((1<<m_TSPmap->getNBits(i))-1);
     if(!pmap->isSCT(i)) { // Pixel plane
      /* pattern[][][] has the strips used by each pattern, if there is a
	 difference the two roads could be different.
	 The different is meaningful only if the two roads are empty in both
	 patterns or if the patt2 is empty while patt1 is a complete pattern*/
        if ( ((r1->getSSID(i)&nonDcMask)!=(r2->getSSID(i)&nonDcMask)) &&
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
        bool condition1 = ((r1->getSSID(i)&nonDcMask)!=
                           (r2->getSSID(i)&nonDcMask)) &&
           r2->hasHitOnLayer(i);
        // if SCT plane pairing is not used both conditions are the same
        bool condition2 (condition1);
        if (i!=ipair) { // pairing is used
           int nonDcMaskPair=~((1<<m_TSPmap->getNBits(ipair))-1);
           condition2 = ((r1->getSSID(ipair)&nonDcMaskPair)!=
                         (r2->getSSID(ipair)&nonDcMaskPair)) &&
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
   int error=0,errorSector=0,errorPattern=0;
   for(std::list<FTKRoad>::iterator iroad = m_roads.begin();
       iroad!=m_roads.end();++iroad) {
      // prepare list of all superstrips attached to any of the roads
      // DC mask: if bit is set: bit takes value [X]
      int dcMask=(*iroad).getDCBitmask();
      // HB mask: if bit is set: bit takes value [0|1]
      int hbMask=(*iroad).getHLBitmask();
      for (int ipl=0;(ipl<getNPlanes())&&(!getStoreAllSS());++ipl) {
         int nDCbits=m_TSPmap->getNBits(ipl);
         // bitMask: set to one for all positions with ternary bits
         int bitMask=(1<<nDCbits)-1;
         // isHB: set to one if bits is [0|1]
         int isHB=bitMask & ~dcMask;
         //  hb:  set to one if bit is [1]
         int hb=hbMask & isHB; 
         // mask out all ternary bits
         int nodc_ssid=(*iroad).getSSID(ipl);
         int found=0;
         for(int bits=0;bits<(1<<nDCbits);bits++) {
            // skip ternary bit combinations if HB bits are not properly set
            if( (bits & isHB) != hb) continue;
            std::unordered_map<int,FTKSS>::iterator item0 =
               m_FiredSSmap[ipl].find(nodc_ssid | bits);
            if (item0==m_FiredSSmap[ipl].end()) 
               continue; // no hits
            // map for this plane
            std::unordered_map<int,FTKSS> &imap = m_UsedSSmap[ipl];
            // find this ss is in the bank
            std::unordered_map<int,FTKSS>::iterator item =
               imap.find(nodc_ssid|bits);
            if (item==imap.end()) { // not found
               imap[nodc_ssid|bits] = (*item0).second;
            }
	    naoClusRoadAdd(ipl,(imap[nodc_ssid|bits]).getNHits());

            found++;
         }
         if((*iroad).hasHitOnLayer(ipl) && !found) {
            Error("getRoads")
               <<"Plane="<<ipl<<" no fired SS found, ssid="
               <<(nodc_ssid>>nDCbits)
               <<" pattern="<<(*iroad).getPatternID()<<"\n";
            std::vector<int> const &ssid_with_dcbits=getTSPssidVector
               (ipl,(*iroad).getSectorID(),nodc_ssid>>nDCbits);
            std::cout<<"DC="<<(nodc_ssid>>nDCbits)<<" -> TSP=[";
            for(size_t i=0;i<ssid_with_dcbits.size();i++) {
               std::cout<<" "<<ssid_with_dcbits[i];
            }
            std::cout<<"]\n";
            std::cout<<"nDCbits="<<nDCbits<<" bitMask="<<bitMask
                     <<" isHB="<<isHB<<" hb="<<hb<<"\n";
            error++;
            errorSector=(*iroad).getSectorID();
            errorPattern=(*iroad).getPatternID();
         }
         // begin debugging
         // if((*iroad).getPatternID()==2113347) error++;
         // end debugging
         dcMask>>=nDCbits;
         hbMask>>=nDCbits;
      } // end loop over the layers
   }
   if(error) {
      printRoads(m_roads,/* -1 */ errorSector);
      printStrips();
      printSector(errorSector,2,errorPattern);
      throw FTKException("FTK_CompressedAMBank::getRoads inconsistency in SSID numbering");
   }

   // //JAAA now add number of hits in each plane within a road for dataflow
   // for (int ipl = 0; ipl<getNPlanes(); ipl++) { // loop over planes
   //   for(std::unordered_map<int,FTKSS>::const_iterator is=
   // 	   m_UsedSSmap[ipl].begin();is!=m_UsedSSmap[ipl].end();is++) { 
   //     ////       std::cerr << "JAAAA ipl = " << ipl << " and adding " << (*is).second.getNHits() << std::endl;
   //     naoClusRoadAdd(ipl,(*is).second.getNHits());
   //   }
   // }


  static int print=PRINT_DETAILS_NEVENT;
  if(print) {
     Info("getRoads")<<"number of roads="<<m_roads.size()<<"\n";
     printRoads(m_roads,PRINT_ROADS_SECTORID);
     Info("getRoads")
        <<"number of strips fired"
        <<" "<< m_FiredSSmap[0].size()
        <<" "<< m_FiredSSmap[1].size()
        <<" "<< m_FiredSSmap[2].size()
        <<" "<< m_FiredSSmap[3].size()
        <<" "<< m_FiredSSmap[4].size()
        <<" "<< m_FiredSSmap[5].size()
        <<" "<< m_FiredSSmap[6].size()
        <<" "<< m_FiredSSmap[7].size()<<"\n";
     for(int ipl=0;ipl<getNPlanes();ipl++) {
        Info("getRoads")<<"number of strips in plane "<<ipl
                        <<" is "<< m_UsedSSmap[ipl].size()<<"\n";
        for(std::unordered_map<int,FTKSS>::const_iterator is=
               m_UsedSSmap[ipl].begin();is!=m_UsedSSmap[ipl].end();is++) {
           Info("getRoads")<<"SSID="<<(*is).first<<" (0x"<<std::setbase(16)<<(*is).first<<std::setbase(10)<<")"
                           <<" nhit="
                           <<(*is).second.getNHits()<<"\n";
           for(int h=0;h<(*is).second.getNHits();h++) {
              FTKHit const &hit=(*is).second.getHit(h);
              int moduleID=hit.getIdentifierHash();
              std::cout<<"pl="<<ipl;
              std::cout<<" hash="<<moduleID
                       <<" localID="<<getSSMapTSP()->getRegionMap()
                 ->getLocalId(getBankID(),ipl,moduleID)
                       <<" x="<<hit[0];
              if(ipl<3) std::cout<<" y="<<hit[1];
              std::cout<<"\n";;
              for(int imap=0;imap<3;imap++) {
                 FTKSSMap *ssmap=getSSMapTSP();
                 if(imap==2) ssmap=getSSMap();
                 if(imap==1) {
                    getSSMapTSP()->setSSDCX(ipl,m_TSPmap->getNBits(ipl,0));
                    getSSMapTSP()->setSSDCY(ipl,m_TSPmap->getNBits(ipl,1));
                 }
                 int ssid=ssmap->getSSTower(hit,getBankID());
                 if(imap<2) std::cout<<" SSID(TSP)=";
                 else {
                    ssid<<=m_TSPmap->getNBits(ipl);
                    std::cout<<" SSID(AM)=";
                 }
                 std::cout<<ssid<<" (0x"<<std::setbase(16)<<ssid<<std::setbase(10)<<")"
                          <<" ssmap phi="<<ssmap->getSSDCX(ipl)
                          <<"/"<<ssmap->getSSPhiWidth(hit)
                          <<"/"<<ssmap->getSSPhiSize(hit)
                          <<" eta="<<ssmap->getSSDCY(ipl)
                          <<"/"<<ssmap->getSSEtaWidth(hit)
                          <<"/"<<ssmap->getSSEtaSize(hit)<<"\n";
                 if(imap==1) {
                    getSSMapTSP()->setSSDCX(ipl,0);
                    getSSMapTSP()->setSSDCY(ipl,0);
                 }
              }
           }
        }
     }
     print--;
  }
#ifdef DEBUG_EVENT
  if(g_event==DEBUG_EVENT) {
     printStrips(-1);
     printSector(11,10,6310853);
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
     printStrips(plane);
     printSS--;
  }
#endif

  if(getStoreAllSS()) {
     //m_FiredSSmap[plane].erase(m_WCID);
     return m_FiredSSmap[plane];
  }
  return m_UsedSSmap[plane];
}
/**
   printStrips()

   for debugging purposes
 */

void FTK_CompressedAMBank::printStrips(int printPlane) const {
   for(int plane=0;plane<getNPlanes();plane++) {
      if((printPlane>=0)&&(printPlane!=plane)) continue;
     Info("getStrips")<<"plane="<<plane<<" fired="<< m_FiredSSmap[plane].size()
                      <<" used="<<m_UsedSSmap[plane].size()<<"\n";
     MAP<int,std::pair<FTKSS const *,int> > sorted;
     for(std::unordered_map<int,FTKSS>::const_iterator i=m_UsedSSmap[plane].begin();
         i!=m_UsedSSmap[plane].end();i++) {
        std::pair<FTKSS const *,int> &ss=sorted[(*i).first];
        ss.first = & (*i).second;
        ss.second |=2;
     }
     for(std::unordered_map<int,FTKSS>::const_iterator i=m_FiredSSmap[plane].begin();
         i!=m_FiredSSmap[plane].end();i++) {
        std::pair<FTKSS const *,int> &ss=sorted[(*i).first];
        ss.first = & (*i).second;
        ss.second |=1;
     }

     for(MAP<int,std::pair<FTKSS const *,int> >::const_iterator
            i=sorted.begin();i!=sorted.end();i++) {
        std::cout<<" "<<(*i).first<<"[";
        if((*i).second.second & 1)std::cout<<"F";
        if((*i).second.second & 2)std::cout<<"U";
        std::cout<<"]";
     }
     std::cout<<"\n";
     for(std::unordered_map<int,FTKSS>::const_iterator 
            is=m_FiredSSmap[plane].begin();
         is!=m_FiredSSmap[plane].end();is++) {
        for(int h=0;h<(*is).second.getNHits();h++) {
           FTKHit const &hit=(*is).second.getHit(h);
           int moduleID=hit.getIdentifierHash();
           std::cout<<"pl="<<plane;
           std::cout<<" hash="<<moduleID
                    <<" localID="<<getSSMapTSP()->getRegionMap()
              ->getLocalId(getBankID(),plane,moduleID)
                    <<" xHW="<<hit.getHwCoord(0)<<" x="<<hit[0];
           if(plane<3) std::cout<<" yHW="<<hit.getHwCoord(1)<<" y="<<hit[1];
           std::cout<<"\n";;
           for(int imap=0;imap<3;imap++) {
              FTKSSMap *ssmap=getSSMapTSP();
              if(imap==2) ssmap=getSSMap();
              if(imap==1) {
                 getSSMapTSP()->setSSDCX(plane,m_TSPmap->getNBits(plane,0));
                 getSSMapTSP()->setSSDCY(plane,m_TSPmap->getNBits(plane,1));
              }
              int ssid=ssmap->getSSTower(hit,getBankID());
              if(imap<2) std::cout<<" SSID(TSP)=";
              else {
                 ssid<<=m_TSPmap->getNBits(plane);
                 std::cout<<" SSID(AM)=";
              }
              std::cout<<ssid<<" (0x"<<std::setbase(16)<<ssid<<std::setbase(10)<<")"
                       <<" ssmap phi="<<ssmap->getSSDCX(plane)
                       <<"/"<<ssmap->getSSPhiWidth(hit)
                       <<"/"<<ssmap->getSSPhiSize(hit)
                       <<" eta="<<ssmap->getSSDCY(plane)
                       <<"/"<<ssmap->getSSEtaWidth(hit)
                       <<"/"<<ssmap->getSSEtaSize(hit)<<"\n";
              if(imap==1) {
                 getSSMapTSP()->setSSDCX(plane,0);
                 getSSMapTSP()->setSSDCY(plane,0);
              }
           }
        }
     }
   }
}

/**
   printSector()

   for debugging purposes
 */

void FTK_CompressedAMBank::printSector(int sector,int npattern,
                                       int ipattern) {
   std::cout<<" FTK_CompressedAMBank::printSector\n";
   MAP<int,std::pair<int,int> >::const_iterator sectorPtr=
      m_SectorFirstLastPattern.find(sector);
   if(sectorPtr!=m_SectorFirstLastPattern.end()) {
      std::cout<<" sector="<<sector<<" ["
               <<(*sectorPtr).second.first<<","
               <<(*sectorPtr).second.second<<"]\n";
      int i0[2],i1[2];
      int npart=1;
      i0[0]=(*sectorPtr).second.first;
      i1[0]=(*sectorPtr).second.second;
      if(i1[0]-i0[0]+1>npattern) {
         i1[1]=i1[0];
         i1[0]=i0[0]+npattern/2-1;
         i0[1]=i1[1]-(npattern-npattern/2)+1;
         npart=2;
      }
      if(ipattern>=0) {
         if(((ipattern<i0[0])||(ipattern>i1[0]))&&
            ((ipattern<i0[1])||(ipattern>i1[1]))) {
            if((ipattern<i0[0])||(ipattern>i1[1])) {
               std::cout<<"Error: requested pattern "<<ipattern
                        <<" is not in sector "<<sector<<"\n";
            } else {
               npart=1;
               i0[0]=ipattern-npattern/2;
               i1[0]=ipattern+npattern/2;
               if(i0[0]<(*sectorPtr).second.first)
                  i0[0]=(*sectorPtr).second.first;
               if(i1[0]>(*sectorPtr).second.second)
                  i1[0]=(*sectorPtr).second.second;
            }
         }
      }
      std::cout<<"printing patterns "<<i0[0]<<"-"<<i1[0];
      if(npart>1) {
         std::cout<<" and "<<i0[1]<<"-"<<i1[1];
      }
      std::cout<<" of sector "<<sector<<"\n";
      std::cout<<"==pattID";
      for(int ipl=0;ipl<getNPlanes();ipl++) {
         int nDCbits=m_TSPmap->getNBits(ipl);
         if(nDCbits) std::cout<<"=";
         for(int i=0;i<nDCbits;i++) std::cout<<"=";
         std::cout<<"=====L"<<ipl;
      }
      std::cout<<"\n";
      VECTOR<MAP<int,int> > usedSSID(getNPlanes());
      for(int part=0;part<npart;part++) {
         if(part>0) std::cout<<" ...\n";
         for(int patternID=i0[part];patternID<=i1[part];patternID++) {
            std::cout<<std::setw(8)<<patternID;
            int nmax=0;
            for(int ipl=0;ipl<getNPlanes();ipl++) {
               int nDCbits=m_TSPmap->getNBits(ipl);
               int nDCHBbits=2*nDCbits;
               unsigned dcSSID_with_DCbits;
               if(m_bank.m_pattern8Data[ipl].size()) {
                  dcSSID_with_DCbits=m_bank.m_pattern8Data[ipl][patternID];
               } else {
                  dcSSID_with_DCbits=m_bank.m_pattern16Data[ipl][patternID];
               }
               int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
               uint32_t submask= m_subSSmask[ipl][dcBits];
               int imax=0;
               while(submask) {
                  if(submask&1) imax++;
                  submask>>=1;
               }
               if(imax>nmax) nmax=imax;
               int dcSSIDindex=dcSSID_with_DCbits >> nDCHBbits;
               int dcSSID=m_bank.m_PatternByLayer[ipl].
                  m_dcSSIDbySectorIndex[sector][dcSSIDindex];
               int dcMask = m_dcMaskLookup[ipl][dcBits];
               int hbmask = m_hbMaskLookup[ipl][dcBits];
               std::cout<<std::setw(7)<<dcSSID;
               if(nDCbits) {
                  std::cout<<"-";
                  for(int i=nDCbits-1;i>=0;i--) {
                     if(dcMask & (1<<i)) {
                        std::cout<<"X";
                     } else if(hbmask &(1<<i)) {
                        std::cout<<"1";
                     } else {
                        std::cout<<"0";
                     }
                  }
               }
            }
            std::cout<<"\n";
            for(int line=0;line<nmax;line++) {
               std::cout<<"        ";
               for(int ipl=0;ipl<getNPlanes();ipl++) {
                  int nDCbits=m_TSPmap->getNBits(ipl);
                  int nDCHBbits=2*nDCbits;
                  unsigned dcSSID_with_DCbits;
                  if(m_bank.m_pattern8Data[ipl].size()) {
                     dcSSID_with_DCbits=m_bank.m_pattern8Data[ipl][patternID];
                  } else {
                     dcSSID_with_DCbits=m_bank.m_pattern16Data[ipl][patternID];
                  }
                  int dcSSIDindex=dcSSID_with_DCbits >> nDCHBbits;
                  int dcSSID=m_bank.m_PatternByLayer[ipl].m_dcSSIDbySectorIndex
                     [sector][dcSSIDindex];
                  int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
                  uint32_t submask= m_subSSmask[ipl][dcBits];
                  std::vector<int> const &tspSSID=
                     getTSPssidVector(ipl,sector,dcSSID);
                  int isub=0;
                  unsigned ipos;
                  if(nDCbits) std::cout<<" ";
                  for(int i=0;i<nDCbits;i++) std::cout<<" ";
                  for(ipos=0;ipos<tspSSID.size();ipos++) {
                     if(submask &1) {
                        if(isub==line) {
                           std::cout<<std::setw(7)<<tspSSID[ipos];
                           usedSSID[ipl].insert
                              (std::make_pair(tspSSID[ipos],sector));
                           break;
                        }
                        isub++;
                     }
                     submask>>=1;
                  }
                  if(ipos==tspSSID.size()) std::cout<<"       ";
               }
               std::cout<<"\n";
            }
            std::cout<<"\n";
         }
      }
      std::cout<<"Encoded DC-space patterns with DC bits (sector="
               <<sector<<")\n";
      std::cout<<"==pattID";
      for(int ipl=0;ipl<getNPlanes();ipl++) {
         std::cout<<"=SS=";
         for(int i=0;i<m_TSPmap->getNBits(ipl);i++) {
            std::cout<<"T";
         }
      }
      std::cout<<"\n";
      for(int part=0;part<npart;part++) {
         if(part>0) std::cout<<" ...\n";
         for(int patternID=i0[part];patternID<=i1[part];patternID++) {
            std::cout<<std::setw(8)<<patternID;
            for(int ipl=0;ipl<getNPlanes();ipl++) {
               int nDCbits=m_TSPmap->getNBits(ipl);
               int nDCHBbits=2*nDCbits;
               unsigned dcSSID_with_DCbits;
               if(m_bank.m_pattern8Data[ipl].size()) {
                  dcSSID_with_DCbits=m_bank.m_pattern8Data[ipl][patternID];
               } else {
                  dcSSID_with_DCbits=m_bank.m_pattern16Data[ipl][patternID];
               }
               int dcSSIDindex=dcSSID_with_DCbits >> nDCHBbits;
               int dcBits=dcSSID_with_DCbits & ((1<<nDCHBbits)-1);
               int dcMask = m_dcMaskLookup[ipl][dcBits];
               int hbmask = m_hbMaskLookup[ipl][dcBits];
               std::cout<<std::setw(3)<<dcSSIDindex;
               if(nDCbits) {
                  std::cout<<"-";
                  for(int i=nDCbits-1;i>=0;i--) {
                     if(dcMask & (1<<i)) {
                        std::cout<<"X";
                     } else if(hbmask &(1<<i)) {
                        std::cout<<"1";
                     } else {
                        std::cout<<"0";
                     }
                  }
               }
            }
            std::cout<<"\n";
         }
      }
      std::cout<<"DC-SSID translation tables for sector "<<sector<<"\n";
      std::cout<<"=L=size======================data=====================================\n";
      for(int ipl=0;ipl<getNPlanes();ipl++) {
         int n=m_bank.m_PatternByLayer[ipl].m_dcSSIDbySectorIndex[sector]
            .size();
         std::cout<<" "<<ipl<<std::setw(5)<<n<<" [";
         if(n<8) {
            for(int j=0;j<n;j++) {
               std::cout<<" "<<m_bank.m_PatternByLayer[ipl].
                  m_dcSSIDbySectorIndex[sector][j];
            }
         } else {
            for(int j=0;j<4;j++) {
               std::cout<<" "<<m_bank.m_PatternByLayer[ipl].
                  m_dcSSIDbySectorIndex[sector][j];
            }
            std::cout<<" ...";
            for(int j=n-4;j<n;j++) {
               std::cout<<" "<<m_bank.m_PatternByLayer[ipl].
                  m_dcSSIDbySectorIndex[sector][j];
            }
         }
         std::cout<<" ]\n";
      }
      std::cout<<"relevant part of compressed lookup-tables\n";
      std::cout<<"=L===SSID=sector===first=npatt=nbyte==========data=====================\n";
      for(int ipl=0;ipl<getNPlanes();ipl++) {
         for(MAP<int,int>::const_iterator iSSID=usedSSID[ipl].begin();
             iSSID!=usedSSID[ipl].end();iSSID++) {
            PatternBySectorSSidMap_t::const_iterator jSSID=
               m_bank.m_PatternByLayer[ipl].m_SSidData.find((*iSSID).first);
            if(jSSID!=m_bank.m_PatternByLayer[ipl].m_SSidData.end()) {
               PatternBySector_t::const_iterator jSector=
                  (*jSSID).second.find((*iSSID).second);
               if(jSector != (*jSSID).second.end()) {
                  std::cout<<" "<<ipl<<std::setw(7)<<(*iSSID).first
                           <<std::setw(6)<<sector
                           <<std::setw(8)<<(*jSector).second.m_FirstPattern
                           <<std::setw(6)<<(*jSector).second.m_NPattern
                           <<std::setw(6)<<(*jSector).second.m_length
                           <<" ["<<std::setbase(16);
                  if((*jSector).second.m_length<6) {
                     for(unsigned k=0;k<(*jSector).second.m_length;k++) {
                        std::cout
                           <<" 0x"<<(int)m_bank.m_PatternByLayer[ipl].
                           m_CompressedDeltaBySector[(*jSector).first]
                           [(*jSector).second.m_offset+k];
                     }
                  } else {
                     for(int k=0;k<3;k++) {
                        std::cout
                           <<" 0x"<<(int)m_bank.m_PatternByLayer[ipl].
                           m_CompressedDeltaBySector[(*jSector).first]
                           [(*jSector).second.m_offset+k];
                     }
                     std::cout<<" ...";
                     for(unsigned k=(*jSector).second.m_length-3;
                         k<(*jSector).second.m_length;k++) {
                        std::cout
                           <<" 0x"<<(int)m_bank.m_PatternByLayer[ipl].
                           m_CompressedDeltaBySector[(*jSector).first]
                           [(*jSector).second.m_offset+k];
                     }
                  }
                  std::cout<<std::setbase(10)<<" ]\n";
               }
            }
            
         }
      }
   } else {
      std::cout<<"no data for sector="<<sector<<"\n";
   }
}

bool FTK_CompressedAMBank::isReservedPatternId(int patternID) const {
   return !(patternID & 0x1ffff);
}
