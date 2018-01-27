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
      return e * m_pressureTable / m_pressure;
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
    std::string m_name;
    // Temperature [K]
    double m_temperature;
    // Pressure [Torr]
    double m_pressure;
    // Number of components
    int m_nComponents;

    int m_nExcListElements;
    int m_nIonListElements;

    int m_nEfields;
    int m_nBfields;
    int m_nAngles;

    std::vector<double> m_eFields;
    std::vector<double> m_bFields;
    std::vector<double> m_bAngles;

    bool m_map2d;

    bool m_hasElectronVelocityE, m_hasElectronVelocityB, m_hasElectronVelocityExB;
    bool m_hasElectronDiffLong, m_hasElectronDiffTrans;
    std::vector<std::vector<std::vector<double> > > m_tabElectronVelocityE;
    std::vector<std::vector<std::vector<double> > > m_tabElectronVelocityExB;
    std::vector<std::vector<std::vector<double> > > m_tabElectronVelocityB;
    std::vector<std::vector<std::vector<double> > > m_tabElectronDiffLong;
    std::vector<std::vector<std::vector<double> > > m_tabElectronDiffTrans;


    // Extrapolation methods
    int m_extrLowVelocity, m_extrHighVelocity;
    int m_extrLowDiffusion, m_extrHighDiffusion;
    int m_extrLowTownsend, m_extrHighTownsend;
    int m_extrLowAttachment, m_extrHighAttachment;
    int m_extrLowMobility, m_extrHighMobility;
    int m_extrLowDissociation, m_extrHighDissociation;

    // Interpolation methods
    int m_intpVelocity;
    int m_intpDiffusion;
    int m_intpTownsend;
    int m_intpAttachment;
    int m_intpMobility;
    int m_intpDissociation;

    double Interpolate1D(const double e,
			 const std::vector<double>& table, 
			 const std::vector<double>& fields,
			 const int intpMeth, 
			 const int jExtr, const int iExtr);

    void InitParamArrays(const int eRes, const int bRes, const int aRes,
			 std::vector<std::vector<std::vector<double> > >& tab,
			 const double val);


    // from MediumGas.hh
    static const int s_nMaxGases=6;

    std::string m_gas[s_nMaxGases];
    double m_fraction[s_nMaxGases];


    double m_pressureTable, m_temperatureTable;



};

#endif
