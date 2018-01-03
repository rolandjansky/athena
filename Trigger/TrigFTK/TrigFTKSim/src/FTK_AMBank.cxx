/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/FTKRoadKDTree.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/FTKPatternBySector.h"
using namespace ftk;

#include <TFile.h>
#include <TTree.h>

#include  <time.h>
#include  <sys/time.h>

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <map>

using namespace std;

//#define USEAMIN2
//#define PRINT_SS 1
//#define PRINT_ROADS_SECTOR -1
#define PRINT_ROADS_NUM 100

#ifdef TESTTBB
#include "tbb/parallel_for.h"
using namespace tbb;
#endif

const int MAXROADS = 10000000;
const int SS_LOOKUP_SIZE = 360000;


int FTK_AMBank::m_HBseg = sizeof(unsigned char)*8;
const int FTK_AMBank::m_maxRoads(MAXROADS);
const int FTK_AMBank::m_ssLookupSize(SS_LOOKUP_SIZE);

int FTK_AMBank::m_WCID = SS_LOOKUP_SIZE-2; // value for the wildcard
int FTK_AMBank::m_VetoID = SS_LOOKUP_SIZE-1; // value for the veto, used to disable duplicated patterns

const unsigned char FTK_AMBank::m_matchword_maskshift(4);
const unsigned char FTK_AMBank::m_matchword_countermask((1<<m_matchword_maskshift)-1); // maximum number of matches, it has to be a 2^N-1


#define USEKDTREE

FTK_AMBank::FTK_AMBank(int id, int subid)
   : FTK_AMsimulation_base(id,subid), 
    m_CachedBank(false),
     //m_ssmap(0),
     //m_nplanes(0),
     //m_npatterns(0),
    m_patterns(0),
    m_patternCoverage(0),
    m_sectorCoverage(0),
    m_totalCoverage(0),
    m_patterns_matchstatus(0x0),
     //m_nroads(0),
     //m_do_pattern_stats(0),
     //m_StoreAllSS(false),
     //m_useMinimalAMIN(false),
     //m_nao_nroads_am(0), m_nao_nroads_rw(0),
     m_fired_ssmap(0),
     m_fired_ss(0),
    //m_require_first(false), m_require_last(false),
     //m_SaveAllRoads(0),
    m_amout(0),
    m_pattern_stats(0x0),
    m_useWC(false),
     m_WCSS(0),
     m_WCSS2(0),
     m_bad_module_map(0),
     m_ss_patt_lookup_map(0),
    m_lutsepplane(3), m_upperindex(true),
     m_ipatt_step(0),
     m_stlhit_sort(0x0)

{
  // nothing todo
}


FTK_AMBank::~FTK_AMBank()
{
  // clean memory
   if (m_nplanes>0) {
    delete [] m_fired_ssmap;
    delete [] m_fired_ss;
    delete [] m_WCSS;
    delete [] m_WCSS2;
  }
  if (m_npatterns>0) {
    delete [] m_patterns;
    delete [] m_patterns_matchstatus;

    delete [] m_patternCoverage;

    delete m_sectorCoverage;

    delete [] m_ss_patt_lookup_map;
  }

  delete [] m_stlhit_sort;
}

/** initialize the base structures */
void FTK_AMBank::init()
{
   FTK_AMsimulation_base::init();
   m_stlhit_sort = new vector<FTKHit>[m_nplanes];

   // resize the ss map, could save something
   m_usedssmap.clear();
   m_usedssmap.reserve(m_nplanes);
   for (int ipl=0;ipl!=m_nplanes;++ipl) {
      m_usedssmap.push_back(unordered_map<int,FTKSS>());
   }
}


/** End of the session, this method is called before the destructors
    are called, all the objects have valid pointers */
void FTK_AMBank::end()
{
  if (getPatternStats()) {
    // write the A bank statistic
    ftk_dcap::ostream bankstat_file;
    ftk_dcap::open_for_write("patternbank.dat",bankstat_file);

    bankstat_file << "PattID Freq" << endl;
    for (int ipatt=0;ipatt!=m_npatterns;++ipatt) {
      bankstat_file << ipatt << ' ' << m_pattern_stats[ipatt] << endl;
    }
  }
}

/** This method prepare the structures used to store the
    pattern bank. This method is private and it is called within the
    various readXXXBank(...) methods.
 */
int FTK_AMBank::readBankInit()
{
   // the process counter
   m_ipatt_step = (m_npatterns+9)/10;
   if (!m_ipatt_step) m_ipatt_step = 1;

   // create the maps where the SS map used in the events are stored
   m_fired_ssmap = new unordered_map<int,FTKSS>[m_nplanes];
   m_fired_ss = new unordered_set<int>[m_nplanes];

   // allocate the space to store the patterns, each pattern is composed by m_nplanes+1 numbers, the SSs and the sectors ID
   m_patterns = new int[(m_nplanes+1)*m_npatterns];

   // check if the pattstatus_t type width is sufficient
   if (sizeof(pattstatus_t)*8<static_cast<size_t>(m_nplanes+m_matchword_maskshift)) {
     FTKSetup::PrintMessage(sevr,"Number of bits in the pattstatus_t type insufficient");
   }
   // allocate the compressed structure that store the match status for the patterns
   m_patterns_matchstatus = new pattstatus_t[m_npatterns];

   // allocate m_patternCoverage and m_sectorCoverage
   m_patternCoverage = new int[m_npatterns];
   m_sectorCoverage = new std::map<int,int>;

   // create the array of list with the wildcards
   m_WCSS = new set<int>*[m_nplanes];
   m_WCSS2 = new set<int>*[m_nplanes];
   for (int ip=0;ip!=m_nplanes;++ip) {
     m_WCSS[ip] = 0x0; // inialize to 0
     m_WCSS2[ip] = 0x0; // inialize to 0
   }

   //set bad ss map 
   ifstream checkbadmodpath(getBadSSMapPath());
   ifstream checkbadmodpath2(getBadSSMapPath2());
   if (static_cast<bool>(checkbadmodpath) != 0)  setBadSSMap();
   if (static_cast<bool>(checkbadmodpath2) != 0) setBadSSMap2();

  return 0;
}


/** This method finalize the bank initialization. It is private and it is 
    called within the various readXXXBank(...) methods.
 */
int FTK_AMBank::readBankInitEnd() {
   FTKSetup &ftkset = FTKSetup::getFTKSetup();

   // setup the Wildcards in the bank
   if (!m_CachedBank) applyWildcard();

   ftkset.usageStat("Prepare the AM structures");   
   // allocate a lookup table between ss and patterns

   // STL hashmap version:
   m_ss_patt_lookup_map = new hash_map<int, vector<int> >[m_nplanes];

   pattlookup_make_map();

   return 0;
}

/** This method apply the WC then look for duplicate patterns created by the use of the wildcard
    and do other final check on those patterns. Also prints some message. */
