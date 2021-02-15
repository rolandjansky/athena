/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           MultiSVTag.cxx
***************************************************************************/
#include "JetTagTools/MultiSVTag.h"
#include "GaudiKernel/IToolSvc.h"
#include "Navigation/NavigationToken.h"
#include "GaudiKernel/ITHistSvc.h"
#include "JetTagTools/HistoHelperRoot.h"

#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "xAODJet/Jet.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/SecVtxHelper.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include "AthenaKernel/Units.h"

#include "xAODBTagging/ftagfloat_t.h"

using Athena::Units::GeV;

namespace Analysis
{

  MultiSVTag::MultiSVTag(const std::string& t, const std::string& n, const IInterface* p)
    : base_class(t,n,p),
    m_runModus("analysis")
  {
    declareProperty("Runmodus",       m_runModus= "analysis");
    declareProperty("jetCollectionList", m_jetCollectionList);
    declareProperty("useForcedCalibration", m_doForcedCalib   = false);
    declareProperty("ForcedCalibrationName", m_ForcedCalibName = "AntiKt4TopoEM");//Cone4H1Tower
    declareProperty("SecVxFinderName",m_secVxFinderName);
    declareProperty("taggerNameBase",m_taggerNameBase = "MultiSVbb1");
    declareProperty("taggerName", m_taggerName = "MultiSVbb1");
    declareProperty("xAODBaseName",m_xAODBaseName);
    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
    declareProperty("inputSV1SourceName", m_sv1_infosource = "SV1");
  }

  MultiSVTag::~MultiSVTag() {
  }

