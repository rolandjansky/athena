/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SLTRUEINFO
#define JETTAGINFO_SLTRUEINFO

#include "EventPrimitives/EventPrimitives.h"
#include <iostream>

class MsgStream;

namespace Analysis {

  class SLTrueInfo {
    public:
      SLTrueInfo();
      SLTrueInfo(int, int, int, bool, bool, bool, const Eigen::Vector3d&, const Eigen::Vector3d&);
      inline int  barcode()         const { return m_barcode; }
      inline int  pdgId()           const { return m_pdgCode; } 
      inline int  pdgIdMother()     const { return m_pdgCodeMother; }
      inline bool FromB()           const { return m_isFromBhadron; }
      inline bool FromD()           const { return m_isFromDhadron; }
      inline bool FromGH()          const { return m_isFromGHboson; }
      inline Eigen::Vector3d momentum()  const { return m_Momentum; }
      inline Eigen::Vector3d prodvtx()   const { return m_ProductionVertex; }
    private:
      int        m_barcode;
      int        m_pdgCode;
      int        m_pdgCodeMother;
      bool       m_isFromBhadron;
      bool       m_isFromDhadron;
      bool       m_isFromGHboson;
      Eigen::Vector3d m_Momentum;
      Eigen::Vector3d m_ProductionVertex;
  };

  MsgStream& operator<<(MsgStream& out, const SLTrueInfo&);
  std::ostream& operator<<(std::ostream& out, const SLTrueInfo&);

}
#endif // JETTAGINFO_SLTRUEINFO
