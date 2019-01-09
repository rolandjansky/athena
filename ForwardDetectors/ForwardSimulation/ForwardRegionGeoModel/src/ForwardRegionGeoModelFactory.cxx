/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegionGeoModelFactory.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoEllipticalTube.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoDefinitions.h"  
#include "GeoModelKernel/Units.h"  
#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "CLHEP/Geometry/Point3D.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "PathResolver/PathResolver.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <math.h>
#include <algorithm>

// #include <stdio.h> // FILENAME_MAX
// #include <unistd.h> // getcwd()

#include <stdlib.h>


void FWD_CONFIGURATION::clear()
{
    TCL4JawDistB1I = 57*GeoModelKernelUnits::mm;
    TCL5JawDistB1I = 57*GeoModelKernelUnits::mm;
    TCL6JawDistB1I = 57*GeoModelKernelUnits::mm;
    TCL4JawDistB2I = 57*GeoModelKernelUnits::mm;
    TCL5JawDistB2I = 57*GeoModelKernelUnits::mm;
    TCL6JawDistB2I = 57*GeoModelKernelUnits::mm;
    TCL4JawDistB1O = 57*GeoModelKernelUnits::mm;
    TCL5JawDistB1O = 57*GeoModelKernelUnits::mm;
    TCL6JawDistB1O = 57*GeoModelKernelUnits::mm;
    TCL4JawDistB2O = 57*GeoModelKernelUnits::mm;
    TCL5JawDistB2O = 57*GeoModelKernelUnits::mm;
    TCL6JawDistB2O = 57*GeoModelKernelUnits::mm;
    vp1Compatibility = false;
    buildTCL4 = false;
    buildTCL6 = false;
    ALFAInNewPosition = false;
    newPosB7L1 = 245656.77*GeoModelKernelUnits::mm;
    newPosB7R1 = -245656.11*GeoModelKernelUnits::mm;
    posAFPL1 = 204500*GeoModelKernelUnits::mm;
    posAFPL2 = 212675*GeoModelKernelUnits::mm;
    posAFPR1 = -204500*GeoModelKernelUnits::mm;
    posAFPL2 = -212675*GeoModelKernelUnits::mm;
}


ForwardRegionGeoModelFactory::ForwardRegionGeoModelFactory(StoreGateSvc *detStore, const PFWD_CONFIGURATION pConfig)
  :m_detectorStore(detStore),
   m_properties("ForwardRegionProperties")
{
    m_detectorManager = NULL;
    MsgStream LogStream(Athena::getMessageSvc(), "ForwardRegionGeoModel::ForwardRegionGeoModel()");

    m_Config = *pConfig;

    if(m_properties.retrieve().isFailure()){
        LogStream << MSG::ERROR << ": Failed to load magnet properties" << endmsg;
        return;
    }

    m_MagConfig = *(m_properties->getConf());
}


ForwardRegionGeoModelFactory::~ForwardRegionGeoModelFactory()
{

}


