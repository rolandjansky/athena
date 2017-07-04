/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GeometrySystems/VisAttributes.h"

#include "VP1Base/VP1QtInventorUtils.h"
#include <Inventor/nodes/SoMaterial.h>
#include <iostream>
#include <QtCore/QBuffer>

/////////////////////////////////////// Base class ///////////////////////////////////////

#include <map>
#include <iostream>

class VisAttributes::Imp {
public:
  // The material map is here:
  std::map< std::string, SoMaterial *> _map;

  QMap<QString,QByteArray> currentState() const;
  QMap<QString,QByteArray> initialState;
};

//____________________________________________________________________
void VisAttributes::init()
{
  d->initialState = d->currentState();
}

//____________________________________________________________________
QByteArray VisAttributes::getState(bool onlyChangedMaterials)
{
  //Figure out states to store
  QMap<QString,QByteArray> storedstates, statesnow = d->currentState();
  if (onlyChangedMaterials) {
    QMap<QString,QByteArray>::const_iterator it, itE(statesnow.constEnd());
    QMap<QString,QByteArray>::const_iterator itOrig, itOrigE(d->initialState.constEnd());
    for (it = statesnow.constBegin(); it!=itE; ++it) {
      itOrig = d->initialState.constFind(it.key());
      if (itOrig==itOrigE||it.value()!=itOrig.value())
	storedstates.insert(it.key(),it.value());
    }
  } else {
    storedstates = statesnow;
  }

  //Put map in bytearray and return:
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  QDataStream out(&buffer);
  out << qint32(0);//version
  out << storedstates;
  buffer.close();
  return byteArray;

}

//____________________________________________________________________
void VisAttributes::applyState(QByteArray ba)
{
  //Get map out:
  QMap<QString,QByteArray> storedstates;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);
  qint32 version;
  state >> version;
  if (version!=0)
    return;//ignore silently
  state >> storedstates;
  buffer.close();

  std::map< std::string, SoMaterial *>::iterator itMat,itMatE(d->_map.end());

  //Apply states from map:
  QMap<QString,QByteArray>::const_iterator it, itE(storedstates.constEnd());
  for (it = storedstates.constBegin(); it!=itE; ++it) {
    itMat = d->_map.find(it.key().toStdString());
    if (itMat!=itMatE) {
      QByteArray b(it.value());
      VP1QtInventorUtils::deserialiseSoMaterial(b,itMat->second);
    }
  }

}

//____________________________________________________________________
QMap<QString,QByteArray> VisAttributes::Imp::currentState() const
{
  QMap<QString,QByteArray> outmap;
  std::map< std::string, SoMaterial *>::const_iterator it(_map.begin()), itE(_map.end());
  for (;it!=itE;++it)
    outmap.insert(QString(it->first.c_str()),VP1QtInventorUtils::serialiseSoMaterial(it->second));
  return outmap;
}

//____________________________________________________________________
VisAttributes::VisAttributes() : d(new Imp) {
}

//____________________________________________________________________
VisAttributes::~VisAttributes() {

  std::map<std::string, SoMaterial *>::iterator m,e=d->_map.end();
  for (m=d->_map.begin();m!=e;m++)
    (*m).second->unref();

  delete d;
}

SoMaterial *VisAttributes::get (const std::string & name) const {
  std::map <std::string, SoMaterial *>::const_iterator m = d->_map.find(name);
  if (m!=d->_map.end()) {
    return (*m).second;
  } else {
    return NULL;
  }
}

void VisAttributes::add(const std::string & name, SoMaterial *material) {
  if (d->_map.find(name)!=d->_map.end()) {
    std::cout<<"VisAttributes::add ERROR: Material " <<name<<" already added!"<<std::endl;
    return;
  }
  material->ref();
  d->_map[name]=material;
  if (material->transparency.getNum()!=1)
    std::cout<<"VisAttributes::add Warning: Found #transparency values different from 1 in material "<<name<<std::endl;
  if (material->transparency[0]!=0.0)
    std::cout<<"VisAttributes::add Warning: Found transparency value different from 0 in material "<<name<<std::endl;
}

void VisAttributes::overrideTransparencies(float transpfact)
{
  std::map< std::string, SoMaterial *>::iterator it, itE = d->_map.end();
  for (it=d->_map.begin();it!=itE;++it)
    it->second->transparency.set1Value( 0, transpfact );
}

//////////////////////////////// Attributes for detectors ////////////////////////////////

DetVisAttributes::DetVisAttributes() {

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0, .157811, .187004);
    material->diffuseColor.setValue(0, .631244, .748016);
    material->specularColor.setValue(.915152, .915152, .915152);
    material->shininess.setValue(0.642424);
    add("Pixel",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0, .157811, .187004);
    material->diffuseColor.setValue(.40, .631244, .748016);
    material->specularColor.setValue(.915152, .915152, .915152);
    material->shininess.setValue(0.642424);
    add("Tile",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0, .157811, .187004);
    material->diffuseColor.setValue(1, .8, .7);
    material->specularColor.setValue(.915152, .915152, .915152);
    material->shininess.setValue(0.642424);
    add("BeamPipe",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0, .6, .6);
    material->diffuseColor.setValue(1, .8, .7);
    material->specularColor.setValue(.91515, .915152, .915152);
    material->shininess.setValue(0.642424);
    add("CavernInfra",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0.2, 0.2, 0.2);
    material->diffuseColor.setValue(0, 0.6667, 1.0);
    material->specularColor.setValue(0,0,0);
    //    material->shininess.setValue(0.642424);
