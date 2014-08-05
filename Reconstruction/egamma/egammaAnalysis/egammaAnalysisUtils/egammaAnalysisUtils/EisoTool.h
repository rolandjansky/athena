/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  ***************************************************************************
//  *                                                                         *
//  *   EisoTool - Providing a uniform electron isolation efficiency          *
//  *              across Et and Eta for Athena 17 electrons                  *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  *   Use:                                                                  *
//  *        There are 2 constructors:                                        *
//  *          * EisoTool() - If you are going to use both data and MC        *
//  *          * EisoTool(bool isMC) - If you know what your running over     *
//  *                                                                         *
//  *        If you choose the top constructor, then you will have to         *
//  *        specify if you are using MC for each EtconeXX function           *
//  *                                                                         *
//  *   Input varibale NPV:                                                   *
//  *        Please note the following for CaloIsoCorrection.h                *
//  *        nPV: number of primary vertices (corrections were                *
//  *        derived requiring >= 2 tracks/vertex in 2011 data)               *
//  *   ==================================================================    *
//  *   Example use case:                                                     *
//  *                                                                         *
//  *     bool isMC(true);                                                    *
//  *     EisoTool* m_eiso = new EisoTool(isMC);                              *
//  *     if( m_eiso->Eiso90Etcone20( ...... ) == 1 ){                        *
//  *       // Isolation passed -- carry on                                   *
//  *       .......                                                           *
//  *     }                                                                   *
//  *                                                                         *
//  *    // or                                                                *
//  *    EisoTool* m_eisoOther = new EisoTool();                              *
//  *    if( m_eisoOther->Eiso90Etcone20( ..... , your_isMC ) == 1 ){         *
//  *      // Isolation passed -- carry on                                    *
//  *       .......                                                           *
//  *     }                                                                   *
//  *                                                                         *
//  *     delete m_eiso;                                                      *
//  *     delete m_eisoOther;                                                 *
//  *                                                                         *
//  *   ==================================================================    *
//  *   Backwards compatability:                                              *
//  *        If you want the Athena 16 pT-leakage corrections                 *
//  *        for continuity studies, please do:                               *
//  *                                                                         *
//  *        EisoTool* m_eiso = new EisoTool()                                *
//  *        m_eiso->SetRel16();                                              *
//  *   ==================================================================    *
//  *                                                                         *
//  *   If you do not know if you are dealing with data or MC when you        *
//  *   instantiate EisoTool, but once running you don't want to always       *
//  *   pass the variable isMC, you can do:                                   *
//  *                                                                         *
//  *        m_eiso->SetData();                                               *
//  *        m_eiso->SetMC();                                                 *
//  *   ==================================================================    *
//  *                                                                         *
//  *   If you want a constant isolation cut for each eta bin that has been   *
//  *   calculated by summing over all Et bins then you can do:               *
//  *                                                                         *
//  *        m_eiso->SetSumEt();                                              *
//  *                                                                         *
//  *   Please note that this will not provide a uniform efficiency           *
//  *   over all Et and eta, but will remove any "spikes" you may see         *
//  *   around the isEM binning                                               *
//  *   ==================================================================    *
//  *                                                                         *
//  *        All functions return 1 if the electron is isolated               *
//  *        All functions return 0 if the electron is not isolated           *
//  *                                                                         *
//  *        Simply pass the variables into the functions to obtain           *
//  *        uniform efficiencies across Et and Eta                           *
//  *                                                                         *
//  *        Please use uncorrected Etcone20                                  *
//  *        Pile-up and pT-leakage corrections are done internally           *
//  *                                                                         *                                                               
//  *                                                                         *
//  *        Eiso90Etcone20 - 90% efficiency for Etcone20                     *
//  *        Eiso95Etcone20 - 95% efficiency for Etcone20                     *
//  *        Eiso98Etcone20 - 98% efficiency for Etcone20                     *
//  *                                                                         *
//  *        Eiso90Etcone30 - 90% efficiency for Etcone30                     *
//  *        Eiso95Etcone30 - 95% efficiency for Etcone30                     *
//  *        Eiso98Etcone30 - 98% efficiency for Etcone30                     *
//  *                                                                         *
//  *        Eiso90Ptcone20 - 90% efficiency for Ptcone20                     *
//  *        Eiso97Ptcone20 - 97% efficiency for Ptcone20                     *
//  *        Eiso98Ptcone20 - 98% efficiency for Ptcone20                     *
//  *        Eiso99Ptcone20 - 99% efficiency for Ptcone20                     *
//  *                                                                         *
//  *        Eiso90Ptcone30 - 90% efficiency for Ptcone30                     *
//  *        Eiso97Ptcone30 - 97% efficiency for Ptcone30                     *
//  *        Eiso98Ptcone30 - 98% efficiency for Ptcone30                     *
//  *        Eiso99Ptcone30 - 99% efficiency for Ptcone30                     *
//  *                                                                         *
//  *        Eiso90Ptcone40 - 90% efficiency for Ptcone40                     *
//  *        Eiso97Ptcone40 - 97% efficiency for Ptcone40                     *
//  *        Eiso98Ptcone40 - 98% efficiency for Ptcone40                     *
//  *        Eiso99Ptcone40 - 99% efficiency for Ptcone40                     *
//  *                                                                         *
//  *        + all functions with DivEt for relative isolation                *
//  *                                                                         *
//  ***************************************************************************/

