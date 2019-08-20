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

#define QSORT_FOR_DC_IMPORT

#define PRINT_ROADS_SECTORID -1
#define PRINT_ROADSUMMARY_NEVENT 2
//#define PRINT_DETAILS_NEVENT 2
// PRINT_SS is per plane
// #define PRINT_SS 8
//#define PRINT_SS_DETAIL
// #define WRITE_DEADMODULE_TEMPLATE
// #define TESTDCSSID 19
//#define DEBUG_EVENT 88

//#define DEBUG

#define HW2_USE_TSPMAP

// #define SEARCH_MEMORY_LEAK

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

#ifdef ROAD_CAND_AS_VECTOR
int const FTK_CompressedAMBank::MAX_ROADCAND=300000;
#endif

int const FTK_CompressedAMBank::s_WILDCARDid=-1;
int const FTK_CompressedAMBank::s_INVALIDid=-2;

#ifdef DEBUG_EVENT
static int g_event=0;
#endif

//using namespace std;

FTK_CompressedAMBank::~FTK_CompressedAMBank() {
   if (m_TSPmap) delete m_TSPmap;
}

FTK_CompressedAMBank::FTK_CompressedAMBank
(int bankID,int subID,FTKSSMap *ssMap,FTKSSMap *ssMapTSP,
 int hwmodeid_TSP,int hwmodeid_DC,char const *name)
   : FTKLogging(name),FTK_AMsimulation_base(bankID,subID),
     m_nhWCmin(0),
     m_MAX_MISSING_PLANES(0),
     m_SSmapTSP(ssMapTSP),
     m_hwmodeIDtsp(hwmodeid_TSP),
     m_hwmodeIDdc(hwmodeid_DC),
     m_nDCmax(-1),m_wildcardPenalty(0),
     m_compressionScheme(FTK_CompressedAMBank::COMPRESSION_DEFAULT) {
   setSSMap(ssMap);
   if(getSSMap()&& getSSMapTSP()) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   } else {
      m_TSPmap=0;
   }
   Info("CompressedAMBank")
      <<"hwmodeIDtsp="<<m_hwmodeIDtsp
      <<" hwmodeIDdc="<<m_hwmodeIDdc
      <<" initial compressionScheme="<<m_compressionScheme
      <<"\n";
}

