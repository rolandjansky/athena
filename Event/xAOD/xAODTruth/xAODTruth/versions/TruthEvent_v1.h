// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHEVENT_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENT_V1_H

#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/ObjectType.h"

#include "xAODTruth/TruthEventBase.h"

#include <utility>

namespace xAOD {


  /// Class describing a signal truth event in the MC record
  ///
  /// @author Jovan Mitervski <Jovan.Mitrevski@cern.ch>
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  ///
  class TruthEvent_v1 : public TruthEventBase_v1 {
  public:

    /// Default constructor
    TruthEvent_v1();

    /// Signal process ID code
    /// @note Not guaranteed to be filled, or unique!
    /// @todo Unreliable and not obviously meaningful: remove?
    //int signalProcessId() const;

    /// Event number from the generator
    /// @note Not guaranteed to be filled, or unique!
    /// @todo Unreliable and not obviously meaningful: remove?
    //int eventNumber() const;

    /// Pointer to a vertex representing the hard process
    /// @deprecated This is not reliable or physically meaningful -- only for interpretation
    ///   as an interation *position*, for which you should prefer to use eventVertex()
    const TruthVertex* signalProcessVertex() const;

    /// Pointer to a vertex representing the primary beam interaction point
    /// @todo Make this the canonical one and drop signalProcessVertex
    const TruthVertex* eventVertex() const { return signalProcessVertex(); }

    /// Test to see if we have two valid beam particles
    // bool validBeamParticles() const;
    /// Pair of pointers to the two incoming beam particles
    std::pair<const TruthParticle*,const TruthParticle*> beamParticles() const;

    /// Const access to the weights vector
    const std::vector<float>& weights() const;
    // std::vector<float>& weights();
    /// @todo Need to add the map-like interface for the weights: very important!

    /// Get the cross section
    float crossSection() const;
    /// Get the cross section error
    float crossSectionError() const;


    /// @name Parton density info (optional due to particle gun & NLO events where a single PDF info doesn't work)
    //@{

    /// Accessor enums for PDF info parameter lookup
    enum PdfParam {
      PDGID1 = 0,
      PDGID2 = 1,
      PDFID1 = 2,
      PDFID2 = 3,
      X1 = 4,
      X2 = 5,
      SCALE = 6,
      Q = 6,
      PDF1 = 7,
      PDF2 = 8,
      XF1 = 7,
      XF2 = 8
    };

    /// Accessor for PDF parameters
    bool pdfInfoParameter(int& value, PdfParam parameter) const;
    /// Accessor for PDF parameters
    bool pdfInfoParameter(float& value, PdfParam parameter) const;


    /// Convenient holder of a full set of PDF info
    struct PdfInfo {
      /// Constructor to set (invalid) defaults
      /// @todo Use C++11 inline member init when allowed
      PdfInfo() :
        pdgId1(-1), pdgId2(-1), pdfId1(-1), pdfId2(-1),
        x1(-1), x2(-1), Q(-1), xf1(-1), xf2(-1) { }

      bool valid() const {
        return \
          pdgId1 > 0 && pdgId2 > 0 && pdfId1 > 0 && pdfId2 > 0 &&
          x1 > 0 && x2 > 0 && Q > 0 && xf1 > 0 && xf2 > 0;
      }

      int pdgId1;
      int pdgId2;
      int pdfId1;
      int pdfId2;
      float x1;
      float x2;
      float Q;
      float xf1;
      float xf2;
    };

    // Retrieve a full PdfInfo with a single call
    /// @note May have invalid values -- use valid() to check.
    PdfInfo pdfInfo() const {
      PdfInfo rtn;
      pdfInfoParameter(rtn.pdgId1, PDGID1);
      pdfInfoParameter(rtn.pdgId2, PDGID2);
      pdfInfoParameter(rtn.pdfId1, PDFID1);
      pdfInfoParameter(rtn.pdfId2, PDFID2);
      pdfInfoParameter(rtn.x1, X1);
      pdfInfoParameter(rtn.x2, X2);
      pdfInfoParameter(rtn.Q, Q);
      pdfInfoParameter(rtn.xf1, XF1);
      pdfInfoParameter(rtn.xf1, XF1);
      return rtn;
    }

