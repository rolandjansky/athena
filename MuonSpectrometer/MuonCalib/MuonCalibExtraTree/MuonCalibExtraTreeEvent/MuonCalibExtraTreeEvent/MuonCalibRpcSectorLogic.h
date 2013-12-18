/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBRPCSECTORLOGIC_H
#define MUONCALIBRPCSECTORLOGIC_H

#include <string>
#include <vector>

namespace MuonCalib{

  /**
     Simplified class designed to store information from RpcSectorLogic. It has :
     
     @author Giuseppe Salamanna
  */
  class MuonCalibRpcSectorLogic{
  public:
    MuonCalibRpcSectorLogic();                                                       
    MuonCalibRpcSectorLogic( int sectorId, int fel1Id, int bcid, int errorCode, int crc, int hasMoreThan2TriggerCand, int slIndex, float triggerRates, int counters, int slIndex2, int rowinBcid, int padid, int ptid, int roi, int outerPlane, int overlapPhi, int overlapEta, int triggerBcid, int isInput ); //!< constructor setting all members
    ~MuonCalibRpcSectorLogic() {} ;                                                                          //!< destructor
    
    int sectorId()  const      { return m_sectorId; }
    int fel1Id()  const      { return m_fel1Id; }
    int bcid()  const      { return m_bcid; }
    int errorCode()  const      { return m_errorCode; }
    int crc()  const      { return m_crc; }
    int hasMoreThan2TriggerCand()  const      { return m_hasMoreThan2TriggerCand; }
    int slIndex()  const      { return m_slIndex; };
    float triggerRates()   const      { return m_triggerRates; };
    int counters()   const  {return m_counters; };
    int slIndex2()  const      { return m_slIndex2; }  
    int rowinBcid()  const      { return m_rowinBcid; }
    int padid()  const      { return m_padid; }
    int ptid()  const      { return m_ptid; }
    int roi()  const      { return m_roi; }
    int outerPlane()  const      { return m_outerPlane; }
    int overlapPhi()  const      { return m_overlapPhi; }
    int overlapEta()  const      { return m_overlapEta; }
    int triggerBcid()  const      { return m_triggerBcid; }
    int isInput()  const      { return m_isInput; }
    
  private:

    int m_sectorId;
    int m_fel1Id;
    int m_bcid;
    int m_errorCode;
    int m_crc;
    int m_hasMoreThan2TriggerCand;
    int m_slIndex;
    float m_triggerRates;
    int m_counters; 
    int m_slIndex2;  
    int m_rowinBcid;
    int m_padid;
    int m_ptid;
    int m_roi;
    int m_outerPlane;
    int m_overlapPhi;
    int m_overlapEta;
    int m_triggerBcid;
    int m_isInput;
    
  } ;

}//namespace MuonCalib

#endif 
