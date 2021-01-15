// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTRAWHIT_H
#define HTTRAWHIT_H

#include <TObject.h>

#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <sstream>
#include "TrigHTTUtils/HTTMacros.h"
#include "TrigHTTUtils/HTTTypes.h"
#include "HTTMultiTruth.h"

/**
 * The HTTRawHit represent the lowest order representation of the ID
 * hit in the HTT simulation after reading inputs from storegate
 * The object of this class are used for clustering or other manipulations
 * but before road finding and track fitting. From the road finding
 * the hits will be moved using the HTTHit format.
 */
class HTTRawHit : public TObject
{
    public:

        //Return the detector type
        SiliconTech getDetType() const { return m_detType; }
        bool isPixel() const { return m_detType==SiliconTech::pixel; }
        bool isStrip() const { return m_detType==SiliconTech::strip; }

        //Get the position of the hit's module
        unsigned getIdentifierHash() const; // { return m_IdentifierHash; } TODO UPDATE THIS FOR REL22
        DetectorZone getDetectorZone() const { return m_detectorZone; }
        unsigned getLayer() const { return m_layer_disk; }
        unsigned getPhiModule() const { return m_phi_module; }
        int getEtaModule() const { return m_eta_module; }

        //Get the position and width on chip of the RawHit/Cluster
        unsigned getEtaIndex() const { return m_eta_index; }
        unsigned getPhiIndex() const { return m_phi_index; }
        //Get the float equivalent of the position on the chip
        float getEtaCoord() const { return m_eta_index; }
        float getPhiCoord() const { return m_phi_index; }

        unsigned getEtaWidth() const { return m_etaWidth; }
        unsigned getPhiWidth() const { return m_phiWidth; }
        //The side of the strip module (used to translate from ATLAS->HTT layers)
        unsigned getSide() const { return m_side; }
        //Time over threshold for the pixel (currently unused)
        unsigned getToT() const  { return m_ToT; }

        //Hardware word
        unsigned getHWWord() const { return m_hw_word; }

        //Setting the position of the hit's module
        void setDetType(const SiliconTech &detType) { m_detType = detType; }
        void setIdentifierHash(const unsigned int &identHash)  { m_IdentifierHash = identHash; }
        void setDetectorZone(const DetectorZone &barrelEC) { m_detectorZone = barrelEC; }
        void setLayer(const unsigned int &layer) { m_layer_disk = layer; }
        void setPhiModule(const unsigned int &phiModule) { m_phi_module = phiModule; }
        void setEtaModule(const unsigned int &etaModule) { m_eta_module = etaModule; }

        //Setting the position and width on chip of the RawHit/Cluster
        void setPhiIndex(const unsigned int &phiIndex) { m_phi_index = phiIndex; }
        void setEtaIndex(const unsigned int &etaIndex) { m_eta_index = etaIndex; }
        void setEtaWidth(const unsigned int &etaWidth) { m_etaWidth = etaWidth; }
        void setPhiWidth(const unsigned int &phiWidth) { m_phiWidth = phiWidth; }
        void setSide(const unsigned int &side) { m_side = side;}
        void setToT(const unsigned int &timeOverThreshold) { m_ToT = timeOverThreshold; }

        void setHardwareWord(const int &hardwareWord) { m_hw_word = hardwareWord; }

        //Get truth information associated with the hit
        const signed long& getBarcode() const { return m_barcode; }
        void setBarcode(const signed long &barcode) { m_barcode = barcode; }
        const signed long& getEventIndex() const { return m_eventindex; }
        void setEventIndex(const signed long &eventIndex) { m_eventindex = eventIndex; }
        const float& getBarcodePt() const { return m_barcode_pt; }
        void setBarcodePt(const float &barcodePt) { m_barcode_pt = barcodePt; }
        const unsigned long& getParentageMask() const { return m_parentage_mask; }
        void setParentageMask(const unsigned long &parentageMask)  { m_parentage_mask = parentageMask; }
        void setTruth(const HTTMultiTruth &truth) { m_truth = truth; }
        const HTTMultiTruth getTruth() const { return m_truth; }

        void normalizeLayerID(); // set the layer id following the HTT convention

        float getX() const { return m_x; }
        float getY() const { return m_y; }
        float getZ() const { return m_z; }
        void setX(float v) { m_x = v; }
        void setY(float v) { m_y = v; }
        void setZ(float v) { m_z = v; }

        friend std::ostream& operator<<(std::ostream &,const HTTRawHit&);

    private:
        SiliconTech m_detType;     /** Flag to know if the module belongs to Pixel or Strip  */
        unsigned int m_IdentifierHash; // Athena identifier hash
        DetectorZone m_detectorZone;   /** and if it's on barrel or endcaps */
        unsigned int m_layer_disk;  /** Layer or disk number */
        unsigned int m_phi_module;  /** Module id along phi */
        unsigned int m_eta_module;  /** Module id along eta */
        unsigned int m_phi_index;     /** phi index for pixel, strip for Strip */
        unsigned int m_eta_index;    /** eta index for pixel, row for Strip */
        unsigned int m_ToT;         /** Time over Threshold */
        unsigned int m_side;        // Side of the strip module
        unsigned int m_etaWidth;    // width of cluster along the 2 directions
        unsigned int m_phiWidth;    //For Strips the NWidth variable is currently stored in the PhiWidth

        ///////////////////////////////////////////////////////////////////////
        // Hit position in global coordinates
        float m_x;
        float m_y;
        float m_z;

        ///////////////////////////////////////////////////////////////////////

        // Split out into HTTStripBitstreamConvertor
        /** store Strip and Pixel cluster positions as integers using the same HTT_IM HW definition */
        unsigned int m_hw_word;

        signed long m_eventindex; /** athena event index assigned to this channel */
        signed long m_barcode; /** geant particle barcode assigned to this channel */
        float m_barcode_pt; /** maximum 'pt' for any 'good'
                              geant particle contributing to the
                              channel. corresponds to the particle with
                              m_barcode */
        unsigned long m_parentage_mask; /** ancestor information of this channel */
        /** geant truth data (clusters only). filled
          during clustering using the m_barcode and
          m_barcode_frac data for each raw hit in the
          cluster. */

        HTTMultiTruth m_truth;


        ClassDef(HTTRawHit,4)
};


#endif // HTTRAWHIT_H