#ifndef _EGAMMA_ATHENA17_EISOTOOL_H_
#define _EGAMMA_ATHENA17_EISOTOOL_H_

#include <vector>
#include <iostream>
#include <cmath>

#include "egammaAnalysisUtils/CaloIsoCorrection.h"

class EisoTool{
  public:
    EisoTool();
    explicit EisoTool(const bool& isMC);
    virtual ~EisoTool();

    inline void SetRel16() {m_rel16False_rel17True = false;}
    inline void SetRel17() {m_rel16False_rel17True = true;}
    inline void SetMC()    {m_isMC = true;}
    inline void SetData()  {m_isMC = false;}
    inline void SetIsEM()  {m_sumEtFalse_isEMTrue = true;}
    inline void SetSumEt() {m_sumEtFalse_isEMTrue = false;}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniform 90% Etcone20 isolation efficiency
    virtual unsigned int Eiso90Etcone20(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone20_Uncorrected,
                                        const bool& isMC);

    virtual unsigned int Eiso90Etcone20(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone20_Uncorrected) const;

    // Get cut value for Uniform 90% Etcone20 isolation efficiency
    double getCutValueEiso90Etcone20(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta) const;

    // Uniform 95% Etcone20 isolation efficiency
    virtual unsigned int Eiso95Etcone20(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone20_Uncorrected,
                                        const bool& isMC);

    virtual unsigned int Eiso95Etcone20(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone20_Uncorrected) const;

    // Uniform 98% Etcone20 isolation efficiency
    virtual unsigned int Eiso98Etcone20(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone20_Uncorrected,
                                        const bool& isMC);

    virtual unsigned int Eiso98Etcone20(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone20_Uncorrected) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniform 90% Etcone30 isolation efficiency
    virtual unsigned int Eiso90Etcone30(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone30_Uncorrected,
                                        const bool& isMC);

    virtual unsigned int Eiso90Etcone30(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone30_Uncorrected) const;

    // Uniform 95% Etcone30 isolation efficiency
    virtual unsigned int Eiso95Etcone30(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone30_Uncorrected,
                                        const bool& isMC);

    virtual unsigned int Eiso95Etcone30(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone30_Uncorrected) const;

    // Uniform 98% Etcone30 isolation efficiency
    virtual unsigned int Eiso98Etcone30(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone30_Uncorrected,
                                        const bool& isMC);

    virtual unsigned int Eiso98Etcone30(const unsigned int& NPV,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta,
                                        const double& Electron_Etas2,
                                        const double& Electron_Etap,
                                        const double& Electron_Etcone30_Uncorrected) const;


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 20 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniform 90% Ptcone20 isolation efficiency
    unsigned int Eiso90Ptcone20(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone20) const;                                        

    // Uniform 97% Ptcone20 isolation efficiency
    unsigned int Eiso97Ptcone20(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone20) const;

    // Uniform 98% Ptcone20 isolation efficiency
    unsigned int Eiso98Ptcone20(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone20) const;

     // Uniform 99% Ptcone20 isolation efficiency
    unsigned int Eiso99Ptcone20(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone20) const;


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 30 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniform 90% Ptcone30 isolation efficiency
    unsigned int Eiso90Ptcone30(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone30) const;

