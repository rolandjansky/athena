/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"

#include <boost/lexical_cast.hpp>

#include <TTree.h>

#include <string>
#include <iostream>
#include <cmath>
#include <cassert>
#include <map>
#include <list>
using namespace ftk;
using namespace std;
using namespace boost;

FTKTSPBank::FTKTSPBank(int bankid, int subid) : 
  FTK_AMBank(bankid,subid),
  m_TSPProcessor(0x0),
  m_SimulateTSP(0), m_npatternsTSP(0), m_TSPMinCoverage(0),
  m_setAMSize(0), m_ssmap_tsp(0x0), m_splitted_ssmap(0x0),
  m_cachepath("")
{}


FTKTSPBank::~FTKTSPBank()
{
  if (m_SimulateTSP>1) {
    delete m_TSPProcessor;
  }

  if (m_splitted_ssmap) delete [] m_splitted_ssmap;
}


/** This function overload the default FTK_AMBank::end() adding
    informations that are more strictly related with the AM banks
    built starting from TSP */
void FTKTSPBank::end()
{
  if (m_do_pattern_stats) {
    // write the A bank statistic
    ftk_dcap::ostream bankstat_file;
    ftk_dcap::open_for_write("patternbank.dat",bankstat_file);

    bankstat_file << "PattID Freq" << endl;
    for (int ipatt=0;ipatt!=m_npatterns;++ipatt) {
      bankstat_file << m_PatternDBID[ipatt] << ' ' << m_pattern_stats[ipatt] << endl;
    }
  }
}


