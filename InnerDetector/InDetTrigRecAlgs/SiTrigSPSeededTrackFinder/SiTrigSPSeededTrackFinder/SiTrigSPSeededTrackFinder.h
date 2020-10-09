/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: SiTrigSPSeededTrackFinder.h
//
// author: Patricia Conde Muino 29/08/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Trigger top algorithm to reconstruct tracks. It follows the
//              structure of the SiSPSeededTrackFinder but it works as an HLT
//              algorithm (it runs once per ROI and it reconstructs only the
//              region of interest indicated in the job options).
//
// Modified :
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////
#ifndef SiTrigSPSeededTrackFinder_H
#define SiTrigSPSeededTrackFinder_H

#include <string>
#include <ostream>

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "IRegionSelector/IRegSelTool.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

#include "TrkEventUtils/PRDtoTrackMap.h"

//forward declarations
class TrigTimer;

namespace InDet {
  class ISiSpacePointsSeedMaker;
  class ISiZvertexMaker;
  class ISiTrackMaker;
}

namespace InDet {
  
  //!< Class-algorithm for track finding in Pixels and SCT
  //!< initiated by space points seeds
  class SiTrigSPSeededTrackFinder : public HLT::FexAlgo 
  {
    
    ///////////////////////////////////////////////////////////////////
    //!< Public methods:
    ///////////////////////////////////////////////////////////////////
    
  public:
    
    ///////////////////////////////////////////////////////////////////
    //!< Standard Algotithm methods
    ///////////////////////////////////////////////////////////////////
    
    SiTrigSPSeededTrackFinder(const std::string &name, 
			      ISvcLocator *pSvcLocator);
    virtual ~SiTrigSPSeededTrackFinder() {};
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    
    ///////////////////////////////////////////////////////////////////
    //!< Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
    
    MsgStream&    dump     (MsgStream&    out) const;
    std::ostream& dump     (std::ostream& out) const;
    
  protected:
    
    ///////////////////////////////////////////////////////////////////
    //!< Protected data 
    ///////////////////////////////////////////////////////////////////
    
    bool                    m_useZvertexTool; 
    bool                    m_useSeedMaker;
    int                     m_outputlevel;  //!< Print level for debug
    int                     m_nprint;       //!< Kind of  print    
    int                     m_nseeds;       //!< Number seeds
    int                     m_ntracks;      //!< Number found tracks
    int                     m_nseedsTotal;  // Number seeds
    int                     m_nGoodSeedsTotal;  // Number of good seeds
    int                     m_nBadSeedsTotal;  // Number of bad seeds
    int                     m_ntracksTotal; // Number found tracks
    int                     m_nInvoked;     // number of invocations
    int                     m_nZvtx;        // Number z vertices
    int                     m_nDetElPixel;  //Number of pixel det elements
    int                     m_nDetElSCT;    //Number of SCT det elements
    int                     m_nBadSeeds{};    // number of bad seeds found
    int                     m_nGoodSeeds{};    // number of good seeds found
    int                     m_nfreeCut;     // Min number free clusters
    double                  m_AvtimeGoodSeeds; //average processing time for good seeds
    double                  m_AvtimeBadSeeds; //average processing time for bad seeds
    double                  m_timeGoodSeeds; //processing time for good seeds
    double                  m_timeBadSeeds; //processing time for bad seeds
    double                  m_timeGoodSeedsTotal{}; //processing time for good seeds
    double                  m_timeBadSeedsTotal{}; //processing time for bad seeds
    double                  m_RoIEta;      
    double                  m_RoIPhi;


    ToolHandle< ISiSpacePointsSeedMaker > m_seedsmaker;   // Space poins seed     maker
    ToolHandle< ISiZvertexMaker         > m_zvertexmaker; // Space poins z-vertex maker
    ToolHandle< ISiTrackMaker           > m_trackmaker;   // Track                maker
    
    //!< Trigger part
    ToolHandle<IRegSelTool>     m_regionSelector_pixel { this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel" };    //!< pixel region selector tool
    ToolHandle<IRegSelTool>     m_regionSelector_sct   { this, "RegSelTool_SCT",   "RegSelTool/RegSelTool_SCT" };      //!< sct region selector tool

    StringProperty m_prdToTrackMap
       {this,"PRDtoTrackMap",""};                        //!< optional map between PRDs and tracks to identify shared hits.

    double       m_etaHalfWidth;   //!< ROI half-width in eta.
    double       m_phiHalfWidth;   //!< ROI half-width in phi.
    bool           m_doFullScan;   //!< skip the RegionSelector and newRegion() reconstruction 
                                   //        in case of FullScan

    bool         m_fastTracking;    //simple version of the internal seeding + ambiguity solving
      
    double  trackQuality(const Trk::Track*);
    void    filterSharedTracks(std::multimap<double,Trk::Track*>&);


    //timeout checking
    bool           m_doTimeOutChecks;   //check global timer

    //!< Timing
    TrigTimer      *m_timerRegSel;
    TrigTimer      *m_timerZVertexTool;
    TrigTimer      *m_timerSeedsMaker;
    TrigTimer      *m_timerTrackMaker;
    TrigTimer      *m_timerSeedProcessing;

    ///////////////////////////////////////////////////////////////////
    // !< Protected methods
    ///////////////////////////////////////////////////////////////////
    
    MsgStream&    dumptools(MsgStream&    out) const;
    MsgStream&    dumpevent(MsgStream&    out) const;
    
  };
  MsgStream&    operator << (MsgStream&   ,const SiTrigSPSeededTrackFinder&);
  std::ostream& operator << (std::ostream&,const SiTrigSPSeededTrackFinder&); 

}
#endif // SiTrigSPSeededTrackFinder_H
