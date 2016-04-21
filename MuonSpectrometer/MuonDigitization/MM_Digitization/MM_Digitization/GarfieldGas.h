/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GarfieldGas_H
#define GarfieldGas_H

// calculate drift velocity based on Garfield's Gas parameter file
// codes mostly come from Garfield++
// http://garfieldpp.web.cern.ch/garfieldpp/
//

#include <iostream>
#include <fstream>

#include <iomanip>

#include <cmath>

#include <string>
#include <vector>

#include <stdlib.h>
#include <string.h>



namespace Numerics {

double
Divdif(const std::vector<double>& f, const std::vector<double>& a, 
       int nn, double x, int mm);

bool
Boxin3(std::vector<std::vector<std::vector<double> > >& value, 
       std::vector<double>& xAxis, 
       std::vector<double>& yAxis, 
       std::vector<double>& zAxis, 
       int nx, int ny, int nz, 
       double xx, double yy, double zz, double& f, int iOrder);
  
}

class GarfieldGas {
  public:
    GarfieldGas();
    virtual ~GarfieldGas(){}

    // from Medium.hh
    double ScaleElectricField(const double e) {
      return e * pressureTable / pressure;
    }

    virtual 
    bool ElectronVelocity(const double ex, const double ey, const double ez, 
                          const double bx, const double by, const double bz, 
                          double& vx, double& vy, double& vz);


    // from MediumGas.hh
    bool LoadGasFile(const std::string filename); 

    bool GetGasName(const int gasnumber, const int version,
		    std::string& gasname);

    void PrintGas();


  protected:

    // from Medium.hh

    // Name
    std::string name;
    // Temperature [K]
    double temperature;
    // Pressure [Torr]
    double pressure;
    // Number of components
    int nComponents;

    int nExcListElements;
    int nIonListElements;

    int nEfields;
    int nBfields;
    int nAngles;

    std::vector<double> eFields;
    std::vector<double> bFields;
    std::vector<double> bAngles;

    bool map2d;

    bool hasElectronVelocityE, hasElectronVelocityB, hasElectronVelocityExB;
    bool hasElectronDiffLong, hasElectronDiffTrans;
    std::vector<std::vector<std::vector<double> > > tabElectronVelocityE;
    std::vector<std::vector<std::vector<double> > > tabElectronVelocityExB;
    std::vector<std::vector<std::vector<double> > > tabElectronVelocityB;
    std::vector<std::vector<std::vector<double> > > tabElectronDiffLong;
    std::vector<std::vector<std::vector<double> > > tabElectronDiffTrans;


    // Extrapolation methods
    int extrLowVelocity, extrHighVelocity;
    int extrLowDiffusion, extrHighDiffusion;
    int extrLowTownsend, extrHighTownsend;
    int extrLowAttachment, extrHighAttachment;
    int extrLowMobility, extrHighMobility;
    int extrLowDissociation, extrHighDissociation;

    // Interpolation methods
    int intpVelocity;
    int intpDiffusion;
    int intpTownsend;
    int intpAttachment;
    int intpMobility;
    int intpDissociation;

    double Interpolate1D(const double e,
			 const std::vector<double>& table, 
			 const std::vector<double>& fields,
			 const int intpMeth, 
			 const int jExtr, const int iExtr);

    void InitParamArrays(const int eRes, const int bRes, const int aRes,
			 std::vector<std::vector<std::vector<double> > >& tab,
			 const double val);


    // from MediumGas.hh
    static const int nMaxGases=6;

    std::string gas[nMaxGases];
    double fraction[nMaxGases];


    double pressureTable, temperatureTable;



};

#endif
