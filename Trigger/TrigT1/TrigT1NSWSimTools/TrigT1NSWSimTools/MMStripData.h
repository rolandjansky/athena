/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSTRIPDATA_H
#define MMSTRIPDATA_H


// local includes
#include <stdint.h>
#include <ostream>

// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short abstract class to interface the data hit fragment output from the MM Strip TDS
   *
   * This class contains pure virtual methods to be implemented by the derived classes.
   * These latters implement the offline and online version of this data fragment.
   *
   *  @authors Alessandro Di Mattia <dimattia@cern.ch>, Geraldine Conti <geraldine.conti@cern.ch>
   *
   *
   */

  class MMStripData {

  public:
    MMStripData();
    virtual ~MMStripData();

    //! methods for retrieving the bare data
    virtual uint32_t id()        const = 0; //!< get the 32 bit word identifing the fragment

    //virtual float setStripCharge() const = 0; //!< get the strip charge
    virtual int trig_BCID()      const = 0;

    //! helper  methods decoding the hit fragment position within the NSW detector
    virtual int sideId()      const = 0;    //!< get the side (0==sideC, 1==sideA)
    virtual int moduleId()    const = 0;    //!< get the physics module (ranging from 0 at lowest R to 1 at highest R)
    virtual int sectorId()    const = 0;    //!< get the sector    (ranging from 0 to 15)
    virtual int multipletId() const = 0;    //!< get the multiplet (ranging from 0 to 1, 1 is at lowest |z|)
    virtual int gasGapId()    const = 0;    //!< get the gas gap   (ranging from 0 to 3, 1 is at lowest |z|)
    virtual int channelId()   const = 0;    //!< get the channel

  };  // end of StripData class

  inline MMStripData::MMStripData() {}

  inline MMStripData::~MMStripData() {}

} // namespace NSWL1

#endif
