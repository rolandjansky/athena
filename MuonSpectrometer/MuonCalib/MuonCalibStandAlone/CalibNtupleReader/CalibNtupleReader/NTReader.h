/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to read in Calibration Ntuples, giving access to  //
// calibration-specific classes stored.                    //
// Author: Zdenko van Kesteren                             //
// Date  : 18 October 2005                                 //
//*********************************************************//

#ifndef NTReader_h
#define NTReader_h

#include "CalibNtupleReader/NTReaderBase.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

#include <vector>
#include <string>

namespace MuonCalib{
  
  class MuonCalibEventInfo;
  class MuonCalibTriggerTimeInfo;
  class MuonCalibPattern;
  class MuonCalibSegment;
  class CscCalibHitBase;
  class MdtCalibHitBase;
  class RpcCalibHitBase;
  class TgcCalibHitBase;
  class MuonCalibTruthCollection;
  class MuonCalibTruth;
  class MuonCalibMdtTruthHit;
  class MuonCalibRpcTruthHit;
  class MuonCalibTgcTruthHit;
  class MuonCalibCscTruthHit;
  class MuonCalibRawHitCollection;
  class MuonCalibRawTriggerHitCollection;
  class MuonCalibRawMdtHit;
  class MuonCalibRawRpcHit;
  class MuonCalibRawTgcHit;
  class MuonCalibRawTgcCoin;
  class MuonCalibRawCscHit;
  class MuonCalibRawRpcTriggerHit;
  
  /**
     @class NTReader
     class storing the variables of the CalibrationNtuple into
     Calib-EDM classes (as described in MuonCalib::MuonCalibEvent)
     @author Zdenko.Van.Kesteren@cern.ch
  */

class NTReader : virtual public NTReaderBase {
  public:
  NTReader( TTree *tree=0 ) : NTReaderBase( tree ),m_event(0) {;};  //!< default constructor
    virtual ~NTReader();                                            //!< virtual destructor, clearing the MuonCalibEvent
    
    const MuonCalibEvent& getEvent( int i_event );                  //!< retrieve a fully constructed MuonCalibEvent
    const MuonCalibEvent& getEvent_light( int i_event );            //!< retrieve a lightweight MuonCalibEvent (only MuonCalibPattern)
    int                   nEvents();                                //!< returns the number of events in the CalibrationNtuple
      
  protected:
    MuonCalibTruthCollection*      buildTruthColl();                //!< create a MuonCalibTruthCollection 
    MuonCalibRawHitCollection*     buildRawColl();                  //!< create a MuonCalibRawHitCollection
    MuonCalibRawTriggerHitCollection* buildRawTriggerColl();        //!< create a MuonCalibRawTriggerHitCollection
    const MuonCalibEventInfo       buildEventInfo();                //!< create a MuonCalibEventInfo
    MuonCalibTriggerTimeInfo*      buildTriggerTimeInfo();          //!< create a MuonCalibEventInfo
    MuonCalibEvent::MCPVec         buildPatternVec();               //!< create a MuonCalibPatternVec

    MuonCalibEvent*                m_event;                         //!< instance of a MuonCalibEvent
    std::vector<const MuonCalibSegment *> m_ordered_segments;       //!< segments ordered by appearence in ntuple
  private:
    void                     clearEvent();                          //!< deletes m_event

    MuonCalibPattern*        getPattern( int i_pattern );           //!< create the i_pattern^th MuonCalibPattern of the entry
    MuonCalibSegment*        getSegment( int i_segment );           //!< create the i_segment^th MuonCalibSegment of the entry
    MdtCalibHitBase*         getMdtHit( int i_hit );                //!< create the i_hit^th MdtCalibHitBase of the entry
    CscCalibHitBase*         getCscHit( int i_hit );                //!< create the i_hit^th CscCalibHitBase of the entry
    RpcCalibHitBase*         getRpcHit( int i_hit );                //!< create the i_hit^th RpcCalibHitBase of the entry
    RpcCalibHitBase*         getRpcOsHit( int i_hit );              //!< create the i_hit^th RpcCalibHitBase of the entry
    TgcCalibHitBase*         getTgcHit( int i_hit );                //!< create the i_hit^th TgcCalibHitBase of the entry
    MuonCalibTruth*          getTruth( int i_truth );               //!< create the i_truth^th MuonCalibTruth of the entry
    MuonCalibMdtTruthHit*    getMdtTruth( int i_truth );            //!< create the i_truth^th MuonCalibMdtTruthHit of the entry
    MuonCalibRpcTruthHit*    getRpcTruth( int i_truth );            //!< create the i_truth^th MuonCalibRpcTruthHit of the entry
    MuonCalibTgcTruthHit*    getTgcTruth( int i_truth );            //!< create the i_truth^th MuonCalibTgcTruthHit of the entry
    MuonCalibCscTruthHit*    getCscTruth( int i_truth );            //!< create the i_truth^th MuonCalibCscTruthHit of the entry
    MuonCalibRawMdtHit*      getMuonCalibRawMdtHit( int i_hit );    //!< create the i_hit^th MuonCalibRawMdtHit of the entry
    MuonCalibRawRpcHit*      getMuonCalibRawRpcHit( int i_hit );    //!< create the i_hit^th MuonCalibRawRpcHit of the entry
    MuonCalibRawTgcHit*      getMuonCalibRawTgcHit( int i_hit );    //!< create the i_hit^th MuonCalibRawTgcHit of the entry
    MuonCalibRawTgcCoin*     getMuonCalibRawTgcCoin( int i_hit );   //!< create the i_hit^th MuonCalibRawTgcCoin of the entry
    MuonCalibRawCscHit*      getMuonCalibRawCscHit( int i_hit );    //!< create the i_hit^th MuonCalibRawCscHit of the entry
    MuonCalibRawRpcTriggerHit*  getMuonCalibRawRpcTriggerHit( int i_hit );     //!< create the i_hit^th MuonCalibRawRpcHit of the entry
};
 
 inline int NTReader::nEvents() { return fChain->GetEntries(); }

}// namespace MuonCalib

#endif //NTReader_h