void FTK_AMBank::applyWildcard()
{
  // if any of dead SS list is 0x0 skip the WC study
  for (int ip=0;ip!=m_nplanes;++ip) {
    if (!m_WCSS[ip]) {
      FTKSetup::PrintMessage(info,"Wildcard cannot be applied");
      return;
    }
    else {

    }
  }

  FTKSetup::PrintMessage(info,"Wildcard can be applied");
  m_useWC = true; // set the use of the WC

  // adject the WC and Veto IDs if the STL map is used
  m_VetoID = ~(~0u<<(sizeof(int)*8-1));
  m_WCID = m_VetoID-1;

  unsigned int *WC_stat = new unsigned int[m_nplanes]; // store the static collected on the wildcard
  unsigned int *WC_stat_nWClayers = new unsigned int[m_nplanes+1]; // store the number of patterns with N WC layers
  unsigned int *WC_stat_patt = new unsigned int[m_npatterns]; // store the static collected on the wildcard
 
  for (int ip=0;ip!=m_nplanes;++ip) WC_stat[ip] = 0;
  for (int ip=0;ip!=m_nplanes+1;++ip) WC_stat_nWClayers[ip] = 0;
  for (int ip=0;ip!=m_npatterns;++ip) WC_stat_patt[ip] = 0;
  
  std::set<int> WC_patterns; // list of patterns where the wildcard was applied
  std::set<int> Veto_patterns ; // list of patterns where the veto was applied

  set<int> veto_patt; 
#if 0 //read ascii file and set veto ID
  ifstream veto_file("Veto_patt.dat");
  while(1){
    int vetoID;
    veto_file >> vetoID;
    if(veto_file.eof())break;
    veto_patt.insert(vetoID);
  }
  veto_file.close();
  cout <<"# Veto patterns are"<<veto_patt.size() <<endl;
#endif
  
  // GV,AA: comments for speed up: loop over pattern first and then over planes in the pattern
  //        should organize patterns by sectors in order to speed up the lookup. This will actually require to know similar sectors.
  for (int iplane=0;iplane!=m_nplanes;++iplane) { // loop over the planes
    set<int> *tmp_WCSS ; 
    tmp_WCSS = m_WCSS[iplane];
    cout <<"loading "<<iplane <<" plane..."<<endl;
    for (int ipatt=0;ipatt!=m_npatterns;++ipatt) { // loop over the patterns

      if(veto_patt.find(ipatt) != veto_patt.end() ){
	m_patterns[_SSPOS(ipatt,iplane)] =  m_VetoID;     // all layers are vetoed
      }else if(tmp_WCSS->find(m_patterns[_SSPOS(ipatt,iplane)]) == tmp_WCSS->end()){
      	continue;
      }else {
	WC_stat[iplane]++;
 	WC_stat_patt[ipatt]++;
 	WC_patterns.insert(ipatt);
 	m_patterns[_SSPOS(ipatt,iplane)] =  m_WCID;
      }
    } // end loop over the patterns
  } // end plane loop
  
  // print the debug messages to inform of which is the status of the WC
  cout << "Tower " << getBankID() << " number of patterns with the wildcard: " << WC_patterns.size() << endl;
  for (int ip=0;ip!=m_nplanes;++ip) {
    cout << "Tower " << getBankID() << " number of WC on layer " << ip << ": " << WC_stat[ip] << endl;
  }

#if 0   //Dump the number of WC on pattern(for debug!) 
  ofstream mapfile_WC_patt("nWC_patt.dat");
  for(int tmp_ipatt=0;tmp_ipatt != m_npatterns;++tmp_ipatt){
    mapfile_WC_patt <<tmp_ipatt <<" "<<WC_stat_patt[tmp_ipatt]<<endl;
    if(tmp_ipatt%10000 == 0 ) cout <<"write patt "<<tmp_ipatt<<endl;
  }
  mapfile_WC_patt.close();
#endif

 if (WC_patterns.empty()) {
   FTKSetup::PrintMessage(info,"No wildcards applied");

   // clear the memory for the information on tge dead-module
   delete [] WC_stat;
   delete [] WC_stat_nWClayers;
   delete [] WC_stat_patt;

   return;
 }
 else {
   cout <<" check duplicated patterns and remove them using the veto ID"<<endl;
    // loop over the patterns to check duplicated patterns and remove them using the veto ID
   set<int>::iterator ipatt_ref = WC_patterns.begin();
   set<int>::iterator ipatt_end = WC_patterns.end();
   // GV,AA: comments for speed up: index WC_patterns with SSID 
   //        make a KDTree that stores the pattern with all SSIDs
   //        the tree must be sorted by SSID (prioritized by layer)
   //        to make the search logarithmic
   for (;ipatt_ref!=ipatt_end;++ipatt_ref) { // loop over the patterns with the WC
     int nWC = 0;
     for (int wcplane=0;wcplane!=m_nplanes;++wcplane) { // loop over the planes
       if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] ==  m_WCID) nWC++;
     } 
     WC_stat_nWClayers[nWC]++;	  

     // veto patterns that have 3 or more m_WCID set.
     if (nWC>=3) {
       Veto_patterns.insert(*ipatt_ref);
       for (int wcplane=0;wcplane!=m_nplanes;++wcplane) // loop over the planes
	 m_patterns[_SSPOS(*ipatt_ref,wcplane)] = m_VetoID;
       continue;
     }

     // for patterns with nWC==2: set the second WC to veto so that only one WC remains active.
     // These patterns are not inserted in the veto list
     if (nWC==2) {
       // Loop over the planes starting from last down to layer 1. 
       // The loop must never reach layer 0 so it is excluded.
       for (int wcplane=m_nplanes-1;wcplane>0;--wcplane) {
	 if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] == m_WCID) {
	   m_patterns[_SSPOS(*ipatt_ref,wcplane)] = m_VetoID;
	   break;
	 }
       }
     }

     // cout <<"continue! check patt "<<(*ipatt_ref) <<endl;
     // if((*ipatt_ref) > 0 )break;  // this part takes a lot of time, for shortening.(for debug!)
     if (m_patterns[_SSPOS(*ipatt_ref,0)]==m_VetoID) continue; // skip patterns with the veto
     set<int>::iterator ipatt_chk = ipatt_ref;
     ipatt_chk++; // start the check from the next
     for (;ipatt_chk!=ipatt_end;++ipatt_chk) { // loop over the pattern with WC to check for duplicates
       if (m_patterns[_SSPOS(*ipatt_chk,0)]==m_VetoID) continue; // skip patterns with the veto
       bool isequal(true);
       for (int ip=0;ip!=m_nplanes;++ip) {
	 if (m_patterns[_SSPOS(*ipatt_ref,ip)]!=m_patterns[_SSPOS(*ipatt_ref,ip)]) {
	   isequal = false;
	   break;
	 }  
       }
       
       if (isequal) {
	 // the patterns are equal, the latter is invalidated using the veto
	 Veto_patterns.insert(*ipatt_chk);
	 for (int ip=0;ip!=m_nplanes;++ip) m_patterns[_SSPOS(*ipatt_chk,ip)] = m_VetoID;
       }
     } // end loop to search for duplicates
   } // end loop over the patterns with the WC
   cout <<"Number of veto patterns:"<<Veto_patterns.size()<<endl;
   for (int ip=0;ip<=m_nplanes;++ip) {
     cout << "Tower " << getBankID() << " number of patterns with WC on " << ip << " layers : " << WC_stat_nWClayers[ip] << endl;
   }

   cout << "Count nWC and nVeto again for debug" << endl;
   ipatt_ref = WC_patterns.begin();
   int nWC0Veto1p=0;
   int nWC1pVeto1p=0;
   int nWC0Veto1=0;
   int nWC1Veto1=0;
   int nWC2pVeto1=0;
   for (int ip=0;ip!=m_nplanes+1;++ip) WC_stat_nWClayers[ip] = 0;
   for (;ipatt_ref!=ipatt_end;++ipatt_ref) { // loop over the patterns with the WC
     int nWC = 0;
     int nVeto = 0;
     for (int wcplane=0;wcplane!=m_nplanes;++wcplane) { // loop over the planes
       if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] ==  m_WCID) nWC++;
       if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] ==  m_VetoID) nVeto++;
     } 
     if (nVeto==0)
       WC_stat_nWClayers[nWC]++;
     else {
       if (nWC>=1 and nVeto>1) {
	 cout << "Pattern with nWC="<<nWC<<" nVeto="<<nVeto<<endl;
	 nWC1pVeto1p++;
       }
       if (nWC==0 and nVeto>1)
	 nWC0Veto1p++;
       if (nVeto==1) {
	 if (nWC==0) nWC0Veto1++;
	 if (nWC==1) nWC1Veto1++;
	 if (nWC>=2) nWC2pVeto1++;
       }
     }
   }
   for (int ip=0;ip<=m_nplanes;++ip) {
     cout << "DEBUG: Tower " << getBankID() << " number of patterns with WC on " << ip << " layers : " << WC_stat_nWClayers[ip] << endl;
   }
   cout << "nWC0Veto1p=" << nWC0Veto1p << endl;
   cout << "nWC1pVeto1p=" << nWC1pVeto1p << endl;
   cout << "nWC0Veto1=" << nWC0Veto1 << endl;
   cout << "nWC1Veto1=" << nWC1Veto1 << endl;
   cout << "nWC2pVeto1=" << nWC2pVeto1 << endl;
  
#if 0 // Dump WC & veto patterns(for debug!)
    ofstream mapfile_Veto_patt("Veto_patt.dat");
    for( set<int>::iterator ite_Veto = Veto_patterns.begin(); ite_Veto != Veto_patterns.end() ; ++ite_Veto){
      mapfile_Veto_patt <<(*ite_Veto) <<endl;
    }
    
    ofstream mapfile_WC_patt("WC_patt.dat");
    for( set<int>::iterator ite_WC = WC_patterns.begin(); ite_WC != WC_patterns.end() ; ++ite_WC){
      mapfile_WC_patt <<(*ite_WC) <<endl;
    }
    
#endif
  }
  
  // clear the memory
  delete [] WC_stat;
  delete [] WC_stat_patt;
  delete [] WC_stat_nWClayers;
}


/** This method read the Pattern Bank file stored
    in ASCII file format. It returns the number of 
    the read pattern, <0  if there was an error */
