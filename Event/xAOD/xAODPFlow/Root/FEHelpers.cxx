/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Author: Bill Balunas <bill.balunas@cern.ch>

#include "xAODPFlow/FEHelpers.h"

namespace FEHelpers {

  TLorentzVector getVertexCorrectedFourVec(const xAOD::FlowElement& fe, const xAOD::Vertex& vertexToCorrectTo){
    TVector3 theVertexVector(vertexToCorrectTo.x(), vertexToCorrectTo.y(), vertexToCorrectTo.z());
    return getVertexCorrectedFourVec(fe, theVertexVector);
  }

  TLorentzVector getVertexCorrectedFourVec(const xAOD::FlowElement& fe, const TVector3& vertexToCorrectTo){

    TLorentzVector theNewVector(0.0,0.0,0.0,0.0);
    theNewVector.SetPtEtaPhiM(fe.pt(), fe.eta(), fe.phi(), fe.m());

    vertexCorrectTheFourVector(fe, vertexToCorrectTo, theNewVector);
    return theNewVector;
  }

  void vertexCorrectTheFourVector(const xAOD::FlowElement& fe, const TVector3& vertexToCorrectTo, TLorentzVector& theFourVector){

    float clusterEta = theFourVector.Eta();

    const static SG::AuxElement::ConstAccessor<float> accCenterMag("CENTER_MAG");
    float centerMag = accCenterMag(fe);

    float radius = centerMag/cosh(clusterEta);

    float EtaVertexCorrection = 0.0, PhiVertexCorrection = 0.0;
    float clusterPhi = theFourVector.Phi();

    if (radius > 1.0 && centerMag > 1e-3){
      EtaVertexCorrection = (-vertexToCorrectTo.Z()/cosh(clusterEta) + (vertexToCorrectTo.X()*cos(clusterPhi) + vertexToCorrectTo.Y()*sin(clusterPhi))*tanh(clusterEta))/radius;
      PhiVertexCorrection = (vertexToCorrectTo.X()*sin(clusterPhi) - vertexToCorrectTo.Y()*cos(clusterPhi))/radius;
    }

    float etaVertexCorrected = clusterEta + EtaVertexCorrection;
    float p = std::sqrt(theFourVector.E()*theFourVector.E()-theFourVector.M()*theFourVector.M());
    float ptVertexCorrected = p/cosh(etaVertexCorrected); 
    theFourVector.SetPtEtaPhiM(ptVertexCorrected, etaVertexCorrected, clusterPhi + PhiVertexCorrection, theFourVector.M());

  }

  bool getClusterMoment(const xAOD::FlowElement& fe, xAOD::CaloCluster::MomentType momentType, float& value){

    // First try to retrieve directly from the cluster
    const xAOD::CaloCluster* cluster = dynamic_cast<const xAOD::CaloCluster*>(fe.otherObject(0));
    if(cluster != nullptr){
      double tmpValue(-99.);
      if(cluster->retrieveMoment(momentType, tmpValue)){
        value = static_cast<float>(tmpValue);
        return true;
      }
    }

    // Cluster or moment from it was unavailable, try to retrieve from the FE itself instead
    std::string momentName = getClusterMomentName(momentType);
    if(!momentName.empty()){
      const SG::AuxElement::ConstAccessor<float> acc(momentName);
      if(acc.isAvailable(fe)){
        value = acc(fe);
        return true;
      }
    }

    return false;
  }

