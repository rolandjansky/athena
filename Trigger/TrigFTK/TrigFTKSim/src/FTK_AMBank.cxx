/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/FTKRoadKDTree.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKPattern.h"
using namespace ftk;

#include <TFile.h>
#include <TTree.h>

#include  <time.h>

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

const unsigned char FTK_AMBank::m_matchword_countermask((1<<4)-1); // maximum number of matches, it has to be a 2^N-1
const unsigned char FTK_AMBank::m_matchword_maskshift(4);

#define USEKDTREE

FTK_AMBank::FTK_AMBank(int id, int subid)
  : m_BankID(id), m_SubID(subid), 
    m_CachedBank(false),
    m_ssmap(0),
    m_nplanes(0),
    m_npatterns(0),
    m_patterns(0),
    m_patternCoverage(0),
    m_sectorCoverage(0),
    m_totalCoverage(0),
    m_patterns_matchstatus(0x0),
    m_nroads(0),
    m_amout(0),
    m_do_pattern_stats(0), m_pattern_stats(0x0),
    m_StoreAllSS(false),
    m_useWC(false),
    m_useMinimalAMIN(false),
    m_nao_nroads_am(0), m_nao_nroads_rw(0),
    m_ss_patt_lookup_map(0),
    m_lutsepplane(3), m_upperindex(true),
    m_require_first(false), m_require_last(false),
    m_stlhit_sort(0x0),
    m_SaveAllRoads(0)
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
   m_stlhit_sort = new vector<FTKHit>[m_nplanes];

   // resize the ss map, could save something
   m_usedssmap.clear();
   m_usedssmap.reserve(m_nplanes);
   for (int ipl=0;ipl!=m_nplanes;++ipl) {
      m_usedssmap.push_back(unordered_map<int,FTKSS>());
   }

   m_stat_totroads = 0.;
   m_stat_nevents = 0;
}


/** End of the session, this method is called before the destructors
    are called, all the objects have valid pointers */
void FTK_AMBank::end()
{
  if (m_do_pattern_stats) {
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
   // create the maps where the SS map used in the events are stored
   m_fired_ssmap = new unordered_map<int,FTKSS>[m_nplanes];
   m_fired_ss = new unordered_set<int>[m_nplanes];

   // allocate the space to store the patterns, each pattern is composed by m_nplanes+1 numbers, the SSs and the sectors ID
   m_patterns = new int[(m_nplanes+1)*m_npatterns];

   // allocate the compressed structure that store the match status for the patterns
   m_patterns_matchstatus = new unsigned int[m_npatterns];

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
   ifstream checkbadmodpath(m_badmap_path.c_str());
   ifstream checkbadmodpath2(m_badmap_path2.c_str());
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
   m_ss_patt_lookup_map = new hash_map<int,std::vector<int> >[m_nplanes];
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
  m_VetoID = ~(~0<<(sizeof(int)*8-1));
  m_WCID = m_VetoID-1;

  unsigned int *m_WC_stat = new unsigned int[m_nplanes]; // store the static collected on the wildcard
  unsigned int *m_WC_stat_nWClayers = new unsigned int[m_nplanes+1]; // store the number of patterns with N WC layers
  unsigned int *m_WC_stat_patt = new unsigned int[m_npatterns]; // store the static collected on the wildcard
 
  for (int ip=0;ip!=m_nplanes;++ip) m_WC_stat[ip] = 0;
  for (int ip=0;ip!=m_nplanes+1;++ip) m_WC_stat_nWClayers[ip] = 0;
  for (int ip=0;ip!=m_npatterns;++ip) m_WC_stat_patt[ip] = 0;
  
  std::set<int> m_WC_patterns; // list of patterns where the wildcard was applied
  std::set<int> m_Veto_patterns ; // list of patterns where the veto was applied

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
	m_WC_stat[iplane]++;
 	m_WC_stat_patt[ipatt]++;
 	m_WC_patterns.insert(ipatt);
 	m_patterns[_SSPOS(ipatt,iplane)] =  m_WCID;
      }
    } // end loop over the patterns
  } // end plane loop
  
  // print the debug messages to inform of which is the status of the WC
  cout << "Tower " << m_BankID << " number of patterns with the wildcard: " << m_WC_patterns.size() << endl;
  for (int ip=0;ip!=m_nplanes;++ip) {
    cout << "Tower " << m_BankID << " number of WC on layer " << ip << ": " << m_WC_stat[ip] << endl;
  }

#if 0   //Dump the number of WC on pattern(for debug!) 
  ofstream mapfile_WC_patt("nWC_patt.dat");
  for(int tmp_ipatt=0;tmp_ipatt != m_npatterns;++tmp_ipatt){
    mapfile_WC_patt <<tmp_ipatt <<" "<<m_WC_stat_patt[tmp_ipatt]<<endl;
    if(tmp_ipatt%10000 == 0 ) cout <<"write patt "<<tmp_ipatt<<endl;
  }
  mapfile_WC_patt.close();
