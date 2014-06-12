/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PanTauDetails.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENT_PANTAUDETAILS_CELLBASED_H
#define PANTAUEVENT_PANTAUDETAILS_CELLBASED_H



#include <iostream>
#include <bitset>

#include "tauEvent/TauDetails.h"
#include "PanTauEvent/PanTauSeed.h"
#include "PanTauEvent/TauSeed.h"
#include "PanTauEvent/PanTauDetails.h"

class MsgStream;

namespace PanTau {
class TauFeature;


/** @class PanTauDetails
    PanTau specific implementation of PanTau::PanTauDetails for CellBased
    @author limbach@physik.uni-bonn.de
    */

class PanTauDetails_CellBased: public PanTau::PanTauDetails {

public:
    
    
    enum DetailsEnumeration {
        t_CellBased_Basic_NNeutralConsts,
        t_CellBased_Charged_JetMoment_EtDRxTotalEt,
        t_CellBased_Charged_StdDev_Et_WrtEtAllConsts,
        t_CellBased_Neutral_HLV_SumM,
        t_CellBased_Neutral_PID_BDTValues_BDTSort_1,
        t_CellBased_Neutral_PID_BDTValues_BDTSort_2,
        t_CellBased_Neutral_Ratio_1stBDTEtOverEtAllConsts,
        t_CellBased_Neutral_Ratio_EtOverEtAllConsts,
        t_CellBased_Neutral_Shots_NPhotonsInSeed,
        t_CellBased_Combined_DeltaR1stNeutralTo1stCharged
    };
    
    /** Default Constructor for POOL. Do not use! */
    PanTauDetails_CellBased();
    /** Copy Constructor */
    PanTauDetails_CellBased(const PanTauDetails_CellBased& rhs);
    /** Assignment operator */
    PanTauDetails_CellBased& operator=(const PanTauDetails_CellBased& rhs);
    
    
    /**
     * THE constructor for generalized pantau:
     * the details ONLY know about a seed and correspond to its features!
     * 
     * PanTauDetails does not take ownership of the seed)
     */
    PanTauDetails_CellBased(PanTau::PanTauSeed* inputSeed);
    
    /** Destructor */
    virtual ~PanTauDetails_CellBased();

    
    virtual const std::string& className() const;
    
    
    void addGeneralDetails(PanTau::PanTauDetails* pantauDetails);
    
    /** the details to be accessable in the container */
    
    bool setScalarDetailByEnum(double newValue, int detEnum_index);
    bool setVectorDetailByEnum(std::vector<double> newVec, int detEnum_index);
    
    
    
    //! =======================================================================
    //! ID Variables
    //! =======================================================================
    
    
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

    static std::string s_className;
    
    
    /** These members will be persistified:*/
    
    /** the details to be accessable in the container */
    
    //! ======================================================================
    //! ID Variables 
    //! ======================================================================
    
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


}

#endif // PANTAUEVENT_PANTAUDETAILS_H

