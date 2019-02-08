/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SETRACKINFO
#define JETTAGINFO_SETRACKINFO

#include "AthLinks/ElementLink.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/PhotonContainer.h"
#include <iostream>

class MsgStream;

namespace Analysis {

  class SETrackInfo {
    public:
      SETrackInfo();
      SETrackInfo(const ElectronContainer* coll, 
                  const Electron* el,
                  double d0val,
                  double pTrel,
		  const std::vector<double>& tagLikelihood);
      SETrackInfo(const PhotonContainer* coll, 
                  const Photon* el,
                  double d0val,
                  double pTrel,
		  const std::vector<double>& tagLikelihood);
      SETrackInfo(const ElementLink<ElectronContainer>& eleLink,
                  double d0val,
                  double pTrel,
		  std::vector<double>&& tagLikelihood);
      SETrackInfo(const ElementLink<PhotonContainer>& gamLink,
                  double d0val,
                  double pTrel,
		  std::vector<double>&& tagLikelihood);
      bool isPhoton() const;
      const egamma* electron() const;
      inline double d0Value() const { return m_valD0wrtPV; } 
      inline double pTrel() const { return m_pTrel; } 
      inline const std::vector<double>& tagLikelihood() const { return m_tagLikelihood; }
      inline const ElementLink<ElectronContainer>& electronLink() const { return m_electron; }
      inline const ElementLink<PhotonContainer>& photonLink() const { return m_photon; }
    private:
      ElementLink<ElectronContainer> m_electron;
      ElementLink<PhotonContainer> m_photon;
      double m_valD0wrtPV;
      double m_pTrel;
      std::vector<double> m_tagLikelihood; //!< vector to hold the taglikelihood (signal plus N background)
  };

  MsgStream& operator<<(MsgStream& out, const SETrackInfo&);
  std::ostream& operator<<(std::ostream& out, const SETrackInfo&);

}
#endif // JETTAGINFO_SETRACKINFO