#endif

 if (m_WC_patterns.empty()) {
   FTKSetup::PrintMessage(info,"No wildcards applied");
   return;
 }
 else {
   cout <<" check duplicated patterns and remove them using the veto ID"<<endl;
    // loop over the patterns to check duplicated patterns and remove them using the veto ID
   set<int>::iterator ipatt_ref = m_WC_patterns.begin();
   set<int>::iterator ipatt_end = m_WC_patterns.end();
   // GV,AA: comments for speed up: index m_WC_patterns with SSID 
   //        make a KDTree that stores the pattern with all SSIDs
   //        the tree must be sorted by SSID (prioritized by layer)
   //        to make the search logarithmic
   for (;ipatt_ref!=ipatt_end;++ipatt_ref) { // loop over the patterns with the WC
     int nWC = 0;
     for (int wcplane=0;wcplane!=m_nplanes;++wcplane) { // loop over the planes 
       if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] ==  m_WCID) nWC++;
     } 
     m_WC_stat_nWClayers[nWC]++;	  

     // veto patterns that have 3 or more m_WCID set.
     if (nWC>=3) {
       m_Veto_patterns.insert(*ipatt_ref);
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
	 m_Veto_patterns.insert(*ipatt_chk);
	 for (int ip=0;ip!=m_nplanes;++ip) m_patterns[_SSPOS(*ipatt_chk,ip)] = m_VetoID;
       }
     } // end loop to search for duplicates
   } // end loop over the patterns with the WC
   cout <<"Number of veto patterns:"<<m_Veto_patterns.size()<<endl;
   for (int ip=0;ip<=m_nplanes;++ip) {
     cout << "Tower " << m_BankID << " number of patterns with WC on " << ip << " layers : " << m_WC_stat_nWClayers[ip] << endl;
   }

   cout << "Count nWC and nVeto again for debug" << endl;
   ipatt_ref = m_WC_patterns.begin();
   int nWC0Veto1p=0;
   int nWC1pVeto1p=0;
   int nWC0Veto1=0;
   int nWC1Veto1=0;
   int nWC2pVeto1=0;
   for (int ip=0;ip!=m_nplanes+1;++ip) m_WC_stat_nWClayers[ip] = 0;
   for (;ipatt_ref!=ipatt_end;++ipatt_ref) { // loop over the patterns with the WC
     int nWC = 0;
     int nVeto = 0;
     for (int wcplane=0;wcplane!=m_nplanes;++wcplane) { // loop over the planes 
       if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] ==  m_WCID) nWC++;
       if (m_patterns[_SSPOS(*ipatt_ref,wcplane)] ==  m_VetoID) nVeto++;
     } 
     if (nVeto==0)
       m_WC_stat_nWClayers[nWC]++;
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
     cout << "DEBUG: Tower " << m_BankID << " number of patterns with WC on " << ip << " layers : " << m_WC_stat_nWClayers[ip] << endl;
   }
   cout << "nWC0Veto1p=" << nWC0Veto1p << endl;
   cout << "nWC1pVeto1p=" << nWC1pVeto1p << endl;
   cout << "nWC0Veto1=" << nWC0Veto1 << endl;
   cout << "nWC1Veto1=" << nWC1Veto1 << endl;
   cout << "nWC2pVeto1=" << nWC2pVeto1 << endl;
  
#if 0 // Dump WC & veto patterns(for debug!)
    ofstream mapfile_Veto_patt("Veto_patt.dat");
    for( set<int>::iterator ite_Veto = m_Veto_patterns.begin(); ite_Veto != m_Veto_patterns.end() ; ++ite_Veto){
      mapfile_Veto_patt <<(*ite_Veto) <<endl;
    }
    
    ofstream mapfile_WC_patt("WC_patt.dat");
    for( set<int>::iterator ite_WC = m_WC_patterns.begin(); ite_WC != m_WC_patterns.end() ; ++ite_WC){
      mapfile_WC_patt <<(*ite_WC) <<endl;
    }
    
