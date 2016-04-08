/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_GEN_HEPEVT_H
#define GENERATORMODULES_GEN_HEPEVT_H

#include "CLIDSvc/CLASS_DEF.h"
// #include "StoreGate/tools/ClassID_traits.h"
// #include "CLHEP/Vector/LorentzVector.h"
#include <vector>
#include <map>


/// @brief C++ access to the Fortran HEPEVT common block
///
/// - NEVHEP           event number (or some special meaning
///                    (see documentation for details)
/// - NHEP             actual number of entries in current event.
/// - ISTHEP[IHEP]     status code for IHEP'th entry - see
///                    documentation for details
/// - IDHEP [IHEP]     IHEP'th particle identifier according to PDG.
/// - JMOHEP[IHEP][0] - pointer to position of 1st mother
/// - JMOHEP[IHEP][1] - pointer to position of 2nd mother
/// - JDAHEP[IHEP][0] - pointer to position of 1st daughter
/// - JDAHEP[IHEP][1] - pointer to position of 2nd daughter
/// - PHEP  [IHEP][0] - X momentum [Gev/c]
/// - PHEP  [IHEP][1] - Y momentum [Gev/c]
/// - PHEP  [IHEP][2] - Z momentum [Gev/c]
/// - PHEP  [IHEP][3] - Energy [Gev]
/// - PHEP  [IHEP][4] - Mass[Gev/c^2]
/// - VHEP  [IHEP][0] - X vertex [mm]
/// - VHEP  [IHEP][1] - Y vertex [mm]
/// - VHEP  [IHEP][2] - Z vertex [mm]
/// - VHEP  [IHEP][3] - production time [mm/c]
///
class Gen_HEPEVT {
public:
  Gen_HEPEVT();

  /// @name "Get" methods
  //@{
  int nevhep() const;
  int nhep() const;
  int isthep(int ihep);
  int idhep(int ihep);
  int first_parent(int ihep);
  int last_parent(int ihep);
  int first_child(int ihep);
  int last_child(int ihep);
  double px(int ihep);
  double py(int ihep);
  double pz(int ihep);
  double e(int ihep);
  double m(int ihep);
  double x(int ihep);
  double y(int ihep);
  double z(int ihep);
  double t(int ihep);
  //@}

  // "Set" methods
  //@{
  void nevhep(int value);
  void nhep(int value);
  void isthep(int& ihep, int value);
  void idhep(int& ihep, int value);
  void mothers(int& ihep, int value1, int value2);
  void daughters(int& ihep, int value1, int value2);
  void momentum(int& ihep, double px, double py, double pz, double e, double m );
  void position(int& ihep, double x, double y, double z, double t);
  //@}


private:

  int m_NEVHEP;
  int m_NHEP;
  std::map<int, int> m_ISTHEP;
  std::map<int, int> m_IDHEP;
  std::map<int, std::pair<int, int> > m_JMOHEP;
  std::map<int, std::pair<int, int> > m_JDAHEP;
  std::map<int, double> m_PHEP0;
  std::map<int, double> m_PHEP1;
  std::map<int, double> m_PHEP2;
  std::map<int, double> m_PHEP3;
  std::map<int, double> m_PHEP4;
  std::map<int, double> m_VHEP0;
  std::map<int, double> m_VHEP1;
  std::map<int, double> m_VHEP2;
  std::map<int, double> m_VHEP3;
};



inline
Gen_HEPEVT::Gen_HEPEVT (void)
  : m_NEVHEP(0), m_NHEP(0)
{ }


inline int
Gen_HEPEVT::nevhep (void) const { return m_NEVHEP; }

inline int
Gen_HEPEVT::nhep(void) const { return m_NHEP; }

inline int
Gen_HEPEVT::isthep(int ihep) { return m_ISTHEP[ihep]; }

inline int
Gen_HEPEVT::idhep(int ihep) { return m_IDHEP[ihep]; }

inline int
Gen_HEPEVT::first_parent(int ihep) { return m_JMOHEP[ihep].first; }

inline int
Gen_HEPEVT::last_parent(int ihep) { return m_JMOHEP[ihep].second; }

inline int
Gen_HEPEVT::first_child(int ihep) { return m_JDAHEP[ihep].first; }

inline int
Gen_HEPEVT::last_child(int ihep) { return m_JDAHEP[ihep].second; }

inline double
Gen_HEPEVT::px(int ihep) { return m_PHEP0[ihep]; }

inline double
Gen_HEPEVT::py(int ihep) { return m_PHEP1[ihep]; }

inline double
Gen_HEPEVT::pz(int ihep) { return m_PHEP2[ihep]; }

inline double
Gen_HEPEVT::e(int ihep) { return m_PHEP3[ihep]; }

inline double
Gen_HEPEVT::m(int ihep) { return m_PHEP4[ihep]; }

inline double
Gen_HEPEVT::x(int ihep) { return m_VHEP0[ihep]; }

inline double
Gen_HEPEVT::y(int ihep) { return m_VHEP1[ihep]; }

inline double
Gen_HEPEVT::z(int ihep) { return m_VHEP2[ihep]; }

inline double
Gen_HEPEVT::t(int ihep) { return m_VHEP3[ihep]; }


inline void
Gen_HEPEVT::nevhep(int value) { m_NEVHEP = value; }

inline void
Gen_HEPEVT::nhep(int value) { m_NHEP = value; }

inline void
Gen_HEPEVT::isthep(int& ihep, int value) { m_ISTHEP[ihep] = value; }

inline void
Gen_HEPEVT::idhep(int& ihep, int value) { m_IDHEP[ihep] = value; }

inline void
Gen_HEPEVT::mothers(int& ihep, int value1, int value2) {
  m_JMOHEP[ihep] = std::make_pair(value1, value2);
}

inline void
Gen_HEPEVT::daughters(int& ihep, int value1, int value2) {
  m_JDAHEP[ihep] = std::make_pair(value1, value2);
}

inline void
Gen_HEPEVT::momentum(int& ihep, double px, double py, double pz, double e, double m ) {
  m_PHEP0[ihep] = px;
  m_PHEP1[ihep] = py;
  m_PHEP2[ihep] = pz;
  m_PHEP3[ihep] = e;
  m_PHEP4[ihep] = m;
}

inline void
Gen_HEPEVT::position(int& ihep, double x, double y, double z, double t) {
  m_VHEP0[ihep] = x;
  m_VHEP1[ihep] = y;
  m_VHEP2[ihep] = z;
  m_VHEP3[ihep] = t;
}


CLASS_DEF( Gen_HEPEVT, 2221, 1)

#endif
