/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTRKINFO_ALIGNTRK_H
#define INDETALIGNTRKINFO_ALIGNTRK_H
// AlignTrk.h - transient copy of information in InDetAlignNt ntuple
// Richard Hawkings, started 13/4/04
// Ntuple contents are documented on ID alignment webpage
// http://rhawking.home.cern.ch/rhawking/atlas/aligntrk/welcome.html

#include <vector>
#include "InDetAlignTrkInfo/AlignSiHit.h"
#include "InDetAlignTrkInfo/AlignTRTHit.h"

class AlignTrk {

public:
  // default constructor
  AlignTrk();
  // access methods
  int nhits() const;
  int noverlapPIX() const; 
  int noverlapSCT() const; 
  int nscat() const;
  int irun() const;
  int ievent() const;
  int itrack() const;
  int nholes() const;
  int nshared() const;
  float trkpar(int) const;
  float trkcov(int) const;
  int ikine() const;
  float truetrkpar(int) const;
  int ivtx() const;
  float trpvtx(int) const;
  float beampos(int) const;
  float beamsigma(int) const;
  float beamtilt(int) const;
  float trtpt() const;
  float trtpterr() const;
  int ntrthits() const;

 
  // set methods
  void set_noverlapPIX(const int); 
  void set_noverlapSCT(const int); 
  void set_nscat(const int);
  void set_irun(const int);
  void set_ievent(const int);
  void set_itrack(const int);
  void set_nholes(const int);
  void set_nshared(const int);
  void set_trkpar(const float,const int);
  void set_trkcov(const float,const int);
  void set_ikine(const int);
  void set_truetrkpar(const float,const int);
  void set_ivtx(const int);
  void set_trpvtx(const float,const int);
  void set_beampos(const float, const int);
  void set_beamsigma(const float, const int);
  void set_beamtilt(const float, const int);
  void set_trtcons(const float, const float);
  std::vector<AlignSiHit>::iterator hitlist_begin();
  std::vector<AlignSiHit>::iterator hitlist_end();
  std::vector<AlignSiHit>::const_iterator hitlist_cbegin() const;
  std::vector<AlignSiHit>::const_iterator hitlist_cend() const;

  void add_hit(AlignSiHit&);

  void print() const;
  void add_trthit(AlignTRTHit&);
  std::vector<AlignTRTHit>::iterator trthitlist_begin();
  std::vector<AlignTRTHit>::iterator trthitlist_end();
  std::vector<AlignTRTHit>::const_iterator trthitlist_cbegin() const;
  std::vector<AlignTRTHit>::const_iterator trthitlist_cend() const;

 /*  void add_overlaphit(AlignSiHit&); // in progress */
/*   std::vector<AlignSiHit>::iterator overlaphitlist_begin(); */
/*   std::vector<AlignSiHit>::iterator overlaphitlist_end(); */
/*   std::vector<AlignSiHit>::const_iterator overlaphitlist_cbegin() const; */
/*   std::vector<AlignSiHit>::const_iterator overlaphitlist_cend() const; */


private:
  int m_nscat;
  int m_irun;
  int m_ievent;
  int m_itrack;
  int m_nholes;
  int m_nshared;
  int m_noverlapPIX;
  int m_noverlapSCT;
  float m_trkpar[5]{};
  float m_trkcov[15]{};
  int m_ikine;
  float m_truetrkpar[5]{};
  int m_ivtx;
  float m_trpvtx[3]{};
  float m_beampos[3]{};
  float m_beamsigma[3]{};
  float m_beamtilt[2]{};
  float m_trtpt = 0.0F;
  float m_trtpterr = 0.0F;
  std::vector<AlignSiHit> m_hits;
  std::vector<AlignTRTHit> m_trthits;
  std::vector<AlignSiHit> m_overlaphits;
};

inline int AlignTrk::nhits() const { return m_hits.size(); }

inline int AlignTrk::noverlapPIX() const { return m_noverlapPIX; }
inline int AlignTrk::noverlapSCT() const { return m_noverlapSCT; }