  StatusCode MultiSVTag::initialize() {
    // define tagger name:

    m_warnCounter=0;

    m_treeName = "BDT";
    m_varStrName = "variables";

    // prepare readKey for calibration data:
    ATH_CHECK(m_readKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode MultiSVTag::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode MultiSVTag::tagJet(const xAOD::Vertex& priVtx,
                                const xAOD::Jet& jetToTag,
                                xAOD::BTagging& BTag,
                                const std::string &jetName) const
  {
    //Retrieval of Calibration Condition Data objects
    SG::ReadCondHandle<JetTagCalibCondData> readCdo(m_readKey);

    /** author to know which jet algorithm: */
    std::string author = jetName;
    if (m_doForcedCalib) author = m_ForcedCalibName;
    ATH_MSG_DEBUG("#BTAG# MSV Using jet type " << author << " for calibrations.");
    //....
    std::string alias = readCdo->getChannelAlias(author);
    ATH_MSG_DEBUG("#BTAG# Jet author for MultiSVTag: " << author << ", alias: " << alias );

    //Retrieve BDT from cond object
    MVAUtils::BDT *bdt(nullptr);
    ATH_MSG_DEBUG("#BTAG# Getting MVAUtils::BDT for "<<m_taggerNameBase);
    bdt = readCdo->retrieveBdt(m_taggerNameBase,author);
    if (!bdt) {
      ATH_MSG_WARNING("#BTAG# No BDT for " << m_taggerNameBase<<" exists in the condition object.. Disabling algorithm.");
      return StatusCode::SUCCESS;
    }

    //Retrieve input variables for BDT in cond store
    std::vector<float*>  inputPointers; inputPointers.clear();
    std::vector<std::string> inputVars = readCdo->retrieveInputVars(m_taggerNameBase,author, m_taggerNameBase+"Calib/"+m_varStrName);
    unsigned nConfgVar=0; bool badVariableFound=false;

    Vars vars;
    vars.SetVariableRefs(msg(),inputVars,nConfgVar,badVariableFound,inputPointers);
    ATH_MSG_DEBUG("#BTAG# nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

    if ( inputVars.size()!=nConfgVar or badVariableFound ) {
      ATH_MSG_WARNING("#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
      return StatusCode::SUCCESS;
    }

    bdt->SetPointers(inputPointers);

    //the jet
    double jeteta = jetToTag.eta(), jetphi = jetToTag.phi();
    vars.m_jetpt = jetToTag.pt();
    ATH_MSG_DEBUG("#BTAG# Jet properties : eta = " << jeteta
                  << " phi = " << jetphi << " pT  = " <<vars.m_jetpt/GeV);

    TLorentzVector jp4; jp4.SetPtEtaPhiM(jetToTag.pt(), jetToTag.eta(), jetToTag.phi(), jetToTag.m());

    int msv_n = 0;
    int all_trks = 0;
    int nvtx2trk = 0;
    int nsv    = 0;
    int singletrk = 0;

    bool status = true;

    status &= BTag.variable<float>(m_secVxFinderName, "normdist", vars.m_normDist);
    status &= BTag.variable<int>(m_secVxFinderName, "nvsec", msv_n);
    std::vector< ElementLink< xAOD::VertexContainer > > msvVertices;
    status &= BTag.variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_secVxFinderName, "vertices", msvVertices);
    ATH_MSG_DEBUG("#BTAG# MSV_vertices: " <<msvVertices.size());
    std::vector<float> v_vtxmass = std::vector<float>(10,0);
    std::vector<float> v_vtxefrc = std::vector<float>(10,0);
    std::vector<float> v_vtxntrk = std::vector<float>(10,0);
    std::vector<float> v_vtxDRj  = std::vector<float>(10,0);
    std::vector<float> v_vtxdls  = std::vector<float>(10,0);
    std::vector<float> v_vtxpt   = std::vector<float>(10,0);
    std::vector<float> v_vtxeta  = std::vector<float>(10,0);
    std::vector<float> v_vtxphi  = std::vector<float>(10,0);
    std::vector<float> v_vtxx   = std::vector<float>(10,0);
    std::vector<float> v_vtxy  = std::vector<float>(10,0);
    std::vector<float> v_vtxz  = std::vector<float>(10,0);
    // loop in msv vertices
    if(msvVertices.size()>0){
      const std::vector<ElementLink<xAOD::VertexContainer> >::const_iterator verticesEnd = msvVertices.end();
      for(std::vector<ElementLink<xAOD::VertexContainer> >::const_iterator vtxIter=msvVertices.begin(); vtxIter!=verticesEnd; ++vtxIter){
        if(msvVertices.size()>=10) continue;
        float mass = xAOD::SecVtxHelper::VertexMass(**vtxIter);
        float efrc = xAOD::SecVtxHelper::EnergyFraction(**vtxIter);
        int   ntrk = xAOD::SecVtxHelper::VtxNtrk(**vtxIter);
        float pt   = xAOD::SecVtxHelper::Vtxpt(**vtxIter);
        float eta  = xAOD::SecVtxHelper::Vtxeta(**vtxIter);
        float phi  = xAOD::SecVtxHelper::Vtxphi(**vtxIter);
        float dls  = xAOD::SecVtxHelper::VtxnormDist(**vtxIter);
        float x = (**vtxIter)->x();
        float y = (**vtxIter)->y();
        float z = (**vtxIter)->z();
        TLorentzVector svp4; svp4.SetPtEtaPhiM(pt,eta,phi,mass);
        //if(jp4.DeltaR(svp4)>0.4) continue;
        vars.m_summass += mass;
        const std::vector<ElementLink<xAOD::TrackParticleContainer> > svTrackLinks = (**vtxIter)->trackParticleLinks();
        if(svTrackLinks.size()==1){ singletrk++;
        }else{ nvtx2trk++;
        }
        all_trks += svTrackLinks.size();

        ATH_MSG_DEBUG("#BTAG# MSV_vtx mass: " <<mass<<", efrc: "<<efrc<<", ntrk: "<<ntrk );
        ATH_MSG_DEBUG("#BTAG# MSV_vtx pt: " <<pt<<", eta: "<<eta<<", phi: "<<phi );
        ATH_MSG_DEBUG("#BTAG# MSV_vtx DRj: " <<jp4.DeltaR(svp4));
        v_vtxmass[nsv] = mass;
        v_vtxefrc[nsv] = efrc;
        v_vtxntrk[nsv] = ntrk;
        v_vtxDRj[nsv]  = jp4.DeltaR(svp4);
        v_vtxdls[nsv]  = dls;
        v_vtxpt[nsv]   = pt;
        v_vtxeta[nsv]  = eta;
        v_vtxphi[nsv]  = phi;
        v_vtxx[nsv]    = x;
        v_vtxy[nsv]    = y;
        v_vtxz[nsv]    = z;
        nsv++;

      }//loop in vertices
      vars.m_nvtx = nsv;
      vars.m_totalntrk = all_trks;

      int SV1ntrk  = 0;
      std::vector< ElementLink< xAOD::VertexContainer > > SV1Vertice;
      status &= BTag.variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", SV1Vertice);
      if (SV1Vertice.size()>0 && SV1Vertice[0].isValid()){
         status &= BTag.taggerInfo(SV1ntrk, xAOD::BTagInfo::SV1_NGTinSvx);
         vars.m_diffntrkSV1 = all_trks - SV1ntrk;
      }else{ vars.m_diffntrkSV1 = all_trks;
      }

      vars.m_diffntrkSV0 = vars.m_diffntrkSV1;

      if (!status) {
        ATH_MSG_WARNING("Error retrieving input values; results will be incorrect!");
      }

      for(int i=0; i<nsv; i++) {
        if(v_vtxntrk[i]!=1){
          if(v_vtxefrc[i] > vars.m_maxefrc ) vars.m_maxefrc = v_vtxefrc[i];
        }
      }

      int ivm1 = -1; int ivm2 = -1;
      float vm1 = 0.; float vm2 = 0.;
      TLorentzVector pvtx1;  TLorentzVector pvtx2;
      TVector3 sv1p3;  TVector3 sv2p3;
      for(int i=0; i<nsv; i++) {
        if( v_vtxntrk[i]!=1) {
          if( v_vtxmass[i] > vm1 ) {
            vm1 = v_vtxmass[i];
            ivm1 = i;
          }
        }
      }
      for(int i=0; i<nsv; i++) {
        if( v_vtxntrk[i]!=1) {
          if( v_vtxmass[i] > vm2 && v_vtxmass[i] < vm1 ) {
            vm2  = v_vtxmass[i];
            ivm2 = i;
          }
        }
      }
      if(ivm1>=0) {
        pvtx1.SetPtEtaPhiM(v_vtxpt[ivm1], v_vtxeta[ivm1], v_vtxphi[ivm1], v_vtxmass[ivm1]);
        TVector3 p1 = pvtx1.Vect();
        sv1p3.SetX(v_vtxx[ivm1] - priVtx.x());
        sv1p3.SetY(v_vtxy[ivm1] - priVtx.y());
        sv1p3.SetZ(v_vtxz[ivm1] - priVtx.z());
        vars.m_mmax_mass  = v_vtxmass[ivm1];
        vars.m_mmax_efrc  = v_vtxefrc[ivm1];

        vars.m_mmax_DRjet = v_vtxDRj[ivm1];
        vars.m_mmax_dist  = v_vtxdls[ivm1];
      }
      if(ivm2>=0) {
        pvtx2.SetPtEtaPhiM(v_vtxpt[ivm2], v_vtxeta[ivm2], v_vtxphi[ivm2], v_vtxmass[ivm2]);
        TVector3 p2 = pvtx2.Vect();
        sv2p3.SetX(v_vtxx[ivm2] - priVtx.x());
        sv2p3.SetY(v_vtxy[ivm2] - priVtx.y());
        sv2p3.SetZ(v_vtxz[ivm2] - priVtx.z());
        vars.m_mmx2_mass  = v_vtxmass[ivm2];
        vars.m_mmx2_efrc  = v_vtxefrc[ivm2];

        vars.m_mmx2_DRjet = v_vtxDRj[ivm2];
        vars.m_mmx2_dist  = v_vtxdls[ivm2];
      }
      // distances: max mass vertex to PV, and mx2 to max vertex:
      if(ivm1>=0&&ivm2>=0) {

        vars.m_mx12_2d12 = TMath::Sqrt(  (v_vtxx[ivm2] - v_vtxx[ivm1]) * (v_vtxx[ivm2] - v_vtxx[ivm1])
                                         +  (v_vtxy[ivm2] - v_vtxy[ivm1]) * (v_vtxy[ivm2] - v_vtxy[ivm1]) );
        vars.m_mx12_DR    = sv1p3.DeltaR(sv2p3);
        
        vars.m_mx12_Angle = sv1p3.Angle(sv2p3);
        
      }
      //end of inputs
      ATH_MSG_DEBUG("#BTAG# MSV inputs: "           <<
                               "nvtx= "            << vars.m_nvtx        <<
                               ", maxefrc= "       << vars.m_maxefrc     <<
                               ", summass= "       << vars.m_summass     <<
                               ", totalntrk= "     << vars.m_totalntrk   <<
                               ", diffntrkSV0= "   << vars.m_diffntrkSV0 <<
                               ", diffntrkSV1= "   << vars.m_diffntrkSV1 <<
                               ", normDist= "      << vars.m_normDist    <<
                               ", mmax_mass= "     << vars.m_mmax_mass   <<
                               ", mmax_efrc= "     << vars.m_mmax_efrc   <<
                               ", mmax_DRjet= "    << vars.m_mmax_DRjet  <<
                               ", mmax_dist= "     << vars.m_mmax_dist   <<
                               ", mmx2_mass= "     << vars.m_mmx2_mass   <<
                               ", mmx2_efrc= "     << vars.m_mmx2_efrc   <<
                               ", mmx2_DRjet= "    << vars.m_mmx2_DRjet  <<
                               ", mmx2_dist= "     << vars.m_mmx2_dist   <<
                               ", mx12_2d12= "     << vars.m_mx12_2d12   <<
                               ", mx12_DR= "       << vars.m_mx12_DR     <<
                               ", mx12_Angle="     << vars.m_mx12_Angle
                   );
    }
    //...
    //compute BDT weight
    ftagfloat_t msvW = -9.;
    if( nvtx2trk>1 ){
      msvW = GetClassResponse(bdt);
    }

    if(m_runModus=="analysis") {
      BTag.setVariable<ftagfloat_t>(m_taggerNameBase, "discriminant", msvW);
    }
    return StatusCode::SUCCESS;
  }

  void MultiSVTag::finalizeHistos() {
    /// implementation for Analysis::ITagTool::finalizeHistos
  }

  void MultiSVTag::Vars::SetVariableRefs(MsgStream& msg,
                                         const std::vector<std::string>& inputVars,
                                         unsigned &nConfgVar,
                                         bool &badVariableFound,
                                         std::vector<float*> &inputPointers)
  {
    for (unsigned ivar=0; ivar<inputVars.size(); ivar++) {
      if      (inputVars.at(ivar)=="pt"               ) { inputPointers.push_back(&m_jetpt       ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="Nvtx"             ) { inputPointers.push_back(&m_nvtx        ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="MaxEfrc"          ) { inputPointers.push_back(&m_maxefrc     ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="sumMass"          ) { inputPointers.push_back(&m_summass     ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="totalntrk"        ) { inputPointers.push_back(&m_totalntrk   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="diffntrkSV0"      ) { inputPointers.push_back(&m_diffntrkSV0 ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="diffntrkSV1"      ) { inputPointers.push_back(&m_diffntrkSV1 ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="normDist"         ) { inputPointers.push_back(&m_normDist    ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="maxVtxMass"       ) { inputPointers.push_back(&m_mmax_mass   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="maxSecVtxMass"    ) { inputPointers.push_back(&m_mmx2_mass   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="EfrcmaxVtxMass"   ) { inputPointers.push_back(&m_mmax_efrc   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="EfrcmaxSecVtxMass") { inputPointers.push_back(&m_mmx2_efrc   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="dlsmaxVtxMass"    ) { inputPointers.push_back(&m_mmax_dist   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="dlsmaxSecVtxMass" ) { inputPointers.push_back(&m_mmx2_dist   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="dRmaxVtxMassj"    ) { inputPointers.push_back(&m_mmax_DRjet  ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="dRmaxSecVtxMassj" ) { inputPointers.push_back(&m_mmx2_DRjet  ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="d2Mass12"         ) { inputPointers.push_back(&m_mx12_2d12   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="DRMass12"         ) { inputPointers.push_back(&m_mx12_DR     ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="AngleMass12"      ) { inputPointers.push_back(&m_mx12_Angle  ) ; nConfgVar++; }
      else {
        msg << MSG::WARNING << "#BTAG# \""<<inputVars.at(ivar)<<"\" <- This variable found in xml/calib-file does not match to any variable declared in MultiSV... the algorithm will be 'disabled'." << endmsg;
	badVariableFound=true;
      }
    }

  }

}//end namespace
