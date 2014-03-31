///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileRawChannel_p1.h 
// Header file for class TileRawChannel_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILERAWCHANNEL_P1_H
#define TILETPCNV_TILERAWCHANNEL_P1_H

#include <vector>

// forward declarations
class TileRawChannelCnv_p1;

class TileRawChannel_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class TileRawChannelCnv_p1;

public:

  /** Default constructor: 
   */
  TileRawChannel_p1() : m_channelID(0), m_length(0), m_data() {}

private:

  unsigned int m_channelID;
  unsigned int m_length;
  std::vector<float> m_data;
};

#endif //> TILETPCNV_TILERAWCHANNEL_P1_H