int FTK_AMBank::readASCIIBank(const char *fname, int maxpatt)
{
   // mainly from ambanks_rd in read_routines.c

   // if boost::iostreams is available, use it to give C++ iostreams
   // the ability to read from dCache, gzip, and/or bzip2 sources.
   // otherwise, stick with Guido's implementation and read text
   // files straight from a regular filesystem.
   ftk_dcap::istream pattfile;
   const bool ok_read = ftk_dcap::open_for_read(fname,pattfile);
   if (!pattfile || !ok_read) { // error opening the file
     cerr << "*** Error opening the file: " << fname << endl;
     return -1;
   }

   cout << "Read ASCII file format bank: " << fname << endl;   
   int nplanes;
   pattfile >> m_npatterns >> nplanes;
   setNPlanes(nplanes);
   
   if (maxpatt<0 || m_npatterns<maxpatt) { 
     // the limit is jus the number of the patterns in the bank
     // value changed just to inform the user
     maxpatt = m_npatterns;
   }

   cout << "Found # " << m_npatterns << " in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

   if (maxpatt<m_npatterns) {
     // the number of pattern to read is limited by the user
     m_npatterns = maxpatt;
   }

   readBankInit();
   
   cout << "Reading : [" << flush;
   for (int ipatt=0;ipatt<m_npatterns;++ipatt) { // loop over patterns
     int ival;
      int npatt;
      pattfile >> npatt;
      if (npatt!=ipatt) {
	 cerr << "*** error reading the pattern " << ipatt << endl;
	 return -3;
      }
      else if (ipatt%m_ipatt_step==0) {
	cout << ipatt/m_ipatt_step << flush;
      }
      //cout<<ipatt<<" "; //hier
      for (int iplane=0;iplane<m_nplanes+1;++iplane) { // loop on the planes
	 // the m_nplnes+1 element is the number of the sector
	 // the other are the SS index
	 pattfile >> ival;

	 // TODO: check if the SS should use the WC

	 m_patterns[_SSPOS(ipatt,iplane)] = ival;
	 //cout<<m_patterns[_SSPOS(ipatt,iplane)]<<" "; //hier
      } // end loop on the planes

      // number of events not used, temporary in npatt
      pattfile >> ival;
      m_patternCoverage[ipatt] = ival;
      m_totalCoverage += ival;
     (*m_sectorCoverage)[m_patterns[_SSPOS(ipatt,m_nplanes)]] += m_patternCoverage[ipatt];
     //cout<<ival<<" "<<endl;; //hier

      // check if there is an error during the file reading
      if (!pattfile) {
	cerr << "*** error scanning file \"" << fname << "\" at pattern " << ipatt << endl;
	return -4;
      }
   } // end loop over patterns
   cout << "]" << endl;

   readBankInitEnd();

   // return the number of the patterns in the bank
   return m_npatterns;
}

/** This method read the Pattern Bank file stored
    in a ROOT file.
    The method decides, which format is provided.
    and returns <0 if there was an error
*/
int FTK_AMBank::readROOTBank(const char *fname, int maxpatt)
{
   cout << "Read ROOT file format bank: " << fname << endl;
   TFile *file = TFile::Open(fname,"READ");
   if (!file) { // error opening the file
      cerr << "*** Error opening the file: " << fname << endl;
      return -1;
   }  
   
   //file->ls();

   // loop over all keys:
   for ( int i = 0 ; i <file->GetListOfKeys()->GetEntries(); i++ ) {
      TString keyname = file->GetListOfKeys()->At(i)->GetName();
      //cout<<"Info: FTK_AMBank::readROOTBank(). Found tree with name: "<<keyname<<endl;
      if ( keyname=="AMPatterns") {
	 cout<<"Found TTree 'AMPatterns'!"<<endl;
	 return readROOTBankFTKPatternTree(file,"AMPatterns",maxpatt);
      }
      else if ( keyname=="Bank0" ) {
	 cout<<"Found TTree 'Bank0'!"<<endl;
	 return readROOTBankFTKPatternTree(file,"Bank0",maxpatt);
      }
      else if ( keyname.Contains("sector") && keyname.Contains("patterns") ) {
	 cout<<"Found sector-ordered patterns!"<<endl;
	 return readROOTBankSectorOrdered(file,maxpatt);
      }
   }
   
   cerr << "*** No TTree with patterns found in: " << fname << endl;
   return -1; 
}



/** This method reads the sector-ordered Pattern Bank.
    returns <0  if there was an error */
int FTK_AMBank::readROOTBankSectorOrdered(const char *fname, int maxpatt)
{
   // try to open the input file using the ROOT wrapper,
   // fname could contain protocol specfication at the begin
   TFile *pattfile = TFile::Open(fname);
   
   if (!pattfile) { // error opening the file
      cerr << "*** Error opening the file: " << fname << endl;
      return -1;
   }
   return readROOTBankSectorOrdered(pattfile,maxpatt);
}


/** This method reads the sector-ordered Pattern Bank.
    returns <0  if there was an error */
int FTK_AMBank::readROOTBankSectorOrdered(TFile* pattfile, int maxpatt)
{
   cout<<"Info. FTK_AMBank::readROOTBankSectorOrdered(). Reading file: "<<pattfile->GetName()<<endl;

   cout<<"\n\n\n *** Todo. The sector-ordered patterns need also information on the subregion! *** \n\n\n"<<endl;
   int iSub=0;
   int nSub=1;

   FTKPatternBySectorForestReader* preader = new FTKPatternBySectorForestReader(*pattfile);
   if(!preader) {
      cerr<<"Error. Cannot read root-file pattern from: "<<pattfile->GetName()<<endl;
      return -1;
   }
   m_npatterns = preader->GetNPatternsTotal();
   setNPlanes(preader->GetNLayers());

   if (maxpatt<0 || m_npatterns<maxpatt) { 
     // the limit is jus the number of the patterns in the bank
     // value changed just to inform the user
     maxpatt = m_npatterns;
   }

   cout << "Found # " << m_npatterns << " in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

   if (maxpatt<m_npatterns) {
     // the number of pattern to read is limited by the user
     m_npatterns = maxpatt;
   }

   // // the number of pattern to read is limited by the user
   // if (maxpatt<m_npatterns)  m_npatterns = maxpatt;

   readBankInit();
   
   // loop over sectors recursively, convert to 'FTKPattern' and write into root-tree
   int patternID = 0;
   cout << "Reading : [" << flush;
   preader->Rewind();
   for(int coverage=preader->GetMaxCoverage();coverage;
         coverage=preader->GetNextCoverage(coverage)) {
      if(patternID>=m_npatterns) break;
      for(int sector=preader->GetCoverageFirstSector(coverage);
          sector>=0;sector=preader->GetCoverageNextSector(coverage,sector)) {
         if((nSub>1)&&(sector%nSub!=iSub)) continue;
         if(patternID>=m_npatterns) break;

         if (patternID%m_ipatt_step==0) cout << patternID/m_ipatt_step << flush;

         FTKPatternOneSector *data=preader->Read(sector,coverage);
         for(FTKPatternOneSector::Ptr_t i=data->Begin();i!=data->End();i++) {
            if(patternID>=m_npatterns) break;

            // //showstats(m_patternID, m_npatterns);
            // inputpattern->setPatternID(m_patternID);
            // inputpattern->setSectorID(sector);
            // inputpattern->setCoverage(coverage);
            //cout<<patternID<<" "; //hier
            const FTKHitPattern& patternData =data->GetHitPattern(i);

            for( int iplane=0 ; iplane<m_nplanes ; iplane++ ) {
               m_patterns[_SSPOS(patternID,iplane)] = patternData.GetHit(iplane); 
	    //cout<<m_patterns[_SSPOS(patternID,iplane)]<<" "; //hier
            }
            m_patterns[_SSPOS(patternID,m_nplanes)] = sector;
            m_patternCoverage[patternID] = coverage;
            m_totalCoverage += coverage;
            (*m_sectorCoverage)[m_patterns[_SSPOS(patternID,m_nplanes)]] += m_patternCoverage[patternID];
	 //cout<<sector<<" "<<coverage<<" "<<endl;; //hier

            patternID++; // increment the global pattern ID
         } // end for (patterns)
         delete data;
      } // end for(sector)
   } // end for(coverage)
   cout << "]" << endl;
  
   cout<<patternID<<" patterns read successfully."<<endl;
   
   // delete preader;
   // preader = NULL;
   cout<<"preader deleted successfully."<<endl;
     
   
   readBankInitEnd();

   delete preader;

   // return the number of the patterns in the bank
   return m_npatterns;

}


/** This method reads the Pattern Bank stored in a Root-tree
    named 'Bank0'. It returns the number of 
    the read pattern, <0  if there was an error */
int FTK_AMBank::readROOTBankBank0(const char *fname, int maxpatt) {
   TFile *pattfile = TFile::Open(fname);
   if (!pattfile) { // error opening the file
      cerr << "*** Error opening the file: " << fname << endl;
      return -1;
   }
   return readROOTBankFTKPatternTree(pattfile,"Bank0",maxpatt);
}



/** This method reads the Pattern Bank stored in a Root-tree
    named AMPatterns. It returns the number of 
    the read pattern, <0  if there was an error */
int FTK_AMBank::readROOTBankAMPatterns(const char *fname, int maxpatt)
{
    // try to open the input file using the ROOT wrapper,
  // fname could contain protocol specfication at the begin
  TFile *pattfile = TFile::Open(fname);

   if (!pattfile) { // error opening the file
     cerr << "*** Error opening the file: " << fname << endl;
     return -1;
   }
   return readROOTBankFTKPatternTree(pattfile,"AMPatterns",maxpatt);
}


/** This method read the Pattern Bank stored in a Root-tree
    named AMPatterns. It returns the number of 
    the read pattern, <0  if there was an error */
