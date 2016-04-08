/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********
AlignTRTHit is the Alignment TRT Hit information, created by iPatAlign
and can be saved in ntuples
*/
#ifndef IPATALIGN_ALIGNTRTHIT_H
#define IPATALIGN_ALIGNTRTHIT_H

class AlignTRTHit {
public:
  // default constructor
  AlignTRTHit();
  // access methods
  int det() const;
  int module() const;
  int phimod() const;
  int layer() const;
  int straw() const;
  float r() const;
  float phi() const;
  float z() const;
  float xdrift() const;
  float tdrift() const;
  float res() const;
  bool highth() const;
  //set methods
  void set_index(int,int,int,int,int);
  void set_r(float);
  void set_phi(float);
  void set_z(float);
  void set_xdrift(float);
  void set_tdrift(float);
  void set_res(float);
  void set_highth(bool);

private:
  int m_det;
  int m_mod;
  int m_phimod;
  int m_layer;
  int m_straw;
  float m_r;
  float m_phi;
  float m_z;
  float m_xdrift;
  float m_tdrift;
  float m_res;
  bool m_highth;
};

inline int AlignTRTHit::det() const { return m_det; }
inline int AlignTRTHit::module() const { return m_mod; }
inline int AlignTRTHit::phimod() const { return m_phimod; }
inline int AlignTRTHit::layer() const { return m_layer; }
inline int AlignTRTHit::straw() const { return m_straw; }
inline float AlignTRTHit::r()    const { return m_r; }
inline float AlignTRTHit::phi()    const { return m_phi; }
inline float AlignTRTHit::z()    const { return m_z; }
inline float AlignTRTHit::xdrift() const { return m_xdrift; }
inline float AlignTRTHit::tdrift() const { return m_tdrift; }
inline float AlignTRTHit::res() const { return m_res; }
inline bool AlignTRTHit::highth() const { return m_highth; }

inline void AlignTRTHit::set_index(int det, int mod, int phimod,
  int lay, int straw) {
  m_det=det; m_mod=mod; m_phimod=phimod; m_layer=lay; m_straw=straw; }
inline void AlignTRTHit::set_r(float f) { m_r=f; }
inline void AlignTRTHit::set_phi(float f) { m_phi=f; }
inline void AlignTRTHit::set_z(float f) { m_z=f; }
inline void AlignTRTHit::set_xdrift(float x) { m_xdrift=x; }
inline void AlignTRTHit::set_tdrift(float t) { m_tdrift=t; }
inline void AlignTRTHit::set_res(float r) { m_res=r; }
inline void AlignTRTHit::set_highth(bool b) { m_highth=b; }

#endif // IPATALIGN_ALIGNTRTHIT_H

