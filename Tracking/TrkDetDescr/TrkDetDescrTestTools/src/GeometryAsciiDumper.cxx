/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeometryAsciiDumper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg includes
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
// Trk include
#include "TrkDetDescrTestTools/GeometryAsciiDumper.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
// Root
#include "TTree.h"
#include "TString.h"
#include <sstream>

// Gaudi
#include "GaudiKernel/ITHistSvc.h"


// constructor
Trk::GeometryAsciiDumper::GeometryAsciiDumper(const std::string& t, const std::string& n, const IInterface* p) : 
  Trk::RecursiveGeometryProcessor(t,n,p),
  m_outputFileName("TrackingGeometryAscii.txt"),
  m_outputPrecision(6)
{
    declareProperty("OutputFileName",  m_outputFileName);
    declareProperty("OutputPrecision", m_outputPrecision);
}

// destructor
Trk::GeometryAsciiDumper::~GeometryAsciiDumper()
{}


StatusCode Trk::GeometryAsciiDumper::initialize()
{
    // open the file for writing
    m_outputFile.open(m_outputFileName.c_str());
    m_outputFile << " ============================= TrackingGeometry ASCII file (BOF) =============================" << std::endl;
    // return the base::initialize() state
    return Trk::RecursiveGeometryProcessor::initialize();    
}

StatusCode Trk::GeometryAsciiDumper::finalize()
{
    // close the file
    m_outputFile << " ============================= TrackingGeometry ASCII file (EOF) =============================" << std::endl;
    m_outputFile.close();
    // return the base::finalize() state    
    return Trk::RecursiveGeometryProcessor::finalize();
}



StatusCode Trk::GeometryAsciiDumper::processNode(const Trk::TrackingVolume& tvol, size_t level) const
{

   ATH_MSG_VERBOSE("Dumping information for TrackingVolume.");
   std::stringstream levelBuffer;
   for (size_t il = 0; il < level; ++il) levelBuffer << "  ";

   m_outputFile << levelBuffer.str() << "Trk::TrackingVolume  '" << tvol.volumeName() << "'"                              << std::endl;
   std::string rotationOffset = levelBuffer.str() +"                      ";
   m_outputFile << levelBuffer.str() << "  - transform       : " << Amg::toString( tvol.transform(), 
                                                                                   m_outputPrecision, rotationOffset )   << std::endl;
   m_outputFile << levelBuffer.str() << "  - listing Trk::BoundarySurface objects :" << std::endl;
   const std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >& bSurfaces = tvol.boundarySurfaces();
   std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >::const_iterator bSurfIter  = bSurfaces.begin();
   std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::TrackingVolume> > >::const_iterator bSurfIterE = bSurfaces.end();
   for (size_t ib = 0 ; bSurfIter != bSurfIterE; ++bSurfIter, ++ib ){
         if ( processNode((**bSurfIter).surfaceRepresentation(), level+4).isFailure() )
         ATH_MSG_WARNING("Problem to write out the boundary surfaces of TrackingVolume '" << tvol.volumeName() << "'." );
   }

  // Process the contained layers if they exist
  const Trk::LayerArray* layerArray = tvol.confinedLayers();
  if (layerArray) {
      m_outputFile << levelBuffer.str() << "  - contains " << layerArray->arrayObjects().size() << " confined Trk::Layer objects." << std::endl;
      m_outputFile << levelBuffer.str() << "  - listing Trk::Layer objects :" << std::endl;
    }

   // Process the contained TrackingVolumes (recursively) if they exist
   const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol.confinedVolumes();
   if (confinedVolumes){ 
       m_outputFile << levelBuffer.str() << "  - contains " << confinedVolumes->arrayObjects().size() << " confined Trk::TrackingVolume objects." << std::endl;
       m_outputFile << levelBuffer.str() << "  - stepping down in hierarchy to Trk::TrackingVolume objects :" << std::endl;
   }

   return StatusCode::SUCCESS;
}


StatusCode Trk::GeometryAsciiDumper::processNode(const Trk::Layer& lay, size_t level) const
{
    
    ATH_MSG_VERBOSE("Dumping information for Layer.");
    std::stringstream levelBuffer;
    for (size_t il = 0; il < level; ++il) levelBuffer << " ";
    
    size_t layerIdx = lay.layerIndex().value();
    if (layerIdx)  {   
        m_outputFile << levelBuffer.str() << "    Trk::Layer with LayerIndex " << layerIdx << std::endl;
        m_outputFile << levelBuffer.str() << "      - writing surface representation : "   << std::endl;
        if (processNode(lay.surfaceRepresentation(),level).isFailure())
            ATH_MSG_VERBOSE("Problems to write out surface representation of Trk::Layer with LayerIndex" << layerIdx);
        const Trk::SurfaceArray* surfArray = lay.surfaceArray();
        if (surfArray) {
            m_outputFile << levelBuffer.str() << "      - contains " << surfArray->arrayObjects().size() << " confined Trk::Surface objects." << std::endl;
            m_outputFile << levelBuffer.str() << "      - listing Trk::Surface objects :" << std::endl;
        }
    } else 
        m_outputFile << levelBuffer.str() << "    Trk::NavigationLayer - no geometrical information." << std::endl;


    return StatusCode::SUCCESS;    
}


StatusCode Trk::GeometryAsciiDumper::processNode(const Trk::Surface& sf, size_t level) const
{
    ATH_MSG_VERBOSE("Dumping information for Surfaces.");
    std::stringstream levelBuffer;
    for (size_t il = 0; il < level; ++il) levelBuffer << " ";

    m_outputFile << levelBuffer.str() << "        Trk::Surface object of type " << sf.type()                               << std::endl;
    std::string rotationOffset = levelBuffer.str() +"                              ";
    m_outputFile << levelBuffer.str() << "          - transform       : " << Amg::toString( sf.transform(), 
                                                                                    m_outputPrecision, rotationOffset )    << std::endl;
    return StatusCode::SUCCESS;    
}





