/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQTCOSMICMUON_H
#define DQTCOSMICMUON_H

//
//   DQTCosmicMuon class : contains some cosmic muon parameters
//  
//   Created June 2007 by Vu Anh Tuan
//

class DQTCosmicMuon
{
  
 private: 

  float m_eta;
  float m_phi;
  float m_x0;
  float m_y0;

 public:
  
  // constructors
  DQTCosmicMuon(const float &eta, const float &phi, const float &x0, const float &y0);

  // copy constructor
  DQTCosmicMuon(const DQTCosmicMuon &input);

  // destructor
  virtual ~DQTCosmicMuon();

  // assignment operator

  DQTCosmicMuon& operator = (const DQTCosmicMuon &input);

  // access methods

  float eta(void)    const {return m_eta   ;}
  float phi(void)    const {return m_phi   ;}
  float x0(void)     const {return m_x0    ;}
  float y0(void)     const {return m_y0    ;}

};

#endif 


