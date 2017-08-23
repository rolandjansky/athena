/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <string>
#include <vector>
#include <set>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBPhys/BPhysHelper.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "DataModel/DataVector.h"

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
      
    public:
      std::string     name;
      std::string     bname;
      std::string     prefix;
    };

  public:
      //
      // enumeration for types of tracks to be considered
      //
      enum track_type {ASSOCPV, PVTYPE0, PVTYPE1, PVTYPE2, PVTYPE3, NONE};
      static const int          n_track_types;
      static const std::string  track_type_str[];
      static const unsigned int track_type_bit[];
  private:
      static       unsigned int track_type_all_cached;

  public:
      //
      // convenience methods
      //
      static const std::string tts(track_type type);
      static unsigned int      ttb(track_type type);
      static unsigned int      ttall();
      static std::string       wrapLines(std::string lines,
					 std::string prefix);
      
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
      virtual int         detTrackTypes(const xAOD::TrackParticle* track,
					const xAOD::Vertex* candPV) const;
      virtual double      getTrackCandPVLogChi2(const xAOD::TrackParticle*
						track,
						const Amg::Vector3D& pos) const;
      virtual std::vector<double> getTrackLogChi2DCA(const xAOD::TrackParticle*
						     track,
						     const Amg::Vector3D& pos,
						     bool doDCAin3D=false)
	const;
      virtual std::string buildBranchBaseName(unsigned int its,
					      unsigned int ipv,
					      unsigned int itt) const;

  protected:      
      // job options
      std::vector<std::string>         m_branchPrefixes;
      std::string                      m_branchBaseName;
      std::string                      m_branchSuffix;
      std::vector<std::string>         m_vertexContainerNames;
      std::string                      m_trackParticleContainerName;
      ToolHandleArray<TrkSelTool>      m_trackSelectionTools;
      
      ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

      std::string                      m_pvContainerName;
      std::vector<std::string>         m_refPVContainerNames;

      int                              m_doVertexType;
      std::vector<int>                 m_useTrackTypes;
      bool                             m_incPrecVerticesInDecay;

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

      // cache for similar PV-to-SV associations
      typedef std::map<std::string, int> StringIntMap_t;
      mutable StringIntMap_t m_pvAssocResMap;
      
  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BPhysVertexTrackBase_H
