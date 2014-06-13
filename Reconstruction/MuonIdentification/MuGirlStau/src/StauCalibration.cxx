/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauCalibration.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "PathResolver/PathResolver.h"

#include <ostream>
#include <fstream>
#include <string>

using namespace std;

MuGirlNS::StauCalibration::StauCalibration(const char* mdtCalibFile, const char* rpcCalibFile, const char* rpcNewCalibFile, const char* tileCalibFile, const char* phaseShiftFile)
{
	std::cout << "MDT file " << mdtCalibFile << std::endl;
	std::cout << "RPC file " << rpcCalibFile << std::endl;
	std::cout << "New RPC file " << rpcNewCalibFile << std::endl;
	std::cout << "Tile file " << tileCalibFile << std::endl;
	std::cout << "Phase shift file " << phaseShiftFile << std::endl;
	loadMdtStauCalibration(mdtCalibFile);
	loadRpcStauCalibration(rpcCalibFile);
	loadNewRpcStauCalibration(rpcNewCalibFile);
	loadTileStauCalibration(tileCalibFile);
	loadPhaseShiftMap(phaseShiftFile);
}

void MuGirlNS::StauCalibration::loadPhaseShiftMap(const char* shiftFile)
{
	ifstream phaseShiftFile(shiftFile);//, ios::in); 
	string line;
	getline(phaseShiftFile, line);

	while(!phaseShiftFile.eof())
	{
		int runNumber = 0;
		double phaseShift = 0;
		const char* cline = line.c_str();
		sscanf(cline,"%d %lf",&runNumber, &phaseShift);
		
		m_phaseShiftMap[runNumber] = phaseShift;
		getline(phaseShiftFile, line);
				
	}
	phaseShiftFile.close();
}

void MuGirlNS::StauCalibration::loadMdtStauCalibration(const char* calibFile)
{
	ifstream calibrationFile(calibFile);//, ios::in); 
	string line;
	getline(calibrationFile, line);

	while(!calibrationFile.eof())
	{
		unsigned int id = 0;
		double mean = 0;
		double sigma = 0;
		double chi2 = 0;
		int entries = 0;
		const char* cline = line.c_str();
		sscanf(cline,"%x %lf %lf %lf %d",&id, &mean, &sigma, &chi2, &entries);
		
		StauCalibrationParameters cp(mean,sigma);
		m_calibrationMdt[id] = cp;
		getline(calibrationFile, line);
				
	}
	calibrationFile.close();
}

void MuGirlNS::StauCalibration::loadRpcStauCalibration(const char* calibFile)
{ 
	ifstream calibrationFile(calibFile);//, ios::in); 
	string line;
	getline(calibrationFile, line);
	while(!calibrationFile.eof())
	{
		double min_r = 0;
		double max_r = 0;
		double mean_eta = 0;
		double sigma_eta = 0;
		double chi2_eta = 0;
		int entries_eta = 0;
                double mean_phi = 0;
                double sigma_phi = 0;
                double chi2_phi = 0;
                int entries_phi = 0;
		const char* cline = line.c_str();
		sscanf(cline,"%lf %lf %lf %lf %lf %d %lf %lf %lf %d",&min_r, &max_r, 
                      &mean_eta, &sigma_eta, &chi2_eta, &entries_eta, 
                      &mean_phi, &sigma_phi, &chi2_phi, &entries_phi);

		StauCalibrationParameters cp_eta(mean_eta,sigma_eta);
                StauCalibrationParameters cp_phi(mean_phi,sigma_phi);
		StauRpcCalibRange range_eta = rpcCalibRange(((double)(min_r+max_r))/2, true); // true => eta
                StauRpcCalibRange range_phi = rpcCalibRange(((double)(min_r+max_r))/2, false); // false => phi
		m_calibrationRpc[range_eta] = cp_eta;
                m_calibrationRpc[range_phi] = cp_phi; 
		getline(calibrationFile, line);
	}
	calibrationFile.close();
}
// new RPC calibrations

void MuGirlNS::StauCalibration::loadNewRpcStauCalibration(const char* calibFile)
{
        ifstream calibrationFile(calibFile);//, ios::in); 
        string line;
        getline(calibrationFile, line);
        while(!calibrationFile.eof())
        {
            unsigned int compact_id = 0;
            double mean = 0;
            int entries = 0;
            const char* cline = line.c_str();
            sscanf(cline,"%u %lf %d",&compact_id, &mean, &entries);
            StauCalibrationParameters cp(mean ,0);
            m_calibrationRpcNew[compact_id] = cp; 
            getline(calibrationFile, line);
        }
        calibrationFile.close();
}
///////////////////////////////
void MuGirlNS::StauCalibration::loadTileStauCalibration(const char* calibFile)
{
        ifstream calibrationFile(calibFile);//, ios::in); 
        string line;
        getline(calibrationFile, line);

        while(!calibrationFile.eof())
        {
                unsigned int id = 0;
                double mean = 0;
                double sigma = 0;
                double chi2 = 0;
                int entries = 0;
                const char* cline = line.c_str();
                sscanf(cline,"%x %lf %lf %lf %d",&id, &mean, &sigma, &chi2, &entries);

                StauCalibrationParameters cp(mean,sigma);
                m_calibrationTile[id] = cp;
                getline(calibrationFile, line);

        }
        calibrationFile.close();
}

 
MuGirlNS::StauCalibration::~StauCalibration()
{
}
