/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_EXTRATREEALG_H
#define MUONCALIB_EXTRATREEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <set>
#include "MuonPattern/MuonPatternCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"

#include "MuonCalibExtraTreeAlg/MuonCalibPhiHit_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibPhiPattern_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTrack_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibHit_EBranch.h"
#include "MuonCalibExtraTreeAlg/MuonCalibTrackSegmentBranch.h"

class StoreGateSvc;
class TFile;
class TDirectory;
class TTree;

class MdtIdHelper;

namespace MuonGM {
 class MuonDetectorManager;
}

namespace MuonCalib {
class IExtraTreeFillerTool;
class IIdToFixedIdTool ;
class  ISegmentOnTrackSelector;
  /**
     @class MuonCalibExtraTreeAlg

     This athena algorithm retrieves several collection (PhiPatterns,
     Tracks and Holes) from StoreGate and feeds the collection to the
     different 'Extended Branches':
     - MuonCalib::MuonCalibHit_EBranch
     - MuonCalib::MuonCalibHole_EBranch 
     - MuonCalib::MuonCalibPhiHit_EBranch 
     - MuonCalib::MuonCalibPhiPattern_EBranch 
     - MuonCalib::MuonCalibTrack_EBranch

     These branches are added to the CalibrationNtuple as created in
     MuonSpectrometer/MuonCalib/MuonCalibNtuple.

     For the content of the Extended Branches, see 
     <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibExtraNtupleContent"><span>ExtraTree Ntuple wiki</span></a>

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class MuonCalibExtraTreeAlg : public AthAlgorithm {
  public:
    MuonCalibExtraTreeAlg(const std::string &name, ISvcLocator *pSvcLocator); //!< Athena Algorithm constructor declaring the several properties ( doPhi/doTracks/doHoles adding extra branches to the CalibrationNtuple, and their locations in StoreGate)
    ~MuonCalibExtraTreeAlg(); //!< Athena Algorithm destructor
    StatusCode initialize();  //!< Algorithm initialize: retrieve StoreGateSvc, MuonHolesOnTrackTool and IdentifierConverter. Retrieves the ntuple to write in.
    StatusCode execute();     //!< Algorithm execute, called once per event: resets branches, retrieves the PhiPattern and Track Collections. Finally, it writes the Tree.
    StatusCode finalize();    //!< Algorithm finalize: resets all Branches.

  private:
    bool  retrievePatterns();              //!< retrieves PhiPatterns from StoreGate
    bool  handlePatterns();                //!< feeds PhiHits to MuonCalibPhiHit_EBranch   
    void  finishEvent();                   //!< resets all branches
    bool  createPhiHitBranch(TTree* tree); //!< creates MuonCalibPhiHit_EBranc
    bool  createHitBranch(TTree* tree);    //!< creates MuonCalibHit_EBranch
    bool  createTrackBranch(TTree* tree);  //!< creates hit overlap between two tracks

    const MuonGM::MuonDetectorManager*      m_detMgr;      //<! MuonDetector manager
    const MdtIdHelper*                      m_muonIdHelper;//<! //  const MuonPrdPatternCollection* m_patterns;    //!< Athena pattern Collection
    const MuonPatternCombinationCollection* m_patterns;    //!< Athena pattern Collection

    bool        m_doPhi;                         //!< property flagging wether to store PhiHits on ExtraTree
    std::string m_ntupleName;                    //!< Name of the Ntuple
    std::string m_patternLocation;               //!< Location of the Phi or Eta-Phi Patterns within StoreGate

    bool m_delayFinish;                          //!< property to fill the CalibrationNtuple later in MuonCalibExtraTreeTriggerAlg 
    ToolHandleArray<IExtraTreeFillerTool> m_track_fillers;
    ToolHandle<IIdToFixedIdTool> m_idToFixedIdTool;
    ToolHandle<ISegmentOnTrackSelector> m_segmentOnTrackSelector;
    TDirectory* m_dir;
    TTree*      m_tree; 

    MuonCalibPhiHit_EBranch     m_phiHitBranch;     
    MuonCalibPhiPattern_EBranch m_phiPatternBranch;
    MuonCalibTrack_EBranch      m_trackBranch;
    MuonCalibHit_EBranch        m_hitBranch;
    MuonCalibTrackSegmentBranch m_trackSegmentBranch;
    bool m_init;
  };

}//namespace MuonCalib

#endif //MUONCALIB_EXTRATREEALG_H
