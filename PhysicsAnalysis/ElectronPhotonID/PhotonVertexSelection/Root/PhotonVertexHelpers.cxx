/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "PhotonVertexSelection/PhotonVertexHelpers.h"

// EDM includes
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Asg tools
#include "IsolationCorrections/ShowerDepthTool.h"

// Messaging
#define ATH_MSG_WARNING( MSG ) \
  std::cout << "PhotonVertexHelpers" << std::string( 21, ' ' ) \
            << "WARNING " << MSG << std::endl;


#define CHECK_AND_RETURN( METHODNAME, EXP, RETURNVALUE ) {         \
  if ( EXP )                                                       \
  {                                                                \
    ATH_MSG_WARNING(METHODNAME << ": (" << #EXP                    \
                    << "), returning " << #RETURNVALUE);           \
    return RETURNVALUE;                                            \
  }                                                                \
}

namespace xAOD {
  namespace PVHelpers {

    //__________________________________________________________________________
    const xAOD::Vertex* getHardestVertex(const xAOD::VertexContainer *vertices)
    {
      // Check PV collection
      CHECK_AND_RETURN("getHardestVertex", vertices == nullptr, nullptr);

      // Check for primary vertex in collection
      for (auto vertex: *vertices) {
        if (vertex->vertexType() == xAOD::VxType::VertexType::PriVtx)
          return vertex;
      }
      
      // Couldn't find one
      ATH_MSG_WARNING( "getHardestVertex(): "
                       << "Couldn't find a primary vertex, returning nullptr." );
      return nullptr;
    }

    //__________________________________________________________________________
    std::pair<float, float> getZCommonAndError(const xAOD::EventInfo *eventInfo,
                                               const xAOD::EgammaContainer *egammas,
                                               float convPtCut)
    {
      // Static accessors
      static SG::AuxElement::Accessor<float> zvertex("zvertex");
      static SG::AuxElement::Accessor<float> errz("errz");
      static SG::AuxElement::Accessor<float> HPV_zvertex("HPV_zvertex");
      static SG::AuxElement::Accessor<float> HPV_errz("HPV_errz");

      // Clear values
      float zCommon = 0.0, zCommonError = 0.0;

      // Check eventInfo
      CHECK_AND_RETURN("getZCommonAndError", eventInfo == nullptr, std::make_pair(0.0, 0.0));

      // Beam position is the base for zCommon weighted average
      double beamPosZ      = eventInfo->beamPosZ();
      double beamPosSigmaZ = eventInfo->beamPosSigmaZ();

      zCommon       = beamPosZ/beamPosSigmaZ/beamPosSigmaZ;
      zCommonError  = 1.0     /beamPosSigmaZ/beamPosSigmaZ;

      // Include z-position pointed at by egammas
      for (const xAOD::Egamma* egamma: *egammas) {
        if (egamma == nullptr) {
          ATH_MSG_WARNING("Null pointer to egamma object, excluding from ZCommon calculation");
          continue;
        }

        const xAOD::Photon *photon = nullptr;
        if (egamma->type() == xAOD::Type::Photon)
          photon = dynamic_cast<const xAOD::Photon*>(egamma);

        if (photon && passConvSelection(photon, convPtCut)) {
          if (!HPV_zvertex.isAvailable(*photon) || !HPV_errz.isAvailable(*photon)) {
            ATH_MSG_WARNING( "setZCommon: Skipping photon not decorated with " <<
                             "HPV_zvertex/HPV_errz information, FIX THIS!" );
            continue;
          }
          if (HPV_errz(*photon) == 0.0) {
            ATH_MSG_WARNING( "setZCommon: HPV_errz == 0, skipping this photon!" );
            continue;
          }
          zCommon      += HPV_zvertex(*photon)/HPV_errz(*photon)/HPV_errz(*photon);
          zCommonError += 1.0                 /HPV_errz(*photon)/HPV_errz(*photon);
        } else {
          if (!zvertex.isAvailable(*egamma) || !errz.isAvailable(*egamma)) {
            ATH_MSG_WARNING( "setZCommon: Skipping photon not decorated with " <<
                             "zvertex/errz information, FIX THIS!" );
            continue;
          }
          if (errz(*egamma) == 0.0) {
            ATH_MSG_WARNING( "setZCommon: errz == 0, skipping this photon!" );
            continue;
          }
          zCommon      += zvertex(*egamma)/errz(*egamma)/errz(*egamma);
          zCommonError += 1.0             /errz(*egamma)/errz(*egamma);
        }
      }

      // Normalize by error (weighted average)
      zCommon     /= zCommonError;
      zCommonError = 1.0/sqrt(zCommonError);

      return std::make_pair(zCommon, zCommonError);
    }
    