  std::vector<float> getEnergiesPerSampling(const xAOD::FlowElement& fe){

    const static SG::AuxElement::ConstAccessor< std::vector<float> > accEPerSampling("ePerSampling");
    if(accEPerSampling.isAvailable(fe)) return accEPerSampling(fe);

    // This wasn't stored as a vector, try to retrieve the individual elements
    const static SG::AuxElement::ConstAccessor<float> accPreSamplerB("LAYERENERGY_PreSamplerB");
    const static SG::AuxElement::ConstAccessor<float> accEMB1("LAYERENERGY_EMB1");
    const static SG::AuxElement::ConstAccessor<float> accEMB2("LAYERENERGY_EMB2");
    const static SG::AuxElement::ConstAccessor<float> accEMB3("LAYERENERGY_EMB3");
    const static SG::AuxElement::ConstAccessor<float> accPreSamplerE("LAYERENERGY_PreSamplerE");
    const static SG::AuxElement::ConstAccessor<float> accEME1("LAYERENERGY_EME1");
    const static SG::AuxElement::ConstAccessor<float> accEME2("LAYERENERGY_EME2");
    const static SG::AuxElement::ConstAccessor<float> accEME3("LAYERENERGY_EME3");
    const static SG::AuxElement::ConstAccessor<float> accHEC0("LAYERENERGY_HEC0");
    const static SG::AuxElement::ConstAccessor<float> accHEC1("LAYERENERGY_HEC1");
    const static SG::AuxElement::ConstAccessor<float> accHEC2("LAYERENERGY_HEC2");
    const static SG::AuxElement::ConstAccessor<float> accHEC3("LAYERENERGY_HEC3");
    const static SG::AuxElement::ConstAccessor<float> accTileBar0("LAYERENERGY_TileBar0");
    const static SG::AuxElement::ConstAccessor<float> accTileBar1("LAYERENERGY_TileBar1");
    const static SG::AuxElement::ConstAccessor<float> accTileBar2("LAYERENERGY_TileBar2");
    const static SG::AuxElement::ConstAccessor<float> accTileGap1("LAYERENERGY_TileGap1");
    const static SG::AuxElement::ConstAccessor<float> accTileGap2("LAYERENERGY_TileGap2");
    const static SG::AuxElement::ConstAccessor<float> accTileGap3("LAYERENERGY_TileGap3");
    const static SG::AuxElement::ConstAccessor<float> accTileExt0("LAYERENERGY_TileExt0");
    const static SG::AuxElement::ConstAccessor<float> accTileExt1("LAYERENERGY_TileExt1");
    const static SG::AuxElement::ConstAccessor<float> accTileExt2("LAYERENERGY_TileExt2");
    const static SG::AuxElement::ConstAccessor<float> accFCAL0("LAYERENERGY_FCAL0");
    const static SG::AuxElement::ConstAccessor<float> accFCAL1("LAYERENERGY_FCAL1");
    const static SG::AuxElement::ConstAccessor<float> accFCAL2("LAYERENERGY_FCAL2");
    const static SG::AuxElement::ConstAccessor<float> accMINIFCAL0("LAYERENERGY_MINIFCAL0");
    const static SG::AuxElement::ConstAccessor<float> accMINIFCAL1("LAYERENERGY_MINIFCAL1");
    const static SG::AuxElement::ConstAccessor<float> accMINIFCAL2("LAYERENERGY_MINIFCAL2");
    const static SG::AuxElement::ConstAccessor<float> accMINIFCAL3("LAYERENERGY_MINIFCAL3");

    // Build the vector up from the individual samplings
    std::vector<float> result(28, 0); 
    result[0]  = accPreSamplerB(fe);
    result[1]  = accEMB1(fe);
    result[2]  = accEMB2(fe);
    result[3]  = accEMB3(fe);
    result[4]  = accPreSamplerE(fe);
    result[5]  = accEME1(fe);
    result[6]  = accEME2(fe);
    result[7]  = accEME3(fe);
    result[8]  = accHEC0(fe);
    result[9]  = accHEC1(fe);
    result[10] = accHEC2(fe);
    result[11] = accHEC3(fe);
    result[12] = accTileBar0(fe);
    result[13] = accTileBar1(fe);
    result[14] = accTileBar2(fe);
    result[15] = accTileGap1(fe);
    result[16] = accTileGap2(fe);
    result[17] = accTileGap3(fe);
    result[18] = accTileExt0(fe);
    result[19] = accTileExt1(fe);
    result[20] = accTileExt2(fe);
    result[21] = accFCAL0(fe);
    result[22] = accFCAL1(fe);
    result[23] = accFCAL2(fe);
    result[24] = accMINIFCAL0(fe);
    result[25] = accMINIFCAL1(fe);
    result[26] = accMINIFCAL2(fe);
    result[27] = accMINIFCAL3(fe);

    return result;
  }

