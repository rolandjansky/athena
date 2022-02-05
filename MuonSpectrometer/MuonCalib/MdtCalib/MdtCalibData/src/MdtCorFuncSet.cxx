/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/MdtCorFuncSet.h"

#include "MdtCalibData/IMdtBFieldCorFunc.h"
#include "MdtCalibData/IMdtBackgroundCorFunc.h"
#include "MdtCalibData/IMdtSlewCorFunc.h"
#include "MdtCalibData/IMdtTempCorFunc.h"
#include "MdtCalibData/IMdtWireSagCorFunc.h"

namespace MuonCalib {

    MdtCorFuncSet::MdtCorFuncSet(IMdtSlewCorFunc* s, IMdtBFieldCorFunc* bf, IMdtTempCorFunc* t, IMdtBackgroundCorFunc* bg,
                                 IMdtWireSagCorFunc* w) :
        m_slewing(s), m_bField(bf), m_temperature(t), m_background(bg), m_wireSag(w) {}

    MdtCorFuncSet::MdtCorFuncSet() :
        m_slewing(nullptr), m_bField(nullptr), m_temperature(nullptr), m_background(nullptr), m_wireSag(nullptr) {}

    MdtCorFuncSet::~MdtCorFuncSet() {
        delete m_slewing;
        delete m_bField;
        delete m_temperature;
        delete m_background;
        delete m_wireSag;
    }

    // MdtCorFuncSet& MdtCorFuncSet::operator=(const MdtCorFuncSet&) {return *this;}
    // MdtCorFuncSet::MdtCorFuncSet(const MdtCorFuncSet&) {}

    void MdtCorFuncSet::setSlewing(IMdtSlewCorFunc* slew) {
        if (m_slewing) delete m_slewing;
        m_slewing = slew;
    }
    void MdtCorFuncSet::setBField(IMdtBFieldCorFunc* bField) {
        if (m_bField) delete m_bField;
        m_bField = bField;
    }
    void MdtCorFuncSet::setTemperature(IMdtTempCorFunc* temperature) {
        if (m_temperature) delete m_temperature;
        m_temperature = temperature;
    }
    void MdtCorFuncSet::background(IMdtBackgroundCorFunc* background) {
        if (m_background) delete m_background;
        m_background = background;
    }
    void MdtCorFuncSet::wireSag(IMdtWireSagCorFunc* wireSag) {
        if (m_wireSag) delete m_wireSag;
        m_wireSag = wireSag;
    }

}  // namespace MuonCalib
