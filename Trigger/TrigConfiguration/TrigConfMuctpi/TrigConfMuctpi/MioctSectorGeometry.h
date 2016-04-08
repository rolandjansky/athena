/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MioctSectorGeometry
#define TrigConf_MioctSectorGeometry

#include <string>
#include <vector>

/*
   <MIOCT id="0" slot="4">
        <Sector connector="0" name="B30">
            <ROI eta="-0.934032" phi="-0.348145" etacode="0x2" phicode="0x0" etamin="-0.905571" etamax="-0.962493" phimin="-0.389923" phimax="-0.306366" roiid="21"/>
 */

class MictROIGeometry;

class MioctSectorGeometry {
   public:
      MioctSectorGeometry();
      ~MioctSectorGeometry();
     
      // getters
    unsigned int                            connector() const {return m_connector; };
    const std::string&                      name() const {return m_name; };
    const std::vector<MioctROIGeometry>&    ROIs() const {return m_ROIs; };


      // setters
    void addROI( MioctROIGeometry roi) {m_ROIs.push_back(roi);};
    void setName(std::string name) { m_name = name; };
    void setConnector(unsigned int id) { m_connector = id; };
    
    virtual void print(const std::string& indent="", unsigned int detail=1) const;
    virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   private:
    unsigned int  m_connector;
    std::string   m_name;
    std::vector<MioctROIGeometry>   m_ROIs;
  
};


#endif 
  
