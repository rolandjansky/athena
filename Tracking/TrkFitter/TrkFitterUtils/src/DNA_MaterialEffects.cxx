/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DNA_MaterialEffects.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include <string>

Trk::DNA_MaterialEffects::DNA_MaterialEffects()
  : SurfacePtrHolderDetEl()
  , m_xmin(0.)
  , m_dxmin(0.)
  , m_chi2min(0.)
  , m_thickness(0.)
  , m_sigmaDNA(0.)
  , m_addSigmaQoverP(0.)
  , m_foundByForwardFilter(false)
  , m_deltaP(0.)
  , m_sigmaDeltaP(0.)
{}

Trk::DNA_MaterialEffects::DNA_MaterialEffects(double dna_x,
                                              double dna_dx,
                                              double chi2min,
                                              double thicknessInX0,
                                              double sigmaDNA,
                                              double addSigmaQoverP,
                                              bool foundByForwardFilter,
                                              const Surface& assocSurf)
  : SurfacePtrHolderDetEl(assocSurf)
  , m_xmin(dna_x)
  , m_dxmin(dna_dx)
  , m_chi2min(chi2min)
  , m_thickness(thicknessInX0)
  , m_sigmaDNA(sigmaDNA)
  , m_addSigmaQoverP(addSigmaQoverP)
  , m_foundByForwardFilter(foundByForwardFilter)
  , m_deltaP(0.)
  , m_sigmaDeltaP(0.)
{}

const Trk::Surface&
Trk::DNA_MaterialEffects::associatedSurface() const
{
  return *m_associatedSurface;
}

const Trk::MaterialEffectsBase*
Trk::DNA_MaterialEffects::makeMEFOT() const
{
  // trouble: can not turn deltaP into retainedEnFraction w/o TrackPars
  return new Trk::EstimatedBremOnTrack(m_thickness,
                                       1.0,
                                       0.0,
                                       m_addSigmaQoverP * m_addSigmaQoverP,
                                       *m_associatedSurface,
                                       m_foundByForwardFilter ? outside
                                                              : inside);
}

std::unique_ptr<Trk::MaterialEffectsBase> 
Trk::DNA_MaterialEffects::makeUniqueMEFOT() const{
return std::make_unique<Trk::EstimatedBremOnTrack>(m_thickness,
                                       1.0,
                                       0.0,
                                       m_addSigmaQoverP * m_addSigmaQoverP,
                                       *m_associatedSurface,
                                       m_foundByForwardFilter ? outside
                                                              : inside);
}

/**Overload of << operator for MsgStream and std::ostream for debug output*/
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::DNA_MaterialEffects& dme)
{
  sl << "DNA_MaterialEffects :\n\t( xmin,    dxmin,   chi2min, thickness, "
        "sigmaDNA, sigma(q/p),  dP    ) = \n\t"
     << "(" << dme.xmin() << ", " << dme.dxmin() << ", " << dme.chi2min()
     << ", " << dme.thicknessInX0() << ", " << dme.sigmaDNA() << ", "
     << dme.addSigmaQoverP() << ", " << dme.deltaP() << ", "
     << dme.sigmaDeltaP() << ")";
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::DNA_MaterialEffects& dme)
{
  sl << "DNA_MaterialEffects :\n\t( xmin,    dxmin,   chi2min, thickness, "
        "sigmaDNA, sigma(q/p), dP    ) = \n\t"
     << "(" << dme.xmin() << ", " << dme.dxmin() << ", " << dme.thicknessInX0()
     << ", " << dme.sigmaDNA() << ", " << dme.addSigmaQoverP() << ", "
     << dme.deltaP() << ", " << dme.sigmaDeltaP() << ")";
  return sl;
}
