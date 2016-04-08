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

void MuGirlNS::StauCalibration::initialize(
					   std::string &mdtCalibFileName,
					   std::string &rpcCalibFileName,
					   std::string &caloCalibFileName)
{
    m_log << MSG::INFO << "StauCalibration::initialize() - entered" << endreq;
    loadMdtStauCalibration(mdtCalibFileName);
    loadRpcStauCalibration(rpcCalibFileName);
    loadTileStauCalibration(caloCalibFileName);
    m_log << MSG::INFO << "StauCalibration::initialize() - done" << endreq;
}

void MuGirlNS::StauCalibration::loadMdtStauCalibration(std::string &calibFileName)
{
  std::string calibFile = PathResolver::find_file (calibFileName, "DATAPATH");
 
    m_log << MSG::INFO << "StauCalibration::loadMdtStauCalibration() - loading "  << calibFileName << endreq;
    ifstream calibrationFile(calibFile); //, ios::in);
    string line;
    int num_records = 0;
    getline(calibrationFile, line);
    while (!calibrationFile.eof())
    {
        unsigned int id = 0;
        double mean = 0, sigma = 0;
        const char* cline = line.c_str();
        sscanf(cline, "%80x %80lf %80lf", &id, &mean, &sigma);
	m_log<<MSG::DEBUG<<"MDT "<<id<<"  "<<mean<<"  "<<sigma<<endreq;
        StauCalibrationParameters cp(mean, sigma);
        m_calibrationMdt[id] = cp;
        num_records++;
        getline(calibrationFile, line);
    }
    calibrationFile.close();
    m_log << MSG::DEBUG << "StauCalibration::loadMdtStauCalibration() - loaded " << num_records << " records" << endreq;
}

// new RPC calibrations
void MuGirlNS::StauCalibration::loadRpcStauCalibration(std::string &calibFileName)
{
  std::string calibFile = PathResolver::find_file (calibFileName, "DATAPATH");

    m_log << MSG::INFO << "StauCalibration::loadRpcStauCalibration() - loading "  << calibFile << endreq;
    ifstream calibrationFile(calibFile); //, ios::in);
    string line;
    int num_records = 0;
    getline(calibrationFile, line);
    while (!calibrationFile.eof())
    {
        unsigned int compact_id = 0;
        double mean = 0, sigma = 0;
        const char* cline = line.c_str();
        sscanf(cline, "%80u %80lf %80lf", &compact_id, &mean, &sigma);
	m_log<<MSG::DEBUG<<"RPC "<<compact_id<<"  "<<mean<<"  "<<sigma<<endreq;
        StauCalibrationParameters cp(mean, sigma);
        m_calibrationRpc[compact_id] = cp;
        num_records++;
        getline(calibrationFile, line);
    }
    calibrationFile.close();
    m_log << MSG::DEBUG << "StauCalibration::loadRpcStauCalibration() - loaded " << num_records << " records" << endreq;
}

///////////////////////////////
void MuGirlNS::StauCalibration::loadTileStauCalibration(std::string &calibFileName)
{
  std::string calibFile = PathResolver::find_file (calibFileName, "DATAPATH");

    m_log << MSG::INFO << "StauCalibration::loadTileStauCalibration() - loading "  << calibFile << endreq;
    ifstream calibrationFile(calibFile); //, ios::in);
    string line;
    int num_records = 0;
    getline(calibrationFile, line);
    while (!calibrationFile.eof())
    {
        int layer = 0, bin = 0;
        double mean = 0, sigma = 0;
        const char* cline = line.c_str();
        sscanf(cline, "%80d %80df %80lf %80lf", &layer, &bin, &mean, &sigma);
	m_log<<MSG::DEBUG<<"CALO "<<layer<<"  "<<bin<<"  "<<mean<<"  "<<sigma<<endreq;
        StauCalibrationParameters cp(mean, sigma);
        m_calibrationTile[std::make_pair(layer, bin)] = cp;
        num_records++;
        getline(calibrationFile, line);
    }
    calibrationFile.close();
    m_log << MSG::DEBUG << "StauCalibration::loadTileStauCalibration() - loaded " << num_records << " records" << endreq;
}

MuGirlNS::StauCalibration::~StauCalibration()
{
}
