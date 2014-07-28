/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDIMUON_CANDIDATE_H
#define TRIGDIMUON_CANDIDATE_H

#include <stdlib.h>
#include <math.h>

class Candidate
{
public:

    Candidate();

    void   doRpc(bool);
    void   doTgc(bool);
    bool   Tgc() const;
    bool   Rpc() const;
    void   setParameters( double, double, double, int);
    void   setEtaExtrapolatedInnerEndCap( double );
    void   setPhiExtrapolatedInnerEndCap( double );
    void   setEtaExtrapolatedInnerBarrel( double );
    void   setPhiExtrapolatedInnerBarrel( double );
    void   setEtaExtrapolatedMiddleEndCap( double );
    void   setPhiExtrapolatedMiddleEndCap( double );
    void   setEtaExtrapolatedMiddleBarrel( double );
    void   setPhiExtrapolatedMiddleBarrel( double );
    void   setNMdtHits( int );
    void   setNTgcEtaHits( int );
    void   setNTgcPhiHits( int );
    void   setNRpcEtaHits( int );
    void   setNRpcPhiHits( int );
    double EtaExtrapolatedInnerEndCap() const;
    double PhiExtrapolatedInnerEndCap() const;
    double EtaExtrapolatedInnerBarrel() const;
    double PhiExtrapolatedInnerBarrel() const;
    double EtaExtrapolatedMiddleEndCap() const;
    double PhiExtrapolatedMiddleEndCap() const;
    double EtaExtrapolatedMiddleBarrel() const;
    double PhiExtrapolatedMiddleBarrel() const;
    int    NMdtHits() const;
    int    NTgcEtaHits() const;
    int    NTgcPhiHits() const;
    int    NRpcEtaHits() const;
    int    NRpcPhiHits() const;
    double Pt() const;
    double Pt2() const;
    double Eta() const;
    double Phi() const;
    int    Charge() const;

private:

    double m_pt;
    double m_eta;
    double m_phi;
    int    m_charge;
    double m_eta_ext_inner_barrel;
    double m_phi_ext_inner_barrel;
    double m_eta_ext_middle_barrel;
    double m_phi_ext_middle_barrel;
    double m_eta_ext_inner_endcap;
    double m_phi_ext_inner_endcap;
    double m_eta_ext_middle_endcap;
    double m_phi_ext_middle_endcap;
    int    m_n_mdt_hits;
    int    m_n_tgc_eta_hits;
    int    m_n_tgc_phi_hits;
    int    m_n_rpc_eta_hits;
    int    m_n_rpc_phi_hits;
    bool   m_do_rpc;
    bool   m_do_tgc;
};

inline void Candidate::doRpc( bool do_rpc)
{
    m_do_rpc = do_rpc;
}

inline void Candidate::doTgc( bool do_tgc)
{
    m_do_tgc = do_tgc;
}

inline void Candidate::setParameters( double pt, double eta, double phi, int charge)
{
    m_pt = fabs(pt);
    m_eta = eta;
    m_phi = phi;
    m_charge = charge;
}

inline void Candidate::setEtaExtrapolatedMiddleBarrel(double eta_ext_middle_barrel)
{
    m_eta_ext_middle_barrel = eta_ext_middle_barrel;
}

inline void Candidate::setPhiExtrapolatedMiddleBarrel(double phi_ext_middle_barrel)
{
    m_phi_ext_middle_barrel = phi_ext_middle_barrel;
}

inline void Candidate::setEtaExtrapolatedInnerBarrel(double eta_ext_inner_barrel)
{
    m_eta_ext_inner_barrel = eta_ext_inner_barrel;
}

inline void Candidate::setPhiExtrapolatedInnerBarrel(double phi_ext_inner_barrel)
{
    m_phi_ext_inner_barrel = phi_ext_inner_barrel;
}

inline void Candidate::setEtaExtrapolatedMiddleEndCap(double eta_ext_middle_endcap)
{
    m_eta_ext_middle_endcap = eta_ext_middle_endcap;
}

inline void Candidate::setPhiExtrapolatedMiddleEndCap(double phi_ext_middle_endcap)
{
    m_phi_ext_middle_endcap = phi_ext_middle_endcap;
}

inline void Candidate::setEtaExtrapolatedInnerEndCap(double eta_ext_inner_endcap)
{
    m_eta_ext_inner_endcap = eta_ext_inner_endcap;
}

inline void Candidate::setPhiExtrapolatedInnerEndCap(double phi_ext_inner_endcap)
{
    m_phi_ext_inner_endcap = phi_ext_inner_endcap;
}

inline void Candidate::setNMdtHits(int n_mdt_hits)
{
    m_n_mdt_hits = n_mdt_hits;
}

inline void Candidate::setNTgcEtaHits(int n_tgc_eta_hits)
{
    m_n_tgc_eta_hits = n_tgc_eta_hits;
}

inline void Candidate::setNTgcPhiHits(int n_tgc_phi_hits)
{
    m_n_tgc_phi_hits = n_tgc_phi_hits;
}

inline void Candidate::setNRpcEtaHits(int n_rpc_eta_hits)
{
    m_n_rpc_eta_hits = m_n_rpc_eta_hits + n_rpc_eta_hits;
}

inline void Candidate::setNRpcPhiHits(int n_rpc_phi_hits)
{
    m_n_rpc_phi_hits = m_n_rpc_phi_hits + n_rpc_phi_hits;
}

inline int Candidate::NMdtHits() const
{
    return m_n_mdt_hits;
}

inline int Candidate::NTgcEtaHits() const
{
    return m_n_tgc_eta_hits;
}

inline int Candidate::NTgcPhiHits() const
{
    return m_n_tgc_phi_hits;
}

inline int Candidate::NRpcEtaHits() const
{
    return m_n_rpc_eta_hits;
}

inline int Candidate::NRpcPhiHits() const
{
    return m_n_rpc_phi_hits;
}

inline double Candidate::EtaExtrapolatedInnerBarrel() const
{
    return m_eta_ext_inner_barrel;
}

inline double Candidate::PhiExtrapolatedInnerBarrel() const
{
    return m_phi_ext_inner_barrel;
}

inline double Candidate::EtaExtrapolatedMiddleBarrel() const
{
    return m_eta_ext_middle_barrel;
}

inline double Candidate::PhiExtrapolatedMiddleBarrel() const
{
    return m_phi_ext_middle_barrel;
}

inline double Candidate::EtaExtrapolatedInnerEndCap() const
{
    return m_eta_ext_inner_endcap;
}

inline double Candidate::PhiExtrapolatedInnerEndCap() const
{
    return m_phi_ext_inner_endcap;
}

inline double Candidate::EtaExtrapolatedMiddleEndCap() const
{
    return m_eta_ext_middle_endcap;
}

inline double Candidate::PhiExtrapolatedMiddleEndCap() const
{
    return m_phi_ext_middle_endcap;
}

inline double Candidate::Pt() const
{
    return m_pt;
}

inline double Candidate::Pt2() const
{
    return m_pt*m_pt;
}

inline double Candidate::Eta() const
{
    return m_eta;
}

inline double Candidate::Phi() const
{
    return m_phi;
}

inline int Candidate::Charge() const
{
    return m_charge;
}

inline bool Candidate::Tgc() const
{
    return m_do_tgc;
}

inline bool Candidate::Rpc() const
{
    return m_do_rpc;
}

#endif //TRIGDIMUON_CANDIDATE_H