int FTKTSPBank::readROOTBank(const char* fname, int maxpatt) {
  FTKSetup &ftksetup = FTKSetup::getFTKSetup();

  cout << "Reading TSP ROOT bank: " << fname << endl;
  m_file = TFile::Open(fname);
  if (!m_file) {
    FTKSetup::PrintMessage(ftk::sevr,"Error opening the ROOT bank");
    return -1;
  }

  /* this is the pointer to the TTree containing the AM bank, 
     if a real TSP/DC simulation is performed this points to Bank1,
     if the TSP bank is used as test could be Bank0 */
  TTree *AMBank(0x0);

  // this maps will contain the map of used AM pattern and the relative TSP patterns
  map< int, AMSelection > AMPatternMap;
  bool hasBuiltAM(false); // it becomes true if an AM is built: best tsp or minimum coverage
  ftksetup.usageStat("Create AM Bank using the ROOT file");

  /* The AM bank is created in 2 different ways:
     1. if the m_SimulateTSP is greater than 0 the AM bank is created looking at the grouping algorithm
     2. if the flag s set to 0 the TSP bank is read sequencially to form he AM
  */
  if (m_SimulateTSP>0 || m_TSPMinCoverage>0) {
    FTKSetup::PrintMessage(ftk::info,"Generating an AM bank");
    hasBuiltAM = true;

    /** The AM bank has other two variants:
	a. if the request for a minimum coverage is not used (m_TSPMinCoverage==0)
	the AM bank is created building grouping the fatherID field of a given 
	number of TSP patterns, the number of TSP patterns is decided by the
	user
	b. if a minimum coverage for the TSP patterns is required the AM bank
	is obtained grouping the fatherID field for all TSP patterns, then
	this number is limited according the user request.
	In both methods the AM patterns are ordered by the coverage.
    */
    if (m_TSPMinCoverage==0) { // no minimum coverage block
      FTKSetup::PrintMessage(ftk::info,"Bank from Best TSP patterns");
      // Get the pointer to the TSP bank TTree and the TTree with the relations
      AMBank = (TTree*) m_file->Get("Bank1");
      TTree *TSPBank = (TTree*) m_file->Get("Bank0");
      TTree *TSPRelation = (TTree*) m_file->Get("TSPRelation0");

      // get the maxium number of TSP patterns
      m_npatternsTSP = TSPBank->GetEntries();

      // prepare to read the patterns
      FTKPattern *tsppatt = new FTKPattern();
      TSPBank->SetBranchAddress("Pattern",&tsppatt);
      // prepare to read the AM parent IDs
      Int_t AMID;
      TSPRelation->SetBranchAddress("AMID",&AMID);

      // get the first element to complete the information on the bank
      TSPBank->GetEntry(0);
      m_nplanes = tsppatt->getNPlanes();


      // disable alla the useless branches
      TSPRelation->SetBranchAddress("HalfPlaneMask",0);

      if (!m_setAMSize) {
	// set number of TSP patterns to read following the maxpatt value
	if (m_npatternsTSP<maxpatt || maxpatt<0) {
	  cout << "Maximum number of TSP patterns to be read: " << m_npatternsTSP << " (asked " << maxpatt << ')' << endl;
	  maxpatt = m_npatternsTSP;
	}
	cout << "Found # " << m_npatternsTSP << " TSP patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

	// m_patternsTSP is set to the maximum number or patterns
	if (maxpatt<m_npatternsTSP) m_npatternsTSP = maxpatt;

	// set m_npatterns to an impossible value the AM generation loop to stop
	m_npatterns = -1;
      }
      else {
	// set number of AM patterns to load following the maxpatt value
	m_npatterns = AMBank->GetEntries(); // set the number of AM patterns

	// check if there are enough patterns
	if (m_npatterns<maxpatt || maxpatt<0) {
	  cout << "Maximum number of AM patterns to be read: " << m_npatterns << " (asked " << maxpatt << ')' << endl;
	  maxpatt = m_npatterns;
	}

	cout << "Found # " << m_npatterns << " AM patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns, mode " << m_setAMSize << ')' << endl;

	// set the correct limit to the number of AM pattern to create
	if (maxpatt<m_npatterns) m_npatterns = maxpatt;
      }

      int nAMPatterns(0); // number of AM patterns loaded
      for (int ipatt=0;ipatt!=m_npatternsTSP;++ipatt) { // loop over TSP patterns
	// get the ID of this TSP pattern
	TSPRelation->GetEntry(ipatt);
	TSPBank->GetEntry(ipatt);

	map<int,AMSelection>::iterator iAM = AMPatternMap.find(AMID);
	if (iAM==AMPatternMap.end()) { 
	  /* if m_setAMSize==2 the AM is stoppped when the number of patterns
	     is going to be exceeded */
	  if (m_setAMSize==2 && nAMPatterns==m_npatterns) {
	    // set this as limit in the TSP pattern bank
	    m_npatternsTSP = ipatt;
	    break;
	  }

	  // new AM pattern
	  AMPatternMap[AMID] = AMSelection(AMID,ipatt,tsppatt->getCoverage());
	  nAMPatterns+=1;
	  
	  /* if m_setAMSize==1 the AM bank creation stops as soon as 
	     the required size is obtained */
	  if (m_setAMSize==1 && nAMPatterns==m_npatterns) {
	    // set this as last TSP pattern to be read
	    m_npatternsTSP = ipatt+1;
	    break;
	  }
	}
	else {
	  // already used by another TSP pattern, just add the current TSP pattern to the relation
	  (*iAM).second.addTSP(ipatt,tsppatt->getCoverage());
	}
      } // end loop over TSP patterns

      // set the number of AM patterns found
      m_npatterns = AMPatternMap.size();
      if (m_setAMSize>0) {
	assert(m_npatterns==nAMPatterns);
	cout << "Number of TSP patterns: " << m_npatternsTSP << endl;

      }

      FTKSetup::PrintMessageFmt(ftk::info,"AM bank created with %d AM patterns",m_npatterns);

    } // end of no minimum coverage block
    else { // minimum coverage block
      FTKSetup::PrintMessage(ftk::info,"Generating an AM bank from TSP with minimum coverage");      // check the TSP bank size
      FTKSetup::PrintMessage(ftk::sevr,"Not supported for ROOT banks");

      m_nplanes = 0;
      m_npatterns = 0; // max number AM patterns

      m_npatterns = 0; // real number of AM patterns
      FTKSetup::PrintMessageFmt(ftk::info,"AM bank created with %d AM patterns, asked %d",m_npatterns,maxpatt);
    } // end minimum coverage block
	  
  } // and block for AM building
  else { 
    FTKSetup::PrintMessage(ftk::info,"Using TSP format to read an AM bank");

    /* Use the ROOT format prepared for the AMTSP/DC simulation but
       uses only the TSP. If here the simulation level and the minimum
       coverage are 0: ! (m_SimulateTSP>0 || m_TSPMinCoverage>0) */

    // The AM and TSP banks are equal
    AMBank = (TTree*) m_file->Get("Bank0");

    FTKPattern *tsppatt = new FTKPattern();
    AMBank->SetBranchAddress("Pattern",&tsppatt);
    AMBank->GetEntry(0);
    m_nplanes = tsppatt->getNPlanes();

    m_npatterns = AMBank->GetEntries();

    if (maxpatt < 0 || m_npatterns < maxpatt) maxpatt = m_npatterns;

    cout << "Found # " << m_npatterns << " in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;
	      
    if (maxpatt<m_npatterns) m_npatterns = maxpatt;
  }

  // order the AM bank by coverage
  list<AMSelection> AMPatternList;
  map< int, AMSelection >::const_iterator iAMmap = AMPatternMap.begin();
  for (;iAMmap!=AMPatternMap.end();++iAMmap) {
    AMPatternList.push_back((*iAMmap).second);
  }
  AMPatternMap.clear(); // remove the map used to store the AM patterns
  AMPatternList.sort(); // use the sorting metho of the list

  ftksetup.usageStat("Reading the AM bank");
  

  // initialize the AM data structures
  readBankInit();

  // define the step
  ipatt_step = (m_npatterns+9)/10;
  if (!ipatt_step) ipatt_step = 1;

  cout << "Reading : [" << flush;
  FTKPattern *ampatt = new FTKPattern();
  AMBank->SetBranchAddress("Pattern",&ampatt);
  list<AMSelection>::iterator iAMlist = AMPatternList.begin();
  for (int ipatt = 0;ipatt!=m_npatterns;++ipatt){
    if(ipatt % ipatt_step == 0) cout << ipatt/ipatt_step << flush;

    // Read the AM pattern
    int curAMID(ipatt);
    if (hasBuiltAM) {
      curAMID = (*iAMlist).getAMID();
      ++iAMlist; // move the pointer to the next
    }

    AMBank->GetEntry(curAMID);
    m_PatternDBID.push_back(curAMID);

    for(int iplane = 0; iplane != m_nplanes; ++iplane) { // loop on the planes
      // the m_nplnes+1 element is the number of the sector
      // the other are the SS index
      m_patterns[iplane][ipatt] = ampatt->getSSID(iplane);
    }
    m_patterns[m_nplanes][ipatt] = ampatt->getSectorID();
  }
  cout << "]" << endl;


  if (m_SimulateTSP>1) {
    ftksetup.usageStat("Read TSP patterns");

    FTKSetup::PrintMessage(ftk::info,"TSPProcessor creation");
    // prepare don't care data
    m_splitted_ssmap = new map<int,FTKSS>[m_nplanes];
    // nested TSP processor
    m_TSPProcessor = new TSPLevel(m_file,2,
				  this,AMPatternList,
				  m_nplanes-1,m_ssmap,m_ssmap_tsp,
				  m_splitted_ssmap,m_SimulateTSP>2);
    if (!m_npatternsTSP) // if the number of patterns is missing it can be get from the TSP processor
      m_npatternsTSP = m_TSPProcessor->getNPatterns();

  }

  // can close the ROOT file, this 
  m_file->Close();

  readBankInitEnd();

  if (m_makecache) {
    // save  the current AM bank content for future use
    dumpCache();
  }

  // return the number of the patterns in the bank
  return m_npatterns;
}