  std::string getClusterMomentName(xAOD::CaloCluster::MomentType momentType){

    switch(momentType){
      case xAOD::CaloCluster::ENG_FRAC_CORE:       return "ENG_FRAC_CORE";
      case xAOD::CaloCluster::FIRST_ENG_DENS:      return "FIRST_ENG_DENS";
      case xAOD::CaloCluster::CENTER_LAMBDA:       return "CENTER_LAMBDA";
      case xAOD::CaloCluster::SECOND_R:            return "SECOND_R";
      case xAOD::CaloCluster::DELTA_ALPHA:         return "DELTA_ALPHA";
      case xAOD::CaloCluster::LATERAL:             return "LATERAL";
      case xAOD::CaloCluster::LONGITUDINAL:        return "LONGITUDINAL";
      case xAOD::CaloCluster::SECOND_LAMBDA:       return "SECOND_LAMBDA";
      case xAOD::CaloCluster::ISOLATION:           return "ISOLATION";
      case xAOD::CaloCluster::ENG_FRAC_MAX:        return "ENG_FRAC_MAX";
      case xAOD::CaloCluster::ENG_BAD_CELLS:       return "ENG_BAD_CELLS";
      case xAOD::CaloCluster::N_BAD_CELLS:         return "N_BAD_CELLS";
      case xAOD::CaloCluster::BADLARQ_FRAC:        return "BADLARQ_FRAC";
      case xAOD::CaloCluster::ENG_POS:             return "ENG_POS";
      case xAOD::CaloCluster::SIGNIFICANCE:        return "SIGNIFICANCE";
      case xAOD::CaloCluster::CELL_SIGNIFICANCE:   return "CELL_SIGNIFICANCE";
      case xAOD::CaloCluster::CELL_SIG_SAMPLING:   return "CELL_SIG_SAMPLING";
      case xAOD::CaloCluster::AVG_LAR_Q:           return "AVG_LAR_Q";
      case xAOD::CaloCluster::AVG_TILE_Q:          return "AVG_TILE_Q";
      case xAOD::CaloCluster::EM_PROBABILITY:      return "EM_PROBABILITY";
      case xAOD::CaloCluster::ENG_CALIB_TOT:       return "ENG_CALIB_TOT";
      case xAOD::CaloCluster::ENG_CALIB_FRAC_EM:   return "ENG_CALIB_FRAC_EM";
      case xAOD::CaloCluster::ENG_CALIB_FRAC_HAD:  return "ENG_CALIB_FRAC_HAD";
      case xAOD::CaloCluster::ENG_CALIB_FRAC_REST: return "ENG_CALIB_FRAC_REST";
      default: return "";
    }
  }

  xAOD::Type::ObjectType signalToXAODType(const xAOD::FlowElement& fe){
    xAOD::FlowElement::signal_t st = fe.signalType();
    if(st & xAOD::FlowElement::PFlow)       return xAOD::Type::ParticleFlow;
    if(st & xAOD::FlowElement::CaloCluster) return xAOD::Type::CaloCluster;
    if(st & xAOD::FlowElement::Track)       return xAOD::Type::TrackParticle;
    if(st & xAOD::FlowElement::Muon)        return xAOD::Type::Muon;
    if(st & xAOD::FlowElement::TCC)         return xAOD::Type::TrackCaloCluster;
    if(st & xAOD::FlowElement::UFO)         return xAOD::Type::TrackCaloCluster; // UFO doesn't have its own xAOD type
    return xAOD::Type::Other;
  }
}
