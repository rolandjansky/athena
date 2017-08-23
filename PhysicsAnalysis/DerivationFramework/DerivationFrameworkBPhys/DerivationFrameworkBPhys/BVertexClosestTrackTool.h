
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BVertexClosestTrackTool.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Add B vertex closest track information for different configurations,
// different track selections and different PV-to-SV association methods.
//
// For an usage example see BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BVertexClosestTrackTool_H
#define DERIVATIONFRAMEWORK_BVertexClosestTrackTool_H

#include "DerivationFrameworkBPhys/BPhysVertexTrackBase.h"
#include "boost/multi_array.hpp"

namespace InDet {
  class IInDetTrackSelectionTool;
}

namespace DerivationFramework {
  
  class BVertexClosestTrackTool : virtual public BPhysVertexTrackBase {

  private:
    typedef BPhysVertexTrackBase super;
    
    //
    // internal helper class
    //
  protected:
    class CtItem : public BaseItem {

      public:
	CtItem(std::string Name="_none_",
	       std::string Bname = "ctrk",
	       std::string Prefix="",
	       double Dca=-999., double DcaErr=-99.,
	       double Zca=-999., double ZcaErr=-99.,
	       int NTrksChi2=0, xAOD::TrackParticle* CloseTrack=NULL);
	virtual ~CtItem();
	
	virtual void        setup(std::string Name="_none_",
				  std::string Bname="ctrk",
				  std::string Prefix="");
	virtual void        setup(std::string Name, std::string Bname,
				  std::string Prefix,
				  double Dca, double DcaErr,
				  double Zca, double ZcaErr,
				  int NTrksChi2,
				  xAOD::TrackParticle* CloseTrack=NULL);
	virtual void        resetVals();
	virtual void        copyVals(const BaseItem& item);
	virtual void        copyVals(const CtItem& item);
	virtual std::string dcaName();
	virtual std::string dcaErrName();
	virtual std::string zcaName();
	virtual std::string zcaErrName();
	virtual std::string nTrksChi2Name();
	virtual std::string closeTrackName();

      public:
	mutable double             dca;
	mutable double             dcaErr;
	mutable double             zca;
	mutable double             zcaErr;
	mutable int                nTrksChi2;
	const xAOD::TrackParticle* closeTrack;
  }; // CtItem
      
  public: 
      BVertexClosestTrackTool(const std::string& t, const std::string& n,
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
      virtual StatusCode  saveClosestTrack(const xAOD::Vertex* vtx) const;
      virtual void        initResults();
      virtual void        setResultsPrefix(std::string prefix) const;

  private:      
      // job options
      bool   m_minDCAin3D;
      double m_closeTrackMaxLogChi2;
      double m_nCloseTrackMaxLogChi2;

      // results array
      typedef boost::multi_array<CtItem, 3> CtItem3_t;
      mutable CtItem3_t m_results;

  }; // BVertexClosestTrackTool
} // namespace

#endif // DERIVATIONFRAMEWORK_BVertexClosestTrackTool_H