int FTK_AMBank::readROOTBankFTKPatternTree(TFile* pattfile, TString treename, int maxpatt)
{

   cout << "Read ROOT file format bank: " << pattfile->GetName() << endl;
   // look for the main TTree containing the patterns
   TTree *patttree = (TTree*) pattfile->Get(treename);
   if (!patttree) {
     cerr << "*** AMPatterns doesn't exist in: " << pattfile->GetName() << endl;
     return -1;
   }
   
   // get the number of entries
   m_npatterns = patttree->GetEntries();

   // if not patterns are found just exit
   if (!m_npatterns) return 0;

   // create the connection with the list of the patterns
   FTKPattern *tmppatt = new FTKPattern();
   if ( patttree->FindBranch("pattern") )
      patttree->SetBranchAddress("pattern",&tmppatt);
   else if ( patttree->FindBranch("Pattern") )
      patttree->SetBranchAddress("Pattern",&tmppatt);
   else {
     cerr << "*** Could not find branch 'pattern' or 'Pattern' in: " << pattfile->GetName() << endl;
     return -1;
   }

   // use the first pattern to extract generic information
   patttree->GetEntry(0);
   setNPlanes(tmppatt->getNPlanes());
   
   if (maxpatt<0 || m_npatterns<maxpatt) { 
     // the limit is jus the number of the patterns in the bank
     // value changed just to inform the user
     maxpatt = m_npatterns;
   }

   cout << "Found # " << m_npatterns << " in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

   if (maxpatt<m_npatterns) {
     // the number of pattern to read is limited by the user
     m_npatterns = maxpatt;
   }

   readBankInit();
   
   cout << "Reading : [" << flush;
   for (int ipatt=0;ipatt<m_npatterns;++ipatt) { // loop over patterns
     // move the TTree in the ipatt position
     patttree->GetEntry(ipatt);

     //cout<<ipatt<<" "; //hier
     for (int iplane=0;iplane!=m_nplanes;++iplane) { // loop on the planes
       // get the SSID for this plane
       m_patterns[_SSPOS(ipatt,iplane)] = tmppatt->getSSID(iplane);
       //cout<<m_patterns[_SSPOS(ipatt,iplane)]<<" "; //hier
    } // end loop on the planes

     // the last position is the sectord ID
     m_patterns[_SSPOS(ipatt,m_nplanes)] = tmppatt->getSectorID();


     // read the pattern coverage
     m_patternCoverage[ipatt] = tmppatt->getCoverage();
     m_totalCoverage += m_patternCoverage[ipatt];
     (*m_sectorCoverage)[m_patterns[_SSPOS(ipatt,m_nplanes)]] += m_patternCoverage[ipatt];
 
     //cout<<tmppatt->getSectorID()<<" "<<tmppatt->getCoverage()<<" "<<endl;; //hier

   } // end loop over patterns
   cout << "]" << endl;

   readBankInitEnd();

   // return the number of the patterns in the bank
   return m_npatterns;
}


/** Write the AM patterns to a root-file in the 'Bank0'-format */
void FTK_AMBank::writeAMBank0(TFile* file,const std::string& TTreename, const std::string& TTreeTitle){
   // prepare the writing
   TDirectory* dir = gDirectory;
   file->cd();
   FTKPattern* inputpattern = new FTKPattern(m_nplanes);
   TTree* bank = new TTree(TTreename.c_str(),TTreeTitle.c_str());
   bank->Branch("Pattern",&inputpattern);
   
   for (int ipatt=0;ipatt<m_npatterns;++ipatt) { // loop over patterns
      //int ival[m_npatterns];
      //for (int iplane=0;iplane<m_nplanes+1;++iplane) { 
      for (int iplane=0;iplane<m_nplanes;++iplane) { 
	 // loop on the planes
	 //    the m_nplnes+1 element is the number of the sector
	 //    the other are the SS index
	 int val =  m_patterns[_SSPOS(ipatt,iplane)];
	 //ival[ipatt] = m_patterns[_SSPOS(ipatt,iplane)];
	 inputpattern->setSSID(iplane,val);
      }
      int cov = m_patternCoverage[ipatt];
      inputpattern->setCoverage(cov);
      inputpattern->setPatternID(ipatt);
      inputpattern->setSectorID(m_patterns[_SSPOS(ipatt,m_nplanes)]); // the last position [of m_patterns] is the sectord ID
      // write the pattern
      bank->Fill();
   }

   // Write patterns to disk
   bank->Write();
   //file->Write();
   dir->cd();
}


/** Write the AM patterns to a 'recreated' root-file in the 'Bank0'-format */
void FTK_AMBank::writeAMBank0(const std::string& filename,const std::string& TTreename, const std::string& TTreeTitle){

   //FTKRootFile::Instance()->CreateRootFile(filename.c_str());
   
   TFile* file = TFile::Open(filename.c_str(),"recreate");
   FTKSetup::PrintMessageFmt(ftk::info,"Output bank root-file created: %s\n",filename.c_str());

   writeAMBank0(file, TTreename, TTreeTitle);
   file->Close();
}


// STL hashmap version of lookup table:
void FTK_AMBank::pattlookup_make_map()
{
   if (getUseMinimalAMIN()) {
      FTKSetup::PrintMessage(info,"Use am_in_minimal(), no LUT required.");
      return;
  }

  int startplane(0);
  int endplane(m_nplanes);
#ifdef USEAMIN2
  if (m_upperindex) {
      startplane = m_lutsepplane;
      endplane = m_nplanes;
  }
  else {
      startplane = 0;
      endplane = m_lutsepplane;
  }
#endif

  // Main loop, over planes
  for (int iplane=startplane; iplane!=endplane; ++iplane) {
    // Count patterns for ss;
    for (int i = 0; i < m_npatterns; ++i) {	
      if(!(i%m_ipatt_step)) cout << "Layer " << iplane << ": counting patterns per ss (" << i << ")" << endl;
      // m_pattern content is an array, not a matrix
      int iss = m_patterns[_SSPOS(i,iplane)];
      if ( m_ss_patt_lookup_map[iplane].find(iss) == m_ss_patt_lookup_map[iplane].end() ) {
	m_ss_patt_lookup_map[iplane][iss] = std::vector<int>();
      }
      m_ss_patt_lookup_map[iplane][iss].push_back(i);
    }
    cout << endl;

    unsigned nsslayer =  m_ss_patt_lookup_map[iplane].size();
    cout << "Number of SS in the map: " << nsslayer << endl;

    // small study on the map
    unsigned int npattsmax(0);
    unsigned int npattsmin(m_npatterns);
    for (auto ssdic: m_ss_patt_lookup_map[iplane]) {
        unsigned int npatts = ssdic.second.size();
        if (npatts>npattsmax) npattsmax = npatts;
        if (npatts<npattsmin) npattsmin = npatts;
    }

    FTKSetup::PrintMessageFmt(info,"Layer %d dictionary statistic:\naverage connected patterns %.1f, min %u, max %u\n",iplane, 1.*m_npatterns/nsslayer,npattsmin,npattsmax);
  }
}

void FTK_AMBank::attach_SS() {
  
  list<FTKRoad>::iterator iroad = m_roads.begin();
  for (;iroad!=m_roads.end();++iroad) {
    FTKRoad &last_road = *iroad; 
    int pattID = last_road.getPatternID();
    
    // last_road.setSectorID(m_patterns[_SSPOS(pattID,m_nplanes)]);
    if (FTKSetup::getFTKSetup().getSectorsAsPatterns()) last_road.setSectorID(pattID);
    else last_road.setSectorID(m_patterns[_SSPOS(pattID,m_nplanes)]);

    // retrieve the hits
    for (int ipl=0;ipl!=m_nplanes;++ipl) {
      // return the super-strip for this plane
      int ss = m_patterns[_SSPOS(pattID,ipl)];
      
      // set SS id
      last_road.setSSID(ipl,ss);
      last_road.setHLID(ipl,0);

      // if the SS id is the wildcard the bitmask is set to the correct value
      if (ss == m_WCID)
        last_road.setBitMask(last_road.getBitMask()&(~(1<<ipl)));
#ifdef VERBOSE_DEBUG
      cout << last_road << endl;
#endif
      
    }
  }
}


/** This function return the roads, it now returns the m_amout content
    in a different format, then when m_roads list will active it
    will use that */
const std::list<FTKRoad>& FTK_AMBank::getRoads()
{
  // FlagJT: moved ~half of this function to attach_SS() so that we
  // can take advantage of the KD Tree in road_warrior().
  std::list<FTKRoad>::iterator iroad = m_roads.begin();
  for (;iroad!=m_roads.end();++iroad) {
    int pattID = iroad->getPatternID();

    /* retrieve the hits connecting preparing the SS to be saved.
       the connection is skipped if the whole map of SS is required for AUX-card related studies. 
    */
    for (int ipl=0;(ipl!=m_nplanes)&&(!getStoreAllSS());++ipl) {
      // return the super-strip for this plane
      int ss = m_patterns[_SSPOS(pattID,ipl)];

      // skip the ss map check if the SS ID is in this list
      if( ss < 0 || ss == m_WCID || ss == m_VetoID) {
	continue;
      }

      unordered_map<int,FTKSS>::iterator item0 = m_fired_ssmap[ipl].find(ss);
      if (item0==m_fired_ssmap[ipl].end()) 
	continue; // no hits


      // map for this plane
      unordered_map<int,FTKSS> &imap = m_usedssmap[ipl];
      // find this ss is in the bank
      unordered_map<int,FTKSS>::iterator item = imap.find(ss);
      if (item==imap.end()) { // not found
	imap[ss] = (*item0).second;
      }
      else {
	// super-strip already in the map, skip this
	continue;
      }
      
    } // end loop over the layers

  }
#ifdef PRINT_ROADS_SECTOR
  static int print=PRINT_ROADS_NUM;
  if(print) {
     cout<<"FTK_AMBank::getRoads number of roads="<<m_roads.size()<<"\n";
     printRoads(m_roads,PRINT_ROADS_SECTOR);
     print--;
  }
#endif
  return m_roads;
}


