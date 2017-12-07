/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
///
/// This class aims to get the event-by-event comparison
/// of two files one containing HW FTK tracks (BS/RAW file)
/// the other containing simulated (SW) FTK tracks (NTUP_FTK file)
/// The comparison output consits of histograms published on OH,
/// if the partition is set-up, or by histograms saved in the 
/// out.histo.root file.
///
/// There are 4 types of histograms:
/// HWSW*: if HW and SW tracks are completely matched, i.e. have the same track parameters within 0.1% of relative error
/// 	  the distrubutions of the track paramters (like pt, eta, ...) are displayed in the histograms
/// HWonly*: if HW tracks do not have SW tracks within m_dmax=0.2, 
/// 	    the distrubutions of these HW track paramters (like pt, eta, ...) are displayed in the histograms
/// SWonly*: if SW tracks do not have HW tracks within m_dmax=0.2, 
/// 	    the distrubutions of these SW track paramters (like pt, eta, ...) are displayed in the histograms
/// HWvsSW: if HW and SW tracks are not completely matched, 
/// 	    i.e. they are geometrically closed but do not have the same track parameters
/// 	    these histograms in turn are divided into :
/// 	    HWvsSWsw*: the distrubutions of the SW track paramters (like pt, eta, ...) are displayed in the histograms
/// 	    HWvsSWhw*: the distrubutions of the HW track paramters (like pt, eta, ...) are displayed in the histograms
/// 	    HWvsSWdiff*: the distrubutions of the difference btw the HW and SW track paramters (like pt, eta, ...) 
/// 			 are displayed in the histograms
/// other general histos are 
/// nTrk_SW: number of SW tracks per event
/// nTrk_HW: number of HW tracks per event
///
////////////////////////////////////////////////////////////////////

#ifndef __CompareFTKEvents__
#define __CompareFTKEvents__


#include <iostream>
#include <fstream>

#include <getopt.h>
#include <vector>
#include <cmath>
#include "TTree.h"
#include "TFile.h"


#include "ByteStreamData/RawEvent.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include "FTKStandaloneMonitoring/FTKTrkAssoc.h"
#include "FTKStandaloneMonitoring/CompareFTKTracks.h"
#undef PACKAGE_VERSION
#include "RunControl/Common/OnlineServices.h"
#include "ipc/partition.h"
#include "ipc/core.h"
#include "oh/OHRootProvider.h"	
#include "RunControl/Common/Exceptions.h"
class CompareFTKEvents{
  public:
    CompareFTKEvents();
    /// initialization with the path/namefile string of the HW (BS/RAW) and SW (NTUP_FTK) files
    CompareFTKEvents(const std::string &BSfile, const std::string &NTUP_FTK_file);
    
    /// setting up partition
    void SetupPartition(const std::string &partition_name);
       
    /// function to read each BS event given the pointer position where it last stopped reading the event
    std::streampos readBSevent(int ievent,std::streampos startbufpos);
    
    /// reading the SW NTUP_FTK TTrees which should contain some TTrees: 
    ///  "evtinfo" containing basic event infos, 
    ///  "ftkdata" containing a branch of FTKTrackStream objects with name "FTKMergedTracksStream"
    void readNTUP_FTKfile();
    
    /// print input files
    void PrintFiles();
    
    /// Looping over events of the HW (BS_FTK) and SW (NTUP_FTK files) and collecting track info
    void EventLoop();
    
    /// histogram initialization: for the naming convention see CompareFTKEvents.h
    void SetHistos(std::vector<std::string> histo_list);
    
    /// change the name of the event info ttree: default is evtinfo
    void SetEvtInfoString(std::string &str_tree_evtinfo){m_str_tree_evtinfo=str_tree_evtinfo;}

    /// change the name of the data ttree: default is ftkdata
    void SetTreeNTUPString(std::string &str_tree_ftkdata){m_str_tree_ftkdata=str_tree_ftkdata;}

    /// change the name of the FTK Stream branch: default is FTKMergedTracksStream
    void SetBranchString(std::string &str_tree_ftkstream){m_str_tree_ftkstream=str_tree_ftkstream;}
    
