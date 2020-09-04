/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKGXFMATERIALEFFECTS_H
#define TRKGXFMATERIALEFFECTS_H

#include "TrkMaterialOnTrack/EnergyLoss.h"

#include <memory>

/**
 * These headers, as well as other headers in the TrkGlobalChi2Fitter package
 * use modern C++11 memory ownership semantics expressed through smart
 * pointers. See GlobalChi2Fitter.h for more information.
 */

namespace Trk {
  class MaterialEffectsOnTrack;
  class MaterialEffectsBase;
  class EnergyLoss;
  class Surface;
  class MaterialProperties;

  /**
   * @brief class that is similar to MaterialEffectsOnTrack, but has 'set' methods for more flexibility during track fitting

   @author thijs.cornelissen@cern.ch
  */

  class GXFMaterialEffects {

  public:
    GXFMaterialEffects();

    /**
     * @brief Constructor that captures the properties of a more standard
     * MaterialEffectsOnTrack object.
     *
     * This is one of the possible constructors for GXFMaterialEffects. It
     * takes as an argument a MaterialEffectsOnTrack, which is the more widely
     * used class to represent material effects. It copies the properties of
     * the passed object pointer.
     *
     * @note This method assumes no ownership of the passed pointer whatsoever,
     * and ownership of the MaterialEffectsOnTrack remains the responsibility
     * of the original owner.
     */
    GXFMaterialEffects(const MaterialEffectsOnTrack *);
    GXFMaterialEffects(GXFMaterialEffects &);
    GXFMaterialEffects & operator=(GXFMaterialEffects & rhs);

    void setScatteringAngles(double, double);
    void setScatteringSigmas(double, double);
    double x0() const;
    void setX0(double);
    double deltaPhi() const;
    double measuredDeltaPhi() const;
    void setMeasuredDeltaPhi(double);
    double deltaTheta() const;
    double sigmaDeltaPhi() const;
    double sigmaDeltaTheta() const;
    double deltaE() const;

    /**
     * @brief Set the energy loss properties of this material effect.
     *
     * This method sets the energy loss properties of the material effects
     * instance through an instance of Trk::EnergyLoss.
     *
     * @note This method assumes full ownership of the passed pointer.
     */
    void setEloss(std::unique_ptr<EnergyLoss>);
    double sigmaDeltaE() const;
    double sigmaDeltaEPos() const;
    double sigmaDeltaENeg() const;
    double sigmaDeltaEAve() const;
    void setDeltaE(double);
    void setSigmaDeltaE(double);
    void setSigmaDeltaEPos(double);
    void setSigmaDeltaENeg(double);
    double delta_p() const;
    void setdelta_p(double);
    void setKink(bool);
    bool isKink() const;
    void setMeasuredEloss(bool);
    bool isMeasuredEloss() const;
    const Surface *surface() const;

    /**
     * @brief Set the surface for this material effects instance.
     *
     * This method links the instance of GXFMaterialEffects to a surface. The
     * surface can then be accessed through the surface() method.
     *
     * @note This method does NOT assume ownership of the passed pointer.
     */
    void setSurface(const Surface *);
    std::unique_ptr<MaterialEffectsBase> makeMEOT();
    const MaterialProperties *materialProperties() const;

    /**
     * @brief Set the material properties of this material effects instance.
     *
     * @note The argument pointer is a raw, non-owning pointer and the method
     * assumes no ownership of the pointee.
     */
    void setMaterialProperties(const MaterialProperties *);

  private:
    double m_scatphi;
    double m_scattheta;
    double m_sigmascatphi;
    double m_sigmascattheta;
    double m_x0;
    double m_deltap;
    double m_deltae;
    double m_sigmadeltae;
    double m_sigmadeltaepos;
    double m_sigmadeltaeneg;
    std::unique_ptr<const EnergyLoss> m_eloss;
    const Surface *m_surf;
    const MaterialProperties *m_matprop;
    bool m_iskink;
    bool m_ismeasuredeloss;
    double m_measscatphi; // fudge to stabilize fit in muon system
    double m_sintheta;
  };
}
#endif
