/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/RoadFinder.h"
#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"
#include "TrigFTKSim/FTK_RegionalRawInput.h"
#include "TrigFTKSim/FTK_RawInput.h"
#include "TrigFTKSim/FTKRoadFileOutput.h"
#include "TrigFTKSim/FTKSectorMap.h"
#include "TrigFTKSim/tsp/FTKAMSplit.h"
#include "common_fcn.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// instance of the RoadFinder object
RoadFinder rfobj;

// ss offset (set through config file)
double ss_offset_fraction = 0;

// decide if the input come from the regional wrapper or not. 0 means NO
int RegionalWrapper(0);
// set the sub-region under analysis, if 0 the information is not used
int encode_subregion(0);
// set the use of the TSP DB banks
int useTSPBank(0);
// if the TSP bank is used it is possible to ask a specific level
int BankLevel(1);
// set if the TSP simulation is performed, 0 not used
int doTSPSim(0);
// set if patterns should be split, 0 no splitting occurs
int DCSplit(0);
// minimum TSP coverage, if >1 the way the AM bank is built change
int minTSPCoverage(1);
// integer flag to set if the number of patterns is at AM or TSP level
int setAMSize(0);
// store the variable if the TSP bank cache has to be saved
int doMakeCache(0);
// path of the output file
string CachePath("bankcache.root");
// boolean value (0 false, !=0 true) that interprets an existing bank as a cached, usually for TSP/DC banks
int CachedBank(0);

int SaveAllRoads(0);

// flag for the AM statistic collection
int doBankStat(0);
int dumpAllSS(0);

// Plane map pointer, to be set during the initialization
FTKPlaneMap *pmap(0);
FTKPlaneMap *pmap_unused(0);
// Super-strip map, this object transform an hit in a SS index
FTKSSMap *ssmap(0);
FTKSSMap *ssmap_unused(0);
FTKSSMap *ssmap_tsp(0);
// set if the SS use the HW-like mode
unsigned int HWModeSS(0);
// require presence of first layer (pixel B-layer)
bool require_first(false);
// use hashmap internally in AM simulation (allows super-small SS widths)
bool force_am_hashmap(false);
// region map used in this session
FTKRegionMap *rmap(0);
FTKRegionMap *rmap_unused(0);
int CUR_REGION(-1);
// lists containing output of 8L run
std::string scttrk_tracklist;
std::string scttrk_roadlist;

std::string m_badmap_path;
std::string m_badmap_path2;

// 4L,8L -> 11L lookup map
FTKSectorMap *scttrk_sectormap(0);

int read_commands();

/** main function */
int main(int argc, char *argv[])
{
    cout << "Road Finding" << endl;
    FTKSetup &ftkset = FTKSetup::getFTKSetup();
    ftkset.setVerbosity(0);

    common_fcn_init(argc, argv);

    // Prepare the output module
    FTKRoadFileOutput *road_output = new FTKRoadFileOutput();
    rfobj.setRoadOutputModule(road_output);

    // preliminary parsing of the input file
    ifstream optionfile;
    streambuf *oldrdbuf = cin.rdbuf();
    if (argc > 1)
    {
        // input option in an external file
        optionfile.open(argv[1]);
        if (!optionfile)
        {
            cerr << "*** Error reading input file: " << argv[1] << endl;
            return -1;
        }
        // input option parsed from stdin
        cin.rdbuf(optionfile.rdbuf());
    }
    // call the parser
    if (read_commands() < 0)
    {
        cerr << "*** Error parsing the options" << endl;
        return -1;
    }
    cin.rdbuf(oldrdbuf);

    int totevt = ftkset.getTotEvents();
    int ievt_step = totevt > 500 ? totevt / 100 : 1;
    // input module init
    cout << "I/O initialization" << endl;
    // instanciate the output module
    rfobj.init();
    for (int ievt = 0; ievt < totevt; ++ievt)   // events processing loop
    {
        if (ievt % ievt_step == 0)
        {
            cout << "Processing evt # " << ievt << " / " << totevt << endl;
            if (ievt % (ievt_step * 10) == 0)
                ftkset.usageStat();
        }

        int res = rfobj.nextEvent();
        if (res < 0)   // error
        {
            cerr << "*** error reading event # " << ievt << endl;
            break;
        }
    } // events procesing loop
    rfobj.end();

    ftkset.usageStat();

    // destroy the I/O modules
    delete road_output;
    delete rfobj.getDataInputModule();
    delete ssmap_tsp;
    return 0;
}