/** This method reads the AM bank saved using the dumpCache() method. */
int FTKTSPBank::readROOTBankCache(const char *fname)
{
  FTKSetup &ftksetup = FTKSetup::getFTKSetup();

  // open the cache root file
  TFile *rootcache = TFile::Open(fname);

  // Create the TTree with 1 branch: the list of the FTK patterns (AM level)
  TTree *amtree = dynamic_cast<TTree*>(rootcache->Get("Bank"));
  FTKPattern *curpatt = 0x0;
  amtree->SetBranchAddress("Pattern",&curpatt);

  // get the number of the patterns
  m_npatterns = amtree->GetEntries();

  // uset the first pattern to setup global variables
  amtree->GetEntry(0);
  m_nplanes = curpatt->getNPlanes();

  // initialize the AM data structures
  readBankInit();

  if (m_SimulateTSP>1) {
    FTKSetup::PrintMessage(ftk::info,"TSPProcessor preparation for cache read");
    // prepare don't care data
    m_splitted_ssmap = new map<int,FTKSS>[m_nplanes];
    // nested TSP processor
    m_TSPProcessor = new TSPLevel(2,m_npatterns,m_nplanes-1,m_ssmap,m_ssmap_tsp, m_splitted_ssmap, m_SimulateTSP>2);
  }
  
  ftksetup.usageStat("Read AM cached bank");
  // define the step
  ipatt_step = (m_npatterns+9)/10;
  if (!ipatt_step) ipatt_step = 1;
  
  // reset the number of the TSP patterns
  m_npatternsTSP = 0;
  
  cout << "Reading : [" << flush;
  for (int ipatt=0;ipatt!=m_npatterns;++ipatt) { // pattern loop
    if(ipatt % ipatt_step == 0) cout << ipatt/ipatt_step << flush;    

    m_PatternDBID.push_back(curpatt->getPatternDBID());
    // set the current pattern
    amtree->GetEntry(ipatt);
    for (int iplane=0;iplane!=m_nplanes;++iplane) {
      m_patterns[iplane][ipatt] = curpatt->getSSID(iplane);
    }
    m_patterns[m_nplanes][ipatt] = curpatt->getSectorID();

    if (m_SimulateTSP>1) {
      int nTSPpatterns = m_TSPProcessor->getPatternInfo(ipatt, curpatt,m_SimulateTSP>2);
      m_npatternsTSP += nTSPpatterns;
    }
    
  } // end pattern loop
  cout << ']' << endl;

  ftksetup.usageStat("End AM cache read");


  FTKSetup::PrintMessageFmt(ftk::info,"Loaded #%d TSP Patterns",m_npatternsTSP);
  readBankInitEnd();
  
  // set the flag to use a cached bank
  m_CachedBank = true;
  return m_npatterns;
}


