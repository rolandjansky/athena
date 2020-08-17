/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_CALOTAG_H
#define MUONCOMBINEDEVENT_CALOTAG_H

#include "MuonCombinedEvent/TagBase.h"

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "muonEvent/DepositInCalo.h"

#include <vector> 

namespace MuonCombined {
  

  /** TagBase implementation for a calo tag */
  class CaloTag : public TagBase, public Trk::EnergyLoss {
  public:
    
  /** Calo Energy Loss Type 
          Parametrized : reconstruction configured to use the parametrization w/o looking in the calo (eg calo off)
          NotIsolated  : the measurement in the calorimeter is not reliable due to additional energy around the muon
                         --> the parametrized value isused.
          MOP          : measurement found to be compatible with most probable value --> mop used as more reliable at this 
                         region of the eloss
          Tail         : measured eloss significantly higher than mop --> the calo measurement used
          FSRcandidate : in standalone reconstruction the Tail option was used. but an imbalance is observed when comparing 
                         Pstandalone and Pinnerdetector (Pstandalone>Pinnerdetector) --> if using the mop resolves the imbalance the 
                         excess energy loss is stored as fsrEnergy and the mop is used as the eloss.
  */
    enum EnergyLossType { Parametrized=0, NotIsolated=1, MOP=2, Tail=3, FSRcandidate=4 };

    /** default constructor - to be used only for persistency */
    CaloTag(void);

    /** full constructor */
    CaloTag (const Trk::EnergyLoss& eloss);

    CaloTag (xAOD::Muon::Author author,
		float   deltaE,
                float   sigmaDeltaE,
                float   sigmaMinusDeltaE=0.0,
                float   sigmaPlusDeltaE=0.0,
                unsigned short energyLossType=0,
                float          likelihood=0,
                float          muonScore=0,
                unsigned short tag=0);

    /** full constructor with the detailed deposits in Calo */
    CaloTag (xAOD::Muon::Author author,
		float   deltaE,
                float   sigmaDeltaE,
                float   sigmaMinusDeltaE,
                float   sigmaPlusDeltaE,
                unsigned short energyLossType,
                float likelihood,
                float muonScore,
                unsigned short tag,
                const std::vector<DepositInCalo>& deposits);

    /** destructor */
    ~CaloTag();

    /** name string */
    std::string name() const { return "CaloTag"; }

    /** print content to string */
    std::string toString() const { return name(); }
    /** Accessor methods */

    /** energy determined from parametrization or not (measured) */
    CaloTag::EnergyLossType energyLossType (void) const;

    /** the Calo Muon Identification tag */
    unsigned short caloMuonIdTag() const { return m_caloMuonIdTag; }

    /** the calo Muon Identification likehood */
    double caloLRLikelihood() const { return  m_caloLRLikelihood; }

    /** the calo Muon score */
    double caloMuonScore() const { return  m_caloMuonScore; }

    /** the vector of detailed deposits in calo layers */
    const std::vector<DepositInCalo>& depositInCalo() const { return m_deposits; }

    /**  FSR Candidate Energy */
    float fsrCandidateEnergy() const { return m_fsrCandidateEnergy; }

    /** isolation ET in core deltaR */
    float etCore() const { return m_etCore; }

    /** set methods */

    /** set is_parametrized or measured energy loss */
    void set_energyLossType(const CaloTag::EnergyLossType lossType) { m_energyLossType = lossType; }

    /** set the likelihood */
    void set_caloLRLikelihood ( const float likelihood ) { m_caloLRLikelihood = likelihood; }

    /** set the calo muon score */
    void set_caloMuonScore ( const float muonScore ) { m_caloMuonScore = muonScore; }

    /** set the tag */
    void set_caloMuonIdTag ( unsigned short tag ) { m_caloMuonIdTag = tag; }

    /** set the detailed energy deposits in the calorimeter layers */
    void set_deposits( const std::vector<DepositInCalo> & deposits ) { m_deposits = deposits; }

    /** insert a deposit */
    void insert_deposit ( const DepositInCalo& deposit ) { m_deposits.push_back( deposit ); }

    /**  FSR Candidate Energy */
    void set_fsrCandidateEnergy(const float fs) { m_fsrCandidateEnergy=fs; }

    /** isolation ET in core deltaR */
    void set_etCore ( const float etcore ) { m_etCore=etcore; }

    /** access to secondary author */
    Author author2() const { return m_author2; }

    /** access to third author */
    Author author3() const { return m_author3; }

    /** set the secondary author */
    void set_author2 ( const Author author2 ) { m_author2 = author2; }

    /** set the third author */
    void set_author3 ( const Author author3 ) { m_author3 = author3; }

  private:
    /** block copy and assignment */
    CaloTag(const CaloTag&) = delete;
    CaloTag& operator=(const CaloTag&) = delete;

    /** data content */

    EnergyLossType m_energyLossType;
    float          m_caloLRLikelihood;
    float          m_caloMuonScore;
    unsigned short m_caloMuonIdTag;
    float          m_fsrCandidateEnergy;
    std::vector<DepositInCalo> m_deposits;
    float          m_etCore;  // summed cell ET in core DeltaR 
    Author	   m_author2; // in case of tag by more than one algorithm
    Author	   m_author3; // in case of tag by all three algorithms


  };
inline CaloTag::EnergyLossType CaloTag::energyLossType (void) const
{ return m_energyLossType; }

// should be blocked ?
// inline CaloTag* CaloTag::clone() const
// { return new CaloTag(*this); }


}


#endif