inline int AlignTrk::nscat()   const { return m_nscat; }
inline int AlignTrk::irun()    const { return m_irun; }
inline int AlignTrk::ievent()  const { return m_ievent; }
inline int AlignTrk::itrack()  const { return m_itrack; }
inline int AlignTrk::nholes()  const { return m_nholes; }
inline int AlignTrk::nshared() const { return m_nshared; }
inline float AlignTrk::trkpar(int i) const { return m_trkpar[i-1]; }
inline float AlignTrk::trkcov(int i) const { return m_trkcov[i-1]; }
inline int AlignTrk::ikine() const { return m_ikine; }
inline float AlignTrk::truetrkpar(int i) const { return m_truetrkpar[i-1]; }
inline int AlignTrk::ivtx() const { return m_ivtx; }
inline float AlignTrk::trpvtx(int i) const { return m_trpvtx[i-1]; }
inline float AlignTrk::beampos(int i) const { return m_beampos[i-1]; }
inline float AlignTrk::beamsigma(int i) const { return m_beamsigma[i-1]; }
inline float AlignTrk::beamtilt(int i) const { return m_beamtilt[i-1]; }
inline float AlignTrk::trtpt() const { return m_trtpt; }
inline float AlignTrk::trtpterr() const { return m_trtpterr; }
inline int AlignTrk::ntrthits() const { return m_trthits.size(); }


inline void AlignTrk::set_noverlapPIX(const int i) { m_noverlapPIX=i; }
inline void AlignTrk::set_noverlapSCT(const int i) { m_noverlapSCT=i; }

inline void AlignTrk::set_nscat(const int i)   { m_nscat=i; }
inline void AlignTrk::set_irun(const int i)    { m_irun=i; }
inline void AlignTrk::set_ievent(const int i)  { m_ievent=i; }
inline void AlignTrk::set_itrack(const int i)  { m_itrack=i; }
inline void AlignTrk::set_nholes(const int i)  { m_nholes=i; }
inline void AlignTrk::set_nshared(const int i) { m_nshared=i; }
inline void AlignTrk::set_trkpar(const float f,const int i) { m_trkpar[i-1]=f;}
inline void AlignTrk::set_trkcov(const float f,const int i) { m_trkcov[i-1]=f;}
inline void AlignTrk::set_ikine(const int i) { m_ikine=i; }
inline void AlignTrk::set_truetrkpar(const float f,const int i) 
  { m_truetrkpar[i-1]=f; }
inline void AlignTrk::set_ivtx(const int i) { m_ivtx=i; }
inline void AlignTrk::set_trpvtx(const float f,const int i) { m_trpvtx[i-1]=f;}
inline void AlignTrk::set_beampos(const float f, const int i) 
  { m_beampos[i-1]=f;}
inline void AlignTrk::set_beamsigma(const float f, const int i) 
  { m_beamsigma[i-1]=f;}
inline void AlignTrk::set_beamtilt(const float f, const int i) 
  { m_beamtilt[i-1]=f;}
inline void AlignTrk::set_trtcons(const float pt, const float pterr)
{ m_trtpt=pt; m_trtpterr=pterr; }


inline std::vector<AlignSiHit>::iterator AlignTrk::hitlist_begin()
{ return m_hits.begin(); }
inline std::vector<AlignSiHit>::iterator AlignTrk::hitlist_end() 
{ return m_hits.end(); }
inline std::vector<AlignSiHit>::const_iterator AlignTrk::hitlist_cbegin()
const { return m_hits.begin(); }
inline std::vector<AlignSiHit>::const_iterator AlignTrk::hitlist_cend() 
const { return m_hits.end(); }

inline std::vector<AlignTRTHit>::iterator AlignTrk::trthitlist_begin()
{ return m_trthits.begin(); }
inline std::vector<AlignTRTHit>::iterator AlignTrk::trthitlist_end() 
{ return m_trthits.end(); }
inline std::vector<AlignTRTHit>::const_iterator AlignTrk::trthitlist_cbegin()
     const { return m_trthits.begin(); }
inline std::vector<AlignTRTHit>::const_iterator AlignTrk::trthitlist_cend() 
     const { return m_trthits.end(); }


/* inline std::vector<AlignSiHit>::iterator AlignTrk::overlaphitlist_begin()  */
/* { return m_overlaphits.begin(); }  */
/* inline std::vector<AlignSiHit>::iterator AlignTrk::overlaphitlist_end() */
/* { return m_overlaphits.end(); } */

/* inline std::vector<AlignSiHit>::const_iterator AlignTrk::overlaphitlist_cbegin()  */
/*      const { return m_overlaphits.begin(); }  */
/* inline std::vector<AlignSiHit>::const_iterator AlignTrk::overlaphitlist_cend()  */
/*      const { return m_overlaphits.end(); }  */


inline void AlignTrk::add_hit(AlignSiHit& hit) { m_hits.push_back(hit); }
inline void AlignTrk::add_trthit(AlignTRTHit& hit) {m_trthits.push_back(hit);}
//inline void AlignTrk::add_overlaphit(AlignSiHit& hit) {m_overlaphits.push_back(hit);} //in progress

#endif // INDETALIGNTRKINFO_ALIGNTRK_H



