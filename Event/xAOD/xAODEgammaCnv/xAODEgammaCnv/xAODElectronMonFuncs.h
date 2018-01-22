/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEGAMMACNV_XAODELECTONMONFUNCS_h
#define XAODEGAMMACNV_XAODELECTONMONFUNCS_h

// Monitoring macros for egamma
// Separate for Electron and Photon
// GETTER for Shower Shape monitoring
#define GETTER(_name_) inline float getShowerShape_##_name_(const xAOD::Electron* eg) \
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
#define GETTER(_name_) inline float getIsolation_##_name_(const xAOD::Electron* eg) \
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; } 
GETTER(etcone20)
GETTER(etcone30)
GETTER(etcone40)    
GETTER(topoetcone20)
GETTER(topoetcone30)
GETTER(topoetcone40)    
GETTER(ptcone20)
GETTER(ptcone30)
GETTER(ptcone40)    
#undef GETTER    

// GETTERs for CaloCluster monitoring   
#define GETTER(_name_) inline float getCluster_##_name_(const xAOD::Electron* eg) \
{    if(eg && eg->caloCluster()) \
        return eg->caloCluster()->_name_(); \
    else return -99.;}
GETTER(et)
GETTER(phi)
GETTER(eta)   
#undef GETTER

// GETTERs for Track monitoring   
#define GETTER(_name_) inline float getTrack_##_name_(const xAOD::Electron* eg) \
{    if(eg && eg->trackParticle()) \
        return eg->trackParticle()->_name_(); \
    else return -99.;}
GETTER(pt)
GETTER(phi)
GETTER(eta)   
GETTER(d0)  
GETTER(z0)    
#undef GETTER
// GETTERs for Track details monitoring    
#define GETTER(_name_) inline float getTrackSummary_##_name_(const xAOD::Electron* eg) \
{ uint8_t val_uint8{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_uint8,xAOD::_name_); \
        return val_uint8; } \
    else return -99; }
GETTER(numberOfInnermostPixelLayerHits) 
GETTER(numberOfInnermostPixelLayerOutliers) 
GETTER(numberOfPixelHits)
GETTER(numberOfPixelOutliers)
GETTER(numberOfSCTHits)    
GETTER(numberOfSCTOutliers)    
GETTER(numberOfTRTHits)
GETTER(numberOfTRTHighThresholdHits)
GETTER(numberOfTRTHighThresholdOutliers)
GETTER(numberOfTRTOutliers)
GETTER(expectInnermostPixelLayerHit)
GETTER(numberOfPixelDeadSensors)
GETTER(numberOfSCTDeadSensors)
GETTER(numberOfTRTXenonHits)

#undef GETTER

#define GETTER(_name_) inline float getTrackSummaryFloat_##_name_(const xAOD::Electron* eg) \
{ float val_float{0}; \
    if(eg && eg->trackParticle()){ \
        eg->trackParticleSummaryValue(val_float,xAOD::_name_); \
        return val_float; } \
    else return -99; }
GETTER(eProbabilityComb)
GETTER(eProbabilityHT)
GETTER(pixeldEdx)    

#undef GETTER
// GETTERs for Calo-Track monitoring
#define GETTER(_name_) inline float getCaloTrackMatch_##_name_(const xAOD::Electron* eg) \
{ float val={-99.}; \
    if(eg && eg->trackParticle()){ \
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
#undef GETTER    
// Additional monitoring function   
inline float getEta2(const xAOD::Electron* eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return fabs(cluster->etaBE(2)); 
    }
    else return -99.;
}
inline float getEtCluster37(const xAOD::Electron* eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        float eta2   = fabs(cluster->etaBE(2)); 
        return cluster->e()/cosh(eta2);      
    }
    else return -99.;
}
inline float getDEmaxs1(const xAOD::Electron *eg){
    if(eg){
        float emax2=0.;
        eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
        float emax=0.;
        eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
        float val = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
        return val;
    }
    else return -99.;
}
inline float rTRT  (const xAOD::Electron* eg) {
    if(eg && eg->trackParticle()){ 
        uint8_t trtHits   = 0;
        eg->trackParticleSummaryValue(trtHits,xAOD::numberOfTRTHits);
        uint8_t trtHTHits = 0; 
        eg->trackParticleSummaryValue(trtHTHits,xAOD::numberOfTRTHighThresholdHits);
        if(trtHits!=0) {
            return ( (double)trtHTHits / (double)trtHits ); 
        }
        else return -99.;
    }
    else return -99.;
}

inline float getSigmaD0(const xAOD::Electron *eg){
    const xAOD::TrackParticle* t = eg->trackParticle();   
    float d0sigma=0.;
    if (t)
    {
        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
            d0sigma=sqrtf(vard0);
        }
        else return -99.;
        return d0sigma;
    }
    else return -99.;
}

inline float getD0sig(const xAOD::Electron *eg){
    const xAOD::TrackParticle* t = eg->trackParticle();   
    float d0sigma=0.;
    if (t)
    {
    
        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
            d0sigma=sqrtf(vard0);
        }
        else return -99.;
        return t->d0()/d0sigma;
    }
    else return -99.;
}

inline float getEnergyBE0(const xAOD::Electron *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(0); 
    }
    else return 0;
}

inline float getEnergyBE1(const xAOD::Electron *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(1); 
    }
    else return 0.;
}

inline float getEnergyBE2(const xAOD::Electron *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(2); 
    }
    else return 0.;
}

inline float getEnergyBE3(const xAOD::Electron *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        return cluster->energyBE(3); 
    }
    else return 0.;
}

inline float getEaccordion(const xAOD::Electron *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster(); 
        float ebe1 = cluster->energyBE(1); 
        float ebe2 = cluster->energyBE(2); 
        float ebe3 = cluster->energyBE(3);
        return (ebe1+ebe2+ebe3);
    }
    else return 0.;
}

inline float getE0Eaccordion(const xAOD::Electron *eg){
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        float ebe0 = cluster->energyBE(0); 
        float ebe1 = cluster->energyBE(1); 
        float ebe2 = cluster->energyBE(2); 
        float ebe3 = cluster->energyBE(3);
        float eacc = ebe1+ebe2+ebe3;
        if(eacc==0.) return 0.;
        return (ebe0/eacc);
    }
    else return 0.;
}

#endif
