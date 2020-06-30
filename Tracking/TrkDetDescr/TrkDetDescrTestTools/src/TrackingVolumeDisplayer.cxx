/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeDisplayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTestTools/TrackingVolumeDisplayer.h"
#include "TrkDetDescrUtils/BinnedArray.h"

#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/DiscTrapezoidalBounds.h"
#include "TrkSurfaces/Surface.h"
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
// Root
#include "TString.h"
#include <sstream>
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

int Trk::TrackingVolumeDisplayer::s_displaySurfaces = 0;

// constructor
Trk::TrackingVolumeDisplayer::TrackingVolumeDisplayer(const std::string& t, const std::string& n, const IInterface* p) : 
  Trk::RecursiveGeometryProcessor(t,n,p),
  m_volumeCounter(0),
  m_fileVolumeOutputName("TrackingGeometryVolumeDisplay.C"),
  m_fileVolumeOutputMode(true),
  m_fileLayerOutputName("TrackingGeometryLayerDisplay.C"),
  m_fileLayerOutputMode(true),
  m_fileSurfaceOutputName("TrackingGeometrySurfaceDisplay.C"),
  m_fileSurfaceOutputMode(true),
  m_fileSurfaceOutputSplit(false)
{
    // set name and output type
    declareProperty("TrackingVolumeOutputFile",  m_fileVolumeOutputName);
    declareProperty("TrackingVolumeOutput",      m_fileVolumeOutputMode);
    declareProperty("LayerOutputFile",           m_fileLayerOutputName);
    declareProperty("LayerOutput",               m_fileLayerOutputMode);
    declareProperty("SurfaceOutputFile",         m_fileSurfaceOutputName);
    declareProperty("SurfaceOutput",             m_fileSurfaceOutputMode);
    declareProperty("SurfaceOutputSplit",        m_fileSurfaceOutputSplit);
}

// destructor
Trk::TrackingVolumeDisplayer::~TrackingVolumeDisplayer()
{}


// the interface methods
StatusCode Trk::TrackingVolumeDisplayer::initialize()
{

    ATH_MSG_INFO( "initialize()" );    

    if (Trk::RecursiveGeometryProcessor::initialize().isFailure()){
        ATH_MSG_FATAL("Could not initialize base tool. Aborting.");
        return StatusCode::FAILURE;
    }

    // open the file for writing 
    if (m_fileVolumeOutputMode)
        openFile(m_fileVolumeOutput, m_fileVolumeOutputName);
    // open the file for writing
    if (m_fileLayerOutputMode)
        openFile(m_fileLayerOutput, m_fileLayerOutputName);
    // open the file for writing
    if (m_fileSurfaceOutputMode && !m_fileSurfaceOutputSplit)
        openFile(m_fileSurfaceOutput, m_fileSurfaceOutputName);

    return StatusCode::SUCCESS;

}    

StatusCode Trk::TrackingVolumeDisplayer::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );    

    // open the file for writing 
    if (m_fileVolumeOutputMode)
        closeFile(m_fileVolumeOutput);
    // open the file for writing
    if (m_fileLayerOutputMode)
        closeFile(m_fileLayerOutput);
    // open the file for writing
    if (m_fileSurfaceOutputMode && !m_fileSurfaceOutputSplit)
        closeFile(m_fileSurfaceOutput);

    return StatusCode::SUCCESS;
}


void Trk::TrackingVolumeDisplayer::openFile(std::ofstream& out, const std::string& filename) const
{
    // open the file
    out.open(filename.c_str());
    // write the header
    out << "{" << '\n';  
    out << "tg = new TCanvas(\"tg\",\"Geometry Shapes\",200,10,700,500);" <<'\n';
    out << "   world = new TBRIK(\"BRIK\",\"BRIK\",\"void\",2,2,2);" << '\n';
    out << "   worldnode = new TNode(\"NODE1\",\"NODE1\",\"BRIK\");" << '\n';
    out << "   worldnode->cd();" << '\n';
}


void Trk::TrackingVolumeDisplayer::closeFile(std::ofstream& out) const
{
    out << "worldnode->cd();" << '\n';
    out << "worldnode->Draw(\"ogl\");" << '\n';
    out << "tg->Update();" << '\n';
    out << "}" << '\n';
    out.close();
}