/** return the list of the super-strips used in a good road.
    The list of the strips if filled using the getRoads() method,
for this reason that method has to be called before */
const unordered_map<int,FTKSS>& FTK_AMBank::getStrips(int plane)
{
#ifdef PRINT_SS
  static int printSS=PRINT_SS;
  if(printSS) {
     cout<<"getStrips plane="<<plane<<" fired="<< m_fired_ssmap[plane].size()
         <<" used="<<m_usedssmap[plane].size()<<"\n";
#ifdef DEBUG_DETAIL
     map<int,FTKSS const *> sorted;
     for(std::unordered_map<int,FTKSS>::const_iterator i=m_usedssmap[plane].begin();
         i!=m_usedssmap[plane].end();i++) {
        sorted[(*i).first]=& (*i).second;
     }
     for(map<int,FTKSS const *>::const_iterator i=sorted.begin();i!=sorted.end();i++) {
        cout<<" "<<(*i).first;
     }
     cout<<"\n";
     // locate strip #21224 in one of the roads
     int SSID=21224;
     int SSIDplane=0;
     for (std::list<FTKRoad>::iterator iroad = m_roads.begin();
          iroad!=m_roads.end();++iroad) {
        if((*iroad).getSSID(SSIDplane)==SSID) {
           FTKRoad *road=&(*iroad);
           cout<<setw(6)<<road->getSectorID();
           for(int i=0;i<road->getNPlanes();i++) {
              cout<<setw(6)<<road->getSSID(i);
           }
           cout<<" ";
           for(int i=0;i<road->getNPlanes();i++) {
              if(road->hasHitOnLayer(i)) cout<<"1";
              else cout<<"0";
           }
           cout<<setw(6)<<road->getRoadID()
               <<setw(9)<<road->getPatternID()
               <<setw(9)<<road->getPatternDBID()
               <<setw(3)<<road->getBankID()
               <<setw(3)<<road->getRegion()
               <<setw(3)<<road->getSubRegion()
               <<setw(3)<<road->getNSubRoads()<<" "<<setbase(16);
           for(int i=0;i<road->getNPlanes();i++) {
              cout<<road->getSubSSMask(i);
           }
           cout<<setw(7)<<road->getDCBitmask()<<setw(7)<<road->getHLBitmask();
           cout<<setbase(10);
           cout<<"\n";        
        }
     }
#endif
     printSS--;
  }
#endif
  if (getStoreAllSS()) {
    // if the whole SS map has to be stored the SS related to the Veto and SS are removed to not bias the statistic
    m_fired_ssmap[plane].erase(m_WCID);
    m_fired_ssmap[plane].erase(m_VetoID);   

    return m_fired_ssmap[plane];
  }
  else return m_usedssmap[plane];
}


/*******************************************
 * Function: sort_hits()
 * Purpose:  Sorts hits according to plane/sector; duplicated hits in the overlap regions
*******************************************/
void FTK_AMBank::sort_hits(const vector<FTKHit> &hitlist) {
  // refence to the setup
  const FTKSetup &ftkset = FTKSetup::getFTKSetup();

#ifdef VERBOSE_DEBUG
  printf("** sort_hits\n");
#endif

  vector<FTKHit>::const_iterator ihit = hitlist.begin();
  for (;ihit<hitlist.end();++ihit) { // begin loop over the hits
     const FTKHit &hitref = *ihit;
     const int curplane(hitref.getPlane());

     // verify the hit according the mask that masks particular logical laeyrs
     if (ftkset.getMaskLayers()&(1<<curplane))
	 continue; // skip this hit because the layer

     if (ftkset.getVerbosity()>2) {
       cout << "HIT: " << setw(3) << hitref.getPlane();
       cout << setw(7) << hitref.getSector();
       for (int ic=0;ic<hitref.getDim();++ic)
	 cout << setw(8) << hitref[ic];
       cout << endl;
     }

     // verify that the hit is in this region
     if (!getSSMap()->getRegionMap()->isHitInRegion(hitref,getBankID()))
	continue; // skip if the is not in this regione

     // Organize the hits per layer, as the AUX board data organizer does
     m_stlhit_sort[curplane].push_back(hitref);
  } // end loop over the hits

  for (int iplane=0;iplane!=m_nplanes;++iplane) naoClusPushBack(m_stlhit_sort[iplane].size());
}

/** The method organizes the hits in SS. The WC is also enabled because the m_WCID fake hit
 * is sent.
 * The Hits grouped by SS are stored into the m_fired_ssmap structure, when required also the
 * m_fired_ss structure is filled.
 */
void FTK_AMBank::data_organizer() {
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const FTKPlaneMap *pmap = getSSMap()->getPlaneMap();
  
  for (int iplane=0; iplane!=m_nplanes&&m_useWC; ++iplane) { // loop over the planes
    // create the WC SS
    FTKSS WCSS;

    FTKHit fakehit(pmap->isSCT(iplane) ? 1 : 2);
    // cout <<" WC plane=" <<iplane <<endl;
    WCSS.addHit(fakehit);
    // cout <<"WCSS.addHit(fakehit)"<<endl;

    // add the WC SS to the map of fired WC
    m_fired_ssmap[iplane][m_WCID] = WCSS;
    if (getUseMinimalAMIN()) m_fired_ss[iplane].insert(m_WCID);
  }

  // send real hits into the SS
  for (int iplane=0; iplane!=m_nplanes; ++iplane) { // loop over the planes
     for (vector<FTKHit>::iterator ihit = m_stlhit_sort[iplane].begin();
          ihit!=m_stlhit_sort[iplane].end(); ++ihit ) { // hit loop

        FTKHit &curhit(*ihit);

        int ss(-1);
        if (ftkset.getSectorsAsPatterns() && FTKSetup::getFTKSetup().getHWModeSS()==0) {
          // Using a dummy pattern bank representing just the number of setors, the IDs are the module IDs, for historical reason called sector.
          ss = curhit.getSector();
        }
        else if (ftkset.getSectorsAsPatterns() && FTKSetup::getFTKSetup().getHWModeSS()==2) {
          // Using a dummy pattern bank representing just the number of sectors, the IDs are the module IDs identifier hash
          ss = curhit.getIdentifierHash();
        }
        else if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
          //SS calculated assuming a global SS id
          ss = getSSMap()->getSSGlobal(curhit);
        }
        else {
          // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
          ss = getSSMap()->getSSTower(curhit,getBankID());
        }

       //       cout<<"KAMA"<<endl;
       if(m_WCSS2[iplane] != 0x0 ){
         if(m_WCSS2[iplane]->find(ss) != m_WCSS2[iplane]->end() ){
           // FlagAA 2013-11-27: commenting this out to avoid big printout for pileup events
           //           cout <<"plane:"<<iplane<<" ss:"<<ss<<"  this hit is ignored"<<endl;
           continue;
         }
       }

       // try to add an empty SS objects
       pair<unordered_map<int,FTKSS>::iterator, bool> res = m_fired_ssmap[iplane].insert(make_pair(ss,FTKSS()));
       unordered_map<int,FTKSS>::iterator curss = res.first;
       if ( res.second ) {
         if (m_fired_ssmap[iplane].size() >= MAXSS)
           FTKSetup::PrintMessage(sevr,"am_in:MAXSS exceeded");
         if (getUseMinimalAMIN()) m_fired_ss[iplane].insert(ss);
       }
       (*curss).second.addHit(curhit);
    } // end loop over elms
    naoSSPushBack(m_fired_ssmap[iplane].size());
  } // end loop over the layers
}

/*******************************************
 * Function: am_in()
 * Purpose: FlagAK - new version of am_in()
 *          Rearranges the linked list of hits so that hits in the same SS are grouped together (using which_SS function above).
 *          This is needed to accomodate 2-d partitioning of pixel superstrips.
 * Arguments: none
 * Returns: none
 * Comments:
 *          This is completely unoptimized!!! (i.e. loops in which_SS() can get huge for 10^34)
 *          But it should still be fast compared to the time spent in track fitting.
*******************************************/
void FTK_AMBank::am_in() {
   if (getUseMinimalAMIN()) {
      am_in_minimal();
      return;
  }
#ifdef USEAMIN2
  am_in2();
  return;
#endif

  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-ftkset.getMaxMissingSctPairs();
  m_fired_patts.clear();

  // send real hits into the SS
  for (int iplane=0; iplane!=m_nplanes; ++iplane) { // loop over the planes
    const unsigned int mask(1<<(iplane+m_matchword_maskshift));

    unordered_map<int,FTKSS>::iterator ssiter = m_fired_ssmap[iplane].begin();
    unordered_map<int,FTKSS>::iterator ssiter_end = m_fired_ssmap[iplane].end();
    for (;ssiter!=ssiter_end;++ssiter) { // loop over the SS found in that layer
      const int &curssid((*ssiter).first);

      /* input one SuperStrip to AM */
      hash_map<int,std::vector<int> >::iterator issmap = m_ss_patt_lookup_map[iplane].find(curssid);
      if (issmap != m_ss_patt_lookup_map[iplane].end()) { // pattern match update block
        vector<int>::const_iterator ipatt = (*issmap).second.begin();
        vector<int>::const_iterator ipattE = (*issmap).second.end();
        for(;ipatt!=ipattE;++ipatt) { // loop over the patterns with this SS
          const int &patt = (*ipatt);

          /* the ma<tch status for each pattern is store in an unsigned int word, the least significant 8 bits are reserved
           * for the hitcounter (number of layer matched) while the bits 9-32 are for the bitsmask.
           */
          pattstatus_t &word = m_patterns_matchstatus[patt];
          if(!(word & mask)) { // double check the layer has not been already marked as matched (can be overkilling)
              word |= mask;
              word +=1;

            if ((word&m_matchword_countermask)==match_threshold)
              m_fired_patts.push_back(patt);
          }
        } // end loop over the patterns
      } // end pattern match update block

    } // end loop over the SS found in this layer

  } // loop over planes
  
  if (FTKSetup::getDBG()) {
    for(int i=0;i!=m_nplanes;i++) {
      cout << "DBG: Routed superstrips " << i << " = " << m_fired_ssmap[i].size() << endl;
    }
  }
}


