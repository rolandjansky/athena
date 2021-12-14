/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTRKINFO_ALIGNSIHIT_H
#define INDETALIGNTRKINFO_ALIGNSIHIT_H
// AlignSiHit.h - hold information on one silicon hit for InDetAlignNt ntuple
// Richard Hawkings, started 13/4/04

class AlignSiHit {
public:
  // default constructor
  AlignSiHit();
  // access methods
  int index() const;
  bool isOverlap()const; // in progress 
  int chan1() const;
  int chan2() const;
  int clusw() const;
  int clusl() const;
  int cluss() const;
  int status() const;
  float stereo() const;
  float rphi_resid() const;
  float z_resid() const;
  float erphi_resid() const;
  float ez_resid() const;
  float trkExtrapLocalXError() const;
  float trkExtrapLocalYError() const;
  float r() const;
  float phi() const;
  float z() const;
  float delzr() const;
  float sinlocal() const;
  float locang(int) const;
  float locdel(int) const;
  float locedel(int) const;
  float globang(int) const;
  float globdel(int) const;
  float globedel(int) const;
  float trklen() const;
  float dtpar1(int) const;
  float dtpar2(int) const;
  //set methods
  void set_index(int);
  void set_chan1(int);
  void set_chan2(int);
  void set_clusw(int);
  void set_clusl(int);
  void set_cluss(int);
  void set_status(int);
  void set_isOverlap(bool); // in progress
  void set_stereo(float);
  void set_rphi_resid(float);
  void set_z_resid(float);
  void set_erphi_resid(float);
  void set_ez_resid(float);
  void set_trkExtrapLocalXError(float);
  void set_trkExtrapLocalYError(float);
  void set_r(float);
  void set_phi(float);
  void set_z(float);
  void set_delzr(float);
  void set_sinlocal(float);
  void set_locang(float,int);
  void set_locdel(float,int);
  void set_locedel(float,int);
  void set_globang(float,int);
  void set_globdel(float,int);
  void set_globedel(float,int);
  void set_trklen(float);
  void set_dtpar1(float,int);
  void set_dtpar2(float,int);

private:
  int m_index;
  int m_chan1;
  int m_chan2;
  int m_clusw;
  int m_clusl;
  int m_cluss;
  int m_status;
  bool m_isOverlap; //In progress:  this variable identify overlaphit=true 
  float m_stereo;
  float m_rphi_resid;
  float m_z_resid;
  float m_erphi_resid;
  float m_ez_resid;
  float m_trkExtrapLocalXError;  // error of the track extrapolation to module surface in the local x coordinate
  float m_trkExtrapLocalYError;  // error of the track extrapolation to module surface in the local y coordinate
  float m_r;
  float m_phi;
  float m_z;
  float m_delzr;
  float m_sinlocal;
  float m_locang[2]{};  // local angles: theta,phi
  float m_locdel[2]{};  // local scatterings: theta, phi
  float m_locedel[2]{}; // error on local scatterings: theta, phi
  float m_globang[2]{};  // global angles: cot_theta,phi
  float m_globdel[2]{};  // global scatterings: cot_theta, phi
  float m_globedel[2]{}; // error on global scatterings: cot_theta, phi
  float m_trklen;    // track length since last scatterer
  float m_dtpar1[5]{}; // rphi trackpar derivatives
  float m_dtpar2[5]{}; // rz trackpar derivatives
};




inline int AlignSiHit::index()  const { return m_index; }
inline int AlignSiHit::chan1()  const { return m_chan1; }
inline int AlignSiHit::chan2()  const { return m_chan2; }
inline int AlignSiHit::clusw()  const { return m_clusw; }
inline int AlignSiHit::clusl()  const { return m_clusl; }
inline int AlignSiHit::cluss()  const { return m_cluss; }
inline int AlignSiHit::status() const { return m_status; }
inline bool AlignSiHit::isOverlap() const { return m_isOverlap; }// in progress

