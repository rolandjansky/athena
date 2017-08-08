/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BVertexTrackIsoTool.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add B vertex track isolation information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BVertexTrackIsoTool_H
#define DERIVATIONFRAMEWORK_BVertexTrackIsoTool_H

#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
#include "boost/multi_array.hpp"

namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace DerivationFramework {
  
  class BVertexTrackIsoTool : virtual public BPhysVertexTrackBase {
    
  private:
    typedef BPhysVertexTrackBase super;
    
    //
    // internal helper class
    //
  protected:
    class IsoItem : public BaseItem {
    
  public:
    IsoItem(std::string Name="_none_", std::string Bname="iso",
	    std::string Prefix="",
	    double IsoValue=-1., int NTracks=0);
    virtual ~IsoItem();
    
    virtual void setup(std::string Name, std::string Bname="iso",
		       std::string Prefix="");
    virtual void setup(std::string Name, std::string Bname,
		       std::string Prefix,
		       double IsoValue, int NTracks=0);
    virtual void resetVals();
    virtual void copyVals(const BaseItem& item);
    virtual void copyVals(const IsoItem& item);
    virtual std::string isoName();
    virtual std::string nTracksName();
    
  public:
    mutable double  isoValue;
    mutable int     nTracks;
  }; // IsoItem

  public: 
      BVertexTrackIsoTool(const std::string& t, const std::string& n,
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
      virtual StatusCode  calculateIsolation(const xAOD::Vertex* vtx) const;
      virtual StatusCode  calcIsolation(const IsoItem& iso,
					const xAOD::Vertex* vtx,
					const double coneSize,
					const double logChi2Max,
					const ToolHandle<TrkSelTool>& tSelTool,
					const xAOD::BPhysHelper::pv_type
					pvAssocType,
					const int trackTypes ) const;

      virtual void        initResults();
      virtual void        setResultsPrefix(std::string prefix) const;
      
      virtual std::string buildBranchName(unsigned int ic,
					  unsigned int its,
					  unsigned int ipv,
					  unsigned int itt) const;
      
  private:      
      // job options
      std::vector<double>              m_isoConeSizes;
      std::vector<double>              m_isoTrkImpLogChi2Max;
      bool                             m_useOptimizedAlgo;

      // results array
      typedef boost::multi_array<IsoItem, 4> IsoItem4_t;
      mutable IsoItem4_t m_results;

  }; // BVertexTrackIsoTool
} // namespace

#endif // DERIVATIONFRAMEWORK_BVertexTrackIsoTool_H
