/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardRegionMgField_h
#define ForwardRegionMgField_h 1

#include "FadsField/MagneticFieldMap.h"

//#include "IForwardRegionProperties/IForwardRegionProperties.h"
#include "ForwardRegionMgField/IForwardRegionProperties.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>

#include <string>
#include <vector>

//typedef G4ThreeVector (*PBF) (G4ThreeVector);

class ForwardRegionMgField : public FADS::MagneticFieldMap, virtual public IIncidentListener
{
public:
    ForwardRegionMgField(std::string, int);

    virtual void Initialize();

    void handle(const Incident& runIncident);

    long unsigned int addRef(){
        return ++m_refCounter;
    }

    long unsigned int release(){
        return --m_refCounter;
    }

    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    G4ThreeVector FieldValue(G4ThreeVector Point) const;
    
    void FieldValue(const double*, double*) const;

    // get magnetic induction vector in certain point inside Q1-Q3 quadrupoles (uses bilinear interpolation), q=0 - Q1, q=1 - Q2, q=2 - Q3
    G4ThreeVector getMagInd(G4ThreeVector Point, int q, int beam) const;

    double getMag(int magnet, int beam) const
    {
        if(m_magDataType == 1) return m_magData[beam-1];
        return atof(m_magnets[magnet][beam].c_str());
    }

    G4ThreeVector getKick(int beam) const
    {
        if(m_magDataType == 1) return G4ThreeVector(m_magData[beam-1]*CLHEP::tesla,m_magData[beam+1]*CLHEP::tesla,0);
        return G4ThreeVector(0,0,0); // magnets.dat does not contain kickers
    }

    double getMagXOff(int magnet) const
    {
        if(m_magDataType == 1) return m_magData[2];
        return atof(m_magnets[magnet][3].c_str())*CLHEP::mm;
    }

    double getMagYOff(int magnet) const
    {
        if(m_magDataType == 1) return m_magData[3];
        return atof(m_magnets[magnet][4].c_str())*CLHEP::mm;
    }

    // file indexes
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


private:
    int m_magnet;

    int m_refCounter;

    void InitMagData();

    std::string m_name;
    std::vector<std::vector<std::string> > m_magnets;

    // array for storing magnets data
    // dipoles/quadrupoles: field/gradient beam 1 | field/gradient beam 2 | x offset | y offset
    // kickers: B_x beam 1 | B_x beam 2 | B_y beam 1 | B_y beam 2
    double m_magData[4];

    // switch for magnetic data input file type: 0 = magnets.dat, 1 = twiss file
    int m_magDataType;

    //std::string m_twissFileB1, m_twissFileB2;

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
    int getMagNumFromName(std::string name);
};

#endif
