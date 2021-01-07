/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BPhysVertexTrackBase.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Base class for vertex-track related classes in need of
// track-to-vertex association handling.
//
// For an usage example see BVertexTrackIsoTool and BPHY8.py .
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BPhysVertexTrackBase_H
#define DERIVATIONFRAMEWORK_BPhysVertexTrackBase_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODBPhys/BPhysHelper.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include <string>
#include <vector>
#include <map>

// forward declarations
namespace InDet {
  class IInDetTrackSelectionTool;
}

class TVector3;

namespace DerivationFramework {
  //
  // typedefs -- to abbreviate long lines
  //
  typedef std::vector<const xAOD::TrackParticle*> TrackBag;
  typedef std::vector<const xAOD::Muon*>          MuonBag;
  typedef InDet::IInDetTrackSelectionTool         TrkSelTool;
  
  class BPhysVertexTrackBase : public AthAlgTool,
    virtual public IAugmentationTool {

  protected:
    class BaseItem {
      
    public:
      BaseItem(std::string Name="_none_", std::string Bname="iso",
	       std::string Prefix="");
      virtual ~BaseItem();
      
      virtual void        setup(std::string Name, std::string Bname="iso",
				std::string Prefix="");
      virtual void        setPrefix(std::string Prefix);
      virtual void        resetVals();
      virtual void        copyVals(const BaseItem& item) = 0;
      virtual std::string buildName(std::string qualifier="",
                                    std::string suffix="");
      virtual std::string toString() const;
      
    public:
      std::string     name;
      std::string     bname;
      std::string     prefix;
    };

  protected:
    class TrackTypeCounter {

    public:
      TrackTypeCounter(BPhysVertexTrackBase& Parent, std::string Name);
      virtual ~TrackTypeCounter();

      virtual void addToCounter(uint64_t atype, uint64_t rtype=0,
                                std::string prefix="", std::string suffix="",
                                uint64_t counts=1);

      virtual void addToCounter(std::string name, uint64_t atype=0,
                                uint64_t counts=1);

      virtual std::string countsToString(uint indent=0) const;
      
    public:
      std::string name;

    private:
      typedef std::map<std::string, std::pair<uint64_t, uint64_t> >
        NameCountMap_t;
      NameCountMap_t        m_cnts;
      BPhysVertexTrackBase& m_parent;
    };
    
  public:
      //
      // enumeration for types of tracks to be considered
      //
      enum track_type {ASSOCPV, PVTYPE0, PVTYPE1, PVTYPE2, PVTYPE3, NONE,
                       NULLVP,
                       CAPVRFN3U0, CAPVNRN3U0, CAPVRF3DU0, CAPVNR3DU0,
                       CAPVRFN3U1, CAPVNRN3U1, CAPVRF3DU1, CAPVNR3DU1,
                       CAPVRFN3U2, CAPVNRN3U2, CAPVRF3DU2, CAPVNR3DU2,
                       CAPVRFNNU3, CAPVNRNNU3, CAPVRFNNU4, CAPVNRNNU4,
                       CAPVRFNNU5, CAPVNRNNU5, CAPVRFNNU6, CAPVNRNNU6,
                       CAPVRFNNU7, CAPVNRNNU7, CAPVRFNNU8, CAPVNRNNU8,
                       CAPVRFNNU9, CAPVNRNNU9 };
      static const int          n_track_types;
      static const std::string  track_type_str[];
      static const uint64_t     track_type_bit[];
  private:
      static       uint64_t     s_track_type_all_cached;

  public:
      //
      // convenience methods
      //
      static const std::string tts(track_type type);
      static uint64_t          ttb(track_type type);
      static uint64_t          ttall();
      static uint64_t          ttallMin();
      static uint64_t          rttor(const std::vector<uint64_t> &vtypes);
      static std::string       wrapLines(std::string lines,
					 std::string prefix);
      static std::string trackToString(const xAOD::TrackParticle* track);

  public:
      //
      // public methods called by the framework
      //
      BPhysVertexTrackBase(const std::string& t, const std::string& n,
			   const IInterface* p);
      
      virtual StatusCode  initialize();
      virtual StatusCode  finalize();
      virtual StatusCode  addBranches() const;

  protected:
      //
      // Hook methods -- need be be overwritten in the concrete class
      //
      virtual StatusCode  initializeHook();
      virtual StatusCode  finalizeHook();
      virtual StatusCode  addBranchesHook() const;
      virtual StatusCode  addBranchesVCSetupHook(size_t ivc) const;
      virtual StatusCode  addBranchesSVLoopHook(const xAOD::Vertex* vtx) const;
      virtual StatusCode  calcValuesHook(const xAOD::Vertex* vtx,
					 const unsigned int ipv,
					 const unsigned int its,
					 const unsigned int itt) const;
      virtual bool        fastFillHook(const xAOD::Vertex* vtx,
				       const int ipv) const;

      //
      // Methods to be called from within addBranchesSVLoopHook()
      //
      virtual StatusCode  calculateValues(const xAOD::Vertex* vtx) const;

      //
      // internal methods
      //
      // name string for vertex pointer and PV index
      virtual std::string buildPvAssocCacheName(const xAOD::Vertex* vtx,
						const int ipv) const;
      
      virtual void        initPvAssocTypeVec();
      virtual TrackBag    findAllTracksInDecay(xAOD::BPhysHelper& vtx) const;
      virtual void        findAllTracksInDecay(xAOD::BPhysHelper& vtx,
					       TrackBag& tracks) const;
      virtual MuonBag     findAllMuonsInDecay(xAOD::BPhysHelper& vtx) const;
      virtual void        findAllMuonsInDecay(xAOD::BPhysHelper& vtx,
					      MuonBag& muons) const;
      virtual TrackBag    findAllMuonIdTracksInDecay(xAOD::BPhysHelper& vtx,
						     MuonBag& muons) const;
      virtual std::vector<TVector3>
	findMuonRefTrackMomenta(xAOD::BPhysHelper& vtx, MuonBag& muons) const;

      virtual TrackBag    selectTracks(const xAOD::TrackParticleContainer*
				       inpTracks,
				       xAOD::BPhysHelper& cand,
				       const unsigned int ipv,
				       const unsigned int its,
				       const unsigned int itt) const;
      virtual TrackBag    selectTracks(const xAOD::TrackParticleContainer*
				       inpTracks,
				       const TrackBag& exclTracks,
				       xAOD::BPhysHelper& cand,
				       const unsigned int ipv,
				       const unsigned int its,
				       const unsigned int itt) const;
      virtual uint64_t detTrackTypes(const xAOD::TrackParticle* track,
                                     const xAOD::Vertex* candPV,
                                     const xAOD::Vertex* candRefPV) const;
      virtual double   getTrackCandPVLogChi2(const xAOD::TrackParticle*
                                             track,
                                             const xAOD::Vertex* vtx,
                                             bool doDCAin3D=false,
                                             int chi2DefToUse=0) const;
      virtual std::vector<double> getTrackLogChi2DCA(const xAOD::TrackParticle*
                                                     track,
                                                     const xAOD::Vertex* vtx,
                                                     bool doDCAin3D=false,
                                                     int chi2DefToUse=0)
        const;
      virtual std::string buildBranchBaseName(unsigned int its,
                                              unsigned int ipv,
                                              unsigned int itt,
                                              std::string preSuffix="") const;
      
      virtual std::pair<const xAOD::Vertex*, double>
        findMinChi2PV(const xAOD::TrackParticle* track,
                      const xAOD::Vertex* candPV,
                      const xAOD::Vertex* candRefPV,
                      const std::vector<uint64_t>& pvtypes,
                      const int minNTracksInPV,
                      const bool useRefittedPvs,
                      const bool doDCAin3D,
                      const int chi2DefToUse) const;

      virtual const xAOD::Vertex*
        findAssocPV(const xAOD::TrackParticle* track,
                    const xAOD::Vertex* candPV,
                    const xAOD::Vertex* candRefPV,
                    const std::vector<uint64_t>& pvtypes,
                    const int minNTracksInPV,
                    const bool useRefittedPvs) const;
      
  protected:      
      // job options
      std::vector<std::string>         m_branchPrefixes;
      std::string                      m_branchBaseName;
      std::string                      m_branchSuffix;
      std::vector<std::string>         m_vertexContainerNames;
      std::string                      m_trackParticleContainerName;
      ToolHandleArray<TrkSelTool>      m_trackSelectionTools;
      
      ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

      ToolHandle<CP::ITrackVertexAssociationTool> m_tvaTool;

      std::string                      m_pvContainerName;
      std::vector<std::string>         m_refPVContainerNames;

      int                              m_doVertexType;
      std::vector<uint64_t>            m_useTrackTypes;
      bool                             m_incPrecVerticesInDecay;
      int                              m_minNTracksInPV;
      std::vector<uint64_t>            m_pvTypesToConsider;
      int                              m_debugTrackTypes;
      std::vector<uint64_t>            m_debugTracksInEvents;

      // working point of TVA tool
      bool m_tvaToolHasWpLoose;

      // containers
      mutable const xAOD::TrackParticleContainer*    m_tracks;
      mutable const xAOD::TrackParticleAuxContainer* m_tracksAux;
      mutable const xAOD::VertexContainer*           m_pvtxContainer;
      mutable const xAOD::VertexContainer*           m_svtxContainer;
      mutable const xAOD::VertexAuxContainer*        m_svtxAuxContainer;
      mutable const xAOD::VertexContainer*           m_refPVContainer;
      mutable const xAOD::VertexAuxContainer*        m_refPVAuxContainer;
      
      // cache for individual vertex types
      std::vector<xAOD::BPhysHelper::pv_type> m_pvAssocTypes;

      mutable unsigned int m_nEvtsSeen;

      // event info
      mutable const xAOD::EventInfo* m_eventInfo;

      // cache for similar PV-to-SV associations
      typedef std::map<std::string, int> StringIntMap_t;
      mutable StringIntMap_t m_pvAssocResMap;

      // track types considered
      uint64_t m_trackTypesUsed;

      // track type counter map (for debugging)
      std::unique_ptr<TrackTypeCounter> m_mttc;

      // run and event numbers (see EventIDBase.h for types)
      mutable unsigned int  m_runNumber;
      mutable uint64_t      m_evtNumber;

      // debug tracks in the current event?
      mutable bool          m_debugTracksInThisEvent;
      
  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BPhysVertexTrackBase_H
