#include "StepHistogram.h"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"

namespace G4DebuggingHelpers {

  const G4String ClassifyParticle( const G4ParticleDefinition* def ) {
    if (def == G4Electron::Electron())
      return "e-";
    else if (def == G4Positron::Positron())
      return "e+";
    else if (def == G4Gamma::Gamma())
      return "gamma";
    else if (def == G4Neutron::Neutron())
      return "neutron";
    else if (def == G4Proton::Proton())
      return "proton";
    return "other";
  }

  const G4String ClassifyMaterial( const G4String &nom ) {
    if (nom == "FCal1Absorber"
    ||  nom == "LiquidArgon"
    ||  nom == "Copper"
    ||  nom == "Lead"
    ||  nom == "Aluminum"
    ||  nom == "FCal23Absorber"
    ||  nom == "Iron"
    ||  nom == "Air"
    ||  nom == "myLead"
    ||  nom == "shieldIron"
    ||  nom == "FCal23Slugs"
    ||  nom == "Glue"
    ||  nom == "KaptonC"
    ||  nom == "Kapton"
    ||  nom == "ShieldSteel"
    ||  nom == "myIron"
    ||  nom == "ShieldBrass"
    ||  nom == "Straw"
    ||  nom == "XeCO2O2"
    ||  nom == "CO2"
    ||  nom == "Valmat"
    ||  nom == "BoratedPolyethelyne"
    ||  nom == "FoilRadiatorB"
    ||  nom == "G10"
    ||  nom == "FoilRadiatorAC"
    ||  nom == "PyrogelXT"
    ||  nom == "Vacuum")
      return nom;
    else if (nom.substr(0,12)=="pix::IBL_Fwd")
      return "IBL_Fwd";
    return "other";
  }

  const G4String ClassifyVolume( const G4String &nom ) {
    if ( nom.length() >= 17 && nom.substr(13, 4) == "EMEC" ) {
      return "EMEC";
    }
    else if ( nom.length() >= 16 && nom.substr(13, 3) == "EMB" ) {
      return "EMB";
    }
    else if ( nom.length() >= 25 && nom.substr(21, 4) == "Cryo" ) {
      return "Cryo";
    }
    else if ( nom.length() >= 26 && nom.substr(13, 13) == "FCAL::Module1" ) {
      return "FC1";
    }
    else if ( nom.length() >= 25 && nom.substr(13, 12) == "FCAL::Module" ) {
      return "FC23";
    }
    else if ( nom.length() >= 17 && nom.substr(13, 4) == "FCAL" ) {
      return "FCOther";
    }
    else if ( nom.length() >= 16 && nom.substr(13, 3) == "HEC" ) {
      return "HEC";
    }
    else if ( nom.length() >= 31 && nom.substr(21, 10) == "Presampler" ) {
      return "Presampler";
    }
    else if ( nom.length() >= 3 && nom.substr(0, 3) == "LAr" ) {
      return "LAr";
    }
    else if ( ( nom.substr(0, 4) == "MUON" )
         ||   ( nom.length() >= 4  && nom.substr(0, 4)  == "Muon" )
         ||   ( nom.length() >= 9  && nom.substr(0, 9)  == "DriftTube" )
         ||   ( nom.length() >= 12 && nom.substr(0, 12) == "SensitiveGas" )
         ||     nom.contains("MDT")
         ||     nom.contains("station") ) {
      return "Muon";
    }
    else if ( ( nom.length() >= 5 && nom.substr(0, 5) == "Pixel" )
         ||     nom == "Outside Barrel Service") {
      return "Pixel";
    }
    else if ( nom.length() >= 3 && nom.substr(0, 3) == "SCT" ) {
      return "SCT";
    }
    else if ( ( nom.length() >= 3 && nom.substr(0, 3) == "TRT" )
         ||     nom == "GasMANeg" ) {
      return "TRT";
    }
    else if ( nom.length() >= 4 && nom.substr(0, 4) == "Tile" ) {
      return "Tile";
    }
    else if (nom.length() >= 7 && nom.substr(0, 7) == "Section" )
      return "Section";
    else if ( ( nom.length() >= 12 && nom.substr(0, 12) == "InDetServMat" )
         ||   ( nom.length() >= 4  && nom.substr(0, 4)  == "IDET" )
         ||   ( nom.length() >= 8  && nom.substr(0, 8)  == "BeamPipe" )
         ||   ( nom.length() >= 3  &&
                ( nom.substr(0, 3) == "BLM" || nom.substr(0, 3) == "BCM" ) ) ) {
      return "Service";
    }
    return "other";
  }

} // end namespace G4DebuggingHelpers