//     material->ambientColor.setValue(0, .157811, .187004);
//     material->diffuseColor.setValue(1, 0, 0);
//     material->specularColor.setValue(.915152, .915152, .915152);
//     material->shininess.setValue(0.642424);

    add("Muon",material);

    {
       SoMaterial *material = new SoMaterial;
       material->ambientColor.setValue(0, .157811, .187004);
       material->diffuseColor.setValue(.98, .8, .21);
       material->specularColor.setValue(.915152, .915152, .915152);
       material->shininess.setValue(0.2);
       add("CSC",material);
    }

    {
      SoMaterial *material = new SoMaterial;
      material->ambientColor.setValue(0, .157811, .187004);
      material->diffuseColor.setValue(0, .9, .5);
      material->specularColor.setValue(.915152, .915152, .915152);
      material->shininess.setValue(0.2);
      add("EndcapMdt",material);
    }

    {
      SoMaterial *material = new SoMaterial;
      material->ambientColor.setValue(0,0,0);
      material->diffuseColor.setValue(0.41,0,0.26);
      material->specularColor.setValue(0,0,0);
      material->shininess.setValue(0.2);
      add("TGC",material);
    }

    {
      SoMaterial *material = new SoMaterial;

    material->ambientColor.setValue(0.2, 0.2, 0.2);
    material->diffuseColor.setValue(0, 0.6667, 1.0);
    material->specularColor.setValue(0,0,0);
//       material->ambientColor.setValue(0, .157811, .187004);
//       material->diffuseColor.setValue(1, .2, .7);
//       material->specularColor.setValue(.915152, .915152, .915152);
    material->shininess.setValue(0.2);


      add("BarrelInner",material);
      add("BarrelMiddle",material);
      add("BarrelOuter",material);
    }

    {
      SoMaterial *material = new SoMaterial;
      material->ambientColor.setValue(0, .157811, .187004);
      material->diffuseColor.setValue(1, .5, .5);
      material->specularColor.setValue(.915152, .915152, .915152);
      material->shininess.setValue(0.2);
      add("BarrelToroid",material);
      add("EndcapToroid",material);
    }

    {
      SoMaterial *material = new SoMaterial;
      material->ambientColor.setValue(0, .157811, .187004);
      material->diffuseColor.setValue(.5, .5, 1.0);
      material->specularColor.setValue(.915152, .915152, .915152);
      material->shininess.setValue(0.2);
      add("Feet",material);
    }
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(.37, .69, 1.00);
    material->diffuseColor.setValue(.21, .64, 1.00);
    material->specularColor.setValue(1, 1, 1);
    material->shininess.setValue(1.0);
    add("SCT",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    add("LAr",material);
    add("LArBarrel",material);
    add("LArEndcapPos",material);
    add("LArEndcapNeg",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    add("TRT",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    add("InDetServMat",material);
    material->diffuseColor.setValue(0.4,0.31,0);
    material->shininess.setValue(1.0);
  }

  {
    SoMaterial *material = new SoMaterial();
    add("LucidSideA",material);
    add("LucidSideC",material);
    material->diffuseColor.setValue(0.6,0.11,0.3);
    material->shininess.setValue(1.0);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0, .157811, .187004);
    material->diffuseColor.setValue(0., 0., 0.56862745);
    material->specularColor.setValue(.915152, .915152, .915152);
    material->shininess.setValue(0.2);
    add("Zdc",material);
  }


  //   {
  //     SoMaterial *material = new SoMaterial();
  //     add("LcdXXX",material);
  //     add("LcdYYY",material);
  //     material->diffuseColor.setValue(0.6,0.11,0.3);
  //     material->shininess.setValue(1.0);
  //   }

  init();
}

//////////////////////////////// Attributes for materials ////////////////////////////////

