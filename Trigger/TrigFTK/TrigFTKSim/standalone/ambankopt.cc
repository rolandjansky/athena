/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigFTKSim/tsp/FTKAMSplit.h"
#include "TCanvas.h"

using namespace std;


/*1- calculate total AMbank volume
  2- calculate total DeltaNumberPatterns i.e. increase in # of patterns
  3- calculate volume reduction as average DeltaVolume/DeltaNumberPatters * total DeltaNumberPatters
  4- calculate new volume (AMbank volume - vol. reductio) over AMbank volume
*/


int main(int argc, char *argv[])
{
    /* The arguments are now assumed to be the pattern file (ROOT format)
       and the number of TSP pattern to read.
       In the number of options grows consider moving to getopt (google it)
       or an input file (if really many) */

    // init a FTKSetup object
    FTKSetup &ftksetup = FTKSetup::getFTKSetup();

    // print arguments
    std::cout << "\n\t input arguments: ";
    for ( int i=1; i<argc; i++ ) {
       std::cout << "\n\t\t ["<<i<<"] : " << argv[i];
     }
    std::cout << "\n----------------------------\n\n" << std::flush;



    cout << "Reading TSP ROOT bank: " << argv[1] << endl;
    TFile *m_file = TFile::Open(argv[1]);
    if (!m_file)
    {
        FTKSetup::PrintMessage(ftk::sevr, "Error opening the ROOT bank");
        return -1;
    }

    int maxpatt = atoi(argv[2]);
    int m_setAMSize = 0;

    if ( argc == 4)
    {
        m_setAMSize = atoi(argv[3]);
        cout << "GOT AM SIZE:\t " << m_setAMSize << endl;
    }

    int maxDCbits = atoi(argv[4]);
    

//    else
//    {
//        // m_setAMSize = 0;
//    }

    /* this is the pointer to the TTree containing the AM bank,
       if a real TSP/DC simulation is performed this points to Bank1,
       if the TSP bank is used as test could be Bank0 */
    TTree *AMBank(0x0);

    // this maps will contain the map of used AM pattern and the relative TSP patterns
    map< int, AMSelection > AMPatternMap;
    bool hasBuiltAM(false); // it becomes true if an AM is built: best tsp or minimum coverage
    ftksetup.usageStat("Create AM Bank using the ROOT file");

    /** AM bank is created building grouping the fatherID field of a given
        number of TSP patterns, the number of TSP patterns is decided by the
        user */
    FTKSetup::PrintMessage(ftk::info, "Bank from Best TSP patterns");
    // Get the pointer to the TSP bank TTree and the TTree with the relations
    AMBank = (TTree *) m_file->Get("Bank1");
    TTree *TSPBank = (TTree *) m_file->Get("Bank0");
    TTree *TSPRelation = (TTree *) m_file->Get("TSPRelation0");

    // get the maxium number of TSP patterns
    Long64_t m_npatternsTSP = TSPBank->GetEntries();
    std::cout << "\n\t m_npatternsTSP: " << m_npatternsTSP << std::endl;

    // prepare to read the patterns
    FTKPattern *tsppatt = new FTKPattern();
    TSPBank->SetBranchAddress("Pattern", &tsppatt);
    // prepare to read the AM parent IDs
    Int_t AMID;
    TSPRelation->SetBranchAddress("AMID", &AMID);
    // get the first element to complete the information on the bank
    TSPBank->GetEntry(0);
    int m_nplanes = tsppatt->getNPlanes();

    // disable alla the useless branches
    /*Changed the SetBranchAddress for TSPRelation to keep the HalfPlaneMask in order to save
      the HBMask of every TSP Pattern for the DC bits, in order to choose a better grouping*/
    TBranch *hpmask_b;
    UInt_t hpmask;
    TSPRelation->SetBranchAddress("HalfPlaneMask", &hpmask, &hpmask_b);
    Long64_t m_npatterns;

    // // set number of TSP patterns to read following the maxpatt value
    //     if (m_npatternsTSP<maxpatt || maxpatt<0) {
    //        cout << "Maximum number of TSP patterns to be read: " << m_npatternsTSP << " (asked " << maxpatt << ')' << endl;
    //        maxpatt = m_npatternsTSP;
    //     }
    //     cout << "Found # " << m_npatternsTSP << " TSP patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

    // // m_patternsTSP is set to the maximum number or patterns
    //     if (maxpatt<m_npatternsTSP) m_npatternsTSP = maxpatt;

    // // set m_npatterns to an impossible value the AM generation loop to stop
    //     Long64_t m_npatterns = -1;

    if (!m_setAMSize)
    {
        // set number of TSP patterns to read following the maxpatt value
        if (m_npatternsTSP < maxpatt || maxpatt < 0)
        {
            cout << "Maximum number of TSP patterns to be read: " << m_npatternsTSP << " (asked " << maxpatt << ')' << endl;
            maxpatt = m_npatternsTSP;
        }
        cout << "1 Found # " << m_npatternsTSP << " TSP patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns)" << endl;

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

        cout << "2 Found # " << m_npatterns << " AM patterns in a " << m_nplanes << " planes geometry (limited to " << maxpatt << " patterns, mode " << m_setAMSize << ')' << endl;

        // set the correct limit to the number of AM pattern to create
        if (maxpatt < m_npatterns) m_npatterns = maxpatt;
    }

    
    std::cout << "\n\t values after applying maxpatt:\t m_npatternsTSP: " << m_npatternsTSP << "\t m_npatterns: " << m_npatterns << std::endl; 
    int nAMPatterns(0); // number of AM patterns loaded

    for (int ipatt = 0; ipatt != m_npatternsTSP; ++ipatt) // loop over TSP patterns
    {

        if ( ipatt%1000000==0 )
          std::cout << "\t processing pattern #" << ipatt << " in bank " << TSPBank->GetName() << std::endl;
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

    if (m_setAMSize > 0)
    {
        cout << "m_npatterns:\t" << m_npatterns << endl;
        cout << "nAMPatterns:\t" << nAMPatterns << endl;
        assert(m_npatterns == nAMPatterns);
        cout << "Number of TSP patterns: " << m_npatternsTSP << endl;

    }

    // order the AM bank by coverage
    list<AMSelection> AMPatternList;
    map< int, AMSelection >::const_iterator iAMmap = AMPatternMap.begin();
    FTKAMSplit *amsplit = new FTKAMSplit( maxDCbits );
    for (; iAMmap != AMPatternMap.end(); ++iAMmap)
    {
        AMPatternList.push_back((*iAMmap).second);
    }
    AMPatternMap.clear(); // remove the map used to store the AM patterns
    AMPatternList.sort(); // use the sorting metho of the list


    // TEST threshold
    //    amsplit->setdeltaThresholdBin( 66 );
    // result: this cause a crash!

    if (m_setAMSize > 0)
    {
        AMPatternList = amsplit->splittingAlgorithms(AMPatternList);
    }
    else
    {
        cout << "The number of AM patterns:\t" << AMPatternList.size() << endl;
        AMPatternList = amsplit->splittingAlgorithms(AMPatternList);
    }

    return 0;
}

