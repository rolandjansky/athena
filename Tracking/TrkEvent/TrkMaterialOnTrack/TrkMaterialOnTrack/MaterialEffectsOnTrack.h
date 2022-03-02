/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKMATERIALEFFECTSONTRACK_MATERIALEFFECTSONTRACK_H
#define TRKMATERIALEFFECTSONTRACK_MATERIALEFFECTSONTRACK_H

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include <iosfwd>
#include <memory>
#include <optional>

class MsgStream;
class TrackCollectionCnv;
class MaterialEffectsOnTrackCnv_p2;
class ScatteringAngleOnTrackCnv_p1;

namespace Trk {

class Surface;

/** @brief  represents the full description of deflection and e-loss
    of a track in material.

    This class is part of the ATLAS tracking EDM and extends the
    Trk::MaterialOnTrack base class known to the track.
    It holds two objects, one representing the deflection angles wrt.
    the track's theta and phi and their uncertainties, the other
    the energy loss and its uncertainties. Both objects are passed by
    pointer, implying that Trk::MaterialEffectsOnTrack will
    take ownership of them and that not both of them need to be present.

    @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/>
  */

class MaterialEffectsOnTrack : public MaterialEffectsBase
{

public:
  //! default constructor for POOL - do not use in reconstruction!
  MaterialEffectsOnTrack();
  /** @brief full constructor passing (with ownership) both scattering
      angle and energy loss

      @param[in] tInX0 layer thickness in terms of rad length
      @param[in] scat  pointer to ScatteringAngles object (passing ownership)
      @param[in] eloss pointer to EnergyLoss base class (passing ownership)
      @param[in] assocSurf Surface on which material effects are expressed
      @param[in] typePattern bitset scheme to characterise type of MEOT.
   */
  MaterialEffectsOnTrack(
    const double tInX0,
    std::optional<ScatteringAngles> scat,
    const Trk::EnergyLoss* eloss,
    const Surface& assocSurf,
    const std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>&
      typePattern =
        std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>());

  /** @brief partial constructor passing (with ownership) a scattering angle
   object
   @param[in] tInX0 layer thickness in terms of rad length
   @param[in] scat  pointer to ScatteringAngles object (passing ownership)
   @param[in] assocSurf surface on which material effects are expressed
   @param[in] typePattern bitset scheme to characterise type of MEOT.
   */
  MaterialEffectsOnTrack(
    const double tInX0,
    std::optional<ScatteringAngles> scat,
    const Surface& assocSurf,
    const std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>&
      typePattern =
        std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>());

  /** @brief partial constructor passing (with ownership) an energy loss object
   @param[in] tInX0 layer thickness in terms of rad length
   @param[in] eloss pointer to EnergyLoss object (passing ownership)
   @param[in] assocSurf surface on which material effects are expressed
   @param[in] typePattern bitset scheme to characterise type of MEOT.
  */
  MaterialEffectsOnTrack(
    const double tInX0,
    const EnergyLoss* eloss,
    const Surface& assocSurf,
    const std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>&
      typePattern =
        std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>());

  /** @brief partial constructor passing (with ownership) an energy loss object
   @param[in] tInX0 layer thickness in terms of rad length
   @param[in] eloss pointer to EnergyLoss object (passing ownership)
   @param[in] assocSurf surface on which material effects are expressed
   @param[in] typePattern bitset scheme to characterise type of MEOT.
  */
  MaterialEffectsOnTrack(
    const double tInX0,
    std::unique_ptr<const EnergyLoss> eloss,
    const Surface& assocSurf,
    const std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>&
      typePattern =
        std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>());

  /** @brief partial constructor with only a thickness, input to ME-Updator
   @param[in] tInX0 layer thickness in terms of rad length
   @param[in] assocSurf surface on which material effects are expressed
   The type will be defaulted to MaterialUpdatorInput.
   */
  MaterialEffectsOnTrack(const double tInX0, const Surface& assocSurf);

  //! Copy constructor
  MaterialEffectsOnTrack(const MaterialEffectsOnTrack& meot);

  //! Assignment operator
  MaterialEffectsOnTrack& operator=(const MaterialEffectsOnTrack& rhs);

  //! Moveconstructor
  MaterialEffectsOnTrack(MaterialEffectsOnTrack&& meot) noexcept = default;

  //! Assignment operator
  MaterialEffectsOnTrack& operator=(MaterialEffectsOnTrack&& rhs) noexcept =
    default;

  //! virtual destructor
  virtual ~MaterialEffectsOnTrack();

  /** @brief returns the MCS-angles object. Careful: DO NOT DELETE!
             Pointer may be NULL if no MCS but only energy loss is contained. */
  const ScatteringAngles* scatteringAngles() const;

  /** @brief returns the energy loss object. Careful: DO NOT DELETE!
             Pointer may be NULL if no E loss but only scattering angles are
     contained. */
  const EnergyLoss* energyLoss() const;

  //! Virtual constructor
  virtual MaterialEffectsOnTrack* clone() const;

  //! NVI uniqueClone
  std::unique_ptr<MaterialEffectsOnTrack> uniqueClone() const
  {
    return std::unique_ptr<MaterialEffectsOnTrack>(clone());
  }

  //! Interface method for output, implemented in child class
  virtual MsgStream& dump(MsgStream& sl) const;
  //! Interface method for output, implemented in child class
  virtual std::ostream& dump(std::ostream& sl) const;

protected:
private:
  friend class ::MaterialEffectsOnTrackCnv_p2;
  friend class ::ScatteringAngleOnTrackCnv_p1;

  //! holds the scattering angles+errors in a separate object
  std::optional<ScatteringAngles> m_scatteringAngles = std::nullopt;
  //! holds the measured and/or estimated energy loss in a separate object
  std::unique_ptr<const EnergyLoss> m_energyLoss = nullptr;
};

} // end of Trk ns

inline const Trk::ScatteringAngles*
Trk::MaterialEffectsOnTrack::scatteringAngles() const
{
  if (m_scatteringAngles == std::nullopt) {
    return nullptr;
  }
  return &(*m_scatteringAngles);
}

inline const Trk::EnergyLoss*
Trk::MaterialEffectsOnTrack::energyLoss() const
{
  return m_energyLoss.get();
}

inline Trk::MaterialEffectsOnTrack*
Trk::MaterialEffectsOnTrack::clone() const
{
  return new MaterialEffectsOnTrack(*this);
}

#endif

