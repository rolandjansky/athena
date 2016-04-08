/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElementTable.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ELEMENTTABLE_H
#define TRKGEOMETRY_ELEMENTTABLE_H


#include "TrkGeometry/Material.h"
#include <vector>
#include <climits>
#include <memory>
#include "CLIDSvc/CLASS_DEF.h"

class MsgStream;

namespace Trk {
      
  /** @class ElementTable */
  class ElementTable {
    public:  
      /** Empty constructor */
      ElementTable() :
        m_table(static_cast<size_t>(UCHAR_MAX),(Material*)0),
        m_names(static_cast<size_t>(UCHAR_MAX),std::string("None"))
     {}
              
      /** Empty constructor */
      ElementTable(const ElementTable& et) :
        m_table(static_cast<size_t>(UCHAR_MAX),(Material*)0),
        m_names(et.m_names)
        {
            for (size_t im = 0; im < et.m_table.size(); ++im ){
                if (et.m_table[im]) 
                    m_table[im] = new Trk::Material(*et.m_table[im]);
            }
        }
    
      /** Destructor */
      virtual ~ElementTable();
    
      /** Assignment operator*/
      ElementTable& operator=(const ElementTable& et) {
          if (this != &et){
              // clear first, then copy
              auto elIter = m_table.begin();
              for ( ; elIter != m_table.end(); delete(*elIter), ++elIter );
              m_names = et.m_names;
              for (size_t im = 0; im < et.m_table.size(); ++im ){
                  m_table[im] = 0;
                  if (et.m_table[im]) m_table[im] = new Trk::Material(*et.m_table[im]);
              }
          }
          return (*this);
      }
    
      /** Adding two tables : add table entries from another table */
      ElementTable& operator+= (const ElementTable& et); 
    
      /** Get the material 
         - it can be 0 - if the entry in the table is not filled */
      const Material* element(unsigned int Z) const;
      
      /** Get the element name */
      const std::string& elementName(unsigned int Z) const;
      
      /** Add material to the Table - if the elment is already filled ignore*/
      void addElement(const Material& mat, std::string mname="") const;
      
      /** Version that takes ownership of a pointer. */
      void addElement(std::unique_ptr<Material> mat, const std::string& mname);
      
      /** quick check */
      bool contains(unsigned int Z) const { return bool(element(Z)); }
      
      /** Print the table size */
      size_t size() const { return m_table.size(); }
      
    private:
      mutable std::vector<Material*>   m_table;
      mutable std::vector<std::string> m_names;
            
  };

  inline ElementTable& ElementTable::operator+=(const ElementTable& et) {
      for (size_t ie = 0; ie < size_t(UCHAR_MAX); ++ie){
          if (et.element(ie)){
              addElement(*et.element(ie),et.elementName(ie));
          }
      }
      return (*this);
  }

  inline const Material* ElementTable::element(unsigned int Z) const {
      return m_table[Z];
  }

  inline const std::string& ElementTable::elementName(unsigned int Z) const {
      return m_names[Z];
  }

  inline void ElementTable::addElement(const Material& mat, std::string mname) const {
      unsigned int Zint = (unsigned int)mat.Z;
      if (!m_table[Zint]){
          m_table[Zint] = new Material(mat);
          m_names[Zint] = mname;
      }
  }

  inline void ElementTable::addElement(std::unique_ptr<Material> mat,
                                       const std::string& mname)
  {
      unsigned int Zint = (unsigned int)mat->Z;
      if (!m_table[Zint]){
          m_table[Zint] = mat.release();
          m_names[Zint] = mname;
      }
  }

  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/
  MsgStream& operator << ( MsgStream& sl, const ElementTable& etab);
  std::ostream& operator << ( std::ostream& sl, const ElementTable& etab);

}

CLASS_DEF( Trk::ElementTable, 247342244, 1 )

#endif 