    //____________________________________________________________________________
    bool passConvSelection(const xAOD::Vertex& conversionVertex,
                           size_t i,
                           float convPtCut)
    {
        const xAOD::TrackParticle *tp = conversionVertex.trackParticle(i);
        CHECK_AND_RETURN("passConvSelection", tp == nullptr, false);
        
        if (!xAOD::EgammaHelpers::numberOfSiHits(tp)) return false;
        // pt1,pt2 is only set for 2-track vertices
        if (conversionVertex.nTrackParticles() == 1)
          return xAOD::EgammaHelpers::momentumAtVertex(conversionVertex).perp() > convPtCut;
        
        std::string s = Form("pt%lu", i+1);
        return (conversionVertex.isAvailable<float>(s) &&
                conversionVertex.auxdata<float>(s) > convPtCut);
    }

    //____________________________________________________________________________
    bool passConvSelection(const xAOD::Photon *photon,
                           float convPtCut)
    {
      CHECK_AND_RETURN("passConvSelection", photon == nullptr, false);
      const xAOD::Vertex *conversionVertex = photon->vertex();
      if (conversionVertex == nullptr) return false;

      size_t NumberOfTracks = conversionVertex->nTrackParticles();
      for (size_t i = 0; i < NumberOfTracks; ++i) {
        if (passConvSelection(*conversionVertex, i, convPtCut)) return true;
      }

      return false;
    }

    //__________________________________________________________________________
    TLorentzVector getTrackAtFirstMeasurement(const xAOD::TrackParticle *tp)
    {
      CHECK_AND_RETURN("getTrackAtFirstMeasurement", tp == nullptr, TLorentzVector());

      static SG::AuxElement::ConstAccessor<std::vector<float> > accParameterPX("parameterPX");
      static SG::AuxElement::ConstAccessor<std::vector<float> > accParameterPY("parameterPY");
      
      TLorentzVector v;
      v.SetPtEtaPhiM(tp->pt(), tp->eta(), tp->phi(), 0.);
      if (!accParameterPX.isAvailable(*tp) || !accParameterPY.isAvailable(*tp)) {
        ATH_MSG_WARNING("getTrackAtFirstMeasurement(): "
                        << "Could not find first parameter, returning default pt." );
        return v;
      }

      for (unsigned int i = 0; i < accParameterPX(*tp).size(); ++i) {
        if (tp->parameterPosition(i) == xAOD::FirstMeasurement) {
          v.SetPxPyPzE(accParameterPX(*tp)[i], accParameterPY(*tp)[i], 0, 0);
          return v;
        }
      }

      ATH_MSG_WARNING("getTrackAtFirstMeasurement(): "
                      << "Could not find first parameter, returning default pt." );
      return v;
    }

    //__________________________________________________________________________
    TLorentzVector getVertexMomentum(const xAOD::Vertex *vertex, bool useAux /* true */, std::string derivationPrefix /* "" */)
    {
      TLorentzVector v;
      CHECK_AND_RETURN("getVertexMomentum", vertex == nullptr, v);

      SG::AuxElement::ConstAccessor<float> pt (derivationPrefix + "pt" );
      SG::AuxElement::ConstAccessor<float> eta(derivationPrefix + "eta");
      SG::AuxElement::ConstAccessor<float> phi(derivationPrefix + "phi");
      
      if (useAux                   and
          pt .isAvailable(*vertex) and
          eta.isAvailable(*vertex) and
          phi.isAvailable(*vertex) )
      {
        v.SetPtEtaPhiM(pt (*vertex),
                       eta(*vertex),
                       phi(*vertex), 0.0);
        return v;
      }

      // Sum the 4-momenta of all track particles at the vertex
      const xAOD::TrackParticle *tp = nullptr;
      for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
        tp = vertex->trackParticle(i);
        if (tp == nullptr)
        {
          ATH_MSG_WARNING("getVertexMomentum: null pointer to trackParticle." <<
                          " Are you running on a thinned sample?" );
          continue; // protect against thinned tracks
        }
        v += tp->p4();
      }
      return v;
    }

    //__________________________________________________________________________
    float getVertexSumPt(const xAOD::Vertex *vertex, int power, bool useAux /* = true */)
    {
      CHECK_AND_RETURN("getVertexSumPt", vertex == nullptr, 0.0);
      
      std::string pw = (power == 1) ? "sumPt" : Form("sumPt%d", power);
      if (useAux and vertex->isAvailable<float>(pw))
        return vertex->auxdata<float>(pw);
    
      // Loop over all track particles, sum up their pt
      float pt = 0.0;
      const xAOD::TrackParticle *tp = nullptr;
      for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
        tp = vertex->trackParticle(i);
        if (tp == nullptr) continue; // protect against slimmed tracks

        pt += pow(tp->pt()/1e3, power);
      }
      return pt;
    }

  } // namespace PVHelpers
} // namespace xAOD