void FTK_AMBank::am_in_minimal() {
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-ftkset.getMaxMissingSctPairs();
  m_fired_patts.clear();

  /* the bias is the criteria that means a road is matched, if remain >0, this is the initial value,
   * the  no matching layer will decrease the bias
   */
  const unsigned int biasinit(ftkset.getMaxMissingPlanes()+FTKSetup::getFTKSetup().getMaxMissingSctPairs()+1);

  for (int ipatt=0;ipatt!=m_npatterns;++ipatt) { // loop over the patterns
    unsigned int bias(biasinit); // initialize the bias
    // retrieve the status word for the current pattern
    pattstatus_t word(0);

    /* loop over the layers of the pattern, stop if too many layers are missing, causing the bias to go to 0 */
    for (int iplane=0;iplane!=m_nplanes&&bias;++iplane) { // lopp over the layers
        // extract the SS of the current position
        const int &curss = m_patterns[_SSPOS(ipatt,iplane)];
        const unsigned int mask(1<<(iplane+m_matchword_maskshift));

        if (curss==m_WCID) { // check if it is a WC
            // update the match
            word |= mask;
            word +=1;
        }
        else if (m_fired_ss[iplane].find(curss)!=m_fired_ss[iplane].end()) {
            // the SS exists in the map of fired patterns
            // update the match status
            word |= mask;
            word +=1;
        }
        else {
            // no match found, decrease the bias
            bias -= 1;
        }
    } // end loop over the planes

    m_patterns_matchstatus[ipatt] = word;

    // track here matched in enough layers
    if (bias) {
        if ((word&m_matchword_countermask)<match_threshold) FTKSetup::PrintMessage(sevr,"Wrong bias");
        m_fired_patts.push_back(ipatt);
    }

  } // end loop over the patterns

  if(FTKSetup::getDBG()) {
    for(int i=0;i<m_nplanes;i++) {
      cout << "DBG: Routed superstrips " << i << " = " << m_fired_ssmap[i].size() << endl;
    }
  }
}


void FTK_AMBank::am_in2() {
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-ftkset.getMaxMissingSctPairs();
  m_fired_patts.clear();

  unsigned int prematch_threshold = m_upperindex ? m_nplanes-m_lutsepplane : m_lutsepplane;
  prematch_threshold -= ftkset.getMaxMissingPlanes()+ftkset.getMaxMissingSctPairs();

  vector<int> prefiredpatts;
  int startplane = m_upperindex ? m_lutsepplane : 0;
  int endplane = m_upperindex ? m_nplanes : m_lutsepplane;

  for (int iplane=startplane;iplane!=endplane;++iplane) { // loop over the mapped planes
    const unsigned int mask(1<<(iplane+m_matchword_maskshift));
    unordered_map<int,FTKSS>::iterator ssiter = m_fired_ssmap[iplane].begin();
    unordered_map<int,FTKSS>::iterator ssiter_end = m_fired_ssmap[iplane].end();
    for (;ssiter!=ssiter_end;++ssiter) { // loop over the SS found in that layer
      const int &curssid((*ssiter).first);

      /* input one SuperStrip to AM */
      hash_map<int,std::vector<int> >::iterator issmap = m_ss_patt_lookup_map[iplane].find(curssid);
      if (issmap != m_ss_patt_lookup_map[iplane].end()) { // pattern match update block
        vector<int>::const_iterator ipatt = (*issmap).second.begin();
        vector<int>::const_iterator ipattE = (*issmap).second.end();
        for(;ipatt!=ipattE;++ipatt) { // loop over the patterns with this SS
          const int &patt = (*ipatt);

          /* the match status for each pattern is store in an unsigned int word, the least significant 8 bits are reserved
           * for the hitcounter (number of layer matched) while the bits 9-32 are for the bitsmask.
           */
          pattstatus_t &word = m_patterns_matchstatus[patt];
          if(!(word & mask)) { // double check the layer has not been already marked as matched (can be overkilling)
              word |= mask;
              word +=1;

            if ((word&m_matchword_countermask)==prematch_threshold)
              prefiredpatts.push_back(patt);
          }
        } // end loop over the patterns
      } // end pattern match update block
    } // end loop over the SSs
  } // end loop over the planes

  startplane = !m_upperindex ? m_lutsepplane : 0;
  endplane = !m_upperindex ? m_nplanes : m_lutsepplane;

  unsigned int biasinit(ftkset.getMaxMissingPlanes()+FTKSetup::getFTKSetup().getMaxMissingSctPairs()+1);
  unsigned int biasmod(m_upperindex ? m_nplanes-m_lutsepplane : m_lutsepplane);

  for (auto iprepatt: prefiredpatts) { // loop over the patterns
    unsigned int bias(biasinit); // initialize the bias
    pattstatus_t &word = m_patterns_matchstatus[iprepatt];
    bias -= biasmod-(word&m_matchword_countermask);

    /* loop over the layers of the pattern, stop if too many layers are missing, causing the bias to go to 0 */
    for (int iplane=startplane;iplane!=endplane&&bias;++iplane) { // lopp over the layers
        // extract the SS of the current position
        const int curss = m_patterns[_SSPOS(iprepatt,iplane)];
        const unsigned int mask(1<<(iplane+m_matchword_maskshift));

        if (curss==m_WCID) { // check if it is a WC
            // update the match
            word |= mask;
            word +=1;
        }
        else if (m_fired_ssmap[iplane].find(curss)!=m_fired_ssmap[iplane].end()) {
            // the SS exists in the map of fired patterns
            // update the match status
            word |= mask;
            word +=1;
        }
        else {
            // no match found, decrease the bias
            bias -= 1;
        }
      } // end loop over the planes

    // track here matched in enough layers
    if (bias) {
        if ((m_patterns_matchstatus[iprepatt]&m_matchword_countermask)<match_threshold) FTKSetup::PrintMessage(sevr,"Wrong bias");
        m_fired_patts.push_back(iprepatt);
    }

  } // end loop over the patterns

  if(FTKSetup::getDBG()) {
    for(int i=0;i<m_nplanes;i++) {
      cout << "DBG: Routed superstrips " << i << " = " << m_fired_ssmap[i].size() << endl;
    }
  }
}

unsigned int FTK_AMBank::getBitmask(int pattid) 
{
  return m_patterns_matchstatus[pattid]>>m_matchword_maskshift;
}

