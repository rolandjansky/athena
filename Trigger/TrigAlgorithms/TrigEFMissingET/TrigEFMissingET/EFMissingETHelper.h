/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
 * @Project:  HLT, PESA algorithms
 * @Package:  Trigger/TrigAlgorithms/TrigEFMissingET
 * @class:    TrigEFMissingEtHelper, TrigEFMissingEtComponent
 *
 * @brief:    Transient-only helper class (companion to TrigMissingET)
 *
 * @author  Diego Casadei
 * File and Version Information: 
 * $Id: EFMissingETHelper.h,v 1.4 2009-02-13 15:05:13 venkat Exp $ 
 ********************************************************************
 **/

#ifndef TRIGEFMISSINGET_EFMISSINGETHELPER_H
#define TRIGEFMISSINGET_EFMISSINGETHELPER_H

#include "CaloGeoHelpers/CaloSampling.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <string>

/** single component **/

class TrigEFMissingEtComponent {
  public: // all attributes are public: like a struct
    enum Component 
    { 
        PreSamplB, EMB1     , EMB2     , EMB3     ,     // LAr barrel
        PreSamplE, EME1     , EME2     , EME3     ,     // LAr EM endcap 
        HEC0     , HEC1     , HEC2     , HEC3     ,     // Hadronic end cap cal.
        TileBar0 , TileBar1 , TileBar2 ,                // Tile barrel
        TileGap1 , TileGap2 , TileGap3 ,                // Tile gap (ITC & scint)
        TileExt0 , TileExt1 , TileExt2 ,                // Tile extended barrel
        FCalEM   , FCalHad1 , FCalHad2 ,                // Forward cal endcap
        TCLCW    ,                                      // Topo. clusters Had
        TCLCWB1  , TCLCWB2  ,                           // pos. and neg. eta barrel 
        TCLCWE1  , TCLCWE2  ,                           // pos. and neg. eta endcap 
        TCEM     ,                                      // Topo. clusters EM
        TCEMB1   , TCEMB2   ,                           // pos. and neg. eta barrel 
        TCEME1   , TCEME2   ,                           // pos. and neg. eta endcap 
        JET      ,                                      // Jet 
        JETB1    , JETB2    ,                           //
        JETE1    , JETE2    ,                           // 
        TCPUC    ,                                      // Topo. cluster Fit 
        TCPUCUnc ,                                      // Topo. cluster Fit -- uncorrected
        Muons                                           // Muons 
  };


    unsigned short m_usedChannels; //<! counter for channels
    short m_sumOfSigns;            //<! sum of energy signs (-1, +1)
    float m_ex, m_ey, m_ez; //<! 3 components of the missing energy
    float m_sumEt, m_sumBadEt, m_sumE;  //<! scalar sums of Et, bad Et and total energy
    float m_calib0;  //<! def. 0: additive const. for the quantities above
    float m_calib1;  //<! def. 1: multipl. const. for the quantities above
    float m_maxTime; //<! maximum channel time (ns)
    float m_minTime; //<! minimum channel time (ns)
    float m_maxE;    //<! maximum channel energy (signed)
    float m_minE;    //<! minimum channel energy (signed)
    float m_maxQlty; //<! maximum channel quality -> CaloCell::quality()
    short m_status;  //<! local status flag
    char m_name[10];
    bool m_skip;    //<! default false

    TrigEFMissingEtComponent();
    void Reset();
    std::string getFormattedHeader();
    std::string getFormattedValues(); 
};


/** transient-only companion to TrigMissingET **/

class TrigEFMissingEtHelper {

  private:
    static const unsigned char DEFAULT_LEN = 6+CaloSampling::FCAL2; //<! default no. of components
    static const unsigned char REASONABLE_MAX = 2*DEFAULT_LEN;      //<! max no. of components

  public:
    TrigEFMissingEtHelper(unsigned char len=DEFAULT_LEN); //<! muons + all CaloSample items

    void Reset();
    std::string getFormattedValues(); //<! to be used ONLY for debugging

    //<! use this for REGTEST output:
    friend MsgStream& operator<<(MsgStream& out, TrigEFMissingEtHelper& obj);

    //<! getter methods:
    int GetStatus(){return m_status;};           //<! event status
    unsigned char GetElements(){return m_elements;};  //<! no. of auxiliary components
    float GetEx(){return m_ex;};        //<! X-component of missing energy
    float GetEy(){return m_ey;};        //<! Y-component of missing energy
    float GetEz(){return m_ez;};        //<! Z-component of missing energy
    float GetSumEt(){return m_sumEt;};  //<! scalar sum of Et
    float GetSumE(){return m_sumE;};    //<! scalar sum of total energy

    //<! setter methods:
    void SetStatus(int v){m_status=v;};
    void SetElements(unsigned char v){v>REASONABLE_MAX ? m_elements=REASONABLE_MAX : m_elements=v;};
    void SetEx(float v){m_ex=v;};
    void SetEy(float v){m_ey=v;};
    void SetEz(float v){m_ez=v;};
    void SetSumEt(float v){m_sumEt=v;};
    void SetSumE(float v){m_sumE=v;};

    /**
     * to access Calo Samplings provide a value from CaloSampling::CaloSample;
     * to access muons, pass 1+CaloSampling::FCAL2
     * [beware: these methods provide read/write access]
     **/
    std::vector<TrigEFMissingEtComponent>* AccessVecOfComponents(){return &m_vecOfComponents;};
    TrigEFMissingEtComponent* GetComponent(unsigned char c);

  private:
    // (the following 2 integer attributes should be packed into a single word)
    int           m_status;                                  //<! global status flag
    unsigned char m_elements;                                //<! length of the vector of components
    float         m_ex, m_ey, m_ez, m_sumEt, m_sumE; //<! global information
    std::vector<TrigEFMissingEtComponent> m_vecOfComponents; //<! local info
};


#endif // TRIGEFMISSINGET_EFMISSINGETHELPER_H
