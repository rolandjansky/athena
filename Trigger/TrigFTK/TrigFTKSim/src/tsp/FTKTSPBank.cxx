/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPattern.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"
#include "TrigFTKSim/tsp/FTKAMSplit.h"
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

#define PRINT_ROADS_SECTOR -1
#define PRINT_ROADS_NUM 100

FTKTSPBank::FTKTSPBank(int bankid, int subid) :
    FTK_AMBank(bankid, subid),
    m_TSPProcessor(0x0),
    m_file(0),
    m_SimulateTSP(0), m_npatternsTSP(0), m_TSPMinCoverage(0),
    m_setAMSize(0), m_AMSplit(0), m_maxAMAfterSplit(-1), m_minDVolOverDNPatt(0),
    m_DCMatchMethod(0),
    m_ssmap_tsp(0x0), m_splitted_ssmap(0x0),
    m_cachepath(""),
    m_makecache(false)
{}


FTKTSPBank::~FTKTSPBank()
{
    if (m_SimulateTSP > 1)
    {
        delete m_TSPProcessor;
    }

    if (m_splitted_ssmap) delete [] m_splitted_ssmap;
}


/** This function overload the default FTK_AMBank::end() adding
    informations that are more strictly related with the AM banks
    built starting from TSP */
void FTKTSPBank::end()
{
    if (m_do_pattern_stats)
    {
        // write the A bank statistic
        ftk_dcap::ostream bankstat_file;
        ftk_dcap::open_for_write("patternbank.dat", bankstat_file);

        bankstat_file << "PattID Freq" << endl;
        for (int ipatt = 0; ipatt != m_npatterns; ++ipatt)
        {
            bankstat_file << m_PatternDBID[ipatt] << ' ' << m_pattern_stats[ipatt] << endl;
        }
    }
}