void ForwardRegionGeoModelFactory::DefineMaterials()
{
    std::string matName;

    const StoredMaterialManager* materialManager = nullptr;
    if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
      return;
    }


    //-----------------------------------------------------------------------------------//
    // Get the materials that we shall use.                                              //
    // ----------------------------------------------------------------------------------//

    // vacuum
    matName = "std::Vacuum";
    const GeoMaterial *vacuum = materialManager->getMaterial(matName);
    m_MapMaterials.emplace(matName,vacuum);

    // water
    matName = "water";
    GeoMaterial *water = new GeoMaterial("H20", 1.0*GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3);
    GeoElement *hydrogen = new GeoElement("Hydrogen","H",1.0, 1.010);
    GeoElement *oxygen   = new GeoElement("Oxygen",  "O", 8.0, 16.0);
    water->add(hydrogen,0.11);
    water->add(oxygen,0.89);
    water->lock();
    m_MapMaterials.emplace(matName,water);

    // elements
    const GeoElement* C  = materialManager->getElement("Carbon");
    const GeoElement* N  = materialManager->getElement("Nitrogen");
    const GeoElement* Si = materialManager->getElement("Silicon");
    const GeoElement* P  = materialManager->getElement("Phosphorus");
    const GeoElement* S  = materialManager->getElement("Sulfur");
    const GeoElement* Cr = materialManager->getElement("Chromium");
    const GeoElement* Mn = materialManager->getElement("Manganese");
    const GeoElement* Fe = materialManager->getElement("Iron");
    const GeoElement* Ni = materialManager->getElement("Nickel");
    const GeoElement* Mo = materialManager->getElement("Molybdenum");
    const GeoElement* Cu = materialManager->getElement("Copper");


    const GeoElement* Al  = materialManager->getElement("Aluminium");
    const GeoElement* B  = materialManager->getElement("Boron");
    const GeoElement* O  = materialManager->getElement("Oxygen");

    // Copper for beam screens
    matName = "Copper";
    GeoMaterial *copper = new GeoMaterial("Copper", 8.94*GeoModelKernelUnits::g/GeoModelKernelUnits::cm3);
    copper->add(const_cast<GeoElement*> (Cu),1.0);
    copper->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,copper));

    // Tungsten for TCL6
    matName = "Tungsten";
    const GeoElement* W = materialManager->getElement("Wolfram");
    GeoMaterial *tungsten = new GeoMaterial("Tungsten", 19.25*GeoModelKernelUnits::g/GeoModelKernelUnits::cm3);
    tungsten->add(const_cast<GeoElement*> (W),1.0);
    tungsten->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,tungsten));

    // GlidCop AL15 copper -- aproximate composition (trace impurities (< 0.01 wt. %) not included)
    // source: http://www-ferp.ucsd.edu/LIB/PROPS/compcu15.html
    matName = "GlidCopAL15";
    GeoMaterial *glidcop=new GeoMaterial("GlidCopAL15", 8.90*GeoModelKernelUnits::g/GeoModelKernelUnits::cm3);

    double aCu, aAl, aO, aB, aTot;

    aCu=99.7*Cu->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aAl=0.15*Al->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aO=0.13*O->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aB=0.02*B->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aTot=aCu+aAl+aO+aB;

    glidcop->add(const_cast<GeoElement*> (Cu), aCu/aTot);
    glidcop->add(const_cast<GeoElement*> (Al),  aAl/aTot);
    glidcop->add(const_cast<GeoElement*> (O), aO/aTot);
    glidcop->add(const_cast<GeoElement*> (B), aB/aTot);
    glidcop->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,glidcop));

    // Steel Grade 316L (Roman Pot)
    matName = "Steel";
    GeoMaterial *steel=new GeoMaterial("Steel", 8*GeoModelKernelUnits::g/GeoModelKernelUnits::cm3);

    double aC,aN,aSi,aP,aS,aCr,aMn,aFe,aNi,aMo,Atot;

    aFe=62.045*Fe->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aC =0.03*C ->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aMn=2.0*Mn ->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aSi=0.75*Si->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aP =0.045*P->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aS =0.03*S ->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aCr=18.0*Cr->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aMo=3.0*Mo ->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aNi=14.0*Ni->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    aN =0.10*N ->getA()/(GeoModelKernelUnits::g/GeoModelKernelUnits::mole);
    Atot=aFe+aC+aMn+aSi+aP+aS+aCr+aMo+aNi+aN;

    steel->add(const_cast<GeoElement*> (Fe),aFe/Atot);
    steel->add(const_cast<GeoElement*> (C),  aC/Atot);
    steel->add(const_cast<GeoElement*> (Mn),aMn/Atot);
    steel->add(const_cast<GeoElement*> (Si),aSi/Atot);
    steel->add(const_cast<GeoElement*> (P),  aP/Atot);
    steel->add(const_cast<GeoElement*> (S),  aS/Atot);
    steel->add(const_cast<GeoElement*> (Cr),aCr/Atot);
    steel->add(const_cast<GeoElement*> (Mo),aMo/Atot);
    steel->add(const_cast<GeoElement*> (Ni),aNi/Atot);
    steel->add(const_cast<GeoElement*> (N),  aN/Atot);
    steel->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,steel));
}

