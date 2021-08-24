/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MUONGEOMETRYWRITER
#define JIVEXML_MUONGEOMETRYWRITER

#include "JiveXML/IGeometryWriter.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"

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

    /** process the geometry of New Small Wheel */
    void processNSW(std::ofstream &out);

    /**
     * Writes the footer of the XML file to a stream.
     * \param out stream where the XML fragment is written to
     */
    void writeFooter(std::ofstream &out) const ;

    /**
     * Writes a trapezoid station in XML to a stream.
     * \param out stream where the XML fragment is written to
    */
    void writeATrd(std::ofstream &out, std::string stationTech, std::string stationName, double zi, double zo, double ri, double ro, double wi, double wo, int eta, std::string phiString, double dphi, double shift, double alpha) const ;


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

    /**
     * Returns the global position of the NSW station, rotated to sector 1.
     * \param position of the station under consideration
     * \param maxPhi maximum phi index for this station type
     * \return position of the station after rotation
     */
    HepGeom::Point3D<double> getPositionNSW(Amg::Vector3D pos, int maxPhi) const ;

    /**
     * Returns phi index of the sector
     * \param phi phi of the sector
     * \param maxPhi maximum phi index for this station type
     * \return phi index of the sector
     */
    int getPhiIndex(double phi, int maxPhi) const ;

    /**
     * Reads the geometry parameters of a NSW Micromegas chamber
     * \param pv pointer to the detector volume considered
     * \param maxPhi maximum number of phi sectors (=8 for NSW)
     * other parameters are used to store the retrieved parameters
     */
    void readNSWMMPars(const GeoVolumeCursor *pv, int maxPhi, std::string& chamberName, HepGeom::Point3D<double>& pos_rot,
          double& zi, double& zo, double& ri, double& ro, double& wi, double& wo, double& dphi, double& shift, int& phiIndex) const ;

    /**
     * Reads the geometry parameters of a NSW sTGC chamber.
     * \param pv pointer to the detector volume considered
     * \param maxPhi maximum number of phi sectors (=8 for NSW)
     * other parameters are used to store the retrieved parameters
     */
    void readNSWSTGCPars(const GeoVolumeCursor *pv, int maxPhi,
          std::string& chamberName, HepGeom::Point3D<double>& pos_rot, const GeoSimplePolygonBrep*& theBrep,
          int& nvtx, double& dz, double& dphi, double& shift, int& phiIndex) const ;

    /**
     * Takes four vetecies of a GeoSimplePolygonBrep to form a trapezoid shape and reads the parameters of the trapezoid shape.
     * \param theBrep pointer to the GeoSimplePolygonBrep
     * \param rho radius of the center of the GeoSimplePolygonBrep for calculating the inner and outer radius (ri and ro)
     * \param vtx a list of the indecies of the four verticies of GeoSimplePolygonBrep to be used in drawing the trapezoid
     * other parameters are used to store the retrieved parameters
     */
    void readBrepAsATrd(const GeoSimplePolygonBrep* theBrep, double rho, const int* vtx, double& ri, double& ro, double& wi, double& wo) const ;

    /**
     * Compares two angles
     * \return true if the difference is within a precision defined by m_smallAngle
     */
    bool equalAngle(double a, double b) const ;

    /**
     * Compares two coordinates or lenghts
     * \return true if the difference is within a precision defined by m_smallDistance
     */
    bool equalLength(double a, double b) const ;

     /** Pointer to the muon detector manager (GeoModel) */
    const MuonGM::MuonDetectorManager* m_muon_manager ;

    /** Maximum deviation from the reference value before the station is considered different. */
    static const double m_smallAngle, m_smallDistance ;
  };
}
#endif
