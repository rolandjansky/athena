/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SMTRACKINFO
#define JETTAGINFO_SMTRACKINFO

#include "AthLinks/ElementLink.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include <iostream>

class MsgStream;

namespace Analysis {

  class SMTrackInfo {
    public:
      SMTrackInfo();
      SMTrackInfo(const MuonContainer* coll, 
                  const Muon* mu,
                  double d0val,
                  double pTrel,
		  const std::vector<double>& tagLikelihood);
      SMTrackInfo(const ElementLink<MuonContainer>& muonLink,
                  double d0val,
                  double pTrel,
		  std::vector<double>&& tagLikelihood);
      virtual ~SMTrackInfo();
      const Muon* muon() const;
      inline const ElementLink<MuonContainer>& muonLink() const { return m_muon; }
      inline double d0Value() const { return m_valD0wrtPV; } 
      inline double pTrel() const { return m_pTrel; } 
      inline const std::vector<double>& tagLikelihood() const { return m_tagLikelihood; }
    private:
      ElementLink<MuonContainer> m_muon;
      double m_valD0wrtPV;
      double m_pTrel;
      std::vector<double> m_tagLikelihood; //!< vector to hold the taglikelihood (signal plus N background)
  };

  MsgStream& operator<<(MsgStream& out, const SMTrackInfo&);
  std::ostream& operator<<(std::ostream& out, const SMTrackInfo&);

}
#endif // JETTAGINFO_SMTRACKINFO
