/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_SHOWERSHAPESPLOTS_H
#define EGAMMAPHYSVALMONITORING_SHOWERSHAPESPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEventInfo/EventInfo.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace Egamma{
  
class ShowerShapesPlots:public PlotBase {
    public:
      ShowerShapesPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
      void fill(const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo) const;
      
      std::string m_sParticleType;

      TH1* hadleak;
      TH1* middlee;
      TH1* reta37;
      TH1* reta33;
      TH1* weta1;
      TH1* weta2;
      TH1* demax1;
      TH1* demax2;
      TH1* de;
      TH1* fracs1;
      TH1* wtots1;
      TH1* f1;
      TH1* f3;
      TH1* pos7;
      TH1* clusiso;
      TH1* Eratio;
      TH1* E0;
      TH1* E1;
      TH1* E2;
      TH1* E3;
      TH1* Rhad;
      TH1* Rhad1;
      TH1* Reta;
      TH1* Rphi;

      TH2* hadleakvset;
      TH2* middleevset;
      TH2* reta37vset;
      TH2* reta33vset;
      TH2* weta1vset;
      TH2* weta2vset;
      TH2* demax1vset;
      TH2* demax2vset;
      TH2* devset;
      TH2* fracs1vset;
      TH2* wtots1vset;
      TH2* f1vset;
      TH2* f3vset;
      TH2* pos7vset;
      TH2* clusisovset;
      TH2* Eratiovset;
      TH2* Rhadvset;
      TH2* Rhad1vset;
      TH2* Retavset;
      TH2* Rphivset;

      TH2* hadleakvseta;
      TH2* middleevseta;
      TH2* reta37vseta;
      TH2* reta33vseta;
      TH2* weta1vseta;
      TH2* weta2vseta;
      TH2* demax1vseta;
      TH2* demax2vseta;
      TH2* devseta;
      TH2* fracs1vseta;
      TH2* wtots1vseta;
      TH2* f1vseta;
      TH2* f3vseta;
      TH2* pos7vseta;
      TH2* clusisovseta;
      TH2* Eratiovseta;
      TH2* Rhadvseta;
      TH2* Rhad1vseta;
      TH2* Retavseta;
      TH2* Rphivseta;

    private:
      virtual void initializePlots();
      
};

}

#endif