StatusCode Trk::TrackingVolumeDisplayer::processNode(const Trk::TrackingVolume& tvol, size_t) const
{

    ATH_MSG_VERBOSE("Writing display information for TrackingVolume.");

    // make a dynamic cast to the CylinderVolumeBounds
    const Trk::CylinderVolumeBounds* cvol = dynamic_cast<const Trk::CylinderVolumeBounds*>(&tvol.volumeBounds());

    double rMin  = 0.;
    double rMax  = 0.;
    double halfZ = 0.; 

    if (cvol) {
        // from the color code one can determine if its a gap volume
        // set the boundaries
        rMin  = cvol->innerRadius();
        rMax  = cvol->outerRadius();
        halfZ = cvol->halflengthZ();
    }

   // get the name
   std::string volumeName = tvol.volumeName();
   // write a debug line
   m_fileVolumeOutput << "// Processing TrackingVolume '" << volumeName << "'. " << '\n'; 
   m_fileVolumeOutput << "CylinderVolume" << m_volumeCounter  << " = new TTUBE(\"" << volumeName << "\",\"" << volumeName << "\",\"void\",";
   m_fileVolumeOutput << int(rMin)+1 << "," << int(rMax)-1 << "," << int(halfZ)-1 << ");" << '\n';
   m_fileVolumeOutput << "CylinderVolume" << m_volumeCounter  << "->SetLineColor(" << tvol.colorCode() << ");" << '\n';
   double zPos = tvol.center().z();
   m_fileVolumeOutput << "CylinderNode" << m_volumeCounter << " = new TNode(\"" << volumeName << "Node\",\"" << volumeName << "Node\",\"";
   m_fileVolumeOutput << volumeName << "\"," << 0 << "," << 0 << "," << int(zPos) << ");" << '\n';
   m_fileVolumeOutput <<'\n';
   ++m_volumeCounter;
   
   // return successful
   return StatusCode::SUCCESS;
}


