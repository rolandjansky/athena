/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PanTauDetails.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENT_PANTAUDETAILS_H
#define PANTAUEVENT_PANTAUDETAILS_H



#include <iostream>
#include <bitset>

#include "tauEvent/TauDetails.h"
#include "PanTauEvent/PanTauSeed.h"
#include "PanTauEvent/TauSeed.h"
class MsgStream;

namespace PanTau {
class TauFeature;


/** @class PanTauDetails
    PanTau specific implementation of Analysis::TauDetails
    @author limbach@physik.uni-bonn.de
    @author Sebastian.Fleischmann@cern.ch
    */

class PanTauDetails: public Analysis::TauDetails {

public:
    
    
    enum DetailsEnumeration {
        t_isPanTauCandidate, //!< tells you whether this tauRec seed is considered as candidate for PanTau
        
        t_RecoMode_SubAlg,
        t_RecoMode_PanTau,
        
        t_BDTValue_1p0n_vs_1p1n,
        t_BDTValue_1p1n_vs_1pXn,
        t_BDTValue_3p0n_vs_3pXn,
        
        t_pantau_proto_pt,
        t_pantau_proto_eta,
        t_pantau_proto_phi,
        t_pantau_proto_m,
        
        t_pantau_final_pt,
        t_pantau_final_eta,
        t_pantau_final_phi,
        t_pantau_final_m,
        
        t_ChargedEFOs_pt,
        t_ChargedEFOs_eta,
        t_ChargedEFOs_phi,
        t_ChargedEFOs_m,
        
        t_NeutralEFOs_pt,
        t_NeutralEFOs_eta,
        t_NeutralEFOs_phi,
        t_NeutralEFOs_m,
        
        t_Pi0NeutEFOs_pt,
        t_Pi0NeutEFOs_eta,
        t_Pi0NeutEFOs_phi,
        t_Pi0NeutEFOs_m

    };
    
    /** Default Constructor for POOL. Do not use! */
    PanTauDetails();
    /** Copy Constructor */
    PanTauDetails(const PanTauDetails& rhs);
    /** Assignment operator */
    PanTauDetails& operator=(const PanTauDetails& rhs);
    
    
    /**
     * THE constructor for generalized pantau:
     * the details ONLY know about a seed and correspond to its features!
     * 
     * PanTauDetails does not take ownership of the seed)
     */
    PanTauDetails(PanTau::PanTauSeed* inputSeed);
    
    
    /**
    * Full constructor. NOTE: delete it
    *
    * The TauSeed object passed in does NOT belong to the this object!
    *
    * @param[in] seed pointer to TauSeed (PanTauDetails does not take ownership)
    * @param[in] features pointer to TauFeature (PanTauDetails takes ownership, do not delete yourself!)
    */
    /*
    PanTauDetails(
        const PanTau::TauSeed                               *seed,
        const PanTau::TauFeature                            *features = 0
        );
    */

    /** Destructor */
    virtual ~PanTauDetails();

    
    //NOTE: new way of retrieving the features:
    PanTau::TauFeature* getFeatures() const;
    
    //NOTE: new way of retreiving the seed
    PanTau::PanTauSeed* getPanTauSeed() const;
    
    
    //TODO: delete these two functions
    /** returns PanTau::TauFeature (own features if we have features, otherwise return features of the seed) */
    const PanTau::TauFeature* features() const;
    /** returns PanTau::TauSeed the TauJet was created from*/
    const PanTau::TauSeed* seed() const;


    virtual const std::string& className() const;
    
    
    /** the details to be accessable in the container */
    
    virtual bool setScalarDetailByEnum(double newValue, int detEnum_index);
    virtual bool setVectorDetailByEnum(std::vector<double> newVec, int detEnum_index);
    

    //! =============================================================
    //! Basic PanTau Variables (validity, reco mode & momenta)
    //! =============================================================
    int     isPanTauCandidate() const;
    std::string inputAlgName() const;
    int     RecoModeSubAlg() const;
    int     RecoModePanTau() const;
    double  BDTValue_1p0n_vs_1p1n() const;
    double  BDTValue_1p1n_vs_1pXn() const;
    double  BDTValue_3p0n_vs_3pXn() const;
    double  pantau_proto_pt() const;
    double  pantau_proto_eta() const;
    double  pantau_proto_phi() const;
    double  pantau_proto_m() const;
    double  pantau_final_pt() const;
    double  pantau_final_eta() const;
    double  pantau_final_phi() const;
    double  pantau_final_m() const;
    std::vector<double> ChargedEFOs_pt() const;
    std::vector<double> ChargedEFOs_eta() const;
    std::vector<double> ChargedEFOs_phi() const;
    std::vector<double> ChargedEFOs_m() const;
    std::vector<double> NeutralEFOs_pt() const;
    std::vector<double> NeutralEFOs_eta() const;
    std::vector<double> NeutralEFOs_phi() const;
    std::vector<double> NeutralEFOs_m() const;
    std::vector<double> Pi0NeutEFOs_pt() const;
    std::vector<double> Pi0NeutEFOs_eta() const;
    std::vector<double> Pi0NeutEFOs_phi() const;
    std::vector<double> Pi0NeutEFOs_m() const;
    
