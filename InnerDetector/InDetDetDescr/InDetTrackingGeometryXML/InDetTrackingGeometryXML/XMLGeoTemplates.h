///////////////////////////////////////////////////////////////////
// TrkGeoTemplates.h, (c) ATLAS Detector software
// Define a few templates to be filled from XML files
// Used to build Trk detector layouts
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_XMLGEOTEMPLATES_H
#define INDETTRACKINGGEOMETRY_XMLGEOTEMPLATES_H

// STD
#include <iostream>
#include <vector>

namespace InDet {

  class MaterialTmp {
  public:
    MaterialTmp() {thickness = X0 = L0 =  A = Z =  density = 0.;}
    ~MaterialTmp() {}

    inline void Print() {
      std::cout << "-----Trk Material Template '" << name  << "' Thickness = " << thickness <<" [mm]"<< std::endl;
      for(unsigned int i=0;i<components.size();i++) 
	std::cout << "     Component '" << components[i] << "' fraction = " << fractions[i]; std::cout << std::endl;       
      std::cout <<"     Averaged: X0 = " << X0 << "  L0 = " << L0 << " A = " << A 
		<< " Z = " << Z << " rho = "   << density <<  " [gram/mm^3]" << std::endl; 
    }

    std::string name;
    double thickness;
    std::vector<std::string> components;
    std::vector<double> fractions;
    double X0, L0, A, Z, density;

  };

  class ComponentTmp {
  public:
    ComponentTmp() {L0 = X0 = A = Z = density = 0.;}
    ~ComponentTmp() {}

    inline void Print() {
      std::cout << "-----Trk Component Template '" << name << "'" << std::endl;
      std::cout << "     L0 = " << L0 << " X0 = " << X0 << " A = " << A << " Z = " 
		<< Z << " rho = " << density << " [gram/mm^3]"<< std::endl;
    }

    std::string name;
    double L0, X0, A, Z, density;
  };

  class ChipTmp {
  public:
    ChipTmp() { length = width = edgew = edgen = edgel = pitchEta = pitchPhi = 0.; rows = columns = 0;}
    ~ChipTmp() {}

    inline void Print() {
      std::cout << "-----Trk Chip Template '" << name << "'" << std::endl;
      std::cout << "     length = " << length << " Width = " << width << " [mm]" << std::endl;
      std::cout << "     pixel pitchEta = " << pitchEta << " pitchPhi = " << pitchPhi << " [mm] "<< std::endl;
      std::cout << "     number of rows = " << rows << " number of columns = " << columns << std::endl;
      std::cout << "     Edges wide = " << edgew << " narrow = " << edgen << " inlength = " << edgel << " [mm]" << std::endl;
    }
    
    std::string name;
    double length;  // Length in mm
    double width;   // Width in mm
    double edgew;   // Edges in mm
    double edgen;
    double edgel;
    double pitchEta; 
    double pitchPhi; 
    int rows;
    int columns;
  };

  class ModuleTmp {
  public:
    ModuleTmp() { lengthChips= widthMaxChips = widthMinChips = 0; thickness = length = widthmin = widthmax = pitchEta = pitchPhi = 0; }
    ~ModuleTmp() {}

    inline void Print() {
      std::cout << "-----Trk Module Template '" << name << "'" << std::endl;
      std::cout << "     Chip Type = '" << chip_type << "' Length in chips = " << lengthChips 
		<< " Width min/max in chips = " << widthMaxChips  << "/"<< widthMinChips  << std::endl;
      std::cout << "     Length = " << length << " Width min/max = " << widthmin << "/" << widthmax << " [mm]" << std::endl;
      std::cout << "     Pixel pitchEta = " << pitchEta << " pitchPhi = " << pitchPhi << " [mm] "<< std::endl;
    }

    std::string name;
    std::string chip_type;
    double lengthChips;      // Length in number of chips (can be non integer)
    double widthMaxChips;    // Width Max in number of chips (can be non integer)
    double widthMinChips;    // Width Min in number of chips (can be non integer)
    double length;           // Length in mm
    double widthmin;         // Width min in mm
    double widthmax;         // Width max in mm
    double thickness;        // Thickness in mm (sensor + chip + hybrid thicknesses)
    double thickness_sensor; // Thickness in mm (sensor only)
    double pitchEta;         // pixel pitch in mm
    double pitchPhi;         // pixel pitch in mm
  };

  class StaveTmp {
  public:
    StaveTmp() {  layer = b_modn = 0; support_material = "DefaultPixelStaveMaterial"; 
      active_halflength = support_halflength = b_gap = b_tilt = b_angle = 
      b_stereoI = b_stereoO = b_stereoSep = b_rshift = b_zoffset = trans_tilt = 
      trans_angle = trans_gap = alp_tilt = alp_angle = alp_rshift =
      alp_zoffset = alp_roffset = alp_radialTilt = 0; 
      rMax = -99999; rMin = 99999; double_sided = b_sameAngle = false;}
    ~StaveTmp() {}

