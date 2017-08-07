/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BMuonTrackIsoTool.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add muon track isolation information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BMuonTrackIsoTool_H
#define DERIVATIONFRAMEWORK_BMuonTrackIsoTool_H

#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
#include "xAODMuon/MuonContainer.h"
#include "boost/multi_array.hpp"

namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace DerivationFramework {
  
  class BMuonTrackIsoTool : virtual public BPhysVertexTrackBase {

  private:
    typedef BPhysVertexTrackBase super;
    
    //
    // internal helper class
    //
  protected:
    class MuIsoItem : public BaseItem {
    
  public:
    MuIsoItem(std::string Name="_none_", std::string Bname="muiso",
	      std::string Prefix="");
    virtual ~MuIsoItem();
	
    virtual void        resetVals();
    virtual void        copyVals(const BaseItem& item);
    virtual void        copyVals(const MuIsoItem& item);
    virtual void        fill(double isoValue=-2., int nTracks=-1,
			     const xAOD::Muon* muon=NULL);
    virtual std::string muIsoName();
    virtual std::string nTracksName();
    virtual std::string muLinkName();
    
  public:
    mutable std::vector<float>  vIsoValues;
    mutable std::vector<int>    vNTracks;
    mutable MuonBag             vMuons;
  }; // MuIsoItem
    
  public: 
      BMuonTrackIsoTool(const std::string& t, const std::string& n,
			const IInterface* p);

  protected:
      // Hook methods 
      virtual StatusCode  initializeHook();
      virtual StatusCode  finalizeHook();
      
      virtual StatusCode  addBranchesVCSetupHook(size_t ivc) const;

      virtual StatusCode  addBranchesSVLoopHook(const xAOD::Vertex* vtx) const;

      virtual StatusCode  calcValuesHook(const xAOD::Vertex* vtx,
					 const unsigned int ipv,
					 const unsigned int its,
					 const unsigned int itt) const;
      virtual bool        fastFillHook(const xAOD::Vertex* vtx,
				       const int ipv) const;
      
  private:
      virtual StatusCode  saveIsolation(const xAOD::Vertex* vtx) const;
      virtual void        initResults();
      virtual void        setResultsPrefix(std::string prefix) const;
      
      virtual std::string buildBranchName(unsigned int ic,
					  unsigned int its,
					  unsigned int ipv,
					  unsigned int itt) const;
      
  private:      
      // job options
      std::string                      m_muonContainerName;
      std::vector<double>              m_isoConeSizes;
      std::vector<double>              m_isoTrkImpLogChi2Max;

      // containers
      mutable const xAOD::MuonContainer* m_muons;
      
      
      // results array
      typedef boost::multi_array<MuIsoItem, 4> MuIsoItem4_t;
      mutable MuIsoItem4_t m_results;

  }; // BMuonTrackIsoTool
} // namespace

#endif // DERIVATIONFRAMEWORK_BMuonTrackIsoTool_H
