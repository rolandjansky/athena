/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SHOWER_LIB_SHOWER_H
#define SHOWER_LIB_SHOWER_H

// STL includes
#include <vector>
#include <string>
#include <iostream>

// local includes
// #include "LArG4Code/EnergySpot.h"

#include "DataModel/DataVector.h"
#include "LArG4ShowerLib/ShowerEnergySpot.h"

// Namespace for the ShowerLib related classes
namespace ShowerLib {

  /**
   *
   *   @short Class for shower library shower
   *
   *          Store single shower (collection of hits)
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version \$Id: Shower.h 489536 2012-03-20 01:26:37Z gsedov $
   *
   */

  class Shower : public std::vector<ShowerEnergySpot*> {
//  class Shower : public DataVector<ShowerEnergySpot> {

  public:
  
    //! empty constructor
    Shower() : m_rsize(0.0), m_zsize(0.0) {}

    Shower(float r, float z) : m_rsize(r), m_zsize(z) {}

    inline float getZSize() const;
    inline float getRSize() const;

    inline void setZSize(const float zsize);
    inline void setRSize(const float rsize);


  private:

    float m_rsize;          //!< r size for containment check
    float m_zsize;          //!< z size for containment check

  };

  // inline functions
  float Shower::getZSize() const { return m_zsize; }
  float Shower::getRSize() const { return m_rsize; }

  void Shower::setZSize(const float zsize) { m_zsize = zsize; }
  void Shower::setRSize(const float rsize) { m_rsize = rsize; }


} // namespace ShowerLib

#endif // SHOWER_LIB_SHOWER_H
