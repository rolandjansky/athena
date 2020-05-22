/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PatternNtupleMaker.h.h
//   Header file for class PatternNtupleMaker.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_PATTERNNTUPLEMAKER_H
#define MUONCALIB_PATTERNNTUPLEMAKER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonCalibITools/IMuonCalibTool.h"

#include <string>
#include <map>

namespace MuonCalib {
  class MuonCalibEvent;
  class MuonFixedId;
  class MuonCalibBranchNtuple;
  class MuonTruthBranchNtuple;
  class MuonCalibTruthCollection;
  class MuonCalibEventInfo;
  class MuonCalibTriggerTimeInfo;
  class MuonCalibRawHitCollection;
  class MuonCalibPattern;

  /**@class PatternNtupleMaker.h
     Creates and fills ntuple with MuonCalibEvents
      
     @author Niels.Van.Eldik@cern.ch, 
  */

  class PatternNtupleMaker : public AthAlgTool, virtual public IMuonCalibTool {
  public:
    PatternNtupleMaker(const std::string&,const std::string&,const IInterface*); //!<AlgTool constructor 
    virtual ~PatternNtupleMaker()=default;

    StatusCode initialize();  //!< AlgTool initailize method: Creates an instance of MuonCalib::MuonCalibBranchNtuple, retrieves Storegate.
    StatusCode finalize();    //!< AlgTool finalize method: does not do anything special 
    
    virtual bool handleEvent( const MuonCalibEvent* event );   //!< Interface method: feeds the several constituents of the MuonCalibEvent to the MuonCalibBranchNtuple.
    virtual bool analyse();   //!< Interface method: writes the Tree.

  private:
    bool handleEventInfo( const MuonCalibEventInfo* eventInfo, const MuonCalibTriggerTimeInfo* trigTimeInfo);  //!< Feeds MuonCalibEventInfo to muonCalibBranchNtuple
    bool handleTruthCollection( const MuonCalibTruthCollection* truthCollection );  //!< Feeds MuonCalibTruthCollection to muonCalibBranchNtuple
    bool handlePattern( const MuonCalibPattern* pattern );                          //!< Feeds MuonCalibPattern to muonCalibBranchNtuple
    bool handleRawHits( const MuonCalibRawHitCollection* rawHits );                 //!< Feeds MuonCalibRawHitCollection to muonCalibBranchNtuple

  
    std::string m_ntupleName;         //!< ntuple name
    std::string m_eventTag;           //!< tag of the event, describing reconstruction mechanism 
    int m_printLevel;                 //!< print level of ntuple maker 
    bool m_delayFinish;               //!< property flag used to delay the writing of the CalibrationNtuple when a MuonCalibExtraTree is created in the same Tree.
    MuonCalibBranchNtuple* m_ntMaker; //!< pointer to the MuonCalibBranchNtuple 
    bool m_fillTruth;
    bool m_doRawTgc;
  };
  
}

#endif
