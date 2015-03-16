/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// EnergyLossMonitor.h
///////////////////////////////////////////////////////////////////

#ifndef TRK_ENERGYLOSSMONITOR_H
#define TRK_ENERGYLOSSMONITOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/NTuple.h"

#include "TrkValInterfaces/IEnergyLossMonitor.h"

class INTupleSvc;

namespace Trk {

/** @class EnergyLossMonitor

    This validation tool Energy states of partlices.

    @author Sebastian.Fleischmann@cern.ch
*/

  class EnergyLossMonitor : virtual public Trk::IEnergyLossMonitor, public AthAlgTool {
    public:

    /** standard AlgTool constructor / destructor */
    EnergyLossMonitor(const std::string&,const std::string&,const IInterface*);
    ~EnergyLossMonitor(){}

    /** standard Athena methods */
    StatusCode initialize();
    StatusCode finalize();

    /** Initialize State */
    virtual void initializeTrack(double p, 
                                 double E,
                                 double eta,
                                 double phi) const;

    /** Record a single TrackState */
    virtual void recordTrackState(const Amg::Vector3D& pos, 
                                  const Amg::Vector3D& mom,
                                  double mass) const;

    /** Finalization State */
    virtual void finalizeTrack() const;
    
private:    
    /** Ntuple Business */
    INTupleSvc*                              m_ntupleSvc;  
    std::string                              m_outputNtuplePath;
    std::string                              m_outputNtupleDescription;

    /** Ntuple helper */
    mutable int                              m_currentStep;    

    /** Step variables */
    mutable NTuple::Item<long>               m_steps;
    /** Initial variables */
    mutable NTuple::Item<float>              m_initialP;
    mutable NTuple::Item<float>              m_initialE;
    mutable NTuple::Item<float>              m_initialEta;
    mutable NTuple::Item<float>              m_initialPhi;

    mutable NTuple::Array<float>             m_p;
    mutable NTuple::Array<float>             m_E;
    mutable NTuple::Array<float>             m_eta;  
    mutable NTuple::Array<float>             m_phi;
    mutable NTuple::Array<float>             m_hitX;
    mutable NTuple::Array<float>             m_hitY;
    mutable NTuple::Array<float>             m_hitR;
    mutable NTuple::Array<float>             m_hitZ;

};


} // end of namespace

#endif // TRK_TRACKPOSITIONNTUPLEHELPER_H
