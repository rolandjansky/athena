/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "AthenaKernel/IAthRNGSvc.h"
#include "HepMC_Interfaces/ILorentzVectorGenerator.h"

// InDetBeamSpotService
#include "BeamSpotConditionsData/BeamSpotData.h"

// Forward declarations
namespace ATHRNG {
  class RNGWrapper;
}
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
     virtual ~CrabKissingVertexPositioner() = default;

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector  *generate() const override final;

    private:

      inline double heaviside(double val) const {return (val >= 0.0) ? 1.0 : 0.0;};
      double getDisplacement(double bunchSize, double angle1, double angle2,
                             CLHEP::HepRandomEngine* rng) const;
      double beamspotFunction(double displacement, double angle1, double angle2) const;

      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      ServiceHandle<IAthRNGSvc>   m_rndGenSvc{this, "RandomSvc", "AthRNGSvc"};
      ATHRNG::RNGWrapper*             m_randomEngine{};             //!< Slot-local RNG
      Gaudi::Property<std::string>     m_randomEngineName{this, "RandomStream", "VERTEX", "Name of the random number stream"};
      Gaudi::Property<std::string> m_bunchShapeProp{this, "BunchShape", "GAUSS", "GAUSS or FLAT"};
      void BunchShapeHandler(Gaudi::Details::PropertyBase&);
      enum BunchShape{GAUSS,FLAT,NSHAPES};
      BunchShape m_bunchShape{BunchShape::GAUSS}; // GAUSS or FLAT
      Gaudi::Property<double> m_bunchLength{this, "BunchLength", 75., "75.0 mm"}; //!< Parameter in the Z distribution of the beamspot
      /// parameters according to S.Fartoukh Phys.Rev.ST Accel.Beams 17 (2014) no.11, 111001 ----------------------------
      Gaudi::Property<double> m_betaStar{this, "BetaStar", 150., "beta* in the parallel (kissing) plane, we assume betax=betay, units: mm"};
      Gaudi::Property<double> m_epsilon{this, "Epsilon", 2.5e-3, "Normalized emittance, unit: mm"};
      Gaudi::Property<double> m_alphaPar{this, "AlfasParallel", 0., "Kissing angle (Radians)"}; // Kissing angle
      Gaudi::Property<double> m_alphaX{this, "AlfaX", 295e-6};
      Gaudi::Property<double> m_thetaX{this, "ThetaX", 295e-6};
      // ----------------------------------------------------------------------------------------------------------------
  };

}

#endif //> !BEAMEFFECTS_CRABKISSINGVERTEXPOSITIONER_H
