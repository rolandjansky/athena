/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTRKINFO_ALIGNSIMODULE_H
#define INDETALIGNTRKINFO_ALIGNSIMODULE_H
// AlignSiModule.h - hold information on one silicon module for InDetAlignNt 
// ntuple
// Richard Hawkings, started 13/4/04

class AlignSiModule {

public:
  // constructor
  AlignSiModule();
  // access methods
  int index() const;
  int dettype() const;
  int bec() const;
  int layer() const;
  int ring() const;
  int sector() const;
  int side() const;
  int ncoord() const;
  float pos(int) const;
  float offset(int) const;
  float rot(int) const;
  float pitch() const;
  float length() const;
  float width(int) const;
  int nhits() const;
  bool dead() const;
  bool stereo() const;

  // set methods
  void set_index(int);
  void set_dettype(int);
  void set_bec(int);
  void set_layer(int);
  void set_ring(int);
  void set_sector(int);
  void set_side(int);
  void set_ncoord(int);
  void set_pos(float,int);
  void set_offset(float,int);
  void set_rot(float,int);
  void set_pitch(float);
  void set_length(float);
  void set_width(float,int);
  int inchits() const;
  void set_dead(bool);
  void set_stereo(bool);

 private:
  int m_index;
  int m_dettype;
  int m_bec;
  int m_layer;
  int m_ring;
  int m_sector;
  int m_side;
  int m_ncoord;
  float m_pos[3];
  float m_offset[3];
  float m_rot[9];
  float m_pitch;
  float m_length;
  float m_width[3];
  mutable int m_nhits;
  bool m_dead;
  bool m_stereo;
};

inline AlignSiModule::AlignSiModule() : m_nhits(0), m_dead(false) {}

inline int AlignSiModule::index() const { return m_index; }
inline int AlignSiModule::dettype() const { return m_dettype; }
inline int AlignSiModule::bec() const { return m_bec; }
inline int AlignSiModule::layer() const { return m_layer; }
inline int AlignSiModule::ring() const { return m_ring; }
inline int AlignSiModule::sector() const { return m_sector; }
inline int AlignSiModule::side() const { return m_side; }
inline int AlignSiModule::ncoord() const { return m_ncoord; }
inline float AlignSiModule::pos(int i) const { return m_pos[i]; }
inline float AlignSiModule::offset(int i) const { return m_offset[i]; }
inline float AlignSiModule::rot(int i) const { return m_rot[i]; }
inline float AlignSiModule::pitch() const { return m_pitch; }
inline float AlignSiModule::length() const { return m_length; }
inline float AlignSiModule::width(int i) const { return m_width[i]; }
inline int AlignSiModule::nhits() const {return m_nhits; }
inline bool AlignSiModule::dead() const {return m_dead; }
inline bool AlignSiModule::stereo() const {return m_stereo; }


inline void AlignSiModule::set_index(int i) { m_index=i; }
inline void AlignSiModule::set_dettype(int i) { m_dettype=i; }
inline void AlignSiModule::set_bec(int i) { m_bec=i; }
inline void AlignSiModule::set_layer(int i) { m_layer=i; }
inline void AlignSiModule::set_ring(int i) { m_ring=i; }
inline void AlignSiModule::set_sector(int i) { m_sector=i; }
inline void AlignSiModule::set_side(int i) { m_side=i; }
inline void AlignSiModule::set_ncoord(int i) { m_ncoord=i; }
inline void AlignSiModule::set_pos(float f,int i) { m_pos[i]=f; }
inline void AlignSiModule::set_offset(float f,int i) { m_offset[i]=f; }
inline void AlignSiModule::set_rot(float f,int i) { m_rot[i]=f; }
inline void AlignSiModule::set_pitch(float f) { m_pitch=f; }
inline void AlignSiModule::set_length(float f) { m_length=f; }
inline void AlignSiModule::set_width(float f,int i) { m_width[i]=f; }
inline int AlignSiModule::inchits() const { return ++m_nhits; }
inline void AlignSiModule::set_dead(bool b) { m_dead=b; }
inline void AlignSiModule::set_stereo(bool b) { m_stereo=b; }

#endif // INDETALIGNTRKINFO_ALIGNSIMODULE_H