    // get cut value for Uniform 90% Ptcone30 isolation efficiency
    double getCutValueEiso90Ptcone30(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta) const;

    // Uniform 97% Ptcone30 isolation efficiency
    unsigned int Eiso97Ptcone30(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone30) const;

    // Uniform 98% Ptcone30 isolation efficiency
    unsigned int Eiso98Ptcone30(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone30) const;

     // Uniform 99% Ptcone30 isolation efficiency
    unsigned int Eiso99Ptcone30(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone30) const;


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 40 Isolation Functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniform 90% Ptcone40 isolation efficiency
    unsigned int Eiso90Ptcone40(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone40) const;                                

    // Uniform 97% Ptcone40 isolation efficiency
    unsigned int Eiso97Ptcone40(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone40) const;

    // Uniform 98% Ptcone40 isolation efficiency
    unsigned int Eiso98Ptcone40(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone40) const;

     // Uniform 99% Ptcone40 isolation efficiency
    unsigned int Eiso99Ptcone40(const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Ptcone40) const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 20 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////    

    // Uniform 90% Etcone20 / Et isolation efficiency
    virtual unsigned int Eiso90Etcone20DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone20_Uncorrected,
                                             const bool& isMC);

    virtual unsigned int Eiso90Etcone20DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone20_Uncorrected) const;

    // Uniform 95% Etcone20 / Et isolation efficiency
    virtual unsigned int Eiso95Etcone20DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone20_Uncorrected,
                                             const bool& isMC);

    virtual unsigned int Eiso95Etcone20DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone20_Uncorrected) const;

    // Uniform 98% Etcone20 / Et isolation efficiency
    virtual unsigned int Eiso98Etcone20DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone20_Uncorrected,
                                             const bool& isMC);

    virtual unsigned int Eiso98Etcone20DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone20_Uncorrected) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Etcone 30 / Et Isolation Functions ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////// 

    // Uniform 90% Etcone30 / Et isolation efficiency
    virtual unsigned int Eiso90Etcone30DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone30_Uncorrected,
                                             const bool& isMC);

    virtual unsigned int Eiso90Etcone30DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone30_Uncorrected) const;

    // Uniform 95% Etcone30 / Et isolation efficiency
    virtual unsigned int Eiso95Etcone30DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone30_Uncorrected,
                                             const bool& isMC);

    virtual unsigned int Eiso95Etcone30DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone30_Uncorrected) const;

    // Uniform 98% Etcone30 / Et isolation efficiency
    virtual unsigned int Eiso98Etcone30DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone30_Uncorrected,
                                             const bool& isMC);

    virtual unsigned int Eiso98Etcone30DivEt(const unsigned int& NPV,
                                             const double& Electron_Cluster_E,
                                             const double& Electron_Track_Eta,
                                             const double& Electron_Cluster_Eta,
                                             const double& Electron_Etas2,
                                             const double& Electron_Etap,
                                             const double& Electron_Etcone30_Uncorrected) const;
    

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 20 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    // Uniform 90% Ptcone20 / Et isolation efficiency
    unsigned int Eiso90Ptcone20DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone20) const;                                             

    // Uniform 97% Ptcone20 / Et isolation efficiency
    unsigned int Eiso97Ptcone20DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone20) const;

    // Uniform 98% Ptcone20 / Et isolation efficiency
    unsigned int Eiso98Ptcone20DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone20) const;

     // Uniform 99% Ptcone20 / Et isolation efficiency
    unsigned int Eiso99Ptcone20DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone20) const;


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 30 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////     

    // Uniform 90% Ptcone30 / Et isolation efficiency
    unsigned int Eiso90Ptcone30DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone30) const;
                                     
    // Uniform 97% Ptcone30 / Et isolation efficiency
    unsigned int Eiso97Ptcone30DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone30) const;

    // Uniform 98% Ptcone30 / Et isolation efficiency
    unsigned int Eiso98Ptcone30DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone30) const;

     // Uniform 99% Ptcone30 / Et isolation efficiency
    unsigned int Eiso99Ptcone30DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone30) const;


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   Ptcone 40 / Et Isolation Functions ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////     

    // Uniform 90% Ptcone40 / Et isolation efficiency
    unsigned int Eiso90Ptcone40DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone40) const;
                                     
    // Uniform 97% Ptcone40 / Et isolation efficiency
    unsigned int Eiso97Ptcone40DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone40) const;

    // Uniform 98% Ptcone40 / Et isolation efficiency
    unsigned int Eiso98Ptcone40DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone40) const;

     // Uniform 99% Ptcone40 / Et isolation efficiency
    unsigned int Eiso99Ptcone40DivEt(const double& Electron_Cluster_E,
                                     const double& Electron_Track_Eta,
                                     const double& Electron_Cluster_Eta,
                                     const double& Ptcone40) const;

                                

    double Etcone20Corrected(const unsigned int& NPV,
                             const double& Electron_Cluster_E,
                             const double& Electron_Cluster_Eta,
                             const double& Electron_Etas2,
                             const double& Electron_Etap,
                             const double& Electron_Etcone20_Uncorrected) const;

    double Etcone30Corrected(const unsigned int& NPV,
                             const double& Electron_Cluster_E,
                             const double& Electron_Cluster_Eta,
                             const double& Electron_Etas2,
                             const double& Electron_Etap,
                             const double& Electron_Etcone30_Uncorrected) const;
    
  protected:

    void Init();
    void Init_Bins();

    void Init_Etcone20_iso90();
    void Init_Etcone20_iso95();
    void Init_Etcone20_iso98();
    
    void Init_Etcone30_iso90();
    void Init_Etcone30_iso95();
    void Init_Etcone30_iso98();

    void Init_Etcone20_iso90_rel17();
    void Init_Etcone20_iso95_rel17();
    void Init_Etcone20_iso98_rel17();

    void Init_Etcone30_iso90_rel17();
    void Init_Etcone30_iso95_rel17();
    void Init_Etcone30_iso98_rel17();    

    void Init_Ptcone20_iso90();
    void Init_Ptcone20_iso97();
    void Init_Ptcone20_iso98();
    void Init_Ptcone20_iso99();

    void Init_Ptcone30_iso90();
    void Init_Ptcone30_iso97();
    void Init_Ptcone30_iso98();
    void Init_Ptcone30_iso99();

    void Init_Ptcone40_iso90();
    void Init_Ptcone40_iso97();
    void Init_Ptcone40_iso98();
    void Init_Ptcone40_iso99();


    void Init_Etcone20DivEt_iso90();
    void Init_Etcone20DivEt_iso95();
    void Init_Etcone20DivEt_iso98();

    void Init_Etcone30DivEt_iso90();
    void Init_Etcone30DivEt_iso95();
    void Init_Etcone30DivEt_iso98();

    void Init_Etcone20DivEt_iso90_rel17();
    void Init_Etcone20DivEt_iso95_rel17();
    void Init_Etcone20DivEt_iso98_rel17();

    void Init_Etcone30DivEt_iso90_rel17();
    void Init_Etcone30DivEt_iso95_rel17();
    void Init_Etcone30DivEt_iso98_rel17();    

    void Init_Ptcone20DivEt_iso90();
    void Init_Ptcone20DivEt_iso97();
    void Init_Ptcone20DivEt_iso98();
    void Init_Ptcone20DivEt_iso99();

    void Init_Ptcone30DivEt_iso90();
    void Init_Ptcone30DivEt_iso97();
    void Init_Ptcone30DivEt_iso98();
    void Init_Ptcone30DivEt_iso99();

    void Init_Ptcone40DivEt_iso90();
    void Init_Ptcone40DivEt_iso97();
    void Init_Ptcone40DivEt_iso98();
    void Init_Ptcone40DivEt_iso99();


    void Init_Etcone20_iso90_rel17_sumEt();
    void Init_Etcone20_iso95_rel17_sumEt();
    void Init_Etcone20_iso98_rel17_sumEt();

    void Init_Etcone30_iso90_rel17_sumEt();
    void Init_Etcone30_iso95_rel17_sumEt();
    void Init_Etcone30_iso98_rel17_sumEt();

    void Init_Ptcone20_iso90_sumEt();
    void Init_Ptcone20_iso97_sumEt();
    void Init_Ptcone20_iso98_sumEt();
    void Init_Ptcone20_iso99_sumEt();

    void Init_Ptcone30_iso90_sumEt();
    void Init_Ptcone30_iso97_sumEt();
    void Init_Ptcone30_iso98_sumEt();
    void Init_Ptcone30_iso99_sumEt();

    void Init_Ptcone40_iso90_sumEt();
    void Init_Ptcone40_iso97_sumEt();
    void Init_Ptcone40_iso98_sumEt();
    void Init_Ptcone40_iso99_sumEt();

    void Init_Etcone20DivEt_iso90_rel17_sumEt();
    void Init_Etcone20DivEt_iso95_rel17_sumEt();
    void Init_Etcone20DivEt_iso98_rel17_sumEt();

    void Init_Etcone30DivEt_iso90_rel17_sumEt();
    void Init_Etcone30DivEt_iso95_rel17_sumEt();
    void Init_Etcone30DivEt_iso98_rel17_sumEt();

    void Init_Ptcone20DivEt_iso90_sumEt();
    void Init_Ptcone20DivEt_iso97_sumEt();
    void Init_Ptcone20DivEt_iso98_sumEt();
    void Init_Ptcone20DivEt_iso99_sumEt();

    void Init_Ptcone30DivEt_iso90_sumEt();
    void Init_Ptcone30DivEt_iso97_sumEt();
    void Init_Ptcone30DivEt_iso98_sumEt();
    void Init_Ptcone30DivEt_iso99_sumEt();

    void Init_Ptcone40DivEt_iso90_sumEt();
    void Init_Ptcone40DivEt_iso97_sumEt();
    void Init_Ptcone40DivEt_iso98_sumEt();
    void Init_Ptcone40DivEt_iso99_sumEt();    



    

    unsigned int FindEtBin(const double Et) const;
    unsigned int FindEtaBin(const double Eta) const;
    virtual double CheckValidAndReturn(std::vector<std::vector<double> >* cutVec,const unsigned int et, const unsigned int eta) const;
    virtual double CheckValidAndReturn(std::vector<double>* cutVec,const unsigned int eta) const;
    virtual double CheckValidAndReturn(const std::vector<double>& cutVec,const unsigned int eta) const;

    double ElectronEt(const double& Electron_Cluster_E,const double& Electron_Track_Eta) const;
                           

  private:
    double m_GeV;
    bool m_isMC;
    bool m_rel16False_rel17True;
    bool m_sumEtFalse_isEMTrue;

    std::vector<double>* m_etBins;
    std::vector<double>* m_etaBins;

    std::vector<std::vector<double> >* m_etcone20_iso90;
    std::vector<std::vector<double> >* m_etcone20_iso95;
    std::vector<std::vector<double> >* m_etcone20_iso98;

    std::vector<std::vector<double> >* m_etcone30_iso90;
    std::vector<std::vector<double> >* m_etcone30_iso95;
    std::vector<std::vector<double> >* m_etcone30_iso98;
    
    std::vector<std::vector<double> >* m_ptcone20_iso90;
    std::vector<std::vector<double> >* m_ptcone20_iso97;
    std::vector<std::vector<double> >* m_ptcone20_iso98;
    std::vector<std::vector<double> >* m_ptcone20_iso99;

    std::vector<std::vector<double> >* m_ptcone30_iso90;    
    std::vector<std::vector<double> >* m_ptcone30_iso97;
    std::vector<std::vector<double> >* m_ptcone30_iso98;
    std::vector<std::vector<double> >* m_ptcone30_iso99;

    std::vector<std::vector<double> >* m_ptcone40_iso90;    
    std::vector<std::vector<double> >* m_ptcone40_iso97;
    std::vector<std::vector<double> >* m_ptcone40_iso98;
    std::vector<std::vector<double> >* m_ptcone40_iso99;


    std::vector<std::vector<double> >* m_etcone20DivEt_iso90;
    std::vector<std::vector<double> >* m_etcone20DivEt_iso95;
    std::vector<std::vector<double> >* m_etcone20DivEt_iso98;

    std::vector<std::vector<double> >* m_etcone30DivEt_iso90;
    std::vector<std::vector<double> >* m_etcone30DivEt_iso95;
    std::vector<std::vector<double> >* m_etcone30DivEt_iso98;

    std::vector<std::vector<double> >* m_ptcone20DivEt_iso90;
    std::vector<std::vector<double> >* m_ptcone20DivEt_iso97;
    std::vector<std::vector<double> >* m_ptcone20DivEt_iso98;
    std::vector<std::vector<double> >* m_ptcone20DivEt_iso99;

    std::vector<std::vector<double> >* m_ptcone30DivEt_iso90;    
    std::vector<std::vector<double> >* m_ptcone30DivEt_iso97;
    std::vector<std::vector<double> >* m_ptcone30DivEt_iso98;
    std::vector<std::vector<double> >* m_ptcone30DivEt_iso99;

    std::vector<std::vector<double> >* m_ptcone40DivEt_iso90;    
    std::vector<std::vector<double> >* m_ptcone40DivEt_iso97;
    std::vector<std::vector<double> >* m_ptcone40DivEt_iso98;
    std::vector<std::vector<double> >* m_ptcone40DivEt_iso99;

    // Rel 17 CaloIsoCorrection
    std::vector<std::vector<double> >* m_etcone20_iso90_rel17;
    std::vector<std::vector<double> >* m_etcone20_iso95_rel17;
    std::vector<std::vector<double> >* m_etcone20_iso98_rel17;

    std::vector<std::vector<double> >* m_etcone30_iso90_rel17;
    std::vector<std::vector<double> >* m_etcone30_iso95_rel17;
    std::vector<std::vector<double> >* m_etcone30_iso98_rel17;

    std::vector<std::vector<double> >* m_etcone20DivEt_iso90_rel17;
    std::vector<std::vector<double> >* m_etcone20DivEt_iso95_rel17;
    std::vector<std::vector<double> >* m_etcone20DivEt_iso98_rel17;

    std::vector<std::vector<double> >* m_etcone30DivEt_iso90_rel17;
    std::vector<std::vector<double> >* m_etcone30DivEt_iso95_rel17;
    std::vector<std::vector<double> >* m_etcone30DivEt_iso98_rel17;

    // Eta only vectors - isolation per eta bin summed over all Et
    std::vector<double>* m_etcone20_iso90_rel17_sumEt;
    std::vector<double>* m_etcone20_iso95_rel17_sumEt;
    std::vector<double>* m_etcone20_iso98_rel17_sumEt;

    std::vector<double>* m_etcone30_iso90_rel17_sumEt;
    std::vector<double>* m_etcone30_iso95_rel17_sumEt;
    std::vector<double>* m_etcone30_iso98_rel17_sumEt;

    std::vector<double>* m_ptcone20_iso90_sumEt;
    std::vector<double>* m_ptcone20_iso97_sumEt;
    std::vector<double>* m_ptcone20_iso98_sumEt;
    std::vector<double>* m_ptcone20_iso99_sumEt;

    std::vector<double>* m_ptcone30_iso90_sumEt;
    std::vector<double>* m_ptcone30_iso97_sumEt;
    std::vector<double>* m_ptcone30_iso98_sumEt;
    std::vector<double>* m_ptcone30_iso99_sumEt;

    std::vector<double>* m_ptcone40_iso90_sumEt;
    std::vector<double>* m_ptcone40_iso97_sumEt;
    std::vector<double>* m_ptcone40_iso98_sumEt;
    std::vector<double>* m_ptcone40_iso99_sumEt;

    std::vector<double>* m_etcone20DivEt_iso90_rel17_sumEt;
    std::vector<double>* m_etcone20DivEt_iso95_rel17_sumEt;
    std::vector<double>* m_etcone20DivEt_iso98_rel17_sumEt;

    std::vector<double>* m_etcone30DivEt_iso90_rel17_sumEt;
    std::vector<double>* m_etcone30DivEt_iso95_rel17_sumEt;
    std::vector<double>* m_etcone30DivEt_iso98_rel17_sumEt;    

    std::vector<double>* m_ptcone20DivEt_iso90_sumEt;
    std::vector<double>* m_ptcone20DivEt_iso97_sumEt;
    std::vector<double>* m_ptcone20DivEt_iso98_sumEt;
    std::vector<double>* m_ptcone20DivEt_iso99_sumEt;

    std::vector<double>* m_ptcone30DivEt_iso90_sumEt;
    std::vector<double>* m_ptcone30DivEt_iso97_sumEt;
    std::vector<double>* m_ptcone30DivEt_iso98_sumEt;
    std::vector<double>* m_ptcone30DivEt_iso99_sumEt;

    std::vector<double>* m_ptcone40DivEt_iso90_sumEt;
    std::vector<double>* m_ptcone40DivEt_iso97_sumEt;
    std::vector<double>* m_ptcone40DivEt_iso98_sumEt;
    std::vector<double>* m_ptcone40DivEt_iso99_sumEt;

};
#endif

