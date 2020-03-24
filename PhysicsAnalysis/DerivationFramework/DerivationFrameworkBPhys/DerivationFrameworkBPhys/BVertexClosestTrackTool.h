/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
         double VtxNDErr2=-99., double TrkNDErr2=-99.,
         double Phi0Used=-999.,
	       int NTrksChi2=0, xAOD::TrackParticle* CloseTrack=NULL,
         TrackBag Tracks = {},
         std::vector<std::vector<double> > Vtap = {},
         std::vector<unsigned short> Selpat = {});

	virtual ~CtItem();
	
	virtual void        setup(std::string Name="_none_",
				  std::string Bname="ctrk",
				  std::string Prefix="");
	virtual void        setup(std::string Name, std::string Bname,
				  std::string Prefix,
				  double Dca, double DcaErr,
				  double Zca, double ZcaErr,
          double VtxNDErr2, double TrkNDErr2,
          double Phi0Used,
				  int NTrksChi2,
          xAOD::TrackParticle* CloseTrack=NULL,
          TrackBag Tracks = {},
          std::vector<std::vector<double> > Vtap = {},
          std::vector<unsigned short> Selpat = {});
	virtual void        resetVals();
	virtual void        copyVals(const BaseItem& item);
	virtual void        copyVals(const CtItem& item);
	virtual std::string dcaName();
	virtual std::string dcaErrName();
	virtual std::string zcaName();
	virtual std::string zcaErrName();
	virtual std::string vtxNDErr2Name();
	virtual std::string trkNDErr2Name();
	virtual std::string phi0UsedName();
	virtual std::string nTrksChi2Name();
	virtual std::string closeTrackName();
  virtual std::string toString() const;

  public:
	mutable double             dca;
	mutable double             dcaErr;
	mutable double             zca;
	mutable double             zcaErr;
  mutable double             vtxNDErr2;
  mutable double             trkNDErr2;
  mutable double             phi0Used;
	mutable int                nTrksChi2;
	const xAOD::TrackParticle* closeTrack;
  mutable TrackBag                          tracks;
  mutable std::vector<std::vector<double> > vtap;
  mutable std::vector<unsigned short>       selpat;

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
      virtual StatusCode  logCloseTracksDebugInfo() const;

  private:      
      // job options

      std::vector<std::string> m_closeTrackChi2SetName;
      std::vector<int>         m_closeTrackCorrChi2;
      std::vector<bool>        m_minDCAin3D;
      std::vector<double>      m_closeTrackMaxLogChi2;
      std::vector<double>      m_nCloseTrackMaxLogChi2;

      // results array
      typedef boost::multi_array<CtItem, 4> CtItem4_t;
      mutable CtItem4_t m_results;

      // last run and event numbers seen
      mutable unsigned int  m_lastRunNumber;
      mutable uint64_t      m_lastEvtNumber;

      // last secondary vertex (candidate) index
      mutable unsigned int  m_svIdx;
      
  }; // BVertexClosestTrackTool
} // namespace

#endif // DERIVATIONFRAMEWORK_BVertexClosestTrackTool_H