/*******************************************
 * Function: am_output()
 * Purpose: builds the list of roads and saves it in the am_out[ibank][iroad] array
 * Arguments: none
 * Returns: none
 * Comments: 
*******************************************/
void FTK_AMBank::am_output() {
  int MAX_MISSING_PLANES = FTKSetup::getFTKSetup().getMaxMissingPlanes();
  int MAX_MISSING_SCT_PAIRS = FTKSetup::getFTKSetup().getMaxMissingSctPairs();

  clearNRoads();
  int iroads(0); //same as getNRroads(), but keeps total count of roads (even those in excess of MAXROADS)
  const FTKPlaneMap *pmap = getSSMap()->getPlaneMap();

  vector<int>::iterator fpiter = m_fired_patts.begin();
  for (;fpiter!=m_fired_patts.end();++fpiter) {
    int ipatt = *fpiter;
    int nhit = m_patterns_matchstatus[ipatt]&m_matchword_countermask; // filter 8 bits
    if(nhit == m_nplanes) { // complete roads
      if(iroads >= MAXROADS) {
        if(iroads==MAXROADS)
          printf("Maximum %ld roads reached, full pattern %d rejected\n",getNRoads(),ipatt);
        ++iroads;
        continue;
      }
      else {
         countNRoads();
         ++iroads;
      }

      if (getPatternStats()) {
	// increment the effective coverage
	m_pattern_stats[ipatt] += 1;
      }

      // add the road in the list
      m_roads.push_front(FTKRoad(getNRoads()-1,getBankID()+100*getSubID(),
				 ipatt,m_nplanes,
				 nhit,getBitmask(ipatt)));
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

    } else if ( nhit >= m_nplanes-MAX_MISSING_PLANES-MAX_MISSING_SCT_PAIRS ) {
      
      // check requirements on first or last layers
      const unsigned int bitmask = getBitmask(ipatt);
      bool first_check = !getRequireFirst() || (bitmask & 1);
      bool last_check = !getRequireLast() || (bitmask & (1<<(m_nplanes-1)));
      bool missinglast =  !(bitmask & (1 << (m_nplanes-1)));

      if( first_check && last_check && nhit + int(missinglast) >= m_nplanes - MAX_MISSING_PLANES ) {
	// Check for SCT pair restrictions:
	if( missinglast && nhit < m_nplanes-MAX_MISSING_PLANES   && MAX_MISSING_SCT_PAIRS > 0 ) {
	  // 1. Is the last  layer devoid of hits?
	  //if( m_hitbit[m_nplanes-1][ipatt/m_HBseg] & (1<<(ipatt%m_HBseg))   )
	  //  continue;
	  // 2. Are the right modules missing?
	  int ieta1 = abs(m_patterns[_SSPOS(ipatt,m_nplanes-1)])%100;
	  if( !((ieta1==0 ) || (ieta1==12) || (ieta1==21) ||(ieta1==31)) )
	    continue;
	}
	if(iroads >= MAXROADS) {
	  if(iroads==MAXROADS)
	    printf("Maximum %ld roads reached, partial pattern %d rejected\n",getNRoads(),ipatt);
	  ++iroads;
	  continue;
	}   else {
           countNRoads();
	  ++iroads;
	}

	if (getPatternStats()) {
	  // increment the effective coverage
	  m_pattern_stats[ipatt] += 1;
	}
	
	// add the road in the list
	m_roads.push_front(FTKRoad(getNRoads()-1,getBankID()+100*getSubID(),
				   ipatt,m_nplanes,
				   nhit,getBitmask(ipatt))); 
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
	
      } // if allowed
    } // if checking for missing SCT pairs
    else {
      /* at this point a road was ok at standard FTK_AMBank::am_in()
	 level but at further stage could be downgraded */
      if (getSaveAllRoads()) {
	if(iroads >= MAXROADS) {
	  if(iroads==MAXROADS)
	    printf("Maximum %ld roads reached, full pattern %d rejected\n",getNRoads(),ipatt);
	  ++iroads;
	  continue;
	}
	else {
           countNRoads();
	  ++iroads;
	}

	m_roads.push_front(FTKRoad(getNRoads()-1,getBankID()+100*getSubID(),
				   ipatt,m_nplanes,
				   nhit,getBitmask(ipatt)));
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

      }
    }
  }

  naoSetNroadsAM(getNRoads());
  naoSetNroadsAMComplete(getNRoads_complete());
  naoSetNroadsAMMissPix(getNRoads_misspix());
  naoSetNroadsAMMissSCT(getNRoads_misssct());

  if(FTKSetup::getDBG()) {
    cout << "DBG: AM found " << m_roads.size() << " roads" << endl;
  }

  // dress the roads with the SS ids and other information
  attach_SS();
    
#ifdef VERBOSE_DEBUG
  printf("Total %d/%d roads found in bank %d with %d patts\n",getNRoads(),iroads,ibank,m_npatterns);
#endif
}

void hitlist_del(struct hitlist_elm *list) {
  if (list != NULL) {
    hitlist_del(list->cdr);
    delete list;
  }
}

/** this function clears all the lists, structures, and arrays used in 
    the previous event to store the found roads */
void FTK_AMBank::clear()
{
   // clear the list of the roads
  m_roads.clear();

  // clean the map of the used SS
  vector< unordered_map<int,FTKSS> >::iterator mapitem = m_usedssmap.begin();
  for (;mapitem!=m_usedssmap.end();++mapitem) { // loop over planes
     (*mapitem).clear();
  } // end loop over planes


  // clear SS maps
  for (int ip=0;ip!=m_nplanes;++ip) {
      m_fired_ssmap[ip].clear();
      m_fired_ss[ip].clear();
      m_stlhit_sort[ip].clear();
  }

  /* this block performs a loop over the planes to reset
       the bits of the previos event */
  if (!getUseMinimalAMIN()) memset(m_patterns_matchstatus,0,m_npatterns*sizeof(pattstatus_t));

  // reset cluster/ss counters (for Naoki's timing simulation)
  naoClear();

#ifdef VERBOSE_DEBUG
  printf("** am_clear\n\thitbit set to zero\n");
#endif
}


/*******************************************
 * Function: informationMatch()
 * Purpose: applies road warrior between two patterns (indexed by patt1 and patt2)
 * Arguments: bank number and two road ID's that index patterns in pattern[ibank][iplane][ipatt]
 * Returns: 0, if two roads are considered independent, so that both will be kept
            1, if one road is the ghost of another
 * Comments: 
*******************************************/
int FTK_AMBank::informationMatch(FTKRoad *r1,FTKRoad *r2) {
   int patt1=r1->getPatternID();
   int patt2=r2->getPatternID();
  const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const FTKPlaneMap *pmap = getSSMap()->getPlaneMap();

  //int nhits1 = hitbit[ibank][NPLANES][patt1];
  //int nhits2 = hitbit[ibank][NPLANES][patt2];
  int nsame = m_nplanes; // count the number of shared SS

  // pattern group and bit, to extract information from hitbit
  const unsigned int bitmask1 = m_patterns_matchstatus[patt1]>>m_matchword_maskshift;
  const unsigned int bitmask2 = m_patterns_matchstatus[patt2]>>m_matchword_maskshift;

  /* Roadwar>>m_matchword_maskshiftr CORE. FC, FlagAK - added planePair abstraction */
  if(m_patterns[_SSPOS(patt1,m_nplanes)] != m_patterns[_SSPOS(patt2,m_nplanes)]) {
#ifdef VERBOSE_DEBUG
    printf("\t\t\tSECTOR differs\n");
#endif
    if (ftkset.getRoadWarrior()<=1)
      return 0; // If ROADWARRIOR=0 this means always different patterns
  }

  for(int i = 0; i < m_nplanes; ++i) { // loop to count the shared SS
    if(!pmap->isSCT(i)) { // Pixel plane
      /* pattern[][][] has the strips used by each pattern, if there is a
	 difference the two roads could be different.
	 The different is meaningful only if the two roads are empty in both
	 patterns or if the patt2 is empty while patt1 is a complete pattern*/
      if ( (m_patterns[_SSPOS(patt1,i)]!=m_patterns[_SSPOS(patt2,i)]) &&
          (( (bitmask1 & (1<<i)) && (bitmask2&(1<<i)) )
              ||
              ( (bitmask1&(1<<i))==0 && (bitmask2&(1<<i)) )) ) {
        /* The bitmaskrequirement is sufficient because because if patt1
	   has 1 empty SS patt2 has at least a different empty SS,
	   for this reason an empty SS in patt2 can be suspected to be
	   a ghost road while an empty SS in patt1 doesn-t */

        nsame -= 1;
        return 0;
      }
    } else { // SCT plane
      int ipair = pmap->getPlanePair(i);
      bool condition1 = (m_patterns[_SSPOS(patt1,i)]!=m_patterns[_SSPOS(patt2,i)]) &&
          (( (bitmask1 & (1<<i)) && (bitmask2&(1<<i)) ) ||
          ( (bitmask1&(1<<i))==0 && (bitmask2&(1<<i)) ));

      // if SCT plane pairing is not used both conditions are the same
      bool condition2 (condition1);
      if (i!=ipair) { // pairing is used
	condition2 = (m_patterns[_SSPOS(patt1,ipair)]!=m_patterns[_SSPOS(patt2,ipair)]) &&
	  (((bitmask1&(1<<i)) && (bitmask2&(1<<i)))
	   ||
	   ((bitmask1&(1<<i))==0 && (bitmask2&(1<<i)) ));
	if (ipair==i+1) {
	  // the 1st SCT plane can be paired to the next,
	  // if it is the case the next step can be skipped
	  i += 1; // skip one step
	}
      }

      if ( condition1 && condition2 ) {
	// here if the SS of the two patterns in this plane differs
#ifdef VERBOSE_DEBUG
	printf("\t\t\tSCT %d and %d (%u) differs\n",m_patterns[_SSPOS(patt1,i)],
	       m_patterns[_SSPOS(patt2,i)],bitmask2);
#endif
	nsame -= 1;
	return 0;
      }
    }
  } // end loop to count the shared SS

#ifdef VERBOSE_DEBUG
  printf("\t\t\t%d and %d differ for %d super-strips\n",patt2,patt1,nsame);
#endif

  if (nsame==m_nplanes)
    return 1;
  else 
    return 0;
}


