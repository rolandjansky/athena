/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DNA_MaterialEffects.h
//   Class grouping variables for saving DNA values
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_DNA_MATERIALEFFECTS_H
#define TRK_DNA_MATERIALEFFECTS_H

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkSurfaces/SurfaceHolders.h"
#include <memory>

namespace Trk {

class MaterialEffectsBase;

/** @brief Class to store temporary values of dynamic noise adjustment
during Kalman filtering and before the final MaterialEffectOnTrack is
written on the track.

@author V. Kartvelishvili, W. Liebig <http://consult.cern.ch/xwho>

*/
class DNA_MaterialEffects : public SurfacePtrHolderDetEl
{
public:
  //! default constructor - needed to make a vector of these objects
  DNA_MaterialEffects();
  //! full constructor
  DNA_MaterialEffects(double dna_xmin,
                      double dna_dxmin,
                      double chi2min,
                      double thicknessInX0,
                      double sigmaDNA,
                      double addSigmaQoverP,
                      bool foundByForwardFilter,
                      const Surface& assocSurf);

  DNA_MaterialEffects(const DNA_MaterialEffects&) = default;
  DNA_MaterialEffects(DNA_MaterialEffects&&) = default;
  DNA_MaterialEffects& operator=(const DNA_MaterialEffects&) = default;
  DNA_MaterialEffects& operator=(DNA_MaterialEffects&&) = default;
  //! default desctructor
  ~DNA_MaterialEffects() = default;

  //! returns the inverse retained energy fraction @f$ xmin=1./z @f$
  double xmin() const;
  //! returns the uncertainty on the retained inv. E fraction @f$ \Delta z @f$
  double dxmin() const;
  //! returns the min @f$ \chi^2 @f$ from the trial-updates (used to seed DNA)
  //! */
  double chi2min() const;
  //! returns the @f$ t/X_0 @f$
  double thicknessInX0() const;
  //! returns  @f$ \sigma_{DNA} @f$
  double sigmaDNA() const;
  /** @brief returns the additional noise term @f$ \sigma{noise} @f$.

      Will be squared when filled into the EDM MaterialEffectsOnTrack,
      which demand @f$ \sigma^2 @f$. */
  double addSigmaQoverP() const;
  //! returns flag if DNA point comes from forward or backward filter
  bool foundByForwardFilter() const;

  /** @brief sets and returns the momentum change and its error.

      This quantity can alternatively be calculated outside the DNA when
      more information about the track is available. For this version
      therefore the quantity is not part of the constructor.
      The quantities are mainly aimed at completing the information
      for the EDM class EstimatedBremOnTrack. */
  void setDeltaP(double deltaP, double sigmaDeltaP = 0.0);
  double deltaP() const;
  double sigmaDeltaP() const;

  //! returns the surface to which this mefot is associated to
  const Surface& associatedSurface() const;

  //! make a Mefot class of the official EDM
  const MaterialEffectsBase* makeMEFOT() const;
  
    //! make a Mefot class of the official EDM
  std::unique_ptr<MaterialEffectsBase> makeUniqueMEFOT() const;

private:
  double m_xmin, m_dxmin, m_chi2min, m_thickness, m_sigmaDNA, m_addSigmaQoverP;
  bool m_foundByForwardFilter;
  double m_deltaP;
  double m_sigmaDeltaP;
};

//! Overload of << operator for MsgStream for debug output
MsgStream&
operator<<(MsgStream& sl, const DNA_MaterialEffects& dme);
//! Overload of << operator for std::ostream for debug output
std::ostream&
operator<<(std::ostream& sl, const DNA_MaterialEffects& dme);

inline double
DNA_MaterialEffects::xmin() const
{
  return m_xmin;
}
inline double
DNA_MaterialEffects::dxmin() const
{
  return m_dxmin;
}
inline double
DNA_MaterialEffects::chi2min() const
{
  return m_chi2min;
}
inline double
DNA_MaterialEffects::thicknessInX0() const
{
  return m_thickness;
}
inline double
DNA_MaterialEffects::sigmaDNA() const
{
  return m_sigmaDNA;
}
inline double
DNA_MaterialEffects::addSigmaQoverP() const
{
  return m_addSigmaQoverP;
}
inline void
DNA_MaterialEffects::setDeltaP(double deltaP, double sigmaDeltaP)
{
  m_deltaP = deltaP;
  m_sigmaDeltaP = sigmaDeltaP;
}
inline double
DNA_MaterialEffects::deltaP() const
{
  return m_deltaP;
}
inline double
DNA_MaterialEffects::sigmaDeltaP() const
{
  return m_sigmaDeltaP;
}
inline bool
DNA_MaterialEffects::foundByForwardFilter() const
{
  return m_foundByForwardFilter;
}

} // end namespace Trk
#endif
