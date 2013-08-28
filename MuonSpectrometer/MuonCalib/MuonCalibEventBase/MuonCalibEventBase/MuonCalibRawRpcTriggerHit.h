/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Rpc TRIGGER hits                     //
// Author: Giuseppe Salamanna                              //
// Date  : 18th february 2009                              //
//*********************************************************//

#ifndef MUONCALIBRAWRPCTRIGGERHIT_H
#define MUONCALIBRAWRPCTRIGGERHIT_H
// std
#include <iostream>
#include <iomanip>
#include <string>


namespace MuonCalib {

  /**
     @class MuonCalibRawRpcTriggerHit
     Calib-EDM version of RpcRawData class
   */

  class MuonCalibRawRpcTriggerHit {
    public:
    MuonCalibRawRpcTriggerHit() {}; //!< default constructor
    MuonCalibRawRpcTriggerHit( int sector,
			       int padId,
			       int status,
			       int errorCode,
			       int cmaId,
			       int fel1Id,
			       int febcId,
			       int crc,
			       int bcId,
			       int time,
			       int ijk,
			       int channel,
			       int overlap,
			       int threshold ); //!< constructor setting all attributes 
    MuonCalibRawRpcTriggerHit( const MuonCalibRawRpcTriggerHit& hit ) ; //!< copy constructor
    ~MuonCalibRawRpcTriggerHit() {}; //!< destructor
    
    MuonCalibRawRpcTriggerHit& operator=( const MuonCalibRawRpcTriggerHit& rhs ) ; //!< assignment operator
    
    std::ostream& dump( std::ostream& stream) const; //!< dump to be used for operator<<() to dump the MuonCalibRawRpcTriggerHit

    int sector() const { return m_sector; };           // pad sector Id
    int padId() const { return m_padId; };             // pad Id
    int status() const { return m_status; };           // status
    int errorCode() const { return m_errorCode; };     // error code
    int cmaId() const { return m_cmaId; };             // CMA Id
    int fel1Id() const { return m_fel1Id; };           // fel1Id
    int febcId() const { return m_febcId; };           // febcId
    int crc() const { return m_crc; };                 // crc
    int bcId() const { return m_bcId; };               // bc Id
    int time() const { return m_time; };               // time
    int ijk() const { return m_ijk; };                 // ijk code
    int channel() const { return m_channel; };         // fired channel
    int overlap() const { return m_overlap; };         // overlap
    int threshold() const { return m_threshold; };
    
  private:
    
    int m_sector;           
    int m_padId;            
    int m_status;           
    int m_errorCode;        
    int m_cmaId;            
    int m_fel1Id;           
    int m_febcId;           
    int m_crc;              
    int m_bcId;             
    int m_time;             
    int m_ijk;              
    int m_channel;          
    int m_overlap;          
    int m_threshold;
    
  };
  
} // namespace MuonCalib

std::ostream& operator << (std::ostream& stream, const MuonCalib::MuonCalibRawRpcTriggerHit& rawHit);

#endif // MuonCalibRawRpcTriggerHit_h
