///////////////////////////////////////////////////////////////////
// TrkGeoTemplates.h, (c) ATLAS Detector software
// Define a few templates to be filled from GMX files
// Used to build Trk detector layouts
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_GMXGEOTEMPLATES_H
#define INDETTRACKINGGEOMETRY_GMXGEOTEMPLATES_H

// STD
#include <iostream>
#include <vector>

namespace InDet {
  
  class SensorClass {
  public:
    SensorClass() {name = type = ""; isBrl=true; eta_module = rows = -1.;}
    SensorClass(std::string n, std::string t, bool brl, int eta = -1) : name(n), type(t), isBrl(brl), eta_module(eta) {rows = -1.;};
    ~SensorClass() {}
    
    inline void Print() {
      std::cout << "----- GMX SensorClass " << name << " -- " << type << std::endl;
      std::cout <<"----- IsBrl = " << isBrl << " Rows = " << rows << " eta_module = " << eta_module << std::endl;
      for(unsigned int i=0;i<strips.size();i++) 
	std::cout << "----- Row " << i << " with nstrips = " << strips[i] << std::endl;       
       
    }
    
    std::string name;
    std::string type;
    bool isBrl;
    int eta_module;
    double rows;
    std::vector<double> strips;
  };

  class LayerDiscClass {
  public:
    LayerDiscClass() {isBrl=true; layer_disc = phi_modules = eta_modules = -1.; }
    LayerDiscClass(bool brl, int layerdisc, int phi, double eta) : isBrl(brl), layer_disc(layerdisc), phi_modules(phi), eta_modules(eta) {};
    ~LayerDiscClass() {}

    inline void Print() {
      std::cout << "----- GMX LayerDiscClass " << std::endl;
      std::cout << "----- IsBrl = " << isBrl << " layer_disc = " << layer_disc << " eta_modules = " << eta_modules << " phi_modules = " << phi_modules << std::endl;
      std::cout << "----- Sensor "; sensor.Print();	
    }

    bool isBrl;
    int layer_disc;
    int phi_modules;
    double eta_modules;
    SensorClass sensor;
  };

}

#endif // INDETTRACKINGGEOMETRY_GMXGEOTEMPLATES_H
