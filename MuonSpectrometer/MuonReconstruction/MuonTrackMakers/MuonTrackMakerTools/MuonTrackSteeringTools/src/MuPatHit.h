/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUPATHIT_H
#define MUPATHIT_H

#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"
#include "CxxUtils/atomic_fetch_minmax.h"

#include <list>
#include <mutex>

namespace Trk {
  class MeasurementBase;
  class ResidualPull;
}

namespace Muon {


  class MuPatHit {
  public:
    enum Type   { UnknownType = -1, MDT = 0, RPC = 1, TGC = 2, CSC = 3, MM = 4, sTGC = 5, PREC = 6, Pseudo = 7, Scatterer = 8 };
    enum Status { UnknownStatus = -1, OnTrack = 0, Outlier, NotOnTrack };
    enum HitSelection { UnknownSelection = -1, Precise = 0, Broad = 1 };

    struct Unowned
    {
      void operator() (const Trk::TrackParameters*) const {}
    };

    struct Info {
      Info() : id(),measuresPhi(false),type(UnknownType),selection(UnknownSelection),status(UnknownStatus) {}
      Identifier   id;
      bool         measuresPhi;
      Type         type;
      HitSelection selection;
      Status       status;

    };

    /** @brief construction taking all members as argument, ownership is taken only of the broadMeas.
      @param pars         predicted TrackParameters at the surface of the measurement
      @param presMeas     precisely calibrated measurement
      @param broadMeas    measurement with enlarged errors
      @param presResPull  residual and pull of the hit for the precise measurement
      @param broadResPull residual and pull of the hit for the broad measurement
      @param id           Hit Identifier (can be invalid (Pseudos), user should check validity)
      @param type         Hit type enum
      @param measuresPhi boolean indicating whether this is an eta or phi measurement
      @param used        enum indicating the hit status
    */
    MuPatHit( std::shared_ptr<const Trk::TrackParameters> pars,
              const Trk::MeasurementBase* presMeas,
              std::unique_ptr<const Trk::MeasurementBase> broadMeas,
              const Info& info );

    /** @brief copy constructor */
    MuPatHit( const MuPatHit& hit );

    /** assignment operator */
    MuPatHit& operator=( const MuPatHit& );

    /** destructor */
    ~MuPatHit();

    /** @brief returns a reference to the TrackParameters */
    const Trk::TrackParameters& parameters() const;

    /** @brief returns a reference to the selected measurement */
    const Trk::MeasurementBase& measurement() const;

    /** @brief returns a reference to the hit info */
    const Info& info() const;

    /** @brief returns a reference to the hit info */
    Info& info();

    /** @brief clones the MuPatHit */
    MuPatHit* clone() const { return new MuPatHit(*this); }

    /** @brief returns precise measurement */
    const Trk::MeasurementBase& preciseMeasurement() const;

    /** @brief returns broad measurement */
    const Trk::MeasurementBase& broadMeasurement() const;

    /** @brief update the track parameters and residual of a MuPatHit */
    void updateParameters( std::shared_ptr<const Trk::TrackParameters> pars );

    /** @brief maximum number of objects of this type in memory */
    static unsigned int maxNumberOfInstantiations();

    /** current number of objects of this type in memory */
    static unsigned int numberOfInstantiations();

    /** @brief number of times the copy constructor was called since last reset */
    static unsigned int numberOfCopies();

  private:
    //
    // private static functions
    //
    /** @brief Keeping track of number of object instances */
    static void addInstance();

    /** @brief Keeping track of number of object instances */
    static void removeInstance();

    //
    // private static data members
    //
    static std::atomic<unsigned int> s_numberOfInstantiations;    //<! current number of objects of this type in memory
    static std::atomic<unsigned int> s_maxNumberOfInstantiations; //<! maximum number of objects of this type in memory
    static std::atomic<unsigned int> s_numberOfCopies;            //<! number of times the copy constructor was called since last reset

    //
    // private member functions
    //
    /** no default constructor */
    MuPatHit() = delete;

    /** @brief copy hit */
    void copy( const MuPatHit& hit );

    // private member data
    std::shared_ptr<const Trk::TrackParameters> m_pars;
    const Trk::MeasurementBase* m_precisionMeas;
    std::unique_ptr<const Trk::MeasurementBase> m_broadMeas;
    Info m_info;

  }; // class MuPatHit



  /**
      List of MuPatHit pointers.
  */
  typedef std::list<MuPatHit*>         MuPatHitList;
  typedef MuPatHitList::const_iterator MuPatHitCit;
  typedef MuPatHitList::iterator       MuPatHitIt;


  //
  // static inline functions implementations
  //
  inline unsigned int MuPatHit::numberOfInstantiations() {
    return s_numberOfInstantiations;
  }

  inline unsigned int MuPatHit::maxNumberOfInstantiations() {
    return s_maxNumberOfInstantiations;
  }

  inline unsigned int MuPatHit::numberOfCopies() {
    return s_numberOfCopies;
  }

  inline void MuPatHit::addInstance() {
    ++s_numberOfInstantiations;
    CxxUtils::atomic_fetch_max (&s_maxNumberOfInstantiations,
                                s_numberOfInstantiations.load());
  }

  inline void MuPatHit::removeInstance() {
    --s_numberOfInstantiations;
  }

  //
  // inline member functions implementations
  //
  inline const Trk::TrackParameters& MuPatHit::parameters() const { return *m_pars;  }

  inline const Trk::MeasurementBase& MuPatHit::measurement() const {
    if( info().selection == Precise ) return *m_precisionMeas;
    return *m_broadMeas;
  }

  inline const Trk::MeasurementBase& MuPatHit::preciseMeasurement() const { return *m_precisionMeas; }

  inline const Trk::MeasurementBase& MuPatHit::broadMeasurement() const { return *m_broadMeas; }

  inline const MuPatHit::Info& MuPatHit::info() const { return m_info; }

  inline MuPatHit::Info& MuPatHit::info() { return m_info; }


} // Muon

#endif
