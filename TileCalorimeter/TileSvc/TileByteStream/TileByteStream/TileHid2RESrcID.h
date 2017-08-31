/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TILEHID2RESRCID__
#define __TILEHID2RESRCID__

#include "eformat/FullEventFragment.h"

class TileHWID;
class HWIdentifier;
class MsgStream;

#include <map>
#include <vector>
#include <string>
#include <stdint.h> 

/**
 * @class TileHid2RESrcID
 * @brief This class provides conversion between fragment ID and RESrcID.
 * @author Alexander Solodkov
 *
 * Fragment ID is the unique ID for every super-drawer.
 * RESrcID is used for identifying each ROD. 
 */ 

class TileHid2RESrcID {

public:

  typedef int COLLECTION_ID;
  
  /** constrcutor 
  */ 
  TileHid2RESrcID(const TileHWID* tileHWID=0, uint32_t runnum=0); 
  void setTileHWID (const TileHWID* tileHWID, uint32_t runnum=0);
  void setTileMuRcvHWID(const TileHWID* tileHWID, uint32_t runnum=0);

  void initialize(uint32_t runnum);
  void initializeMuRcv(uint32_t runnum);

  void setROD2ROBmap (const std::vector<std::string> & ROD2ROB,
                      MsgStream & log);

  void setROD2ROBmap (const eformat::FullEventFragment<const uint32_t*> * event,
                      MsgStream & log);

  /** make a ROB SrcID for a fragment ID
  */ 
  uint32_t getRobFromFragID  (int frag_id) const;
  uint32_t getRobFromTileMuRcvFragID (int frag_id) const;

  /** make a ROD SrcID for a fragment ID
  */ 
  uint32_t getRodID  (int frag_id) const;
  uint32_t getRodTileMuRcvID (int frag_id) const;

  /** Make a ROB Source ID from a ROD source ID
  */ 
  uint32_t getRobID  ( uint32_t rod_id) const; 
  uint32_t getRobID_8  ( uint32_t rod_id) const;

  /** Make a ROS Source ID from a ROB source ID
  */ 
  uint32_t getRosID  ( uint32_t rob_id) const; 
  uint32_t getRosID_8  ( uint32_t rob_id) const;

  /** Make a SubDetector ID from ROS source ID 
  */
  uint32_t getDetID  ( uint32_t ros_id) const;

  /** Retrieve run number for which hash was initialized
  */
  uint32_t getRunNum  () { return m_runnum; };

private: 

  const TileHWID* m_tileHWID;
  typedef std::map<int, uint32_t> FRAGRODMAP; 
  FRAGRODMAP m_frag2ROD, m_TileMuRcvFrag2ROD;
  uint32_t m_runnum, m_TileMuRcvRunnum;
};

#endif 
