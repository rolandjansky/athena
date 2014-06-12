/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PanTauDetails.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENT_PANTAUDETAILS_EFLOWREC_H
#define PANTAUEVENT_PANTAUDETAILS_EFLOWREC_H



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
    PanTau specific implementation of PanTau::PanTauDetails for eflowRec
    @author limbach@physik.uni-bonn.de
    */

class PanTauDetails_eflowRec: public PanTau::PanTauDetails {

public:
    
    
    enum DetailsEnumeration {
        t_eflowRec_Basic_NPi0NeutConsts,
        t_eflowRec_Basic_NNeutralConsts,
        t_eflowRec_Charged_HLV_SumPt,
        t_eflowRec_Charged_Ratio_EtOverEtAllConsts,
        t_eflowRec_Neutral_HLV_SumM,
        t_eflowRec_Neutral_PID_BDTValues_EtSort_1,
        t_eflowRec_Neutral_PID_BDTValues_BDTSort_2,
        t_eflowRec_Neutral_Ratio_EtOverEtAllConsts,
        t_eflowRec_Neutral_Mean_DRToLeading_WrtEtAllConsts,
        t_eflowRec_Combined_DeltaR1stNeutralTo1stCharged
    };
    
    /** Default Constructor for POOL. Do not use! */
    PanTauDetails_eflowRec();
    /** Copy Constructor */
    PanTauDetails_eflowRec(const PanTauDetails_eflowRec& rhs);
    /** Assignment operator */
    PanTauDetails_eflowRec& operator=(const PanTauDetails_eflowRec& rhs);
    
    
    /**
     * THE constructor for generalized pantau:
     * the details ONLY know about a seed and correspond to its features!
     * 
     * PanTauDetails does not take ownership of the seed)
     */
    PanTauDetails_eflowRec(PanTau::PanTauSeed* inputSeed);
    
    /** Destructor */
    virtual ~PanTauDetails_eflowRec();
    
    
    virtual const std::string& className() const;
    
    
    void addGeneralDetails(PanTau::PanTauDetails* pantauDetails);
    
    /** the details to be accessable in the container */
    
    bool setScalarDetailByEnum(double newValue, int detEnum_index);
    bool setVectorDetailByEnum(std::vector<double> newVec, int detEnum_index);
    
    
    
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
    
    
protected:
    
    static std::string s_className;
    
    
    /** These members will be persistified:*/
    
    /** the details to be accessable in the container */
    
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
    
    
};


}

#endif // PANTAUEVENT_PANTAUDETAILS_EFLOWREC_H

