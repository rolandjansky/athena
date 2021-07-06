/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMonitorBaseAlgorithm.h"



egammaMonitorBaseAlgorithm::egammaMonitorBaseAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  AthReentrantAlgorithm(name,pSvcLocator)
{}

egammaMonitorBaseAlgorithm::~egammaMonitorBaseAlgorithm() {}


StatusCode egammaMonitorBaseAlgorithm::initialize() 
{
    
 
  return StatusCode::SUCCESS;
}


/*! Macros for plotting */
#define GETTER(_name_) float egammaMonitorBaseAlgorithm::getShowerShape_##_name_(const xAOD::Egamma* eg) const \
{ float val{-99}; \
    eg->showerShapeValue(val,xAOD::EgammaParameters::_name_); \
    return val; }
    GETTER(e011)
    GETTER(e132)
    GETTER(e237)
    GETTER(e277)
    GETTER(ethad)
    GETTER(ethad1)
    GETTER(weta1)
    GETTER(weta2)
    GETTER(f1)
    GETTER(e2tsts1)
    GETTER(emins1)
    GETTER(emaxs1)
    GETTER(wtots1)
    GETTER(fracs1)
    GETTER(Reta)
    GETTER(Rphi)
    GETTER(f3)
    GETTER(f3core)
    GETTER(Eratio)
    GETTER(Rhad)
    GETTER(Rhad1)
    GETTER(DeltaE)
#undef GETTER


// GETTER for Isolation monitoring
#define GETTER(_name_) float egammaMonitorBaseAlgorithm::getIsolation_##_name_(const xAOD::Egamma* eg) const\
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; }
    GETTER(ptcone20)
    GETTER(ptcone30)
    GETTER(ptcone40)
    GETTER(ptvarcone20)
    GETTER(ptvarcone30)
    GETTER(ptvarcone40)
#undef GETTER


#define GETTER(_name_) float egammaMonitorBaseAlgorithm::getIsolation_##_name_(const xAOD::Egamma* eg) const\
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; }
    GETTER(etcone20)
    GETTER(etcone30)
    GETTER(etcone40)
    GETTER(topoetcone20)
    GETTER(topoetcone30)
    GETTER(topoetcone40)
#undef GETTER


// GETTERs for Track monitoring
#define GETTER(_name_) float egammaMonitorBaseAlgorithm::getTrack_##_name_(const xAOD::Electron* eg) const\
{    if(eg && eg->trackParticle()) \
    return eg->trackParticle()->_name_(); \
    else return -99.;}
    GETTER(pt)
    GETTER(phi)
    GETTER(eta)
    GETTER(d0)
    GETTER(z0)
#undef GETTER
    
  
// GETTERs for Calo-Track monitoring
#define GETTER(_name_) float egammaMonitorBaseAlgorithm::getCaloTrackMatch_##_name_(const xAOD::Electron* eg) const\
{ float val={-99.}; \
    if(eg){ \
        eg->trackCaloMatchValue(val,xAOD::EgammaParameters::_name_);} \
    return val; }
    GETTER(deltaEta0)
    GETTER(deltaPhi0)
    GETTER(deltaPhiRescaled0)
    GETTER(deltaEta1)
    GETTER(deltaPhi1)
    GETTER(deltaPhiRescaled1)
    GETTER(deltaEta2)
    GETTER(deltaPhi2)
    GETTER(deltaPhiRescaled2)
    GETTER(deltaEta3)
    GETTER(deltaPhi3)
    GETTER(deltaPhiRescaled3)
    GETTER(deltaPhiFromLastMeasurement)
#undef GETTER




