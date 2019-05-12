///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METAssociator.h
// Header file for class METAssociator
//
// This is the base class for tools that construct MET terms
// from other object collections.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
///////////////////////////////////////////////////////////////////
#ifndef METRECONSTRUCTION_METASSOCIATOR_H
#define METRECONSTRUCTION_METASSOCIATOR_H

// STL includes
#include <string>

// FrameWork includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// METRecoInterface includes
#include "METRecoInterface/IMETAssocToolBase.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
//#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"

#include "TRandom3.h"

namespace InDet {
  class IInDetTrackSelectionTool;
}
 
namespace xAOD {
  class ITrackIsolationTool;
  class ICaloTopoClusterIsolationTool;
}

namespace met {
  class METAssociator
    : virtual public asg::AsgTool,
      virtual public IMETAssocToolBase
  {
 
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:
 
    struct ConstitHolder {
      const xAOD::TrackParticleContainer* trkCont = 0;
      // Use IParticleContainer for flexibility e.g. if combining clusters & towers
      const xAOD::IParticleContainer* tcCont = 0;
      const xAOD::PFOContainer* pfoCont = 0;
      const xAOD::Vertex* pv = 0;
    };

    // Constructor w/ name
    METAssociator(const std::string& name);
    // Default Destructor
    virtual ~METAssociator();

    // AsgTool Handles
    virtual StatusCode initialize();
    virtual StatusCode execute   (xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const;
    virtual StatusCode finalize  ();

    ///////////////////////////////////////////////////////////////////
    // Protected methods:
    ///////////////////////////////////////////////////////////////////
    protected:

    std::string m_input_data_key;
    std::string m_pvcoll;
    std::string m_trkcoll;
    std::string m_clcoll;
    std::string m_pfcoll;

    bool m_pflow;
    bool m_recoil;
    bool m_useTracks;
    bool m_useRapidity;
    bool m_useIsolationTools;
    bool m_useModifiedClus;
    bool m_weight_charged_pfo;
    bool m_cleanChargedPFO;

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;
    ToolHandle<xAOD::ITrackIsolationTool> m_trkIsolationTool;
    ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloIsolationTool;
    bool m_skipconst;
    std::string m_forcoll;
    double m_foreta;

    double m_cenTrackPtThr;
    double m_forTrackPtThr;

    // reconstruction process to be defined in the individual tools
    // pure virtual -- we have no default
    virtual StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const = 0;
    StatusCode retrieveConstituents(met::METAssociator::ConstitHolder& constits) const;

    bool acceptTrack (const xAOD::TrackParticle* trk, const xAOD::Vertex* pv) const;
    bool isGoodEoverP(const xAOD::TrackParticle* trk) const;

    virtual StatusCode fillAssocMap(xAOD::MissingETAssociationMap* metMap,
				    const xAOD::IParticleContainer* hardObjs) const;
    virtual StatusCode extractPFO(const xAOD::IParticle* obj,
				  std::vector<const xAOD::IParticle*>& pfolist,
				  const met::METAssociator::ConstitHolder& constits,
				  std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const = 0;

    virtual StatusCode extractPFOHR(const xAOD::IParticle* /*obj*/,
                                    std::vector<const xAOD::IParticle*> /*hardObjs*/,
                                    std::vector<const xAOD::IParticle*>& /*pfolist*/,
                                    const met::METAssociator::ConstitHolder& /*constits*/,
                                    std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &/*momenta*/,
                                    float& /*UEcorr*/) const
    {return StatusCode::FAILURE;}

    StatusCode GetUEcorr(const met::METAssociator::ConstitHolder& constits,  
                         std::vector<TLorentzVector>& v_clus,
                         TLorentzVector& clus, 
                         TLorentzVector& HR,
                         const float Drcone,
                         const float MinDistCone,
                         float& UEcorr) const;

    virtual StatusCode extractTracks(const xAOD::IParticle* obj,
				     std::vector<const xAOD::IParticle*>& constlist,
				     const met::METAssociator::ConstitHolder& constits) const = 0;
    virtual StatusCode extractTopoClusters(const xAOD::IParticle* obj,
					   std::vector<const xAOD::IParticle*>& tclist,
					   const met::METAssociator::ConstitHolder& constits) const = 0;
    
    static inline bool greaterPt(const xAOD::IParticle* part1, const xAOD::IParticle* part2) {
      return part1->pt()>part2->pt();
    }
    static inline bool greaterPtPFO(const xAOD::PFO* part1, const xAOD::PFO* part2) {
      if (part1->charge()==0 && part2->charge()!=0) return false;
      if (part1->charge()!=0 && part2->charge()==0) return true;
      if (part1->charge()==0 && part2->charge()==0) return part1->ptEM()>part2->ptEM();
      return part1->pt()>part2->pt();
    }
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
    private:

    // Default Constructor
    METAssociator();
  };
}

#endif // METRECONSTRUCTION_METASSOCBUILDERTOOL_H
