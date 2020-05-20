/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "BCMPrime/BCMPrimeXMLHelper.h"
#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

#include "CLHEP/Evaluator/Evaluator.h"

BCMPrimeXMLHelper::BCMPrimeXMLHelper(int module, const PixelGeoBuilderBasics *basics) : GeoXMLUtils(), PixelGeoBuilder(basics), m_module(module)
{

    std::string fileName = "BcmPrime.xml";
    if (const char *env_p = std::getenv("PIXEL_BCMPRIME_GEO_XML"))
        fileName = std::string(env_p);

    bool readXMLfromDB = getBasics()->ReadInputDataFromDB();
    bool bParsed = false;
    if (readXMLfromDB)
    {
        basics->msgStream() << MSG::DEBUG << "XML input : DB CLOB " << fileName << "  (DB flag : " << readXMLfromDB << ")" << endreq;
        DBXMLUtils dbUtils(getBasics());
        std::string XMLtext = dbUtils.readXMLFromDB(fileName);
        InitializeXML();
        bParsed = ParseBuffer(XMLtext, std::string(""));
    }
    else
    {
        basics->msgStream() << MSG::DEBUG << "XML input : from file " << fileName << "  (DB flag : " << readXMLfromDB << ")" << endreq;
        std::string file = PathResolver::find_file(fileName, "DATAPATH");
        InitializeXML();
        bParsed = ParseFile(file);
    }

    // No XML file was parsed
    if (!bParsed)
    {
        basics->msgStream() << MSG::ERROR << "XML file " << fileName << " not found" << endreq;
        return;
    }

    m_moduleIndices = getChildValue_Indices("BCMPrimeGeo", "Module", m_module);
}

BCMPrimeXMLHelper::~BCMPrimeXMLHelper()
{
    TerminateXML();
}

int BCMPrimeXMLHelper::getECRingNumber() const
{
    std::string ecring_number = getString("BCMPrimeGeo", m_moduleIndices, "ECRingNumber");
    return atoi(ecring_number.c_str());
}

int BCMPrimeXMLHelper::getNumberOfModules() const
{
    std::string number_of_modules = getString("BCMPrimeGeo", m_moduleIndices, "NumberOfModules");
    return atoi(number_of_modules.c_str());
}

double BCMPrimeXMLHelper::getTransX() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "TransX");
}

double BCMPrimeXMLHelper::getTransY() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "TransY");
}

double BCMPrimeXMLHelper::getRotX() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "RotX");
}

double BCMPrimeXMLHelper::getRotY() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "RotY");
}

double BCMPrimeXMLHelper::getRotZ() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "RotZ");
}

double BCMPrimeXMLHelper::getModuleLength() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "ModuleLength");
}

double BCMPrimeXMLHelper::getModuleWidth() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "ModuleWidth");
}

double BCMPrimeXMLHelper::getModuleTilt() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "ModuleTilt");
}

double BCMPrimeXMLHelper::getDiamSize() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "DiamSize");
}

double BCMPrimeXMLHelper::getDiamOffsetY() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "DiamOffsetY");
}

double BCMPrimeXMLHelper::getDiamSeparZ() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "DiamSeparZ");
}

double BCMPrimeXMLHelper::getRingRot() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "RingRot");
}

double BCMPrimeXMLHelper::getRingOffset() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "RingOffset");
}

double BCMPrimeXMLHelper::getCuThickness() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "CuThickness");
}

double BCMPrimeXMLHelper::getG10Thickness() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "G10Thickness");
}

double BCMPrimeXMLHelper::getDiamThickness() const
{
    return getDouble("BCMPrimeGeo", m_moduleIndices, "DiamThickness");
}
