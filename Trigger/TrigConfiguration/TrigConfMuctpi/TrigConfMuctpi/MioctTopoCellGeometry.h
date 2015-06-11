/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_MioctTopoCellGeometry
#define TrigConf_MioctTopoCellGeometry

#include <string>

/*
        <Decode>
            <TopoCell etacode="0x0" phicode="0x0" eta="-0.162496" phi="-0.347931" etamin="-0.304085" etamax="-0.020907" phimin="-0.389909" phimax="-0.305953"/>
 */

class MioctTopoCellGeometry {
   public:
      MioctTopoCellGeometry();
      ~MioctTopoCellGeometry();
     
      // getters
    float        eta()     const{ return m_eta; };
    float        phi()     const{ return m_phi; };
    int          ieta()    const{ return m_ieta; };
    int          iphi()    const{ return m_iphi; };
    unsigned int etacode() const{ return m_etacode; };
    unsigned int phicode() const{ return m_phicode; };
    float        etamin()  const{ return m_etamin; };
    float        etamax()  const{ return m_etamax; };
    float        phimin()  const{ return m_phimin; };
    float        phimax()  const{ return m_phimax; };

      // setters
     void setEta(float v)     { m_eta = v; };
     void setPhi(float v)     { m_phi = v; };
     void setIEta(int v)      { m_ieta = v; };
     void setIPhi(int v)      { m_iphi = v; };
     void setEtacode(unsigned int v) { m_etacode = v; };
     void setPhicode(unsigned int v) { m_phicode = v; };
     void setEtamin(float v) { m_etamin = v; };
     void setEtamax(float v) { m_etamax = v; };
     void setPhimin(float v) { m_phimin = v; };
     void setPhimax(float v) { m_phimax = v; };

    virtual void print(const std::string& indent="", unsigned int detail=1) const;
    virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   private:
     float        m_eta;
     float        m_phi;
     int          m_ieta;
     int          m_iphi;
     unsigned int m_etacode;
     unsigned int m_phicode;
     float        m_etamin;
     float        m_etamax;
     float        m_phimin;
     float        m_phimax;

};


#endif 
  
