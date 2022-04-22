// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTPLANEMAP_H
#define HTTPLANEMAP_H

/**
 * @file HTTPlaneMap.h
 * @author Riley Xu - riley.xu@cern.ch (rewrite from FTK)
 * @date Janurary 7th, 2020
 * @brief Maps physical layers to logical layers
 *
 *
 * The plane map file is a columnar text file that defines the logical layers for patterns.
 * The file definition is described in https://twiki.cern.ch/twiki/bin/view/Atlas/FTKConfigFiles#Plane_map_PMAP,
 * with the addition of the line
 *      > # logical
 * at the top of the file, which denotes the total number of logical layers.
 *
 *
 * NB: There are a bunch of different layers floating around. My understanding is that
 *      - detLayer: A detector element, i.e. a unique line in the plane map file
 *      - physLayer: A layer index used in HTT, a different 0-indexed series for each (pixel/strip) X (barrel/endcap) combination.
 *              Total number of physical layers = number of detector layers. The physLayer basically just breaks the detLayers into
 *              different series for each hit type and detector region.
 *      - logiLayer: A logical layer corresponding to an AM bus,  In general, `layer` with no other specifiers refers to a logical layer.
 */

#include "TrigHTTObjects/HTTHit.h"

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>


struct LayerSection
{
  int layer = -1; // logical layer, -1 if unused
  uint32_t section = 0; // arbitrary enumeration (identifier) if there are multiple detector elements in the same logical layer
}; 


/* Max index of modules along phi and eta is specified.
 * Attention: on SCT layers the modules with eta index 0 is missing so the actual number of modules is neta-1.
 * ^ ^ ^
 * Is this still true???
 */
struct LayerInfo
{
  SiliconTech siTech = SiliconTech::undefined;
  DetectorZone zone = DetectorZone::undefined;
  int physLayer = -1;	// physical layer number (see this file's header)
  int physDisk = -1;	//
  int stereo = -1;     // strip layer side
}; 