    //@}


    /// @name Heavy ion info
    //@{

    /// Heavy ion parameter enum
    enum HIParam {
      NCOLLHARD = 0,
      NPARTPROJ = 1,
      NPARTTARG = 2,
      NCOLL = 3,
      SPECTATORNEUTRONS = 4,
      SPECTATORPROTONS = 5,
      NNWOUNDEDCOLLISIONS = 6,
      NWOUNDEDNCOLLISIONS = 7,
      NWOUNDEDNWOUNDEDCOLLISIONS = 8,
      IMPACTPARAMETER = 9,
      EVENTPLANEANGLE = 10,
      ECCENTRICITY = 11,
      SIGMAINELNN = 12,
      CENTRALITY = 13,
    };

    /// Accessor for HI parameters
    bool heavyIonParameter(int& value, HIParam parameter) const;
    /// Accessor for HI parameters
    bool heavyIonParameter(float& value, HIParam parameter) const;

    // /// Convenient holder of a full set of HI info
    // struct HeavyIonInfo {
    //   int nCollHard;
    //   int nPartProj;
    //   int nPartTarg;
    //   int nColl;
    //   int spectatorNeutrons;
    //   int spectatorProtons;
    //   int nnWoundedCollisions;
    //   int nWoundednCollisions;
    //   int nWoundednWoundedCollisions;
    //   float impactParameter;
    //   float eventPlaneAngle;
    //   float eccentricity;
    //   float sigMainELNN; //< @todo Correct name?
    //   float centrality;
    // };

    // // Retrieve a full HeavyIonInfo with a single call
    // HeavyIonInfo heavyIonInfo() const {
    //   HeavyIonInfo rtn;
    //   heavyIonParameter(rtn.nCollHard, NCOLLHARD);
    //   // ...
    //   return rtn;
    // }

    //@}


    /// @name Modifiers
    //@{

    /// Set signal process ID code
    //void setSignalProcessId(int id);

    /// Set the generator event number
    //void setEventNumber(int eventno);

    /// Set pointer to the vertex containing the signal process
    /// @deprecated This is physically dubious, and practically unreliable. Use eventVertex()
    void setSignalProcessVertexLink(const ElementLink<TruthVertexContainer>& link);

    /// Set pointer to a vertex representing the primary beam interaction point
    /// @todo Make this the canonical one and drop signalProcessVertex
    void setEventVertex(const ElementLink<TruthVertexContainer>& link) { setSignalProcessVertexLink(link); }

    /// Set incoming beam particles
    void setBeamParticleLinks(const ElementLink<TruthParticleContainer>& pcl1,
                              const ElementLink<TruthParticleContainer>& pcl2);

    /// Set one incoming beam particle
    void setBeamParticle1Link(const ElementLink<TruthParticleContainer>& pcl1);
    /// Set one incoming beam particle
    void setBeamParticle2Link(const ElementLink<TruthParticleContainer>& pcl2);

    /// Set the event weights
    /// @todo Need to add named weight access: vector<string>
    void setWeights(const std::vector<float>& weights);

    /// Set the cross-section
    void setCrossSection(float);
    /// Set the cross-section error
    void setCrossSectionError(float);
    /// Set the cross-section and error
    void setCrossSection(float xs, float err) {
      setCrossSection(xs);
      setCrossSectionError(err);
    }

    /// Set PdfInfo parameter values
    void setPdfParameter(float value, PdfParam parameter);
    void setPdfParameter(int value, PdfParam parameter);

    /// Set HI parameter values
    void setHeavyIonParameter(float value, HIParam parameter);
    void setHeavyIonParameter(int value, HIParam parameter);

    //@}


    /// The type of the object as a simple enumeration
    Type::ObjectType type() const;

    /// Function making sure that the object is ready for persistification
    void toPersistent();

  };


}

#include "xAODCore/BaseInfo.h"
SG_BASE (xAOD::TruthEvent_v1, xAOD::TruthEventBase_v1);

#endif