    /// set verbose
    void SetVerbose(){m_verbose=true;}

    /// set maximum number of events
    void SetNEvents(int nevtmax){m_nevtmax=nevtmax;}

    /// reading BS file, starting loop on events and writing histograms
    void Execute();

    /// Get the total number of events in the BS file
    int GetNEventsBS();
    
    /// destructor that prints out if the tracks were all matched or differences were found
    ~CompareFTKEvents();
  private:

    /// decoding functions from FTKByteStreamDecoderEncoder of package Trigger/TrigFTK/TrigFTKByteStream
    StatusCode decode(uint32_t nTracks, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result);

    /// decoding functions from FTKByteStreamDecoderEncoder to get track info from BS
    FTK_RawTrack* unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data);

    /// decoding functions from FTKByteStreamDecoderEncoder **never tested** and **never used**
    void unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster);
    
    /// decoding functions from FTKByteStreamDecoderEncoder **never tested** and **never used**
    void unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawSCT_Cluster& cluster) {cluster.setWord(*data);}
    
    /// writing histograms into file and/or publishing on OH
    void WriteHistos();

    int m_nevtmax=0;
    const size_t m_TrackBlobSize = 22; // magic number from BS specification
    const size_t m_TrackParamsBlobSize = 6; // --||--
    const size_t m_PixHitParamsBlobSize = 2; // --||--
    const size_t m_SCTHitParamsBlobSize = 1; // --||--
    const size_t m_NPixLayers=4;
    const size_t m_NSCTLayers=8;
    std::vector<uint32_t> m_data;
    std::string m_BSfile;
    std::string m_NTUP_FTK_file;
    uint32_t m_checkword=0xaa1234aa;
    uint32_t m_trackBlockOffsetStart=0;	  // Nr of words in ROB data block before track blocks start
    uint32_t m_trackBlockOffsetEnd=0;	  // Nr of words in ROB data block after track blocks end
    int m_Nevents_BS;
    int m_Nevents_NTUPFTK;
    std::ifstream m_myBSfile;
    std::streampos m_tmpbufpos=0;
    TTree *m_tevtinfo;
    TTree *m_theTree;
    TFile *m_fntupftk;
    int m_RN, m_EN, m_LB, m_BCID, m_EL1ID, m_l1TT;
    FTKTrackStream *m_ft;
    bool m_allmatched=true;
    bool m_verbose=false;
    bool m_setup_partition=false;
    CompareFTKTracks * m_compTrk;
    std::vector<std::string> m_histo_list;
    std::map<std::string , TH1D * > m_map_histo;
    std::map<std::string , TH2D * > m_map_histo_2D;
    TFile * m_fout;
    std::string m_str_tree_evtinfo="evtinfo";
    std::string m_str_tree_ftkdata="ftkdata";
    std::string m_str_tree_ftkstream="FTKMergedTracksStream";
    std::string m_partition_name="";
    std::map<std::string, std::vector<double> > m_histo_param={{"pt",{1000.,0.,100000.}},
                                                              {"eta",{100.,-2.5,2.5}},
							      {"phi",{100.,-3.2,3.2}},
							      {"d0",{100.,-5.,5.}},
							      {"z0",{100,-50,50}},
							      {"chi2",{100,0,50}},
							      {"ETA_PHI",{100,-2.5,2.5,100,-3.2,3.2}}};
    std::map<std::string, std::string>  m_param_units={{"pt"," [MeV]"},
                                                              {"eta",""},
							      {"phi",""},
							      {"d0"," [mm]"},
							      {"z0"," [mm]"},
							      {"chi2",""},
							      {"ETA_PHI",""}};
    std::vector<std::string> m_variable_list={"pt","eta","phi","d0","z0","chi2","ETA_PHI"};    
    IPCPartition                                    m_ipcpartition;     // Partition 
    std::string                         m_name="HLmon_HWSWcomp";
    OHRootProvider                      *m_ohProvider;
    TH1F                                *m_histogram;       // Histogram
  };
#endif //__CompareFTKEvents__