/** this function parse the input commands and prepare a simple structure
    used by the main function */
int read_commands()
{
    // ftk environemnt
    FTKSetup &ftkset = FTKSetup::getFTKSetup();

    const char prompt[] = "PARSE> ";

    string line;
    while (replace_getline(cin, line) != 0)
    {
        if (line[0] == '#' || line.size() == 0)
        {
            continue;
        }

        cout << prompt << line << endl;

        // extraxt the pair (key,value)
        istringstream sline(line);
        string key;
        sline >> key;
        if (key == "IGNORE_SIGSEGV")
        {
            // disable ROOT signal handler (it's buggy and may cause deadlocks after segfault)
            int ignore_sigsegv;
            sline >> ignore_sigsegv;
            if (ignore_sigsegv)
                gSystem->IgnoreSignal(kSigSegmentationViolation, true);
        }
        else if (key == "BANK_STAT")
        {
            sline >> doBankStat;
        }
        else if (key == "DUMPALLSS")
        {
            sline >> dumpAllSS;
        }
        else if (key == "events#")
        {
            int ival;
            sline >> ival;
            cout << prompt << ival << " events will be processed" << endl;
            ftkset.setTotEvents(ival);
        }
        else if (key == "VERBOSITY")
        {
            int ival;
            sline >> ival;
            ftkset.setVerbosity(ival);
        }
        else if (key == "BARREL_ONLY")
        {
            int ival;
            sline >> ival;
            cout << prompt << "BARREL_ONLY = " << ival << endl;
            ftkset.setBarrelOnly(ival);
        }
        else if (key == "ENABLE_FTKSIM")
        {
            int ival;
            sline >> ival;
            cout << prompt << key << " = " << ival << endl;
            ftkset.setEnableFTKSim(ival);
        }
        else if (key == "HWMODEID")
        {
            sline >> HWModeSS;
            cout << prompt << key << " = " << HWModeSS << endl;
        }
        else if (key == "SS_FILE")
        {
            string sval;
            sline >> sval;
            cout << prompt << key << " = " << sval << endl;
            ssmap = new FTKSSMap(rmap, sval.c_str(), force_am_hashmap == false);
        }
        else if (key == "SS_FILE_UNUSED")
        {
            string sval;
            sline >> sval;
            cout << prompt << key << " = " << sval << endl;
            rmap_unused = new FTKRegionMap(pmap_unused, rmap->getPath());
            // the map on unused layer skip the boundary check by default
            ssmap_unused = new FTKSSMap(rmap_unused, sval.c_str(), false);
        }
        else if (key == "SS_FILE_TSP")
        {
            string sval;
            sline >> sval;
            cout << prompt << key << " = " << sval << endl;
            // the map on unused layer skip the boundary check by default
            ssmap_tsp = new FTKSSMap(rmap, sval.c_str(), false);
        }
        else if (key == "SS_OFFSET")
        {
            double sval;
            sline >> sval;
            cout << prompt << key << " = " << sval << endl;
            ss_offset_fraction = sval;
        }
        else if (key == "MAX_MISSING_PLANES")
        {
            int ival;
            sline >> ival;
            cout << prompt << key << " = " << ival << endl;
            ftkset.setMaxMissingPlanes(ival);
        }
        else if (key == "MASKLAYERS")
        {
            int ival;
            sline >> ival;
            cout << prompt << key << " = " << ival << endl;
            ftkset.setMaskLayers(ival);
        }
        else if (key == "ROADWARRIOR")
        {
            int ival;
            sline >> ival;
            cout << prompt << key << " = " << ival << endl;
            ftkset.setRoadWarrior(ival);
        }
        else if (key == "KEEP_REMOVED")
        {
            int ival;
            sline >> ival;
            ftkset.setKeepRemoved(ival);
        }
        else if (key == "REGIONAL_WRAPPER")
        {
            sline >> RegionalWrapper;
        }
        else if (key == "INPUTFILE")
        {
            // add a single file to the list
            string sval;
            sline >> sval;
            FTKDataInput *dinput = rfobj.getDataInputModule();
            if (dinput->addFile(sval.c_str()) != -1)
            {
                cout << "File: " << sval << " added" << endl;
            }
        }
        else if (key == "FTKDAT_LIST")
        {
            // add this list of files as input
            string sval;
            sline >> sval;
            FTKDataInput *dinput = rfobj.getDataInputModule();
            int res = dinput->addFilesList(sval.c_str());
            cout << "Added: " << res << " files" << endl;
        }
        else if (key == "SCTTRK_MODE")
        {
            int ival;
            sline >> ival;
            ftkset.setSCTtrkMode(ival);
        }
        else if (key == "IBL_MODE")
        {
            int ival;
            sline >> ival;
            ftkset.setIBLMode(ival);
        }
        else if (key == "SCTTRK_TRACKLIST")
        {
            sline >> scttrk_tracklist;
        }
        else if (key == "SCTTRK_ROADLIST")
        {
            sline >> scttrk_roadlist;
        }
        else if (key == "SCTTRK_SECTORMAP")
        {
            string sval;
            sline >> sval;
            // sector map is only needed in SCTtrk mode
            if (ftkset.getSCTtrkMode())
            {
                scttrk_sectormap = new FTKSectorMap(sval.c_str());
            }
        }
        else if (key == "SCTTRK_NLAYERS")
        {
            int ival;
            sline >> ival;
            rfobj.setSCTtrkNlayers(ival);
        }
        else if (key == "SCTTRK_NSUBS")
        {
            int ival;
            sline >> ival;
            rfobj.setSCTtrkNsubs(ival);
        }
        else if (key == "NSUBREGIONS")
        {
            int ival;
            sline >> ival;
            rfobj.setNsubregions(ival);
        }
        else if (key == "SCTTRK_LASTLAYER")
        {
            int ival;
            sline >> ival;
            rfobj.setSCTtrkLastLayer(ival);
        }
        else if (key == "SCTTRK_MAX_MISSING_PLANES")
        {
            int ival;
            sline >> ival;
            rfobj.setSCTtrkMaxMissingPlanes(ival);
        }
        else if (key == "CUR_REGION")
        {
            int ival;
            sline >> ival;
            CUR_REGION = ival;
        }
        else if (key == "CUR_SUBREGION")
        {
            int ival;
            sline >> ival;
            rfobj.setSubregion(ival);
        }
        else if (key == "REQUIRE_FIRST")
        {
            bool ival;
            sline >> ival;
            require_first = ival;
        }
        else if (key == "FORCE_AM_HASHMAP")
        {
            bool ival;
            sline >> ival;
            force_am_hashmap = ival;
        }
        else if (key == "ENCODE_SUBREGION")
        {
            sline >> encode_subregion;
        }
        else if (key == "SECTORSASPATTERNS")
        {
            int ival;
            sline >> ival;
            ftkset.setSectorsAsPatterns(ival);
        }
        else if (key == "MAKECACHE")
        {
            sline >> doMakeCache;
        }
        else if (key == "CACHEPATH")
        {
            sline >> CachePath;
        }
        else if (key == "NBANKS")
        {
            int ival;
            sline >> ival;
            // set the number of banks
            rfobj.setNBanks(ival);
        }
        else if (key == "USETSP_BANK")
        {
            // 0 - not use the TSP ready bank, >0 use the TSP bank
            sline >> useTSPBank;
        }
        else if (key == "USETSP_DBLEVEL")
        {
            // choose the level:
            // 1 - best TSP level,, 2 - 1st group, 3 - 2nd group....
            sline >> BankLevel;
        }
        else if (key == "USETSP_SIM")
        {
            /* 0 -  use the TSP bank but only as AM,
            1 - build the AM related without further steps
             2 - build the AM related bank and the DC mask
             3 - simulate the TSP
                 */
            sline >> doTSPSim;
        }
        else if (key == "DCBITS_SPLIT")
        {
            /*
            0: No DC Split is performed, maximum grouping of thin patterns into AM patterns is done
            1: Use at most 1 DC bit for AM patterns after split
            2: Use at most 2 DC bit for AM patterns after split
             ...
            99: Use at most 99 DC bit for AM patterns after split
            100: Complete-Linkage Clustering algorithm
            */
            sline >> DCSplit;
        }
        else if (key == "USETSP_MINCOVERAGE")
        {
            /* If this value is >1 the AM is built using all the TSP
            patterns with the required coverage, than the AM bank is
             cutted
                 */
            sline >> minTSPCoverage;
        }
        else if (key == "USETSP_SETAMSIZE")
        {
            /* if 1 the TSP bank set the number of patterns to load as number of
            AM patterns */
            sline >> setAMSize;
        }
        else if (key == "SAVEALLROADS")
        {
            sline >> SaveAllRoads;
        }
        else if (key == "BADMOD_FILE")      // set the bad module map path
        {
            sline >> m_badmap_path;
        }
        else if (key == "BADMOD_FILE_FOR_HIT")      // set the bad module map path
        {
            sline >> m_badmap_path2;
        }
        else if (key == "CACHEDBANK")
        {
            sline >> CachedBank; // 0 False (default), !=0 true. The values has an effect in the following lines
        }
        else if (key == "BANK")
        {
            int ibank, maxpatt;
            string bankpath;
            /* retrieve the number of the bank the tha max number
            of patterns to read (-1 means all patterns) */
            sline >> ibank >> maxpatt;
            // the next line is the bank path
            replace_getline(cin, bankpath);
            FTK_AMBank *bank = 0x0;
            if (useTSPBank)
            {
                FTKTSPBank *tspbank = new FTKTSPBank(ibank, encode_subregion);
                // set the TSP variables
                tspbank->setSimulateTSP(doTSPSim);
                tspbank->setSSMapTSP(ssmap_tsp);
                tspbank->setTSPMinCoverage(minTSPCoverage);
                tspbank->setAMSize(setAMSize);
                tspbank->setMakeCache(doMakeCache > 0 ? true : false);
                tspbank->setCachePath(CachePath.c_str());
                tspbank->setAMSplit(DCSplit);
                bank = tspbank;
            }
            else bank = new FTK_AMBank(ibank, encode_subregion);

      FTKSetup::getFTKSetup().setHWModeSS(HWModeSS);
            bank->setPatternStats(doBankStat);
            bank->setStoreAllSS(dumpAllSS > 0 ? true : false);

            /* if this is set in a TSP simulation also the roads rejected
            by the DC mechanism or by the TSP are recorded. In this case
             the number of hits is correctly updated to the number of verified
             layers */
            bank->setSaveAllRoads(SaveAllRoads);
            bank->setRequireFirst(require_first);
            // additional adjustments for SCTtrk mode
            if (ftkset.getSCTtrkMode())
            {
                // always require SCTtrk layer in AM matching
                bank->setRequireLast(true);
            }

            ssmap->setOffsetFraction(ss_offset_fraction);
            bank->setSSMap(ssmap);
            bank->setSSMapUnused(ssmap_unused);

            // set the bad module map path
            bank->setBadSSMapPath ( m_badmap_path );
            bank->setBadSSMapPath2 ( m_badmap_path2 );


            if (bankpath.substr(bankpath.size() - 2).compare("db") == 0)
            {
                if (bank->readDBBank(bankpath.c_str(), maxpatt, BankLevel) < 0)
                {
                    delete bank;
                    return -1;
                }
            }
            else if (bankpath.substr(bankpath.size() - 4) == string("root"))
            {
                if (!CachedBank)
                {
                    // use the ROOT routine
                    if (bank->readROOTBank(bankpath.c_str(), maxpatt) < 0)
                    {
                        // error reading the file
                        delete bank;
                        return -1;
                    }
                }
                else
                {
                    // use the ROOT cached routine
                    if (bank->readROOTBankCache(bankpath.c_str()) < 0)
                    {
                        // error reading the file
                        delete bank;
                        return -1;
                    }
                }
            }
            else
            {
                // if the previous check failed try with ASCII file
                if (bank->readASCIIBank(bankpath.c_str(), maxpatt) < 0)
                {
                    // error reading the file
                    delete bank;
                    return -1;
                }
            }
            rfobj.setAMBank(ibank, bank);
            ftkset.usageStat();
        }
        else if (key == "RAW_MODE")
        {
            int ival;
            sline >> ival;
            ftkset.setRawMode(ival);
            FTKDataInput *imod(0);
            switch (ival)
            {
            case 1:
                if (!pmap)
                {
                    cout << "*** RAW data input module needs a valid PMap"
                         << endl;
                    cout << "*** Call PMAP before RAW_MODE" << endl;
                    return -1;
                }

                if (RegionalWrapper)
                    imod = new FTK_RegionalRawInput(pmap);
                else
                    imod = new FTK_RawInput(pmap);

                rfobj.setDataInputModule(imod);
                break;
            case 2:
                if (!pmap)
                {
                    cout << "*** RAW data input module needs a valid PMap"
                         << endl;
                    cout << "*** Call PMAP before RAW_MODE" << endl;
                    return -1;
                }
                else if (!pmap_unused)
                {
                    cout << "*** RAW data input module needs a valid PMap_unused"
                         << endl;
                    cout << "*** Call PMAP_UNUSED before RAW_MODE" << endl;
                    return -1;
                }


                if (RegionalWrapper)
                    imod = new FTK_RegionalRawInput(pmap, pmap_unused);
                else
                    imod = new FTK_RawInput(pmap, pmap_unused);

                imod->setSaveUnused(true);
                rfobj.setDataInputModule(imod);
                rfobj.setSSSearchUnused(true);
                break;
            default:
                cout << "*** RAW_MODE=" << ival << " is not supported" << endl;
                return -1;
            }
        }
        else if (key == "PMAP_FILE")
        {
            // load the plane map configuration file
            string sval;
            sline >> sval;
            pmap = new FTKPlaneMap(sval.c_str());
            if (!(*pmap))
            {
                cerr << "*** error loading plane map in: " << sval << endl;
                return -1;
            }
        }
        else if (key == "PMAP_FILE_UNUSED")
        {
            // load the plane map configuration file
            string sval;
            sline >> sval;
            pmap_unused = new FTKPlaneMap(sval.c_str());
            if (!(*pmap_unused))
            {
                cerr << "*** error loading plane map in: " << sval << endl;
                return -1;
            }
        }
        else if (key == "SCT_PAIR")
        {
            int ival;
            sline >> ival;
            if (pmap)
            {
                pmap->setEnablePlanePair(ival);
            }
            else
            {
                cerr
                        << "*** impossible to define SCT_PAIR variable, PMAP not instantiated"
                        << endl;
                return -1;
            }
        }
        // First key is for backwards compatibility
        else if (key == "ENABLESCTMISSPAIR" || key == "MAX_MISSING_SCT_PAIRS")
        {
            int ival;
            sline >> ival;
            ftkset.setMaxMissingSctPairs(ival);
        }
        else if (key == "RESTRICT_SCT_PAIR_LAYER")
        {
            int ival;
            sline >> ival;
            ftkset.setRestrictSctPairLayer(ival);
        }
        else if (key == "RESTRICT_SCT_PAIR_MODULE")
        {
            int ival;
            sline >> ival;
            ftkset.setRestrictSctPairModule(ival);
            if (ival)
                // This implies RESTRICT_SCT_PAIR_LAYER
                ftkset.setRestrictSctPairLayer(ival);
        }
        else if (key == "RMAP_FILE")
        {
            // set rmap file
            string sval;
            sline >> sval;
            rmap = new FTKRegionMap(pmap, sval.c_str());
            if (!(*rmap))
            {
                cerr << "*** error using map from: " << sval << endl;
                return -1;
            }
        }
        else if (key == "MODULE_LUT") {
            // request the RMAP to load the online module LUT
            if (!rmap) {
                cerr << "The RMAP object has to be declared before" << endl;
                return -1;
            }
            string sval;
            sline >> sval;
            rmap->loadModuleIDLUT(sval.c_str());
        }
        else if (key == "CLUSTERING")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                cout << "Clustering enabled" << endl;
                dinput->setClustering(true);
            }
            else
            {
                cout << "Clustering disabled" << endl;
                dinput->setClustering(false);
            }
        }
        else if (key == "SAVE_CLUSTER_CONTENT")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                cout << "Clustering is saved" << endl;
                dinput->setSaveClusterContent(true);
            }
            else
            {
                cout << "Clustering content is dropped" << endl;
                dinput->setSaveClusterContent(false);
            }
        }
        else if (key == "DIAG_CLUSTERING")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                dinput->setDiagClustering(true);
            }
            else
            {
                dinput->setDiagClustering(false);
            }
        }
        else if (key == "SCT_CLUSTERING")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                dinput->setSctClustering(true);
            }
            else
            {
                dinput->setSctClustering(false);
            }
        }
        else if (key == "PIXEL_CLUSTERING_MODE")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            dinput->setPixelClusteringMode(ival);
        }
        else if (key == "IBL3D_REALISTIC")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                dinput->setIbl3DRealistic(true);
            }
            else
            {
                dinput->setIbl3DRealistic(false);
            }
        }
        else if (key == "DUPLICATE_GANGED")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                dinput->setDuplicateGanged(true);
            }
            else
            {
                dinput->setDuplicateGanged(false);
            }
        }
        else if (key == "GANGED_PATTERN_RECOGNITION")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                dinput->setGangedPatternRecognition(true);
            }
            else
            {
                dinput->setGangedPatternRecognition(false);
            }
        }
        else if (key == "SPLIT_BLAYER_MODULES")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();

            if (ival > 0)
            {
                dinput->setSplitBlayerModules(true);
            }
            else
            {
                dinput->setSplitBlayerModules(false);
            }
        }
        else if (key == "MULT_OUT")
        {
            int ival;
            sline >> ival;
            // set if the output module saves an outfile
            // for each input file, works only with ascii input data
            FTKRoadOutput *outmode = rfobj.getRoadOutputModule();
            if (ival)
                outmode->setMultiOut(true);
            else
                outmode->setMultiOut(false);
        }
        else if (key == "OUT_FILE")
        {
            string sval;
            sline >> sval;
            FTKRoadOutput *outmode = rfobj.getRoadOutputModule();
            outmode->setFileName(sval.c_str());
        }
        else if (key == "OUT_DIR")
        {
            string sval;
            sline >> sval;
            FTKRoadOutput *outmode = rfobj.getRoadOutputModule();
            outmode->setOutDir(sval.c_str());
        }
        else if (key == "READ_CLUSTERS")
        {
            int ival;
            sline >> ival;
            FTKDataInput *dinput = rfobj.getDataInputModule();
            if ( ival )
            {
                dinput->setReadClusters(true);
                cout << "Reading clusters rather than raw hits" << endl;
            }
            else
            {
                dinput->setReadClusters(false);
                cout << "Reading raw hits" << endl;
            }
        }
        else
        {
            cout << prompt << "\"" << key << "\" is not a valid key" << endl;
            continue;
        }
    }

    // Initialize 8L SCTtrk road and track input
    if (ftkset.getSCTtrkMode()) // cy
    {
        if (!scttrk_sectormap)
        {
            cout << "SCTTRK_SECTORMAP was not set in the config file" << endl;
            exit(0);
        }
        rfobj.setSectorMap(scttrk_sectormap);
        FTKDataInput *dinput = rfobj.getDataInputModule();
        if (!rmap)
        {
            cout << "*** SCTTRK_LIST data input module needs a valid RMap"
                 << endl;
            return -1;
        }
        if (CUR_REGION == -1)
        {
            cout << "*** Set CUR_REGION in order to load 8L SCTtrk roads"
                 << endl;
            return -1;
        }
        // Sanity check
        if (ftkset.getMaxMissingSctPairs() > rfobj.getSCTtrkMaxMissingPlanes())
        {
            cout
                    << "*** WARNING: MAX_MISSING_SCT_PAIRS > SCTTRK_MAX_MISSING_PLANES"
                    << endl << "*** Setting SCTTRK_MAX_MISSING_PLANES = "
                    << ftkset.getMaxMissingSctPairs() << endl;
            rfobj.setSCTtrkMaxMissingPlanes(ftkset.getMaxMissingSctPairs());
        }
        dinput->useRoadsTracks(rmap->getNumRegions(), CUR_REGION);
        int res = dinput->addTrackFilesList(CUR_REGION,
                                            scttrk_tracklist.c_str());
        cout << "Added: " << res << " track files" << endl;
        res = dinput->addRoadFilesList(CUR_REGION, scttrk_roadlist.c_str());
        cout << "Added: " << res << " road files" << endl;
        dinput->initRoadsTracks();
    }
    // Sanity check for the default case
    else if (ftkset.getMaxMissingSctPairs() > ftkset.getMaxMissingPlanes())
    {
        cout << "*** WARNING: MAX_MISSING_SCT_PAIRS > MAX_MISSING_PLANES"
             << endl << "*** Setting MAX_MISSING_PLANES = "
             << ftkset.getMaxMissingSctPairs() << endl;
        ftkset.setMaxMissingPlanes(ftkset.getMaxMissingSctPairs());
    }

    return 0;
}
