/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPCLUSTERDATA_H
#define STRIPCLUSTERDATA_H

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

  class StripClusterData {

  public:
    StripClusterData();
    virtual ~StripClusterData();

    //! methods for retrieving the bare data
    virtual int bandId()         const = 0; //!< get the band id
    //! helper  methods decoding the hit fragment position within the NSW detector
    virtual int isSmall()      const = 0;    //!< get the side (0==sideC, 1==sideA)
    virtual int moduleId()    const = 0;    //!< get the physics module (ranging from 0 at lowest R to 3 at highest R)
    virtual int sectorId()    const = 0;    //!< get the sector (ranging from 0 to 15)
    virtual int wedge() const = 0;    //!< get the multiplet (ranging from 0 to 1, 1 is at lowest |z|)
    virtual int layer()    const = 0;    //!< get the gas gap (ranging from 0 to 3, 1 is at lowest |z|)
    virtual int size()      const = 0;
    virtual float charge() const = 0;
    virtual float globX()   const = 0;    //!< global X pos
    virtual float globY()   const = 0;    //!< global Y pos 
    virtual float globZ()    const = 0;    //!< globzl Z pos 
 

  };  // end of StripClusterData class

  inline StripClusterData::StripClusterData() {}

  inline StripClusterData::~StripClusterData() {}

} // namespace NSWL1

#endif
