/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFParticle.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_ISFPARTICLE_H
#define ISF_EVENT_ISFPARTICLE_H

// Gaudi Kernel
#include "GaudiKernel/MsgStream.h"
// Barcode includes
#include "BarcodeEvent/Barcode.h"
// ISF include
#include "ISF_Event/TruthBinding.h"
#include "ISF_Event/SimSvcID.h"
#include "ISF_Event/ParticleOrder.h"
#include "ISF_Event/ParticleUserInformation.h"
// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"
// Amg classes
#include "GeoPrimitives/GeoPrimitives.h"
// CHLEP classes
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
// Common classes
#include "GeneratorObjects/HepMcParticleLink.h"

namespace ISF {

  /** the datatype to be used to store each individual particle hop */
  typedef std::pair< AtlasDetDescr::AtlasRegion, ISF::SimSvcID >   DetRegionSvcIDPair;
  /** the container type to be used for the particle hops */
  typedef std::vector< DetRegionSvcIDPair >                        ParticleHistory;

  /**
     @class ISFParticle

     The generic ISF particle definition,

     @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */

  class ISFParticle {

  public:
    /** disallow default constructor */
    ISFParticle() = delete;

    /** use this constructor whenever possible */
    ISFParticle(const Amg::Vector3D& pos,
                const Amg::Vector3D& mom,
                double mass,
                double charge,
                int pdgCode,
                double time,
                const ISFParticle &parent,
                Barcode::ParticleBarcode barcode = Barcode::fUndefinedBarcode,
                TruthBinding* truth = nullptr,
                const HepMcParticleLink * partLink = nullptr );

    /** CLHEP-compatible constructor */
    ISFParticle(const HepGeom::Point3D<double>& pos,
                const HepGeom::Vector3D<double>& mom,
                double mass,
                double charge,
                int pdgCode,
                double time,
                const ISFParticle &parent,
                Barcode::ParticleBarcode barcode = Barcode::fUndefinedBarcode,
                TruthBinding* truth = nullptr,
                const HepMcParticleLink * partLink = nullptr );

    /** this constructor should only be used for event read-in */
    ISFParticle(const Amg::Vector3D& pos,
                const Amg::Vector3D& mom,
                double mass,
                double charge,
                int pdgCode,
                double time,
                const DetRegionSvcIDPair &origin,
                int bcid,
                Barcode::ParticleBarcode barcode = Barcode::fUndefinedBarcode,
                TruthBinding* truth = nullptr,
                const HepMcParticleLink * partLink = nullptr );

    /** Copy constructor */
    ISFParticle(const ISFParticle& isfp);

    /** Move copy constructor */
    ISFParticle(ISFParticle&& isfp);

    /** Destructor */
    ~ISFParticle();

    /** Assignment operator */
    ISFParticle& operator=(const ISFParticle& rhs);

    /** Move assignment operator */
    ISFParticle& operator=(ISFParticle&& rhs);

    /** Comparisons */
    bool operator==(const ISFParticle& rhs) const;
    bool isEqual(const ISFParticle& rhs) const;
    bool isIdent(const ISFParticle& rhs) const;

    /** The current momentum vector of the ISFParticle */
    const Amg::Vector3D& momentum() const;

    /** The current position of the ISFParticle */
    const Amg::Vector3D& position() const;

    /** Update the position of the ISFParticle */
    void updatePosition(const Amg::Vector3D& position);

    /** Update the momentum of the ISFParticle */
    void updateMomentum(const Amg::Vector3D& momentum);

    /** mass of the particle */
    double mass() const;

    /** charge of the particle */
    double charge() const;

    /** Timestamp of the ISFParticle */
    double timeStamp() const;

    /** PDG value */
    int pdgCode() const;

    /** Kinetic energy */
    double ekin() const;

    /** the particle's full history */
    const ParticleHistory& history() const;
    /** the sub-detector ID the particle's originates from */
    AtlasDetDescr::AtlasRegion  originGeoID() const;
    /** the simulation service the particle originates from */
    SimSvcID                    originSimID() const;
    /** previous geoID that the particle was simulated in */
    AtlasDetDescr::AtlasRegion  prevGeoID() const;
    /** the simulation service that previously treated this particle */
    SimSvcID                    prevSimID() const;
    /** next geoID the particle will be simulated in */
    AtlasDetDescr::AtlasRegion  nextGeoID() const;
    /** the next simulation service the particle will be sent to */
    SimSvcID                    nextSimID() const;
    /** register the next AtlasDetDescr::AtlasRegion */
    void                        setNextGeoID(AtlasDetDescr::AtlasRegion geoID);
    /** register the next SimSvcID */
    void                        setNextSimID(SimSvcID simID);

    /** the barcode */
    Barcode::ParticleBarcode barcode() const;

    /** set a new barcode */
    void setBarcode(Barcode::ParticleBarcode bc);
    /** set a new barcode and update the HepMcParticleLink  */
    void setBarcodeAndUpdateHepMcParticleLink(Barcode::ParticleBarcode bc);

    /** bunch-crossing identifier */
    int getBCID() const;

    /** set bunch-crossing identifier */
    void setBCID(int bcid);

    /** pointer to the simulation truth - optional, can be 0 */
    TruthBinding* getTruthBinding() const;
    void          setTruthBinding(TruthBinding *truth);

    /** HepMcParticleLink accessors */
    inline const HepMcParticleLink* getParticleLink() const {return  m_partLink;};
    inline void setParticleLink(const HepMcParticleLink* partLink) {m_partLink=partLink;};

    /** return the particle order (eg used to assure ID->Calo->MS simulation order) */
    ParticleOrder  getOrder() const;
    void           setOrder(ParticleOrder order);

    /** get/set ParticleUserInformation */
    ParticleUserInformation *getUserInformation() const;
    void                     setUserInformation(ParticleUserInformation *userInfo);

    /** Dump methods to be used by the overloaded stream operator (inheritance!) */
    MsgStream&    dump(MsgStream& out) const;
    std::ostream& dump(std::ostream& out) const;

  private :
    Amg::Vector3D                m_position;
    Amg::Vector3D                m_momentum;
    double                       m_mass;
    double                       m_charge;
    int                          m_pdgCode;
    double                       m_tstamp;
    ParticleHistory              m_history;
    Barcode::ParticleBarcode     m_barcode;
    int                          m_bcid;                  //!< bunch-crossing identifier
    TruthBinding*                m_truth;
    ParticleOrder                m_order;                 //!< particle simulation order
    mutable ParticleUserInformation *m_userInfo;          //!< user information stored with the ISFParticle
    const HepMcParticleLink*     m_partLink;
  };

  // Overload of << operator for MsgStream for debug output
  //
  inline MsgStream&    operator << ( MsgStream& sl,    const ISF::ISFParticle& isfp) { isfp.dump(sl); return sl; }
  inline std::ostream& operator << ( std::ostream& sl, const ISF::ISFParticle& isfp) { isfp.dump(sl); return sl; }

} // end of namespace

/* implementation for inlined methods */
#include <ISF_Event/ISFParticle.icc>

#endif // ISF_EVENT_ISFPARTICLE_H
