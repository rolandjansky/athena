/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_SLOWMUON_V1_H
#define XAODMUON_VERSIONS_SLOWMUON_V1_H

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODMuon/MuonContainer.h" 

namespace xAOD {

  /// Class describing a SlowMuon.
  ///
  /// @author Susan Cheatham <Susan.Cheatham@cern.ch>
  /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @author Shlomit Tarem <shlomit.tarem@cern.ch>
  /// @author Heberth Torres <htorres@cern.ch>
  /// @nosubgrouping
  ///
  class SlowMuon_v1 : public SG::AuxElement {

  public:
    
    /// Default constructor
    SlowMuon_v1();

    /// @name 
    /// Returns 
    /// @{
    /// Returns 
    float beta () const;
    /// Returns 
    float betaT() const;
    /// Returns 
    float ann() const;
    /// Sets
    void setBeta(float beta);
    /// Sets
    void setBetaT(float betaT);
    /// Sets
    void setAnn(float ann);
    /// @}

    /// @name 
    /// Returns 
    /// @{
    /// Returns 
    int nRpcHits() const;
    /// Returns 
    int nTileCells() const;
    /// Sets
    void setNRpcHits(int nRpcHits);
    /// Sets
    void setNTileCells(int nTileCells);
    /// @}

    /// @name 
    /// Returns 
    /// @{
    /// Returns 
    float rpcBetaAvg() const;
    /// Returns 
    float rpcBetaRms() const;
    /// Returns 
    float rpcBetaChi2() const;
    /// Returns 
    int rpcBetaDof() const;
    /// Sets
    void setRpcInfo(float rpcBetaAvg, float rpcBetaRms, float rpcBetaChi2, int rpcBetaDof);
    /// @}

    /// @name 
    /// Returns 
    /// @{
    /// Returns 
    float mdtBetaAvg() const;
    /// Returns 
    float mdtBetaRms() const;
    /// Returns 
    float mdtBetaChi2() const;
    /// Returns 
    int mdtBetaDof() const;
    /// Sets
    void setMdtInfo(float mdtBetaAvg, float mdtBetaRms, float mdtBetaChi2, int mdtBetaDof);
    /// @}

    /// @name 
    /// Returns 
    /// @{
    /// Returns 
    float caloBetaAvg() const;
    /// Returns 
    float caloBetaRms() const;
    /// Returns 
    float caloBetaChi2() const;
    /// Returns 
    int caloBetaDof() const;
    /// Sets
    void setCaloInfo(float caloBetaAvg, float caloBetaRms, float caloBetaChi2, int caloBetaDof);
    /// @}

    /// @name
    /// Returns
    /// @{
    /// Returns 
    float dEdxPixel() const;
    /// Returns 
    float dEdxCalo() const;
    /// Returns 
    int dEdxNClusters() const;
    /// Sets
    void setDEdxPixel(float dEdxPixel);
    /// Sets
    void setDEdxCalo(float dEdxCalo);
    /// Sets
    void setDEdxNClusters(int dEdxNClusters);
    /// @}

    /// @name Links
    /// Returns 
    /// @{
    /// Returns 
    const ElementLink< MuonContainer >& muonLink() const;
    /// Sets
    void setMuonLink(const ElementLink< MuonContainer >& muonLink);
    /// @}

  }; // end of the SlowMuon_v1 class definitions

} // end of the xAOD namespace

#endif // XAODMUON_VERSIONS_SLOWMUON_V1_H
