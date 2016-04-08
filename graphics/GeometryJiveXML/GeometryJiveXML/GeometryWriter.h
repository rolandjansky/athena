/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_GEOMETRYWRITER
#define JIVEXML_GEOMETRYWRITER

#include "CLHEP/Geometry/Point3D.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "JiveXML/IGeometryWriter.h"
#include <string>
#include <vector>
#include <fstream>

class CaloDetDescrManager;
class CaloDetDescriptor;
class Identifier;
class TileDetDescrManager;
class LArDetectorManager;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace InDetDD {
	class SiDetectorManager;
	class TRT_DetectorManager;
}

namespace JiveXML{

  /**
   * Writes the inner detector and calorimeter geometry to an XML file for use with Atlantis.
   */
  class GeometryWriter : virtual public IGeometryWriter, public AthAlgTool {
    
  public:
    
    /**
     * Constructor.
     * \param detStore pointer to the detector store service
     * \param log log message stream
     */
    GeometryWriter(const std::string& t,const std::string& n,const IInterface* p):
      AthAlgTool(t,n,p){declareInterface<IGeometryWriter>(this);};
    
    /**
     * Writes the inner detector and calorimeter geometry to an XML file.
     */
    virtual StatusCode writeGeometry();

    virtual ~GeometryWriter(){};
    
  private:
    
    /**
     * Writes the header of the XML file.
     * \param out stream where the XML fragment is written to
     */
    void writeHeader(std::ofstream &out);
    
    /**
     * Writes the geometry of the pixel detector.
     * \param out stream where the XML fragment is written to
     */
    void writePixelGeometry(std::ofstream &out);
    
    /**
     * Writes the geometry of the silicon detector.
     * \param out stream where the XML fragment is written to
     */
    void writeSCTGeometry(std::ofstream &out);
    
    /**
     * Writes the geometry of the TRT detector.
     * \param out stream where the XML fragment is written to
     */
    void writeTRTGeometry(std::ofstream &out);
    
    /**
     * Writes the (hardcoded) geometry of the solenoid. 
     * \param out stream where the XML fragment is written to
     */
    void writeSolenoidGeometry(std::ofstream &out);
    
    /**
     * Writes the geometry of the LAr calorimeters.
     * \param out stream where the XML fragment is written to
     */
    void writeLArGeometry(std::ofstream &out);
    
    /**
     * Writes the geometry of the Minimum Bias Trigger Scintillators.
     * \param out stream where the XML fragment is written to
     */
    void writeMBTSGeometry(std::ofstream &out);
    
    /**
     * Writes the geometry of the TILE calorimeters.
     * \param out stream where the XML fragment is written to
     */
    void writeTILEGeometry(std::ofstream &out);
    
    /**
     * Writes the footer of the XML file.
     * \param out stream where the XML fragment is written to
     */
    void writeFooter(std::ofstream &out);
    
    /**
     * Writes <ADisc> element to the XML file.
     * \param out stream where the element is written to
     * \param p projection attribute
     * \param c fill color from colormap
     * \param n detector/module name
     * \param rIn inner radius of the disc
     * \param rOut outer radius of the disc
     */
    void writeDiscElement(std::ofstream &out, std::string p, std::string c,
			  std::string n, double rIn, double rOut);
    
    /**
     * Writes <ARectangle> element to the XML file.
     * \param out stream where the element is written to
     * \param p projection attribute
     * \param c fill color from colormap
     * \param n detector/module name
     * \param xIn inner x coordinate
     * \param xOut outer x coordinate
     * \param yIn inner y coordinate
     * \param yOut outer y coordinate
     */
    void writeRectangleElement(std::ofstream &out, std::string p, std::string c, 
			       std::string n, double xIn, double xOut,
			       double yIn, double yOut);
    
    /**
     * Writes <AGeneralPath> element to the XML file. This element is basically a rectangle, but its 
     * (xOut,yIn) corner has been removed. (Used for SCT endcaps.)
     * \param out stream where the element is written to
     * \param p projection attribute
     * \param c fill color from colormap
     * \param n detector/module name
     * \param xIn inner x coordinate
     * \param xOut outer x coordinate
     * \param yIn inner y coordinate
     * \param yOut outer y coordinate
     */
    void writeRectanglishElement(std::ofstream &out, std::string p, std::string c,
				 std::string n, double xIn, double xOut,
				 double yIn, double yOut);
    
    
    /** Pixel detector manager and ID helper */
    const InDetDD::SiDetectorManager* m_pixel_manager;
    const PixelID * m_pixelIdHelper;   

    /** Silicon detector manager and ID helper */
    const InDetDD::SiDetectorManager* m_silicon_manager;
    const SCT_ID * m_sctIdHelper;
    
    /** TRT detector manager and ID helper */
    const InDetDD::TRT_DetectorManager* m_trt_manager;
    const TRT_ID * m_trtIdHelper;
    
    /** Calorimeter detector manager. */
    const CaloDetDescrManager* m_calo_manager;
    const TileDetDescrManager* m_tile_manager;
    const LArDetectorManager *m_lar_manager;
    
    /** The number of calorimeter sampling types. */
    static const int m_numCaloTypes = 24;
    
    std::vector<double> m_larMin;
    std::vector<double> m_larMax;
  };
}
#endif
