/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHI2LJETS_H_
#define CHI2LJETS_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

#include "TLorentzVector.h"

class TMinuit;

namespace top {
class Event;

/**
 * @brief Hold the information that is required for the chi^2 minimisation
 */
class FitInfo : public TObject {
public:
    ///Mass of the hadronic W (j+j)
    double hadronic_w_mass;

    ///Mass of the hadronic top quark (j+j+j)
    double hadronic_t_mass;

    ///Missing ET - px
    double neutrino_px;

    ///Missing ET - py
    double neutrino_py;

    ///Four-vector of the lepton (electron or muon)
    TLorentzVector lepton;

    ///Four vector of the jet that we think is the leptonic b quark
    TLorentzVector leptonic_b;
};

/**
 * @brief Not complete. Work in progress
 */
class Chi2LJets : public EventSelectorBase {
public:
    Chi2LJets();
    virtual ~Chi2LJets();

    virtual bool apply(const top::Event&) const override;

    std::string name() const { return "RECO:CHI2LJETS"; }

private:
    ///So we can do a minimisation
    TMinuit* min;

    mutable FitInfo params;

//so the fcn function can see them
public:
    ///The top mass that we're using as the baseline - used in chi2 function.
    static const double input_topmass;

    ///The W mass that we're using as the baseline - used in chi2 function.
    static const double input_wmass;

    ///The top resolution that we're using as the baseline - used in chi2 function.
    static const double input_topsigma;

    ///The W resolution that we're using as the baseline - used in chi2 function.
    static const double input_wsigma;
};

}

void fcn(int& npar, double* deriv, double& result, double par[], int flag);

#endif
