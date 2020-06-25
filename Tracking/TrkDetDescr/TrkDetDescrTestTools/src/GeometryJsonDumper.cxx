/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryJsonDumper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg includes
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
// Trk include
#include "TrkDetDescrTestTools/GeometryJsonDumper.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RectangleBounds.h"
// output stream
#include <sstream>

// Gaudi
#include "GaudiKernel/ITHistSvc.h"


// constructor
Trk::GeometryJsonDumper::GeometryJsonDumper(const std::string& t, const std::string& n, const IInterface* p) : 
  Trk::RecursiveGeometryProcessor(t,n,p),
  m_outputFileName("TrackingGeometry.json"),
  m_outputPrecision(6),
  m_firstLayerWritten(false)
{
    declareProperty("OutputFileName",  m_outputFileName);
    declareProperty("OutputPrecision", m_outputPrecision);
}

// destructor
Trk::GeometryJsonDumper::~GeometryJsonDumper()
{}


StatusCode Trk::GeometryJsonDumper::initialize()
{
    // open the file for writing
    m_outputFile.open(m_outputFileName.c_str());
    m_outputFile << "[";
    m_outputFile << std::setiosflags(std::ios::fixed);
    m_outputFile << std::setprecision(3);        
    // return the base::initialize() state
    return Trk::RecursiveGeometryProcessor::initialize();    
}

StatusCode Trk::GeometryJsonDumper::finalize()
{
    // close the file
    m_outputFile << "]" << std::endl;
    m_outputFile.close();
    // return the base::finalize() state    
    return Trk::RecursiveGeometryProcessor::finalize();
}



StatusCode Trk::GeometryJsonDumper::processNode(const Trk::TrackingVolume& /*tvol*/, size_t /*level*/) const
{
   return StatusCode::SUCCESS;
}


StatusCode Trk::GeometryJsonDumper::processNode(const Trk::Layer& lay, size_t /*level*/) const
{
    
    ATH_MSG_VERBOSE("Dumping information for Layer with index " << lay.layerIndex().value());

    if (lay.surfaceArray()){
        size_t nSurfaces = lay.surfaceArray()->arrayObjects().size();
        // the layer has a surface Array - go for it
        // get the dimensions
        const Trk::Surface* referenceSurface = lay.subSurfaceReference();
        // the reference Surface exists
        if (referenceSurface){
            // dynamic_cast to RectangleBounds
            const Trk::RectangleBounds* rBounds = dynamic_cast<const Trk::RectangleBounds*>(&(referenceSurface->bounds()));
            // we have rBounds - go on for the moment
            if (rBounds){
                if (m_firstLayerWritten) m_outputFile << ",";
                // the layer is defined
                m_outputFile << "{\"Name\" : \"Layer " << lay.layerIndex().value() << "\", ";
                m_outputFile << "\"Dimensions\" : [" <<  2*rBounds->halflengthX() << "," << 2.*rBounds->halflengthY() << ", 1.0 ], ";
                // count the surfaces
                size_t is = 1;
                // now loop of the surfaces and dumpt their position
                m_outputFile << "\"Coords\": [";
                for (auto& sf : lay.surfaceArray()->arrayObjects()){
                    // get x,y,z 
                    double cx = sf->center().x();
                    double cy = sf->center().y();
                    double cz = sf->center().z();
                    // get the euler angles         
                    auto ea = sf->transform().rotation().eulerAngles(0, 1, 2); 
                    double e0 = ea[0];
                    double e1 = ea[1]; 
                    double e2 = ea[2];
                    m_outputFile << "[" << cx << "," << cy << "," << cz << "],[" << e0 << "," << e1 << "," << e2 << "]";
                    if (is < nSurfaces) m_outputFile << ",";
                    ++is;
                }
                m_outputFile << "]}";
                m_firstLayerWritten = true;
            }
        }
    }
    return StatusCode::SUCCESS;    
}


StatusCode Trk::GeometryJsonDumper::processNode(const Trk::Surface& /*sf*/, size_t /*level*/) const
{
    ATH_MSG_VERBOSE("Dumping information for Surfaces.");
    
    return StatusCode::SUCCESS;    
}