/** This is a new version of am_in() method. The basic functionality
    is made in the FTK_AMBank::am_in() method. The following code emulates
    the prepare the SS within the fired roads to be used in the DC filter
    and in for the TSP simulation */
void FTKTSPBank::am_in()
{
  /* the basic work of the AM is simulated in the base implementation
     of the AM simulation */
  FTK_AMBank::am_in();

  // if the simulation is at level 0 or 1 only default AM simulation is performed
  if (m_SimulateTSP<2) return;

  // clear the splitted SS maps
  for (int ip=0;ip!=m_nplanes;++ip) { // plane loop
    m_splitted_ssmap[ip].clear();
  } // end plane loop

  /* The TSP simulation works only on the list of fired roads */
  vector<int>::iterator fpiter = m_fired_patts.begin();
  for (;fpiter!=m_fired_patts.end();++fpiter) { // road loop
    int ipatt = (*fpiter);

    // look in more detail the fired pattern
    filterPattern(ipatt);
  } // end road loop
  
#ifdef NEWSSLIST
  // reassign the SS, the unsplitted SS are sub changed using the splitted version
  for (int ip=0;ip!=m_nplanes;++ip) { // plane loop
    map<int,FTKSS>::iterator iss = m_splitted_ssmap[ip].begin();
    for (;iss!=m_splitted_ssmap[ip].end();++iss) { // ss loop
      // substitute the unsplitted SS with the splitted version
      m_fired_ssmap[ip][(*iss).first] = (*iss).second;
    } // end ss loop
  } // end plane loop
#endif
}

/** This function for this kind of bank scan the layers
    to verify the hits position in the SS, discarding the hits
    in the half layer vetoed by DC mask */
