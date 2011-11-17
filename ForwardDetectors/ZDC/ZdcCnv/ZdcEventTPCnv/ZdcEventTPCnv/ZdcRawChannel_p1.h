///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcRawChannel_p1.h
// Header file for class ZdcRawChannel_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
///////////////////////////////////////////////////////////////////
#ifndef ZDCTPCNV_ZDCRAWCHANNEL_P1_H
#define ZDCTPCNV_ZDCRAWCHANNEL_P1_H

#include <vector>

// forward declarations
class ZdcRawChannelCnv_p1;

class ZdcRawChannel_p1 {

  // Make the AthenaPoolCnv class our friend
  friend class ZdcRawChannelCnv_p1;

public:

  /** Default constructor:
   */
  ZdcRawChannel_p1() {m_id=0;}

private:

  unsigned int m_id;
  std::vector<float> m_energy;
  std::vector<float> m_time;
  std::vector<float> m_chi;

};

#endif //> ZDCTPCNV_ZDCRAWCHANNEL_P1_H

