/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/PatternBank.h"
#include "TrigFTKSim/FTKSetup.h"
#include <TSystem.h>

#include <boost/unordered_map.hpp>

#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <ext/new_allocator.h>
using namespace std;

#define USEBOOSTUMAP
//#define USEMEDIUMPATTERN
//#define USENODEFALLOC

int verbosity = 0;     // unused for now
int delete_files = 0;  // delete input patterns once they are read into memory?
int vmem_limit = -1;   // stop loading patterns if virtual memory limit is exceeded

unsigned int MinCoverage(0); // set the minimum coverage for the patterns

/** minimal class to store patterns */
class MiniPattern {
protected:
  static unsigned m_nplanes;
  static bool m_insert_sort;

  int *m_SSID;
  int m_SectorID;
#ifndef USEMEDIUMPATTERN
  unsigned int m_Coverage;
#endif
public:

  MiniPattern();
  MiniPattern(const MiniPattern&);
#ifndef USEMEDIUMPATTERN
  MiniPattern(const MiniPattern&, unsigned);
#endif
  virtual ~MiniPattern();

  const int* getSSID() const { return m_SSID; }

  const int& getSectorID() const { return m_SectorID; }

#ifndef USEMEDIUMPATTERN
  void setCoverage(unsigned val) { m_Coverage = val; }
  void addCoverage(unsigned val) { m_Coverage+= val; }
  const unsigned& getCoverage() const { return m_Coverage; }
#endif

  static void setNPlanes(int np) { m_nplanes = np; }
  static unsigned getNPlanes() { return m_nplanes; }

  static void setInsertSorting(bool v) { m_insert_sort = v; }
  static bool getInsertSorting() { return m_insert_sort; }

  MiniPattern& operator=(const MiniPattern&);
  bool operator<(const MiniPattern&) const;


  bool operator==(const MiniPattern&) const;

  friend istream& operator>>(istream&,MiniPattern&);
#ifndef USEMEDIUMPATTERN
  friend ostream& operator<<(ostream&,MiniPattern&);
#endif
};


unsigned MiniPattern::m_nplanes = 0;
bool MiniPattern::m_insert_sort = true;

MiniPattern::MiniPattern() :
  m_SectorID(-1)
#ifndef USEMEDIUMPATTERN
  ,m_Coverage(0)
#endif
{
  m_SSID = new int[m_nplanes];
}

MiniPattern::MiniPattern(const MiniPattern &cpy) :
  m_SectorID(cpy.m_SectorID)
#ifndef USEMEDIUMPATTERN
  ,m_Coverage(cpy.m_Coverage)
#endif
{
  m_SSID = new int[m_nplanes];
  for (unsigned ip=0;ip!=m_nplanes;++ip)
    m_SSID[ip] = cpy.m_SSID[ip];
}

#ifndef USEMEDIUMPATTERN
MiniPattern::MiniPattern(const MiniPattern &cpy, unsigned newCoverage)
  : m_SectorID(cpy.m_SectorID), m_Coverage(newCoverage)
{
  m_SSID = new int[m_nplanes];
  for (unsigned ip=0;ip!=m_nplanes;++ip)
    m_SSID[ip] = cpy.m_SSID[ip];
}
#endif

MiniPattern::~MiniPattern()
{
  delete [] m_SSID;
}


MiniPattern& MiniPattern::operator=(const MiniPattern &patt)
{
  m_SectorID = patt.m_SectorID;
#ifndef USEMEDIUMPATTERN
  m_Coverage = patt.m_Coverage;
#endif
  for (unsigned ip=0;ip!=m_nplanes;++ip)
    m_SSID[ip] = patt.m_SSID[ip];
  return *this;
}

bool MiniPattern::operator==(const MiniPattern &right) const
{
  for (unsigned ip=0;ip!=m_nplanes;++ip) {
    if (m_SSID[ip]!=right.m_SSID[ip]) return false;
  }
  return true;
}

istream& operator>>(istream &sin, MiniPattern &pattern)
{
  for (unsigned il=0;il<pattern.m_nplanes;++il) {
    sin >> pattern.m_SSID[il];
  }
  sin >> pattern.m_SectorID;
#ifndef USEMEDIUMPATTERN
  sin >> pattern.m_Coverage;
#endif
  return sin;
}


bool MiniPattern::operator<(const MiniPattern &right) const
{
  // before the pattern with the best coverage
#ifndef USEMEDIUMPATTERN
  if (m_insert_sort) {
    for (unsigned i=0;i!=m_nplanes;++i) {
      if (m_SSID[i]<right.m_SSID[i]) return true;
      else if (m_SSID[i]>right.m_SSID[i]) return false;      
    }
    return false; // is here if all the SS are the same
  }
  else {
    return right.m_Coverage<m_Coverage;
  }
#else
  for (unsigned i=0;i!=m_nplanes;++i) {
    if (m_SSID[i]<right.m_SSID[i]) return true;
    else if (m_SSID[i]>right.m_SSID[i]) return false;      
  }
  return false; // is here if all the SS are the same
#endif
}

