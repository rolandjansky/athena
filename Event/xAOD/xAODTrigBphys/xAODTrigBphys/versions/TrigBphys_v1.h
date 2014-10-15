// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphys_v1.h 621909 2014-10-15 13:48:56Z jwalder $
#ifndef XAODTRIGBPHYS_VERSIONS_TRIGBPHYS_V1_H
#define XAODTRIGBPHYS_VERSIONS_TRIGBPHYS_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// athena includes
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"

namespace xAOD {

    // forward declarations, so that we can use TrigBphysContainer_v1 in TrigBphys_v1
    class TrigBphys_v1;
    typedef DataVector< xAOD::TrigBphys_v1 > TrigBphysContainer_v1;

    /// Class describing a Bphysics online composite object
    ///
    /// This class describes the properties of BPhysics composite object
    /// in the xAOD format.
    ///
    /// @author James Walder <jwalder@cern.ch>
    /// Based on the previous TrigL2Bphys and TrigEFBphys classes
    ///
    /// $Revision: 621909 $
    /// $Date: 2014-10-15 15:48:56 +0200 (Wed, 15 Oct 2014) $
    ///
    class TrigBphys_v1 : public SG::AuxElement {

    public:
        /** enum for different particle types */
        enum pType {
           PHIKK     = 0,
           DSPHIPI   = 1,
           BMUMU     = 2,
           BMUMUX    = 3,
           JPSIEE    = 4,
           JPSIMUMU  = 5,
           MULTIMU   = 6,
           BKMUMU    = 7,
           BDKSTMUMU = 8,
           BSPHIMUMU = 9,
           LBLMUMU   = 10,
           BCDSMUMU  = 11,
           UNKNOWNPTYPE = 9999
        };

        /** enum to describe the level this object is operating at */
        enum levelType{
           UNKOWNLEVEL = 0,
           L2          = 2,
           EF          = 3,
           HLT         = 4,
           LASTLEVEL
        };

        /// Default constructor
        TrigBphys_v1();

        //**clone method, essentially a replacement for a copy constructor*/
        void clone(const TrigBphys_v1 &rhs);
        
        //** initialise with defaults. Attempt to call this after creating a particle */
        void initialise();

        //** initialise with given parameters */
        void initialise(uint32_t roi, float eta, float phi, pType particleType,
                        float mass, levelType level);
        /** constructor replacement for particle of type "particleType" with mass "mass" 
         and a secondary decay (given as ElementLink to another TrigEFBphys) */
        void initialise(uint32_t roi, float eta, float phi, pType particleType,
                        float mass, const ElementLink<xAOD::TrigBphysContainer_v1>& sDecay,
                        levelType level);

        bool operator==(const TrigBphys_v1& rhs) const;
        bool operator!=(const TrigBphys_v1& rhs) const {return !((*this)==rhs);}

        /** accessor method:  ID of L1 RoI */
        uint32_t roiId()    const;
        /** accessor method: particle Type */
        pType particleType() const;
        /** accessor method: level Type */
        levelType level() const;
        /** accessor method: eta */
        float eta()      const;
        /** accessor method: phi */
        float phi()      const;
        /** accessor method: mass */
        float mass()      const;
        /** accessor method: mass from vertex fit */
        float fitmass()  const;
        /** accessor method: chi2 from vertex fit */
        float fitchi2()  const;
        /** accessor method: ndof from vertex fit */
        int fitndof()  const;
        /** accessor method: x position of vertex */
        float fitx()  const;
        /** accessor method: y position of vertex */
        float fity()  const;
        /** accessor method: z position of vertex */
        float fitz()  const;

        /** accessor method: secondary decay particle */
        const TrigBphys_v1* secondaryDecay() const;
        /** accessor method: secondary decay particle */
        const ElementLink< TrigBphysContainer_v1 >& secondaryDecayLink() const;

        /** accessor method:vector of tracks used to make particle */
        const std::vector< ElementLink< xAOD::TrackParticleContainer > >&
        trackParticleLinks() const;
        /** Set the track particle links on the object */
        void setTrackParticleLinks(
           const std::vector< ElementLink< TrackParticleContainer > >& links );
        /** Number of tracks used to make particle */
        size_t nTrackParticles() const;
        /** Get the Nth track's pointer */
        const xAOD::TrackParticle* trackParticle( size_t i ) const;

        /** Set the pseudorapidity of the object */
        void setEta (float);
        /** Set the azimuth angle of the object */
        void setPhi (float);
        /** Set the mass of the object */
        void setMass(float);

        /** set method: roiId **/
        void setRoiId(uint32_t id);
        /** set method: particle type **/
        void setParticleType(pType type);
        /** set method: level type **/
        void setLevel(levelType type);
        /** set method: mass from vertex fit **/
        void setFitmass(float FitMass);
        /** set method: chi2 from vertex fit **/
        void setFitchi2(float FitChi2);
        /** set method: ndof from vertex fit **/
        void setFitndof(int FitNdof);
        /** set method: x position of vertex  **/
        void setFitx(float FitX);
        /** set method: y position of vertex  **/
        void setFity(float FitY);
        /** set method: z position of vertex  **/
        void setFitz(float FitZ);
        /** set method: add track to particle  **/
        void addTrackParticleLink(const ElementLink<xAOD::TrackParticleContainer>& track);
        /** set method: link to secondary decay particle  **/
        void setSecondaryDecayLink(const ElementLink<xAOD::TrigBphysContainer_v1> & link);

        
    private:
        //** Copy constructor */
        TrigBphys_v1(const TrigBphys_v1 & rhs);

    }; // TrigBphys

} // namespace AOD

#endif // XAODTRIGBPHYS_VERSIONS_TRIGBPHYS_V1_H
