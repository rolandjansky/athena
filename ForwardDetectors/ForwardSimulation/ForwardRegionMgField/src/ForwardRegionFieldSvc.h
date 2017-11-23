/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ForwardRegionFieldSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FORWARDREGIONMGFIELD_FORWARDREGIONFIELDFIELDSVC_H
#define FORWARDREGIONMGFIELD_FORWARDREGIONFIELDFIELDSVC_H 1

//CLHEP includes
#include "CLHEP/Geometry/Point3D.h"

//Geant4 includes
#include "G4ThreeVector.hh"

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "ForwardRegionMgField/IForwardRegionProperties.h"
// MagField includes
#include "MagFieldInterfaces/IMagFieldSvc.h"

namespace MagField {

  class ForwardRegionFieldSvc : public IMagFieldSvc, virtual public IIncidentListener, virtual public AthService {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    /** Constructor with parameters */
    ForwardRegionFieldSvc(const std::string& name,ISvcLocator* svc);

    /** AthService interface methods */
    StatusCode initialize() override final;
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override final;

    /** IIncidentListener interface methods **/
    void handle(const Incident& runIncident) override final;

    /** IMagFieldSvc interface methods **/
    /** get B field value at given position */
    /** xyz[3] is in mm, bxyz[3] is in kT */
    /** if deriv[9] is given, field derivatives are returned in kT/mm */
    virtual void getField(const double *xyz, double *bxyz, double *deriv = 0) override final;

    /** get B field value on the z-r plane at given position */
    /** works only inside the solenoid; otherwise calls getField() above */
    /** xyz[3] is in mm, bxyz[3] is in kT */
    /** if deriv[9] is given, field derivatives are returned in kT/mm */
    virtual void getFieldZR(const double *xyz, double *bxyz, double *deriv = 0) override final;

    /** Non-inherited public methods FIXME - add new interface? */
    // get magnetic induction vector in certain point inside Q1-Q3 quadrupoles (uses bilinear interpolation), q=0 - Q1, q=1 - Q2, q=2 - Q3
    G4ThreeVector getMagInd(G4ThreeVector Point, int q, int beam) const;

    double getMag(int magnet, int beam) const;

    G4ThreeVector getKick(int beam) const;

    double getMagXOff(int magnet) const;

    double getMagYOff(int magnet) const;

  private:

    G4ThreeVector FieldValue(G4ThreeVector Point) const;

    int m_magnet;

    void InitMagData();

    std::vector<std::vector<std::string> > m_magnets;

    // array for storing magnets data
    // dipoles/quadrupoles: field/gradient beam 1 | field/gradient beam 2 | x offset | y offset
    // kickers: B_x beam 1 | B_x beam 2 | B_y beam 1 | B_y beam 2
    double m_magData[4];

    // switch for magnetic data input file type: 0 = magnets.dat, 1 = twiss file
    int m_magDataType;

    std::string m_MQXA_DataFile; // @TODO DOCUMENT
    std::string m_MQXB_DataFile; // @TODO DOCUMENT

    //std::string m_twissFileB1, m_twissFileB2;

     // file indexes - FIXME need to rename these to comply with naming conventions.
    static const int Q1 = 0;
    static const int Q2 = 1;
    static const int Q3 = 2;
    static const int D1 = 3;
    static const int D2 = 4;
    static const int Q4 = 5;
    static const int Q5 = 6;
    static const int Q6 = 7;
    static const int Q7 = 8;
    static const int Q1HKick = 9;
    static const int Q1VKick = 10;
    static const int Q2HKick = 11;
    static const int Q2VKick = 12;
    static const int Q3HKick = 13;
    static const int Q3VKick = 14;
    static const int Q4VKickA = 15;
    static const int Q4HKick = 16;
    static const int Q4VKickB = 17;
    static const int Q5HKick = 18;
    static const int Q6VKick = 19;
   //mapfile parameters
    static const int s_rowsMQXA = 97;
    static const int s_colsMQXA = 97;
    static const int s_rowsMQXB = 89;
    static const int s_colsMQXB = 89;

    double m_magIndMQXA[s_rowsMQXA][s_colsMQXA][2];
    double m_magIndMQXB[s_rowsMQXB][s_colsMQXB][2];

    // Load data from file into 2D array of strings
    std::vector<std::vector<std::string> > loadDataFile(const char * fileName, int cols);
    std::vector<std::vector<std::string> > loadDataFileNLines(const char * fileName, int cols, int N, std::string& header);

    // Calculate magnetic induction value / gradient from K*L values
    double kLToB(double kL, double lenght, double momentum);

    void InitMagDataFromTwiss(std::vector<std::vector<std::string> > loadedTwissFile, int beam, double momentum);

    // property configuration
    FWDMg_CONFIGURATION m_Config;

    ToolHandle<IForwardRegionProperties> m_properties;

//    G4ThreeVector applyDisplacement(G4ThreeVector Point, G4ThreeVector field, int beam) const;

    void writeOutTwiss(std::vector<std::vector<std::string> > loadedTwissFile, int beam, std::string header);
    void getMagnetTransformParams(int beam, int magnet, G4ThreeVector Point, HepGeom::Point3D<double> &pointMagStart,HepGeom::Point3D<double> &pointMagEnd, double &rotZ) const;
    int getMagNumFromName(std::string name) const;


  };
}
#endif // FORWARDREGIONMGFIELD_FORWARDREGIONFIELDFIELDSVC_H
