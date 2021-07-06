/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElementTable.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ELEMENTTABLE_H
#define TRKGEOMETRY_ELEMENTTABLE_H

#include <climits>
#include <memory>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkGeometry/Material.h"

class MsgStream;

namespace Trk {

/** @class ElementTable */
class ElementTable {
 public:
  /** Empty constructor */
  ElementTable()
      : m_table(static_cast<size_t>(UCHAR_MAX), (Material*)nullptr),
        m_names(static_cast<size_t>(UCHAR_MAX), std::string("None")) {}

  /** Empty constructor */
  ElementTable(const ElementTable& et)
      : m_table(static_cast<size_t>(UCHAR_MAX), (Material*)nullptr),
        m_names(et.m_names) {
    for (size_t im = 0; im < et.m_table.size(); ++im) {
      if (et.m_table[im]) m_table[im] = new Trk::Material(*et.m_table[im]);
    }
  }

  /** Destructor */
  ~ElementTable();

  /** Assignment operator*/
  ElementTable& operator=(const ElementTable& et) {
    if (this != &et) {
      // clear first, then copy
      auto elIter = m_table.begin();
      for (; elIter != m_table.end(); delete (*elIter), ++elIter)
        ;
      m_names = et.m_names;
      for (size_t im = 0; im < et.m_table.size(); ++im) {
        m_table[im] = nullptr;
        if (et.m_table[im]) m_table[im] = new Trk::Material(*et.m_table[im]);
      }
    }
    return (*this);
  }

  /** Adding two tables : add table entries from another table */
  ElementTable& operator+=(const ElementTable& et) {
    for (size_t ie = 0; ie < size_t(UCHAR_MAX); ++ie) {
      if (et.element(ie)) {
        addElement(*et.element(ie), et.elementName(ie));
      }
    }
    return (*this);
  }

  /** Get the material
     - it can be 0 - if the entry in the table is not filled */
  const Material* element(unsigned int Z) const { return m_table[Z]; }
  /** Get the element name */
  const std::string& elementName(unsigned int Z) const { return m_names[Z]; }
  /** Add material to the Table - if the elment is already filled ignore*/
  void addElement(const Material& mat, const std::string& mname = "") {
    unsigned int Zint = (unsigned int)mat.Z;
    if (!m_table[Zint]) {
      m_table[Zint] = new Material(mat);
      m_names[Zint] = mname;
    }
  }

  /** Version that takes ownership of a pointer. */
  void addElement(std::unique_ptr<Material> mat, const std::string& mname) {
    unsigned int Zint = (unsigned int)mat->Z;
    if (!m_table[Zint]) {
      m_table[Zint] = mat.release();
      m_names[Zint] = mname;
    }
  }

  /** quick check */
  bool contains(unsigned int Z) const { return bool(element(Z)); }

  /** Print the table size */
  size_t size() const { return m_table.size(); }

 private:
  std::vector<Material*> m_table;
  std::vector<std::string> m_names;
};
/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream& operator<<(MsgStream& sl, const ElementTable& etab);
std::ostream& operator<<(std::ostream& sl, const ElementTable& etab);
}  // namespace Trk

CLASS_DEF(Trk::ElementTable, 247342244, 1)

#endif
