/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef PADDATA_H
#define PADDATA_H


// local includes
#include <ostream>


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short abstract class to interface the data hit fragment output from the Pad TDS
   *
   * This class contains pure virtual methods to be implemented by the derived classes.
   * These latters implement the offline and online version of this data fragment.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *          Alaettin Serhan Mete <amete@cern.ch>
   *
   */

  class PadData {

  public:
    PadData();
    virtual ~PadData();

    //! methods for retrieving the bare data
    virtual uint32_t id()     const = 0;   //!< get the 32 bit word identifing the fragment
    virtual uint16_t BC_Tag() const = 0;   //!< get the BC Tag

    //! helper  methods decoding the hit fragment position within the NSW detector
    virtual int sideId()      const = 0;   //!< get the side            (0==sideC, 1==sideA)
    virtual int moduleId()    const = 0;   //!< get the physics module  (ranging from 1 at lowest R to 3 at highest R)
    virtual int sectorId()    const = 0;   //!< get the sector          (ranging from 1 to 8)
    virtual int sectorType()  const = 0;   //!< get the sector type     (ranging from 0 to 1, Small and Large) 
    virtual int multipletId() const = 0;   //!< get the multiplet       (ranging from 1 to 2, 1 is at lowest |z|)
    virtual int gasGapId()    const = 0;   //!< get the gas gap         (ranging from 1 to 4, 1 is at lowest |z|)
    virtual int channelId()   const = 0;   //!< get the channel
    virtual int padEtaId()    const = 0;   //!< get the pad eta channel 
    virtual int padPhiId()    const = 0;   //!< get the pad phi channel 

  };  // end of PadData class

  inline PadData::PadData() {}

  inline PadData::~PadData() {}

} // namespace NSWL1

std::ostream& operator<<(std::ostream& stream, NSWL1::PadData& data);

#endif