inline float AlignSiHit::stereo() const { return m_stereo; }
inline float AlignSiHit::rphi_resid()  const { return m_rphi_resid; }
inline float AlignSiHit::z_resid()     const { return m_z_resid; }
inline float AlignSiHit::erphi_resid() const { return m_erphi_resid; }
inline float AlignSiHit::ez_resid()    const { return m_ez_resid; }
inline float AlignSiHit::trkExtrapLocalXError() const { return m_trkExtrapLocalXError; }
inline float AlignSiHit::trkExtrapLocalYError() const { return m_trkExtrapLocalYError; }
inline float AlignSiHit::r()    const { return m_r; }
inline float AlignSiHit::phi()    const { return m_phi; }
inline float AlignSiHit::z()    const { return m_z; }
inline float AlignSiHit::delzr()    const { return m_delzr; }
inline float AlignSiHit::sinlocal() const { return m_sinlocal; }
inline float AlignSiHit::locang(int i) const  { return m_locang[i]; }
inline float AlignSiHit::locdel(int i) const  { return m_locdel[i]; }
inline float AlignSiHit::locedel(int i) const { return m_locedel[i]; }
inline float AlignSiHit::globang(int i) const  { return m_globang[i]; }
inline float AlignSiHit::globdel(int i) const  { return m_globdel[i]; }
inline float AlignSiHit::globedel(int i) const { return m_globedel[i]; }
inline float AlignSiHit::trklen() const { return m_trklen; }
inline float AlignSiHit::dtpar1(int i) const  { return m_dtpar1[i]; }
inline float AlignSiHit::dtpar2(int i) const  { return m_dtpar2[i]; }

inline void AlignSiHit::set_index(int i) { m_index=i; }
inline void AlignSiHit::set_chan1(int i) { m_chan1=i; }
inline void AlignSiHit::set_chan2(int i) { m_chan2=i; }
inline void AlignSiHit::set_clusw(int i) { m_clusw=i; }
inline void AlignSiHit::set_clusl(int i) { m_clusl=i; }
inline void AlignSiHit::set_cluss(int i) { m_cluss=i; }
inline void AlignSiHit::set_status(int i) { m_status=i; }
inline void AlignSiHit::set_isOverlap(bool b) { m_isOverlap=b; }// in progress

inline void AlignSiHit::set_stereo(float f) { m_stereo=f; }
inline void AlignSiHit::set_rphi_resid(float f) { m_rphi_resid=f; }
inline void AlignSiHit::set_z_resid(float f) { m_z_resid=f; }
inline void AlignSiHit::set_erphi_resid(float f) { m_erphi_resid=f; }
inline void AlignSiHit::set_ez_resid(float f) { m_ez_resid=f; }
inline void AlignSiHit::set_trkExtrapLocalXError(float f) { m_trkExtrapLocalXError = f; }
inline void AlignSiHit::set_trkExtrapLocalYError(float f) { m_trkExtrapLocalYError = f; }
inline void AlignSiHit::set_r(float f) { m_r=f; }
inline void AlignSiHit::set_phi(float f) { m_phi=f; }
inline void AlignSiHit::set_z(float f) { m_z=f; }
inline void AlignSiHit::set_delzr(float f) { m_delzr=f; }
inline void AlignSiHit::set_sinlocal(float f) { m_sinlocal=f; }
inline void AlignSiHit::set_locang(float f,int i) { m_locang[i]=f; }
inline void AlignSiHit::set_locdel(float f,int i) { m_locdel[i]=f; }
inline void AlignSiHit::set_locedel(float f,int i) { m_locedel[i]=f; }
inline void AlignSiHit::set_globang(float f,int i) { m_globang[i]=f; }
inline void AlignSiHit::set_globdel(float f,int i) { m_globdel[i]=f; }
inline void AlignSiHit::set_globedel(float f,int i) { m_globedel[i]=f; }
inline void AlignSiHit::set_trklen(float f) { m_trklen=f; }
inline void AlignSiHit::set_dtpar1(float f,int i) { m_dtpar1[i]=f; }
inline void AlignSiHit::set_dtpar2(float f,int i) { m_dtpar2[i]=f; }

#endif // INDETALIGNTRKINFO_ALIGNSIHIT_H
