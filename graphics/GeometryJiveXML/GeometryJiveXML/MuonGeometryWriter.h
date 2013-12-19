/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MUONGEOMETRYWRITER
#define JIVEXML_MUONGEOMETRYWRITER

#include "JiveXML/IGeometryWriter.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Geometry/Point3D.h"

#include <string>
#include <fstream>

namespace MuonGM {
  class MuonDetectorManager;
  class MuonStation;
}

namespace JiveXML
{
  /**
   * Writes the GeoModel muon geometry to an XML file for use with Atlantis.
   */
  class MuonGeometryWriter : virtual public IGeometryWriter, public AthAlgTool {

  public:

    /**
     * Constructor.
     * \param detStore pointer to the detector store service
     * \param log log message stream 
     */
    MuonGeometryWriter(const std::string& t,const std::string& n,const IInterface* p):
      AthAlgTool(t,n,p){declareInterface<IGeometryWriter>(this);};

    /**
     * Writes the geometry of the ATLAS muon spectrometer to an XML file for use with Atlantis.
     */
    virtual StatusCode writeGeometry();

  private:

    /**
     * Writes the header of the XML file to a stream.
     * \param out stream where the XML fragment is written to
     */
    void writeHeader(std::ofstream &out) const ;

    /**
     * Retrieves all stations from GeoModel and writes the corresponding XML elements to a stream.
     * \param out stream where the XML fragment is written to
     */
    void writeStations(std::ofstream &out) const ;

    /**
     * Writes the footer of the XML file to a stream.
     * \param out stream where the XML fragment is written to
     */
    void writeFooter(std::ofstream &out) const ;

    /**
     * Returns the global position of the station, rotated to sector 1.
     * \param station under consideration
     * \param maxPhi maximum phi index for this station type
     * \return position of the station
     */
    HepGeom::Point3D<double> getPosition(const MuonGM::MuonStation *station, int maxPhi) const ;

    /**
     * Returns the rotation of the station with respect to the center of the sector.
     * \param pos position of the station rotated to sector 1.
     * \param maxPhi maximum phi index for this station type
     * \return rotation of the station
     */
    double getDeltaPhi(const HepGeom::Point3D<double> &pos, int maxPhi) const ;

    /**
     * Returns the shift of the station with respect to the center of the sector.
     * \param pos position of the station rotated to sector 1
     * \param dphi rotation of the station with respect to the center of the sector
     * \return shift of the station
     */
    double getShift(const HepGeom::Point3D<double> &pos, double dphi) const ;

    /**
     * Returns the forward tilt angle of a station (for CSCs).
     * \param trans local to global coordinates transformation matrix
     * \return forward tilt angle (radians)
     */
    double getAlpha(const HepGeom::Transform3D &trans) const ;

     /** Pointer to the muon detector manager (GeoModel) */
    const MuonGM::MuonDetectorManager* m_muon_manager ;

    /** Maximum deviation from the reference value before the station is considered different. */
    static const double m_smallAngle, m_smallDistance ;
  };
}
#endif
