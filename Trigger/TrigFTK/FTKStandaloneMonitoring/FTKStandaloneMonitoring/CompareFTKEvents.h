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
#include <memory>


#include "ByteStreamData/RawEvent.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include "FTKStandaloneMonitoring/FTKTrkAssoc.h"
#include "FTKStandaloneMonitoring/CompareFTKTracks.h"
#undef PACKAGE_VERSION
#include "RunControl/Common/OnlineServices.h"
#include "RunControl/Common/Exceptions.h"

class IPCPartition;
class TFile;
class TTree;

class CompareFTKEvents{

  public:
    CompareFTKEvents();
    /// initialization with the path/namefile string of the HW (BS/RAW) and SW (NTUP_FTK) files
    CompareFTKEvents(const std::string &BSfile, const std::string &NTUP_FTK_file);
    
   /// setting up partition
   std::unique_ptr<const IPCPartition*> GetIPCPartition(const std::string& partition_name);
       
    /// function to read each BS event given the pointer position where it last stopped reading the event
    std::streampos readBSevent(int ievent,std::streampos startbufpos, std::vector<uint32_t>& data);
    
    /// reading the SW NTUP_FTK TTrees which should contain some TTrees: 
    ///  "evtinfo" containing basic event infos, 
    ///  "ftkdata" containing a branch of FTKTrackStream objects with name "FTKMergedTracksStream"
    bool readNTUP_FTKfile(const std::string& FTKfile);
    
    /// Get the total number of events in the BS file
    bool read_BSfile(const std::string& BSfile);
    
    /// Looping over events of the HW (BS_FTK) and SW (NTUP_FTK files) and collecting track info
    void ExecuteEventLoop(int evtmax=0);
    
    /// histogram initialization: for the naming convention see CompareFTKEvents.h
    void SetHistos(std::vector<std::string> histo_list);
    
    /// set verbose
    void SetVerbose(){m_verbose=true;}

    /// writing histograms into file and/or publishing on OH
    void WriteHistosToFile(const std::string& filename = "out.histo.root");
    void PublishHistosToPartition(const std::string& partition_name = "");

    /// destructor that prints out if the tracks were all matched or differences were found
    ~CompareFTKEvents();
  private:

    /// decoding functions from FTKByteStreamDecoderEncoder of package Trigger/TrigFTK/TrigFTKByteStream
    void decode(uint32_t nTracks, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result);

    /// decoding functions from FTKByteStreamDecoderEncoder to get track info from BS
    FTK_RawTrack* unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data);

    /// decoding functions from FTKByteStreamDecoderEncoder **never tested** and **never used**
    void unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster);
    
    /// decoding functions from FTKByteStreamDecoderEncoder **never tested** and **never used**
    void unpackSCTCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawSCT_Cluster& cluster) {cluster.setWord(*data);}
    
    static const size_t m_TrackBlobSize = 22; // magic number from BS specification
    static const size_t m_TrackParamsBlobSize = 6; // --||--
    static const size_t m_PixHitParamsBlobSize = 2; // --||--
    static const size_t m_SCTHitParamsBlobSize = 1; // --||--
    static const size_t m_NPixLayers=4;
    static const size_t m_NSCTLayers=8;
    static const uint32_t m_checkword=0xaa1234aa;
   //std::vector<uint32_t> m_data;
    uint32_t m_trackBlockOffsetStart=0;	  // Nr of words in ROB data block before track blocks start
    uint32_t m_trackBlockOffsetEnd=0;	  // Nr of words in ROB data block after track blocks end
    int m_Nevents_BS;
    std::ifstream m_myBSfile;
    TTree *m_tevtinfo;
    TTree *m_theTree;
    TFile *m_fntupftk;
    int m_RN, m_EN, m_LB, m_BCID, m_EL1ID, m_l1TT;
    FTKTrackStream *m_ft;
    bool m_allmatched=true;
    bool m_verbose=false;
    //bool m_setup_partition=false;
   //std::string m_partition_name;
    CompareFTKTracks* m_compTrk;
    std::vector<std::string> m_histo_list;
    std::map<std::string , TH1D * > m_map_histo;
    std::map<std::string , TH2D * > m_map_histo_2D;
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
    static const std::string             m_name;
  };
#endif //__CompareFTKEvents__