int FTKTSPBank::readROOTBank(const char *fname, int maxpatt)
{
    FTKSetup &ftksetup = FTKSetup::getFTKSetup();

    cout << "Reading TSP ROOT bank: " << fname << endl;
    m_file = TFile::Open(fname);
    if (!m_file)
    {
        FTKSetup::PrintMessage(ftk::sevr, "Error opening the ROOT bank");
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
    if (m_SimulateTSP > 0 || m_TSPMinCoverage > 0)
    {
        FTKSetup::PrintMessage(ftk::info, "Generating an AM bank");
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
        if (m_TSPMinCoverage == 0) // no minimum coverage block
        {
            FTKSetup::PrintMessage(ftk::info, "Bank from Best TSP patterns");
            // Get the pointer to the TSP bank TTree and the TTree with the relations
            AMBank = (TTree *) m_file->Get("Bank1");
            TTree *TSPBank = (TTree *) m_file->Get("Bank0");
            TTree *TSPRelation = (TTree *) m_file->Get("TSPRelation0");

            // get the maxium number of TSP patterns


            m_npatternsTSP = TSPBank->GetEntries();

            // prepare to read the patterns
            FTKPattern *tsppatt = new FTKPattern();
            TSPBank->SetBranchAddress("Pattern", &tsppatt);
            // prepare to read the AM parent IDs
            Int_t AMID;
            TSPRelation->SetBranchAddress("AMID", &AMID);

            // get the first element to complete the information on the bank
            TSPBank->GetEntry(0);
            setNPlanes(tsppatt->getNPlanes());

            //get the half plane mask for the splitting algorithms

            TBranch *hpmask_b;
            UInt_t hpmask;
            TSPRelation->SetBranchAddress("HalfPlaneMask", &hpmask, &hpmask_b);
            // disable alla the useless branches
            // TSPRelation->SetBranchAddress("HalfPlaneMask", 0);

            if (!m_setAMSize)
            {
                // set number of TSP patterns to read following the maxpatt value
                if (m_npatternsTSP < maxpatt || maxpatt < 0)
                {
                    cout << "Maximum number of TSP patterns to be read: " << m_npatternsTSP << " (asked " << maxpatt << ')' << endl;
                    maxpatt = m_npatternsTSP;
                }
                cout << "Found # " << m_npatternsTSP << " TSP patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

                // m_patternsTSP is set to the maximum number or patterns
                if (maxpatt < m_npatternsTSP) m_npatternsTSP = maxpatt;

                // set m_npatterns to an impossible value the AM generation loop to stop
                m_npatterns = -1;
            }
            else
            {
                // set number of AM patterns to load following the maxpatt value
                m_npatterns = AMBank->GetEntries(); // set the number of AM patterns

                // check if there are enough patterns
                if (m_npatterns < maxpatt || maxpatt < 0)
                {
                    cout << "Maximum number of AM patterns to be read: " << m_npatterns << " (asked " << maxpatt << ')' << endl;
                    maxpatt = m_npatterns;
                }

                cout << "Found # " << m_npatterns << " AM patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns, mode " << m_setAMSize << ')' << endl;

                // set the correct limit to the number of AM pattern to create
                if (maxpatt < m_npatterns) m_npatterns = maxpatt;
            }

            int nAMPatterns(0); // number of AM patterns loaded
            // for (int ipatt = 0; ipatt != m_npatternsTSP; ++ipatt) // loop over TSP patterns
            // {
            //     // get the ID of this TSP pattern
            //     TSPRelation->GetEntry(ipatt);
            //     TSPBank->GetEntry(ipatt);
            //     hpmask_b->GetEntry(ipatt);

            //     map<int, AMSelection>::iterator iAM = AMPatternMap.find(AMID);
            //     if (iAM == AMPatternMap.end())
            //     {
            //         /* if m_setAMSize==2 the AM is stoppped when the number of patterns
            //            is going to be exceeded */
            //         if (m_setAMSize == 2 && nAMPatterns == m_npatterns)
            //         {
            //             // set this as limit in the TSP pattern bank
            //             m_npatternsTSP = ipatt;
            //             break;
            //         }

            //         // new AM pattern
            //         AMPatternMap[AMID] = AMSelection(AMID, ipatt, tsppatt->getCoverage(), hpmask);
            //         nAMPatterns += 1;

            //         /* if m_setAMSize==1 the AM bank creation stops as soon as
            //            the required size is obtained */
            //         if (m_setAMSize == 1 && nAMPatterns == m_npatterns)
            //         {
            //             // set this as last TSP pattern to be read
            //             m_npatternsTSP = ipatt + 1;
            //             break;
            //         }
            //     }
            //     else
            //     {
            //         // already used by another TSP pattern, just add the current TSP pattern to the relation
            //         (*iAM).second.addTSP(ipatt, tsppatt->getCoverage(), hpmask);
            //     }
            // } // end loop over TSP patterns

            for (int ipatt = 0; ipatt != m_npatternsTSP; ++ipatt) // loop over TSP patterns
            {
                // get the ID of this TSP pattern
                TSPRelation->GetEntry(ipatt);
                TSPBank->GetEntry(ipatt);
                hpmask_b->GetEntry(ipatt);
                map<int, AMSelection>::iterator iAM = AMPatternMap.find(AMID);
                if (iAM == AMPatternMap.end())
                {
                    /* if m_setAMSize==2 the AM is stoppped when the number of patterns
                       is going to be exceeded */
                    if (m_setAMSize == 2 && nAMPatterns == m_npatterns)
                    {
                        // set this as limit in the TSP pattern bank
                        m_npatternsTSP = ipatt;
                        break;
                    }

                    // new AM pattern
                    AMPatternMap[AMID] = AMSelection(AMID, ipatt, tsppatt->getCoverage(), hpmask);
                    nAMPatterns += 1;

                    /* if m_setAMSize==1 the AM bank creation stops as soon as
                       the required size is obtained */
                    if (m_setAMSize == 1 && nAMPatterns == m_npatterns)
                    {
                        // set this as last TSP pattern to be read
                        m_npatternsTSP = ipatt + 1;
                        break;
                    }
                }
                else
                {
                    // already used by another TSP pattern, just add the current TSP pattern to the relation
                    (*iAM).second.addTSP(ipatt, tsppatt->getCoverage(), hpmask);
                }
            } // end loop over TSP patterns

            // set the number of AM patterns found
            m_npatterns = AMPatternMap.size();
            if (m_setAMSize > 0)
            {
                assert(m_npatterns == nAMPatterns);
                cout << "Number of TSP patterns: " << m_npatternsTSP << endl;

            }

            FTKSetup::PrintMessageFmt(ftk::info, "AM bank created with %d AM patterns", m_npatterns);

        } // end of no minimum coverage block
        else   // minimum coverage block
        {
            FTKSetup::PrintMessage(ftk::info, "Generating an AM bank from TSP with minimum coverage");     // check the TSP bank size
            FTKSetup::PrintMessage(ftk::sevr, "Not supported for ROOT banks");

            m_nplanes = 0;
            m_npatterns = 0; // max number AM patterns

            m_npatterns = 0; // real number of AM patterns
            FTKSetup::PrintMessageFmt(ftk::info, "AM bank created with %d AM patterns, asked %d", m_npatterns, maxpatt);
        } // end minimum coverage block

    } // and block for AM building
    else
    {
        FTKSetup::PrintMessage(ftk::info, "Using TSP format to read an AM bank");

        /* Use the ROOT format prepared for the AMTSP/DC simulation but
           uses only the TSP. If here the simulation level and the minimum
           coverage are 0: ! (m_SimulateTSP>0 || m_TSPMinCoverage>0) */

        // The AM and TSP banks are equal
        AMBank = (TTree *) m_file->Get("Bank0");

        FTKPattern *tsppatt = new FTKPattern();
        AMBank->SetBranchAddress("Pattern", &tsppatt);
        AMBank->GetEntry(0);
        m_nplanes = tsppatt->getNPlanes();

        m_npatterns = AMBank->GetEntries();

        if (maxpatt < 0 || m_npatterns < maxpatt) maxpatt = m_npatterns;

        cout << "Found # " << m_npatterns << " in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

        if (maxpatt < m_npatterns) m_npatterns = maxpatt;
    }

    // order the AM bank by coverage
    list<AMSelection> AMPatternList;
    map< int, AMSelection >::const_iterator iAMmap = AMPatternMap.begin();
    for (; iAMmap != AMPatternMap.end(); ++iAMmap)
    {
        AMPatternList.push_back((*iAMmap).second);
    }
    AMPatternMap.clear(); // remove the map used to store the AM patterns
    AMPatternList.sort(); // use the sorting metho of the list

    if (m_AMSplit>0) {
//      FTKAMSplit amsplit(m_AMSplit); //Set up the functions to split AM Patterns
     FTKAMSplit amsplit(m_AMSplit, m_maxAMAfterSplit, m_minDVolOverDNPatt); //Set up the functions to split AM Patterns
     AMPatternList = amsplit.splittingAlgorithms(AMPatternList); //Function that calls the selected splitting algorithm
    }

    ftksetup.usageStat("Reading the AM bank");


    // initialize the AM data structures
    readBankInit();

    // prepare the DC
    if (m_SimulateTSP > 1)
    {
        ftksetup.usageStat("Read TSP patterns");

        FTKSetup::PrintMessage(ftk::info, "TSPProcessor creation");
        // prepare internal hit distribution structure, if required
        if (!m_DCMatchMethod) m_splitted_ssmap = new map<int, FTKSS>[m_nplanes];

        // nested TSP processor
        m_TSPProcessor = new TSPLevel(m_file, 2,
                                      this, AMPatternList,
                                      m_nplanes - 1, m_ssmap, m_ssmap_tsp,
                                      m_splitted_ssmap, m_SimulateTSP > 2);
        if (!m_npatternsTSP) // if the number of patterns is missing it can be get from the TSP processor
            m_npatternsTSP = m_TSPProcessor->getNPatterns();

    }

    // define the step
    ipatt_step = (m_npatterns + 9) / 10;
    if (!ipatt_step) ipatt_step = 1;

    cout << "Reading : [" << flush;
    FTKPattern *ampatt = new FTKPattern();
    AMBank->SetBranchAddress("Pattern", &ampatt);
    list<AMSelection>::iterator iAMlist = AMPatternList.begin();
    for (int ipatt = 0; ipatt != m_npatterns; ++ipatt)
    {
        if (ipatt % ipatt_step == 0) cout << ipatt / ipatt_step << flush;

        // Read the AM pattern
        int curAMID(ipatt);
        if (hasBuiltAM)
        {
            curAMID = (*iAMlist).getAMID();
            ++iAMlist; // move the pointer to the next
        }

        AMBank->GetEntry(curAMID);
        m_PatternDBID.push_back(curAMID);

        for (int iplane = 0; iplane != m_nplanes; ++iplane)  // loop on the planes
        {
            // the m_nplnes+1 element is the number of the sector
            // the other are the SS index
            m_patterns[_SSPOS(ipatt, iplane)] = ampatt->getSSID(iplane);
        }
        m_patterns[_SSPOS(ipatt, m_nplanes)] = ampatt->getSectorID();
    }
    cout << "]" << endl;

// set number of DC bits in FTKSSMap
    for (int iplane=0; iplane<m_nplanes; iplane++)
    {
      m_ssmap_tsp->setSSDCX(iplane,m_TSPProcessor->getTSPMap().getNBits(iplane,0));
      m_ssmap_tsp->setSSDCY(iplane,m_TSPProcessor->getTSPMap().getNBits(iplane,1));
    }

    // can close the ROOT file, this
    m_file->Close();

    readBankInitEnd();

    if (m_makecache)
    {
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
    TTree *amtree = dynamic_cast<TTree *>(rootcache->Get("Bank"));
    if (amtree == NULL)  {
       FTKSetup::PrintMessage(ftk::sevr, "Could not get amtree");
       return 0;
    }
    FTKPattern *curpatt = 0x0;
    // 2016-05-04 STS: add modifications to read flat file
    TBranch *branch_nplanes=0,*branch_sectorID=0,
       *branch_ssid=0,*branch_dcmask=0,*branch_hbmask=0;
    int nplanes;
    if(amtree->SetBranchAddress("Pattern", &curpatt)!=0) {
       // no match, assume this is a flat file
       FTKSetup::PrintMessage(ftk::info,"pcache read failed, try flat format");
       branch_nplanes=amtree->GetBranch("nplanes");
       branch_nplanes->SetAddress(&nplanes);
       branch_nplanes->GetEntry(0);
       setNPlanes(nplanes);
       curpatt=new FTKPattern(nplanes);
       branch_sectorID=amtree->GetBranch("sectorID");
       branch_ssid=amtree->GetBranch("ssid");
       branch_dcmask=amtree->GetBranch("dcmask");
       branch_hbmask=amtree->GetBranch("hbmask");
       branch_sectorID->SetAddress(&curpatt->m_sectorID);
       branch_ssid->SetAddress(curpatt->m_ssid);
       branch_hbmask->SetAddress(&curpatt->m_hbmask);
       branch_dcmask->SetAddress(&curpatt->m_dcmask);
    } else {
       // use the first pattern to setup global variables
       amtree->GetEntry(0);
       setNPlanes(curpatt->getNPlanes());
    }

    // get the number of the patterns
    m_npatterns = amtree->GetEntries();

    // initialize the AM data structures
    readBankInit();

    if (m_SimulateTSP > 1)
    {
        FTKSetup::PrintMessage(ftk::info, "TSPProcessor preparation for cache read");
        // prepare don't care data
        if (!m_DCMatchMethod) m_splitted_ssmap = new map<int, FTKSS>[m_nplanes];
        // nested TSP processor
        m_TSPProcessor = new TSPLevel(2, m_npatterns, m_nplanes - 1, m_ssmap, m_ssmap_tsp, m_splitted_ssmap, m_SimulateTSP > 2);
    }

    ftksetup.usageStat("Read AM cached bank");
    // define the step
    ipatt_step = (m_npatterns + 9) / 10;
    if (!ipatt_step) ipatt_step = 1;

    // reset the number of the TSP patterns
    m_npatternsTSP = 0;
    int nWCtotal=0;
    int nVETOtotal=0;

    cout << "Reading : [" << flush;
    for (int ipatt = 0; ipatt != m_npatterns; ++ipatt) // pattern loop
    {
        if (ipatt % ipatt_step == 0) cout << ipatt / ipatt_step << flush;

        if(!branch_sectorID) {
           // STS 2016-05-04
           // this is a bug??? DBID is taken from the previous pattern???
           //    m_PatternDBID.push_back(curpatt->getPatternDBID());
           // set the current pattern
           amtree->GetEntry(ipatt);
        } else {
           branch_sectorID->GetEntry(ipatt);
           branch_ssid->GetEntry(ipatt);
           branch_dcmask->GetEntry(ipatt);
           branch_hbmask->GetEntry(ipatt);
           curpatt->setPatternDBID(ipatt);
           curpatt->setPatternID(ipatt);
        }
        // STS 2016-05-04
        //  moved here
        m_PatternDBID.push_back(curpatt->getPatternDBID());

        for (int iplane = 0; iplane != m_nplanes; ++iplane)
           {
              m_patterns[_SSPOS(ipatt, iplane)] = curpatt->getSSID(iplane);
           }
        // translate wildcards and dead modules
        // for HWMODEID==2
         if (FTKSetup::getFTKSetup().getHWModeSS()==2) {
            int nWC=0;
            for (int iplane = 0; iplane != m_nplanes; ++iplane) {
               if(m_patterns[_SSPOS(ipatt, iplane)] == -1) {
                  // wildcard in HWMODEID==2
                  nWC++;
               }
            }
            for (int iplane = 0; iplane != m_nplanes; ++iplane) {
               if(m_patterns[_SSPOS(ipatt, iplane)] == -1) {
                  // wildcard or 
                  if(nWC == m_nplanes) {
                     // special pattern
                     // ignore
                     m_patterns[_SSPOS(ipatt, iplane)] =m_VetoID;
                  } else {
                     // wildcard
                     m_patterns[_SSPOS(ipatt, iplane)] = m_WCID;
                     nWCtotal++;
                  }
               } else if(m_patterns[_SSPOS(ipatt, iplane)] ==-2) {
                  // veto
                  m_patterns[_SSPOS(ipatt, iplane)] = m_VetoID;
                  nVETOtotal++;
               }
            }
        }
        m_patterns[_SSPOS(ipatt, m_nplanes)] = curpatt->getSectorID();
        
        if (m_SimulateTSP > 1)
           {
              int nTSPpatterns = m_TSPProcessor->getPatternInfo(ipatt, curpatt, m_SimulateTSP > 2);
              m_npatternsTSP += nTSPpatterns;
           }
    } // end pattern loop
    cout << ']' << endl;
    //cout<<"nVETOtotal="<<nVETOtotal<<" nWCtotal="<<nWCtotal<<"\n";

    ftksetup.usageStat("End AM cache read");

    // set number of DC bits in FTKSSMap
    for (int iplane=0; iplane<m_nplanes; iplane++)
    {
      m_ssmap_tsp->setSSDCX(iplane,m_TSPProcessor->getTSPMap().getNBits(iplane,0));
      m_ssmap_tsp->setSSDCY(iplane,m_TSPProcessor->getTSPMap().getNBits(iplane,1));
    }

    FTKSetup::PrintMessageFmt(ftk::info, "Loaded #%d TSP Patterns", m_npatternsTSP);
    readBankInitEnd();

    if (m_DCMatchMethod>0) {
      m_TSPProcessor->clearExtraInfo();
    }
    // set the flag to use a cached bank
    m_CachedBank = true;
    return m_npatterns;
}


void FTKTSPBank::pattlookup_make_map() {
  if (!m_DCMatchMethod) {
    FTK_AMBank::pattlookup_make_map();
    return;
  }

  // create a LUT that is aware of the DC
  FTKSetup::PrintMessage(info,"Creating the pattern LUT extracting the DC information");

  // Main loop, over planes
  int nSpecial=0;
  for (int iplane=0; iplane!=m_nplanes; ++iplane) {
    const int ndcbits(m_TSPProcessor->getTSPMap().getNBits(iplane));

    // Count patterns for ss;
    for (int i = 0; i < m_npatterns; ++i) {
      if(!(i%ipatt_step)) cout << "Layer " << iplane << ": counting patterns per ss (" << i << ")" << endl;
      // m_pattern content is an array, not a matrix
      int basess = m_patterns[_SSPOS(i,iplane)];

      // retrive the DC mask for this pattenr in this layer
      unsigned int localdcmask(m_TSPProcessor->getDCMask(i,iplane));

      // retrieve the reference high resolution position for this pattern
      unsigned int localhbmask(m_TSPProcessor->getHBMask(i,iplane));
      // set to zero all the bits intereseted by the DC
      localhbmask &= ~localdcmask; // where the DC is set to 0 become 1 end viceversa

      // the index is composed by the full SS, with highres part, and the DC
      // STS 26.2.2017: take care of wildcards
      int iss;
      if((basess==m_WCID)||(basess==m_VetoID)) {
         iss=basess;
         nSpecial++;
      } else {
         iss = basess<<(ndcbits*2) | localhbmask<<ndcbits | localdcmask; // create the SS
      }
#if 0
      if (i==0) {
        cout << ">>> " << basess << " " << localdcmask << " " << m_TSPProcessor->getHBMask(i,iplane) << " " << localhbmask << " " << iss <<endl;
      }
#endif

      if ( m_ss_patt_lookup_map[iplane].find(iss) == m_ss_patt_lookup_map[iplane].end() ) {
        m_ss_patt_lookup_map[iplane][iss] = std::vector<int>();
      }
      m_ss_patt_lookup_map[iplane][iss].push_back(i);
    }
    cout << endl;
    //cout<<"Number of veto/wildcard SSIDs: "<<nSpecial<<"\n";

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

/** This method organizes the hits in SSs, in case of two stages match the
 * original implementation is called.*
 */
void FTKTSPBank::data_organizer() {
  if (!m_DCMatchMethod) {
    FTK_AMBank::data_organizer();
    return;
  }

  /* if the DC matching method is enabled the hits need to be organized
   * in SS accordin the 2 precision levels
   */
  //FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const FTKPlaneMap *pmap = m_ssmap->getPlaneMap();

  for (int iplane=0; iplane<m_nplanes /* &&m_useWC */; ++iplane) { // loop over the planes
    // create the WC SS
    FTKSS WCSS;

    FTKHit fakehit(pmap->isSCT(iplane) ? 1 : 2);
    // cout <<" WC plane=" <<iplane <<endl;
    WCSS.addHit(fakehit);
    // cout <<"WCSS.addHit(fakehit)"<<endl;

    // add the WC SS to the map of fired WC
    m_fired_ssmap[iplane][m_WCID] = WCSS;
    if (m_useMinimalAMIN) m_fired_ss[iplane].insert(m_WCID);
  }

  // send real hits into the SS
  for (int iplane=0; iplane!=m_nplanes; ++iplane) { // loop over the planes
    for (vector<FTKHit>::iterator ihit = m_stlhit_sort[iplane].begin();
        ihit!=m_stlhit_sort[iplane].end(); ++ihit ) { // hit loop

      FTKHit &curhit(*ihit);

      int ss(-1);
      if (FTKSetup::getFTKSetup().getHWModeSS()==0) {
        // First calculate the SS for the coarse resolution part
        ss = m_ssmap->getSSGlobal(curhit) << m_TSPProcessor->getTSPMap().getNBits(iplane);


        // then calculate the SS for the high resolution part
        // The TSPMap object knows about the relation between the low and high resolution
        // part of the SSs
        ss |= m_TSPProcessor->getTSPMap().getHighResSSPart(curhit);
#if 0
        cout << ">>> " << iplane << " " << m_ssmap->getSSGlobal(curhit) << " " << m_TSPProcessor->getTSPMap().getHighResSSPart(curhit) << " " << ss << " " << curhit[0] << " " << curhit[1] << endl;
#endif
      }
      else if (FTKSetup::getFTKSetup().getHWModeSS()==2) {
      // Calculate the SS for HWModeID=2
	  // tower ID = bankid
      ss = m_ssmap_tsp->getSSTower(curhit,FTK_AMBank::getBankID());
      }
      else {
        FTKSetup::PrintMessage(sevr,"HWMode should be 0 or 2");
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
        if (m_useMinimalAMIN) m_fired_ss[iplane].insert(ss);
      }
      (*curss).second.addHit(curhit);
    } // end loop over elms
    m_nao_nss.push_back(m_fired_ssmap[iplane].size());
  } // end loop over the layers

}


void FTKTSPBank::am_in_dc() {

  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  unsigned int match_threshold = m_nplanes-ftkset.getMaxMissingPlanes()-FTKSetup::getFTKSetup().getMaxMissingSctPairs();
  m_fired_patts.clear();

  // send real hits into the SS
  int nSpecial=0;
  for (int iplane=0; iplane!=m_nplanes; ++iplane) { // loop over the planes
    const unsigned int mask(1<<(iplane+m_matchword_maskshift));

    // extract the infromations related to the DC
    const unsigned int ndcbits(m_TSPProcessor->getTSPMap().getNBits(iplane));
    const unsigned int ndcconf(1<<ndcbits);

    unordered_map<int,FTKSS>::iterator ssiter = m_fired_ssmap[iplane].begin();
    unordered_map<int,FTKSS>::iterator ssiter_end = m_fired_ssmap[iplane].end();
    for (;ssiter!=ssiter_end;++ssiter) { // loop over the SS found in that layer
      const int &basessid((*ssiter).first);

      // the SS from the real hits is exploded, according the DC configuration
      for (unsigned int dcconf=0;dcconf!=ndcconf;++dcconf) { // loop over the dc configurations
         unsigned int curssid;
         // 26.2.2017 STS: bug-fix to take care of wildcards
         if(basessid==m_WCID) {
            curssid=basessid;
            nSpecial++;
         } else {
            curssid= basessid & (~dcconf); // mask to 0 all the hits where the dc is on
            curssid <<= ndcbits; // shift to the left to make room for the dc as key
            curssid |= dcconf; // add the DC configuration in the key
         }
#if 0
        cout << ">>> " << iplane << " " << basessid << " " << dcconf << " " << curssid << endl;
#endif
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
      } // end loop over the dc configurations
    } // end loop over the SS found in this layer

  } // loop over planes
  /*cout<<"nSpecial="<<nSpecial<<"\n";
    if(!nSpecial) exit(0); */

  if (FTKSetup::getDBG()) {
    for(int i=0;i!=m_nplanes;i++) {
      cout << "DBG: Routed superstrips " << i << " = " << m_fired_ssmap[i].size() << endl;
    }
  }

}

/** This is a new version of am_in() method. The basic functionality
    is made in the FTK_AMBank::am_in() method. The following code emulates
    the prepare the SS within the fired roads to be used in the DC filter
    and in for the TSP simulation */
void FTKTSPBank::am_in()
{
  if (m_DCMatchMethod) {
    am_in_dc();
    return;
  }

  /* the basic work of the AM is simulated in the base implementation
       of the AM simulation */
  FTK_AMBank::am_in();

  // for now when the DC LUT is created, the data_organizer() collects the hits in proper way
  // and the LUT takes care of everyhin, so the orifinal am_in is enough.
  if (m_DCMatchMethod) return;

  // if the simulation is at level 0 or 1 only default AM simulation is performed
  if (m_SimulateTSP < 2) return;

  // clear the splitted SS maps
  for (int ip = 0; ip != m_nplanes; ++ip) // plane loop
  {
    m_splitted_ssmap[ip].clear();
  } // end plane loop

  /* The TSP simulation works only on the list of fired roads */
  vector<int>::iterator fpiter = m_fired_patts.begin();
  for (; fpiter != m_fired_patts.end(); ++fpiter) // road loop
  {
    int ipatt = (*fpiter);

    // look in more detail the fired pattern
    filterPattern(ipatt);
  } // end road loop

#ifdef NEWSSLIST
  // reassign the SS, the unsplitted SS are sub changed using the splitted version
  for (int ip = 0; ip != m_nplanes; ++ip) // plane loop
  {
    map<int, FTKSS>::iterator iss = m_splitted_ssmap[ip].begin();
    for (; iss != m_splitted_ssmap[ip].end(); ++iss) // ss loop
    {
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
    pattstatus_t &matchstatus = m_patterns_matchstatus[ipatt];

    for (int ip = 0; ip != m_nplanes; ++ip) // layers loop
    {
        // check the bitmask and skip if the layer is empty
        if ((matchstatus & (1 << (ip + m_matchword_maskshift))) == 0) continue;

        // search the super-strip for this plane
        int ss = m_patterns[_SSPOS(ipatt, ip)];

        // if the SS id is not valid, TSP or DC don't apply
        if ( ss < 0 || ss == m_WCID || ss == m_VetoID)  continue;

        // find the splitted version for this super-strip
        map<int, FTKSS>::iterator isplittedSS = m_splitted_ssmap[ip].find(ss);
        if (isplittedSS == m_splitted_ssmap[ip].end())
        {
            // the splitted version doesn't exist and should be created

            /* The super-strips content is divided according the internal
            sub-division. The number of sub-divisions depend on the
             the number of bits needed to describe this layer */
            int nssdivs(m_TSPProcessor->getTSPMap().getNPositions(ip));
            vector<FTKSS> splittedstrips(nssdivs); // internal SS vector in X direction


            // retrieve the hit list for the unsplitted SS
            // obtain the reference to the unsplitted version of the current SS
            unordered_map<int, FTKSS>::iterator iterss = m_fired_ssmap[ip].find(ss);
            assert(iterss != m_fired_ssmap[ip].end()); // not found, very bad

            FTKSS &curss = (*iterss).second; // reference an existing SS
            // get the hits' list
            const vector<FTKHit> &curhits = curss.getHits();
            vector<FTKHit>::const_iterator ihit = curhits.begin();
            for (; ihit != curhits.end(); ++ihit)
            {
                const FTKHit &curhit = (*ihit);
#ifdef VERBOSE_DEBUG
                cout << "Adding coordinate: " << curhit.getCoord() << endl;
#endif


                /* evaluate the half layer positions. In any case HLx and HLy should
                   be greater than m_tspmap.getFactor(ip,0 or 1) */
                int internal_position(0);
                int HLx(0), HLy(0);
                if (curhit.getDim() == 2) // pixel hit
                {
                    HLx = static_cast<int>(floor(curhit[0]) / m_ssmap_tsp->getSSPhiWidth(curhit)) - static_cast<int>(floor(curhit[0]) / m_ssmap->getSSPhiWidth(curhit)) * m_TSPProcessor->getTSPMap().getFactor(ip, 0);
                    HLy = static_cast<int>(floor(curhit[1]) / m_ssmap_tsp->getSSEtaWidth(curhit)) - static_cast<int>(floor(curhit[1]) / m_ssmap->getSSEtaWidth(curhit)) * m_TSPProcessor->getTSPMap().getFactor(ip, 1);
                    assert(HLx < m_TSPProcessor->getTSPMap().getFactor(ip, 0) && HLy < m_TSPProcessor->getTSPMap().getFactor(ip, 1));

                    internal_position = HLx |
                                        (HLy << m_TSPProcessor->getTSPMap().getInternalBitOffset(ip, 0));

                }
                else if (curhit.getDim() == 1)
                {
                    HLx = static_cast<int>(floor(curhit[0]) / m_ssmap_tsp->getSSPhiWidth(curhit)) - static_cast<int>(floor(curhit[0]) / m_ssmap->getSSPhiWidth(curhit)) * m_TSPProcessor->getTSPMap().getFactor(ip, 0);
                    assert(HLx < m_TSPProcessor->getTSPMap().getFactor(ip, 0));

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
        for (int ifld = 0; ifld != nfields; ++ifld) // field loop
        {
            // check the region is allowed, otherwise skip
            if ( !(ssmask & (1 << ifld)) ) continue;

            if (curSS.hasHits(ifld))
            {
                isOK = true;
                // because 1 is enough
                break;
            }
        } // end field loop

        if (!isOK)
        {
            /* if the test fails the TSP precision doesn't confirm the layer
            for this pattern, the number of layers and the bitmask are updated */
            matchstatus &= (~(1 << (ip + m_matchword_maskshift))); // zeroes the bit
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
    if (m_SimulateTSP < 3) return;

    list<FTKRoad>::iterator iroad = m_roads.begin();
    while (iroad != m_roads.end()) // road loop
    {
        FTKRoad &curroad = *iroad;

        // check if he road is a single-pattern road
        int ipatt = curroad.getPatternID();
        // TODO: introduce a flag to decide if this category hase to be preocessed or not
        if (m_TSPProcessor->getSubPatternID()[ipatt].size() == 1 && false)
        {
            ++iroad; continue; // move to next and continue the loop
        }

        m_TSPProcessor->simulate(curroad);

        // check if there is at least 1 kid
        if (curroad.getNSubRoads() > 0)
        {
            ++iroad; // move to the next item
        }
        else
        {
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
    for (; iroad != m_roads.end(); ++iroad)
    {
        FTKRoad &last_road = *iroad;
        const int &pattID = last_road.getPatternID();

        last_road.setPatternDBID(m_PatternDBID[pattID]);
        last_road.setDCMatchMode(m_DCMatchMethod);

        if (m_SimulateTSP > 1)
        {
            // set Pattern ID in the DB
            // set the don't care mask
            last_road.setDCBitmask(m_TSPProcessor->getDCMask()[pattID]);
            // set the mask of the *good* half bins
            last_road.setHLBitmask(m_TSPProcessor->getHBMask()[pattID][0]);
            // set the masks of the allowed sub-SS, one each lane
            for (int iplane = 0; iplane != m_nplanes && !m_DCMatchMethod; ++iplane)
                last_road.setSubSSMask(iplane, m_TSPProcessor->getSubSSMasks()[pattID][iplane]);
        }

        // retrieve the hits
        for (int ipl = 0; ipl < m_nplanes; ++ipl)
        {
          // set the number of bits for each layer
          if (m_SimulateTSP > 1) {
            unsigned int ndcbits = m_TSPProcessor->getTSPMap().getNBits(ipl);

            last_road.setHLID(ipl, ndcbits);

            if (m_DCMatchMethod) { // if the direct match is used additional fields need to be reported
              // change the SS accoring the specific convention
              int basess = m_patterns[_SSPOS(pattID,ipl)];

              // retrieve the DC mask for this pattenr in this layer
              unsigned int localdcmask(m_TSPProcessor->getDCMask(pattID,ipl));
              // retrieve the reference high resolution position for this pattern
              unsigned int localhbmask(m_TSPProcessor->getHBMask(pattID,ipl));
              // set to zero all the bits intereseted by the DC
              localhbmask &= ~localdcmask; // where the DC is set to 0 become 1 end viceversa

              // the SS is composed by the full SS, with highres part
              last_road.setSSID(ipl,basess<<ndcbits | localhbmask);
            }

          }
#ifdef VERBOSE_DEBUG
            cout << last_road << endl;
#endif

        }
    }

}


/** This function return the roads, it now returns the m_amout content
    in a different format, then when m_roads list will active it
    will use that */
const std::list<FTKRoad>& FTKTSPBank::getRoads()
{
  if (!m_DCMatchMethod) return FTK_AMBank::getRoads();

  // FlagJT: moved ~half of this function to attach_SS() so that we
  // can take advantage of the KD Tree in road_warrior().
  std::list<FTKRoad>::iterator iroad = m_roads.begin();
  for (;iroad!=m_roads.end();++iroad) {
    const FTKRoad& curroad(*iroad);

    // retrive the DC information, obtain for all the layers
    unsigned int dcmask = curroad.getDCBitmask();
    unsigned int maskoffset(0); // offset to read the DC

    const int pattID = curroad.getPatternID();

    /* retrieve the hits connecting preparing the SS to be saved.
       the connection is skipped if the whole map of SS is required for AUX-card related studies.
    */
    for (int ipl=0;(ipl!=m_nplanes)&&(!getStoreAllSS());++ipl) {
      // number of DC bits for this layer
      unsigned int ndcbits = curroad.getHLID(ipl);

      if (!curroad.hasHitOnLayer(ipl)) {
        maskoffset += ndcbits;
        continue;
      }

      // return the super-strip for this plane, TO CLEAN
      int ss = m_patterns[_SSPOS(pattID,ipl)];

      // skip the ss map check if the SS ID is in this list
      if( ss < 0 || ss == m_WCID || ss == m_VetoID) {
        maskoffset += ndcbits;
        continue;
      }

      // base full resolution SS for this layer, as written in the road
      // this is done because slightly differnt convention can be used
      // for the AM map and the format written on disk/memory
      int basessid = curroad.getSSID(ipl);

      // extract DC mask for this layer, reading only the related bits
      unsigned int localdcmask = (dcmask>>maskoffset)&(~(~0<<ndcbits));
      // in the basessid mask the bits involved by the DC placing them to 0
      basessid &= ~localdcmask;

      // check how many bits are set
      unsigned int nuseddcbits(0);
      for ( unsigned int ibit = 0; ibit!=ndcbits; ++ibit) {
        if ((localdcmask>>ibit)&1) nuseddcbits += 1;
      }

      // check if there is at least 1 combination that is ok
      bool hasHits(false);

      // number of combination of SS to read, it is a power of 2
      unsigned int ncombs(1<<nuseddcbits);
      for ( unsigned int icomb=0;icomb!=ncombs;++icomb) { // loop over the bit combinations
        int curssid(basessid); // initiaze the SS ID using the masked full resolution SS

        // selector of the bit to be copied
        unsigned int activepos(0);
        for (unsigned ibit=0;ibit!=ndcbits;++ibit) { // loop over the DC bits
          if (localdcmask&(1<<ibit)) {
            // if this is one of the bits masked by the DC,
            // copy the given bit content of the combination into the ssid
            curssid |= ((icomb>>activepos)&1)<<ibit;
            activepos += 1; // next position
          }
        } // end loop over the DC bits

#if 0
        cout << ">>> " << ipl << " " << curssid << " " << basessid << " " << localdcmask << " " << ndcbits << " ";
#endif

        // check if the SS was found during the event
        unordered_map<int,FTKSS>::iterator item0 = m_fired_ssmap[ipl].find(curssid);
        if (item0==m_fired_ssmap[ipl].end()) {
#if 0
          cout << 0 << endl;
#endif
          continue; // no hits
        }

#if 0
        cout << 1 << endl;
#endif

        hasHits = true;

        // map for this plane
        unordered_map<int,FTKSS> &imap = m_usedssmap[ipl];
        // find this ss is in the bank
        unordered_map<int,FTKSS>::iterator item = imap.find(curssid);

        if (item==imap.end()) { // not found
          imap[curssid] = (*item0).second;
        }
        else {
          // super-strip already in the map, skip this
          continue;
        }
      } // end loop over the combination of DC configurations

#if 1
      if (!hasHits) {
        FTKSetup::PrintMessage(sevr,"No SS found while storing SS maps");
      }
#endif
#if 0
      if (!hasHits) cout << "No SS found pattern " << pattID<< endl;
#endif

      maskoffset += ndcbits;
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


void FTKTSPBank::setCachePath(const char *fname)
{
    m_cachepath = fname;
}


/** this function saves the */
void FTKTSPBank::dumpCache()
{
    FTKSetup::PrintMessageFmt(ftk::info, "Saving cache in: %s\n", m_cachepath.c_str());
    // open the cache root file
    TFile *rootcache = TFile::Open(m_cachepath.c_str(), "recreate");

    // Create the TTree with 1 branch: the list of the FTK patterns (AM level)
    TTree *amtree = new TTree("Bank", "Pattern bank");
    FTKPattern *curpatt = new FTKPattern(m_nplanes);
    amtree->Branch("Pattern", &curpatt);

    // set the number of planes once for all
    curpatt->setNPlanes(m_nplanes);
    // the coverage in the cache file is arbitrary set to -1
    curpatt->setCoverage(-1);

    ipatt_step = (m_npatterns + 9) / 10;
    if (!ipatt_step) ipatt_step = 1;

    cout << "Writing AM bank cache [" << flush;
    for (int ipatt = 0; ipatt != m_npatterns; ++ipatt) // pattern loop
    {
        if (ipatt % ipatt_step == 0) cout << ipatt / ipatt_step << flush;
        curpatt->clearSubPatterns();

        curpatt->setPatternID(ipatt);
        // the DB is actually read in a different way according the simualtion level
        curpatt->setPatternDBID(m_PatternDBID[ipatt]);

        // set the SS id
        for (int iplane = 0; iplane != m_nplanes; ++iplane)
        {
            curpatt->setSSID(iplane, m_patterns[_SSPOS(ipatt, iplane)]);
        }

        // specify the sectors
        curpatt->setSectorID(m_patterns[_SSPOS(ipatt, m_nplanes)]);

        if (m_SimulateTSP > 0)
            m_TSPProcessor->fillPatternInfo(ipatt, *curpatt, m_SimulateTSP > 2);
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
{
    ;
}

AMSelection::AMSelection(int amid, int tspid, unsigned coverage, unsigned int hpmask) :
    m_AMID(amid), m_Coverage(coverage), m_TSPID(), m_HalfPlaneMask()
{
    m_TSPID.push_back(tspid);
    m_HalfPlaneMask.push_back(hpmask);
    m_TSPCoverage.push_back(coverage);
}


void AMSelection::addTSP(int id, unsigned coverage, unsigned int hpmask )
{
    m_Coverage += coverage;
    m_TSPID.push_back(id);
    m_HalfPlaneMask.push_back(hpmask);
    m_TSPCoverage.push_back(coverage);
}

bool AMSelection::operator<(const AMSelection &AMSel) const
{
    if (m_Coverage > AMSel.m_Coverage) return true;
    else if (m_Coverage < AMSel.m_Coverage) return false;

    if (m_AMID < AMSel.m_AMID) return true;
    return false;
}
