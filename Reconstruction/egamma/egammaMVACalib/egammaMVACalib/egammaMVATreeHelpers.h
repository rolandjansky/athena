/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAMVACALIB_EGAMMAMVATREEHELPERS
#define EGAMMAMVACALIB_EGAMMAMVATREEHELPERS

#include <functional>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>
#include <cstdint>
#include <numeric>

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <TTree.h>
#include <TLorentzVector.h>

#include <AsgTools/AsgMessaging.h>

#include <egammaMVACalib/egammaMVALayerDepth.h>



/**
 * These functions are for calculating variables used by the
 * MVA calibration
 **/

namespace egammaMVATreeHelpers
{
  // inline functions to avoid duplicates problem during linking (and performance)
  // cluster functions
  // REMEMBER to add the functions using corrected layer energies
  inline float compute_cl_eta(const xAOD::CaloCluster& cluster) { return cluster.eta(); }
  inline float compute_cl_phi(const xAOD::CaloCluster& cluster) { return cluster.phi(); }
  inline float compute_cl_e(const xAOD::CaloCluster& cluster) { return cluster.e(); }
  inline float compute_cl_etaCalo(const xAOD::CaloCluster& cluster) {
    double tmp = 0.;
    if(! (cluster.retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, tmp))) {
      throw std::runtime_error("etaCalo not found in CaloCluster object");
    }
    return tmp;
  }
  inline float compute_cl_phiCalo(const xAOD::CaloCluster& cluster) {
    double tmp = 0.;
    if(! (cluster.retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, tmp))) {
      throw std::runtime_error("phiCalo not found in CaloCluster object");
    }
    return tmp;
  }
  inline float compute_cl_etas1(const xAOD::CaloCluster& cluster) { return cluster.etaBE(1); }
  inline float compute_cl_etas2(const xAOD::CaloCluster& cluster) { return cluster.etaBE(2); }
  inline float compute_rawcl_Es0(const xAOD::CaloCluster& cl) { return cl.energyBE(0); }
  /*inline std::function<float(const xAOD::CaloCluster&)> compute_rawcl_Es0_auto(bool use_corrected)
  {
      if (use_corrected) return [](const xAOD::CaloCluster& cl) { return cl.energyBE(0); };
      else return [](const xAOD::CaloCluster& cl) { return cl.energyBE(0); };
  }*/
  inline float compute_rawcl_Es1(const xAOD::CaloCluster& cl) { return cl.energyBE(1); }
  inline float compute_rawcl_Es2(const xAOD::CaloCluster& cl) { return cl.energyBE(2); }
  inline float compute_rawcl_Es3(const xAOD::CaloCluster& cl) { return cl.energyBE(3); }

  inline float compute_correctedcl_Es0(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es0") ? cl.auxdataConst<double>("correctedcl_Es0") : cl.energyBE(0); }
  inline float compute_correctedcl_Es1(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es1") ? cl.auxdataConst<double>("correctedcl_Es1") : cl.energyBE(1); }
  inline float compute_correctedcl_Es2(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es2") ? cl.auxdataConst<double>("correctedcl_Es2") : cl.energyBE(2); }
  inline float compute_correctedcl_Es3(const xAOD::CaloCluster& cl) { return cl.isAvailable<double>("correctedcl_Es3") ? cl.auxdataConst<double>("correctedcl_Es3") : cl.energyBE(3); }

  inline float compute_rawcl_Eacc(const xAOD::CaloCluster& cl) { return cl.energyBE(1) + cl.energyBE(2) + cl.energyBE(3); }
  inline float compute_rawcl_f0(const xAOD::CaloCluster& cl) { return cl.energyBE(0) / (cl.energyBE(1) + cl.energyBE(2) + cl.energyBE(3)); }

  inline float compute_correctedcl_Eacc(const xAOD::CaloCluster& cl) { return compute_correctedcl_Es1(cl) + compute_correctedcl_Es2(cl) + compute_correctedcl_Es3(cl); }
  inline float compute_correctedcl_f0(const xAOD::CaloCluster& cl) { return compute_correctedcl_Es0(cl) / (compute_correctedcl_Eacc(cl)); }


  inline float compute_calibHitsShowerDepth(const std::array<float, 4>& cl, float eta)
  {
    const std::array<float, 4> radius(get_MVAradius(eta));

    // loop unrolling
    const float denominator = cl[0] + cl[1] + cl[2] + cl[3];
    if (denominator == 0) return 0.;

    return (radius[0] * cl[0]
          + radius[1] * cl[1]
          + radius[2] * cl[2]
          + radius[3] * cl[3]) / denominator;
  }

  inline float compute_rawcl_calibHitsShowerDepth(const xAOD::CaloCluster& cl)
  {
      const std::array<float, 4> cluster_array {{ compute_rawcl_Es0(cl),
                                                  compute_rawcl_Es1(cl),
                                                  compute_rawcl_Es2(cl),
                                                  compute_rawcl_Es3(cl) }};
      return compute_calibHitsShowerDepth(cluster_array, compute_cl_eta(cl));
  }

  inline float compute_correctedcl_calibHitsShowerDepth(const xAOD::CaloCluster& cl) {
    const std::array<float, 4> cluster_array {{ compute_correctedcl_Es0(cl),
                                                compute_correctedcl_Es1(cl),
                                                compute_correctedcl_Es2(cl),
                                                compute_correctedcl_Es3(cl) }};
    return compute_calibHitsShowerDepth(cluster_array, compute_cl_eta(cl));
  }




  // electron functions
  inline float compute_el_charge(const xAOD::Electron& el) { return el.charge(); }
  inline float compute_el_tracketa(const xAOD::Electron& el) { return el.trackParticle()->eta(); }
  inline float compute_el_trackpt(const xAOD::Electron& el) { return el.trackParticle()->pt(); }
  inline float compute_el_trackz0(const xAOD::Electron& el) { return el.trackParticle()->z0(); }
  inline float compute_el_refittedTrack_qoverp(const xAOD::Electron& el) { return el.trackParticle()->qOverP(); }
  inline int compute_el_author(const xAOD::Electron& el) { return el.auxdata<unsigned short int>("author"); }


  // photon functions
  inline int compute_ph_convFlag(const xAOD::Photon& ph) {
    const auto original = xAOD::EgammaHelpers::conversionType(&ph);
    if (original == 3) return 2;
    else if (original != 0) return 1;
    else return original;
  }

  // a utility function
  inline float getPtAtFirstMeasurement(const xAOD::TrackParticle* tp)
  {
    if (!tp) return 0;
    for (unsigned int i = 0; i < tp->numberOfParameters(); ++i) {
      if (tp->parameterPosition(i) == xAOD::FirstMeasurement) {
	return hypot(tp->parameterPX(i), tp->parameterPY(i));
      }
    }
    return tp->pt();
  }

  // define a few without using conversion helper
  inline float compute_ptconv(const xAOD::Photon* ph)
  {
    static const SG::AuxElement::Accessor<float> accPx("px");
    static const SG::AuxElement::Accessor<float> accPy("py");
    
    auto vx = ph->vertex();
    return vx ? std::hypot(accPx(*vx), accPy(*vx)) : 0.0;
  }

  inline float compute_pt1conv(const xAOD::Photon* ph)
  {
    static const SG::AuxElement::Accessor<float> accPt1("pt1");
    
    const xAOD::Vertex* vx = ph->vertex();
    if (!vx) return 0.0;
    if (accPt1.isAvailable(*vx)) {
      return accPt1(*vx);
    } else {
      return getPtAtFirstMeasurement(vx->trackParticle(0));
    }
  }

  inline float compute_pt2conv(const xAOD::Photon* ph)
  {
    static const SG::AuxElement::Accessor<float> accPt2("pt2");
    
    const xAOD::Vertex* vx = ph->vertex();
    if (!vx) return 0.0;
    if (accPt2.isAvailable(*vx)) {
      return accPt2(*vx);
    } else {
      return getPtAtFirstMeasurement(vx->trackParticle(1));
    }
  }

  struct ConversionHelper
  {
    ConversionHelper(const xAOD::Photon* ph)
      : m_vertex(ph ? ph->vertex() : nullptr),
        m_tp0(m_vertex ? m_vertex->trackParticle(0) : nullptr),
        m_tp1(m_vertex ? m_vertex->trackParticle(1) : nullptr),
        m_pt1conv(0.), m_pt2conv(0.)
    {
      static asg::AsgMessaging static_msg("ConversionHelper");
      static_msg.msg(MSG::DEBUG) << "init conversion helper";
      if (!m_vertex) return;

      static SG::AuxElement::Accessor<float> accPt1("pt1");
      static SG::AuxElement::Accessor<float> accPt2("pt2");
      if (accPt1.isAvailable(*m_vertex) && accPt2.isAvailable(*m_vertex))
      {
        m_pt1conv = accPt1(*m_vertex);
        m_pt2conv = accPt2(*m_vertex);
      }
      else
      {
        static_msg.msg(MSG::WARNING) << "pt1/pt2 not available, will approximate from first measurements";
        m_pt1conv = getPtAtFirstMeasurement(m_tp0);
        m_pt2conv = getPtAtFirstMeasurement(m_tp1);
      }
    }

    ConversionHelper(const xAOD::Photon& ph)
      : ConversionHelper(&ph) { }  // delegating constr

    inline float ph_Rconv() const { return m_vertex ? hypot(m_vertex->position().x(), m_vertex->position().y()) : 0; }
    inline float ph_zconv() const { return m_vertex ? m_vertex->position().z() : 0.; }
    inline int ph_convtrk1nPixHits() const {
      if (!m_tp0) { return 0; }
      uint8_t hits = 0;
      if (m_tp0->summaryValue(hits, xAOD::numberOfPixelHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfPixelHits";
        return 0;
      }
    }
    inline int ph_convtrk2nPixHits() const {
      if (!m_tp1) return 0;
      uint8_t hits;
      if (m_tp1->summaryValue(hits, xAOD::numberOfPixelHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfPixelHits";
        return 0;
      }
    }
    inline int ph_convtrk1nSCTHits() const {
      if (!m_tp0) { return 0; }
      uint8_t hits;
      if (m_tp0->summaryValue(hits, xAOD::numberOfSCTHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfSCTHits";
        return 0;
      }
    }
    inline int ph_convtrk2nSCTHits() const {
      if (!m_tp1) { return 0; }
      uint8_t hits;
      if (m_tp1->summaryValue(hits, xAOD::numberOfSCTHits)) { return hits; }
      else {
        static asg::AsgMessaging static_msg("ConversionHelper");
        static_msg.msg(MSG::WARNING) << "cannot read xAOD::numberOfSCTHits";
        return 0;
      }
    }
    inline float ph_pt1conv() const { return m_pt1conv; }
    inline float ph_pt2conv() const { return m_pt2conv; }
    inline float ph_ptconv() const {
      // TODO: evaluate if move to this new definition, by now keep the previous one
      // to be consistent with the training
      // return m_vertex ? xAOD::EgammaHelpers::momentumAtVertex(*m_vertex).perp() : 0.;
      TLorentzVector sum;
      if (m_tp0) sum += m_tp0->p4();
      if (m_tp1) sum += m_tp1->p4();
      return sum.Perp();
    }
  private:
    const xAOD::Vertex* m_vertex;
    const xAOD::TrackParticle* m_tp0;
    const xAOD::TrackParticle* m_tp1;
    float m_pt1conv, m_pt2conv;
  };
}

#endif
