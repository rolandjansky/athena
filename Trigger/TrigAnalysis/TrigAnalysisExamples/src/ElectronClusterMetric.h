/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRON_CLUSTER_METRIC
#define ELECTRON_CLUSTER_METRIC

// EDM include(s):
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODEgamma/Electron.h"
//#include "TrigObjectMatching/DistanceFunctor.h"

/**
 * @brief The ElectronClusterMetric is a brief example how to
 *        define a custom metric for performing trigger object 
 *        matching.  
 *
 *        In this case, we determine the distance between a
 *        TrigElectronContainer and a reconstructed electron
 *        as the smallest DeltaR between the cluster associated
 *        with the electron and the clusters associated
 *        with the trigger electrons.  Please see 
 *        ElectronClusterMetric.cxx for more information
 *
 * @author Ben Smith <bcsmith@fas.harvard.edu> - Harvard University
 *
 */


class ElectronClusterMetric {
    public:
        ElectronClusterMetric() {};
        virtual ~ElectronClusterMetric() {};
    
    private:
        virtual float calculateDistance(const xAOD::TrigElectron *t, const xAOD::Electron *u) const;
};

/*class ElectronClusterMetric : 
  public DistanceFunctor<xAOD::TrigElectron, xAOD::Electron> {

  public:

    // note that if the metric needed access to private data
    // (like the trig decision tool, or an eta cut, etc.) it
    // can be passed via the constructor like any other object
    ElectronClusterMetric() {};
    virtual ~ElectronClusterMetric() {};

  private:

    // this is the only function that you need to implement. The base
    // class takes care of everything else, including reflexivity.
    virtual float calculateDistance(const xAOD::TrigElectron *t, const xAOD::Electron *u) const;

};*/

#endif
