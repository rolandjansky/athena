/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_LAYERMATERIALMAP_H
#define TRKGEOMETRY_LAYERMATERIALMAP_H

#include <map>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/LayerMaterialProperties.h"

namespace Trk {

/** @class LayerMaterialMap

This class extends the DataVector<Trk::LayerMaterialProperties> by an
elementTable;

    @author Andreas.Salzburger -at- cern.ch
*/

class LayerMaterialMap
    : public std::map<Trk::LayerIndex, const Trk::LayerMaterialProperties*> {
 public:
  /** Default constructor */
  LayerMaterialMap()
      : std::map<Trk::LayerIndex, const Trk::LayerMaterialProperties*>(),
        m_elementTable(SharedObject<const ElementTable>(0)) {}

  /** Default constructor */
  LayerMaterialMap(const Trk::SharedObject<const ElementTable>& eTable)
      : std::map<Trk::LayerIndex, const Trk::LayerMaterialProperties*>(),
        m_elementTable(eTable) {}

  /** Copy constructor */
  LayerMaterialMap(const Trk::LayerMaterialMap& lmc)
      : std::map<Trk::LayerIndex, const Trk::LayerMaterialProperties*>(lmc),
        m_elementTable(lmc.m_elementTable) {}

  /** Assigment operator */
  LayerMaterialMap& operator=(const LayerMaterialMap& lmc) {
    if (this != &lmc) {
      m_elementTable = lmc.m_elementTable;
      std::map<Trk::LayerIndex, const Trk::LayerMaterialProperties*>::operator=(
          lmc);
    }
    return (*this);
  }

  /** Destructor */
  ~LayerMaterialMap();

  /** retrieval method */
  const ElementTable* elementTable() const { return m_elementTable.get(); }

  /** update method*/
  void updateElementTable(const SharedObject<const ElementTable>& eTable) {
    m_elementTable = eTable;
    synchronizeElementTable();
  }

  /** synchronize the ElementTable */
  void synchronizeElementTable();

 private:
  SharedObject<const ElementTable> m_elementTable;
};

inline LayerMaterialMap::~LayerMaterialMap() {
  // delete the entries to act like a DataVector
  for (auto& it : (*this)) delete (it.second);
}

inline void LayerMaterialMap::synchronizeElementTable() {
  // loop 1 - add up all the ElementTable
  Trk::ElementTable* eTable = new Trk::ElementTable();
  if (elementTable()) (*eTable) += (*elementTable());
  for (auto& it : (*this)) {
    const Trk::ElementTable* eTableLM = it.second->elementTable();
    if (eTableLM) (*eTable) += (*eTableLM);
  }
  // loop 2 - set the updated ElementTable
  m_elementTable = SharedObject<const ElementTable>(eTable);
  for (auto& it : (*this)) it.second->updateElementTable(m_elementTable);
}

}  // namespace Trk

CLASS_DEF(Trk::LayerMaterialMap, 142190734, 1)

// 142190734 Trk::LayerMaterialMap None

#endif
