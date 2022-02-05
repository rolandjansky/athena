/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Common Muon Detector Element properties
 -----------------------------------------
 ***************************************************************************/

//!  A MuonDetectorElement Class
/*!
  MuonDetectorElements are as granular as offline muon data collections
  (prepRawData); hence they can be retrieved using the collection hash
  identifier.  MuonDetectorElement will hold an array of MuonReadoutElements
  which will actually implement the real functionality; MuonDetectorElement
  methods will delegate the job to the appropriate MuonReadoutElement.
*/

#ifndef MUONREADOUTGEOMETRY_MUONDETECTORELEMENT_H
#define MUONREADOUTGEOMETRY_MUONDETECTORELEMENT_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "TrkDetElementBase/TrkDetElementBase.h"

class GeoVFullPhysVol;

namespace MuonGM {

    class MuonDetectorManager;

    /**
       Base class for the XxxDetectorelement,
       with Xxx = Mdt, Rpc, Tgc, Csc. It's a Trk::TrkDetElementBase,
       therefore it must implement the generic tracking interfaces requested
       to the geometry: center, normal, surfaceBound, transform.  It is
       synched to the raw geometry via the pointer, required by the constructor,
       to its corresponding GeoVFullPhysVol.  A MuonDetectorElement holds a
       pointer to its parent MuonStation.
    */

    class MuonDetectorElement : public Trk::TrkDetElementBase {
    public:
        MuonDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash);
        virtual ~MuonDetectorElement();

        // Identification methods
        Identifier identify() const { return m_id; }               //!< returns the collection extended identifier (EDM granularity)
        IdentifierHash identifyHash() const { return m_idhash; };  //!< returns the collectionHash (EDM granularity)
        virtual int getStationEta() const = 0;                     //!< returns stationEta
        virtual int getStationPhi() const = 0;                     //!< returns stationPhi

        // DetectorElement content
        unsigned int nReadoutElements() const { return m_nREinDetectorElement; }
        //!< returns the number of ReadoutElements collected under this detector
        // Common tracking generic interfaces

        virtual unsigned int nMDTinStation() const = 0;
        virtual unsigned int nCSCinStation() const = 0;
        virtual unsigned int nTGCinStation() const = 0;
        virtual unsigned int nRPCinStation() const = 0;

    protected:
        inline const MuonDetectorManager* manager() const;
        Identifier m_id;          //!< extended data-collection identifier
        IdentifierHash m_idhash;  //!< data-collection hash identifier

        unsigned int m_nREinDetectorElement{0};

    private:
        MuonDetectorManager* m_muon_mgr{nullptr};
    };

    const MuonDetectorManager* MuonDetectorElement::manager() const { return m_muon_mgr; }

}  // namespace MuonGM

#endif  // MUONREADOUTGEOMETRY_MUONDETECTORELEMENT_H
