/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MioctROIGeometry
#define TrigConf_MioctROIGeometry

#include <string>
#include <ostream>
#include <iomanip>

/*
   <MIOCT id="0" slot="4">
        <Sector connector="0" name="B30">
            <ROI eta="-0.934032" phi="-0.348145" etacode="0x2" phicode="0x0" etamin="-0.905571" etamax="-0.962493" phimin="-0.389923" phimax="-0.306366" roiid="21"/>
 */


std::ostream&
mioctIndent(std::ostream& o, int lvl, int size) ;

class MioctROIGeometry {
   public:
      MioctROIGeometry();
      ~MioctROIGeometry();
     
      // getters
    float        eta()     const { return m_eta; };
    float        phi()     const{ return m_phi; };
    unsigned int etacode() const{ return m_etacode; };
    unsigned int phicode() const{ return m_phicode; };
    float        etamin()  const{ return m_etamin; };
    float        etamax()  const{ return m_etamax; };
    float        phimin()  const{ return m_phimin; };
    float        phimax()  const{ return m_phimax; };
    unsigned int roiid()   const{ return m_roiid; };

      // setters
     void setEta(float v)     { m_eta = v; };
     void setPhi(float v)     { m_phi = v; };
     void setEtacode(unsigned int v) { m_etacode = v; };
     void setPhicode(unsigned int v) { m_phicode = v; };
     void setEtamin(float v) { m_etamin = v; };
     void setEtamax(float v) { m_etamax = v; };
     void setPhimin(float v) { m_phimin = v; };
     void setPhimax(float v) { m_phimax = v; };
     void setRoiid(unsigned int v)   { m_roiid = v; };

    virtual void print(const std::string& indent="", unsigned int detail=1) const;
    virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   private:
     float        m_eta;
     float        m_phi;
     unsigned int m_etacode;
     unsigned int m_phicode;
     float        m_etamin;
     float        m_etamax;
     float        m_phimin;
     float        m_phimax;
     unsigned int m_roiid;

};

#endif 
  
