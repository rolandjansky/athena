// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


/**
 * @file HTTPlaneMap.h
 * @author Riley Xu - rixu@cern.ch (rewrite from FTK)
 * @date Janurary 7th, 2020
 * @brief See header.
 */


#include "TrigHTTMaps/HTTPlaneMap.h"

#include <AsgMessaging/MessageCheck.h>
#include <numeric>
#include <sstream>
#include <utility>

using namespace asg::msgUserCode;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Constructor/Desctructor
///////////////////////////////////////////////////////////////////////////////


HTTPlaneMap::HTTPlaneMap(const std::string & filepath, unsigned region, unsigned stage, std::vector<int> layerOverrides) :
    m_map(static_cast<int>(SiliconTech::nTechs),
          vector<vector<LayerSection>>(static_cast<int>(DetectorZone::nZones))
    ),
    m_layerOverrides(std::move(layerOverrides))
{
    ANA_MSG_INFO("Reading " << filepath);
    ifstream fin(filepath);
    if (!fin.is_open())
    {
        ANA_MSG_FATAL("Couldn't open " << filepath);
        throw ("HTTPlaneMap Couldn't open " + filepath);
    }

    // Reads the header of the file to resize all the vector members
    allocateMap(fin, stage);

    // Seek to the correct region
    seek(fin, region);

    // Reads the rest of the file to populate all the member vectors
    readLayers(fin, stage);

    // Postprocessing on coordinate indices
    for (uint32_t l = 0; l < m_nLogiLayers; l++)
    {
        m_coordOffset[l] = m_nCoords;
        m_nCoords += m_dimension[l];
        for (uint32_t i = 0; i < m_dimension[l]; i++)
            m_coordLayer.push_back(l);
    }

    ANA_MSG_INFO("Using " << m_nLogiLayers << " logical layers and " << m_nCoords << " coordinates");
}


// Reads the header of the file to resize all the vector members
void HTTPlaneMap::allocateMap(ifstream & fin, uint32_t stage)
{
    // Initialize read variables
    vector<int> layerCounts((int)SiliconTech::nTechs * static_cast<int>(DetectorZone::nZones)); // pixel_barrel, pixel_EC, SCT_barrel, SCT_EC
    string line, silicon, detReg, layerKey, geoKey;
    bool ok = true;

    // Read Geometry Version
    ok = ok && getline(fin, line);
    ANA_MSG_VERBOSE(line);
    istringstream sline(line);
    ok = ok && (sline >> geoKey);
    if (geoKey == "ATLAS-P2-ITK-22-02-00") m_diskIndex = {0,17,47,58,66};
    else if (geoKey == "ATLAS-P2-ITK-23-00-01") m_diskIndex = {0,15,44,50,61,69,77,86};
    else ANA_MSG_ERROR("Bad geometry version " << geoKey);

    ANA_MSG_INFO("Allocating map for geometry " << geoKey <<" diskIndex size="<<m_diskIndex.size());


    // Read number of logical layers
    if (stage == 1)
    {
        ok = ok && getline(fin, line);
        ANA_MSG_VERBOSE(line);
        istringstream sline(line);
        ok = ok && (sline >> m_nLogiLayers >> layerKey);
        ok = ok && (layerKey == "logical_s1");
        ok = ok && getline(fin, line); // skip the 2nd stage line
    }
    else if (stage == 2)
    {
        ok = ok && getline(fin, line); // skip the 1st stage line
        ok = ok && getline(fin, line);
        ANA_MSG_VERBOSE(line);
        istringstream sline(line);
        ok = ok && (sline >> m_nLogiLayers >> layerKey);
        ok = ok && (layerKey == "logical_s2");
    }
    else ANA_MSG_ERROR("Bad stage " << stage << ". Stage should be 1 or 2");

    if (!m_layerOverrides.empty()) m_nLogiLayers = m_layerOverrides.size();

    // Read number of physical layers
    int nHeaderLines = 6;
    for (int i = 0; i < nHeaderLines; i++)
    {
        if (!ok) break;
        ok = ok && getline(fin, line);
        ANA_MSG_VERBOSE(line);

        istringstream sline(line);
        ok = ok && (sline >> layerCounts[i] >> silicon >> detReg);

        ok = ok && ( (i < (nHeaderLines/2) && silicon == "pixel") || (i >= (nHeaderLines/2) && silicon == "SCT") );
        ok = ok && ( (i % (nHeaderLines/2) == 0 && detReg == "barrel") || (i % (nHeaderLines/2) != 0 && (detReg == "endcap+" || detReg == "endcap-")) );
    }
    if (!ok) ANA_MSG_FATAL("Error reading layer counts");
    m_nDetLayers = std::accumulate(layerCounts.begin(), layerCounts.end(), 0);

    // Resize objects
    m_map[(int)SiliconTech::pixel][(int)DetectorZone::barrel].resize(layerCounts[0]);
    m_map[(int)SiliconTech::pixel][(int)DetectorZone::posEndcap].resize(layerCounts[1]);
    m_map[(int)SiliconTech::pixel][(int)DetectorZone::negEndcap].resize(layerCounts[2]);
    m_map[(int)SiliconTech::strip][(int)DetectorZone::barrel].resize(layerCounts[3]);
    m_map[(int)SiliconTech::strip][(int)DetectorZone::posEndcap].resize(layerCounts[4]);
    m_map[(int)SiliconTech::strip][(int)DetectorZone::negEndcap].resize(layerCounts[5]);

    m_dimension.resize(m_nLogiLayers);
    m_layerInfo.resize(m_nLogiLayers);
    m_coordOffset.resize(m_nLogiLayers);
}