#ifndef USEMEDIUMPATTERN
ostream& operator<<(ostream &sout, MiniPattern &pattern)
{
  for (unsigned il=0;il<pattern.m_nplanes;++il) {
    sout << ' ' <<  pattern.m_SSID[il];
  }
  sout << ' ' << pattern.m_SectorID;
  sout << ' ' << pattern.m_Coverage;
  return sout;
}
#endif

struct ihash
    : std::unary_function<MiniPattern, std::size_t>
{
    std::size_t operator()(const MiniPattern& patt) const
    {
      size_t seed = 0;
      boost::hash_combine(seed, patt.getSectorID());
      for (unsigned ip=0;ip!=MiniPattern::getNPlanes();++ip)
	boost::hash_combine(seed, patt.getSSID()[ip]);
      return seed;
    }
};

class MediumPattern {
protected:
  MiniPattern m_MiniPattern;
  unsigned m_Coverage;
public:
  MediumPattern(const MiniPattern &mini, unsigned coverage) :
    m_MiniPattern(mini), m_Coverage(coverage)
  {;}

  void setCoverage(unsigned val) { m_Coverage = val; }
  void addCoverage(unsigned val) { m_Coverage+= val; }
  const unsigned& getCoverage() const { return m_Coverage; }

  bool operator<(const MediumPattern &) const;
  friend ostream& operator<<(ostream&,MediumPattern&);
};

bool MediumPattern::operator<(const MediumPattern &right) const
{
  return right.m_Coverage<m_Coverage;
}

ostream& operator<<(ostream &sout, MediumPattern &pattern)
{
  for (unsigned il=0;il<MiniPattern::getNPlanes();++il) {
    sout << ' ' <<  pattern.m_MiniPattern.getSSID()[il];
  }
  sout << ' ' << pattern.m_MiniPattern.getSectorID();
  sout << ' ' << pattern.m_Coverage;
  return sout;
}

// because the object in the map are constant the coverage should be separated
#ifdef USEBOOSTUMAP
#ifdef USENODEFALLOC
typedef boost::unordered_map<MiniPattern,unsigned,ihash,
			     equal_to<MiniPattern>,
			     __gnu_cxx::new_allocator<pair<const MiniPattern,unsigned> > > PatternMap;
#else
typedef boost::unordered_map<MiniPattern,unsigned,ihash> PatternMap;
#endif
#else
typedef map<MiniPattern,unsigned> PatternMap;
#endif

#ifdef USEMEDIUMPATTERN
typedef list<MediumPattern> PatternList;
#else
typedef list<MiniPattern> PatternList;
#endif

int NINPUTS(0);
std::vector<std::string> inputfiles; // pattern files
std::vector<int> extraweights;        // extra event weight
std::string outfile;

int read_commands();

