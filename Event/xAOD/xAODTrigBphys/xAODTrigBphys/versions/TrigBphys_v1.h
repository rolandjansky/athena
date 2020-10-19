// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphys_v1.h 696633 2015-09-24 14:59:50Z jwalder $
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
    /// $Revision: 696633 $
    /// $Date: 2015-09-24 16:59:50 +0200 (Thu, 24 Sep 2015) $
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
            // added JW: 21/09/15 for 2016 edm running
            LAMBDAPIP = 12, // Lambda^0 -> pi- p
            BCDPMUMU  = 13, // B_c -> D+ mu mu
            BCDSTMUMU = 14, // B_c -> D*+ mu mu
            OMEGALAMK = 15, // Omega- -> Lambda^0 K-
            XILAMPI   = 16, // Xi- -> Lambda^0 pi-
            DSTDZPI   = 17, // D*+ -> D0 pi+
            DZMUKX    = 18, // D0 -> mu+ K- X
            DZKPI     = 19, // D0 -> K- pi+
            DPKPIPI   = 20, // D+ -> K- pi+ pi+
            BCPIMUMU  = 21, // B_c+ -> pi+ mu mu
            LBPQMUMU  = 22, // Lambda_b0 -> P_c+ (-> J/psi p) K-, pentaquark states

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
        void initialise(uint32_t roi, float eta, float phi, float pt,
                        pType particleType,
                        float mass, levelType level);
        void initialise(uint32_t roi, float eta, float phi, pType particleType,
                        float mass, levelType level);
        /** constructor replacement for particle of type "particleType" with mass "mass" 
         and a secondary decay (given as ElementLink to another TrigEFBphys) */
        void initialise(uint32_t roi, float eta, float phi, float pt,
                        pType particleType,
                        float mass, const ElementLink<xAOD::TrigBphysContainer_v1>& sDecay,
                        levelType level);
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
        /** accessor method: pt */
        float pt()      const;
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

        /** accessor method: lxy  */
        float lxy     () const;
        /** accessor method: lxy uncertainty */
        float lxyError() const;
        /** accessor method: tau  */
        float tau     () const;
        /** accessor method: tau uncertainty */
        float tauError() const;
        /** accessor method: fitmass uncertainty */
        float fitmassError() const;
        
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
        /** Set the pT of the object */
        void setPt(float);

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

        /** set method: lxy  **/
        void setLxy     (float v);
        /** set method: lxyError  **/
        void setLxyError(float v);
        /** set method: tau  **/
        void setTau     (float v);
        /** set method: tauError  **/
        void setTauError(float v);
        /** set method: fitmassError  **/
        void setFitmassError(float v);

        
        /** accessor method:vector of tracks used to make particle */
        const std::vector< uint32_t > & vecRoiIds() const;
        /** Set the track particle links on the object */
        void setVecRoiIds(const std::vector< uint32_t >& roiIds );
        /** Number of tracks used to make particle */
        size_t nVecRoiIds() const;
        /** Get the Nth roiId */
        uint32_t vecRoiId( size_t i ) const;
        void addVecRoiId(uint32_t roiId);

        
        
        /** accessor method: lowerChain decay particle */
        const TrigBphys_v1* lowerChain() const;
        /** accessor method: lowerChain decay particle */
        const ElementLink< TrigBphysContainer_v1 >& lowerChainLink() const;
        /** set method: link to lowerChain decay particle  **/
        void setLowerChainLink(const ElementLink<xAOD::TrigBphysContainer_v1> & link);
        
        /** accessor method:vector of tracks used to make particle */
        const std::vector< ElementLink< xAOD::IParticleContainer > >&
        particleLinks() const;
        /** Set the track particle links on the object */
        void setParticleLinks(const std::vector< ElementLink< IParticleContainer > >& links );
        /** Number of tracks used to make particle */
        size_t nParticles() const;
        /** Get the Nth track's pointer */
        const xAOD::IParticle* particle( size_t i ) const;
        void addParticleLink(const ElementLink<xAOD::IParticleContainer>& particle);
        
    private:
        //** Copy constructor */
        TrigBphys_v1(const TrigBphys_v1 & rhs) = delete;

    }; // TrigBphys

} // namespace AOD

#endif // XAODTRIGBPHYS_VERSIONS_TRIGBPHYS_V1_H