// Seeks to the selected region
void HTTPlaneMap::seek(ifstream & fin, unsigned region)
{
    std::string line, key;
    unsigned region_read = -1;

    while (getline(fin, line))
    {
        if (line.empty() || line[0] == '!') continue;
        ANA_MSG_VERBOSE(line);
        istringstream sline(line);

        bool ok = static_cast<bool>(sline >> key >> region_read);
        if (ok && key == "region" && region_read == region) break;
    }

    ANA_MSG_DEBUG("seek() ended at:" << line);
}


// Reads the rest of the file to populate all the member vectors
void HTTPlaneMap::readLayers(ifstream & fin, uint32_t stage)
{
    // Initialize read variables
    int BEC, physLayer, physDisk, logiLayer = 0, logiLayer1, logiLayer2, stereo, nDim = 0;
    string line, silicon, planeKey1, planeKey2, stereoKey;
    bool ok = true;

    uint32_t linesRead = 0;

    // Each line is a detector layer
    while (getline(fin, line))
    {
        if (line.empty() || line[0] == '!') continue;
        ANA_MSG_VERBOSE(line);
        istringstream sline(line);

        ok = ok && (sline >> silicon >> BEC >> physDisk >> physLayer);

        // Detector
        DetectorZone zone  = DetectorZone::undefined;
        if (BEC == 0) zone = DetectorZone::barrel;
        else if (BEC == 1) zone = DetectorZone::posEndcap;
        else if (BEC == 2) zone = DetectorZone::negEndcap;
        else ANA_MSG_FATAL("Bad Detector zone" << BEC);

        // Case on silicon type
        int sil = 0;
        SiliconTech siTech = SiliconTech::pixel;
        if (silicon == "pixel")
        {
            sil = static_cast<int>(SiliconTech::pixel);
            nDim = 2;
            stereo = -1;
            ok = ok && (sline >> planeKey1 >> logiLayer1 >> planeKey2 >> logiLayer2);
            ok = ok && (planeKey1 == "plane1" && planeKey2 == "plane2");
        }
        else if (silicon == "SCT")
        {
            sil = static_cast<int>(SiliconTech::strip);
            siTech = SiliconTech::strip;

            nDim = 1;
            ok = ok && (sline >> stereoKey >> stereo >> planeKey1 >> logiLayer1 >> planeKey2 >> logiLayer2);
            ok = ok && (planeKey1 == "plane1" && planeKey2 == "plane2" && stereoKey == "stereo");
        }
        else ANA_MSG_FATAL("Bad silicon type " << silicon);


        if (!ok) break;

        // Logical layer
        if (!m_layerOverrides.empty()) logiLayer = getOverrideLayer(siTech, zone, physLayer);
        else if (stage == 1) logiLayer = logiLayer1;
        else if (stage == 2) logiLayer = logiLayer2;

        // Check number of layers
        linesRead++;
        if (logiLayer >= (int)m_nLogiLayers) ANA_MSG_FATAL("Logical layer " << logiLayer << " exceeds expected number " << m_nLogiLayers);

        // Store to map
        m_map.at(sil).at(BEC).at(physLayer).layer = logiLayer;
        if (logiLayer >= 0)
        {
            m_dimension[logiLayer] = nDim;
            m_map[sil][BEC][physLayer].section = m_layerInfo[logiLayer].size(); // i.e. index into m_layerInfo[logiLayer] entry below
            m_layerInfo[logiLayer].push_back({ siTech, zone, physLayer, physDisk, stereo});
        }

        if (m_nDetLayers == linesRead) break;
    }

    if (!ok) ANA_MSG_FATAL("Error reading file");
}


///////////////////////////////////////////////////////////////////////////////
// Interface Functions
///////////////////////////////////////////////////////////////////////////////


void HTTPlaneMap::map(HTTHit & hit) const
{
    if (hit.isMapped())  return;
    
    // re-assign layers in the pixel endcap to be each individual disk
    unsigned HTTlayer = hit.getPhysLayer();
    if (hit.isPixel() && !hit.isBarrel()) {
        if (hit.getPhysLayer()< m_diskIndex.size())
            HTTlayer = hit.getHTTEtaModule() + m_diskIndex[hit.getPhysLayer()];
        else ANA_MSG_ERROR("Error: requesting "<< hit.getPhysLayer() <<" element in m_diskIndex which is of size "<< m_diskIndex.size() );
    }
    hit.setPhysLayer(HTTlayer);

    const LayerSection &pinfo = getLayerSection(hit.getDetType(), hit.getDetectorZone(), hit.getPhysLayer());
    hit.setSection(pinfo.section);
    hit.setLayer(pinfo.layer);
    if (!hit.isMapped()) // failsafe if for some reason someone calls this on a clustered hit again, or something
        hit.setHitType(HitType::mapped);
}


///////////////////////////////////////////////////////////////////////////////
// Helper Functions
///////////////////////////////////////////////////////////////////////////////

// Gets the logical layer number based on the override list
int HTTPlaneMap::getOverrideLayer(SiliconTech si, DetectorZone dz, int physLayer) // TODO does dz need edit for negEndcap?
{
    int code = static_cast<int>(si) * 1000 + static_cast<int>(dz) * 100 + physLayer;
    for (unsigned i = 0; i < m_layerOverrides.size(); i++)
        if (code == m_layerOverrides[i]) return i;
    return -1;
}

std::string HTTPlaneMap::layerName(uint32_t layer, uint32_t section) const
{
    std::string out;
    out += (isPixel(layer) ? "PIX" : "SCT");
    out += (isEC(layer, section) ? "_EC_" : "_B_");
    out += std::to_string(getLayerInfo(layer, section).physLayer);
    return out;
}
