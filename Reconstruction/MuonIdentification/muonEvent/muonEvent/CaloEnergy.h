
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 calorimeter energy deposit (measured or parametrized) with asymmetric error
 Author: Alan Poppleton
 Author: Gustavo Ordenez
 Integration with CombinedMuon.h - Ketevi A. Assamagan
 ---------------------------------------------------------------------------
 ***************************************************************************/

#ifndef MUONEVENT_CALOENERGY_H
#define MUONEVENT_CALOENERGY_H

#include "muonEvent/DepositInCalo.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

#include <vector>

/** @class CaloEnergy
    @brief class extending the basic Trk::EnergyLoss to describe
           the measured or parameterised muon energy loss in the ATLAS calorimeters
    @author K. Assamagan, G. Ordonez, A. Poppleton
*/

class CaloEnergy : public Trk::EnergyLoss {

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
    CaloEnergy(void);

    /** full constructor */
    CaloEnergy (const Trk::EnergyLoss& eloss);
 
    CaloEnergy (float   deltaE,
                float   sigmaDeltaE,
                float   sigmaMinusDeltaE=0.0,
                float   sigmaPlusDeltaE=0.0,
		unsigned short energyLossType=0,
		float          likelihood=0,
		unsigned short tag=0);

    /** full constructor with the detailed deposits in Calo */
    CaloEnergy (float	deltaE,
		float	sigmaDeltaE,
		float	sigmaMinusDeltaE,
		float   sigmaPlusDeltaE,
		unsigned short energyLossType,
		float likelihood,
		unsigned short tag,
		const std::vector<DepositInCalo>& deposits);

    /** destructor */
    ~CaloEnergy();

    /** copy constructor */
    CaloEnergy (const CaloEnergy&);
    CaloEnergy* clone() const;

    /** assignment operator */
    CaloEnergy &operator= (const CaloEnergy&);

    /** Accessor methods */

    /** energy determined from parametrization or not (measured) */
    CaloEnergy::EnergyLossType energyLossType (void) const;

    /** the Calo Muon Identification tag */
    unsigned short caloMuonIdTag() const { return m_caloMuonIdTag; }

    /** the calo Muon Identification likehood */
    double caloLRLikelihood() const { return  m_caloLRLikelihood; }

    /** the vector of detailed deposits in calo layers */
    const std::vector<DepositInCalo>& depositInCalo() const { return m_deposits; }
 
    /**  FSR Candidate Energy */
    float fsrCandidateEnergy() const { return m_fsrCandidateEnergy; }

    /** isolation ET in core deltaR */   
    float etCore() const { return m_etCore; }
  
    /** set methods */

    /** set is_parametrized or measured energy loss */
    void set_energyLossType(const CaloEnergy::EnergyLossType lossType) { m_energyLossType = lossType; }

    /** set the likelihood */
    void set_caloLRLikelihood ( const float likelihood ) { m_caloLRLikelihood = likelihood; }

    /** set the tag */
    void set_caloMuonIdTag ( unsigned short tag ) { m_caloMuonIdTag = tag; }

    /** set the detailed energy deposits in the calorimeter layers */
    void set_deposits( const std::vector<DepositInCalo> & deposits ) { m_deposits = deposits; }
    void set_deposits( std::vector<DepositInCalo>&& deposits ) { m_deposits = std::move(deposits); }

    /** insert a deposit */
    void insert_deposit ( const DepositInCalo& deposit ) { m_deposits.push_back( deposit ); }

    /**  FSR Candidate Energy */
    void set_fsrCandidateEnergy(const float fs) { m_fsrCandidateEnergy=fs; }

    /** isolation ET in core deltaR */ 
    void set_etCore ( const float etcore ) { m_etCore=etcore; }

    /** set measured energy loss*/
    void set_measEnergyLoss(const double deltaE, const double sigmaDeltaE) { m_deltaE_meas=deltaE; m_sigmaDeltaE_meas=sigmaDeltaE;}

    /** get measured energy loss*/
    double deltaEMeas() const { return m_deltaE_meas; }
    
    /** get measured energy loss error*/
    double sigmaDeltaEMeas() const { return m_sigmaDeltaE_meas; }
    
    /** set parametrised energy loss*/
    void set_paramEnergyLoss(const double deltaE, const double sigmaMinusDeltaE, const double sigmaPlusDeltaE) 
    { m_deltaE_param=deltaE; m_sigmaMinusDeltaE_param=sigmaMinusDeltaE; m_sigmaPlusDeltaE_param=sigmaPlusDeltaE;}
    
    /** get parametrised energy loss*/
    double deltaEParam() const { return m_deltaE_param; }
    
    /** get parametrised energy loss minus error*/
    double sigmaMinusDeltaEParam() const { return m_sigmaMinusDeltaE_param; }

    /** get parametrised energy loss plus error*/
    double sigmaPlusDeltaEParam() const { return m_sigmaPlusDeltaE_param; }
    
private:

    EnergyLossType m_energyLossType;
    float          m_caloLRLikelihood;
    unsigned short m_caloMuonIdTag;
    float          m_fsrCandidateEnergy;
    std::vector<DepositInCalo> m_deposits; 
    float          m_etCore;  // summed cell ET in core DeltaR 

    double         m_deltaE_param;
    double         m_sigmaMinusDeltaE_param;
    double         m_sigmaPlusDeltaE_param;
    double         m_deltaE_meas;
    double         m_sigmaDeltaE_meas;

};

inline CaloEnergy::EnergyLossType CaloEnergy::energyLossType (void) const
{ return m_energyLossType; }

inline CaloEnergy* CaloEnergy::clone() const
{ return new CaloEnergy(*this); }



#endif // MUONEVENT_CALOENERGY_H