template <class T>
std::string ForwardRegionGeoModelFactory::num2str(T num)
{
    std::ostringstream strs;
    strs << num;
    return strs.str();
}

template <typename T> int ForwardRegionGeoModelFactory::sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void ForwardRegionGeoModelFactory::constructElements(GeoPhysVol *fwrPhys,std::vector<std::vector<std::string> > loadedDataFile, int beam)
{
    //-------------------------------------------------------------------------------------
    // Construction of geometry from geometry file

    // indexes
    int name, zStart, zEnd, type, xAperture, yAperture, xStart, xEnd, yStart, yEnd, tubeThickness;
    name = 0;
    type = 1;
    zStart = 4;
    zEnd = 5;
    xAperture = 2;
    yAperture = 3;
    xStart = 6;
    xEnd = 7;
    yStart = 8;
    yEnd = 9;
    tubeThickness = 10;

    int lDFSize = loadedDataFile.size();

    // apply shifts of magnets defined by magnets properties in JO
    for(int i=0; i < lDFSize; i++)
    {
        // get start and end points defined in JO
        HepGeom::Point3D<double> pointMagStart;
        HepGeom::Point3D<double> pointMagEnd;
        if(m_properties.retrieve().isSuccess()) m_properties->getMagTransforms(loadedDataFile[i][name],beam,pointMagStart,pointMagEnd);

        // are points defined (non-zero)? If not, shifts will not apply
        bool pointsDefined = !(pointMagStart.distance2() == 0 || pointMagEnd.distance2() == 0);

        if(pointsDefined)
        {

            // overlap correction for x rotation
            double startZ = pointMagStart[2];
            double endZ   = pointMagEnd[2];
            double startX = pointMagStart[0];
            double endX   = pointMagEnd[0];
            double rotationAngle = atan2(endX - startX,endZ - startZ);
            double r = atof(loadedDataFile[i][xAperture].c_str())*GeoModelKernelUnits::mm/2;
            double dL = abs(r*tan(rotationAngle))+0.2*GeoModelKernelUnits::mm;


            // move start and end points of the magnet element and neighbour elemens accordingly
            // move (resize) neighbours only in z (needed to avoid overlaps)
            loadedDataFile[i][xStart] = num2str(pointMagStart[0]/1000);
            loadedDataFile[i][yStart] = num2str(pointMagStart[1]/1000);
            loadedDataFile[i][zStart] = num2str(pointMagStart[2]/1000);
            loadedDataFile[i-1][zEnd] = num2str(sgn(pointMagStart[2])*(abs(pointMagStart[2])-dL)/1000);

            loadedDataFile[i][xEnd] = num2str(pointMagEnd[0]/1000);
            loadedDataFile[i][yEnd] = num2str(pointMagEnd[1]/1000);
            loadedDataFile[i+1][zStart] = num2str(sgn(pointMagEnd[2])*(abs(pointMagEnd[2])+dL)/1000);
            loadedDataFile[i][zEnd] = num2str(pointMagEnd[2]/1000);

        }
    }

    double x,y,z,halfL,rotationAngle,r,dL,startZ,endZ,startX,endX,startY,endY;

    // ################ ELEMENTS ############################

    // --------------- elements cycle -----------------
    for(int i=0; i < lDFSize; i++)
    {
        startZ = atof(loadedDataFile[i][zStart].c_str())*GeoModelKernelUnits::m;
        endZ   = atof(loadedDataFile[i][zEnd].c_str())*GeoModelKernelUnits::m;
        startX = atof(loadedDataFile[i][xStart].c_str())*GeoModelKernelUnits::m;
        endX   = atof(loadedDataFile[i][xEnd].c_str())*GeoModelKernelUnits::m;
        startY = atof(loadedDataFile[i][yStart].c_str())*GeoModelKernelUnits::m;
        endY   = atof(loadedDataFile[i][yEnd].c_str())*GeoModelKernelUnits::m;

        // translation of element
        x = (startX + endX)/2;
        y = (startY + endY)/2;
        z = (startZ + endZ)/2;

        // absolute rotation of element
        //rotationAngle_old = rotationAngle;
        rotationAngle = atan2(endX - startX,endZ - startZ);

        // half-length of element
        halfL = sqrt((endX - startX)*(endX - startX) + (endZ - startZ)*(endZ - startZ))/2;

        r = atof(loadedDataFile[i][xAperture].c_str())*GeoModelKernelUnits::mm/2;

        // overlap correction
        dL = abs(r*tan(rotationAngle))+0.2*GeoModelKernelUnits::mm;

        // do not shorten magnetic volumes
        if(loadedDataFile[i][name].find("Mag") != std::string::npos)
            dL = 0;

        // construction of element

        // circular aperture
        if(atoi(loadedDataFile[i][type].c_str()) == 0){
            // envelope to allow tracking with G4TrackAction
            if(loadedDataFile[i][name] == "VCDBP.7R1.B"){
                GeoPhysVol* trackEnv = insertMagnetEnvelope(loadedDataFile[i][name], x, y, z, rotationAngle, 100*GeoModelKernelUnits::mm, halfL, dL, fwrPhys);
                insertCircularElement(loadedDataFile[i][name], x, y, z, rotationAngle, atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()), halfL, dL, atof(loadedDataFile[i][tubeThickness].c_str()), trackEnv);
            }
            else
              insertCircularElement(loadedDataFile[i][name], x, y, z, rotationAngle, atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()), halfL, dL, atof(loadedDataFile[i][tubeThickness].c_str()), fwrPhys);
        }

        // special volumes
        if(atoi(loadedDataFile[i][type].c_str()) == 4){
            if(loadedDataFile[i][name] == "VCTYF.A4R1.X") // Trousers -- transition from 1 to 2 beampipes
                insertTrousersElement(loadedDataFile[i][name], x, y, z, rotationAngle, fwrPhys);
            else if(loadedDataFile[i][name] == "TCL.5R1.B1") // TCL5 collimator
                insertTCLElement(loadedDataFile[i][name], x, y, z, fwrPhys, (beam == 1 ? m_Config.TCL5JawDistB1O : m_Config.TCL5JawDistB2O), (beam == 1 ? m_Config.TCL5JawDistB1I : m_Config.TCL5JawDistB2I));
            else if(m_Config.buildTCL4 && loadedDataFile[i][name] == "VCDSS.4R1.B") // TCL4 collimator
                insertTCLElement(loadedDataFile[i][name], x, y, z, fwrPhys, (beam == 1 ? m_Config.TCL4JawDistB1O : m_Config.TCL4JawDistB2O), (beam == 1 ? m_Config.TCL4JawDistB1I : m_Config.TCL4JawDistB2I));
            else if(m_Config.buildTCL6 && loadedDataFile[i][name] == "TCL6") // TCL6 collimator
                insertTCLElement(loadedDataFile[i][name], x, y, z, fwrPhys, (beam == 1 ? m_Config.TCL6JawDistB1O : m_Config.TCL6JawDistB2O), (beam == 1 ? m_Config.TCL6JawDistB1I : m_Config.TCL6JawDistB2I), true);

            // aproximate rest by circular apperture
            else
                insertCircularElement(loadedDataFile[i][name], x, y, z, rotationAngle, atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()), halfL, dL, atof(loadedDataFile[i][tubeThickness].c_str()), fwrPhys);
        }

        GeoPhysVol* magEnv;

        // elliptical aperture
        if(atoi(loadedDataFile[i][type].c_str()) == 1) {
            magEnv = insertMagnetEnvelope(loadedDataFile[i][name], x, y, z, rotationAngle, 20*GeoModelKernelUnits::cm, halfL, dL, fwrPhys);
            insertEllipticalElement(loadedDataFile[i][name], x, y, z, rotationAngle, atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()), halfL, dL, atof(loadedDataFile[i][tubeThickness].c_str()), magEnv);
        }


        double magDiam = 19.4*GeoModelKernelUnits::cm;
        if(loadedDataFile[i][name].find("Mag") != std::string::npos)
            magDiam= 19.4*GeoModelKernelUnits::cm;
        if(loadedDataFile[i][name] == "LQXAA.1R1MagQ1" || loadedDataFile[i][name] == "LQXAG.3R1MagQ3")
            magDiam = 48*GeoModelKernelUnits::cm;
        if(loadedDataFile[i][name] == "LQXBA.2R1MagQ2a" || loadedDataFile[i][name] == "LQXBA.2R1MagQ2b")
            magDiam = 52*GeoModelKernelUnits::cm;
        //else magDiam = std::max(atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()))+2*atof(loadedDataFile[i][tubeThickness].c_str());
        //else magDiam = 19.4*GeoModelKernelUnits::cm;

        // rectcircular aperture with flats in x
        if(atoi(loadedDataFile[i][type].c_str()) == 2) {
            magEnv = insertMagnetEnvelope(loadedDataFile[i][name], x, y, z, rotationAngle, magDiam, halfL, dL, fwrPhys);
            insertXRecticircularElement(loadedDataFile[i][name], x, y, z, rotationAngle, atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()), halfL, dL, atof(loadedDataFile[i][tubeThickness].c_str()), magEnv);
        }

        // rectcircular aperture with flats in y
        if(atoi(loadedDataFile[i][type].c_str()) == 3) {
            magEnv = insertMagnetEnvelope(loadedDataFile[i][name], x, y, z, rotationAngle, magDiam, halfL, dL, fwrPhys);
            insertYRecticircularElement(loadedDataFile[i][name], x, y, z, rotationAngle, atof(loadedDataFile[i][xAperture].c_str()), atof(loadedDataFile[i][yAperture].c_str()), halfL, dL, atof(loadedDataFile[i][tubeThickness].c_str()), magEnv);
        }
    }
    // ################ ELEMENTS - end ########################
}


