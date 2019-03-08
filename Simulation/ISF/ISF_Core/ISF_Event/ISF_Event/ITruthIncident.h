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
#include "BarcodeEvent/Barcode.h"
#include "BarcodeEvent/PhysicsProcessCode.h"

// forward declarations
namespace HepMC {
  class FourVector;
  class GenParticle;
}

namespace ISF {

  /**  The interaction classifications are described as follows:
       STD_VTX: interaction of a particle without a pre-defined decay;
       QS_SURV_VTX: a particle with a pre-defined decay under-going a
       non-destructive interaction;
       QS_DEST_VTX: a particle with a pre-defined decay under-going a
       destructive interaction other than its pre-defined decay;
       QS_PREDEF_VTX: a particle under-going its pre-defined decay */
  typedef enum InteractionClass_t {
    STD_VTX = 0,
    QS_SURV_VTX = 1,
    QS_DEST_VTX = 2,
    QS_PREDEF_VTX = 3,
    UNKNOWN_VTX
  } InteractionClass_t;

  /** @class ITruthIncident

      ISF interface class for TruthIncidents. Information regarding
      interactions that occur inside simulators are wrapped into
      dedicated TruthIncident implemenations. The ITruthIncident
      interface offers a layer of abstraction to the ISF framework,
      in order to build a MC truth record of the simulated event.

      @author Elmar.Ritsch -at- cern.ch
  */
  class ITruthIncident {
  public:
    ITruthIncident(AtlasDetDescr::AtlasRegion geoID, unsigned short numChildren): m_geoID(geoID),
                                                                                  m_numChildren(numChildren),
                                                                                  m_passWholeVertex(true),
                                                                                  m_childPassedFilters(numChildren,false) { };

    /** Return the SimGeoID corresponding to the vertex of the truth incident */
    AtlasDetDescr::AtlasRegion geoID() { return m_geoID; };

    /** Return HepMC position of the truth vertex */
    virtual const HepMC::FourVector&  position() const = 0;

    /** Return category of the physics process represented by the truth incident (eg hadronic, em, ..) */
    virtual int                       physicsProcessCategory() const = 0;
    /** Return specific physics process code of the truth incident (eg ionisation, bremsstrahlung, ..)*/
    virtual Barcode::PhysicsProcessCode physicsProcessCode() const = 0;

    /** Return p^2 of the parent particle */
    virtual double                    parentP2() const = 0;
    /** Return pT^2 of the parent particle */
    virtual double                    parentPt2() const = 0;
    /** Return Ekin of the parent particle */
    virtual double                    parentEkin() const = 0;
    /** Return the PDG Code of the parent particle */
    virtual int                       parentPdgCode() const = 0;
    /** Return the parent particle as a HepMC particle type
        (only called for particles that will enter the HepMC truth event) */
    virtual HepMC::GenParticle*       parentParticle() const = 0;
    /** Return the barcode of the parent particle */
    virtual Barcode::ParticleBarcode  parentBarcode() const = 0;
    /** Return the bunch-crossing identifier of the parent particle */
    virtual int                       parentBCID() const { return 0; }
    /** Return a boolean whether or not the parent particle survives the incident */
    virtual bool                      parentSurvivesIncident() const = 0;
    /** Return the parent particle after the TruthIncident vertex (and assign
        a new barcode to it) */
    virtual HepMC::GenParticle*       parentParticleAfterIncident(Barcode::ParticleBarcode newBC) = 0;

