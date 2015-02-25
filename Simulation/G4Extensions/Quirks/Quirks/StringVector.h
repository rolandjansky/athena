/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRINGVECTOR_H
#define STRINGVECTOR_H

#include "G4ThreeVector.hh"
#include "G4LorentzVector.hh"

class StringVector {
public:
    StringVector();
    StringVector(const G4ThreeVector& p, G4double m);
    StringVector(G4double x, G4double y, G4double z, G4double m);
    ~StringVector();
    G4ThreeVector vect() const;
    G4double mag() const;
    G4double x() const;
    G4double y() const;
    G4double z() const;
    G4double t() const;
    G4LorentzVector lv() const;
    StringVector reflect(const G4LorentzVector& axis) const;
    void set(const G4ThreeVector& p, G4double m);
    void set(G4double x, G4double y, G4double z, G4double m);
    void operator *= (G4double a);

private:
    G4ThreeVector m_p;
    G4double m_m;
};

inline StringVector operator * (G4double a, const StringVector& s) {
    return StringVector(a * s.vect(), a * s.mag());
}

inline StringVector operator * (const StringVector& s, G4double a) {
    return StringVector(a * s.vect(), a * s.mag());
}

inline G4ThreeVector StringVector::vect() const {
    return m_p;
}

inline G4double StringVector::mag() const {
    return m_m;
}

inline G4double StringVector::x() const {
    return m_p.x();
}

inline G4double StringVector::y() const {
    return m_p.y();
}

inline G4double StringVector::z() const {
    return m_p.z();
}

inline G4double StringVector::t() const {
    return std::sqrt(m_p.mag2() + m_m*m_m);
}

inline G4LorentzVector StringVector::lv() const {
    return G4LorentzVector(m_p, t());
}

inline StringVector StringVector::reflect(const G4LorentzVector& axis) const {
    return StringVector(axis.vect() * (2 * lv() * axis) / axis.mag2() - m_p, m_m);
}

inline void StringVector::set(const G4ThreeVector& p, G4double m) {
    m_p = p;
    m_m = m;
}

inline void StringVector::set(G4double x, G4double y, G4double z, G4double m) {
    m_p.set(x,y,z);
    m_m = m;
}

inline void StringVector::operator *= (G4double a) {
    m_p *= a;
    m_m *= a;
}

#endif