/** main function */
int main(int argc, char *argv[]) {
  FTKSetup &ftksetup = FTKSetup::getFTKSetup();

  // preliminary parsing of the input file
  ifstream optionfile;
  streambuf *oldrdbuf = cin.rdbuf();
  if (argc>1) { 
    // input option in an external file
    optionfile.open(argv[1]);
    if (!optionfile) {
      std::cerr << "*** Error reading input file: " << argv[1] << std::endl;
      return -1;
    }
    // input option parsed from stdin
    cin.rdbuf(optionfile.rdbuf());
  }
  // call the parser
  if (read_commands()<0) {
    std::cerr << "*** Error parsing the options" << std::endl;
  }
  cin.rdbuf(oldrdbuf);

#if 0
  PatternBank *outbank = new PatternBank();
  ProcInfo_t mem;
  for(int i=0;i<NINPUTS;i++) {
    std::cout << "Opening " << inputfiles[i];
    if(extraweights[i]) std::cout << " with extra pattern weight of " << extraweights[i];
    std::cout << std::endl;
    outbank->setFile(inputfiles[i].c_str());
    outbank->AppendLookup(extraweights[i]);
    // on the grid, we try to conserve space by deleting already-read pattern files
    if(delete_files) {
      std::cout << "Warning: deleting file " << inputfiles[i] << std::endl;
      gSystem->Unlink(inputfiles[i].c_str());
    }
    // exit the loop if virtual memory usage is too high
    if(vmem_limit>0 && gSystem->GetProcInfo(&mem)==0) {
      std::cout << "INFO: virtual memory size = " << mem.fMemVirtual/1000 << " MB" << std::endl;
      if(mem.fMemVirtual > vmem_limit) {
	std::cout << "WARNING: virtual memory size (" << mem.fMemVirtual
		  << " KB) exceeds the limit of " << vmem_limit << " KB" << std::endl;
	std::cout << "Breaking out of the loop on file " << i << "/" << NINPUTS << std::endl;
	break;
      }
    }
  }

  outbank->SaveAs(outfile.c_str());
#else

  PatternMap *pattmap(0x0);

  unsigned nplanes;
  ProcInfo_t mem;

  unsigned long npatterns_merged(0);

  MiniPattern::setInsertSorting(true);
  for(int i=0;i<NINPUTS;i++) {
    std::cout << "Opening " << inputfiles[i];
    if(extraweights[i]) std::cout << " with extra pattern weight of " << extraweights[i];
    std::cout << std::endl;


    ftk_dcap::istream pattfile;
    if (!ftk_dcap::open_for_read(inputfiles[i],pattfile)) {
      cerr << "*** Error loading the pattern bank: " << inputfiles[i] << endl;
      return -1;
    }

    int npatterns;
    pattfile >> npatterns >> nplanes;

    if (!pattmap) {
      MiniPattern::setNPlanes(nplanes);
      pattmap = new PatternMap;
    }

    MiniPattern curpatt;
    
    int ipatt_step = (npatterns+4)/5;
    const int &promote = extraweights[i];
    for (long ipatt=0;ipatt!=npatterns;++ipatt) { // pattern loop
      if (ipatt%ipatt_step==0) {
	ftksetup.usageStat();
	cout << ipatt << '/' << npatterns << endl;
      }
      // read the current pattern
      int pattID;
#ifdef USEMEDIUMPATTERN
      unsigned coverage;
      pattfile >> pattID >> curpatt >> coverage;
      if (promote) coverage += promote;
#define CURCOVERAGE coverage
#else
      pattfile >> pattID >> curpatt;
      if (promote) curpatt.addCoverage(promote);
#define CURCOVERAGE curpatt.getCoverage()
#endif
      

      // try to add the pattern
#ifdef USEBOOSTUMAP     
      pair<PatternMap::iterator,bool> res = pattmap->insert(pair<MiniPattern,unsigned> (curpatt,CURCOVERAGE));
      if (! res.second) {
	// The pattern was no added, because one equal exists
	// just increment the coverage
	(*(res.first)).second += CURCOVERAGE;
      }
      else {
	// if added update the counter
	npatterns_merged += 1;
      }
#else
      pair<PatternMap::iterator,bool> res = pattmap->insert(pair<MiniPattern,unsigned> (curpatt,CURCOVERAGE));
      if (! res.second) {
	// The pattern was no added, because one equal exists
	// just increment the coverage
	(*(res.first)).second += CURCOVERAGE;
      }
      else {
	// if added update the counter
	npatterns_merged += 1;
      }
#endif
    } // end pattern loop

    ftksetup.usageStat("End file merging");
    cout << npatterns << '/' << npatterns << " (Tot: " << npatterns_merged << ")" << endl;


    // on the grid, we try to conserve space by deleting already-read pattern files
    if(delete_files) {
      std::cout << "Warning: deleting file " << inputfiles[i] << std::endl;
      gSystem->Unlink(inputfiles[i].c_str());
    }
    // exit the loop if virtual memory usage is too high
    if(vmem_limit>0 && gSystem->GetProcInfo(&mem)==0) {
      std::cout << "INFO: virtual memory size = " << mem.fMemVirtual/1000 << " MB" << std::endl;
      if(mem.fMemVirtual > vmem_limit) {
	std::cout << "WARNING: virtual memory size (" << mem.fMemVirtual
		  << " KB) exceeds the limit of " << vmem_limit << " KB" << std::endl;
	std::cout << "Breaking out of the loop on file " << i << "/" << NINPUTS << std::endl;
	break;
      }
    }
  }
  cout << "Total number of merged patterns: " << npatterns_merged << endl;

  unsigned long ipatt_step = (npatterns_merged+9)/10;
  unsigned long patt_counter(0);
  cout << "Building the ordered list of the merged patterns" << endl;
  MiniPattern::setInsertSorting(false);

  PatternList final_pattbank;
  // print the bank, ordering the pattern by coverage
  PatternMap::iterator upatt_i = pattmap->begin();
  PatternMap::iterator upatt_e = pattmap->end();
  while (upatt_i!=upatt_e) {
    if (patt_counter%ipatt_step==0) {
      ftksetup.usageStat();
      cout << "Mergin " << patt_counter << " / " << npatterns_merged << endl;
    }
    // build the pattern adding the correct coverage
#ifdef USEMEDIUMPATTERN
    MediumPattern fullpatt((*upatt_i).first,(*upatt_i).second);
#else
    MiniPattern fullpatt((*upatt_i).first,(*upatt_i).second);
#endif
    final_pattbank.push_back(fullpatt);

#ifdef USEBOOSTUMAP
    upatt_i = pattmap->erase(upatt_i); // move to next removing the current pattern
#else
    PatternMap::iterator upatt_next = upatt_i;
    ++upatt_next;
    pattmap->erase(upatt_i);
    upatt_i = upatt_next;
#endif
    ++patt_counter;

#ifdef USEMEDIUMPATTERN_USELESS
#ifdef USEBOOSTUMAP
    // try to do magic if the medium pattern class is used to prevent the leak
    if (patt_counter%ipatt_step==0) {
      PatternMap *newtmpmap = new PatternMap();
      newtmpmap->swap(*pattmap);
      pattmap->clear();
      delete pattmap;
      pattmap = newtmpmap;
      upatt_i = pattmap->begin();
      upatt_e = pattmap->end();
    }
#endif
#endif
  }
  pattmap->clear();
  delete pattmap;

  // sort the bank by descending coverage
  cout << "Sorting the list" << endl;
  final_pattbank.sort();

  if (MinCoverage>1) {
    // read the list from the bottom and remove all the patterns
    // with coverage less than MinCoverage
    cout << "Removing all the patterns with Coverage<" << MinCoverage << endl;

    PatternList::reverse_iterator fpatt_iR = final_pattbank.rbegin();
    PatternList::reverse_iterator fpatt_eR = final_pattbank.rend();
    PatternList::iterator fpatt_cR = final_pattbank.end();
    
    unsigned int npatt_tocut(0);
    for (;fpatt_iR!=fpatt_eR;++fpatt_iR) { // reverse loop over the pattern bank
      if ((*fpatt_iR).getCoverage()==MinCoverage) {
	// set the end iterator for the portion that has to be removed
	break;
      }

      npatt_tocut += 1;
      --fpatt_cR;
    } // end reverse loop over the patterns

    cout << "Removing " << npatt_tocut << " (of " << final_pattbank.size() << ") patterns because insufficient coverage" << endl;
    final_pattbank.erase(fpatt_cR,final_pattbank.end());
  }

  ftk_dcap::ostream fout;
  bool success = ftk_dcap::open_for_write(outfile.c_str(),fout);
  if(!success) {
    cout << "Failed to open the file " << outfile << endl;
    return -1;
  }

  fout << final_pattbank.size() << " " << nplanes << endl;

  cout << "Writing the pattern bank" << endl;
  PatternList::iterator fpatt_i = final_pattbank.begin();
  PatternList::iterator fpatt_e = final_pattbank.end();
  
  int mergeID(0);
  for (;fpatt_i!=fpatt_e;++fpatt_i,++mergeID) { // pattern write loop
    if (mergeID%ipatt_step==0) {
      ftksetup.usageStat();
      cout << "Writing " << mergeID << " / " << npatterns_merged << endl;
    }
    fout << mergeID << *fpatt_i << endl;
  } // end pattern write loop
#endif

  return 0;
}

