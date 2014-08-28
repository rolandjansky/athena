/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAUcaloIsolationTool.h, (c) ATLAS Detector software 2010
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_ICALOISOLATIONTOOL_H
#define PHOTONANALYSISUTILS_ICALOISOLATIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

class egamma;
namespace Analysis {
  class Electron;
  class Photon;
}
class TruthParticle;

static const InterfaceID IID_IPAUcaloIsolationTool("IPAUcaloIsolationTool", 1, 0);
        
class IPAUcaloIsolationTool : virtual public IAlgTool {

     
     public:
       /** Virtual destructor */
       virtual ~IPAUcaloIsolationTool(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { 
	 return IID_IPAUcaloIsolationTool; };

       virtual float EtConeCorrectionPt         (const egamma*, float) const =0;
       virtual float EtConeCorrectionPt         (const Analysis::Photon*, float) const =0;
       virtual float EtConeCorrectionPt         (const Analysis::Electron*, float) const =0;

       virtual float EtConeCorrectionJetAreas   (float               , float, int jetsToIgnore=0) =0;
       virtual float EtConeCorrectionJetAreas   (const egamma*       , float, int jetsToIgnore=0) =0;
       virtual float EtConeCorrectionJetAreas   (const TruthParticle*, float, int jetsToIgnore=0) =0;

       virtual double EnergyDensityMedian       (float               , int jetsToIgnore=0) =0;
       virtual double EnergyDensityMedian       (const egamma*       , int jetsToIgnore=0) =0;
       virtual double EnergyDensityMedian       (const TruthParticle*, int jetsToIgnore=0) =0;

       virtual double EnergyDensitySigma        (float               , int jetsToIgnore=0) =0;
       virtual double EnergyDensitySigma        (const egamma*       , int jetsToIgnore=0) =0;
       virtual double EnergyDensitySigma        (const TruthParticle*, int jetsToIgnore=0) =0;

       virtual int    EnergyDensityNjets        (float               , int jetsToIgnore=0) =0;
       virtual int    EnergyDensityNjets        (const egamma*       , int jetsToIgnore=0) =0;
       virtual int    EnergyDensityNjets        (const TruthParticle*, int jetsToIgnore=0) =0;
};

#endif // PHOTONANALYSISUTILS_ICALOISOLATION_H
