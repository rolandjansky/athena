/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexBeamCondPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BEAMEFFECTS_CRABKISSINGVERTEXPOSITIONER_H
#define BEAMEFFECTS_CRABKISSINGVERTEXPOSITIONER_H 1

// STL includes
#include <string>
#include <utility>
#include <vector>
#include <map>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "HepMC_Interfaces/ILorentzVectorGenerator.h"

// InDetBeamSpotService
#include "InDetBeamSpotService/IBeamCondSvc.h"

// Forward declarations
namespace CLHEP {
    class HepRandomEngine;
}

namespace Simulation
{

  /** @class CrabKissingVertexPositioner

      This AthenaTool computes geometrical shifts for the initial GenEvent vertices.

      based on function from S.Fartoukh Phys.Rev.ST Accel.Beams 17 (2014) no.11, 111001

      @author John.Chapman -at- cern.ch, Elmar.Ritsch -at- cern.ch, Daniele.Ruini -at- poltechnique.edu
     */
  class CrabKissingVertexPositioner : public extends<AthAlgTool, ILorentzVectorGenerator> {

    public:

      /** Constructor with parameters */
      CrabKissingVertexPositioner( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~CrabKissingVertexPositioner();

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector  *generate() const override final;

    private:

      inline double heaviside(double val) const {return (val >= 0.0) ? 1.0 : 0.0;};
      double getDisplacement(double bunchSize, double angle1, double angle2) const;
      double beamspotFunction(double displacement, double angle1, double angle2) const;
      ServiceHandle<IBeamCondSvc>     m_beamCondSvc;
      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;
      std::string                     m_randomEngineName;         //!< Name of the random number stream

      StringProperty m_bunchShapeProp;
      void BunchShapeHandler(Property&);
      enum BunchShape{GAUSS,FLAT,NSHAPES};
      BunchShape m_bunchShape; // GAUSS or FLAT
      double m_bunchLength; //!< Parameter in the Z distribution of the beamspot
      /// parameters according to S.Fartoukh Phys.Rev.ST Accel.Beams 17 (2014) no.11, 111001 ----------------------------
      double m_betaStar; // beta* in the parallel (kissing) plane, we assume betax=betay, units: mm
      double m_epsilon; // Normalized emittance, unit: mm
      double m_alphaPar; // Kissing angle
      double m_alphaX;
      double m_thetaX;
      // ----------------------------------------------------------------------------------------------------------------
  };

}

#endif //> !BEAMEFFECTS_CRABKISSINGVERTEXPOSITIONER_H