    void  set_isPanTauCandidate(int isPTC );
    void  set_InputAlgName(std::string inAlg);
    void  set_RecoModeSubAlg( int newValue);
    void  set_RecoModePanTau( int newValue);
    void  set_BDTValue_1p0n_vs_1p1n( double newValue);
    void  set_BDTValue_1p1n_vs_1pXn( double newValue);
    void  set_BDTValue_3p0n_vs_3pXn( double newValue);
    void  set_pantau_proto_pt( double newValue);
    void  set_pantau_proto_eta( double newValue);
    void  set_pantau_proto_phi( double newValue);
    void  set_pantau_proto_m( double newValue);
    void  set_pantau_final_pt( double newValue);
    void  set_pantau_final_eta( double newValue);
    void  set_pantau_final_phi( double newValue);
    void  set_pantau_final_m( double newValue);
    void  set_ChargedEFOs_pt( std::vector<double> newValue);
    void  set_ChargedEFOs_eta( std::vector<double> newValue);
    void  set_ChargedEFOs_phi( std::vector<double> newValue);
    void  set_ChargedEFOs_m( std::vector<double> newValue);
    void  set_NeutralEFOs_pt( std::vector<double> newValue);
    void  set_NeutralEFOs_eta( std::vector<double> newValue);
    void  set_NeutralEFOs_phi( std::vector<double> newValue);
    void  set_NeutralEFOs_m( std::vector<double> newValue);
    void  set_Pi0NeutEFOs_pt( std::vector<double> newValue);
    void  set_Pi0NeutEFOs_eta( std::vector<double> newValue);
    void  set_Pi0NeutEFOs_phi( std::vector<double> newValue);
    void  set_Pi0NeutEFOs_m( std::vector<double> newValue);
    
    
    //! =======================================================================
    //! ID Variables
    //! =======================================================================
    
    
    //! eflowRec
    
    int         eflowRec_Basic_NPi0NeutConsts() const;
    int         eflowRec_Basic_NNeutralConsts() const;
    double      eflowRec_Charged_HLV_SumPt() const;
    double      eflowRec_Charged_Ratio_EtOverEtAllConsts() const;
    double      eflowRec_Neutral_HLV_SumM() const;
    double      eflowRec_Neutral_PID_BDTValues_EtSort_1() const;
    double      eflowRec_Neutral_PID_BDTValues_BDTSort_2() const;
    double      eflowRec_Neutral_Ratio_EtOverEtAllConsts() const;
    double      eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts() const;
    double      eflowRec_Combined_DeltaR1stNeutralTo1stCharged() const;
    
    void        set_eflowRec_Basic_NPi0NeutConsts(double newValue) ;
    void        set_eflowRec_Basic_NNeutralConsts(double newValue) ;
    void        set_eflowRec_Charged_HLV_SumPt(double newValue) ;
    void        set_eflowRec_Charged_Ratio_EtOverEtAllConsts(double newValue) ;
    void        set_eflowRec_Neutral_HLV_SumM(double newValue) ;
    void        set_eflowRec_Neutral_PID_BDTValues_EtSort_1(double newValue) ;
    void        set_eflowRec_Neutral_PID_BDTValues_BDTSort_2(double newValue) ;
    void        set_eflowRec_Neutral_Ratio_EtOverEtAllConsts(double newValue) ;
    void        set_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts(double newValue) ;
    void        set_eflowRec_Combined_DeltaR1stNeutralTo1stCharged(double newValue) ;
    
    
    //! CellBased
    
    int        CellBased_Basic_NNeutralConsts() const;
    double     CellBased_Charged_JetMoment_EtDRxTotalEt() const;
    double     CellBased_Charged_StdDev_Et_WrtEtAllConsts() const;
    double     CellBased_Neutral_HLV_SumM() const;
    double     CellBased_Neutral_PID_BDTValues_BDTSort_1() const;
    double     CellBased_Neutral_PID_BDTValues_BDTSort_2() const;
    double     CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts() const;
    double     CellBased_Neutral_Ratio_EtOverEtAllConsts() const;
    double     CellBased_Neutral_Shots_NPhotonsInSeed() const;
    double     CellBased_Combined_DeltaR1stNeutralTo1stCharged() const;
    
