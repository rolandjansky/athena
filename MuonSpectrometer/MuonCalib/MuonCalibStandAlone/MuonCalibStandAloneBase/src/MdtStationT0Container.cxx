/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/MdtStationT0Container.h"

#include <TString.h>  // for Form

#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::
//:: METHOD t0 ::
//:::::::::::::::

double MdtStationT0Container::t0(const unsigned int& ml, const unsigned int& ly, const unsigned int& tb) const {
    if (!m_t0_loaded) return 9e9;
    return m_t0[ml - 1][ly - 1][tb - 1];
}

//*****************************************************************************

//:::::::::::::::
//:: METHOD adc ::
//:::::::::::::::

double MdtStationT0Container::adc(const unsigned int& ml, const unsigned int& ly, const unsigned int& tb) const {
    if (!m_t0_loaded) return 9e9;
    return m_adc[ml - 1][ly - 1][tb - 1];
}

//*****************************************************************************

//::::::::::::::::::
//:: METHOD Sett0 ::
//::::::::::::::::::

bool MdtStationT0Container::Sett0(const unsigned int& ml, const unsigned int& ly, const unsigned int& tb, double t0, double adc) {
    m_t0[ml - 1][ly - 1][tb - 1] = t0;
    m_adc[ml - 1][ly - 1][tb - 1] = adc;
    m_t0_loaded = true;

    return m_t0_loaded;
}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD readT0File ::
//:::::::::::::::::::::::

void MdtStationT0Container::readT0File(const std::string& file_name) {
    ///////////////
    // VARIABLES //
    ///////////////

    std::string sdummy;    // auxiliary string for file reading
    int idummy;            // auxiliary integer for file reading
    double dummy;          // auxiliary double for file reading
    int ml, ly, tb;        // multilayer, layer, tube
    std::ifstream infile;  // t0 file

    //////////////////
    // OPEN t0 file //
    //////////////////

    infile.open(file_name.c_str());
    if (infile.fail()) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nMdtStationT0Container::readT0File - Could not open file %s!", __FILE__, __LINE__, file_name.c_str()));
    }

    //////////////////////
    // READ THE t0 FILE //
    //////////////////////

    // ignore the first line //
    getline(infile, sdummy);

    // read the t0 look-up table //
    while (!infile.eof()) {
        // station name //
        infile >> sdummy;
        if (infile.eof()) { break; }

        // eta, phi, technology index //
        infile >> idummy >> idummy >> idummy;

        // multilayer, layer, tube //
        infile >> ml >> ly >> tb;

        // t0 //
        infile >> m_t0[ml - 1][ly - 1][tb - 1];
        infile >> m_adc[ml - 1][ly - 1][tb - 1];

        if (infile.fail()) return;

        // the rest of the line //
        for (unsigned k = 0; k < 10; k++) { infile >> dummy; }
    }
    m_t0_loaded = true;
    return;
}