/*******************************************
 * Function: setBadSSMap()
 * Purpose: read bad module map and set bad ss map for wild card algorithm.
 * Arguments: none
 * Returns: none
 * Comments: 
*******************************************/
void FTK_AMBank::setBadSSMap(){
  // read dead moduel
  ifstream mapfile_BadModuleMap(getBadSSMapPath());
  const FTKPlaneMap *pmap = getSSMap()->getPlaneMap();
  m_bad_module_map = new list<FTKHit> [m_nplanes];
  
  //read ASCII file
  cout << "BAD_MODULES_DEBUG: REGION PLANE moduleSector" << endl;
  string curline;
  while( getline(mapfile_BadModuleMap, curline) ){
    int tmpisPixel,tmpBEC,tmpSector,tmpPlane,tmpEtaModule,tmpPhiModule,tmpSection;
    unsigned int tmpidhash;

    istringstream curlinestr(curline);
    curlinestr >>tmpisPixel >>tmpBEC >>tmpSector >>tmpPlane >>tmpEtaModule >>tmpPhiModule >>tmpSection;
    curlinestr >> tmpidhash;
    if (!curlinestr) tmpidhash = 0; // error is assumed to be because the idhash is missing, old file version
    
    FTKHit tmpmodhit(tmpisPixel > 0 ? 2 : 1);
    
    tmpmodhit.setPlane(tmpPlane);
    tmpmodhit.setSector(tmpSector);
    tmpmodhit.setIdentifierHash(tmpidhash);
    if (getSSMap()->getRegionMap()->isHitInRegion(tmpmodhit,getBankID())) {
      cout << "BAD_MODULES_DEBUG: " << getBankID() << " \t" << tmpPlane << " \t" << tmpSector << endl;
      m_bad_module_map[tmpPlane].push_back(tmpmodhit);
    }
  }
  
  //get bad SS map
  for (int ip=0;ip!=m_nplanes;++ip) {
    set<int> *tmpset = new set<int>;
    for( list<FTKHit>::const_iterator itemod = m_bad_module_map[ip].begin();itemod != m_bad_module_map[ip].end() ; ++itemod){
      FTKHit curss(*itemod);
      if(pmap->isSCT((*itemod).getPlane())){//for SCT    
	for (int locPhi=0;locPhi<getSSMap()->getSSPhiSize(*itemod);locPhi += getSSMap()->getSSPhiWidth(*itemod)) {
	  curss[0] = locPhi;
	  tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? getSSMap()->getSSGlobal(curss) : getSSMap()->getSSTower(curss,getBankID()));
	}
      }
      else if(pmap->isPixel((*itemod).getPlane())){//for Pixel                                                                                                                              
	for (int locEta=0;locEta<getSSMap()->getSSEtaSize(*itemod);locEta += getSSMap()->getSSEtaWidth(*itemod)) {
	  curss[1] = locEta;
	  for (int locPhi=0;locPhi<getSSMap()->getSSPhiSize(*itemod);locPhi += getSSMap()->getSSPhiWidth(*itemod)) {
	    curss[0] = locPhi;
	    tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? getSSMap()->getSSGlobal(curss) : getSSMap()->getSSTower(curss,getBankID()));
	  }
	}
      }
    }

    // assign the temporary list of involved SS only if the list is not empty
    if (!tmpset->empty()) m_WCSS[ip] = tmpset;
    else delete tmpset;
  }
  
  
#if 0 //Dump m_WCSS. (for debug!)
  ofstream mapfile_BadModuletest("badSS_test.txt");    
  for (int ip=0;ip!=m_nplanes;++ip) {
    mapfile_BadModuletest <<"P  " <<ip <<" -1" <<endl;
    for(set<int>::iterator ite = m_WCSS[ip]->begin(); ite != m_WCSS[ip]->end(); ++ite ){
      mapfile_BadModuletest << (*ite) <<endl;
    }
  }
  mapfile_BadModuletest.close();
#endif
  
  //clear
  for(int ip=0 ;ip != m_nplanes;++ip){
    m_bad_module_map[ip].clear();
  }
}


/*******************************************
 * Function: setBadSSMap2()
 * Purpose: read bad module map and set bad ss map(m_WCSS2) for ignoring hit if it is in bad module.
 * Arguments: none
 * Returns: none
 * Comments: 
*******************************************/
void FTK_AMBank::setBadSSMap2(){
  // read dead moduel
  ifstream mapfile_BadModuleMap(getBadSSMapPath2());
  const FTKPlaneMap *pmap = getSSMap()->getPlaneMap();
  m_bad_module_map = new list<FTKHit> [m_nplanes];
  
  //read ASCII file
  string curline;
  while( getline(mapfile_BadModuleMap, curline) ){
    int tmpisPixel,tmpBEC,tmpSector,tmpPlane,tmpEtaModule,tmpPhiModule,tmpSection;
    unsigned int tmpidhash;

    istringstream curlinestr(curline);
    curlinestr >>tmpisPixel >>tmpBEC >>tmpSector >>tmpPlane >>tmpEtaModule >>tmpPhiModule >>tmpSection;
    curlinestr >> tmpidhash;
    if (!curlinestr) tmpidhash = 0; // error is assumed to be because the idhash is missing, old file version
    
    FTKHit tmpmodhit(tmpisPixel > 0 ? 2 : 1);
    
    tmpmodhit.setPlane(tmpPlane);
    tmpmodhit.setSector(tmpSector);
    tmpmodhit.setIdentifierHash(tmpidhash);
    if (getSSMap()->getRegionMap()->isHitInRegion(tmpmodhit,getBankID())) {
      m_bad_module_map[tmpPlane].push_back(tmpmodhit);
    }
  }
  
  //get bad SS map
  for (int ip=0;ip!=m_nplanes;++ip) {
    set<int> *tmpset = new set<int>;
    for( list<FTKHit>::const_iterator itemod = m_bad_module_map[ip].begin();itemod != m_bad_module_map[ip].end() ; ++itemod){
      FTKHit curss(*itemod);
      if(pmap->isSCT((*itemod).getPlane())){//for SCT    
	for (int locPhi=0;locPhi<getSSMap()->getSSPhiSize(*itemod);locPhi += getSSMap()->getSSPhiWidth(*itemod)) {
	  curss[0] = locPhi;
	  tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? getSSMap()->getSSGlobal(curss) : getSSMap()->getSSTower(curss,getBankID()));
	}
      }
      else if(pmap->isPixel((*itemod).getPlane())){//for Pixel                                                                                                                              
	for (int locEta=0;locEta<getSSMap()->getSSEtaSize(*itemod);locEta += getSSMap()->getSSEtaWidth(*itemod)) {
	  curss[1] = locEta;
	  for (int locPhi=0;locPhi<getSSMap()->getSSPhiSize(*itemod);locPhi += getSSMap()->getSSPhiWidth(*itemod)) {
	    curss[0] = locPhi;
	    tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? getSSMap()->getSSGlobal(curss) : getSSMap()->getSSTower(curss,getBankID()));
	  }
	}
      }
    }

    // check if there are SS to be vetoed, if not the memory is cleaned
    if (tmpset->empty()) delete tmpset;
    else m_WCSS2[ip] = tmpset;
  }
  
  //clear
  for(int ip=0 ;ip != m_nplanes;++ip){
    m_bad_module_map[ip].clear();
  }
}

/*******************************************
 * Function: getPattern()
 * Purpose: read a pattern
 * Arguments: the pattern index
 * Returns: pattern i
 * Comments: 
*******************************************/
FTKPattern FTK_AMBank::getPattern(int ipatt){
  FTKPattern patt;
  patt.setPatternID(ipatt);
  patt.setNPlanes(m_nplanes);

  for (int iplane=0;iplane!=m_nplanes;++iplane) { // loop over the planes
    patt.setSSID(iplane,m_patterns[_SSPOS(ipatt,iplane)]);
  }
  patt.setSectorID(m_patterns[_SSPOS(ipatt,m_nplanes)]); // the last position [of m_patterns] is the sectord ID
  patt.setCoverage(m_patternCoverage[ipatt]);

  return patt;
}

#ifdef TESTTBB
FTK_AMBank::DataOrganizer::DataOrganizer(FTK_AMBank *amobj) :
    m_amobj(amobj)
{
  //NOTHING more
}
void FTK_AMBank::DataOrganizer::operator() ( const blocked_range<unsigned int>& r ) const {
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  for( unsigned int iplane=r.begin(); iplane!=r.end(); ++iplane ) {
    struct hitlist_elm *h_elm = m_amobj->m_rout_out[iplane];
    while (h_elm != NULL) { // loop over h_elm
       int ss(-1);
       if (ftkset.getSectorsAsPatterns()) {
         // Using a dummy pattern bank representing just the number of setors, the IDs are the module IDs, for historical reason called sector.
         ss = h_elm->car->getSector();
       }
       else if (m_amobj->m_HWModeSS==0) {
         //SS calculated assuming a global SS id
         ss = m_amobj->getSSMap()->getSSGlobal(*(h_elm->car));
       }
       else {
         // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
         ss = m_amobj->getSSMap()->getSSTower(*(h_elm->car),m_amobj->getBankID());
       }

       //       cout<<"KAMA"<<endl;
       if(m_amobj->m_WCSS2[iplane] != 0x0 ){
         if(m_amobj->m_WCSS2[iplane]->find(ss) != m_amobj->m_WCSS2[iplane]->end() ){
           cout <<"plane:"<<iplane<<" ss:"<<ss<<"  this hit is ignored"<<endl;
           h_elm = h_elm->cdr;
           continue;
         }
       }

       // try to add an empty SS objects
       pair<unordered_map<int,FTKSS>::iterator, bool> res = m_amobj->m_fired_ssmap[iplane].insert(make_pair(ss,FTKSS()));
       unordered_map<int,FTKSS>::iterator curss = res.first;
       if ( res.second ) {
         if (m_amobj->m_fired_ssmap[iplane].size() >= MAXSS)
           FTKSetup::PrintMessage(sevr,"am_in:MAXSS exceeded");
       }
       (*curss).second.addHit(*(h_elm->car));
       h_elm = h_elm->cdr;
    } // end loop over elms
    m_amobj->m_nao_nss.push_back(m_amobj->m_fired_ssmap[iplane].size());
  }
}
#endif
