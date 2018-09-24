// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETASSOCIATIONHELPER_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETASSOCIATIONHELPER_V1_H

#include "AthContainers/DataVector.h"

#include "xAODMissingET/MissingETAssociation.h"
#include "xAODMissingET/MissingETAssociationMap.h"

#include <string>
#include <vector>

#include <iostream>

namespace xAOD
{

  class MissingETAssociationHelper_v1
  {
  public:

    /*! @brief Default constructor */
    MissingETAssociationHelper_v1();
    /*! @brief Construct with a map*/
    MissingETAssociationHelper_v1(const MissingETAssociationMap* map);
    /*! @brief Base class destructor */
    virtual ~MissingETAssociationHelper_v1();

    bool objSelected(const MissingETAssociation* assoc, size_t objIdx) const; /*!< @brief Test if object is selected for MET calculation */
    bool objSelected(const MissingETAssociation* assoc, const IParticle* pPart) const; /*!< @brief Test if object is selected for MET calculation */

    MissingETBase::Types::bitmask_t getObjSelectionFlags(const MissingETAssociation* assoc) const; /*!< @brief Return object selection bitmask for the given association */

    void setObjSelectionFlag(const MissingETAssociation* assoc, size_t objIdx, bool status); /*!< @brief Flag object as selected for MET calculation */
    void setObjSelectionFlag(const MissingETAssociation* assoc, const IParticle* pPart, bool status);  /*!< @brief Flag object as selected for MET calculation */

    inline void resetObjSelectionFlags() { m_useObjectFlags.clear(); }
		inline const MissingETAssociationMap* map() { return m_map; }

  private:

    const MissingETAssociationMap* m_map;

    std::vector<MissingETBase::Types::bitmask_t> m_useObjectFlags;  /*< @brief Cache overlap removal decisions */


  }; // class xAOD::MissingETAssociationHelper_v1
} // namespace xAOD

#endif