/** this function parse the input commands and prepare a simple structure
    used by the main function */
int read_commands()
{
  const char prompt[] = "PARSE> ";

  string line;
  while(getline(cin,line)!=0) {    
    if (line[0]=='#' || line.size()==0) {
      continue;
    }

    cout << prompt << line << endl;

    // extraxt the pair (key,value)
    istringstream sline(line);
    string key;
    sline >> key;
    if (key=="VERBOSITY") {
      sline >> verbosity;
    }
    else if (key=="DELETE_FILES_AFTER_USE") {
      sline >> delete_files;
    }
    else if (key=="VMEM_LIMIT") {
      sline >> vmem_limit;
    }
    else if (key=="NINPUTS") {
      sline >> NINPUTS;
      // FORMAT: /path/to/patterns.patt [EXTRA_WEIGHT]
      for(int i=0; i<NINPUTS; i++) {
	std::string bankline;
	getline(cin,bankline);
	istringstream sbankline(bankline);
	std::string bankpath; int extraweight(0);
	sbankline >> bankpath;
	if(sbankline) sbankline >> extraweight;
	inputfiles.push_back(bankpath);
	extraweights.push_back(extraweight);
      }
    }
    else if (key=="MINCOVERAGE") {
      sline >> MinCoverage;
    }
    else if (key=="OUT_FILE") {
      sline >> outfile;
    }
    else {
      cout << prompt << "\"" << key << "\" is not a valid key" << endl;
      continue;
    }
  }

  return 0;
}