void ForwardRegionGeoModelFactory::create(GeoPhysVol *world)
{
  m_detectorManager=new ForwardRegionGeoModelManager();

  MsgStream LogStream(Athena::getMessageSvc(), "ForwardRegionGeoModel::create()");

  LogStream << MSG::INFO << "Constructing forward region model" << endmsg;

  DefineMaterials();

  // Load "geometry" files
  std::vector<std::vector<std::string> > loadedDataFileR = this->loadDataFile((char*)"LSS1Rout.csv",11);
  std::vector<std::vector<std::string> > loadedDataFileL = this->loadDataFile((char*)"LSS1Lout.csv",11);

  double startZ,endZ;

  if(m_Config.vp1Compatibility) startZ = 19.0*GeoModelKernelUnits::m;
  else startZ = 22.0*GeoModelKernelUnits::m;
  endZ = 268.904*GeoModelKernelUnits::m;

  //rotationAngle_old = 0;

  // mother volume -- union of tubes, one for each side
  //const GeoBox      *fwrBox    = new GeoBox(2*GeoModelKernelUnits::m,0.5*GeoModelKernelUnits::m,(endZ-startZ)/2);
  const GeoTube     *fwrTubeL    = new GeoTube(0,2*GeoModelKernelUnits::m,(endZ-startZ)/2);
  GeoTube     *fwrTubeR    = new GeoTube(0,2*GeoModelKernelUnits::m,(endZ-startZ)/2);
  GeoTrf::Transform3D shiftL = GeoTrf::Translate3D(0,0,(endZ+startZ)/2);
  GeoTrf::Transform3D shiftR = GeoTrf::Translate3D(0,0,-(endZ+startZ)/2);

  const GeoShapeShift& fwrTube0 = (*fwrTubeL)<<shiftL;
  const GeoShapeUnion& fwrTube1 = fwrTube0.add((*fwrTubeR)<<shiftR);

  // cut out slots for ALFA
  const GeoTube     *alfa    = new GeoTube(0, 2*GeoModelKernelUnits::m, 500*GeoModelKernelUnits::mm);
  GeoTrf::Transform3D shiftAlfaL1 = GeoTrf::Translate3D(0,0,237388*GeoModelKernelUnits::mm);
  GeoTrf::Transform3D shiftAlfaR1 = GeoTrf::Translate3D(0,0,-237408*GeoModelKernelUnits::mm);
  GeoTrf::Transform3D shiftAlfaL2 = GeoTrf::Translate3D(0,0,(m_Config.ALFAInNewPosition ? m_Config.newPosB7L1 : 241528*GeoModelKernelUnits::mm));
  GeoTrf::Transform3D shiftAlfaR2 = GeoTrf::Translate3D(0,0,(m_Config.ALFAInNewPosition ? m_Config.newPosB7R1 :-241548*GeoModelKernelUnits::mm));
  const GeoShapeSubtraction& fwrTube2 = fwrTube1.subtract((*alfa)<<shiftAlfaL1).subtract((*alfa)<<shiftAlfaL2).subtract((*alfa)<<shiftAlfaR1).subtract((*alfa)<<shiftAlfaR2);

  // cut out slots for AFP
  const GeoTube     *afp    = new GeoTube(0, 2.5*GeoModelKernelUnits::m, 280*GeoModelKernelUnits::mm);
  const GeoTube     *afp2    = new GeoTube(0, 2.5*GeoModelKernelUnits::m, 580*GeoModelKernelUnits::mm);
  GeoTrf::Transform3D shiftAfpL1 = GeoTrf::Translate3D(0,0,m_Config.posAFPL1);
  GeoTrf::Transform3D shiftAfpR1 = GeoTrf::Translate3D(0,0,m_Config.posAFPR1);
  GeoTrf::Transform3D shiftAfpL2 = GeoTrf::Translate3D(0,0,m_Config.posAFPL2);
  GeoTrf::Transform3D shiftAfpR2 = GeoTrf::Translate3D(0,0,m_Config.posAFPR2);
  const GeoShapeSubtraction& fwrTube = fwrTube2.subtract((*afp)<<shiftAfpL1).subtract((*afp)<<shiftAfpR1).subtract((*afp2)<<shiftAfpL2).subtract((*afp2)<<shiftAfpR2);


  const GeoLogVol   *fwrLog    = new GeoLogVol("ForwardRegionGeoModel", &fwrTube, m_MapMaterials[std::string("std::Vacuum")]);
  GeoPhysVol        *fwrPhys   = new GeoPhysVol(fwrLog);
  GeoNameTag *tag = new GeoNameTag("ForwardRegionGeoModel");
  world->add(tag);
  world->add(fwrPhys);
  m_detectorManager->addTreeTop(fwrPhys);

  constructElements(fwrPhys,loadedDataFileR,1);
  constructElements(fwrPhys,loadedDataFileL,2);

  LogStream << MSG::INFO << "Forward region model succesfully constructed." << endmsg;
}