StatusCode Trk::TrackingVolumeDisplayer::processNode(const Trk::Layer& lay, size_t) const
{
    
    ATH_MSG_VERBOSE("Writing display information for Layer.");
    // get the identifier
    int layerIndex = lay.layerIndex().value();
    int layerIndexStr = (layerIndex < 0) ? (-layerIndex)+1 : layerIndex;  
    // the layer position
    const Amg::Vector3D& layerPosition  = lay.surfaceRepresentation().center();
    double               layerPositionZ = layerPosition.z();
    // output mode
    if (m_fileSurfaceOutputMode && m_fileSurfaceOutputSplit) {
        std::stringstream ss;
        std::string str;
        ss << "Layer" << layerIndexStr << "_" << m_fileSurfaceOutputName; 
        ss >> str;
        openFile(m_fileSurfaceOutput, str);
        m_fileSurfaceOutput<<"// Enclosing Volume: "<<lay.enclosingTrackingVolume()->volumeName()<<std::endl;
    }

    if (m_fileSurfaceOutputMode) {
        const Trk::SurfaceArray* surfArray = lay.surfaceArray();
        if (surfArray) {
            auto& layerSurfaces = surfArray->arrayObjects();
            // loop over the surfaces and draw them
            for (auto& laySurfIter : layerSurfaces) {
                if ( laySurfIter && processNode(*laySurfIter).isFailure()){
                    ATH_MSG_FATAL("Failed to call processNode(const Surface& sf) on sub surface. Abort.");
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    if (m_fileSurfaceOutputMode && m_fileSurfaceOutputSplit)
        closeFile(m_fileSurfaceOutput);

    const Trk::Surface& lSurface = lay.surfaceRepresentation();
    int layerColor = lay.enclosingTrackingVolume() ? lay.enclosingTrackingVolume()->colorCode() : 22;

    // CYLINDER SECTION
    if (lSurface.type() == Trk::Surface::Cylinder && layerIndex > 0) {
        // get the Bounds
        const Trk::CylinderBounds* cylBo = dynamic_cast<const Trk::CylinderBounds*>(&lSurface.bounds());
        if (!cylBo) {
          m_fileLayerOutput << "ERROR Cylinder surface without CylinderBounds\n";
          return StatusCode::FAILURE;
        }
        else {
        // set the boundaries
        double radius  = cylBo->r();
        double halfZ   = cylBo->halflengthZ();
        m_fileLayerOutput << "Cylinder" << layerIndexStr << " = new TTUBE(\"CylinderLayer";
        m_fileLayerOutput << layerIndexStr << "\",\"CylinderLayer" << layerIndex << "\",\"void\",";
        m_fileLayerOutput << int(radius-5) << "," << int(radius+5) << "," << int(halfZ) << ");" << '\n';
        // get the line color 
        m_fileLayerOutput << "Cylinder" << layerIndexStr << "->SetLineColor(" << layerColor << ");" << '\n';
        // write the node
        m_fileLayerOutput <<"Node" << layerIndexStr << " = new TNode(\"CylinderNode" << layerIndex;
        m_fileLayerOutput << "\",\"CylinderNode" << layerIndexStr << "\",\"CylinderLayer";
        m_fileLayerOutput << layerIndexStr << "\"," << 0 << "," << 0 << "," << int(layerPositionZ) << ");" << '\n';
        m_fileLayerOutput <<'\n';

        return StatusCode::SUCCESS;
        }
    } else if (lSurface.type() == Trk::Surface::Disc && layerIndex > 0) {
        // get the Bounds
        const Trk::DiscBounds* discBo = dynamic_cast<const Trk::DiscBounds*>(&lSurface.bounds());
        if (!discBo) {
          m_fileLayerOutput << "ERROR Disc surface without DiscBounds\n";
          return StatusCode::FAILURE;
        }
        else {

        // set the boundaries
        double rMin    = discBo->rMin();
        double rMax    = discBo->rMax();

        m_fileLayerOutput << "Disc" << layerIndexStr << " = new TTUBE(\"DiscLayer" << layerIndexStr << "\",\"DiscLayer" << layerIndex << "\",\"void\",";
        m_fileLayerOutput << int(rMin+1.) << "," << int(rMax-1.) << ",10);" << '\n';
        // get the line color 
        m_fileLayerOutput << "Disc" << layerIndexStr << "->SetLineColor(" << layerColor << ");" << '\n';
        // write the node
        m_fileLayerOutput << "DiscNode" << layerIndexStr << " = new TNode(\"DiscNode" << layerIndex;
        m_fileLayerOutput << "\",\"DiscNode" << layerIndexStr << "\",\"DiscLayer";
        m_fileLayerOutput << layerIndexStr << "\"," << 0 << "," << 0 << "," << int(layerPositionZ) << ");" << '\n';
        m_fileLayerOutput <<'\n';
        return StatusCode::SUCCESS;
        }
    }
    return StatusCode::SUCCESS;    
}


StatusCode Trk::TrackingVolumeDisplayer::processNode(const Trk::Surface& sf, size_t) const
{

    ++s_displaySurfaces;

    ATH_MSG_VERBOSE("Writing display information for Surface.");


    m_fileSurfaceOutput << "// ---------------------- Surface -------------------------------// " << std::endl;

    // surface center
    double surfX = sf.center().x();
    double surfY = sf.center().y();
    double surfZ = sf.center().z();
    // rotation matrix
    const Amg::RotationMatrix3D sfRot = sf.transform().rotation();

    // ROOT wants the angles in degrees
    const double convFac = 180./M_PI;
    double xPhi   = sfRot.col(0).phi()*convFac;
    double xTheta = sfRot.col(0).theta()*convFac;
    double yPhi   = sfRot.col(1).phi()*convFac;
    double yTheta = sfRot.col(1).theta()*convFac;
    double zPhi   = sfRot.col(2).phi()*convFac;
    double zTheta = sfRot.col(2).theta()*convFac;

    // surface 
    TString surfaceString   = "Surface_";
    surfaceString  += s_displaySurfaces;
    TString rotationString  = "Rotation_";
    rotationString += s_displaySurfaces;
    TString nodeString      = "Node";
    nodeString     += s_displaySurfaces;
    // test version just with planar bounds
    const Trk::RectangleBounds* recBo = dynamic_cast<const Trk::RectangleBounds*>(&(sf.bounds()));
    const Trk::TrapezoidBounds* traBo = recBo ? 0 : dynamic_cast<const Trk::TrapezoidBounds*>(&(sf.bounds()));
    const Trk::DiscTrapezoidalBounds* disctraBo = (recBo || traBo) ? 0 : dynamic_cast<const Trk::DiscTrapezoidalBounds*>(&(sf.bounds()));
    if (recBo) {
        // it is a rectangle surface
        double halfX = recBo->halflengthX();
        double halfY = recBo->halflengthY();
        // prepare the surface
        m_fileSurfaceOutput << surfaceString << " =  new TBRIK(\""<< surfaceString << "\",\"" << surfaceString<< "\",\"void\",";
        m_fileSurfaceOutput << halfX << ","<<  halfY <<" ,2);" << '\n';

        m_fileSurfaceOutput << surfaceString << "->SetLineColor(80);" << std::endl;

        // the rotation    
        m_fileSurfaceOutput << rotationString << "= new TRotMatrix(\" "<< rotationString << " \",\" " << rotationString << " \", ";
        m_fileSurfaceOutput << xTheta << "," << xPhi << "," << yTheta << "," << yPhi << ", " << zTheta << "," << zPhi << ");" << std::endl;

    } else if (traBo) {
      //example
        //TGTRA gtra = new TGTRA("GTRA","GTRA","void",390,0,0,20,60,40,90,15120,80180,15);
        // prepare the surface (swap Y and Z)
        m_fileSurfaceOutput << surfaceString << " =  new TTRD1(\""<< surfaceString << "\",\"" << surfaceString<< "\",\"void\",";
        m_fileSurfaceOutput << traBo->minHalflengthX() << "," << traBo->maxHalflengthX() << ",2," << traBo->halflengthY() << ");"  << '\n';

        m_fileSurfaceOutput << surfaceString << "->SetLineColor(80);" << std::endl;

        // the rotation (swap Y and Z)
        m_fileSurfaceOutput << rotationString << "= new TRotMatrix(\" "<< rotationString << " \",\" " << rotationString << " \", ";
        m_fileSurfaceOutput << xTheta << "," << xPhi << ", " << zTheta << "," << zPhi << "," << yTheta << "," << yPhi << ");" << std::endl;
    } else if (disctraBo) {
      //double rMedium  = disctraBo->rCenter();
      //double Rc = (sf.center()).perp();
      //double side = (sf.normal()).z();
      //double stereo = side*2.*asin(Rc/(2.*rMedium));
      //double avePhi = (side>0) ? disctraBo->averagePhi()+stereo : -disctraBo->averagePhi()+M_PI+stereo; 

      double stereo  = disctraBo->stereo();
      double rMedium = disctraBo->rCenter();
      double avePhi  = disctraBo->averagePhi()+stereo;
      
      surfX = rMedium*cos(avePhi);
      surfY = rMedium*sin(avePhi);
      surfZ = (sf.center()).z();

      Amg::RotationMatrix3D rotation;
      rotation = Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())*
	Amg::AngleAxis3D(0., Amg::Vector3D::UnitY())*
	Amg::AngleAxis3D(-M_PI/2.+avePhi-surfZ/fabs(surfZ)*stereo, Amg::Vector3D::UnitZ());

      // ROOT wants the angles in degrees
      xPhi   = rotation.col(0).phi()*convFac;
      xTheta = rotation.col(0).theta()*convFac;
      yPhi   = rotation.col(1).phi()*convFac;
      yTheta = rotation.col(1).theta()*convFac;
      zPhi   = rotation.col(2).phi()*convFac;
      zTheta = rotation.col(2).theta()*convFac;
	
      //example
      //TGTRA gtra = new TGTRA("GTRA","GTRA","void",390,0,0,20,60,40,90,15120,80180,15);
      // prepare the surface (swap Y and Z)
      m_fileSurfaceOutput << surfaceString << " =  new TTRD1(\""<< surfaceString << "\",\"" << surfaceString<< "\",\"void\",";
      m_fileSurfaceOutput << disctraBo->minHalflengthX() << "," << disctraBo->maxHalflengthX() << ",2," << disctraBo->halflengthY() << ");"  << '\n';
      
      m_fileSurfaceOutput << surfaceString << "->SetLineColor(80);" << std::endl;
      
      // the rotation (swap Y and Z)
      m_fileSurfaceOutput << rotationString << "= new TRotMatrix(\" "<< rotationString << " \",\" " << rotationString << " \", ";
      m_fileSurfaceOutput << xTheta << "," << xPhi << ", " << zTheta << "," << zPhi << "," << yTheta << "," << yPhi << ");" << std::endl;
    }
    
    // the node
    m_fileSurfaceOutput << nodeString << " = new TNode(\"" << nodeString << "\",\"" << nodeString << "\"," << surfaceString;
    m_fileSurfaceOutput << "," << surfX << ", " << surfY << "," << surfZ << ", " << rotationString << ");" << std::endl;
    
    return StatusCode::SUCCESS;    

}


int Trk::TrackingVolumeDisplayer::colorCodeFromMaterial(const Trk::MaterialProperties* prop, std::ofstream& output) const
{
    if (!prop) return 6;
    // start from 100 and reduce by radiation length
    int color = 100;
    double tInX0 = prop->thicknessInX0();
    // write out a comment line
    output << "// MaterialProperties : t/X0    = " << tInX0 << std::endl;
    output << "//     zOverAtimesRho : Z/A*rho = " << prop->zOverAtimesRho() << std::endl;
    // 0 % : --> color code 100
    // 4 % : --> color code 52 
    color -= int(tInX0 * 1200);
    return color;
}