MatVisAttributes::MatVisAttributes() {
  {
    //NB: Must be here!!
    SoMaterial *m = new SoMaterial;
    add("DEFAULT",m);
  }



  /*
   * ETHER MATERIAL
   * This transparent material was used to hide the "fakeVolume" added by AGDD,
   * but there are other "standard" volumes, which use "Ether" material, for example in CSC chambers.
   * So we do not use this workaround anymore...
  {
	//
    // Ether: ---> it's a special "totally transparent" material, made to hide the "fakeVol" volume used in GeoModel to cope with the G4GeoAssembly objects in G4
	// more details:
	// - https://its.cern.ch/jira/browse/ATLASVPONE-166
	// - https://svnweb.cern.ch/trac/atlasoff/browser/DetectorDescription/AGDD/AGDDControl/trunk/src/AGDD2GeoModelBuilder.cxx?rev=648789#L502
	//
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue(0.2, 0.2, 0.2);
    m->diffuseColor.setValue(0.58, 0.47, 0.81);
    m->specularColor.setValue(0.56, 0.55, 0.56);
    m->transparency.setValue(1.0); // --> TOTALLY TRANSPARENT!!!  (set it to < 1.0 (e.g. 0.5) if you want to see the "fakeVol" cylinder in GeoModel)
    add("Ether",m);
  }
  */
 

  {
    // C02:
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue(0.2, 0.2, 0.2);
    m->diffuseColor.setValue(0.58, 0.47, 0.81);
    m->specularColor.setValue(0.56, 0.55, 0.56);
    add("CO2",m);
    add("ArCO2",m);
  }

  {
    // Silicon
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue(0.9, 0.9, 0.9);
    m->diffuseColor.setValue(0.9, 0.9, 0.9);
    m->specularColor.setValue(0.9, 0.9, 0.9);
    m->shininess.setValue(0.53);
    add("Silicon",m);
  }

  {
    // Kapton
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue(0.2, 0.127931, 0.177402);
    m->diffuseColor.setValue( 0.57665, 0.155139, 0.180815);
    m->specularColor.setValue(0.441667, 0.441667, 0.441667);
    m->shininess.setValue(0.67);
    add("Kapton",m);
    add("KaptonC",m);  //used by LAr.
    add("FCalCableHarness",m);  //used by LAr.
  }

  {
    // Aluminium
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.70, 0.72, 0.72);
    m->diffuseColor.setValue (0.56, 0.57, 0.57);
    m->specularColor.setValue(0.71, 0.48, 0.46);
    m->shininess.setValue(0.23);
    add("Aluminium",m);
  }

  {
    // Iron
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.1, 0.1, 0.1);
    m->specularColor.setValue(0.92, 0.92, 0.89);
    m->shininess.setValue(0.60);
    add("Iron",m);
  }

  {
    // Tungsten
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.14, 0.14, 0.14);
    m->specularColor.setValue(0.84, 0.94, 1.00);
    m->shininess.setValue(0.20);
    add("Tungsten",m);
    add("Wolfram",m);
    add("FCal23Absorber",m);
  }
  {
    // Lead
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.05, .05, 0.7);
    m->diffuseColor.setValue (0.39, 0.36, 0.47);
    m->specularColor.setValue(0.33, 0.33, 0.35);
    m->shininess.setValue(0.30);
    add("Lead",m);
    add("Thinabs",m);
    add("Thickabs",m);
  }

  {
    // G10
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.308764, 0.314286, 0.142384);
    m->specularColor.setValue(0.268276, 0.315312, 0.308455);
    m->shininess.setValue(0.617021);
    add("G10",m);

  }

  {
    // Muon Scintillator
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.381944, 0.748016, 0);
    m->specularColor.setValue(0.963636, 0.963636, 0.963636);
    m->shininess.setValue(0.981818);
    add("Scintillator",m);
  }

  {
    // Epoxy
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0, 0.748016, 0.176015);
    m->specularColor.setValue(0.981818, 0.981818, 0.981818);
    m->shininess.setValue(0.721212);
    add("Epoxy",m);
  }

  {
    // Stainless steel
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.424238, 0.424238, 0.424238);
    m->specularColor.setValue(0.168, 0.168, 0.168);
    m->shininess.setValue(0.153696);
    add("Stainless",m);
  }
  {
    // Liquid Argon
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.05, .05, .06);
    m->diffuseColor.setValue (0.39, .36, .48);
    m->specularColor.setValue(0.33, .33, .35);
    m->emissiveColor.setValue(0.45, .60, .60);
    m->shininess.setValue(0.3);
    add("LiquidArgon",m);
  }
  {
    // Copper
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.0, 0.0,  0.0);
    m->diffuseColor.setValue (1.0, .43, .36);
    m->specularColor.setValue(1.0, 1.0, 1.0);
    m->shininess.setValue(0.4);
    add("Copper",m);
    add("FCal1Absorber",m);
  }

  {
    // Cables
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (1.0, 0, 0);
    add("Cables",m);
  }

  {
    // MBoards
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0, 1, 0);
    add("MBoards",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.2, 0.2, 0.2);
    m->ambientColor.setValue (0.07, 0.07, 0.07);
    m->specularColor.setValue (0.8, 0.8, 0.8);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("Carbon",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (1., 0., 0.);
    m->ambientColor.setValue (1., 0., 0.);
    m->specularColor.setValue (1., 0., 0.);
    m->emissiveColor.setValue (1., 0., 0.);
    add("pix::Default_InclBrl4_Stave_Support_L0_Fixed_Weight",m);
    add("pix::Default_InclBrl4_Stave_Support_L1_Fixed_Weight",m);
    add("pix::Default_InclBrl4_Stave_Support_L2_Fixed_Weight",m);
    add("pix::Default_InclBrl4_Stave_Support_L3_Fixed_Weight",m);
    add("pix::Default_InclBrl4_Stave_Support_L4_Fixed_Weight",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.5, 0., 0.);
    m->ambientColor.setValue (0.5, 0., 0.);
    m->specularColor.setValue (0.5, 0., 0.);
    m->emissiveColor.setValue (0.5, 0., 0.);
    add("pix::Default_Slim_InclBrl4_L2L3W_Fixed_Weight",m);
    add("pix::Default_Slim_InclBrl4_L3L4W_3CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_L3L4W_4CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3W_3CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3W_4CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L3L4W_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3W_3CL_Fixed_WeightINNER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3W_4CL_Fixed_WeightINNER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L3L4W_Fixed_WeightINNER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3W_3CL_Fixed_WeightOUTER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3W_4CL_Fixed_WeightOUTER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L3L4W_Fixed_WeightOUTER", m);
  }
  
  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.3, 0., 0.);
    m->ambientColor.setValue (0.3, 0., 0.);
    //m->specularColor.setValue (0.3, 0., 0.);
    //m->emissiveColor.setValue (0.3, 0., 0.);
    add("pix::Default_Slim_InclBrl4_L2L3C_Fixed_Weight",m);
    add("pix::Default_Slim_InclBrl4_L3L4C_3CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_L3L4C_4CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3C_3CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3C_4CL_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L3L4C_Fixed_Weight", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3C_3CL_Fixed_WeightINNER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3C_4CL_Fixed_WeightINNER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L3L4C_Fixed_WeightINNER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3C_3CL_Fixed_WeightOUTER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L2L3C_4CL_Fixed_WeightOUTER", m);
    add("pix::Default_Slim_InclBrl4_InclDual_L3L4C_Fixed_WeightOUTER", m);
  }
  
  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (1., 0., 0.);
    m->ambientColor.setValue (1., 0., 0.);
    m->specularColor.setValue (1., 0., 0.);
    m->emissiveColor.setValue (1., 0., 0.);
    add("pix::Pixel_Ring_Support_UK_L0_Fixed_Weight",m);
    add("pix::Pixel_Ring_Support_UK_L1_Fixed_Weight",m);
    add("pix::Pixel_Ring_Support_UK_L2_Fixed_Weight",m);
    add("pix::Pixel_Ring_Support_UK_L3_Fixed_Weight",m);
    add("pix::Pixel_Ring_Support_UK_L4_Fixed_Weight",m);
  }
  
  {
    //Titanium
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.62, 0.62, 0.62);
    m->specularColor.setValue (0.294, 0.294, 0.294);
    m->shininess.setValue(.20);
    add("Titanium",m);
  }

  {
    //ECServices
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.7, 0.7, 0.7);
    m->specularColor.setValue (0.5, 0.5, 0.5);
    add("ECServices",m);
  }

  {
    //ECCables
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.1, 0.1, 0.1);
    add("ECCables",m);
  }

  {
    //Services
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.765, 0.718, 0.541);
    m->specularColor.setValue (0.5, 0.5, 0.5);
    add("Services",m);
  }

  {
    //MatOmega
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.22, 0.22, 0.22);
    add("MatOmega",m);
    add("pix::Omega_BL",m);
    add("pix::Omega_L1",m);
    add("pix::Omega_L2",m);
  }

  {
    //MatConn
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0, 0, 0);
    m->specularColor.setValue (0.8, 0.8, 0.8);
    add("MatConn",m);
    add("pix::Connector_BL",m);
    add("pix::Connector_L1",m);
    add("pix::Connector_L2",m);
  }

  {
    //MatPigtail
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.95, 0.58, 0.13);
    m->specularColor.setValue (0.8, 0.75, 0.7);
    m->shininess.setValue(.30);
    add("MatPigtail",m);
  }

  {
    // MatAlTube
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.70, 0.72, 0.72);
    m->diffuseColor.setValue (0.56, 0.57, 0.57);
    m->specularColor.setValue(0.71, 0.48, 0.46);
    m->shininess.setValue(0.23);
    add("MatAlTube",m);
    add("pix::AlTube_BL",m);
    add("pix::AlTube_L1",m);
    add("pix::AlTube_L2",m);
    add("MatAlTubeFix",m);
  }

  {
    //MatT0 (Cable)
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.06, 0.06, 0.06);
    m->specularColor.setValue (0.8, 0.8, 0.8);
    add("MatT0",m);
    add("Cable",m);
  }

  {
    //MatCap1
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0, 0, 0.2);
    add("MatCap1",m);
  }

  {
    //MatCap2
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0, 0, 0.27);
    add("MatCap2",m);
  }

  {
    //MatTMT
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.42, 0.42, 0.42);
    add("MatTMT",m);
    add("pix::AsTMT_BL",m);
    add("pix::AsTMT_L1",m);
    add("pix::AsTMT_L2",m);
  }

  {
    //MatGlue
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (1, 1, 0.78);
    add("MatGlue",m);
    add("pix::GlueOmegaStave_BL0",m);
    add("pix::GlueOmegaStave_L10",m);
    add("pix::GlueOmegaStave_L20",m);
  }

  {
    //Glue
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (1, 1, 0.75);
    add("pix::GlueOmegaStave_BL1",m);
    add("pix::GlueOmegaStave_L11",m);
    add("pix::GlueOmegaStave_L21",m);
  }

  {
    //MatPP11
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.08, 0.03, 0.03);
    add("MatPP11",m);
  }

  {
    //MatPP12
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.325, 0.292, 0.257);
    add("MatPP12",m);
  }

  {
    //MatPP13
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.42, 0.38, 0.30);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("MatPP13",m);
  }

  {
    //MatPP14
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.3, 0.3, 0.3);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("MatPP14",m);
  }

  {
    //MatPP15
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.2, 0.2, 0.23);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("MatPP15",m);
  }

  {
    //MatPP16
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.4, 0.4, 0.4);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("MatPP16",m);
  }

  {
    //MatPP17
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.17, 0.19, 0.16);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("MatPP17",m);
  }

  {
    //Disk
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.5, 0.5, 0.5);
    m->specularColor.setValue (0.5, 0.5, 0.5);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    add("Disk",m);
  }

  {
    //Hybrid
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.15, 0.33, 0);
    m->specularColor.setValue (0.39, 0.39, 0.39);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    m->shininess.setValue(.60);
    add("Hybrid",m);
  }

  {
    //Chip
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.8, 0.51, 0.105);
    m->specularColor.setValue (0.39, 0.39, 0.39);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    m->shininess.setValue(.60);
    add("Chip",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.8, 0.51, 0.105);
    m->specularColor.setValue (0.39, 0.39, 0.39);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    m->shininess.setValue(.60);
    add("pix::Chip",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.15, 0.33, 0);
    m->specularColor.setValue (0.39, 0.39, 0.39);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    m->shininess.setValue(.60);
    add("pix::Hybrid",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.42, 0.42, 0.42);
    add("pix:AsTMT_BL",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.95, 0.52, 0.12);
    m->specularColor.setValue (0.8, 0.75, 0.7);
    m->shininess.setValue(.30);
    add("pix::PigtailCyl",m);
    add("pix::PigtailFlat",m);
  }

  {
    // Rings and Service Support
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.50, 0.49, 0.12);
    m->diffuseColor.setValue (0.665, 0.618, 0.441);
    m->specularColor.setValue(0.51, 0.48, 0.16);
    m->shininess.setValue(0.37);
    //Rings
    add("pix::AsRingCen_BL",m);
    add("pix::AsRingInt_BL",m);
    add("pix::AsRingOut_BL",m);
    add("pix::AsRing_L1",m);
    add("pix::AsRingOut_L1",m);
    add("pix::AsRing_L2",m);
    add("pix::AsRingOut_L2",m);
    //ServiceSupport
    add("pix::ServiceSupport_BL",m);
    add("pix::ServiceSupport_L1",m);
    add("pix::ServiceSupport_L2",m);
  }
  {
    // Halfshell
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.21, 0.23, 0.23);
    m->diffuseColor.setValue (0.1, 0.11, 0.11);
    m->specularColor.setValue(0.31, 0.28, 0.06);
    m->shininess.setValue(0.43);
    add("pix::Halfshell_BL",m);
    add("pix::Halfshell_L1",m);
    add("pix::Halfshell_L2",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.40, 0.42, 0.42);
    m->diffuseColor.setValue (0.36, 0.37, 0.37);
    m->specularColor.setValue(0.51, 0.28, 0.26);
    m->shininess.setValue(0.38);
    add("Prepreg",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.10, 0.12, 0.12);
    m->diffuseColor.setValue (0.09, 0.10, 0.17);
    add("pix::CablesAxial_BL",m);
    add("pix::CoolingAxial_BL",m);
    add("pix::CabCoolAxial_L1",m);
    add("pix::CabCoolAxial_L2",m);
    add("pix::CabCoolRadial_L1",m);
    add("pix::OuterCable_BL",m);
    add("pix::OuterCabCool_L1",m);
    add("pix::OuterCabCool_L2",m);
  }
  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.12, 0.14, 0.14);
    m->diffuseColor.setValue (0.10, 0.11, 0.19);
    add("pix::CablesRadial_BL",m);
    add("pix::CoolingRadial_BL",m);
    add("pix::CabCoolRadial_L2",m);
    add("pix::OuterCooling_BL",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.20, 0.25, 0.25);
    m->diffuseColor.setValue (0.22, 0.22, 0.22);
    add("pix::Ulink_BL_A",m);
    add("pix::AsUlink_BL_C",m);
    add("pix::AsUlink_L1",m);
    add("pix::AsUlink_L2",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.40, 0.42, 0.42);
    m->diffuseColor.setValue (0.36, 0.37, 0.37);
    m->specularColor.setValue(0.51, 0.28, 0.26);
    m->shininess.setValue(0.38);
    add("pix::SSR_BL_A",m);
    add("pix::SSR_BL_C",m);
    add("pix::SSR_L1_A",m);
    add("pix::SSR_L1_C",m);
    add("pix::SSR_L2",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.35, 0.37, 0.37);
    m->diffuseColor.setValue (0.31, 0.32, 0.32);
    m->specularColor.setValue(0.51, 0.28, 0.26);
    m->shininess.setValue(0.38);
    add("pix::InnerSkin_BL",m);
    add("pix::InnerSkin_L1",m);
    add("pix::InnerSkin_L2",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.55, 0.57, 0.57);
    m->diffuseColor.setValue (0.51, 0.52, 0.52);
    m->specularColor.setValue(0.51, 0.28, 0.26);
    m->shininess.setValue(0.33);
    add("pix::Fingers1",m);
    add("pix::Fingers2",m);
    add("pix::Fingers3",m);
    add("pix::Fingers4",m);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue (0.60, 0.62, 0.62);
    m->diffuseColor.setValue (0.51, 0.52, 0.52);
    m->specularColor.setValue(0.61, 0.38, 0.36);
    m->shininess.setValue(0.28);
    add("Peek",m);
  }

  {
    //Glass
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (0.8, 0.9, 1.0);
    m->specularColor.setValue (0.39, 0.39, 0.39);
    m->emissiveColor.setValue (0.028, 0.028, 0.028);
    m->shininess.setValue(.60);
    add("Glass",m);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->diffuseColor.setValue(0.9,0.7,0.5);
    material->ambientColor.setValue(0.57,0.57,0.57);
    material->specularColor.setValue(0.27,0.27,0.27);
    material->shininess.setValue(.80);
    add("AlNitride",material);
    add("Dogleg",material);
    add("BrlBaseBoard",material);
    add("BrlHybrid",material);
    add("BrlBracket",material);
    add("PigTail",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->diffuseColor.setValue(0.40,0.60,0.40);
    material->ambientColor.setValue(0.57,0.57,0.57);
    material->specularColor.setValue(0.27,0.27,0.27);
    material->shininess.setValue(.80);
    add("PowerTape",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->diffuseColor.setValue(0.57,0.82,0.9);
    material->ambientColor.setValue(0.57,0.57,0.57);
    material->specularColor.setValue(0.27,0.27,0.27);
    material->shininess.setValue(.80);
    add("CoolingBlock",material);
    add("sct::CoolBlockSecHi",material);
    add("sct::CoolBlockSecLo",material);
    add("sct::DiscCoolingInn",material);
    add("sct::DiscCoolingOut",material);
  }

  { /* Large structures in sct */
    SoMaterial *material = new SoMaterial;
    //material->diffuseColor.setValue(0.7,0.6,0.5);
    material->diffuseColor.setValue(0.8,0.7,0.6);
    material->ambientColor.setValue(0.57,0.57,0.57);
    material->specularColor.setValue(0.27,0.27,0.27);
    material->shininess.setValue(.80);
    add("sct::DiscCoolingMid",material);
    add("sct::DiscPowerTapeMid",material);
    add("sct::DiscPowerTapeInn",material);
    add("sct::DiscPowerTapeOut",material);
    add("sct::EMI",material);
    add("sct::EMIJoint",material);
    add("sct::Flange0",material);
    add("sct::Flange1",material);
    add("sct::Flange2",material);
    add("sct::Flange3",material);
    add("sct::FwdFrontSupport",material);
    add("sct::FwdITE",material);
    add("sct::FwdLMT",material);
    add("sct::FwdLMTCooling",material);
    add("sct::FwdOTE",material);
    add("sct::FwdRearSupport",material);
    add("sct::FwdRail",material);
    add("sct::FwdSupport",material);
    add("sct::FwdSpineMid",material);
    add("sct::FwdSpineOut",material);
    add("sct::FwdSpineInn",material);
    add("sct::Harness",material);
    add("sct::OptoHarnessO",material);
    add("sct::OptoHarnessOM",material);
    add("sct::OptoHarnessOMI",material);
    add("sct::Spider",material);
    add("sct::SupportCyl0",material);
    add("sct::SupportCyl1",material);
    add("sct::SupportCyl2",material);
    add("sct::SupportCyl3",material);
    add("sct::TSCylinder",material);
  }

  { /* Details in sct (A) */
    SoMaterial *material = new SoMaterial;
    material->diffuseColor.setValue(0.6,0.525,0.45);
    material->ambientColor.setValue(0.57,0.57,0.57);
    material->specularColor.setValue(0.27,0.27,0.27);
    material->shininess.setValue(.80);
    add("sct::FwdHybrid",material);
    add("sct::CoolBlockMainHi",material);
    add("sct::CoolBlockMainLo",material);
  }

  { /* Details in sct (B) */
    SoMaterial *material = new SoMaterial;
    material->diffuseColor.setValue(0.4,0.35,0.30);
    material->ambientColor.setValue(0.57,0.57,0.57);
    material->specularColor.setValue(0.27,0.27,0.27);
    material->shininess.setValue(.80);
    add("sct::CFiberInterLink",material);
    add("sct::Clamp0",material);
    add("sct::Clamp1",material);
    add("sct::Clamp2",material);
    add("sct::Clamp3",material);
    add("sct::CoolingEnd0",material);
    add("sct::CoolingEnd1",material);
    add("sct::CoolingEnd2",material);
    add("sct::CoolingEnd3",material);
    add("sct::CoolingPipe",material);
    add("sct::DiscFixation",material);
    add("sct::DiscSupport0",material);
    add("sct::DiscSupport1",material);
    add("sct::ModuleConnector",material);
    add("sct::DiscSupport2",material);
    add("sct::DiscSupport3",material);
    add("sct::DiscSupport4",material);
    add("sct::DiscSupport5",material);
    add("sct::DiscSupport6",material);
    add("sct::DiscSupport7",material);
    add("sct::DiscSupport8",material);
    add("sct::FSIBL",material);
    add("sct::FSIBH",material);
    add("sct::FSIFL",material);
    add("sct::FSIFH",material);
    add("sct::FwdCoolingPipe",material);
    add("sct::FwdFlangeFrontInn",material);
    add("sct::FwdFlangeFrontOut",material);
    add("sct::FwdFlangeRearInn",material);
    add("sct::FwdFlangeRearOut",material);
    add("sct::FwdNPipe",material);
    add("sct::FwdShuntShield",material);
    add("sct::PPConnector",material);
    add("sct::PPCooling",material);
    add("sct::PPF0c",material);
    add("sct::PPF0e",material);
    add("sct::PPF0o",material);
    add("sct::TSBulkhead",material);
    add("sct::TSEndPanel",material);
  }
  {
    SoMaterial * m = new SoMaterial;
    m->diffuseColor.setValue(SbColor(0.33333,0.33333,0.49804));
    m->shininess.setValue(0);
    add("sct::FwdFibres",m);
  }

//////////////
  init();
}

//////////////////////////////// Attributes for volumes ////////////////////////////////

VolVisAttributes::VolVisAttributes() {

  // WARM CYLINDER
  {
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue(1, .4, .4);
    m->diffuseColor.setValue(1, .4, .4);
    m->specularColor.setValue(0.441667, 0.441667, 0.441667);
    m->shininess.setValue(0.67);
    add( "LAr::Barrel::Cryostat::Cylinder::#4",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#5",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#6",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#7",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#8",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#9",m);
    add( "LAr::Barrel::Cryostat::InnerWall",m);
    add( "LAr::Barrel::Cryostat::InnerEndWall",m);
    add( "LAr::Barrel::Cryostat::Leg",m);
  }

  {
    // WARM CYLINDER
    SoMaterial *m = new SoMaterial;
    m->ambientColor.setValue(.4, .4, 1);
    m->diffuseColor.setValue(.4, .4, 1);
    m->specularColor.setValue(0.441667, 0.441667, 0.441667);
    m->shininess.setValue(0.67);
    add( "LAr::Barrel::Cryostat::Cylinder::#0",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#1",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#2",m);
    add( "LAr::Barrel::Cryostat::Cylinder::#3",m);
    add( "LAr::Barrel::Cryostat::Ear",m);
    add( "LAr::Barrel::Cryostat::OuterWall",m);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(0, .8, 0);
    material->diffuseColor.setValue(0, .8, 0);
    material->specularColor.setValue(0, .8, .0);
    material->shininess.setValue(0.642424);
    add("ModuleSvc",material);
    add("ISM57A_SvcEc20_RadL_L0_Sec0",material);
    add("ISM68A_SvcEc30_RadL_L4_Sec0",material);
    add("ISM58A_SvcEc20_RadL_L7_Sec0",material);
    add("ISM69A_SvcEc30_RadL_L9_Sec0",material);
    add("ISM59A_SvcEc20_RadL_L12_Sec0",material);
    add("ISM70A_SvcEc30_RadL_L15_Sec0",material);
    add("ISM60A_SvcEc20_RadL_L17_Sec0",material);
    add("ISM71A_SvcEc30_RadL_L21_Sec0",material);
    add("ISM61A_SvcEc20_RadL_L22_Sec0",material);
    add("ISM62A_SvcEc20_RadL_L25_Sec0",material);
    add("ISM63A_SvcEc20_RadL_L28_Sec0",material);
    add("ISM64A_SvcEc20_RadL_L31_Sec0",material);
    add("ISM65A_SvcEc20_RadL_L33_Sec0",material);
    add("ISM66A_SvcEc20_RadL_L37_Sec0",material);
    add("ISM67A_SvcEc20_RadL_L38_Sec0",material);
    add("ISM57C_SvcEc20_RadL_L0_Sec0",material);
    add("ISM68C_SvcEc30_RadL_L4_Sec0",material);
    add("ISM58C_SvcEc20_RadL_L7_Sec0",material);
    add("ISM69C_SvcEc30_RadL_L9_Sec0",material);
    add("ISM59C_SvcEc20_RadL_L12_Sec0",material);
    add("ISM70C_SvcEc30_RadL_L15_Sec0",material);
    add("ISM60C_SvcEc20_RadL_L17_Sec0",material);
    add("ISM71C_SvcEc30_RadL_L21_Sec0",material);
    add("ISM61C_SvcEc20_RadL_L22_Sec0",material);
    add("ISM62C_SvcEc20_RadL_L25_Sec0",material);
    add("ISM63C_SvcEc20_RadL_L28_Sec0",material);
    add("ISM64C_SvcEc20_RadL_L31_Sec0",material);
    add("ISM65C_SvcEc20_RadL_L33_Sec0",material);
    add("ISM66C_SvcEc20_RadL_L37_Sec0",material);
    add("ISM67C_SvcEc20_RadL_L38_Sec0",material);
    add("ISM08A_SvcEc21_Hor0_38_Sec0",material);
    add("ISM09A_SvcEc21_Hor0_38_Sec1",material);
    add("ISM10A_SvcEc21_Hor0_38_Sec2",material);
    add("ISM11A_SvcEc21_Hor0_38_Sec3",material);
    add("ISM12A_SvcEc21_Hor0_38_Sec4",material);
    add("ISM13A_SvcEc21_Hor0_38_Sec5",material);
    add("ISM14A_SvcEc21_Hor0_38_Sec6",material);
    add("ISM15A_SvcEc21_Hor0_38_Sec7",material);
    add("ISM16A_SvcEc21_Hor0_38_Sec8",material);
    add("ISM17A_SvcEc21_Hor0_38_Sec9",material);
    add("ISM18A_SvcEc21_Hor0_38_Sec10",material);
    add("ISM19A_SvcEc31_Hor4_21_Sec0",material);
    add("ISM20A_SvcEc31_Hor4_21_Sec1",material);
    add("ISM21A_SvcEc31_Hor4_21_Sec2",material);
    add("ISM22A_SvcEc31_Hor4_21_Sec3",material);
    add("ISM08C_SvcEc21_Hor0_38_Sec0",material);
    add("ISM09C_SvcEc21_Hor0_38_Sec1",material);
    add("ISM10C_SvcEc21_Hor0_38_Sec2",material);
    add("ISM11C_SvcEc21_Hor0_38_Sec3",material);
    add("ISM12C_SvcEc21_Hor0_38_Sec4",material);
    add("ISM13C_SvcEc21_Hor0_38_Sec5",material);
    add("ISM14C_SvcEc21_Hor0_38_Sec6",material);
    add("ISM15C_SvcEc21_Hor0_38_Sec7",material);
    add("ISM16C_SvcEc21_Hor0_38_Sec8",material);
    add("ISM17C_SvcEc21_Hor0_38_Sec9",material);
    add("ISM18C_SvcEc21_Hor0_38_Sec10",material);
    add("ISM19C_SvcEc31_Hor4_21_Sec0",material);
    add("ISM20C_SvcEc31_Hor4_21_Sec1",material);
    add("ISM21C_SvcEc31_Hor4_21_Sec2",material);
    add("ISM22C_SvcEc31_Hor4_21_Sec3",material);
    add("ISM72C_1_SvcEc40_RadL_L1_Sec0",material);
    add("ISM73C_1_SvcEc40_RadL_L5_Sec0",material);
    add("ISM74C_1_SvcEc40_RadL_L10_Sec0",material);
    add("ISM75C_1_SvcEc40_RadL_L14_Sec0",material);
    add("ISM76C_1_SvcEc40_RadL_L19_Sec0",material);
    add("ISM77C_1_SvcEc40_RadL_L23_Sec0",material);
    add("ISM78C_1_SvcEc40_RadL_L27_Sec0",material);
    add("ISM79C_1_SvcEc40_RadL_L32_Sec0",material);
    add("ISM80C_1_SvcEc40_RadL_L36_Sec0",material);
    add("ISM81C_1_SvcEc40_RadL_L39_Sec0",material);
    add("ISM23C_SvcEc41_Hor1_39_Sec0",material);
    add("ISM24C_SvcEc41_Hor1_39_Sec1",material);
    add("ISM25C_SvcEc41_Hor1_39_Sec2",material);
    add("ISM26C_SvcEc41_Hor1_39_Sec3",material);
    add("ISM27C_SvcEc41_Hor1_39_Sec4",material);
    add("ISM28C_SvcEc41_Hor1_39_Sec5",material);
    add("ISM29C_SvcEc41_Hor1_39_Sec6",material);
    add("ISM30C_SvcEc41_Hor1_39_Sec7",material);
    add("ISM31C_SvcEc41_Hor1_39_Sec8",material);
    add("ISM32C_SvcEc41_Hor1_39_Sec9",material);
    add("ISM82C_SvcEc50_RadL_L2_Sec0",material);
    add("ISM83C_SvcEc50_RadL_L8_Sec0",material);
    add("ISM84C_SvcEc50_RadL_L13_Sec0",material);
    add("ISM85C_SvcEc50_RadL_L18_Sec0",material);
    add("ISM86C_SvcEc50_RadL_L24_Sec0",material);
    add("ISM87C_SvcEc50_RadL_L29_Sec0",material);
    add("ISM88C_SvcEc50_RadL_L34_Sec0",material);
    add("ISM89C_SvcEc50_RadL_L40_Sec0",material);
    add("ISM33C_SvcEc51_Hor2_40_Sec0",material);
    add("ISM34C_SvcEc51_Hor2_40_Sec1",material);
    add("ISM35C_SvcEc51_Hor2_40_Sec2",material);
    add("ISM36C_SvcEc51_Hor2_40_Sec3",material);
    add("ISM37C_SvcEc51_Hor2_40_Sec4",material);
    add("ISM38C_SvcEc51_Hor2_40_Sec5",material);
    add("ISM39C_SvcEc51_Hor2_40_Sec6",material);
    add("ISM40C_SvcEc51_Hor2_40_Sec7",material);
    add("ISM90C_SvcEc60_RadL_L3_Sec0",material);
    add("ISM91C_SvcEc60_RadL_L6_Sec0",material);
    add("ISM92C_SvcEc60_RadL_L11_Sec0",material);
    add("ISM93C_SvcEc60_RadL_L16_Sec0",material);
    add("ISM94C_SvcEc60_RadL_L20_Sec0",material);
    add("ISM95C_SvcEc60_RadL_L26_Sec0",material);
    add("ISM96C_SvcEc60_RadL_L30_Sec0",material);
    add("ISM97C_SvcEc60_RadL_L35_Sec0",material);
    add("ISM98C_SvcEc60_RadL_L41_Sec0",material);
    add("ISM41C_SvcEc61_Hor3_41_Sec0",material);
    add("ISM42C_SvcEc61_Hor3_41_Sec1",material);
    add("ISM43C_SvcEc61_Hor3_41_Sec2",material);
    add("ISM44C_SvcEc61_Hor3_41_Sec3",material);
    add("ISM45C_SvcEc61_Hor3_41_Sec4",material);
    add("ISM46C_SvcEc61_Hor3_41_Sec5",material);
    add("ISM47C_SvcEc61_Hor3_41_Sec6",material);
    add("ISM48C_SvcEc61_Hor3_41_Sec7",material);
    add("ISM49C_SvcEc61_Hor3_41_Sec8",material);
    add("ISM72A_1_SvcEc40_RadL_L1_Sec0",material);
    add("ISM73A_1_SvcEc40_RadL_L5_Sec0",material);
    add("ISM74A_1_SvcEc40_RadL_L10_Sec0",material);
    add("ISM75A_1_SvcEc40_RadL_L14_Sec0",material);
    add("ISM76A_1_SvcEc40_RadL_L19_Sec0",material);
    add("ISM77A_1_SvcEc40_RadL_L23_Sec0",material);
    add("ISM78A_1_SvcEc40_RadL_L27_Sec0",material);
    add("ISM79A_1_SvcEc40_RadL_L32_Sec0",material);
    add("ISM80A_1_SvcEc40_RadL_L36_Sec0",material);
    add("ISM81A_1_SvcEc40_RadL_L39_Sec0",material);
    add("ISM23A_SvcEc41_Hor1_39_Sec0",material);
    add("ISM24A_SvcEc41_Hor1_39_Sec1",material);
    add("ISM25A_SvcEc41_Hor1_39_Sec2",material);
    add("ISM26A_SvcEc41_Hor1_39_Sec3",material);
    add("ISM27A_SvcEc41_Hor1_39_Sec4",material);
    add("ISM28A_SvcEc41_Hor1_39_Sec5",material);
    add("ISM29A_SvcEc41_Hor1_39_Sec6",material);
    add("ISM30A_SvcEc41_Hor1_39_Sec7",material);
    add("ISM31A_SvcEc41_Hor1_39_Sec8",material);
    add("ISM32A_SvcEc41_Hor1_39_Sec9",material);
    add("ISM82A_SvcEc50_RadL_L2_Sec0",material);
    add("ISM83A_SvcEc50_RadL_L8_Sec0",material);
    add("ISM84A_SvcEc50_RadL_L13_Sec0",material);
    add("ISM85A_SvcEc50_RadL_L18_Sec0",material);
    add("ISM86A_SvcEc50_RadL_L24_Sec0",material);
    add("ISM87A_SvcEc50_RadL_L29_Sec0",material);
    add("ISM88A_SvcEc50_RadL_L34_Sec0",material);
    add("ISM89A_SvcEc50_RadL_L40_Sec0",material);
    add("ISM33A_SvcEc51_Hor2_40_Sec0",material);
    add("ISM34A_SvcEc51_Hor2_40_Sec1",material);
    add("ISM35A_SvcEc51_Hor2_40_Sec2",material);
    add("ISM36A_SvcEc51_Hor2_40_Sec3",material);
    add("ISM37A_SvcEc51_Hor2_40_Sec4",material);
    add("ISM38A_SvcEc51_Hor2_40_Sec5",material);
    add("ISM39A_SvcEc51_Hor2_40_Sec6",material);
    add("ISM40A_SvcEc51_Hor2_40_Sec7",material);
    add("ISM90A_SvcEc60_RadL_L3_Sec0",material);
    add("ISM91A_SvcEc60_RadL_L6_Sec0",material);
    add("ISM92A_SvcEc60_RadL_L11_Sec0",material);
    add("ISM93A_SvcEc60_RadL_L16_Sec0",material);
    add("ISM94A_SvcEc60_RadL_L20_Sec0",material);
    add("ISM95A_SvcEc60_RadL_L26_Sec0",material);
    add("ISM96A_SvcEc60_RadL_L30_Sec0",material);
    add("ISM97A_SvcEc60_RadL_L35_Sec0",material);
    add("ISM98A_SvcEc60_RadL_L41_Sec0",material);
    add("ISM41A_SvcEc61_Hor3_41_Sec0",material);
    add("ISM42A_SvcEc61_Hor3_41_Sec1",material);
    add("ISM43A_SvcEc61_Hor3_41_Sec2",material);
    add("ISM44A_SvcEc61_Hor3_41_Sec3",material);
    add("ISM45A_SvcEc61_Hor3_41_Sec4",material);
    add("ISM46A_SvcEc61_Hor3_41_Sec5",material);
    add("ISM47A_SvcEc61_Hor3_41_Sec6",material);
    add("ISM48A_SvcEc61_Hor3_41_Sec7",material);
    add("ISM49A_SvcEc61_Hor3_41_Sec8",material);
    
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(.3, .3, .3);
    material->diffuseColor.setValue(.3, .3, .3);
    material->specularColor.setValue(.3, .3, .3);
    material->shininess.setValue(0.642424);
    add("Module",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(.6, 0., 0.);
    material->diffuseColor.setValue(.6, 0., 0.);
    material->specularColor.setValue(.6, 0., 0.);
    material->shininess.setValue(0.642424);
    add("FoamSupport",material);
  }

  {
    SoMaterial *material = new SoMaterial;
    material->ambientColor.setValue(.5, .5, 0.);
    material->diffuseColor.setValue(.5, .5, 0.);
    material->specularColor.setValue(.5, .5, 0.);
    material->shininess.setValue(0.642424);
    add("ChipBrl",material);
  }

  {
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue (1, 1, 0.5);
    m->ambientColor.setValue (0.54, 0.54, 0.27);
    m->emissiveColor.setValue (0.133, 0.133, 0.067);
    add("bcmModLog",m);
    add("bcmWallLog",m);
  }
  init();
}
