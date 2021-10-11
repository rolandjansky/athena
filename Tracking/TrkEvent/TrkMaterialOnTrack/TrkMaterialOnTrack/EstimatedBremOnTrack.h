/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EstimatedBremOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKMATERIALONTRACK_EstimatedBremOnTrack_H
#define TRKMATERIALONTRACK_EstimatedBremOnTrack_H

#include "TrkEventPrimitives/PropDirection.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include <iosfwd>
#include <memory>

class MsgStream;

namespace Trk {

class Surface;

/** @brief class holding information about momentum reduction and an additional
           noise term due to significant energy loss (bremsstrahlung)

    @author Wolfgang.Liebig <http://consult.cern.ch/xwho/people/>

*/
class EstimatedBremOnTrack final : public MaterialEffectsBase
{
public:
  //! default constructor for POOL
  EstimatedBremOnTrack() = default;
  /** @brief Full constructor.
      @param[in] thicknessInX0 material thickness in terms of rad length, @f$
       t/X_0 @f$
      @param[in] retainedEnFraction retained energy fraction after brem, @f$ z
       @f$
      @param[in] sigmaRetainedEnFraction error on estimated retained energy
      fraction,
       @f$ \sigma(z) @f$
      @param[in] sigmaQoverPsquared noise term added to cov matrix momentum
      parameter, @f$ \sigma^2(q/p) @f$
      @param[in] sf tracking surface with which material effects are associated
      @param[in] searchDirection for progressive brem recovery or seeded tracks
  */
  EstimatedBremOnTrack(double thicknessInX0,
                       double retainedEnFraction,
                       double sigmaRetainedEnFraction,
                       double sigmaQoverPsquared,
                       const Surface& sf,
                       const SearchDirection searchDirection = bothway);
  //! Copy constructor
  EstimatedBremOnTrack(const EstimatedBremOnTrack& ebrot) = default;
  //! Assignment operator
  EstimatedBremOnTrack& operator=(const EstimatedBremOnTrack& rhs) = default;
  //! Move constructor
  EstimatedBremOnTrack(EstimatedBremOnTrack&& ebrot) noexcept = default;
  //! Move Assignment operator
  EstimatedBremOnTrack& operator=(EstimatedBremOnTrack&& rhs) noexcept = default;
  //! Destructor
  virtual ~EstimatedBremOnTrack() = default;

  //! Virtual constructor
  virtual EstimatedBremOnTrack* clone() const override final;

  //! NVI uniqueClone
  std::unique_ptr<EstimatedBremOnTrack> uniqueClone() const
  {
    return std::unique_ptr<EstimatedBremOnTrack>(clone());
  }

  //! returns @f$ z @f$
  double retainedEnFraction() const;

  //! return @f$ \sigma(z) @f$
  double sigmaRetainedEnFraction() const;

  //! returns the additional noise term @f$ Q=\sigma^2(q/p) @f$
  double sigmaQoverPsquared() const;

  //! returns if the brem was discovered with a particular search direction
  SearchDirection searchDirection() const;

  //! Interface method for output.
  virtual MsgStream& dump(MsgStream& sl) const override final;
  //! Interface method for output
  virtual std::ostream& dump(std::ostream& sl) const override final;

private:
  //! @f$ z @f$       - the momentum retained after energy loss
  double m_retainedEnFraction{ 0.0 };
  //! @f$ \sigma(z) @f$ - uncertainty on the momentum retained after Eloss
  double m_sigmaRetainedEnFraction{ 0.0 };
  //! @f$ \sigma^2(q/p) @f$  - the applied straggling on the momentum correction
  double m_sigmaQoverPsquared{ 0.0 };

  //! if brem was found in particular direction, outward, inward or global
  SearchDirection m_bremSearchDirection{ Trk::undefinedDirection };
};

inline EstimatedBremOnTrack*
EstimatedBremOnTrack::clone() const
{
  return new EstimatedBremOnTrack(*this);
}

inline double
EstimatedBremOnTrack::retainedEnFraction() const
{
  return m_retainedEnFraction;
}

inline double
EstimatedBremOnTrack::sigmaRetainedEnFraction() const
{
  return m_sigmaRetainedEnFraction;
}

inline double
EstimatedBremOnTrack::sigmaQoverPsquared() const
{
  return m_sigmaQoverPsquared;
}

inline SearchDirection
EstimatedBremOnTrack::searchDirection() const
{
  return m_bremSearchDirection;
}

} // end ns

#endif // TRKMATERIALONTRACK_ESTIMATEDBREMONTRACK_H
