/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MioctGeometry
#define TrigConf_MioctGeometry

#include <string>
#include <vector>


class MioctSectorGeometry;
class MioctTopoCellGeometry;

class MioctGeometry {
   public:
      MioctGeometry();
      ~MioctGeometry();
    
      // getters
    unsigned int mioctId() const {return m_mioctid; }; 
    unsigned int slot() const {return m_slot; }; 
    const  std::vector<MioctSectorGeometry>& sectors() {return m_Sectors; };
    const  std::vector<MioctTopoCellGeometry>& topoCells() {return m_TopoCells; };

      // setters
    void setMioctId (unsigned int id ) {m_mioctid = id; };
    void setSlot( int m )         { m_slot = m; };
    void addSector( const MioctSectorGeometry& sector) {m_Sectors.push_back(sector);};
    void addTopoCell( const MioctTopoCellGeometry& topocell) {m_TopoCells.push_back(topocell); };

    virtual void print(const std::string& indent="", unsigned int detail=1) const;
    virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   private:
     unsigned int  m_mioctid;
     unsigned int  m_slot;
     std::vector<MioctSectorGeometry> m_Sectors;
     std::vector<MioctTopoCellGeometry> m_TopoCells;

};

#endif 
  