    inline void Print() {
      std::cout << "-----Trk Barrel Layer " << layer << " with stave template '" << name << "' of type '" << type << "'" << std::endl;
      std::cout << "     Support halfLength = " << support_halflength << " [mm] material = '" << support_material <<"'" << std::endl;
      std::cout << "     Barrel modules parameters:"                  << std::endl;
      std::cout << "        #modules = "  << b_modn << " type = '" << b_type << "'  gap = " << b_gap 
		<< " [mm] tilt = "<< b_tilt << " [rad] angle =  " << b_angle << " [rad] rshift = " << b_rshift <<
		" [mm] z offset = " << b_zoffset << " [mm]"<< std::endl;
      std::cout << "        stereo angle inner/outer = " << b_stereoI << "/" << b_stereoO << " [rad]" 
		<< " double-sided = " << double_sided <<  " apply same angle = " << b_sameAngle << std::endl;
      std::cout << "        radius min/max = " << rMin << "/" << rMax << " [mm] "<< std::endl;

      if(trans_pos.size()>0){
	std::cout << "     Transition modules parameters:"               << std::endl;
	std::cout << "        #modules = "  << trans_pos.size() << " type = '" << trans_type << "'  gap = " 
		  << trans_gap << " [mm] tilt = "<< trans_tilt << " [rad] angle = " << trans_angle << " [rad]" << std::endl;
     }
      if(alp_pos.size()>0){
	std::cout << "     Alpine modules parameters:"        << std::endl;
	std::cout << "        #modules = "  << alp_pos.size() << " type = '" << alp_type 
		  << " tilt = " << alp_tilt << " [rad] angle = " << alp_angle << " [rad] rshift = " << alp_rshift 
		  << " [mm] z offset = " << alp_zoffset << " [mm] r offset = " << alp_roffset << " [mm] radial tilt = "
		  << alp_radialTilt << " [rad] "<< std::endl;
      }
    }

    // stave parameters
    std::string name;
    std::string type;
    std::string support_material;
    int layer; 
    double support_halflength;
    // Barrel modules
    int b_modn;
    double b_gap;
    double b_tilt;
    double b_angle;
    double b_stereoI;
    double b_stereoO;
    double b_stereoSep;
    double b_rshift;
    double b_zoffset;
    bool double_sided;
    bool b_sameAngle;
    std::string b_type;
    // Transition modules
    double trans_tilt;
    double trans_angle;
    double trans_gap;
    std::vector<double> trans_pos;
    std::string trans_type;
    // Alpine modules
    std::vector<double> alp_pos;
    std::string alp_type;
    double alp_tilt;
    double alp_angle;
    double alp_rshift;
    double alp_zoffset;
    double alp_roffset;
    double alp_radialTilt;
    // Cylindrical envelop
    double active_halflength;
    double rMax;
    double rMin;
  };

  class BarrelLayerTmp {
  public:
    BarrelLayerTmp() {  index = stave_n = 0;  radius = phioffset = stave_tilt = stave_zoffset = 0; }
    ~BarrelLayerTmp() {}
    
    inline void Print() {
      std::cout << "-----Trk Barrel Layer '"  << name  << "'" << std::endl;
      std::cout << "     Layer = "  << index << "  Radius = "  << radius << " [mm]  Number of staves = " << stave_n << "  Phi offset mod0 = " << phioffset << std::endl;
      std::cout << "     Staves type = '"  << stave_type << "' tilt angle = "  << stave_tilt 
		<< " [rad] zOffset = "  << stave_zoffset << " [mm]" << std::endl;
    }
    
    std::string name;
    int index;
    double radius;
    double phioffset;
    int stave_n;
    double stave_tilt;
    double stave_zoffset;
    std::string stave_type;
  };

  class EndcapLayerTmp {
  public:
    EndcapLayerTmp() {  ilayer = -1; layer_pos = -1; stereoI = stereoO = stereoSep = 0.; double_sided = isDisc = useDiscSurface = false;}
    ~EndcapLayerTmp() {}
    
    inline void Print() {
      std::cout << "-----Trk Endcap Layer '"   << name  << "' ilayer = " << ilayer << " layer_pos = " << layer_pos << " isDisc = " << isDisc << " useDiscSurface = " << useDiscSurface << std::endl;
      std::cout << "     Support material = '"  << support_material << "'" << std::endl;
      std::cout << "     Double-Sided = "  << double_sided << "  Stereo Angle inner/outer = " 
		<< stereoI << "/" <<  stereoO << " [rad] Stereo Separation = " << stereoSep << " [mm]"<<std::endl;
      for(unsigned int i=0;i<ringpos.size();i++){
	std::cout << "     Ring " << i << " nsectors = "  << nsectors.at(i) << " z = " << ringpos.at(i) 
		  << " rMin/rMax = "<< innerRadius.at(i) <<"/"<< outerRadius.at(i) << " Zoffset = "  
		  << zoffset.at(i) << " Phi offset mod0 = " << phioffset.at(i) 
		  << " thck = " <<  thickness.at(i)<< " [mm] mtype = '" << modtype.at(i) << "' splitMode = " << splitMode.at(i) 
		  << " splitOffset = " << splitOffset.at(i) << std::endl;
      }
    }
    
    // layer parameters
    int ilayer, layer_pos; 
    bool isDisc;
    bool useDiscSurface;
    std::string name;
    std::string support_material = "DefaultPixelRingMaterial";
    double stereoI,stereoO, stereoSep;
    bool double_sided;

    // rings parameters
    std::vector<std::string> modtype;
    std::vector<double>      ringpos;
    std::vector<double>      zoffset;
    std::vector<double>      phioffset;
    std::vector<double>      outerRadius;
    std::vector<double>      innerRadius;
    std::vector<double>      thickness;
    std::vector<int>         nsectors;
    std::vector<std::string> splitMode;
    std::vector<double>      splitOffset;
   
  };

}

#endif // INDETTRACKINGGEOMETRY_XMLGEOTEMPLATES_H
