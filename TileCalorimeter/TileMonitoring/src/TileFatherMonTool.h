/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileFatherMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//
// July 2006	
// ********************************************************************
#ifndef TILEMONITORING_TILEFATHERMONTOOL_H
#define TILEMONITORING_TILEFATHERMONTOOL_H

#include "TilePaterMonTool.h"

class CaloCell;
class TriggerInfo;

namespace Trig {
  class TrigDecisionTool;
}

/** @class TileFatherMonTool
 *  @brief Base class for TileCal online monitoring tools in physics runs
 */ 

class TileFatherMonTool: public TilePaterMonTool
{

 public:
  
  TileFatherMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TileFatherMonTool();

  StatusCode initialize();
    
  int getPartition( const CaloCell* cell );

  void get_eventTrigs(uint32_t lvl1info);

  //method to set an histogram axis bin labels according to a vector of strings
  template<class T>
  void SetBinLabel(T* axis, const std::vector<std::string>& labels); 

  //method to set an histogram axis bin labels according to an array of strings
  template<class T>
  void SetBinLabel(T* axis, const std::string* labels, int nLabels);

  // Method to move bins of TProfile histogra,
  // Used for online to display only latest 100 LB
  void ShiftTprofile(TProfile *, int);

  /// Method that returns the histogram index corresponding 
  /// to trigger 'i'
  inline int vecIndx(unsigned int i) { 
    return m_activeTrigs[ m_eventTrigs[i] ]; 
  }


/// Method to get the Level1 Trigger word: 
/// 32bit long, but only 8 bit are used 
/// If no trigger word or empty it will return 0
  inline uint32_t getL1info() { return m_lvl1info;};

/// Method to get the event number: 
  inline uint32_t getEvtNum() { return m_evtNum;};

/// Method to get the lumi_block word: 
  inline uint32_t getLumiBlock() { return m_lumiBlock;};

/// Method to get the event BCID: 
  inline uint32_t getEvtBCID() { return m_evtBCID;};

/// Method to get the run Number: 
  inline uint32_t getRunNum() { return m_runNum;};

/// Method to get the run Number in words: 
  std::string getRunNumStr();

/// Method to get the tile Flag: 
  inline uint32_t gettileFlag() { return m_tileFlag;};

/// Method to get the tile Error: 
  inline uint32_t gettileError_error() { return m_tileError_error;};

/// Method to fill the lvl1Info variable,
/// evtNumber, evtBCID and lumi_block
  void fillEvtInfo();

  /// enum defining TileCal cell samples
  enum Sampling {
    SampA     =0,
    SampB     =1,
    SampD     =2,
    SampE     =3,
    AllSamp   =4,
    TotalSamp =5
  };

  /// enum defining TileCal partitions
  enum Partitions {
    PartEBA    = 0,
    PartLBA    = 1,
    PartLBC    = 2,
    PartEBC    = 3,
    NumPart    = 4,
    NPartHisto = 5
  };

  /// enum defining L1 trigger bits
  enum L1Trig {
    Trig_b0    = 0,
    Trig_b1    = 1,
    Trig_b2    = 2,
    Trig_b3    = 3,
    Trig_b4    = 4,
    Trig_b5    = 5,
    Trig_b6    = 6,
    Trig_b7    = 7,
    AnyTrig    = 8,
    NTrigHisto = 9
  };


  std::string m_SampStrNames[TotalSamp] ;
  std::string m_PartNames[NPartHisto] ;
  std::string m_GainNames[2] ;
  std::string m_TrigNames[NTrigHisto] ;

  // conversion from ROS index to partition index
  int m_ros2partition[NPartHisto];

  // conversion from partition index to ROS index
  int m_partition2ros[NPartHisto];

  // conversion from partition index to ROS index

/// this array doesn't simply say if the trigger has ever fired for this run, 
/// but it also contains the position of the trigger in the histogram vector. Non-active trigger have -1 value. 
/// 0 value is reserved for AnyTrig:   m_activeTrigs[AnyTrig] = 0 , set in bookHistogram
/// Example: if  m_activeTrigs[4] is 3. It means that trigger bit4 histograms can be accessed by
/// vec_of_histos.at(  m_activeTrigs[4] ) or  vec_of_histos.at(  3 ) or better
/// vec_of_histos.at(  m_activeTrigs[Trig_b4] )
  int m_activeTrigs[9]; 

  uint32_t m_lvl1info;
  uint32_t m_evtNum;
  uint32_t m_lumiBlock;
  uint32_t m_evtBCID;
  uint32_t m_runNum;
  uint32_t m_tileFlag;
  uint32_t m_tileError_error;
  ToolHandle<Trig::TrigDecisionTool>  m_trigDec;

/// Vector containing the list of triggers passed by the event
  std::vector<int> m_eventTrigs;

  static bool  m_is_collision;
  static unsigned int m_lastevent;
  static float m_time_difference;

protected:

private:
  void checkIsCollision(); //moved to private. Should not be called from the derived classes.
  std::string m_MBTSCellContainerID;
  std::string m_cellsContainerID;
  std::vector<unsigned int> m_fillHistogramsForL1Triggers;
  std::vector<bool> m_allowedL1TriggerBits;
};

#endif // TILEMONITORING_TILEFATHERMONTOOL_H