const ForwardRegionGeoModelManager * ForwardRegionGeoModelFactory::getDetectorManager() const
{
  return m_detectorManager;
}

// Load data from file into 2D array of strings. Input is filename and wanted numbestd::stringof columns
std::vector<std::vector<std::string> > ForwardRegionGeoModelFactory::loadDataFile(char* fileName, int cols)
{
    std::vector<std::vector<std::string> > loadedData;

    MsgStream LogStream(Athena::getMessageSvc(), "ForwardRegionGeoModel::loadDataFile()");

    std::ifstream file (fileName);
    if(!file){
        std::string datapath = PathResolver::find_file(fileName,"DATAPATH", PathResolver::RecursiveSearch);
        LogStream << MSG::DEBUG << "File " << fileName << " not found in run directory, trying to load it from DATAPATH" << endmsg;
        file.open(datapath.c_str());
    }

    if(!file)
        LogStream << MSG::FATAL << "Unable to load " << fileName << endmsg;

    if(file.is_open())
    {
	std::vector<std::string> row (cols);
	char c;

        while(file.get(c))
        {
            if(c != '@' && c != '#' && c != '*' && c != '$' && c != '%')
            {
                file.unget();
                for(int i = 0; i<cols; i++) // load desired columns
                {
                    file >> row[i];
                }
                loadedData.push_back(row); //store them
                file.ignore(1024,'\n'); // discard rest of line
            }
            else
              file.ignore(1024, '\n'); // discard commented lines
        }
        LogStream << MSG::INFO << "File " << fileName << " succesfully loaded." << endmsg;
        file.close();
    }
    return loadedData;
}