void FTKTSPBank::filterPattern(int ipatt)
{
  unsigned int &matchstatus = m_patterns_matchstatus[ipatt];

  for (int ip=0;ip!=m_nplanes;++ip) { // layers loop
    // check the bitmask and skip if the layer is empty
    if ((matchstatus&(1<<(ip+m_matchword_maskshift)))==0) continue;

    // search the super-strip for this plane
    int ss = m_patterns[ip][ipatt];

    // if the SS id is not valid, TSP or DC don't apply
    if( ss < 0 || ss == m_WCID || ss == m_VetoID)  continue;

    // find the splitted version for this super-strip
    map<int,FTKSS>::iterator isplittedSS = m_splitted_ssmap[ip].find(ss);
    if (isplittedSS==m_splitted_ssmap[ip].end()) {
      // the splitted version doesn't exist and should be created

      /* The super-strips content is divided according the internal
	 sub-division. The number of sub-divisions depend on the
	 the number of bits needed to describe this layer */
      int nssdivs(m_TSPProcessor->getTSPMap().getNPositions(ip));
      vector<FTKSS> splittedstrips(nssdivs); // internal SS vector in X direction


      // retrieve the hit list for the unsplitted SS
      // obtain the reference to the unsplitted version of the current SS
      unordered_map<int,FTKSS>::iterator iterss = m_fired_ssmap[ip].find(ss);
      assert(iterss!=m_fired_ssmap[ip].end()); // not found, very bad

      FTKSS &curss = (*iterss).second; // reference an existing SS
      // get the hits' list
      const vector<FTKHit> &curhits = curss.getHits();
      vector<FTKHit>::const_iterator ihit = curhits.begin();
      for (;ihit!=curhits.end();++ihit) {
	const FTKHit &curhit = (*ihit);
#ifdef VERBOSE_DEBUG
	cout << "Adding coordinate: " << curhit.getCoord() << endl;
#endif


	/* evaluate the half layer positions. In any case HLx and HLy should
	   be greater than m_tspmap.getFactor(ip,0 or 1) */
	int internal_position(0);
	int HLx(0), HLy(0);
	if (curhit.getDim()==2) { // pixel hit
	  HLx = static_cast<int>(floor(curhit[0])/m_ssmap_tsp->getSSPhiWidth(curhit))-static_cast<int>(floor(curhit[0])/m_ssmap->getSSPhiWidth(curhit))*m_TSPProcessor->getTSPMap().getFactor(ip,0);
	  HLy = static_cast<int>(floor(curhit[1])/m_ssmap_tsp->getSSEtaWidth(curhit))-static_cast<int>(floor(curhit[1])/m_ssmap->getSSEtaWidth(curhit))*m_TSPProcessor->getTSPMap().getFactor(ip,1);
	  assert(HLx<m_TSPProcessor->getTSPMap().getFactor(ip,0) && HLy<m_TSPProcessor->getTSPMap().getFactor(ip,1));

	  internal_position = HLx | 
	    (HLy << m_TSPProcessor->getTSPMap().getInternalBitOffset(ip,0));

	}
	else if (curhit.getDim()==1) {
	  HLx = static_cast<int>(floor(curhit[0])/m_ssmap_tsp->getSSPhiWidth(curhit))-static_cast<int>(floor(curhit[0])/m_ssmap->getSSPhiWidth(curhit))*m_TSPProcessor->getTSPMap().getFactor(ip,0);	
	  assert(HLx<m_TSPProcessor->getTSPMap().getFactor(ip,0));

	  internal_position = HLx;
	}

	// put the current hit in the right sub-SS
	splittedstrips[internal_position].addHit(curhit);
	
      }

      FTKSS newss;
      newss.addSubStrips(splittedstrips);
      m_splitted_ssmap[ip][ss] = newss;     
      isplittedSS = m_splitted_ssmap[ip].find(ss);
    }

    // TODO: if DC is enabled the following lines can be skipped at this point

    // reference to the current splitter SS
    FTKSS &curSS = (*isplittedSS).second;

    /* to become OK one layer needs to have at least one hit in an allowed 
       sub-SS, if not it remains false */
    bool isOK(false);

    const int &nfields = m_TSPProcessor->getTSPMap().getNPositions(ip);
    // check the bits for this layer
    const int &ssmask = m_TSPProcessor->getSubSSMasks()[ipatt][ip];
    for (int ifld=0;ifld!=nfields;++ifld) { // field loop
      // check the region is allowed, otherwise skip
      if ( !(ssmask&(1<<ifld)) ) continue;

      if (curSS.hasHits(ifld)) {
	isOK = true;
	// because 1 is enough
	break;
      }
    } // end field loop

    if (!isOK) {
      /* if the test fails the TSP precision doesn't confirm the layer 
	 for this pattern, the number of layers and the bitmask are updated */
      matchstatus &= (~(1<<(ip+m_matchword_maskshift))); // zeroes the bit
      matchstatus -= 1; // decrease the counter of the fired layers
    }
  } // end layers loop
}

/* the basic functionality is in the FTK_AM::am_output() method.
   In this method, if the correct TSP simulation level is set, the
   roads prepared for the output are filtered again and passed in
   the TSP processor. The roads are updated with the list of fired
   sub-roads. */

void FTKTSPBank::am_output()
{
  FTK_AMBank::am_output();

  // the minumum level for a real TSP simulatin is 3
  if (m_SimulateTSP<3) return;

  list<FTKRoad>::iterator iroad = m_roads.begin();
  while (iroad!=m_roads.end()) { // road loop
    FTKRoad &curroad = *iroad;

    // check if he road is a single-pattern road
    int ipatt = curroad.getPatternID();
    // TODO: introduce a flag to decide if this category hase to be preocessed or not
    if (m_TSPProcessor->getSubPatternID()[ipatt].size()==1 && false) {
      ++iroad; continue; // move to next and continue the loop
    }

    m_TSPProcessor->simulate(curroad);

    // check if there is at least 1 kid
    if (curroad.getNSubRoads()>0) {
      ++iroad; // move to the next item
    }
    else {
      /* remove the current road from the list beacuse is not
	 confirmed by any TSP pattern */
      iroad = m_roads.erase(iroad); // list::erase() move the iterator to the next item
      m_nroads -= 1; // decrease the intependent road counter
    }
    
  } // end road loop
}


