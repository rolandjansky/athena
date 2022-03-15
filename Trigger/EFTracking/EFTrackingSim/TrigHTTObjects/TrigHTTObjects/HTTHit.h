// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTOBJECTS_HTTHIT_H
#define TRIGHTTOBJECTS_HTTHIT_H

/**
 * @file: HTTHit.h
 * @author: Lots of peeps
 * @date: Lightyears ago
 * @brief: HTT-specific class to represent an hit in the detector.
 *
 * Declarations in this file:
 *      enum class HitType
 *      class HTTHit
 *      vector<vector<const HTTHit*>> sortByLayer(vector<const HTTHit*> const & hits);
 *      ostream & operator<<(ostream & os, HitType t);
 *
 *
 * Pay close attention to the enum HitType below. Several members of the class
 * will be invalid until the hit is put through some step of the processing,
 * like mapping or clustering. Calling a get function on the wrong type will
 * throw std::domain_error.
 */

#include <vector>
#include <ostream>

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTMultiTruth.h"

#include <Rtypes.h>
#include "TMath.h"

 // Some types are inclusive of others. For example, clustered implies mapped.
enum class HitType { unmapped, mapped, clustered, wildcard, guessed, undefined, extrapolated, spacepoint };

class HTTHit
{
public:

    ///////////////////////////////////////////////////////////////////////
    // Constructors

    HTTHit() = default;
    virtual ~HTTHit() = default;

    ///////////////////////////////////////////////////////////////////////
    // Getters/Setters

    // --- Hit Type ---
    void setHitType(HitType type) { m_hitType = type; }
    void setDetType(SiliconTech detType) { m_detType = detType; }
    void setDetectorZone(DetectorZone detZone) { m_detectorZone = detZone; }
    HitType      getHitType()      const { return m_hitType; }
    SiliconTech  getDetType()      const { return m_detType; }
    DetectorZone getDetectorZone() const { return m_detectorZone; }

    bool isMapped() const;
    bool isClustered() const;
    bool isReal() const; // ie not wildcard or guessed
    bool isPixel() const { return m_detType == SiliconTech::pixel; }
    bool isStrip() const { return m_detType == SiliconTech::strip; }
    bool isBarrel() const { return m_detectorZone == DetectorZone::barrel; }
    unsigned getDim() const { return isPixel() ? 2 : 1; } // TODO all calls of this should be replaced with a call to plane map

    // --- Unmapped Location ---
    void setIdentifierHash(unsigned v) { m_identifierHash = v; }
    void setLayerDisk(unsigned v) { m_layer_disk = v; } // ITk layer number
    void setSide(unsigned v) { m_side = v; }
    void setPhysLayer(unsigned v); // Sets using the HTT-defined physical layer index (see HTTPlaneMap.h)
    void setEtaModule(int v) { m_etaModule = v; }
    void setPhiModule(unsigned v) { m_phiModule = v; }
    void setEtaWidth(unsigned v) { m_etaWidth = v; }
    void setPhiWidth(unsigned v) { m_phiWidth = v; }
    unsigned getIdentifierHash() const { return m_identifierHash; } // TODO note this might break things in the same way as getSide() a few lines below. If so, recomment.
    unsigned getHTTIdentifierHash() const; // TODO UPDATE THIS FOR REL22
    unsigned getLayerDisk() const { return m_layer_disk; } // ITk layer number
    unsigned getSide() const { return m_side; } // strip side TODO note this has been uncommented on 4/20/21. If wrappers suddenly break, recomment this. Same for getIdentifierHash above.
    unsigned getPhysLayer() const; // Returns the HTT-defined physical layer index (see HTTPlaneMap.h)
    unsigned getEtaWidth() const { return m_etaWidth; }
    unsigned getPhiWidth() const { return m_phiWidth; }
    //unsigned getEtaModule() const { return m_etaModule; }
    int getHTTEtaModule() const; // TODO UPDATE THIS FOR REL22
    unsigned getPhiModule() const { return m_phiModule; }

    // --- Mapped Location ---
    // NB: isMapped() should return true to access these members
    void setLayer(unsigned v) { m_layer = v; } // This is the logical layer
    void setSection(unsigned v) { m_section = v; }
    unsigned getLayer() const;
    unsigned getSection() const;

