/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Material.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIAL_H
#define TRKGEOMETRY_MATERIAL_H

#include <climits>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Trk {

static const double s_oneOverUcharMax = 1. / double(UCHAR_MAX);

/** @class ElementFraction */
class ElementFraction : public std::pair<unsigned char, unsigned char> {
 public:
  /** Default Constructor */
  ElementFraction() : std::pair<unsigned char, unsigned char>(0, 0) {}

  /** Copy Constructor from base class */
  ElementFraction(const std::pair<unsigned char, unsigned char>& ef)
      : std::pair<unsigned char, unsigned char>(ef) {}

  /**Constructor from arguments */
  ElementFraction(unsigned char iz, unsigned char ifrac)
      : std::pair<unsigned char, unsigned char>(iz, ifrac) {}

  /** assignment operator from base class */
  ElementFraction& operator=(
      const std::pair<unsigned char, unsigned char>& ef) {
    if (this != &ef) {
      std::pair<unsigned char, unsigned char>::operator=(ef);
    }
    return (*this);
  }

  /** Return in a nice format */
  unsigned int element() const {
    return static_cast<unsigned int>((*this).first);
  }

  /** Return in a nice format */
  double fraction() const {
    return (static_cast<unsigned int>((*this).second)) * s_oneOverUcharMax;
  }
};

/** @struct MaterialComposition */
class MaterialComposition : public std::vector<ElementFraction> {
 public:
  /** default constructors*/
  MaterialComposition() : std::vector<ElementFraction>() {}
  MaterialComposition(const MaterialComposition&) = default;
  MaterialComposition(MaterialComposition&&) = default;
  MaterialComposition& operator=(const MaterialComposition&) = default;
  MaterialComposition& operator=(MaterialComposition&&) = default;
  ~MaterialComposition() {}

  /** constructor for persistency (1), size optimized */
  MaterialComposition(const std::vector<unsigned char>& iel,
                      const std::vector<unsigned char>& ifrac) {
    reserve(iel.size());
    for (std::size_t elvc = 0; elvc < iel.size() && !ifrac.empty(); ++elvc)
      push_back(ElementFraction(iel[elvc], ifrac[elvc]));
  }

  /** constructor for persistency (2), size optimized */
  MaterialComposition(
      const std::vector<std::pair<unsigned char, unsigned char> >& efracs) {
    reserve(efracs.size());
    for (auto& efracIt : efracs) push_back(efracIt);
  }

  /** Copy constructor from base class */
  MaterialComposition(const std::vector<ElementFraction>& mc)
      : std::vector<ElementFraction>(mc) {}

  /** assignment operator from base class */
  MaterialComposition& operator=(const std::vector<ElementFraction>& mc) {
    if (this != &mc) {
      std::vector<ElementFraction>::operator=(mc);
    }
    return (*this);
  }

  /** assignment operator for persistency (2) */
  MaterialComposition& operator=(
      const std::vector<std::pair<unsigned char, unsigned char> >& efracs) {
    clear();
    reserve(efracs.size());
    for (auto& efracIt : efracs) push_back(efracIt);
    return (*this);
  }
};

/** @class Material

 A common object to be contained by
 - MaterialStep ( for mapping)
 - MaterialProperties ( for reconstruction )
 - Is optimizes for T/P split

 @author Andreas.Salzburger@cern.ch
*/
class Material {
 public:
  // standard x0, l0, A, Z, rho description
  float X0;
  float L0;
  float A;
  float Z;
  float rho;
  float dEdX;
  float zOaTr;
  MaterialComposition* composition;

  /** Default Constructor needed for POOL */
  Material()
      : X0(10.e10),
        L0(10.e10),
        A(0.),
        Z(0.),
        rho(0.),
        dEdX(0.),
        zOaTr(0.),
        composition(nullptr) {}

  /** Constructor with arguments */
  Material(float iX0, float iL0, float iA, float iZ, float iRho,
           float idEdX = 0., MaterialComposition* mc = nullptr)
      : X0(iX0),
        L0(iL0),
        A(iA),
        Z(iZ),
        rho(iRho),
        dEdX(idEdX),
        zOaTr(iA > 0 ? iZ / iA * iRho : 0.),
        composition(mc) {}

  /** Copy Constructor */
  Material(const Material& amc)
      : X0(amc.X0),
        L0(amc.L0),
        A(amc.A),
        Z(amc.Z),
        rho(amc.rho),
        dEdX(amc.dEdX),
        zOaTr(amc.zOaTr),
        composition(amc.composition ? new MaterialComposition(*amc.composition)
                                    : nullptr) {}

  /** Move Constructor */
  Material(Material&& amc)
      : X0(amc.X0),
        L0(amc.L0),
        A(amc.A),
        Z(amc.Z),
        rho(amc.rho),
        dEdX(amc.dEdX),
        zOaTr(amc.zOaTr),
        composition(amc.composition) {
    amc.composition = nullptr;
  }

  /** Destructor - delete the composition if there */
  virtual ~Material() {
    if (composition) delete composition;
  }

  /** Assignment operator */
  Material& operator=(const Material& amc) {
    if (this != &amc) {
      X0 = amc.X0;
      L0 = amc.L0;
      A = amc.A;
      Z = amc.Z;
      rho = amc.rho;
      dEdX = amc.dEdX;
      zOaTr = amc.zOaTr;
      delete composition;
      composition =
          amc.composition ? new MaterialComposition(*amc.composition) : nullptr;
    }
    return (*this);
  }

  /** Move Assignment operator */
  Material& operator=(Material&& amc) {
    X0 = amc.X0;
    L0 = amc.L0;
    A = amc.A;
    Z = amc.Z;
    rho = amc.rho;
    dEdX = amc.dEdX;
    zOaTr = amc.zOaTr;
    if (composition && composition != amc.composition) {
      delete composition;
    }
    composition = amc.composition;
    amc.composition = nullptr;
    return *this;
  }

  /** scaling method */
  Material* scale(float sf) const;

  /** access to members */
  float zOverAtimesRho() const { return (*this).zOaTr; }
  float x0() const { return (*this).X0; }
  float averageZ() const { return (*this).Z; }

  /** spit out as a string */
  std::string toString() const {
    std::ostringstream sout;
    sout << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    sout << "(" << X0 << " | " << L0 << " | " << A << " | " << Z << " | " << rho
         << ")";
    return sout.str();
  }
};

inline Material* Material::scale(float sf) const {
  return new Material(X0 / sf, L0 / sf, sf * A, sf * Z, sf * rho);
}

}  // namespace Trk

#endif
