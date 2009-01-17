/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_EFFICIENCY_H
#define TRIGGER_EFFICIENCY_H

#include <list>
#include <utility>

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"


struct MontecarloThru
{
    float eta;
    float phi;

    float p_x;
    float p_y;
    float p_z;

    float vertex_x;
    float vertex_y;
    float vertex_z;
};


class TrigEfficiency
{
    public:
    typedef std::pair <unsigned int,unsigned long int> TrigData;
    typedef std::list <TrigData> SLwords;
    typedef std::list <MontecarloThru> MCthru;

    private:
    void load_SLwords(TrigData);
    void load_MCthru(MontecarloThru); 

    public:
    TrigEfficiency(NTuple::Tuple*,unsigned int,unsigned int);
    ~TrigEfficiency();

    TrigEfficiency(const TrigEfficiency&);

    TrigEfficiency operator=(const TrigEfficiency&);

    void operator<<(TrigData);
    void operator<<(MontecarloThru);

    const SLwords & sl_words(void)    const {return m_words;}
    const MCthru &  mcthru(void)      const {return m_thru;}
    NTuple::Tuple* ntuple(void)       const {return m_ntup;}
    int max_muon(void)                const {return m_max_muon;}
    int max_roi(void)                 const { return m_max_roi;}


    void write_ntp(INTupleSvc*);

    private:
    NTuple::Tuple* m_ntup; 

    SLwords m_words;
    MCthru  m_thru;

    unsigned int m_max_muon;
    unsigned int m_max_roi;

    NTuple::Item<long>  m_nmuon;
    NTuple::Array<float> m_eta;
    NTuple::Array<float> m_phi;
    NTuple::Array<float> m_Px;
    NTuple::Array<float> m_Py;
    NTuple::Array<float> m_Pz;
    NTuple::Array<float> m_Vx;
    NTuple::Array<float> m_Vy;
    NTuple::Array<float> m_Vz;
  
    NTuple::Item<long>  m_nroi;
    NTuple::Array<float> m_side;
    NTuple::Array<float> m_sector;
    NTuple::Array<float> m_roi;
    NTuple::Array<float> m_th;
};




#endif
