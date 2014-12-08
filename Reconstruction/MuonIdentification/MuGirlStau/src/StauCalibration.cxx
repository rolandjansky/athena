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
        const char* mdtCalibFile,
        const char* rpcCalibFile,
        const char* caloCalibFile)
{
    m_log << MSG::INFO << "StauCalibration::initialize() - entered" << endreq;
    loadMdtStauCalibration(mdtCalibFile);
    loadRpcStauCalibration(rpcCalibFile);
    loadTileStauCalibration(caloCalibFile);
    m_log << MSG::INFO << "StauCalibration::initialize() - done" << endreq;
}

void MuGirlNS::StauCalibration::loadMdtStauCalibration(const char* calibFile)
{
    m_log << MSG::INFO << "StauCalibration::loadMdtStauCalibration() - loading "  << calibFile << endreq;
    ifstream calibrationFile(calibFile); //, ios::in);
    string line;
    int num_records = 0;
    getline(calibrationFile, line);
    while (!calibrationFile.eof())
    {
        unsigned int id = 0;
        double mean = 0, sigma = 0;
        const char* cline = line.c_str();
        sscanf(cline, "%x %lf %lf", &id, &mean, &sigma);
        StauCalibrationParameters cp(mean, sigma);
        m_calibrationMdt[id] = cp;
        num_records++;
        getline(calibrationFile, line);
    }
    calibrationFile.close();
    m_log << MSG::DEBUG << "StauCalibration::loadMdtStauCalibration() - loaded " << num_records << " records" << endreq;
}

// new RPC calibrations
void MuGirlNS::StauCalibration::loadRpcStauCalibration(const char* calibFile)
{
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
        sscanf(cline, "%u %lf %lf", &compact_id, &mean, &sigma);
        StauCalibrationParameters cp(mean, sigma);
        m_calibrationRpc[compact_id] = cp;
        num_records++;
        getline(calibrationFile, line);
    }
    calibrationFile.close();
    m_log << MSG::DEBUG << "StauCalibration::loadRpcStauCalibration() - loaded " << num_records << " records" << endreq;
}

///////////////////////////////
void MuGirlNS::StauCalibration::loadTileStauCalibration(const char* calibFile)
{
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
        sscanf(cline, "%d %df %lf %lf", &layer, &bin, &mean, &sigma);

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