class HTTPlaneMap 
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // Constructor/Destructor
        ///////////////////////////////////////////////////////////////////////

        // See doc on m_layerOverrides for info on argument layerOverrides
        HTTPlaneMap(const std::string & filepath, unsigned region, unsigned stage,
                std::vector<int> layerOverrides = std::vector<int>());

        ///////////////////////////////////////////////////////////////////////
        // Sizing
        ///////////////////////////////////////////////////////////////////////

        uint32_t getNDetLayers() const { return m_nDetLayers; }
        uint32_t getNLogiLayers() const { return m_nLogiLayers; }
        uint32_t getNCoords() const { return m_nCoords; }
        uint32_t getNSections(size_t logiLayer) const { return m_layerInfo[logiLayer].size(); }
        uint32_t getDim(size_t logiLayer) const { return m_dimension[logiLayer]; }
        SiliconTech getDetType(size_t logiLayer) const { return m_dimension[logiLayer] == 2 ? SiliconTech::pixel : SiliconTech::strip; }

        ///////////////////////////////////////////////////////////////////////
        // Layer <-> Coordinates
        ///////////////////////////////////////////////////////////////////////

        /* Coordinates in tracks / matrices are flattened into a single array.
         * Each layer, in order, gets 1 (strip) or 2 (pixel) entries.
         * This entry gives the offset for the first coordinate for a given layer.
         * The coordinates for a pixel hit are e.g.
         *      x = coords[getCoordOffset(layer)];
         *      y = coords[getCoordOffset(layer) + 1]
         */
        uint32_t getCoordOffset(size_t logiLayer) const { return m_coordOffset[logiLayer]; }

        /* This does the inverse search of the above function getCoordOffset.
         * Given the coordinate index/offset, this function returns the logical layer
         * that coordinate belongs to.
         */
        uint32_t getCoordLayer(uint32_t coord) const { return m_coordLayer[coord]; }

        ///////////////////////////////////////////////////////////////////////
        // Info about a logical layer
        ///////////////////////////////////////////////////////////////////////

        const LayerInfo & getLayerInfo(uint32_t layer, uint32_t section) const { return m_layerInfo.at(layer).at(section); }

        bool isSCT(int pl) const { return m_dimension[pl] == 1; }
        bool isPixel(int pl) const { return m_dimension[pl] == 2; }
        bool isEC(uint32_t layer, uint32_t section) const { return getLayerInfo(layer, section).zone != DetectorZone::barrel; }

        std::string layerName(uint32_t layer, uint32_t section) const;

        ///////////////////////////////////////////////////////////////////////
        // Getting the logical layer
        ///////////////////////////////////////////////////////////////////////

        // Main function. Wrapper/shortcuts below.
        const LayerSection & getLayerSection(SiliconTech siTech, DetectorZone zone, uint32_t physLayer) const
        {
            try {
                return m_map.at(static_cast<int>(siTech)).at(static_cast<int>(zone)).at(physLayer);
            } catch (std::exception const & e)
            {
                // TMP HACK FIX REMOVE ME
                std::stringstream s;
                s << "getLayerSection(siTech=" << (int)siTech << ", DZ=" << (int)zone << ", layer=" << physLayer << ") out of bounds";
		std::cerr << s.str() <<std::endl;
                return m_map.at(1).at(0).at(0);
            }
        }

        const LayerSection & getLayerSection(LayerInfo const & mi) const
            { return getLayerSection(mi.siTech, mi.zone, mi.physLayer); }

        int getLayer(SiliconTech siTech, DetectorZone zone, uint32_t physLayer) const
            { return getLayerSection(siTech, zone, physLayer).layer; }

        unsigned getSection(SiliconTech siTech, DetectorZone zone, uint32_t physLayer) const
            { return getLayerSection(siTech, zone, physLayer).section; }


        ///////////////////////////////////////////////////////////////////////
        // Mapping Hits
        ///////////////////////////////////////////////////////////////////////

        void map(HTTHit & hit) const;

    private:

        uint32_t m_nLogiLayers = 0; // number of logical layers
        uint32_t m_nDetLayers = 0;  // number of physical detector layers used (I think this is different than physical layers, i.e. the index into getMap());
        uint32_t m_nCoords = 0;     // total number of coordinates in a pattern == accumulate(m_dimension)

        std::vector<uint32_t> m_dimension;   // Index by (logiLayer). Dimension of layer (1 for strip, 2 for pixels). Assumes same across logical layer.
        std::vector<uint32_t> m_coordOffset; // Index by (logiLayer).
            // Coordinates for a track i.e. are flattened into a single array. Each layer, in order, gets 1 (strip) or 2 (pixel) entries.
            // This entry gives the offset for the first coordinate for a given layer.
            // The coordinates for a pixel hit are e.g. x = coords[m_coordOffset(layer)], y = coords[m_coordOffset(layer) + 1]
        std::vector<uint32_t> m_coordLayer;  // Index by (coordinate #)
            // Inverse of above map. Given the coordinate index, returns the logical layer of the coordinate.

        std::vector<std::vector<std::vector<LayerSection>>> m_map; // index by (SCT/PIXEL, BARREL/posENDCAP/negENDCAP, physLayer)
            // Maps detector layers to logical layer and section numbers
        std::vector<std::vector<LayerInfo>> m_layerInfo; // index by (logiLayer, section)
            // Get info about a logical layer section

        std::vector<int> m_layerOverrides; // Overrides the selection of logical layers
            // If not empty, each entry declares a detector layer to use as a logical layer
            // Specify a detector layer with { SilictonTech * 1000 + DetectorZone * 100 + PhysicalLayer }

        std::vector<uint32_t> m_diskIndex; // index of disks in the pixel endcap, indexed by ITK layer_disk

        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void allocateMap(std::ifstream & fin, uint32_t stage);
        void seek(std::ifstream & fin, unsigned region);
        void readLayers(std::ifstream & fin, uint32_t stage);
        int getOverrideLayer(SiliconTech si, DetectorZone dz, int physLayer);
};



#endif // HTTPLANEMAP_H
