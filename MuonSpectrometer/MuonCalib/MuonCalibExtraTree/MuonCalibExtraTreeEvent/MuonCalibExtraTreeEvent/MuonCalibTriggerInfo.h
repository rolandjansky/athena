/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBTRIGGERINFO_H
#define MUONCALIBTRIGGERINFO_H

#include "CxxUtils/checker_macros.h"

#include <string>
#include <vector>

namespace MuonCalib{

  /** enum for all trigger type */
  enum MuonCalibTriggerType {
    L1_EM3 = 0,
    L1_TAU5,
    L1_J5,
    L1_XE20,
    L1_MBTS_1_COMM,
    L1_MBTS_2_COMM, 
    L1_MU0_LOW_RPC,  
    L1_MU6_RPC,
    L1_MU0_HIGH_RPC, 
    L1_MU0_TGC_HALO,
    L1_MU0_TGC,
    L1_MU6_TGC,
    NUMBEROFKNOWNTRIGGERS,
    UNKNOWNTRIGGERTYPE = 999
  };

  /** helper class to for MuonCalibTriggerType which provides a mapping to the actual bit number */
  class ATLAS_NOT_THREAD_SAFE MuonCalibTriggerTypeHelper {
  public:
    /** get trigger bit for a given type */
    static std::vector<int> triggerBits( MuonCalibTriggerType type );

    /** get trigger bit for a given type */
    static std::string itemName( MuonCalibTriggerType type );

    /** check whether type is valid */
    static bool validType( MuonCalibTriggerType type );

    /** check whether type is valid */
    static bool isRpcTrigger( MuonCalibTriggerType type );

    /** check whether type is valid */
    static bool isTgcTrigger( MuonCalibTriggerType type );

    /** check whether type is valid */
    static bool isMbtsTrigger( MuonCalibTriggerType type );

    /** check whether type is valid */
    static bool isLVL1CaloTrigger( MuonCalibTriggerType type );
    
    /** dump mapping */
    static void dumpMapping();

  private:
    static void addEntry( MuonCalibTriggerType type, int bit, std::string name );
    static void addEntry( MuonCalibTriggerType type, std::vector<int>& bits, std::string name );
    static void initMap();
    static std::vector< std::pair< std::vector<int>,std::string> > m_typeToBitAndStringMapping; 
  };
  
  /**
     Simplified class designed to store information of a trigger. It has :
     - a trigger type 
     - bunch crossing offset
     
     @author Niels van Eldik
  */
  class ATLAS_NOT_THREAD_SAFE MuonCalibTriggerInfo{
  public:
    MuonCalibTriggerInfo();                                                                               //!< default constructor
    MuonCalibTriggerInfo( MuonCalibTriggerType type, double bcOffset );
    MuonCalibTriggerInfo( int tbpbit, int tapbit, int tavbit, int bcIndex );
    ~MuonCalibTriggerInfo() {} ;                                                                          //!< destructor
    
    /**  return the trigger type  */
    MuonCalibTriggerType  type()       const { return m_type; }
    
    /** return the trigger bit before prescale */
    int                tbp()      const { return m_tbpbit; }    

    /** return the trigger bit after prescale */
    int                tap()      const { return m_tapbit; }    

    /** return the trigger bit after veto */
    int                tav()      const { return m_tavbit; }    

    /** return the delay of the signal wrt firing trigger */
    double             delay()      const { return m_delay; }    
    
    /** return the bcIndex of the trigger */
    int                bcIndex()      const { return m_bcIndex; }    

    /** return the trigger bits for this type  */
    std::vector<int> triggerBits() const { return MuonCalibTriggerTypeHelper::triggerBits(m_type); } 

    /** return string representing type  */
    std::string typeString() const { return MuonCalibTriggerTypeHelper::itemName(m_type); } 

  private:
    MuonCalibTriggerType        m_type;       //!< type
    int                         m_tbpbit;     //!< trig bit before pre-scale
    int                         m_tapbit;     //!< trig bit after  pre-scale
    int                         m_tavbit;     //!< trig bit after  veto
    double                      m_delay;      //!< delay of signal wrt firing trigger
    int                         m_bcIndex;    //!< trig bc Index
  } ;

}//namespace MuonCalib

#endif 
