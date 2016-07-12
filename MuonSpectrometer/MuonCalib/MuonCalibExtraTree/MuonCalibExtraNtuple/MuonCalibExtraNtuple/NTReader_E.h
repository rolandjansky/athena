/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NTReader_E_h
#define NTReader_E_h

#include "CalibNtupleReader/NTReader.h"
#include "MuonCalibExtraNtuple/NTReaderBase_E.h"

#include <vector>
#include <list>
#include <map>

namespace MuonCalib{
  class MuonCalibHit_E;
  class MuonCalibCaloHit;
  class MuonCalibTriggerInfo;
  class MuonCalibMUCTPI;
  class MuonCalibRpcSectorLogic;
  class MuonCalibRawRpcCoin;
  class MuonCalibHole_E;
  class MuonCalibTrack_E;
  class MuonCalibEvent_E;
    /**
     @class NTReader_E

     class storing the variables of the Extended CalibrationNtuple
     into the 'extended Calib-EDM' classes as described in MuonCalibExtraTreeEvent.

     @author Zdenko.Van.Kesteren@cern.ch
  */
  class NTReader_E : public NTReaderBase_E, public NTReader {
  public:
    NTReader_E( TTree *tree=0 ) : NTReaderBase(tree), NTReaderBase_E( tree ), NTReader( tree ), m_evente(0), m_debug(false) {}; //!< default constructor
    virtual ~NTReader_E(); //!< virtual destructor, clearing the MuonCalib::MuonCalibEvent_E

    const MuonCalibEvent_E&  getEvent( int i_event );   //!< retrieve a fully constructed MuonCalib::MuonCalibEvent_E
    MuonCalibTrack_E*        getPhiPattern( int i_pat ); //!< create a PhiPattern (of type MuonCalib::MuonCalibTrack_E) 
    MuonCalibTrack_E*        getTrack( int i_trk );      //!< create a Track (of type MuonCalib::MuonCalibTrack_E) 
    MuonCalibTriggerInfo*    getCtpTbpHit( int i_trg);   //!< create a TriggerBeforePrescale object (oftype MuonCalib::MuonCalibTriggerInfo) 
    MuonCalibTriggerInfo*    getCtpHit( int i_trg);      //!< create a Trigger object (oftype MuonCalib::MuonCalibTriggerInfo) 
    MuonCalibMUCTPI*         getMUCTPIHit( int i_muctpi);//!< create a MUCTPI object (oftype MuonCalib::MuonCalibMUCTPI) 
    MuonCalibRpcSectorLogic* getRpcSLHit( int i_rpcsl);  //!< create an RpcSL object (oftype MuonCalib::MuonCalibRpcSectorLogic) 
    MuonCalibRawRpcCoin*     getRpcCoinHit( int i_rpcco);//!< create an RpcCoin object (oftype MuonCalib::MuonCalibRawRpcCoin) 

    MuonCalibCaloHit*        getMBTSHit( int i_trk );    //!< create a MBTS hit  
    MuonCalibCaloHit*        getL1CaloHit( int i_trk );  //!< create Calo hit  

    MuonCalibHit_E*          getPhiHit( int i_phi );     //!< create the i_phi^th PhiHit (of type MuonCalib::MuonCalibHit_E) of the entry
    MuonCalibHit_E*          getTrackHit( int i_trkHit );//!< create the i_trkHit^th Track hit (of type MuonCalib::MuonCalibHit_E) of the entry
    MuonCalibHole_E*         getTrackHole( int i_trkHole );//!< create the i_trkHole^th MuonCalib::MuonCalibHole_E of the entry
    std::map<int, std::list<int> > m_smap;
    void buildSegmentOnTrackMap();
    void setdbgcout(const bool dbglvl);
   
  private:
    MuonCalibEvent_E *m_evente; //!< NTReader has a MuonCalibEvent *m_event, but is not used here
    void clearEvent_E();        //!< deletes m_evente 
    bool m_debug;
  };

}//namespace MuonCalib
#endif