    // --- Local Coordinates ---
    // The local coordinate is stored as an unsigned int, as in the hardware.
    // To get the actual coordinate in units of sensor channels, use the float
    // getCoord functions instead.
    void setPhiIndex(unsigned v) { m_phiIndex = v; }
    void setEtaIndex(unsigned v) { m_etaIndex = v; }
    void setPhiCoord(float v) { m_phiIndex = v; }
    void setEtaCoord(float v) { m_etaIndex = v; }
    unsigned getPhiIndex() const { return m_phiIndex; }
    unsigned getEtaIndex() const { return m_etaIndex; }
    float getPhiCoord() const { return m_phiIndex; }
    float getEtaCoord() const { return m_etaIndex; }

    // --- Global Coordinates ---
    void setX(float v) { m_x = v; }
    void setY(float v) { m_y = v; }
    void setZ(float v) { m_z = v; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getZ() const { return m_z; }
    float getR() const { return TMath::Sqrt(m_x * m_x + m_y * m_y); } // This is cylindrical radius
    float getGPhi() const { return TMath::ATan2(m_y, m_x); }
    float getGCotTheta() const { return m_z / getR(); } // cot(theta) = z/r

    // --- Truth and Other ---
    void setToT(unsigned v) { m_ToT = v; }
    void setBarcode(long v) { m_barcode = v; }
    void setBarcodePt(float v) { m_barcode_pt = v; }
    void setEventIndex(long v) { m_eventindex = v; }
    void setParentageMask(unsigned long v) { m_parentage_mask = v; }
    void setTruth(const HTTMultiTruth& v) { m_truth = v; }
    long getBarcode() const { return m_barcode; }
    long getEventIndex() const { return m_eventindex; }
    float getBarcodePt() const { return m_barcode_pt; }
    unsigned long getParentageMask() const { return m_parentage_mask; }
    const HTTMultiTruth& getTruth() const { return m_truth; }

    ///////////////////////////////////////////////////////////////////////
    // Other Interface

    friend std::ostream& operator<<(std::ostream&, const HTTHit&);

protected:

    // --- Hit Type ---
    HitType m_hitType = HitType::undefined;
    DetectorZone m_detectorZone = DetectorZone::undefined; // barrel / posEC / negEC (0,1,2)
    SiliconTech m_detType = SiliconTech::undefined; // strip / pixel

    // --- Unmapped Location ---
    unsigned m_identifierHash = 0; // Global module ID, from ITk
    unsigned m_layer_disk = 0;     // ITk layer number
    unsigned m_side = 0;           // Side of the strip module
    int m_etaModule = 0; // eta index of the module that the hit is located on
    unsigned m_phiModule = 0; // phi index of the module that the hit is located on
    unsigned m_etaWidth = 0;  // clustering width along eta
    unsigned m_phiWidth = 0;  // clustering width in phi direction

    // --- Mapped Location ---
    // isMapped() should return true to access these members
    int m_layer = -1;   // Logical layer this hit is on
    int m_section = -1; // Index of detector element in the logical layer (see HTTPlaneMap.h)

    // --- Local Coordinates ---
    int m_phiIndex = -1; // phi index for pixel, strip for strip
    int m_etaIndex = -1; // eta index for pixel, row for strip

    // --- Global Coordinates ---
    float m_x = 0;  // Hit position in global coordinates
    float m_y = 0;
    float m_z = 0;

    // --- Truth and Other ---
    unsigned m_hw_word; // store Strip and Pixel cluster positions as integers using the same HTT_IM HW definition
    unsigned m_ToT; // time over Threshold
    long m_eventindex; // athena event index assigned to this channel
    long m_barcode; // geant particle barcode assigned to this channel
    float m_barcode_pt; // maximum 'pt' for any 'good' geant particle contributing to the channel.
                        // corresponds to the particle with m_barcode
    unsigned long m_parentage_mask; // ancestor information of this channel

    HTTMultiTruth m_truth;
    // geant truth data (clusters only). filled during clustering using the m_barcode and
    // m_barcode_frac data for each raw hit in the cluster.

    ClassDef(HTTHit, 5);
};

// Container of <HTTHit const *>
template <class Container>
std::vector<std::vector<const HTTHit*>> sortByLayer(Container const& hits)
{
    std::vector<std::vector<const HTTHit*>> out;
    for (HTTHit const* hit : hits)
    {
        size_t layer = hit->getLayer();
        if (layer >= out.size()) out.resize(layer + 1);
        out[layer].push_back(hit);
    }
    return out;
}


std::ostream& operator<<(std::ostream& os, HitType t);
std::string to_string(HitType t);

#endif // HTTHIT_H