#endif
  }
  
  // clear the memory
  delete [] m_WC_stat;
  delete [] m_WC_stat_patt;
  delete [] m_WC_stat_nWClayers;
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
   pattfile >> m_npatterns >> m_nplanes;
   
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
   
   // define the step
   ipatt_step = (m_npatterns+9)/10;
   if (!ipatt_step) ipatt_step = 1;

   cout << "Reading : [" << flush;
   for (int ipatt=0;ipatt<m_npatterns;++ipatt) { // loop over patterns
     int ival;
      int npatt;
      pattfile >> npatt;
      if (npatt!=ipatt) {
	 cerr << "*** error reading the pattern " << ipatt << endl;
	 return -3;
      }
      else if (ipatt%ipatt_step==0) {
	cout << ipatt/ipatt_step << flush;
      }
      for (int iplane=0;iplane<m_nplanes+1;++iplane) { // loop on the planes
	 // the m_nplnes+1 element is the number of the sector
	 // the other are the SS index
	 pattfile >> ival;

	 // TODO: check if the SS should use the WC

	 m_patterns[_SSPOS(ipatt,iplane)] = ival;
      } // end loop on the planes

      // number of events not used, temporary in npatt
      pattfile >> ival;
      m_patternCoverage[ipatt] = ival;
      m_totalCoverage += ival;
     (*m_sectorCoverage)[m_patterns[_SSPOS(ipatt,m_nplanes)]] += m_patternCoverage[ipatt];

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

// nothing to do for a regular bank at the moment
int FTK_AMBank::readROOTBankCache(const char */*fname*/)
{
  return -1;
}

/** This method read the Pattern Bank file stored
    in a ROOT file format. It returns the number of 
    the read pattern, <0  if there was an error */
int FTK_AMBank::readROOTBank(const char *fname, int maxpatt)
{
  // try to open the input file using the ROOT wrapper,
  // fname could contain protocol specfication at the begin
  TFile *pattfile = TFile::Open(fname);

   if (!pattfile) { // error opening the file
     cerr << "*** Error opening the file: " << fname << endl;
     return -1;
   }

   cout << "Read ROOT file format bank: " << fname << endl;
   // look for the main TTree containing the patterns
   TTree *patttree = (TTree*) pattfile->Get("AMPatterns");
   if (!patttree) {
     cerr << "*** AMPatterns doesn't exist in: " << fname << endl;
     return -1;
   }
   
   // get the number of entries
   m_npatterns = patttree->GetEntries();

   // if not patterns are found just exit
   if (!m_npatterns) return 0;

   // create the connection with the list of the patterns
   FTKPattern *tmppatt = new FTKPattern();
   patttree->SetBranchAddress("pattern",&tmppatt);

   // use the first pattern to extract generic information
   patttree->GetEntry(0);
   m_nplanes = tmppatt->getNPlanes();
   
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
   
   // define the step
   ipatt_step = (m_npatterns+9)/10;
   if (!ipatt_step) ipatt_step = 1;

   cout << "Reading : [" << flush;
   for (int ipatt=0;ipatt<m_npatterns;++ipatt) { // loop over patterns
     // move the TTree in the ipatt position
     patttree->GetEntry(ipatt);

     for (int iplane=0;iplane<m_nplanes;++iplane) { // loop on the planes
       // get the SSID for this plane
       m_patterns[_SSPOS(ipatt,iplane)] = tmppatt->getSSID(iplane);
     } // end loop on the planes

     // the last position is the sectord ID
     m_patterns[_SSPOS(ipatt,m_nplanes)] = tmppatt->getSectorID();

     // read the pattern coverage
     m_patternCoverage[ipatt] = tmppatt->getCoverage();
     m_totalCoverage += m_patternCoverage[ipatt];
     (*m_sectorCoverage)[m_patterns[_SSPOS(ipatt,m_nplanes)]] += m_patternCoverage[ipatt];

   } // end loop over patterns
   cout << "]" << endl;

   readBankInitEnd();

   // return the number of the patterns in the bank
   return m_npatterns;
}


// STL hashmap version of lookup table:
void FTK_AMBank::pattlookup_make_map()
{
  if (m_useMinimalAMIN) {
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
      if(!(i%ipatt_step)) cout << "Layer " << iplane << ": counting patterns per ss (" << i << ")" << endl;
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


/** this function get the hit list and then populate the associative
    memory */
int FTK_AMBank::passHits(const vector<FTKHit> &hitlist)
{
   // this function partially replicate the structure of the old 
   // ftksim main loop, sorting, organizing, and populatig the AM

  // che if  the number of patterns is 0
  if (!m_npatterns)
    return 0;

  clear();  

  sort_hits(hitlist);
    
  //readout_hits();
  
  //routing();


  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  
  if (ftkset.getEnableFTKSim()) {
      am_in();
  }

  am_output();

  if (FTKSetup::getFTKSetup().getRoadWarrior()>0)
    road_warrior();

  m_stat_totroads += m_nroads;
  m_stat_nevents += 1;

  return m_nroads;
}


/** this method elaborate the hits in the layer not used in the pattern 
    matching, those hits are read and grouped according the SS map
    in super-strip */
int FTK_AMBank::passHitsUnused(const vector<FTKHit> &hitlist)
{
  int res(0);

  // clear the memory used to store the SS results
  m_usedssmap_ignored.clear();

  vector<FTKHit>::const_iterator ihit = hitlist.begin();
  for (;ihit!=hitlist.end();++ihit) { // hit loop
    // reference to the current hit
    const FTKHit &tmphit = (*ihit);

    // by convention the hits in the ignored layers have negative plane number
    int plane = tmphit.getPlane();
    // search the map for this plane
    map<int, map<int,FTKSS> >::iterator issmap = m_usedssmap_ignored.find(plane);
    if (issmap==m_usedssmap_ignored.end()) {
      m_usedssmap_ignored[plane] = map<int,FTKSS>();
      issmap = m_usedssmap_ignored.find(plane);
    }
    // reference the map for this plane
    map<int,FTKSS> &ssmap= (*issmap).second;
    // identify the SS on the given plane
    int ssid = FTKSetup::getFTKSetup().getHWModeSS()==0 ? m_ssmap_unused->getSSGlobal(tmphit) : m_ssmap_unused->getSSTower(tmphit,m_BankID);
    
    if (ssid<0) {
      // This means that hits had a recoverable issue and has to be skipped
      continue;
    }

    map<int,FTKSS>::iterator issitem = ssmap.find(ssid);
    if (issitem==ssmap.end()) {
      // ad the item related to this SS number
      ssmap[ssid] = FTKSS();
      issitem = ssmap.find(ssid);
    }
    
    FTKSS &ss = (*issitem).second;
    ss.addHit(tmphit);
  } // end hit loop
  return res;
}


void FTK_AMBank::attach_SS() {
  
  list<FTKRoad>::iterator iroad = m_roads.begin();
  for (;iroad!=m_roads.end();++iroad) {
    FTKRoad &last_road = *iroad; 
    int pattID = last_road.getPatternID();
    
    last_road.setSectorID(m_patterns[_SSPOS(pattID,m_nplanes)]);

    // retrieve the hits
    for (int ipl=0;ipl<m_nplanes;++ipl) {
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
const list<FTKRoad>& FTK_AMBank::getRoads()
{
  // FlagJT: moved ~half of this function to attach_SS() so that we
  // can take advantage of the KD Tree in road_warrior().
  list<FTKRoad>::iterator iroad = m_roads.begin();
  for (;iroad!=m_roads.end();++iroad) {
    int pattID = iroad->getPatternID();

    /* retrieve the hits connecting preparing the SS to be saved.
       the connection is skipped if the whole map of SS is required for AUX-card related studies. 
    */
    for (int ipl=0;(ipl<m_nplanes)&&(!m_StoreAllSS);++ipl) {
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
  return m_roads;
}


/** return the list of the super-strips used in a good road.
    The list of the strips if filled using the getRoads() method,
for this reason that method has to be called before */
const unordered_map<int,FTKSS>& FTK_AMBank::getStrips(int plane)
{
  if (m_StoreAllSS) {
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
     if (!m_ssmap->getRegionMap()->isHitInRegion(hitref,m_BankID))
	continue; // skip if the is not in this regione

     m_stlhit_sort[curplane].push_back(hitref);
  } // end loop over the hits

  for (int iplane=0;iplane!=m_nplanes;++iplane) m_nao_nclus.push_back(m_stlhit_sort[iplane].size());
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
  if (m_useMinimalAMIN) {
      am_in_minimal();
      return;
  }
#ifdef USEAMIN2
  am_in2();
  return;
#endif

  int npatt, i(0);
  
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-FTKSetup::getFTKSetup().getMaxMissingSctPairs();
  m_fired_patts.clear();
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();
  
  // send the WC
  for (int iplane=0; iplane<m_nplanes&&m_useWC; ++iplane) { // loop over the planes
    // create the WC SS
    FTKSS WCSS;
    
    FTKHit fakehit(pmap->isSCT(iplane) ? 1 : 2); 
    // cout <<" WC plane=" <<iplane <<endl;
    WCSS.addHit(fakehit);
    // cout <<"WCSS.addHit(fakehit)"<<endl;
    
    // add the WC SS to the map of fired WC
    m_fired_ssmap[iplane][m_WCID] = WCSS;
    m_fired_ss[iplane].insert(m_WCID);

    /* input one SuperStrip to AM */
    npatt = m_ss_patt_lookup_map[iplane].find(m_WCID) == m_ss_patt_lookup_map[iplane].end() ? 0 : m_ss_patt_lookup_map[iplane][m_WCID].size();
	 
    for(i=0;i<npatt;++i) {
      // tricky indexing to keep it identical to default old lookup map structure:
      int patt = m_ss_patt_lookup_map[iplane][m_WCID][i];

      /* the ma<tch status for each pattern is store in an unsigned int word, the least significant 8 bits are reserved
       * for the hitcounter (number of layer matched) while the bits 9-32 are for the bitsmask.
       */
      unsigned int &word = m_patterns_matchstatus[patt];
      unsigned int mask(1<<(iplane+m_matchword_maskshift));
      if(!(word & mask)) { // double check the layer has not been already marked as matched (can be overkilling)
          word |= mask;
          word +=1;
      }
    }
  } // end loop over the planes



#ifdef TESTTBB
  DataOrganizer DOobj(this);
  tbb::parallel_for(tbb::blocked_range<unsigned int>(0,m_nplanes), DOobj);
#endif

  // send real hits into the SS
  for (int iplane=0; iplane<m_nplanes; ++iplane) { // loop over the planes
#ifndef TESTTBB
    for (vector<FTKHit>::iterator ihit = m_stlhit_sort[iplane].begin();
          ihit!=m_stlhit_sort[iplane].end(); ++ihit ) { // hit loop

        FTKHit &curhit(*ihit);

       int ss(-1);
       if (ftkset.getSectorsAsPatterns()) {
         // Using a dummy pattern bank representing just the number of setors, the IDs are the module IDs, for historical reason called sector.
         ss = curhit.getSector();
       }
       else if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         //SS calculated assuming a global SS id
         ss = m_ssmap->getSSGlobal(curhit);
       }
       else {
         // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
         ss = m_ssmap->getSSTower(curhit,m_BankID);
       }

       //       cout<<"KAMA"<<endl;
       if(m_WCSS2[iplane] != 0x0 ){
         if(m_WCSS2[iplane]->find(ss) != m_WCSS2[iplane]->end() ){
	   // FlagAA 2013-11-27: commenting this out to avoid big printout for pileup events
	   //           cout <<"plane:"<<iplane<<" ss:"<<ss<<"  this hit is ignored"<<endl;
           //h_elm = h_elm->cdr;
           continue;
         }
       }

       // try to add an empty SS objects
       pair<unordered_map<int,FTKSS>::iterator, bool> res = m_fired_ssmap[iplane].insert(make_pair(ss,FTKSS()));
       unordered_map<int,FTKSS>::iterator curss = res.first;
       if ( res.second ) {
         if (m_fired_ssmap[iplane].size() >= MAXSS)
           FTKSetup::PrintMessage(sevr,"am_in:MAXSS exceeded");
       }
       (*curss).second.addHit(curhit);
       //h_elm = h_elm->cdr;
    } // end loop over elms
    m_nao_nss.push_back(m_fired_ssmap[iplane].size());
#endif

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
          unsigned int &word = m_patterns_matchstatus[patt];
          unsigned int mask(1<<(iplane+m_matchword_maskshift));
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
  
  if(FTKSetup::getDBG()) {
    for(int i=0;i<m_nplanes;i++) {
      cout << "DBG: Routed superstrips " << i << " = " << m_fired_ssmap[i].size() << endl;
    }
  }
}


void FTK_AMBank::am_in_minimal() {
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-FTKSetup::getFTKSetup().getMaxMissingSctPairs();
  m_fired_patts.clear();
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();

  for (int iplane=0; iplane<m_nplanes&&m_useWC; ++iplane) { // loop over the planes
    // create the WC SS
    FTKSS WCSS;

    FTKHit fakehit(pmap->isSCT(iplane) ? 1 : 2);
    // cout <<" WC plane=" <<iplane <<endl;
    WCSS.addHit(fakehit);
    // cout <<"WCSS.addHit(fakehit)"<<endl;

    // add the WC SS to the map of fired WC
    m_fired_ssmap[iplane][m_WCID] = WCSS;
    m_fired_ss[iplane].insert(m_WCID);
  }

  // send real hits into the SS
  for (int iplane=0; iplane!=m_nplanes; ++iplane) { // loop over the planes
      for (vector<FTKHit>::iterator ihit = m_stlhit_sort[iplane].begin();
           ihit!=m_stlhit_sort[iplane].end(); ++ihit ) { // hit loop

          FTKHit &curhit(*ihit);

          int ss(-1);
          if (ftkset.getSectorsAsPatterns()) {
              // Using a dummy pattern bank representing just the number of setors, the IDs are the module IDs, for historical reason called sector.
              ss = curhit.getSector();
          }
          else if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
              //SS calculated assuming a global SS id
              ss = m_ssmap->getSSGlobal(curhit);
          }
          else {
              // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
              ss = m_ssmap->getSSTower(curhit,m_BankID);
          }

          //       cout<<"KAMA"<<endl;
          if(m_WCSS2[iplane] != 0x0 ){
              if(m_WCSS2[iplane]->find(ss) != m_WCSS2[iplane]->end() ){
                  // FlagAA 2013-11-27: commenting this out to avoid big printout for pileup events
                  //           cout <<"plane:"<<iplane<<" ss:"<<ss<<"  this hit is ignored"<<endl;
                  continue;
              }
          }

          // Add the current SS in the map
          pair<unordered_map<int,FTKSS>::iterator, bool> res = m_fired_ssmap[iplane].insert(make_pair(ss,FTKSS()));
          // the iterator is always valid, even if the ss key already existed, in this case
          // points to the existing location
          unordered_map<int,FTKSS>::iterator curss = res.first;
          if ( res.second ) {
              if (m_fired_ssmap[iplane].size() >= MAXSS)
                FTKSetup::PrintMessage(sevr,"am_in:MAXSS exceeded");
              m_fired_ss[iplane].insert(ss);
          }
          // add the current hit within the SS location
          (*curss).second.addHit(curhit);
      } // end loop over elms
      m_nao_nss.push_back(m_fired_ssmap[iplane].size());
  } // end loop over the layers

  /* the bias is the criteria that means a road is matched, if remain >0, this is the initial value,
   * the  no matching layer will decrease the bias
   */
  const unsigned int biasinit(ftkset.getMaxMissingPlanes()+FTKSetup::getFTKSetup().getMaxMissingSctPairs()+1);

  for (int ipatt=0;ipatt!=m_npatterns;++ipatt) { // loop over the patterns
    register unsigned int bias(biasinit); // initialize the bias
    // retrieve the status word for the current pattern
    register unsigned int word(0);

    /* loop over the layers of the pattern, stop if too many layers are missing, causing the bias to go to 0 */
    for (int iplane=0;iplane!=m_nplanes&&bias;++iplane) { // lopp over the layers
        // extract the SS of the current position
        const int &curss = m_patterns[_SSPOS(ipatt,iplane)];

        if (curss==m_WCID) { // check if it is a WC
            // update the match
            unsigned int mask(1<<(iplane+m_matchword_maskshift));
            word |= mask;
            word +=1;
        }
        else if (m_fired_ss[iplane].find(curss)!=m_fired_ss[iplane].end()) {
            // the SS exists in the map of fired patterns
            // update the match status
            unsigned int mask(1<<(iplane+m_matchword_maskshift));
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
  unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-FTKSetup::getFTKSetup().getMaxMissingSctPairs();
  m_fired_patts.clear();
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();

  for (int iplane=0; iplane<m_nplanes&&m_useWC; ++iplane) { // loop over the planes
    // create the WC SS
    FTKSS WCSS;

    FTKHit fakehit(pmap->isSCT(iplane) ? 1 : 2);
    // cout <<" WC plane=" <<iplane <<endl;
    WCSS.addHit(fakehit);
    // cout <<"WCSS.addHit(fakehit)"<<endl;

    // add the WC SS to the map of fired WC
    m_fired_ssmap[iplane][m_WCID] = WCSS;
  }

  // send real hits into the SS
  for (int iplane=0; iplane!=m_nplanes; ++iplane) { // loop over the planes
    for (vector<FTKHit>::iterator ihit = m_stlhit_sort[iplane].begin();
          ihit!=m_stlhit_sort[iplane].end(); ++ihit ) { // hit loop

        FTKHit &curhit(*ihit);

        int ss(-1);
       if (ftkset.getSectorsAsPatterns()) {
         // Using a dummy pattern bank representing just the number of setors, the IDs are the module IDs, for historical reason called sector.
         ss = curhit.getSector();
       }
       else if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
         //SS calculated assuming a global SS id
         ss = m_ssmap->getSSGlobal(curhit);
       }
       else {
         // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
         ss = m_ssmap->getSSTower(curhit,m_BankID);
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
       }
       (*curss).second.addHit(curhit);
    } // end loop over elms
    m_nao_nss.push_back(m_fired_ssmap[iplane].size());
  } // end loop over the layers

  unsigned int prematch_threshold = m_upperindex ? m_nplanes-m_lutsepplane : m_lutsepplane;
  prematch_threshold -= ftkset.getMaxMissingPlanes()+FTKSetup::getFTKSetup().getMaxMissingSctPairs();

  vector<int> prefiredpatts;
  int startplane = m_upperindex ? m_lutsepplane : 0;
  int endplane = m_upperindex ? m_nplanes : m_lutsepplane;

  for (int iplane=startplane;iplane!=endplane;++iplane) { // loop over the mapped planes
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
          unsigned int &word = m_patterns_matchstatus[patt];
          unsigned int mask(1<<(iplane+m_matchword_maskshift));
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
    unsigned int &word = m_patterns_matchstatus[iprepatt];
    bias -= biasmod-(word&m_matchword_countermask);

    /* loop over the layers of the pattern, stop if too many layers are missing, causing the bias to go to 0 */
    for (int iplane=startplane;iplane!=endplane&&bias;++iplane) { // lopp over the layers
        // extract the SS of the current position
        const int curss = m_patterns[_SSPOS(iprepatt,iplane)];

        if (curss==m_WCID) { // check if it is a WC
            // update the match
            unsigned int mask(1<<(iplane+m_matchword_maskshift));
            word |= mask;
            word +=1;
        }
        else if (m_fired_ssmap[iplane].find(curss)!=m_fired_ssmap[iplane].end()) {
            // the SS exists in the map of fired patterns
            // update the match status
            unsigned int mask(1<<(iplane+m_matchword_maskshift));
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

  m_nroads = 0;
  int iroads(0); //same as m_nroads, but keeps total count of roads (even those in excess of MAXROADS)

  vector<int>::iterator fpiter = m_fired_patts.begin();
  for (;fpiter!=m_fired_patts.end();++fpiter) {
    int ipatt = *fpiter;
    int nhit = m_patterns_matchstatus[ipatt]&m_matchword_countermask; // filter 8 bits
    if(nhit == m_nplanes) { // complete roads
      if(iroads >= MAXROADS) {
	if(iroads==MAXROADS)
	  printf("Maximum %ld roads reached, full pattern %d rejected\n",m_nroads,ipatt);
	++iroads;
	continue;
      }
      else {
	++m_nroads;
	++iroads;
      }

      if (m_do_pattern_stats) {
	// increment the effective coverage
	m_pattern_stats[ipatt] += 1;
      }

      // add the road in the list
      m_roads.push_front(FTKRoad(m_nroads-1,m_BankID+100*m_SubID,
				 ipatt,m_nplanes,
				 nhit,getBitmask(ipatt)));
    } else if ( nhit >= m_nplanes-MAX_MISSING_PLANES-MAX_MISSING_SCT_PAIRS ) {
      
      // check requirements on first or last layers
      const unsigned int bitmask = getBitmask(ipatt);
      bool first_check = !m_require_first || (bitmask & 1);
      bool last_check = !m_require_last || (bitmask & (1<<(m_nplanes-1)));
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
	    printf("Maximum %ld roads reached, partial pattern %d rejected\n",m_nroads,ipatt);
	  ++iroads;
	  continue;
	}   else {
	  ++m_nroads;
	  ++iroads;
	}

	if (m_do_pattern_stats) {
	  // increment the effective coverage
	  m_pattern_stats[ipatt] += 1;
	}
	
	// add the road in the list
	m_roads.push_front(FTKRoad(m_nroads-1,m_BankID+100*m_SubID,
				   ipatt,m_nplanes,
				   nhit,getBitmask(ipatt))); 
      } // if allowed
    } // if checking for missing SCT pairs
    else {
      /* at this point a road was ok at standard FTK_AMBank::am_in()
	 level but at further stage could be downgraded */
      if (getSaveAllRoads()) {
	if(iroads >= MAXROADS) {
	  if(iroads==MAXROADS)
	    printf("Maximum %ld roads reached, full pattern %d rejected\n",m_nroads,ipatt);
	  ++iroads;
	  continue;
	}
	else {
	  ++m_nroads;
	  ++iroads;
	}

	m_roads.push_front(FTKRoad(m_nroads-1,m_BankID+100*m_SubID,
				   ipatt,m_nplanes,
				   nhit,getBitmask(ipatt)));
      }
    }
  }

  m_nao_nroads_am = m_nroads;
  if(FTKSetup::getDBG()) {
    cout << "DBG: AM found " << m_roads.size() << " roads" << endl;
  }

  // dress the roads with the SS ids and other information
  attach_SS();
    
#ifdef VERBOSE_DEBUG
  printf("Total %d/%d roads found in bank %d with %d patts\n",m_nroads,iroads,ibank,m_npatterns);
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
  if (!m_useMinimalAMIN) memset(m_patterns_matchstatus,0,m_npatterns*sizeof(unsigned int));

  // reset cluster/ss counters (for Naoki's timing simulation)
  m_nao_nclus.clear();
  m_nao_nss.clear();
  m_nao_nroads_am = 0;
  m_nao_nroads_rw = 0;

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
int FTK_AMBank::informationMatch(int patt1, int patt2) {
  const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();

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
	    ( (bitmask1&(1<<i)) && (bitmask2&(1<<i)) )) ) {
	/* The hitbit requirement is sufficient because because if patt1
	   has 1 empty SS patt2 has at least a different empty SS,
	   for this reason an empty SS in patt2 can be suspected to be
	   a ghost road while an empty SS in patt1 doesn-t */

	// here if the SS of the two patterns in this plane differs
#ifdef VERBOSE_DEBUG
	printf("\t\t\tPIXEL %d and %d (%d) differs\n",pattern[ibank][i][patt1],
	       pattern[ibank][i][patt2],hitbit[ibank][i][patt2/8]&(1<<patt2%8));
#endif
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
	printf("\t\t\tSCT %d and %d (%d) differs\n",m_patterns[_SSPOS(patt1,i)],
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
 * Function: road_warrior()
 * Purpose:  1) sorts roads: N/N first, (N-1)/N after, (N-2)/N last (etc)
             2) applies road warrior to the roads and correspondingly updates am_out[]
 * Arguments: none
 * Returns: none
 * Comments: 
*******************************************/
void FTK_AMBank::road_warrior() {

#ifdef VERBOSE_DEBUG
  printf("*** roadwarrior\n");
#endif
  
  const FTKSetup &ftkset = FTKSetup::getFTKSetup();

  // sort the roads list
  m_roads.sort();
  m_roads.reverse();
  list<FTKRoad>::iterator itroad = m_roads.begin();
  if (m_nroads>0 && ftkset.getVerbosity()>1) {
    cout << "From list<FTKRoad>:" << endl;
    for (;itroad!=m_roads.end();++itroad) {
      printf("\t\tRoad %d has %d hits\n",(*itroad).getPatternID(),
	     (*itroad).getNHits());
    }
    itroad = m_roads.begin();
  }
  if( m_roads.empty() ) return;

  int totGhosts = 0;
#ifdef USEKDTREE
  // Build the KD Tree
  FTKRoad *head = &(*itroad);
  FTKRoadKDTree *kd_tree = new FTKRoadKDTree(head->getNPlanes(),head,0);
  int roadcounter = 1;
  for(++itroad; itroad != m_roads.end(); ++itroad, ++roadcounter) {
    head = &(*itroad);
    kd_tree->addNode(head,roadcounter);
  }

  roadcounter=0;
  for(itroad=m_roads.begin(); itroad != m_roads.end(); ++itroad,++roadcounter) { 
    // loop over the found roads
    if( itroad->getRWRejected() ) continue; // don't use rejected
    // skip full roads, only MJ roads can be deleted at this point
    if (itroad->getNHits()==itroad->getNPlanes()) continue;

    int nmatch = 0; /* Start the cycle. No match. */

    // build the mask of majority planes
    unsigned int unconstrmask(0);
    // form the mask of the empty SS, that not constrains the search
    for (int ipl=0;ipl!=itroad->getNPlanes();++ipl) {
      if (!(itroad->hasHitOnLayer(ipl))) 
	unconstrmask |= (1<<ipl);
    }

    // ask for the list of the road sharing all the SSs, except
    // the ones in the majority planes of the reference road
    list<FTKRoadKDTree*> sims;
    kd_tree->findSimilar(&(*itroad),unconstrmask,sims);

    // makes RW between similar road
    list<FTKRoadKDTree*>::iterator isim = sims.begin();
    for (;isim!=sims.end();++isim) {
      // the refroad can be removed only by another in a greater position
      // skip this otherwise
      if ((*isim)->getPosition()>roadcounter) continue;

      // compare the two roads
      /* when ref and chk are both N/N tracks the RW will fail,
	 in future this will be discarded but now is here as debug 
	 or to permit to test other algorithms */
      FTKRoad *ref = (*isim)->getNode();
      if( ref->getRWRejected() ) continue; // don't use rejected
      if (informationMatch(ref->getPatternID(),itroad->getPatternID())) {
	if(!itroad->getRWRejected()) nmatch += 1; // only increment if not already rejected
	// the patt2 is removed because if a match is found
	// it is a majority road, or both of them are MJ roads
#ifdef VERBOSE_DEBUG
	printf("\t\t%d ghosts found\n",nmatch);
#endif
	
	// flag the road as rejected by RW
	itroad->setRWRejected(1);
	itroad->setKillerRoad(ref);
      } 
    } // end loop over the other roads to look for matches
    totGhosts += nmatch;
  } // end loop over the found roads
  delete kd_tree; kd_tree = 0;
#endif
  
#ifndef USEKDTREE
  for(; itroad != m_roads.end(); ++itroad) { // loop over the found roads
    if( (*itroad).getRWRejected() ) continue; // don't use rejected

    int patt1 = (*itroad).getPatternID(); // reference pattern
    int nmatch = 0; /* Start the cycle. No match. */

    list<FTKRoad>::iterator itroad2 = itroad; // point to the next
    ++itroad2;
    for(; itroad2 != m_roads.end(); ++itroad2) {
      // end loop over the other roads to look for matches
      if( (*itroad2).getRWRejected() ) continue; // already rejected

      int patt2 = (*itroad2).getPatternID(); // pattern to compare
#ifdef VERBOSE_DEBUG
      printf("\t\tComparing %d with %d\n",patt1,patt2);
#endif
      if(informationMatch(patt1, patt2)) { // check for the match
	nmatch += 1;
	// the patt2 is removed because if a match is found
	// it is a majority road, or both of them are MJ roads
#ifdef VERBOSE_DEBUG
	printf("\t\t%d ghosts found\n",nmatch);
#endif

	// flag the road as rejected by RW
	(*itroad2).setRWRejected(1);
	(*itroad2).setKillerRoad(&(*itroad));
      } 
    } // end loop over the other roads to look for matches
    totGhosts += nmatch;
  } // end loop over the found roads
#endif

  // so far, m_nroads is not adjusted for ghosts
  m_nao_nroads_rw = m_nroads-totGhosts;
  if(FTKSetup::getDBG()) {
    cout << "DBG: after roadwarrior we have  " << m_nao_nroads_rw << " roads" << endl;
  }
  
  // loop to clean the removed roads 
  if (!ftkset.getKeepRemoved()) {
    for(itroad = m_roads.begin(); itroad != m_roads.end(); ++itroad) { 
      if( itroad->getRWRejected()>0 ) {
	// Make sure the iterators are correct
	list<FTKRoad>::iterator ittmp = itroad;
	--ittmp;
	m_roads.erase(itroad);
	itroad = ittmp;
      }
    }
    // adjust m_nroads
    m_nroads = m_nao_nroads_rw;
  }
  // make sure everything is consistent
  assert(m_roads.size()==static_cast<unsigned int>(m_nroads));
  
  if (m_nroads>0 && ftkset.getVerbosity()>1) {
    cout << "From list<FTKRoad>:" << endl;
    for (itroad = m_roads.begin();itroad!=m_roads.end();++itroad) {
      printf("\t\tRoad %d has %d hits\n",(*itroad).getPatternID(),
	     (*itroad).getNHits());
    }
  }

#ifdef VERBOSE_DEBUG
  printf("%d ghosts found, %d roads left\n", totGhosts,
	 nroads);
#endif
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
  ifstream mapfile_BadModuleMap(m_badmap_path.c_str());
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();
  m_bad_module_map = new list<FTKHit> [pmap->getNPlanes()];
  
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
    if (m_ssmap->getRegionMap()->isHitInRegion(tmpmodhit,m_BankID)) {
      cout << "BAD_MODULES_DEBUG: " << m_BankID << " \t" << tmpPlane << " \t" << tmpSector << endl;
      m_bad_module_map[tmpPlane].push_back(tmpmodhit);
    }
  }
  
  //get bad SS map
  for (int ip=0;ip!=pmap->getNPlanes();++ip) {
    set<int> *tmpset = new set<int>;
    for( list<FTKHit>::const_iterator itemod = m_bad_module_map[ip].begin();itemod != m_bad_module_map[ip].end() ; ++itemod){
      FTKHit curss(*itemod);
      if(pmap->isSCT((*itemod).getPlane())){//for SCT    
	for (int locPhi=0;locPhi<m_ssmap->getSSPhiSize(*itemod);locPhi += m_ssmap->getSSPhiWidth(*itemod)) {
	  curss[0] = locPhi;
	  tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? m_ssmap->getSSGlobal(curss) : m_ssmap->getSSTower(curss,m_BankID));
	}
      }
      else if(pmap->isPixel((*itemod).getPlane())){//for Pixel                                                                                                                              
	for (int locEta=0;locEta<m_ssmap->getSSEtaSize(*itemod);locEta += m_ssmap->getSSEtaWidth(*itemod)) {
	  curss[1] = locEta;
	  for (int locPhi=0;locPhi<m_ssmap->getSSPhiSize(*itemod);locPhi += m_ssmap->getSSPhiWidth(*itemod)) {
	    curss[0] = locPhi;
	    tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? m_ssmap->getSSGlobal(curss) : m_ssmap->getSSTower(curss,m_BankID));
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
  for (int ip=0;ip!=pmap->getNPlanes();++ip) {
    mapfile_BadModuletest <<"P  " <<ip <<" -1" <<endl;
    for(set<int>::iterator ite = m_WCSS[ip]->begin(); ite != m_WCSS[ip]->end(); ++ite ){
      mapfile_BadModuletest << (*ite) <<endl;
    }
  }
  mapfile_BadModuletest.close();
#endif
  
  //clear
  for(int ip=0 ;ip != pmap->getNPlanes();++ip){
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
  ifstream mapfile_BadModuleMap(m_badmap_path2.c_str());
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();
  m_bad_module_map = new list<FTKHit> [pmap->getNPlanes()];
  
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
    if (m_ssmap->getRegionMap()->isHitInRegion(tmpmodhit,m_BankID)) {
      m_bad_module_map[tmpPlane].push_back(tmpmodhit);
    }
  }
  
  //get bad SS map
  for (int ip=0;ip!=pmap->getNPlanes();++ip) {
    set<int> *tmpset = new set<int>;
    for( list<FTKHit>::const_iterator itemod = m_bad_module_map[ip].begin();itemod != m_bad_module_map[ip].end() ; ++itemod){
      FTKHit curss(*itemod);
      if(pmap->isSCT((*itemod).getPlane())){//for SCT    
	for (int locPhi=0;locPhi<m_ssmap->getSSPhiSize(*itemod);locPhi += m_ssmap->getSSPhiWidth(*itemod)) {
	  curss[0] = locPhi;
	  tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? m_ssmap->getSSGlobal(curss) : m_ssmap->getSSTower(curss,m_BankID));
	}
      }
      else if(pmap->isPixel((*itemod).getPlane())){//for Pixel                                                                                                                              
	for (int locEta=0;locEta<m_ssmap->getSSEtaSize(*itemod);locEta += m_ssmap->getSSEtaWidth(*itemod)) {
	  curss[1] = locEta;
	  for (int locPhi=0;locPhi<m_ssmap->getSSPhiSize(*itemod);locPhi += m_ssmap->getSSPhiWidth(*itemod)) {
	    curss[0] = locPhi;
	    tmpset->insert(FTKSetup::getFTKSetup().getHWModeSS()==0 ? m_ssmap->getSSGlobal(curss) : m_ssmap->getSSTower(curss,m_BankID));
	  }
	}
      }
    }

    // check if there are SS to be vetoed, if not the memory is cleaned
    if (tmpset->empty()) delete tmpset;
    else m_WCSS2[ip] = tmpset;
  }
  
  //clear
  for(int ip=0 ;ip != pmap->getNPlanes();++ip){
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
         ss = m_amobj->m_ssmap->getSSGlobal(*(h_elm->car));
       }
       else {
         // SS calculated assuming a tower SS id, HW friendly, with a small number of bits
         ss = m_amobj->m_ssmap->getSSTower(*(h_elm->car),m_amobj->m_BankID);
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
