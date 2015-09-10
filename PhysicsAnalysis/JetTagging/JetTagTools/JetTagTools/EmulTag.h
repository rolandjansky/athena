/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_EMULTAG_H
#define JETTAGTOOLS_EMULTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/ITagTool.h"
#include <string>
#include <vector>

//class Jet;
class TH1;
class TH2;
namespace Trk  { class VxCandidate; }

namespace Analysis
{

  static const int m_etabin = 6;
  static const int m_ptbin  = 10;
  double m_ebin[m_etabin] = {0.0,0.5,1.0,1.5,2.0,2.5};
  double m_pbin[m_ptbin]  = {15.,30.,45.,60.,100.,140.,180.,220.,260.,7000.};

  class EmulTag : public AthAlgTool , virtual public ITagTool
  {
    public:
      EmulTag(const std::string&,const std::string&,const IInterface*);
      virtual ~EmulTag();
      StatusCode initialize();
      StatusCode finalize();
      
      void tagJet(xAOD::Jet& jetToTag);

      // Has to be there...
      void setOrigin(const Trk::VxCandidate*);

    private:      

      std::vector<std::string> m_algoType, m_ialgoType;
      unsigned int m_mode, m_imod, m_atlfBNSet;

      TH2 *m_RejLightP, *m_RejLightNP, *m_RejTau, *m_RejC, *m_EpsB;
      TH1 *m_Wl[2][3][m_etabin-1][m_ptbin-1];
      TH1 *m_Wb[m_etabin-1][m_ptbin-1];
      bool m_hWl[2][3][m_etabin-1][m_ptbin-1];
      bool m_hWb[m_etabin-1][m_ptbin-1];

      StatusCode initHistos(std::string);
      StatusCode initParam(unsigned int);
      void finalizeHistos();
      int findEt(double);
      int findEta(double);

  };
  inline void EmulTag::setOrigin(const Trk::VxCandidate* /* priVtx */) {}
}

#endif
