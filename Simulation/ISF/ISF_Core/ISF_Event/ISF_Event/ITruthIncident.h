/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthIncident.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISF_EVENT_ITRUTHINCIDENT_H
#define ISF_EVENT_ITRUTHINCIDENT_H 1

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// Barcode includes
#include "BarcodeInterfaces/Barcode.h"
#include "BarcodeInterfaces/PhysicsProcessCode.h"

// forward declarations
namespace HepMC {
  class FourVector;
  class GenParticle;
}

namespace ISF {

  /** @class ITruthIncident

      @TODO: class description

      @author Elmar.Ritsch -at- cern.ch
  */
  class ITruthIncident {
  public:
    ITruthIncident(AtlasDetDescr::AtlasRegion geoID):m_geoID(geoID),m_passWholeVertex(true),m_wholeVertexPassed(false) { };

    /** Return the SimGeoID corresponding to the vertex of the truth incident */
    AtlasDetDescr::AtlasRegion geoID() { return m_geoID; };

    /** Return HepMC position of the truth vertex */
    virtual const HepMC::FourVector&  position() const = 0;

    /** Return Physics process code of the truth incident */
    virtual Barcode::PhysicsProcessCode physicsProcessCode() const = 0;

    /** Return p^2 of the primary particle */
    virtual double                    primaryP2() const = 0;
    /** Return pT^2 of the primary particle */
    virtual double                    primaryPt2() const = 0;
    /** Return Ekin of the primary particle */
    virtual double                    primaryEkin() const = 0;
    /** Return the PDG Code of the primary particle */
    virtual int                       primaryPdgCode() const = 0;
    /** Return the primary particle as a HepMC particle type
        (only called for particles that will enter the HepMC truth event) */
    virtual HepMC::GenParticle*       primaryParticle(bool setPersistent=false) const = 0;
    /** Return the barcode of the primary particle */
    virtual Barcode::ParticleBarcode  primaryBarcode() const = 0;
    /** Return the extra barcode of the primary particle */
    virtual Barcode::ParticleBarcode  primaryExtraBarcode() const { return 0; }
    /** Return the primary particle after the TruthIncident vertex (and assign
        a new barcode to it) */
    virtual HepMC::GenParticle*       primaryParticleAfterIncident(Barcode::ParticleBarcode newBC,
                                                                   bool setPersistent=false) = 0;

    /** Return total number of secondary particles */
    virtual unsigned short            numberOfSecondaries() const = 0;
    /** Return p^2 of the i-th secondary particle */
    virtual double                    secondaryP2(unsigned short index) const = 0;
    /** Return pT^2 of the i-th secondary particle */
    virtual double                    secondaryPt2(unsigned short index) const = 0;
    /** Return Ekin of the i-th secondary particle */
    virtual double                    secondaryEkin(unsigned short index) const = 0;
    /** Return the PDG Code of the i-th secondary particle */
    virtual int                       secondaryPdgCode(unsigned short index) const = 0;
    /** Return true if at least one secondary particle passes the given p^2 cut
        (= at least one secondary with p^2 >= pt2cut) */
    inline bool                       secondaryP2Pass(double p2cut) const;
    /** Return true if at least one secondary particle passes the given pT^2 cut
        (= at least one secondary with pT^2 >= pt2cut) */
    inline bool                       secondaryPt2Pass(double pt2cut) const;
    /** Return true if at least one secondary particle passes the given Ekin cut
        (= at least one secondary with Ekin >= ekincut) */
    inline bool                       secondaryEkinPass(double ekincut) const;
    /** Return the i-th secondary as a HepMC particle type and assign the given
        Barcode to the simulator particle (only called for particles that will
        enter the HepMC truth event) */
    virtual HepMC::GenParticle*       secondaryParticle(unsigned short index,
                                                        Barcode::ParticleBarcode bc = Barcode::fUndefinedBarcode,
                                                        bool setPersistent=false) const = 0;
    /** Set the the barcode of all secondary particles to the given bc */
    virtual void                      setAllSecondaryBarcodes(Barcode::ParticleBarcode bc) = 0;
    /** Set the the extra barcode of all secondary particles to the given bc */
    virtual void                      setAllSecondaryExtraBarcodes(Barcode::ParticleBarcode /*bc*/) {};
    /** Set the the extra barcode of a secondary particles to the given bc */
    virtual void                      setSecondaryExtraBarcode(unsigned short /*index*/, Barcode::ParticleBarcode /*bc*/) {};
    /** Record that a particular secondary passed a check */
    virtual void                      setSecondaryPassed(unsigned short index) const = 0;
    /** Should a particular secondary be written out to the GenEvent */
    virtual bool                      writeOutSecondary(unsigned short index) const = 0;
    /** Set whether this TruthIncident should pass the vertex as a whole or individual secondaries */
    inline void                       setPassWholeVertices(bool passWholeVertex);

  private:
    AtlasDetDescr::AtlasRegion                     m_geoID;
  protected:
    bool m_passWholeVertex;
    mutable bool m_wholeVertexPassed; //FIXME clean up logic so that
                                      //this doesn't need to be
                                      //mutable.
  };

  //
  // inline methods :
  //

  // default loops to check a given cut for all secondary particles

  // loop over secondaries to find out whether the momentum cut is passed or not
  bool ISF::ITruthIncident::secondaryP2Pass(double p2cut) const {
    unsigned short numSec = numberOfSecondaries();
    bool pass = false; // true if cut passed
    // as soon as at a particle passes the cut -> end loop and return true
    for ( unsigned short i=0; (!pass && m_passWholeVertex) && (i<numSec); ++i) {
      bool thispassed = (secondaryP2(i) >= p2cut);
      if(thispassed) { setSecondaryPassed(i); }
      pass |= thispassed;
    }
    m_wholeVertexPassed=m_passWholeVertex && pass;
    return pass;
  }

  // loop over secondaries to find out whether the transverse momentum cut is passed or not
  bool ISF::ITruthIncident::secondaryPt2Pass(double pt2cut) const {
    unsigned short numSec = numberOfSecondaries();
    bool pass = false; // true if cut passed
    // as soon as at a particle passes the cut -> end loop and return true
    for ( unsigned short i=0; (!pass && m_passWholeVertex) && (i<numSec); ++i) {
      bool thispassed = (secondaryPt2(i) >= pt2cut);
      if(thispassed) { setSecondaryPassed(i); }
      pass |= thispassed;
    }
    m_wholeVertexPassed=m_passWholeVertex && pass;
    return pass;
  }

  // loop over secondaries to find out whether the transverse momentum cut is passed or not
  bool ISF::ITruthIncident::secondaryEkinPass(double ekincut) const {
    unsigned short numSec = numberOfSecondaries();
    bool pass = false; // true if cut passed
    // as soon as at a particle passes the cut -> end loop and return true
    for ( unsigned short i=0; (!pass && m_passWholeVertex) && (i<numSec); ++i) {
      bool thispassed = (secondaryEkin(i) >= ekincut);
      if(thispassed) { setSecondaryPassed(i); }
      pass |= thispassed;
    }
    m_wholeVertexPassed=m_passWholeVertex && pass;
    return pass;
  }

  void ISF::ITruthIncident::setPassWholeVertices(bool passWholeVertex) {
    m_passWholeVertex=passWholeVertex;
  }

}

#endif //> !ISF_EVENT_ITRUTHINCIDENT_H