    /** Return total number of child particles */
    inline unsigned short             numberOfChildren() const;
    /** Return p^2 of the i-th child particle */
    virtual double                    childP2(unsigned short index) const = 0;
    /** Return pT^2 of the i-th child particle */
    virtual double                    childPt2(unsigned short index) const = 0;
    /** Return Ekin of the i-th child particle */
    virtual double                    childEkin(unsigned short index) const = 0;
    /** Return the PDG Code of the i-th child particle */
    virtual int                       childPdgCode(unsigned short index) const = 0;
    /** Return true if at least one child particle passes the given p^2 cut
        (= at least one child with p^2 >= pt2cut) */
    inline bool                       childrenP2Pass(double p2cut);
    /** Return true if at least one child particle passes the given pT^2 cut
        (= at least one child with pT^2 >= pt2cut) */
    inline bool                       childrenPt2Pass(double pt2cut);
    /** Return true if at least one child particle passes the given Ekin cut
        (= at least one child with Ekin >= ekincut) */
    inline bool                       childrenEkinPass(double ekincut);
    /** Return the i-th child as a HepMC particle type and assign the given
        Barcode to the simulator particle (only called for particles that will
        enter the HepMC truth event) */
    virtual HepMC::GenParticle*       childParticle(unsigned short index,
                                                    Barcode::ParticleBarcode bc = Barcode::fUndefinedBarcode) const = 0;
    /** Update the properties of a child particle from a pre-defined
        interaction based on the properties of the ith child of the
        current TruthIncident (only used in quasi-stable particle
        simulation). */
    virtual HepMC::GenParticle*       updateChildParticle(unsigned short index,
                                                          HepMC::GenParticle *existingChild) const = 0;
    /** Set the the barcode of all child particles to the given bc */
    virtual void                      setAllChildrenBarcodes(Barcode::ParticleBarcode bc) = 0;

    /** Record that a particular child passed a check */
    inline void                       setChildPassedFilters(unsigned short index);
    /** Should a particular child be written out to the GenEvent */
    inline bool                       childPassedFilters(unsigned short index) const;
    /** Set whether this TruthIncident should pass the vertex as a whole or individual children */
    inline void                       setPassWholeVertices(bool passWholeVertex);

    /**  The interaction classifications are described as follows:
         STD_VTX: interaction of a particle without a pre-defined decay;
         QS_SURV_VTX: a particle with a pre-defined decay under-going a
         non-destructive interaction;
         QS_DEST_VTX: a particle with a pre-defined decay under-going a
         destructive interaction other than its pre-defined decay;
         QS_PREDEF_VTX: a particle under-going its pre-defined decay */
    virtual ISF::InteractionClass_t interactionClassification() const {return ISF::STD_VTX;};
  private:
    AtlasDetDescr::AtlasRegion        m_geoID; //!< region that the TruthIncident is located in
  protected:
    int                               m_numChildren;
    bool                              m_passWholeVertex;
    std::vector<bool>                 m_childPassedFilters;

  };

  //
  // inline methods :
  //

  unsigned short ISF::ITruthIncident::numberOfChildren() const {
    return m_numChildren;
  }

  // default loops to check a given cut for all child particles

  // loop over children to find out whether the momentum cut is passed or not
  bool ISF::ITruthIncident::childrenP2Pass(double p2cut) {
    bool pass = false; // true if cut passed
    // as soon as at a particle passes the cut -> end loop and return true
    for ( unsigned short i=0; !(pass && m_passWholeVertex) && (i<m_numChildren); ++i) {
      bool thispassed = (childP2(i) >= p2cut);
      if(thispassed) { setChildPassedFilters(i); }
      pass |= thispassed;
    }
    return pass;
  }

  // loop over children to find out whether the transverse momentum cut is passed or not
  bool ISF::ITruthIncident::childrenPt2Pass(double pt2cut) {
    bool pass = false; // true if cut passed
    // as soon as at a particle passes the cut -> end loop and return true
    for ( unsigned short i=0; !(pass && m_passWholeVertex) && (i<m_numChildren); ++i) {
      bool thispassed = (childPt2(i) >= pt2cut);
      if(thispassed) { setChildPassedFilters(i); }
      pass |= thispassed;
    }
    return pass;
  }

  // loop over children to find out whether the transverse momentum cut is passed or not
  bool ISF::ITruthIncident::childrenEkinPass(double ekincut) {
    bool pass = false; // true if cut passed
    // as soon as at a particle passes the cut -> end loop and return true
    for ( unsigned short i=0; !(pass && m_passWholeVertex) && (i<m_numChildren); ++i) {
      bool thispassed = (childEkin(i) >= ekincut);
      if(thispassed) { setChildPassedFilters(i); }
      pass |= thispassed;
    }
    return pass;
  }

  void ISF::ITruthIncident::setChildPassedFilters(unsigned short index) {
    m_childPassedFilters[index] = true;
    return;
  }

  bool ISF::ITruthIncident::childPassedFilters(unsigned short index) const {
    return m_childPassedFilters[index];
  }

  void ISF::ITruthIncident::setPassWholeVertices(bool passWholeVertex) {
    m_passWholeVertex=passWholeVertex;
  }

}

#endif //> !ISF_EVENT_ITRUTHINCIDENT_H