void FTK_CompressedAMBank::setCompressionScheme(int scheme) {
   m_compressionScheme=scheme;
   Info("setCompressionScheme")
      <<"scheme for next bank import is "<<getCompressionScheme()<<"\n"; 
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
   m_dcMaskLookup.resize(getNPlanes());
   m_hbMaskLookup.resize(getNPlanes());
   for(int ipl=0;ipl<getNPlanes();ipl++) {
      int nBitsX=m_TSPmap->getNBits(ipl,0);
      int nBitsY=0;
      if(m_TSPmap->getDim(ipl)==2) {
         nBitsY=m_TSPmap->getNBits(ipl,1);
      }
      unsigned n2= 1<<(2*(nBitsX+nBitsY));
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
   // loop over sectors, order by first pattern
   MAP<int,size_t> sectorByPatternID;
   for(size_t sector=0;sector<m_bank.m_PatternRangeBySector.size();sector++) {
      Range const &range=m_bank.m_PatternRangeBySector[sector];
      // skip empty sectors
      if(!range.Size()) continue;
      sectorByPatternID[range.First()]=sector;
   }
   int isub=getSubID();
   VECTOR<int> sectorWritten;
   for(MAP<int,size_t>::const_iterator ptr=sectorByPatternID.begin();
       ptr!=sectorByPatternID.end();ptr++) {
      size_t sector=(*ptr).second;
      Range const &range=m_bank.m_PatternRangeBySector[sector];
      // skip empty sectors -- not needed
      //    if(!range.Size()) continue;
      sectorID=sector;
      if(nsub && (!flatFormat) &&((sectorID %nsub)!=isub)) continue;
      if(!((sectorID-isub)%100)) {
         Info("writePCachedBankFile")<<"sector="<<sector<<"\n";
      }
      sectorWritten.push_back(sector);
      int firstPattern=range.First();
      int lastPattern=range.Last();
      // loop over all patterns in the sector
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
            // SSID index is converted to SSID using lookup-table
            int dcSSID=m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
               getSSID(sector,ipattern);
            std::pair<uint16_t,uint16_t> dchb=
               m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
               getDCHB(ipattern);
            hbmask = (hbmask<<nDCbits)|dchb.second;
            dcmask = (dcmask<<nDCbits)|dchb.first;
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
      int sector,firstPattern,lastPattern,isub,lamb,allPatternsUsed;
      int maxNumPatterns;
      auxtree->Branch("nplanes",&nplanes,"nplanes/I");
      auxtree->Branch("ndc",ndc,"ndc[nplanes]/I");
      auxtree->Branch("sector",&sector,"sector/I");
      auxtree->Branch("first",&firstPattern,"first/I");
      auxtree->Branch("last",&lastPattern,"last/I");
      auxtree->Branch("lamb",&lamb,"lamb/I");
      auxtree->Branch("isub",&isub,"isub/I");
      auxtree->Branch("wildcard",&wildcard,"wildcard/I");
      auxtree->Branch("allPatternsUsed",&allPatternsUsed,"allPatternsUsed/I");
      if(m_maxPatterns.size()) {
         // store maximum number of patterns if it was calculated
         auxtree->Branch("maxNumPatterns",&maxNumPatterns,"maxNumPatterns/I");
      }
      //auxtree->Branch("lowestSSID",&lowestSSID,"lowestSSID/I");
      // set number of DC bits per plane (constant)
      for(int i=0;i<getNPlanes();i++) {
         ndc[i]=m_TSPmap->getNBits(i  );
      }
      for(size_t i=0;i<sectorWritten.size();i++) {
         sector=sectorWritten[i];
         Range const &range=m_bank.m_PatternRangeBySector[sector];
         firstPattern=range.First();
         lastPattern=range.Last();
         isub= nsub ? (sector % nsub) : 0;
         lamb= nlamb ? (sector % nlamb) : 0;
         wildcard=m_SectorWC[sector];
         MAP<int,bool>::const_iterator tooFew=m_tooFew.find(sector);
         allPatternsUsed=-1;
         if(tooFew!=m_tooFew.end()) {
            allPatternsUsed=(*tooFew).second ? 1 : 0;
         }
         MAP<int,int>::const_iterator maxPattPtr=m_maxPatterns.find(sector);
         maxNumPatterns=-1;
         if(maxPattPtr!=m_maxPatterns.end()) {
            maxNumPatterns=(*maxPattPtr).second;
         }
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
(int layer,int tspSSID) {
   MAP<int,std::pair<int,int> >::const_iterator
      i=m_TSPtoDC[layer].find(tspSSID);
   if(i==m_TSPtoDC[layer].end()) {
      insertSSID(layer,tspSSID,s_WILDCARDid);
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
(int layer,int dcSSID) {
   MAP<int,std::vector<int> >::const_iterator
      i=m_DCtoTSP[layer].find(dcSSID);
   if(i==m_DCtoTSP[layer].end()) {
      insertSSID(layer,s_WILDCARDid,dcSSID);
      i=m_DCtoTSP[layer].find(dcSSID);
   }   
   return (*i).second;
}

/**
   method to convert tspSSID (fine geometry)
   to a dcSSID (coarse geometry)
 */

int FTK_CompressedAMBank::getDCssidSlow
(int layer,int tspSSID,int *moduleID) {
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
            Fatal("getDCssidSlow")<<"no lookup-table to convert HWMODEID=2 to HWMODEID=0\n";
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
            Fatal("getDCssidSlow")<<"no lookup-table to convert HWMODEID=0 to HWMODEID=2\n";
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
(int layer,int dcSSID,int tspXY /* ,int *moduleID */) {
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
            Fatal("getTSPssidSlow")<<"no lookup-table to convert HWMODEID=2 to HWMODEID=0\n";
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
            Fatal("getTSPssidSlow")<<"no lookup-table to convert HWMODEID=0 to HWMODEID=2\n";
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
(int layer,int tspSSID,int dcSSID0) {
   if(!m_TSPmap) {
      m_TSPmap = new TSPMap(getSSMap(),getSSMapTSP()); // TSPLevel
   }
   // here:
   //  (a) (tspSSID==s_INVALIDid)||(dcSSID0==k_INVALIDid)
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
         dcSSID=getDCssidSlow(layer,tspSSID,0);
      } else {
         dcSSID=s_INVALIDid;
      }
   }
   // at this point:
   //  (a1) tspSSID==s_INVALIDid  && dcSSID==k_INVALIDid
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
         tsp[iTSP]=getTSPssidSlow(layer,dcSSID,iTSP);
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
      int dcSSID1=getDCssidSlow(layer,tspSSID,&moduleID);
      if((!match)||(dcSSID1!=dcSSID)) {
         std::ostream &fatal=Fatal("insertSSID");
         fatal<<"SSID inconsistency: layer="<<layer
              <<" TSP0="<<tspSSID
              <<" (DC="<<dcSSID1<<")"
              <<" DC="<<dcSSID<<" TSP=[";
         for(int i=0;i<nPos;i++) {
            fatal<<" "<<tsp[i]<<" (DC="<<getDCssidSlow(layer,tsp[i],0)<<")";
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
         if(pattern) {
            sectorID=pattern->getSectorID();
         }
         int &pos=sectorPointer[sectorID].second;
         if(!pattern) {
            // flat file
            // determine position in ssidData array
            if(pos>=offsetSSID*nPattern) {
               Fatal("readPCachedBank")
                  <<"overflow sector="<<pattern->getSectorID()
                  <<" pos="<<pos<<" ipattern="<<iPattern
                  <<"\n";
            }
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
               Info("readPCachedBank")
                  <<iPattern<<" sector="<<pattern->getSectorID()<<"\n";
            // NOTE: pos is a reference, so increasing pos (below)
            //   automatically selects the proper position for reading
            //   the next pattern in this sector
            if(pos>=offsetSSID*nPattern) {
               Fatal("readPCachedBank")
                  <<"overflow sector="<<pattern->getSectorID()
                  <<" pos="<<pos<<" ipattern="<<iPattern
                  <<"\n";
            }
            for(int i=0;i<nLayer;i++) {
               // SSID layer 0..N-1
               ssidData[pos++]=pattern->getSSID(i);
            }
            ssidData[pos++]=pattern->getDCMask();
            ssidData[pos++]=pattern->getHBMask();
         }
         // skip invalid pattern
         int nValid=0;
         for(int i=0;i<nLayer;i++) {
            if(ssidData[pos+i-offsetSSID]!=s_WILDCARDid) nValid++;
         }
         if(!nValid) {
            pos -= offsetSSID;
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
      Info("readPCachedBank")<<"importing patterns\n";
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
   m_bank.m_PatternByLayer.resize(0);
   m_bank.m_PatternByLayer.resize(nLayer);

   int nPatternEstimate=0;
   for(size_t sector=0;sector<sectorPointer.size();sector++) {
      int nPattern=(sectorPointer[sector].second-
                    sectorPointer[sector].first)/offsetSSID;
      nPatternEstimate += nPattern;
   }
   nPatternEstimate=(nPatternEstimate + (nPatternEstimate>>17) ) & 0xfffe0000;
   m_bank.m_numTSP.reserve(nPatternEstimate);
   m_bank.m_coverage.reserve(nPatternEstimate);

   // subregion (LAMB) loop
   if(nlamb<=0) {
      // 2^17 per AM chip
      // 2^4 AM chips per LAMB 
      // 4 LAMB per AUX
      int nAUX=(nPatternEstimate+(1<<(17+4+2))-1)>>(17+4+2);
      nlamb=nAUX<<2;
      Info("importDCpatterns")
         <<"nPattern= "<<nPatternEstimate
         <<" nAUX="<<nAUX<<", setting n(LAMB) to "<<nlamb<<"\n";
   }
   for(int ilamb=0;ilamb<nlamb;ilamb++) {
      Info("importDCpatterns")<<"LAMB="<<ilamb<<"\n";
      // sector loop starts here
      unsigned max_sectorByNlamb=(sectorPointer.size()-1)/nlamb;
      unsigned top_sectorByNlamb=0;
      while(top_sectorByNlamb+1 <= max_sectorByNlamb)
         top_sectorByNlamb=(top_sectorByNlamb+1)*2-1;
      // here: top_sectorByLamb has all bits set to one
      //   and is larger or equal the largest sector/nlamb
      unsigned leadingBitMask=(top_sectorByNlamb>>1)+1;


      // in the following loop over sectors, the variables are:
      //     sectorByLamb : sector number/nlamb
      //     sector : sector number
      //     count : counter
      //  sectorByLamb has the bit-order reversed as compared to count
      unsigned sectorByLamb=0;
      for(unsigned count=0;count<=top_sectorByNlamb;count++) {
         unsigned sector=sectorByLamb*nlamb+ilamb;
         // count sectorByLamb from zero to top_sectorByNlamb
         // bits are counted in reverse order: MSB first, then next, etc:
         //  e.g. if top_sectorByNlamb has 12 bit:
         //   0x000, 0x800, 0x400, 0xc00, 0x200, 0xa00,
         //     0x600, 0xe00, 0x100, ... 0xfff
         // This randomizes the sector ordering, for better
         //   load-balancing of patterns in the hardware
         for(unsigned mask=leadingBitMask;mask;mask>>=1) {
            // count the bit indicated in "mask"
            sectorByLamb^=mask;
            // if the it is set, we are done
            if(sectorByLamb & mask) break;
            // if the bit is zero, have to count the next bit
         }
         // when counting in reverse bit order, sectors are not produced
         //  in their natural sequence
         // high (invalid) sector numbers appear in random order and have
         // to be skipped

         // skip sector numbers outside range
         if(sector>=sectorPointer.size()) continue;

         int nPattern=(sectorPointer[sector].second-
                       sectorPointer[sector].first)/offsetSSID;
         if(!nPattern) continue;
         // sort patterns in this sector
#ifdef QSORT_FOR_DC_IMPORT
         int *patternDataSectorNC=ssidData+sectorPointer[sector].first;
         qsort(patternDataSectorNC,nPattern,sizeof(int)*offsetSSID,
               ssidCompare);
         int const *patternDataSector(patternDataSectorNC);
#else
         int const *patternDataSector(ssidData+sectorPointer[sector].first);
#endif
         //
         // loop over layers
         // inside, loop over patterns
         // at the end of the loop, ipatt points to
         //   the first unused pattern number
         uint32_t ipatt=ipattSector;
         int layerShift=0;
         for(int iLayer=0;iLayer<nLayer;iLayer++) {
            // save compressed delta data in temporary structure
            MAP<int,std::set<uint32_t> > patternSetBySSID;
            //
            // ternary bit decoding information
            int nBitsX=m_TSPmap->getNBits(iLayer,0);
            int nBitsY=0;
            int ndim=m_TSPmap->getDim(iLayer);
            if(ndim!=1) {
               nBitsY=m_TSPmap->getNBits(iLayer,1);
            }
            int maskX=(1<<nBitsX)-1;
            int maskY=(1<<nBitsY)-1;
            //
            // first pattern this sector
            ipatt=ipattSector;
            // 
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
               // skip this pattern if all IDs are invalid
               int invalid=0;
               for(int jLayer=0;jLayer<nLayer;jLayer++){
                  if((patternData[jLayer]==s_WILDCARDid)||
                     (patternData[jLayer]==s_INVALIDid)) invalid++;
               }
               if(invalid==nLayer) continue;
               // decode SSIDs
               // a single DC pattern may expand into several TSPssids
               // DC information for this pattern
               uint32_t dcMaskLayer=patternData[nLayer]>>layerShift;
               uint32_t hbMaskLayer=patternData[nLayer+1]>>layerShift;
               //  decode DC information
               int dcx=dcMaskLayer & maskX;
               dcMaskLayer>>=nBitsX;
               int dcy=dcMaskLayer & maskY;
               int hbx=hbMaskLayer & maskX;
               hbMaskLayer>>=nBitsX;
               int hby=hbMaskLayer & maskY;
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
                     (iLayer,ssid,tspIndexes[iTSP]);
                  // inner loop
                  patternSetBySSID[tspid].insert(ipatt);
               }
               ipatt++;
            }
            // here, all SSIDs in this layer,sector have been processed
            // transfer data from compression helper to final data vector
            for(MAP<int,std::set<uint32_t> >::const_iterator iSSID=
                   patternSetBySSID.begin();iSSID!=patternSetBySSID.end();
                iSSID++) {
               int tspid=(*iSSID).first;
               m_bank.m_PatternByLayer[iLayer].m_SSidData[tspid].
                  importPatternSet(sector,(*iSSID).second,
                                   getCompressionScheme());
            }

            // update ternary bit shift for next layer
            layerShift += nBitsX+nBitsY;
         }
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
         // update pattern number for next sector
         ipattSector=ipatt;
         if((!(sector%100))||
            (sector<10)||((sector<100)&&(!(sector%10)))) {
            Debug("importDCpatterns")
               <<"sector "<<sector<<" nPatt="<<ipatt<<"\n";
         }
         if(!sectorByLamb) break; // all bits were counted, stop
      } // end loop over sectors
   } // end loop over subregions
}

void FTK_CompressedAMBank::erase(void) {
   m_bank.m_PatternByLayer.resize(0);
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
#ifdef SEARCH_MEMORY_LEAK
   printVmemUsage("readBankPostprocessing start");
#endif

   readWildcards();

   m_bank.m_PatternRangeBySector.resize(0);
   m_bank.m_PatternRangeBySector.reserve(16384);

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

   //
   // pack data into VECTORMAP structures

   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      layerData.m_SSidData.pack();
   }

   Info(where)<<"repacking pattern bank, compression scheme="
              <<getCompressionScheme()<<"\n";

   // pack and compress pattern 
   if(getCompressionScheme()!=COMPRESSION_SKIP) {
      for(size_t iLayer=0;iLayer<m_bank.m_PatternByLayer.size();iLayer++) {
         for(PatternBySectorSSidMap_t::iterator iSSID=
                m_bank.m_PatternByLayer[iLayer].m_SSidData.begin();
             iSSID!=m_bank.m_PatternByLayer[iLayer].m_SSidData.end();iSSID++) {
            //std::cout<<iLayer<<"/"<<(*iSSID).first<<" start size: "
            //         <<(*iSSID).second.getMemoryEstimate()<<"\n";
            (*iSSID).second.finalize(getCompressionScheme());
            //std::cout<<iLayer<<"/"<<(*iSSID).first<<" end size: "
            //         <<(*iSSID).second.getMemoryEstimate()<<"\n";
         }
      }
   }
   // report encoding schemes actually used
   for(size_t iLayer=0;iLayer<m_bank.m_PatternByLayer.size();iLayer++) {
      MAP<uint32_t,uint32_t> encodingScheme;
      for(PatternBySectorSSidMap_t::const_iterator iSSID=
             m_bank.m_PatternByLayer[iLayer].m_SSidData.begin();
          iSSID!=m_bank.m_PatternByLayer[iLayer].m_SSidData.end();iSSID++) {
         for(FTK_CompressedSectorPatternList::const_iterator iSector=
                (*iSSID).second.begin();iSector!=(*iSSID).second.end();
             iSector++) {
            encodingScheme[(*iSector).second.getEncoding()]++;
         }
      }
      std::ostream &out=(Info(where)<<"L"<<iLayer<<" encoding");
      for(MAP<uint32_t,uint32_t>::const_iterator ienc=encodingScheme.begin();
          ienc!=encodingScheme.end();ienc++) {
         out<<" ["<<(*ienc).first<<"]="<<(*ienc).second;
      }
      out<<"\n";
   }

   // determine pattern range per sector
   // map of DC SSID per layer and sector 
   VECTOR<VECTOR<MAP<int,int> > >
      encodedDCSSIDbySectorLayer(m_bank.m_PatternByLayer.size());

   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      VECTOR<MAP<int,int> > &encodedDCSSIDBySector=
         encodedDCSSIDbySectorLayer[layer];
      encodedDCSSIDBySector.reserve(16384);
      for(PatternBySectorSSidMap_t::iterator iSSID=
             layerData.m_SSidData.begin();
          iSSID!=layerData.m_SSidData.end();++iSSID) {
         int ssid=(*iSSID).first;
         FTK_CompressedSectorPatternList const &patternList((*iSSID).second);

         patternTSPcount[layer]+= patternList.getNPattern();
         uint32_t maxSector=patternList.maxSector();
         while(m_bank.m_PatternRangeBySector.size()<=maxSector) {
            m_bank.m_PatternRangeBySector.push_back(Range());
         }
         if(maxSector>=encodedDCSSIDBySector.size()) {
            encodedDCSSIDBySector.resize(maxSector+1);
         }

         int dcSSID=getDCssid(layer,ssid).first;

         // determine min and max pattern id per sector
         // initialize table of encoded DC-SSID by layer,sector,DCssid
         Range all;
         for(FTK_CompressedSectorPatternList::const_iterator iSector=
                patternList.begin();iSector!=patternList.end();iSector++) { 
            uint32_t sector=(*iSector).first;
            FTK_CompressedPatternList const &plist=(*iSector).second;
            Range &range(m_bank.m_PatternRangeBySector[sector]);
            std::pair<uint32_t,uint32_t> minMax=plist.getMinMaxPattern();
            range.UpdateRange(minMax.first,minMax.second);
            all.UpdateRange(minMax.first,minMax.second);
            if(encodedDCSSIDBySector[sector].find(dcSSID)==
               encodedDCSSIDBySector[sector].end()) {
               int n=encodedDCSSIDBySector[sector].size();
               encodedDCSSIDBySector[sector][dcSSID]=n;
            }
         }
         // determine maximum pattern number
         if(m_npatterns<all.Last()) m_npatterns=all.Last();
      }
   }

   // estimate memory consumption
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData const &layerData=m_bank.m_PatternByLayer[layer];
      ssidCountTotal += layerData.m_SSidData.size();
      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         sectorCountTotal += (*ssid).second.size();
         memorySSIDlookup+= (*ssid).second.getMemoryEstimate();

      }
      memorySSIDlookup += sizeof(LayerData)
         +layerData.m_SSidData.getMemoryEstimate();
   }
   
   // number of patterns = maximum patternID+1
   m_npatterns++;
   m_hitMask.setNPattern(m_npatterns);
#ifdef ROAD_CAND_AS_VECTOR
   m_roadCand.resize(0);
   m_roadCand.reserve(MAX_ROADCAND);
#else
   m_roadCandMap.clear();
#endif
   //
   // determine minimum and maximum number of patterns per sector
   Range patternsPerSector;
   for(size_t sector=0;sector<m_bank.m_PatternRangeBySector.size();sector++) {
      Range const &range=m_bank.m_PatternRangeBySector[sector];
      int nPattern=range.Size();
      if(!nPattern) continue;
      patternsPerSector.UpdateRange(nPattern,nPattern);
   }
   // determine maximum sector ID
   int maxSector=m_bank.m_PatternRangeBySector.size();

   // store DC <-> encoded DC translation table per layer/sector
   //   and set up translation tables
   for(unsigned layer=0;layer<encodedDCSSIDbySectorLayer.size();
       layer++) {
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      VECTOR<MAP<int,int> > const &encodedDCSSIDBySector=
         encodedDCSSIDbySectorLayer[layer];
      layerData.m_SSIDternaryByPattern.m_dcSSIDbySectorIndex.resize(maxSector);
      uint32_t maxNDC=0;
      //
      // sector loop
      for(uint32_t sector=0;sector<encodedDCSSIDBySector.size();
          sector++) {
         MAP<int,int> const &dcSSID(encodedDCSSIDBySector[sector]);
         //
         // number of distinct SSIDs in this layer/sector
         size_t nDCSSID=dcSSID.size();
         if(!nDCSSID) continue;
         VECTOR<int> &dcSSIDtable= layerData.
            m_SSIDternaryByPattern.m_dcSSIDbySectorIndex[sector];
         dcSSIDtable.reserve(nDCSSID);
         dcSSIDtable.resize(nDCSSID);
         memoryPatternIDlookup += sizeof(int)*dcSSIDtable.size();
         //
         // copy SSID indexes to data structure
         for(MAP<int,int>::const_iterator ssid=dcSSID.begin();
             ssid!=dcSSID.end();ssid++) {
            // set up table to decode index->SSID
            if(((unsigned)(*ssid).second)>=dcSSIDtable.size()) {
               Fatal(where)<<"overflow when storing DC-SSID translation table\n";
            }
            dcSSIDtable[(*ssid).second]= (*ssid).first;
         }
         // maximum index in this layer
         if(dcSSIDtable.size()>maxNDC) maxNDC=dcSSIDtable.size();
      }
      // estimate memory consumption for vector 
      memoryPatternIDlookup += sizeof(std::vector<int>)*maxSector*getNPlanes();

      // determine nunber of bits needed
      uint32_t offsetTernary=1;
      for(int nTernary=m_TSPmap->getNBits(layer);nTernary;nTernary--) {
         offsetTernary *=3;
      }
      layerData.m_SSIDternaryByPattern.m_offsetTernary=offsetTernary;
      uint32_t encodedSize=maxNDC*offsetTernary+1;
      int nbit;
      for(nbit=0;encodedSize>(1U<<nbit);nbit++);
      //
      // add space for ternary bits
      Info(where)<<"layer="<<layer<<" #dcSSID="<<maxNDC
                 <<" DC bits="<< m_TSPmap->getNBits(layer)
                 <<" encodedSize="<<encodedSize
                 <<" nbit="<<nbit<<"\n";
      // pattern bank data is stored in 8-bit or 16-bit words per layer
      if(nbit<=8) {
         layerData.m_SSIDternaryByPattern.m_pattern8DataPacked.resize
            (m_npatterns);
         memoryPatternIDlookup += sizeof(uint8_t)*m_npatterns;
      } else {
         layerData.m_SSIDternaryByPattern.m_pattern16DataPacked.resize
            (m_npatterns);
         memoryPatternIDlookup +=  sizeof(uint16_t)*m_npatterns;
      }
   }

   //
   // fill bank indexed by patternID
#ifdef SEARCH_MEMORY_LEAK
   printVmemUsage("readBankPostprocessing fill address-indexed bank");
#endif

   // layer loop
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      //
      // encoding table dcIndexTable[sector][SSID] for the given layer
      VECTOR<MAP<int,int> >
         const &encodedDCSSIDBySector(encodedDCSSIDbySectorLayer[layer]);
      //
      // number of DC bits in X,Y
      int nBitsX=m_TSPmap->getNBits(layer,0);
      int nBitsY=0;
      if(m_TSPmap->getDim(layer)==2) {
         nBitsY=m_TSPmap->getNBits(layer,1);
      }
      // total number of ternary bits in this layer
      int ternaryBitsLayer=nBitsX+nBitsY;
      // total number of "normal" (HB) bit combinations
      int nHB=1<<ternaryBitsLayer;
      // lookup-table to merge HB bit of a given state
      //   with existing ternary bits
      int ternaryMax=layerData.m_SSIDternaryByPattern.m_offsetTernary;
      VECTOR<VECTOR<uint16_t> > ternaryLookupTERNHB(nHB);
      for(int iHB=0;iHB<nHB;iHB++) {
         VECTOR<uint16_t> &ternaryLookup(ternaryLookupTERNHB[iHB]);
         // lookup-table to merge HB bit of a given state
         //   with existing ternary bits
         ternaryLookup.reserve(ternaryMax+1);
         // at index [0] store the HB "as is", but encoded in ternary logic
         // this is used in case no previous pattern was filled
         int iHBtern=0;
         for(int bit=ternaryBitsLayer-1;bit>=0;bit--) {
            iHBtern*=3;
            if(iHB & (1<<bit)) {
               iHBtern +=1;
            }
         }
         ternaryLookup.push_back(iHBtern);
         for(int iTERN=0;iTERN<ternaryMax;iTERN++) {
            // iTERN: initial ternary bit setting
            // iHB: initial HB setting
            int ternaryResult=0;
            int offset=ternaryMax/3;
            for(int bit=ternaryBitsLayer-1;bit>=0;bit--) {
               ternaryResult*=3;
               int ternaryBit=(iTERN/offset)%3;
               offset/=3;
               if(ternaryBit != ((iHB>>bit)&1)) ternaryBit=2;
               ternaryResult +=ternaryBit;
            }
            ternaryLookup.push_back(ternaryResult);
         }
      }
      //
      // loop over all SSIDs in this layer (in TSP space)
      for(PatternBySectorSSidMap_t::const_ptr iSSID=
             layerData.m_SSidData.beginPtr();
          iSSID!=layerData.m_SSidData.endPtr();++iSSID) {

         // get DC-level SSID
         //    dcSSIDxy.first  : SSID in DC-space
         //    dcSSIDxy.second : xy coordinate of TSP relative to DC
         //
         std::pair<int,int> dcSSIDxyNOGC= getDCssid(layer,(*iSSID).first);
         // apply grey code for X,Y coordinates
         // mask out xy overlap bit
         uint32_t overlapXY=1<<nBitsX;
         // grey coded HB part of SSID
         uint32_t HBwithGC =
            dcSSIDxyNOGC.second ^ ((dcSSIDxyNOGC.second & ~overlapXY)>>1);

         // loopup-table to merge this dcSSID with an existing entry
         VECTOR<uint16_t> lookupTERN(ternaryLookupTERNHB[HBwithGC]);
         //
         // class to loop over all sectors and patterns
         class DCBitExtractor
            : public FTK_CompressedPatternListLoopWithCounter_base {
         public:
            inline DCBitExtractor
            (int dcSSID,
             VECTOR<MAP<int,int> > const &encodedDCSSIDbySector,
             VECTOR<uint16_t> const &lookupTERNARY,LayerData &layerData,uint32_t hb)
               : m_dcSSID(dcSSID),
                 m_encodedDCSSIDbySector(encodedDCSSIDbySector),
                 m_lookupTERNARY(lookupTERNARY),m_layerData(layerData) { m_hb=hb; }
            inline bool setSector(uint32_t sector) {
               // determine DC part of the encoded pattern data,
               //    with zeroed ternary bits
               m_ssidIndex=-1;
               MAP<int,int> const &encodedDCSSID
                  (m_encodedDCSSIDbySector[sector]);
               MAP<int,int>::const_iterator iSSID=encodedDCSSID.find(m_dcSSID);
               if(iSSID!=encodedDCSSID.end()) {
                  m_ssidIndex=(*iSSID).second*
                     m_layerData.m_SSIDternaryByPattern.m_offsetTernary
                     +1;
                  m_dataMax=m_layerData.m_SSIDternaryByPattern.
                     m_dcSSIDbySectorIndex[sector].size()*
                     m_layerData.m_SSIDternaryByPattern.m_offsetTernary  +1;
                  if(m_ssidIndex>=m_dataMax) {
                     std::cout<<"DCbitExractor SSID,sector "<<m_dcSSID<<" "<<sector
                              <<" overflow "<<m_layerData.m_SSIDternaryByPattern.
                        m_dcSSIDbySectorIndex[sector].size()
                              <<" "<<m_ssidIndex<<">="<<m_dataMax
                              <<"\n";
                     exit(73);
                  }
                  return true;
               }
               std::cout<<"DCbitExractor SSID,sector "<<m_dcSSID<<" "<<sector<<" not found\n";
               exit(73);
               return false;
            }
            inline void process(void) {
               // merge current bits with previously stored ternary bits
               if(m_layerData.m_SSIDternaryByPattern.m_pattern8DataPacked.size()) {
                  uint8_t &data= m_layerData.m_SSIDternaryByPattern.
                     m_pattern8DataPacked[getPattern()];
                  if(data) {
                     data = (data-1)%m_layerData.m_SSIDternaryByPattern.
                        m_offsetTernary+1;
                  }
                  data = m_lookupTERNARY[data] + m_ssidIndex;
               } else {
                  uint16_t &data= m_layerData.m_SSIDternaryByPattern.
                     m_pattern16DataPacked[getPattern()];
                  if(data) {
                     data = (data-1)%m_layerData.m_SSIDternaryByPattern.
                        m_offsetTernary+1;
                  }
                  data = m_lookupTERNARY[data] + m_ssidIndex;
               }
            }
         protected:
            uint32_t m_hb;
            uint32_t m_dataMax;
            uint16_t m_ssidIndex;
            int m_dcSSID;
            VECTOR<MAP<int,int> > const &m_encodedDCSSIDbySector;
            VECTOR<uint16_t> const &m_lookupTERNARY;
            LayerData &m_layerData;
         };
         //
         // loop over all patterns
         DCBitExtractor extractor
            (dcSSIDxyNOGC.first,encodedDCSSIDBySector,lookupTERN,layerData,HBwithGC);
         (*iSSID).second.patternLoop(extractor);
      }
   }
   // count number of encoded patterns
   uint64_t nTSPpatterns=0;
   for(int iPattern=0;iPattern<m_npatterns;iPattern++) {
      int nTSP=1;
      for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
         int encodedSSIDwithDC=m_bank.m_PatternByLayer[layer].
            m_SSIDternaryByPattern.getPackedData(iPattern);
         if(encodedSSIDwithDC==0) {
            if(!isReservedPatternId(iPattern)) {
               Fatal(where)<<"invalid pattern "<<iPattern<<"\n";
            }
            nTSP=0;
         } else {
            // extract DC bits
            std::pair<uint16_t,uint16_t> dchb=m_bank.m_PatternByLayer[layer].
               m_SSIDternaryByPattern.getDCHB(iPattern);
            // each DC bit doubles the number of TSP patterns
            //  (m_nHit16 counts the number of bits)
            nTSP <<= m_nHit16[dchb.first];
         }
      }
      nTSPpatterns += nTSP;
   }

   double patternTSPcountTotal=0.;
   for(unsigned i=0;i<patternTSPcount.size();i++)
      patternTSPcountTotal+= patternTSPcount[i];

#ifdef SEARCH_MEMORY_LEAK
   printVmemUsage("readBankPostprocessing printout");
#endif

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
      <<" min="<<patternsPerSector.First()<<" max="<<patternsPerSector.Last()<<"\n";
   for(unsigned i=0;i<patternTSPcount.size();i++) {
      Info(where)
         <<"plane="<<i<<" number of TSP/pattern="
         << ((m_npatterns > 0 ) ? (patternTSPcount[i]/(double)m_npatterns)
             : - 1) <<"\n";

   }
   int memoryBuffers=2*sizeof(VECTOR<FTK_HitMask>)+
      (m_SectorWC.size()+m_sectorUsage.size())*sizeof(FTK_HitMask)+
      m_hitMask.memoryEstimate()
#ifdef ROAD_CAND_AS_VECTOR
      +sizeof(VECTOR<uint32_t>)
      +m_roadCand.capacity()*sizeof(uint32_t)
#endif
      ;
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
   for(unsigned iPlane=0;iPlane<m_bank.m_PatternByLayer.size();iPlane++) {
      PatternBySectorSSidMap_t const &planeData=
         m_bank.m_PatternByLayer[iPlane].m_SSidData;
      for(PatternBySectorSSidMap_t::const_iterator
             iSSid=planeData.begin();iSSid!=planeData.end();++iSSid) {
         if(((*iSSid).first!=s_WILDCARDid)&&((*iSSid).first!=s_INVALIDid)) {
            int moduleId=-1;
            getDCssidSlow(iPlane,(*iSSid).first,&moduleId);
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
   } else {
      Info(where)<<"no invalid SSIDs found in pattern bank\n";
   }

   // corruption test of sector structures
   // loop over all planes and all SSIDs
   // construct table of module IDs for each sector
   // verify that there is only one well-defined module per sector and plane
   error=0;
   for(unsigned iPlane=0;iPlane<m_bank.m_PatternByLayer.size();iPlane++) {
      VECTOR<std::set<int> > modulesBySector(maxSector);

      PatternBySectorSSidMap_t const &planeData=
         m_bank.m_PatternByLayer[iPlane].m_SSidData;

      for(PatternBySectorSSidMap_t::const_iterator
             iSSid=planeData.begin();iSSid!=planeData.end();++iSSid) {
         int module=-1;
         int ssid=(*iSSid).first;
         if((ssid!=s_WILDCARDid)&&ssid!=s_INVALIDid) {
            getDCssidSlow(iPlane,ssid,&module);
         }
         for(FTK_CompressedSectorPatternList::const_iterator
                iSector=(*iSSid).second.begin();iSector!=(*iSSid).second.end();
             iSector++) {
            modulesBySector[(*iSector).first].insert(module);
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
   } else {
      Info(where)<<"Pattern bank modules per sector are consistent\n";
   }
#endif
   setupSectorWildcards();
#ifdef SEARCH_MEMORY_LEAK
   printVmemUsage("readBankPostprocessing end");
#endif
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
   uint32_t maxSector=m_bank.m_PatternRangeBySector.size();
   m_SectorWC.resize(0);
   m_SectorWC.resize(maxSector);
   VECTOR<FTK_HitMask> sectorBadList(maxSector);
   
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
            getDCssidSlow(iPlane,(*iSSid).first,&moduleId);
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
         for(FTK_CompressedSectorPatternList::const_iterator
                iSector=(*iSSid).second.begin();
             iSector!=(*iSSid).second.end();++iSector) {
            sectorFlag[(*iSector).first]|=mask;
         }
      }
      for(uint32_t sector=0;sector<maxSector;sector++) {
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
               for(FTK_CompressedSectorPatternList::const_iterator
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
                     getDCssidSlow(iPlane,(*iSSid).first,&moduleId);
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
                  for(FTK_CompressedSectorPatternList::const_iterator
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
         size_t nmax=0;
         for(FTK_CompressedSectorPatternList::const_iterator
                sector1=(*ssid1).second.begin(),
                sector2=(*ssid2).second.begin();
             (sector1!=(*ssid1).second.end())&&
                (sector2!=(*ssid2).second.end());++sector1,++sector2) {
            if(((*sector1).first!=(*sector2).first)&&(!(error & 0x1000))) {
               Info("compare")
                  <<"sector number mismatch (layer="<<layer
                  <<" ssid="<<(*ssid1).first<<") "
                  <<(*sector1).first<<"!="<<(*sector2).first<<"\n";
               error |= 0x0100;
            }
            // extract pattern data and compare
            if((*sector1).second.getNPattern()>nmax) {
               nmax=(*sector1).second.getNPattern();
            }
            if((*sector2).second.getNPattern()>nmax) {
               nmax=(*sector2).second.getNPattern();
            }
         }
         if(!error) {
            VECTOR<uint8_t> data1,data2;
            data1.reserve(nmax*4);
            data2.reserve(nmax*4);
            for(FTK_CompressedSectorPatternList::const_iterator
                sector1=(*ssid1).second.begin(),
                sector2=(*ssid2).second.begin();
             (sector1!=(*ssid1).second.end())&&
                (sector2!=(*ssid2).second.end());++sector1,++sector2) {
               if(((*sector1).second.getNPattern()!=
                   (*sector2).second.getNPattern())&&(!(error & 0x2000))) {
                  Info("compare")
                     <<"number of patterns mismatch (layer="<<layer
                     <<" ssid="<<(*ssid1).first<<" sector="
                     <<(*sector1).first<<") "<<(*sector1).second.getNPattern()
                     <<"!="<<(*sector2).second.getNPattern()<<"\n";
                  error |= 0x2000;
               }
               uint32_t first1,first2;
               (*sector1).second.encode(FTK_CompressedPatternList::ENCODING_U32,
                                        &first1,data1);
               (*sector2).second.encode(FTK_CompressedPatternList::ENCODING_U32,
                                        &first2,data2);
               if(first1!=first2) {
                  Info("compare")
                     <<"first pattern mismatch (layer="<<layer
                     <<" ssid="<<(*ssid1).first<<" sector="
                     <<(*sector1).first<<") "<<first1<<"!="<<first2<<"\n";
                  error|=0x4000;
                  break;
               }
               if(data1.size()!=data2.size()) {
                  Info("compare")
                     <<"pattern data size mismatch (layer="<<layer
                     <<" ssid="<<(*ssid1).first<<" sector="
                     <<(*sector1).first<<") "<<data1.size()<<"!="
                     <<data2.size()<<"\n";
                  error|=0x8000;
               }
               if(memcmp(data1.data(),data2.data(),data1.size())) {
                  std::ostream &log=Info("compare");
                  log<<"pattern data mismatch (layer="<<layer
                     <<" ssid="<<(*ssid1).first<<" sector="
                     <<(*sector1).first;
                  for(size_t l=0;l<data1.size();l++) {
                     if(data1[l]!=data2[l]) {
                        log<<" ["<<l<<"] "<<(int)data1[l]<<"!="<<(int)data2[l];
                     }
                  }
                  log<<")\n";
                  error|=0x10000;
               }
            }
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
#ifdef SEARCH_MEMORY_LEAK
      printVmemUsage("readCCachedBank "+name);
#endif
      LayerData &layerData=m_bank.m_PatternByLayer[layer];
      if(name.BeginsWith("Layer")) {
         TTree *tree;
         inputDir->GetObject(name,tree);
         if(tree) {
            int nSector=tree->GetEntries();
            Int_t ssidVar,sectorVar,firstPatternVar,nPatternVar;
            Int_t nDataVar;
            tree->SetBranchAddress("nData",&nDataVar);
            int dataSize=0;
            /* tree->SetBranchStatus("sector");
               tree->SetBranchStatus("nData"); */
            for(int i=0;i<nSector;i++) {
               tree->GetEntry(i);
               if(nDataVar>dataSize) dataSize=nDataVar;
            }
            std::vector<Int_t> data(dataSize);
            tree->SetBranchAddress("sector",&sectorVar);
            tree->SetBranchAddress("ssid",&ssidVar);
            tree->SetBranchAddress("firstPattern",&firstPatternVar);
            tree->SetBranchAddress("nPattern",&nPatternVar);
            tree->SetBranchAddress("data",data.data());
            /* tree->SetBranchStatus("data");
            tree->SetBranchStatus("ssid");
            tree->SetBranchStatus("firstPattern");
            tree->SetBranchStatus("nPattern"); */
            Info("readCCachedBank")<<"nSector="<<nSector
                                   <<" buffer size="<<data.size()<<"\n";
            for(int i=0;i<nSector;i++) {
               tree->GetEntry(i);
               layerData.m_SSidData[ssidVar].importData
                  (sectorVar,firstPatternVar,nPatternVar,
                   FTK_CompressedPatternList::ENCODING_DELTA,
                   nDataVar,data.data());
            }
            delete tree;
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
   int error=1;
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
   }
   return error;
}

int FTK_CompressedAMBank::writeCCachedBankFile(TDirectory *out) const {
   int error=0;
   TDirectory *back=gDirectory;
   out->cd();
   for(unsigned layer=0;layer<m_bank.m_PatternByLayer.size();layer++) {
      //std::cout<<"Write layer "<<layer<<"\n";
      LayerData const &layerData=m_bank.m_PatternByLayer[layer];
      TString treeName(TString::Format("Layer%d",layer));
      TTree *ssidTree=new TTree(treeName,treeName);
      Int_t ssidVar,sectorVar,firstPatternVar,nPatternVar,nDataVar;
      std::vector<Int_t> data(1);
      ssidTree->Branch("ssid",&ssidVar,"ssid/I");
      ssidTree->Branch("sector",&sectorVar,"sector/I");
      ssidTree->Branch("firstPattern",&firstPatternVar,"firstPattern/I");
      ssidTree->Branch("nPattern",&nPatternVar,"numPattern/I");
      ssidTree->Branch("nData",&nDataVar,"nData/I");
      ssidTree->Branch("data",data.data(),"data[nData]/I");

      for(PatternBySectorSSidMap_t::const_ptr ssid=
             layerData.m_SSidData.beginPtr();
          ssid!=layerData.m_SSidData.endPtr();++ssid) {
         ssidVar=(*ssid).first;
         //int ntotal=0;
         for(FTK_CompressedSectorPatternList::const_iterator
                sector=(*ssid).second.begin();
             sector!=(*ssid).second.end();++sector) {
            sectorVar=(*sector).first;
            Int_t encoding;
            (*sector).second.exportData
               (&firstPatternVar,&nPatternVar,&encoding,data);
            if(encoding!=FTK_CompressedPatternList::ENCODING_DELTA) {
               Fatal("writeCCachedBankFile")
                  <<"pattern encoding "<<encoding<<" not supported\n";
            }
            nDataVar=data.size();
            //ntotal+=nDataVar;
            if(nDataVar>0) ssidTree->SetBranchAddress("data",data.data());
            ssidTree->Fill();
         }
         //std::cout<<"Write SSID "<<ssidVar<<" "<<ntotal<<"\n";
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
      FTK_HitMask badModules=0;
      FTK_HitMask wildcardRequest=0;
      for(int ilayer=nLayer-1;ilayer>=0;ilayer--) {
         std::pair<int,int> const &dcSSID=getDCssid(ilayer,tsp.GetHit(ilayer));
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
         // the FTKPatternBySectorBlockReader is reading the patterns
         // sector-by-sector, in a pre-defined coverage range and
         // for a given selection of sectors (a single partition)
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
            <<(*iPartition).fNumPatternMax
            <<" maxSector="<<(*iPartition).fNumSectorMax
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
         TSPreader->Rewind();
#ifdef SEARCH_MEMORY_LEAK
         printVmemUsage("after Rewind");
#endif
         // print bank statistics
         // determine total number of patterns , patterns*coverage, <coverage>
         uint32_t totalPatterns=0;
         uint64_t totalPatternsCoverage=0;
         int nDC0=nDC;
         maxpatts=nDC0+(*iPartition).fNumPatternMax;
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
         std::set<int> loadedSectorList;
         int maxNumSector=(*iPartition).fNumSectorMax;
         if(maxNumSector<0) maxNumSector=(*iPartition).fSectorSet.size();
         // loop over coverage ranges
         // starting with coverage indicated by (*i)
         bool tooFew=true;
         int covEnd=0;
         for(std::map<int,int>::const_reverse_iterator i=coverageMap.rbegin();
             i!=coverageMap.rend();) {
            // read a minimum of one coverages
            int nAvail=0;
            int covBegin=(*i).first;
            do {
               nAvail +=(*i).second;
               covEnd=(*i).first;
               i++;
            } while((nDC+nAvail<=maxpatts)&&(i!=coverageMap.rend()));
            // read all patterns in the given coverage range
            int nTSPold=nTSP;
            int nDCold=nDC;
            for(std::set<int>::const_iterator sectorPtr=
                   (*iPartition).fSectorSet.begin();
                sectorPtr!=(*iPartition).fSectorSet.end();sectorPtr++) {
               int sector=*sectorPtr;
               if(loadedSectorList.find(sector)==loadedSectorList.end()) {
                  if((int)loadedSectorList.size()>=maxNumSector) continue;
                  loadedSectorList.insert(sector);
               }
               FTKPatternOneSector *patterns=
                  TSPreader->Read(sector,covEnd);
               if(patterns) {
                  insertPatterns(sector,patterns,maxpatts,dcPatterns,nDC,nTSP);
                  delete patterns;
               }
               if(nDC>=maxpatts) break;
            }
            Info("readSectorOrderedBank")
               <<"added coverage range "<<covBegin<<"-"<<covEnd
               <<" DC added="<<nDC-nDCold
               <<" TSP used="<<nTSP-nTSPold<<"/"<<nAvail
               <<"\n";
            if(nDC>=maxpatts){ 
               if((nTSP-nTSPold<nAvail)||(i!=coverageMap.rend())) {
                  tooFew=false;
               }
               break;
            }
            // stop if all patterns have been read
            // or if the maximum number of patterns is reached

            // for DEBUGGING - speed up pattern reading
            //   break;

         }
         for(std::set<int>::const_iterator sectorPtr=
                (*iPartition).fSectorSet.begin();
             sectorPtr!=(*iPartition).fSectorSet.end();sectorPtr++) {
            int sector=*sectorPtr;
            m_tooFew[sector]=tooFew;
         }
         Info("readSectorOrderedBank")
            <<"partition "<<partitionCount<<" number of DC patterns: "<<nDC-nDC0
            <<"/"<<maxpatts-nDC0
            <<" smallest coverage="<<covEnd
            <<" #sectors="<<loadedSectorList.size()
            <<" maxnsector="<<maxNumSector
            <<" tooFew="<<tooFew
            <<" nDC="<<nDC
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
      Info("readSectorOrderedBank")<<"numPattern="<<numPattern<<"\n";
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
   countSectorOrderedPatterns()

   read sector-ordered bank, convert to DC patterns to count maximum
   in each sector

*/
MAP<int,int> FTK_CompressedAMBank::countSectorOrderedPatterns(const char *name) {
   // read bad modules
   readWildcards();

   m_maxPatterns.clear();

   FTKRootFileChain chain;
   chain.AddFile(name);
   FTKPatternBySectorReader *TSPreader=
      FTKPatternBySectorReader::Factory(chain);
   if(TSPreader->GetNLayers()<=0) {
      Fatal("countSectorOrderedPatterns")
         <<"number of layers (reader) "<<TSPreader->GetNLayers()<<"\n";
   }
   TSPreader->CheckConsistency(getSSMapTSP(),getBankID(),getHWModeSS_tsp());
   if(!getNPlanes()) {
      setNPlanes(TSPreader->GetNLayers());
   }
   std::map<int,int> coverageMap;
   TSPreader->GetNPatternsByCoverage(coverageMap);
      uint32_t totalPatterns=0;
      uint64_t totalPatternsCoverage=0;
      for(std::map<int,int>::const_iterator j=coverageMap.begin();
          j!=coverageMap.end();j++) {
         totalPatterns += (*j).second;
         totalPatternsCoverage += (*j).first*(uint64_t)(*j).second;
      }
   Info("countSectorOrderedPatterns")
      <<"number of coverages "<<coverageMap.size()
      <<" total patterns "<<totalPatterns
      <<" patterns*coverage "<<totalPatternsCoverage
      <<"\n";
   for(int sector=TSPreader->GetFirstSector();sector>=0;
       sector=TSPreader->GetNextSector(sector)) {
      // temporarily store patterns here
      int nDC=0;
      int nTSP=0;
      int maxpatts=0;
      VECTOR<HitPatternMap_t> dcPatterns(sector+1);
      // determine total number of patterns , patterns*coverage, <coverage>
      maxpatts=totalPatterns;
      // read all patterns
      FTKPatternOneSector *patterns=TSPreader->Read(sector,0);
      if(patterns) {
         insertPatterns(sector,patterns,maxpatts,dcPatterns,nDC,nTSP);
         delete patterns;
      }
      m_maxPatterns[sector]=nDC;
      if((sector<10)||
         ((sector<100)&&(sector%10==0))||
         ((sector<1000)&&(sector%100==0))) {
         Info("countSectorOrderedPatterns")
            <<"sector "<<sector<<" nTSP "<<nTSP<<" nDC "<<nDC<<"\n";
      }
      
   }
   delete TSPreader;

   return m_maxPatterns;
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
   std::vector< std::set<int> > tsp_ss_set(getNPlanes());
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
   }
   if(error) {
      Warning("sort_hits")<<"failed to calculate SSID for "<<error<<"/"
                        <<expectedNHit<<" FTKHits\n";
   }
   for (int ipl=0;ipl<getNPlanes();++ipl) {
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
   int maxSector=m_bank.m_PatternRangeBySector.size();
   m_sectorUsage.resize(maxSector);
   memset(m_sectorUsage.data(),0,sizeof(FTK_HitMask)*m_sectorUsage.size());
   //
   // loop over all input (layer,ssid)
   // locate the (layer,ssid) in the bank
   // store pointer to the pattern data in the sector-ordered structure
   //    patternDataBySectorLayerSSID[sector][layer][ssid]
   for(int iplane=0;iplane<getNPlanes();iplane++) {
      LayerData const &bankDataThisLayer=m_bank.m_PatternByLayer[iplane];
      FTK_HitMask mask=1<<iplane;
      for(std::list<int>::const_iterator iSSID=tspSSIDfired[iplane].begin();
          iSSID!=tspSSIDfired[iplane].end();iSSID++) {
         int tsp_ssid=(*iSSID);
         // locate the SSID in the pattern bank
         PatternBySectorSSidMap_t::const_iterator ssidInBank=
            bankDataThisLayer.m_SSidData.find(tsp_ssid);
         if(ssidInBank!=bankDataThisLayer.m_SSidData.end()) {
            // tsp_ssid is present in the pattern bank
            //  loop over all its sectors and set mask
            for(FTK_CompressedSectorPatternList::const_iterator sector=
                   (*ssidInBank).second.begin();
                sector!=(*ssidInBank).second.end();++sector) {
               m_sectorUsage[(*sector).first]|=mask;
            }
         }
      }
   }
   //
   // loop over all sectors
   for(size_t sector=0;sector<m_bank.m_PatternRangeBySector.size();sector++) {
      Range const &range=m_bank.m_PatternRangeBySector[sector];
      // skip empty sectors
      if(!range.Size()) continue;
      // determine wildcard for this sector
      FTK_HitMask sectorWildcard=m_SectorWC[sector];
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
         int firstPattern=range.First();
         int lastPattern=range.Last();
         if(firstPattern<=lastPattern) {
            m_hitMask.init(firstPattern,lastPattern,sectorWildcard);
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

   class MaskUpdateBase {
   public:
      MaskUpdateBase
      (FTK_HitMaskVector &hits,VECTOR<FTK_HitMask> const &sectorUsage,
       uint8_t layer)
         : m_hits(hits),m_sectorUsage(sectorUsage),m_layer(layer) { }
      inline void update(uint32_t delta) { m_patternPtr.update(delta); }
      inline void update4(uint32_t delta4) { m_patternPtr.update4(delta4); }
      inline void init(uint32_t firstPattern) {
         m_patternPtr =m_hits.getPointer(firstPattern);
         m_patternPtr.setLayer(m_layer);
      }
   protected:
      FTK_HitMaskVector &m_hits;
      VECTOR<FTK_HitMask> const &m_sectorUsage;
      FTK_HitMask m_layer;
      FTK_HitMaskIterator m_patternPtr;
   };
//#define DEBUG_MASK_UPDATER
   class MaskUpdaterFast : public MaskUpdateBase {
      // update of hitpatterns without checking threshold
   public:
      inline MaskUpdaterFast
      (FTK_HitMaskVector &hits,VECTOR<FTK_HitMask> const &sectorUsage,
       uint8_t layer) : MaskUpdateBase(hits,sectorUsage,layer) { }
      inline bool setSector(uint32_t sector) {
         return m_sectorUsage[sector]!=0; }
      inline void process(void) { m_patternPtr.process(); }
   };
   class MaskUpdaterSlow : public MaskUpdateBase {
      // update of hitpatterns
      // if hit threshold is passed, save road candidate
   public:
      inline MaskUpdaterSlow
      (FTK_HitMaskVector &hits,VECTOR<FTK_HitMask> const &sectorUsage,
       uint8_t layer,uint8_t nhWCmin,
#ifdef ROAD_CAND_AS_VECTOR
       std::vector<std::pair<uint32_t,uint32_t> > &roadCand
#else
       VECTORMAP<uint32_t> &roadCandMap
#endif
       )
         : MaskUpdateBase(hits,sectorUsage,layer),
           m_nhWCmin(nhWCmin),
#ifdef ROAD_CAND_AS_VECTOR
           m_roadCand(roadCand)
#else
           m_roadCandMap(roadCandMap)
#endif
      {
#ifdef DEBUG_MASK_UPDATER
         m_nSector=0;
         m_nProcess=0;
         m_nProcess4=0;
         m_nPass=0;
         for(int i=0;i<4;i++) m_nPassX[i]=0;
         m_nCheck=0;
         m_nPassCheck=0;
#endif
      }
      inline bool setSector(uint32_t sector) {
#ifdef DEBUG_MASK_UPDATER
         m_nSector++;
#endif
         m_sector=sector;
         return m_sectorUsage[sector]!=0; }
      inline void process(void) {
#ifdef DEBUG_MASK_UPDATER
         m_nProcess++;
#endif
         if(m_patternPtr.process()) {
#ifdef DEBUG_MASK_UPDATER
            m_nPass++;
#endif
            checkSaveRoad(0);
         }
      }
      inline void checkSaveRoad(int offset) {
#ifdef DEBUG_MASK_UPDATER
            m_nCheck++;
#endif
         if(m_patternPtr.getNHit(offset)==m_nhWCmin) {
#ifdef DEBUG_MASK_UPDATER
            m_nPassCheck++;
#endif
#ifdef ROAD_CAND_AS_VECTOR
            if(m_roadCand.size()<m_roadCand.capacity()) {
               uint32_t pattern=m_hits.getPatternID(m_patternPtr)+offset;
               m_roadCand.push_back(std::make_pair(pattern,m_sector));
            }
#else
            uint32_t pattern=m_hits.getPatternID(m_patternPtr)+offset;
            m_roadCandMap[pattern]=m_sector;
#endif
         }
      }
#ifdef DEBUG_MASK_UPDATER
      void print(void) const {
         std::cout
            <<"MaskUpdater L"<<(int)m_layer
            <<" nSector="<<m_nSector
            <<" nProcess="<<m_nProcess
         std::cout<<" nCheck="<<m_nCheck<<" nPassCheck="<<m_nPassCheck
                  <<"\n";
      }
#endif
   protected:
#ifdef DEBUG_MASK_UPDATER
      uint32_t m_nSector,m_nProcess;
      uint32_t m_nPass;
      uint32_t m_nCheck,m_nPassCheck;
#endif
      uint32_t m_sector;
      uint8_t m_nhWCmin;
#ifdef ROAD_CAND_AS_VECTOR
      std::vector<std::pair<uint32_t,uint32_t> > &m_roadCand;
#else
      VECTORMAP<uint32_t> &m_roadCandMap;
#endif
   };
#ifdef ROAD_CAND_AS_VECTOR
   m_roadCand.resize(0);
#else
   m_roadCandMap.clear();
#endif
   for(int iplane=0;iplane<m_nhWCmin-1;++iplane) {
      LayerData const &bankDataThisLayer=m_bank.m_PatternByLayer[iplane];
      for(std::list<int>::const_iterator iSSID=tspSSIDfired[iplane].begin();
          iSSID!=tspSSIDfired[iplane].end();iSSID++) {
         int tsp_ssid=(*iSSID);
         // locate the SSID in the pattern bank
         PatternBySectorSSidMap_t::const_iterator ssidInBank=
            bankDataThisLayer.m_SSidData.find(tsp_ssid);
         if(ssidInBank!=bankDataThisLayer.m_SSidData.end()) {
            MaskUpdaterFast updater(m_hitMask,m_sectorUsage,iplane);
            (*ssidInBank).second.patternLoop(updater);
         }
      }
   }
   for(int iplane=m_nhWCmin-1;iplane<getNPlanes();++iplane) {
      LayerData const &bankDataThisLayer=m_bank.m_PatternByLayer[iplane];
#ifdef DEBUG_MASK_UPDATER
      MaskUpdaterSlow updater(m_hitMask,m_sectorUsage,iplane,
                              m_nhWCmin,
#ifdef ROAD_CAND_AS_VECTOR
                              m_roadCand
#else
                              m_roadCandMap
#endif
                              );
#endif
      for(std::list<int>::const_iterator iSSID=tspSSIDfired[iplane].begin();
          iSSID!=tspSSIDfired[iplane].end();iSSID++) {
         int tsp_ssid=(*iSSID);
         // locate the SSID in the pattern bank
         PatternBySectorSSidMap_t::const_iterator ssidInBank=
            bankDataThisLayer.m_SSidData.find(tsp_ssid);
         if(ssidInBank!=bankDataThisLayer.m_SSidData.end()) {
#ifndef DEBUG_MASK_UPDATER
            MaskUpdaterSlow updater(m_hitMask,m_sectorUsage,iplane,
                                    m_nhWCmin,
#ifdef ROAD_CAND_AS_VECTOR
                                    m_roadCand
#else
                                    m_roadCandMap
#endif
                                    );
#endif
            (*ssidInBank).second.patternLoop(updater);
         }
      }
#ifdef DEBUG_MASK_UPDATER
      updater.print();
#endif
   }
#ifdef DEBUG_MASK_UPDATER
   std::cout<<"Road Candidates: "<<m_roadCand.size()
            <<" / "<<m_roadCand.capacity()<<"\n";
   exit(86);
#endif
}

/**
   am_output()

   Output: populate m_roads
 */

#ifdef ROAD_CAND_AS_VECTOR
static int roadCandSorter(const void *a,const void *b) {
   std::pair<uint32_t,uint32_t> const *pairA=
      (std::pair<uint32_t,uint32_t> const *)(a);
   std::pair<uint32_t,uint32_t> const *pairB=
      (std::pair<uint32_t,uint32_t> const *)(b);
   if((*pairA).first<(*pairB).first) return -1;
   if((*pairA).first>(*pairB).first) return 1;
   return 0;
}
#endif

void FTK_CompressedAMBank::am_output() {
#ifdef ROAD_CAND_AS_VECTOR
   qsort(m_roadCand.data(),m_roadCand.size(),
         sizeof(std::pair<uint32_t,uint32_t>),roadCandSorter);
#endif
   const FTKPlaneMap *pmap = getSSMapTSP()->getPlaneMap();
   for(
#ifdef ROAD_CAND_AS_VECTOR
       unsigned iCand=0;iCand<m_roadCand.size();
#else
       VECTORMAP<uint32_t>::const_iterator iCand=m_roadCandMap.begin();
       iCand!=m_roadCandMap.end();
#endif
       iCand++) {
#ifdef ROAD_CAND_AS_VECTOR
      int patternID=m_roadCand[iCand].first;
      int sector=m_roadCand[iCand].second;
#else
      int patternID=(*iCand).first;
      int sector=(*iCand).second;
#endif

      FTK_HitMask hitmaskWithWC =
         m_hitMask.getPointer(patternID).getHitMask();
      FTK_HitMask hitmaskNoWC=hitmaskWithWC & ~m_SectorWC[sector];
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
            int dcmask=0,hbmask=0;
            //
            // vallidate pattern ID
            Range const &sectorRange=m_bank.m_PatternRangeBySector[sector];
            if((patternID<sectorRange.First())||(patternID>sectorRange.Last())) {
               Fatal("am_out")
                  <<"mismatch sector "<<sector
                  <<" ["<<sectorRange.First()<<","<<sectorRange.Last()<<"]"
                  <<" pattern "<<patternID<<"\n";
            }
            //
            // loop over all planes
            for (int ipl=getNPlanes()-1;ipl>=0;--ipl) {
               int nDCbits=m_TSPmap->getNBits(ipl);
               // extract DC SSID
               int dcSSID=m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.getSSID(sector,patternID);
               // extract DC and HB bits
               std::pair<uint16_t,uint16_t> dchb=m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.getDCHB(patternID);
               hbmask = (hbmask<<nDCbits)|dchb.second;
               dcmask = (dcmask<<nDCbits)|dchb.first;

               // construct sub-mask
               // this is a bit-array
               // bit number N is set if an SSID with the least-significant
               //   bits equal to N is valid, given teh DC and HB bits
               uint16_t zeroIsValid=dcmask|~hbmask;
               uint16_t oneIsValid=dcmask|hbmask;
               uint32_t subMask=1;
               int pos=1;
               for(int bit=0;bit<nDCbits;bit++) {
                  uint16_t mask=1<<bit;
                  subMask=
                     ((zeroIsValid & mask) ? subMask : 0) |
                     ((oneIsValid & mask) ? (subMask<<pos) : 0);
                  pos<<=1;
               }
               // set up road
               int roadSSID=-1;
               if(dcSSID>=0) {
#ifdef HW2_USE_TSPMAP
                  roadSSID=(dcSSID<<nDCbits)|
                     (hbmask & (~dcmask) & ((1<<nDCbits)-1));
#else
                  roadSSID=(dcSSID<<nDCbits)|
                     (hbmask & (~dcMask) & ((1<<nDCbits)-1));
#endif
               } else {
                  roadSSID=dcSSID;
               }
               road.setSSID(ipl,roadSSID);  //attachSS[ 655 -> 670]
               //
               // store subss mask
               road.setSubSSMask(ipl,subMask);
               //
               // set HLID (?)  - see FTKTSPBank::attachSS()
               road.setHLID(ipl,m_TSPmap->getNBits(ipl));
            }
            //
            // store ternary bits
            road.setDCBitmask(dcmask);
            road.setHLBitmask(hbmask);
         }
      }
   }
   // std::cout<<"Number of roads: "<<getNRoads()<<"\n";
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
      int roadError=0;
      for (int ipl=0;ipl<getNPlanes();++ipl) {
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
         for(int bits=0;bits<=bitMask;bits++) {
            // skip ternary bit combinations if HB bits are not properly set
            if( (bits & isHB) != hb) continue;
            std::unordered_map<int,FTKSS>::iterator item0 =
               m_FiredSSmap[ipl].find(nodc_ssid | bits);
            if (item0==m_FiredSSmap[ipl].end()) 
               continue; // no hits
            // map for this plane
            if(!getStoreAllSS()) {
               std::unordered_map<int,FTKSS> &imap = m_UsedSSmap[ipl];
               // if requested, find this ss is in the bank
               std::unordered_map<int,FTKSS>::iterator item =
                  imap.find(nodc_ssid|bits);
               if (item==imap.end()) { // not found
                  imap[nodc_ssid|bits] = (*item0).second;
               }
               naoClusRoadAdd(ipl,(imap[nodc_ssid|bits]).getNHits());
            }
            found++;
         }
         /*if((*iroad).getPatternID()==15441589) {
            std::cout<<"road pattern="<<(*iroad).getPatternID()
                     <<" plane="<<ipl<<" found="<<found<<"\n";
                     } */
         if((*iroad).hasHitOnLayer(ipl) && !found) {
            roadError++;
         }
         // begin debugging
         // if((*iroad).getPatternID()==2113347) error++;
         // end debugging
         dcMask>>=nDCbits;
         hbMask>>=nDCbits;
      } // end loop over the layers
      if(roadError) {
         errorSector=(*iroad).getSectorID();
         errorPattern=(*iroad).getPatternID();
         error++;
      }
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

#ifdef PRINT_ROADSUMMARY_NEVENT
  static int print=PRINT_ROADSUMMARY_NEVENT;
  if(print) {
     Info("getRoads")<<"number of roads="<<m_roads.size()<<"\n";
#ifdef PRINT_DETAILS_NEVENT
     printRoads(m_roads,PRINT_ROADS_SECTORID);
#endif
     Info("getRoads")
        <<"storeAllSS="<<getStoreAllSS()
        <<" number of strips fired"
        <<" "<< m_UsedSSmap[0].size()<<"/"<< m_FiredSSmap[0].size()
        <<" "<< m_UsedSSmap[1].size()<<"/"<< m_FiredSSmap[1].size()
        <<" "<< m_UsedSSmap[2].size()<<"/"<< m_FiredSSmap[2].size()
        <<" "<< m_UsedSSmap[3].size()<<"/"<< m_FiredSSmap[3].size()
        <<" "<< m_UsedSSmap[4].size()<<"/"<< m_FiredSSmap[4].size()
        <<" "<< m_UsedSSmap[5].size()<<"/"<< m_FiredSSmap[5].size()
        <<" "<< m_UsedSSmap[6].size()<<"/"<< m_FiredSSmap[6].size()
        <<" "<< m_UsedSSmap[7].size()<<"/"<< m_FiredSSmap[7].size()<<"\n";
#ifdef PRINT_DETAILS_NEVENT
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
#endif
     print--;
  }
#endif
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
#ifdef PRINT_SS_DETAIL
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
#endif
   }
}

/**
   printSector()

   for debugging purposes
 */

void FTK_CompressedAMBank::printSector(int sector,int npattern,
                                       int ipattern) {
   std::cout<<" FTK_CompressedAMBank::printSector\n";
   Range const &range=m_bank.m_PatternRangeBySector[sector];
   if(range.Size()) {
      std::cout<<" sector="<<sector<<" ["
               <<range.First()<<","
               <<range.Last()<<"]\n";
      int i0[2],i1[2];
      int npart=1;
      i0[0]=range.First();
      i1[0]=range.Last();
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
               if(i0[0]<range.First()) i0[0]=range.First();
               if(i1[0]>range.Last()) i1[0]=range.Last();
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
               int dcSSID=m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
                  getSSID(sector,patternID);
               std::pair<uint16_t,uint16_t> dchb=m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.getDCHB(patternID);
               std::cout<<std::setw(7)<<dcSSID;
               if(nDCbits) {
                  std::cout<<"-";
                  for(int i=nDCbits-1;i>=0;i--) {
                     if(dchb.first & (1<<i)) {
                        std::cout<<"X";
                     } else if(dchb.second &(1<<i)) {
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
                  int dcSSID=m_bank.m_PatternByLayer[ipl].
                     m_SSIDternaryByPattern.getSSID(sector,patternID);
                  std::pair<uint16_t,uint16_t> dchb=
                     m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
                     getDCHB(patternID);
                  std::vector<int> const &tspSSID=getTSPssidVector(ipl,dcSSID);
                  int isub=0;
                  unsigned ipos;
                  if(nDCbits) std::cout<<" ";
                  for(int i=0;i<nDCbits;i++) std::cout<<" ";
                  for(ipos=0;ipos<tspSSID.size();ipos++) {
                     // check whether ipos is allowed, given HB and DC
                     //   bit^HB  is non-zero if the HB bit matches
                     //  (bit^HB)&~DC is zero if HB matches of if DC is set
                     if(((ipos ^ dchb.second)& ~dchb.first)==0) {
                        if(isub==line) {
                           std::cout<<std::setw(7)<<tspSSID[ipos];
                           usedSSID[ipl].insert
                              (std::make_pair(tspSSID[ipos],sector));
                           break;
                        }
                        isub++;
                     }
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
               std::pair<uint16_t,uint16_t> dchb=
                  m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
                  getDCHB(patternID);
               std::cout<<std::setw(3)<<m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.getPackedData(patternID)/
                  m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
                  m_offsetTernary;
               if(nDCbits) {
                  std::cout<<"-";
                  for(int i=nDCbits-1;i>=0;i--) {
                     if(dchb.first & (1<<i)) {
                        std::cout<<"X";
                     } else if(dchb.second &(1<<i)) {
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
         int n=m_bank.m_PatternByLayer[ipl].m_SSIDternaryByPattern.
            m_dcSSIDbySectorIndex[sector].size();
         std::cout<<" "<<ipl<<std::setw(5)<<n<<" [";
         if(n<8) {
            for(int j=0;j<n;j++) {
               std::cout<<" "<<m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.m_dcSSIDbySectorIndex[sector][j];
            }
         } else {
            for(int j=0;j<4;j++) {
               std::cout<<" "<<m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.m_dcSSIDbySectorIndex[sector][j];
            }
            std::cout<<" ...";
            for(int j=n-4;j<n;j++) {
               std::cout<<" "<<m_bank.m_PatternByLayer[ipl].
                  m_SSIDternaryByPattern.m_dcSSIDbySectorIndex[sector][j];
            }
         }
         std::cout<<" ]\n";
      }
      std::cout<<"relevant part of compressed lookup-tables\n";
      std::cout<<"=L===SSID=sector===first=npatt=nbyte=encod==========data=====================\n";
      for(int ipl=0;ipl<getNPlanes();ipl++) {
         for(MAP<int,int>::const_iterator iSSID=usedSSID[ipl].begin();
             iSSID!=usedSSID[ipl].end();iSSID++) {
            PatternBySectorSSidMap_t::const_iterator jSSID=
               m_bank.m_PatternByLayer[ipl].m_SSidData.find((*iSSID).first);
            if(jSSID!=m_bank.m_PatternByLayer[ipl].m_SSidData.end()) {
               FTK_CompressedSectorPatternList::const_iterator jSector=
                  (*jSSID).second.find((*iSSID).second);
               if(jSector != (*jSSID).second.end()) {
                  Int_t firstPattern,nPattern,encoding;
                  std::vector<Int_t> deltaData;
                  (*jSector).second.exportData
                     (&firstPattern,&nPattern,&encoding,deltaData);
                  std::cout<<" "<<ipl<<std::setw(7)<<(*iSSID).first
                           <<std::setw(6)<<sector
                           <<std::setw(8)<<firstPattern
                           <<std::setw(6)<<nPattern
                           <<std::setw(6)<<encoding
                           <<std::setw(6)<<deltaData.size()
                           <<" ["<<std::setbase(16);
                  if(deltaData.size()<6) {
                     for(unsigned k=0;k<deltaData.size();k++) {
                        std::cout<<" 0x"<<deltaData[k];
                     }
                  } else {
                     for(int k=0;k<3;k++) {
                        std::cout<<" 0x"<<deltaData[k];
                     }
                     std::cout<<" ...";
                     for(unsigned k=deltaData.size()-3;k<deltaData.size();k++) {
                        std::cout<<" 0x"<<deltaData[k];
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
