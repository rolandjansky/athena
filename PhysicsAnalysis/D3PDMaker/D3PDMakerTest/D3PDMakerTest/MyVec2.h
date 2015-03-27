// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MyVec2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2015
 * @brief Test class for D3PD maker.
 */


#ifndef D3PDMAKERTEST_MYVEC2_H
#define D3PDMAKERTEST_MYVEC2_H


namespace D3PDTest {


class MyVec2
{
public:
  MyVec2() : m_pt(0), m_eta(0), m_phi(0), m_m(0) {}
  float Pt() const { return m_pt; }
  float Eta() const { return m_eta; }
  float Phi() const { return m_phi; }
  float M() const { return m_m; }
  void SetPtEtaPhiM (float pt, float eta, float phi, float m)
  {
    m_pt = pt;
    m_eta = eta;
    m_phi = phi;
    m_m = m;
  }

private:
  float m_pt;
  float m_eta;
  float m_phi;
  float m_m;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_MYVEC2_H