void FTKTSPBank::attach_SS()
{
  FTK_AMBank::attach_SS();

  list<FTKRoad>::iterator iroad = m_roads.begin();
  for (;iroad!=m_roads.end();++iroad) {
    FTKRoad &last_road = *iroad; 
    const int &pattID = last_road.getPatternID();

    last_road.setPatternDBID(m_PatternDBID[pattID]);

    if (m_SimulateTSP>1) {
      // set Pattern ID in the DB
      // set the don't care mask
      last_road.setDCBitmask(m_TSPProcessor->getDCMask()[pattID]);
      // set the mask of the *good* half bins
      last_road.setHLBitmask(m_TSPProcessor->getHBMask()[pattID][0]);
      // set the masks of the allowed sub-SS, one each lane
      for (int iplane=0;iplane!=m_nplanes;++iplane)	
	last_road.setSubSSMask(iplane,m_TSPProcessor->getSubSSMasks()[pattID][iplane]);
    }

    // retrieve the hits
    for (int ipl=0;ipl<m_nplanes;++ipl) {
   
      // set the number of bits for each layer
      if (m_SimulateTSP>1)
	last_road.setHLID(ipl,m_TSPProcessor->getTSPMap().getNBits(ipl));
#ifdef VERBOSE_DEBUG
      cout << last_road << endl;
#endif
      
    }
  }
  
}


void FTKTSPBank::setCachePath(const char *fname)
{
  m_cachepath = fname;
}


/** this function saves the */
void FTKTSPBank::dumpCache()
{
  FTKSetup::PrintMessageFmt(ftk::info,"Saving cache in: %s\n",m_cachepath.c_str());
  // open the cache root file
  TFile *rootcache = TFile::Open(m_cachepath.c_str(),"recreate");

  // Create the TTree with 1 branch: the list of the FTK patterns (AM level)
  TTree *amtree = new TTree("Bank","Pattern bank");
  FTKPattern *curpatt = new FTKPattern(m_nplanes);
  amtree->Branch("Pattern",&curpatt);

  // set the number of planes once for all
  curpatt->setNPlanes(m_nplanes);
  // the coverage in the cache file is arbitrary set to -1
  curpatt->setCoverage(-1);

  ipatt_step = (m_npatterns+9)/10;
  if (!ipatt_step) ipatt_step = 1;

  cout << "Writing AM bank cache [" << flush;
  for (int ipatt=0;ipatt!=m_npatterns;++ipatt) { // pattern loop
    if(ipatt % ipatt_step == 0) cout << ipatt/ipatt_step << flush;
    curpatt->clearSubPatterns();
    
    curpatt->setPatternID(ipatt);
    // the DB is actually read in a different way according the simualtion level
    curpatt->setPatternDBID(m_PatternDBID[ipatt]);

    // set the SS id
    for (int iplane=0;iplane!=m_nplanes;++iplane) {
      curpatt->setSSID(iplane,m_patterns[iplane][ipatt]);
    }
   
    // specify the sectors
    curpatt->setSectorID(m_patterns[m_nplanes][ipatt]);

    if (m_SimulateTSP>0)
      m_TSPProcessor->fillPatternInfo(ipatt,*curpatt,m_SimulateTSP>2);
    amtree->Fill();
  } // end pattern loop
  amtree->Write();

  cout << "]" << endl;

  // close and delete the cache root file
  rootcache->Close();
  delete rootcache;
}


AMSelection::AMSelection() : 
  m_AMID(-1), m_Coverage(0), m_TSPID()
{;}

AMSelection::AMSelection(int amid, int tspid, unsigned coverage) : 
  m_AMID(amid), m_Coverage(coverage), m_TSPID()
{
  m_TSPID.push_back(tspid);
}

void AMSelection::addTSP(int id, unsigned coverage)
{ 
  m_Coverage += coverage;
  m_TSPID.push_back(id);
}
    
bool AMSelection::operator<(const AMSelection &AMSel) const
{
  if (m_Coverage>AMSel.m_Coverage) return true;
  else if (m_Coverage<AMSel.m_Coverage) return false;
  
  if (m_AMID<AMSel.m_AMID) return true;
  return false;
}
