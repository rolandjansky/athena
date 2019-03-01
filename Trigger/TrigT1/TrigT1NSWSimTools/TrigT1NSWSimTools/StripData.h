/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPDATA_H
#define STRIPDATA_H


// local includes
#include <stdint.h>
#include <ostream>


class Identifier;
// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short abstract class to interface the data hit fragment output from the Strip TDS
   *
   * This class contains pure virtual methods to be implemented by the derived classes.
   * These latters implement the offline and online version of this data fragment.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *
   *
   */

  class StripData {

  public:
    StripData();
    virtual ~StripData();

    //! methods for retrieving the bare data
    virtual uint32_t id()        const = 0; //!< get the 32 bit word identifing the fragment
    virtual Identifier Identity()  const = 0; //!< get the 32 bit word identifing the fragment
    virtual int trig_BCID()      const = 0; //!< get the trigger BCID
    virtual float strip_charge() const = 0; //!< get the strip charge
    virtual int strip_charge_6bit() const = 0; //!< get the strip charge in 6 bit
    virtual int strip_charge_10bit() const = 0; //!< get the strip charge in 10 bit
    virtual int bandId()         const = 0; //!< get the band id
    virtual void setBandId(int band_id)= 0; //!< get the band id
    virtual void setPhiId(int band_id)= 0; //!< get the band id
    virtual int phiId()          const = 0; //!< get the phi id
    virtual bool readStrip()   const = 0;
    virtual void set_readStrip(bool readStrip) = 0;
    //! helper  methods decoding the hit fragment position within the NSW detector
    virtual int sideId()      const = 0;    //!< get the side (0==sideC, 1==sideA)
    virtual int isSmall()      const = 0;    //!< get the side (0==sideC, 1==sideA)
    virtual int moduleId()    const = 0;    //!< get the physics module (ranging from 0 at lowest R to 3 at highest R)
    virtual int sectorId()    const = 0;    //!< get the sector (ranging from 0 to 15)
    virtual int wedge() const = 0;    //!< get the multiplet (ranging from 0 to 1, 1 is at lowest |z|)
    virtual int layer()    const = 0;    //!< get the gas gap (ranging from 0 to 3, 1 is at lowest |z|)
    virtual int channelId()   const = 0;    //!< get the channel

    virtual char type()    const = 0;
    virtual std::string stationName() const = 0;
    // for checking BC
    virtual float time()    const = 0;    //!< global X pos
    //! Use this for clustering
    virtual float globX()    const = 0;    //!< global X pos
    virtual float globY()    const = 0;    //!< global Y pos 
    virtual float globZ()    const = 0;    //!< globzl Z pos 
    virtual float locX()    const = 0;    //!< local X pos
    virtual float locY()    const = 0;    //!< local Y pos 
    virtual float locZ()    const = 0;    //!< local Z pos 


  };  // end of StripData class

  inline StripData::StripData() {}

  inline StripData::~StripData() {}

} // namespace NSWL1

#endif
