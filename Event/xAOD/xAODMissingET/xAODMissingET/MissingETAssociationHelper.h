// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_MISSINGETASSOCIATIONHELPER_H
#define XAODMISSINGET_MISSINGETASSOCIATIONHELPER_H

#include "xAODMissingET/versions/MissingETAssociation_v1.h"
#include "xAODMissingET/versions/MissingETAssociationMap_v1.h"

#include <string>
#include <vector>

#include <iostream>

namespace xAOD
{

  // Forward declarations, since this class introduces cyclical dependences
  class MissingETAssociation_v1;
  class MissingETAssociationMap_v1;

  class MissingETAssociationHelper
  {
  public:

    /*! @brief Default constructor */
    MissingETAssociationHelper();
    /*! @brief Construct with a map*/
    MissingETAssociationHelper(const MissingETAssociationMap_v1* map);
    /*! @brief Base class destructor */
    virtual ~MissingETAssociationHelper();

    bool objSelected(const MissingETAssociation_v1* assoc, size_t objIdx) const; /*!< @brief Test if object is selected for MET calculation */
    bool objSelected(const MissingETAssociation_v1* assoc, const IParticle* pPart) const; /*!< @brief Test if object is selected for MET calculation */

    MissingETBase::Types::bitmask_t getObjSelectionFlags(const MissingETAssociation_v1* assoc) const; /*!< @brief Return object selection bitmask for the given association */

    void setObjSelectionFlag(const MissingETAssociation_v1* assoc, size_t objIdx, bool status); /*!< @brief Flag object as selected for MET calculation */
    void setObjSelectionFlag(const MissingETAssociation_v1* assoc, const IParticle* pPart, bool status);  /*!< @brief Flag object as selected for MET calculation */

    inline void resetObjSelectionFlags() { m_useObjectFlags.clear(); }
    inline const MissingETAssociationMap_v1* map() const { return m_map; }

  private:

    const MissingETAssociationMap_v1* m_map;

    std::vector<MissingETBase::Types::bitmask_t> m_useObjectFlags;  /*< @brief Cache overlap removal decisions */


  }; // class xAOD::MissingETAssociationHelper
} // namespace xAOD

#endif