    void     set_CellBased_Basic_NNeutralConsts(double newValue);
    void     set_CellBased_Charged_JetMoment_EtDRxTotalEt(double newValue);
    void     set_CellBased_Charged_StdDev_Et_WrtEtAllConsts(double newValue);
    void     set_CellBased_Neutral_HLV_SumM(double newValue);
    void     set_CellBased_Neutral_PID_BDTValues_BDTSort_1(double newValue);
    void     set_CellBased_Neutral_PID_BDTValues_BDTSort_2(double newValue);
    void     set_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts(double newValue);
    void     set_CellBased_Neutral_Ratio_EtOverEtAllConsts(double newValue);
    void     set_CellBased_Neutral_Shots_NPhotonsInSeed(double newValue);
    void     set_CellBased_Combined_DeltaR1stNeutralTo1stCharged(double newValue);
    
protected:

    /** Only for transient object:*/
    PanTau::PanTauSeed*    m_PanTauSeed;
    
    
    /** the features of the details */
    const TauFeature*   m_features;
    /** the features of the seed */
    const TauSeed*      m_seed;

    static std::string s_className;
    
    
    /** These members will be persistified:*/
    
    /** the details to be accessable in the container */
    
    //! ======================================================================
    //! Basic PanTau Variables (validity, reco mode & momenta)
    //! ======================================================================
    
    int         m_isPanTauCandidate; //!< tells you whether this tauRec seed is considered as candidate for PanTau
    std::string m_Name_InputAlg;
    int     m_RecoMode_SubAlg;
    int     m_RecoMode_PanTau;
    double  m_BDTValue_1p0n_vs_1p1n;
    double  m_BDTValue_1p1n_vs_1pXn;
    double  m_BDTValue_3p0n_vs_3pXn;
    double  m_pantau_proto_pt;
    double  m_pantau_proto_eta;
    double  m_pantau_proto_phi;
    double  m_pantau_proto_m;
    double  m_pantau_final_pt;
    double  m_pantau_final_eta;
    double  m_pantau_final_phi;
    double  m_pantau_final_m;
    
    std::vector<double> m_ChargedEFOs_pt;
    std::vector<double> m_ChargedEFOs_eta;
    std::vector<double> m_ChargedEFOs_phi;
    std::vector<double> m_ChargedEFOs_m;
    
    std::vector<double> m_NeutralEFOs_pt;
    std::vector<double> m_NeutralEFOs_eta;
    std::vector<double> m_NeutralEFOs_phi;
    std::vector<double> m_NeutralEFOs_m;
    
    std::vector<double> m_Pi0NeutEFOs_pt;
    std::vector<double> m_Pi0NeutEFOs_eta;
    std::vector<double> m_Pi0NeutEFOs_phi;
    std::vector<double> m_Pi0NeutEFOs_m;
    
    
    //! ======================================================================
    //! ID Variables 
    //! ======================================================================
    
    int     m_eflowRec_Basic_NPi0NeutConsts;
    int     m_eflowRec_Basic_NNeutralConsts;
    double  m_eflowRec_Charged_HLV_SumPt;
    double  m_eflowRec_Charged_Ratio_EtOverEtAllConsts;
    double  m_eflowRec_Neutral_HLV_SumM;
    double  m_eflowRec_Neutral_PID_BDTValues_EtSort_1;
    double  m_eflowRec_Neutral_PID_BDTValues_BDTSort_2;
    double  m_eflowRec_Neutral_Ratio_EtOverEtAllConsts;
    double  m_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts;
    double  m_eflowRec_Combined_DeltaR1stNeutralTo1stCharged;
    
    int     m_CellBased_Basic_NNeutralConsts;
    double  m_CellBased_Charged_JetMoment_EtDRxTotalEt;
    double  m_CellBased_Charged_StdDev_Et_WrtEtAllConsts;
    double  m_CellBased_Neutral_HLV_SumM;
    double  m_CellBased_Neutral_PID_BDTValues_BDTSort_1;
    double  m_CellBased_Neutral_PID_BDTValues_BDTSort_2;
    double  m_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts;
    double  m_CellBased_Neutral_Ratio_EtOverEtAllConsts;
    double  m_CellBased_Neutral_Shots_NPhotonsInSeed;
    double  m_CellBased_Combined_DeltaR1stNeutralTo1stCharged;
    
    

};

//NOTE: new way of retreiving the seed
inline PanTau::PanTauSeed* PanTau::PanTauDetails::getPanTauSeed() const {
    return m_PanTauSeed;
}

//NOTE: new way of retreiving the features
inline PanTau::TauFeature* PanTau::PanTauDetails::getFeatures() const {
    return m_PanTauSeed->getFeatures();
}


inline const PanTau::TauSeed* PanTau::PanTauDetails::seed() const
{ 
    return m_seed;
}


inline const PanTau::TauFeature* PanTau::PanTauDetails::features() const { 
    // return own features if we have features, otherwise return features of the seed:
    if(m_features != 0) return m_features;
    return m_seed->features();
}

}

#endif // PANTAUEVENT_PANTAUDETAILS_H

