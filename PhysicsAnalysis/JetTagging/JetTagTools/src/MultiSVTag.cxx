/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           MultiSVTag.cxx
***************************************************************************/
#include "JetTagTools/MultiSVTag.h"
//#include "JetEvent/Jet.h"
#include "GaudiKernel/IToolSvc.h"
#include "Navigation/NavigationToken.h"
#include "GaudiKernel/ITHistSvc.h"
#include "JetTagTools/HistoHelperRoot.h"
#include "JetTagTools/JetTagUtils.h"

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
#include "JetTagCalibration/CalibrationBroker.h"
#include "TMVA/Reader.h"
#include "TMVA/Types.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "TMVA/IMethod.h"
#include "TMVA/BinarySearchTree.h"
#include "TMVA/MethodBase.h"
namespace Analysis
{

  MultiSVTag::MultiSVTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
    m_calibrationTool("BTagCalibrationBroker"),
    m_runModus("analysis") 
   // m_secVxFinderName("InDetVKalVxInJetTool")
  {
    declareInterface<ITagTool>(this);
    // access to XML configuration files for TMVA from COOL:
    declareProperty("calibrationTool", m_calibrationTool);
    declareProperty("Runmodus",       m_runModus= "analysis");
    declareProperty("jetCollectionList", m_jetCollectionList);
    declareProperty("useForcedCalibration", m_doForcedCalib   = false);
    declareProperty("ForcedCalibrationName", m_ForcedCalibName = "AntiKt4TopoEM");//Cone4H1Tower
    declareProperty("SecVxFinderName",m_secVxFinderName);
    //declareProperty("SVAlgType",      m_SVmode);
    declareProperty("taggerNameBase",m_taggerNameBase = "MultiSVbb1");
    declareProperty("xAODBaseName",m_xAODBaseName);
    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
  }

  MultiSVTag::~MultiSVTag() {
  }

  StatusCode MultiSVTag::initialize() {
    // define tagger name:
    
    //m_taggerNameBase = instanceName2;
    //std::string tmp = "MultiSV1" ;
    //m_taggerNameBase = tmp;

    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      return sc;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_calibrationTool);
    }
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib");
    ATH_MSG_DEBUG("#BTAG# m_taggerNameBase " << m_taggerNameBase);
    m_tmvaReaders.clear();
    m_tmvaMethod.clear();
    return StatusCode::SUCCESS;                       
  }

  StatusCode MultiSVTag::finalize(){
  // delete readers:
    std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
    for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
    std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
    for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;
    return StatusCode::SUCCESS;
  }

  StatusCode MultiSVTag::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging * BTag){    
    
    /** author to know which jet algorithm: */ 
    std::string author = JetTagUtils::getJetAuthor(jetToTag);
    if (m_doForcedCalib) author = m_ForcedCalibName;
    ATH_MSG_DEBUG("#BTAG# MSV Using jet type " << author << " for calibrations.");
    //....
    std::string alias = m_calibrationTool->channelAlias(author);//why this gives always the same?
    //TString xmlFileName = "btag"+m_taggerNameBase+"Config_"+alias+".xml";//from MV1, so should work
    //ATH_MSG_DEBUG("#BTAG# xmlFileName= "<<xmlFileName);

    TMVA::Reader* tmvaReader;
    std::map<std::string, TMVA::Reader*>::iterator pos;
    ATH_MSG_DEBUG("#BTAG# Jet author for MultiSVTag: " << author << ", alias: " << alias );
    /* check if calibration (neural net structure or weights) has to be updated: */
    std::pair<TList*, bool> calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib");
     
    bool calibHasChanged = calib.second;
    //bool calibHasChanged = true;
    std::ostringstream iss;
    TMVA::MethodBase * kl=0; // = dynamic_cast<TMVA::MethodBase*>(method); 
    std::map<std::string, TMVA::MethodBase*>::iterator itmap;
    if(calibHasChanged) {
      
      ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
      if(!calib.first) {
        ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for "<< m_taggerNameBase );
        return StatusCode::SUCCESS;
      }
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);
      //now the new part istringstream
      TList* list = calib.first; 
      for(int i=0; i<list->GetSize(); ++i) {
        TObjString* ss = (TObjString*)list->At(i);
	    TString sss = ss->String();
	    int pos = sss.First('<');
	    sss.Remove(0,pos); 
    //	std::cout <<ss->String() << std::endl;
	    iss << sss.Data() ;
      }
      ATH_MSG_DEBUG("#BTAG# new std::istringstream created: " );           
    
    // now configure the TMVAReaders:
    // check if the reader for this tagger needs update
    //if(!m_calibrationTool->updatedTagger(m_taggerNameBase, alias, m_taggerNameBase+"Calib", name())) {
    //  if(iss.str().size()>0){
        tmvaReader = new TMVA::Reader();

        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2") tmvaReader->AddVariable( "pt",  &m_jetpt  );                     //v1v2
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "Nvtx",  &m_nvtx  );                    //v1v2                      
        if(m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "MaxEfrc",  &m_maxefrc );               //  v2                        
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "sumMass",  &m_summass    );            //v1v2
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "totalntrk",  &m_totalntrk    );        //v1v2
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "diffntrkSV0",  &m_diffntrkSV0 );       //v1v2
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "normDist",  &m_normDist );            //v1v2//
        if(m_taggerNameBase=="MultiSVbb1")tmvaReader->AddVariable( "maxVtxMass",  &m_mmax_mass    );       //v1 
        if(m_taggerNameBase=="MultiSVbb1")tmvaReader->AddVariable( "maxSecVtxMass",  &m_mmx2_mass    );    //v1
        if(m_taggerNameBase=="MultiSVbb1")tmvaReader->AddVariable( "EfrcmaxVtxMass",  &m_mmax_efrc    );   //v1
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "EfrcmaxSecVtxMass", &m_mmx2_efrc    ); //v1v2
        if(m_taggerNameBase=="MultiSVbb1" ||m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "dlsmaxVtxMass",  &m_mmax_dist    );    //v1v2
        if(m_taggerNameBase=="MultiSVbb1")tmvaReader->AddVariable( "dlsmaxSecVtxMass", &m_mmx2_dist    );  //v1
        if(m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "dRmaxVtxMassj",  &m_mmax_DRjet    );   //  v2
        if(m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "dRmaxSecVtxMassj", &m_mmx2_DRjet   );  //  v2
        if(m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "d2Mass12",  &m_mx12_2d12    );         //  v2
        if(m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "DRMass12",  &m_mx12_DR   );            //  v2
        if(m_taggerNameBase=="MultiSVbb2")tmvaReader->AddVariable( "AngleMass12",  &m_mx12_Angle );        //  v2
//      tmvaReader->BookMVA("BDT", xmlFileName);
        TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kBDT, iss.str().c_str());
        
	    kl = dynamic_cast<TMVA::MethodBase*>(method);
        
      // add it or overwrite it in the map of readers:
        pos = m_tmvaReaders.find(alias);
        if(pos!=m_tmvaReaders.end()) {
          delete pos->second;
          m_tmvaReaders.erase(pos);
        }
        itmap = m_tmvaMethod.find(alias);
        if(itmap!=m_tmvaMethod.end()) {
          delete itmap->second;
          m_tmvaMethod.erase(itmap);
        }
        m_tmvaReaders.insert( std::make_pair( alias, tmvaReader ) );
        m_tmvaMethod.insert( std::make_pair( alias, kl ) );
      //  m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase,alias, m_taggerNameBase+"Calib", false, name());
      }

    //....
    //the jet
    double jeteta = jetToTag.eta(), jetphi = jetToTag.phi(), jetpt = jetToTag.pt();
    m_jetpt = jetpt;
    ATH_MSG_DEBUG("#BTAG# Jet properties : eta = " << jeteta
                    << " phi = " << jetphi << " pT  = " <<jetpt/1000.);
    //ATH_MSG_INFO("Factory PVX x = " << m_priVtx->x() << " y = " << m_priVtx->y() << " z = " << m_priVtx->z());

    TLorentzVector jp4; jp4.SetPtEtaPhiM(jetToTag.pt(), jetToTag.eta(), jetToTag.phi(), jetToTag.m());
   // CLHEP::HepLorentzVector jp4(jetToTag.jetP4().px(), jetToTag.jetP4().px(), jetToTag.jetP4().px(), jetToTag.e());
    
    int msv_n = 0;
    int all_trks = 0;
    int nvtx2trk = 0;
    int nsv    = 0;
    int singletrk = 0;
    float totalmass = 0.;
    float distnorm = 0.;
    
    BTag->variable<float>(m_secVxFinderName, "normdist", distnorm);
    BTag->variable<int>(m_secVxFinderName, "nvsec", msv_n);
    std::vector< ElementLink< xAOD::VertexContainer > > msvVertices;
    BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_secVxFinderName, "vertices", msvVertices);
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
        totalmass += mass;
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
      m_normDist = distnorm;
      m_nvtx = nsv;
      m_totalntrk = all_trks;
      m_summass = totalmass;
      
      int diffntrk = -999;
      int SV0ntrk  = 0;
      std::vector< ElementLink< xAOD::VertexContainer > > SV0Vertice;
      BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv0_infosource, "vertices", SV0Vertice);
      if (SV0Vertice.size()>0 && SV0Vertice[0].isValid()){
         BTag->taggerInfo(SV0ntrk, xAOD::BTagInfo::SV0_NGTinSvx);
         diffntrk = all_trks - SV0ntrk;
      }else{ diffntrk = all_trks;
      }    
      /*if(_msv_n>0 &&SV0Vertices.size()>0){
         diffntrk = all_trks - SV0ntrk;
      }else{if(_msv_n>0)diffntrk = all_trks;
      }*/
      m_diffntrkSV0 = diffntrk;
      float tmpefrc = 0.;
      for(int i=0; i<nsv; i++) {
        if(v_vtxntrk[i]!=1){ 
          if(v_vtxefrc[i] > tmpefrc ) tmpefrc = v_vtxefrc[i];
        } 
      }
      m_maxefrc = tmpefrc;
      m_mmax_mass  = -9.;
      
      m_mmax_efrc  = -9.;
    
      m_mmax_DRjet = -9;
      m_mmax_dist  = -9.;
      m_mmx2_mass  = -9.;      
     
      m_mmx2_efrc  = -9.;
   
      m_mmx2_DRjet = -9.;
      m_mmx2_dist  = -9.;
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
        sv1p3.SetX(v_vtxx[ivm1] - m_priVtx->x());
        sv1p3.SetY(v_vtxy[ivm1] - m_priVtx->y());
        sv1p3.SetZ(v_vtxz[ivm1] - m_priVtx->z());
        m_mmax_mass  = v_vtxmass[ivm1];
        m_mmax_efrc  = v_vtxefrc[ivm1];
  
        m_mmax_DRjet = v_vtxDRj[ivm1];
        m_mmax_dist  = v_vtxdls[ivm1];
      }
      if(ivm2>=0) {
        pvtx2.SetPtEtaPhiM(v_vtxpt[ivm2], v_vtxeta[ivm2], v_vtxphi[ivm2], v_vtxmass[ivm2]);
        TVector3 p2 = pvtx2.Vect();
        sv2p3.SetX(v_vtxx[ivm2] - m_priVtx->x());
        sv2p3.SetY(v_vtxy[ivm2] - m_priVtx->y());
        sv2p3.SetZ(v_vtxz[ivm2] - m_priVtx->z());
        m_mmx2_mass  = v_vtxmass[ivm2];
        m_mmx2_efrc  = v_vtxefrc[ivm2];

        m_mmx2_DRjet = v_vtxDRj[ivm2];
        m_mmx2_dist  = v_vtxdls[ivm2];
      }
      // distances: max mass vertex to PV, and mx2 to max vertex:
      m_mx12_2d12 = -9.;
      m_mx12_DR   = -9.;
      m_mx12_Angle= -9.; 
      if(m_priVtx) {
        if(ivm1>=0&&ivm2>=0) {
                                      
          m_mx12_2d12 = TMath::Sqrt(  (v_vtxx[ivm2] - v_vtxx[ivm1]) * (v_vtxx[ivm2] - v_vtxx[ivm1])
                                   +  (v_vtxy[ivm2] - v_vtxy[ivm1]) * (v_vtxy[ivm2] - v_vtxy[ivm1]) );       
          m_mx12_DR    = sv1p3.DeltaR(sv2p3);
         
          m_mx12_Angle = sv1p3.Angle(sv2p3);
          
        }  
      }else {
        ATH_MSG_WARNING("#BTAG# Tagging requested, but no primary vertex supplied.");
      }
      //end of inputs
      ATH_MSG_DEBUG("#BTAG# MSV inputs: "           <<
                               "nvtx= "            << m_nvtx        <<
                               ", maxefrc= "       << m_maxefrc     <<
                               ", summass= "       << m_summass     <<
                               ", totalntrk= "     << m_totalntrk   <<
                               ", diffntrkSV0= "   << m_diffntrkSV0 <<
                               ", normDist= "      << m_normDist    <<
                               ", mmax_mass= "     << m_mmax_mass   <<
                               ", mmax_efrc= "     << m_mmax_efrc   <<
                               ", mmax_DRjet= "    << m_mmax_DRjet  <<
                               ", mmax_dist= "     << m_mmax_dist   <<
                               ", mmx2_mass= "     << m_mmx2_mass   <<
                               ", mmx2_efrc= "     << m_mmx2_efrc   << 
                               ", mmx2_DRjet= "    << m_mmx2_DRjet  <<
                               ", mmx2_dist= "     << m_mmx2_dist   <<
                               ", mx12_2d12= "     << m_mx12_2d12   << 
                               ", mx12_DR= "       << m_mx12_DR     <<
                               ", mx12_Angle="     << m_mx12_Angle 
                   );
    }
    //...
    //compute BDT weight
    double msvW = -9.;
    if( nvtx2trk>1 ){
      std::map<std::string, TMVA::Reader*>::iterator pos2 = m_tmvaReaders.find(alias);
      if(pos2==m_tmvaReaders.end()) {//    if(pos2==m_tmvaReaders[binnb-1].end()) {
        int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
        if(0==alreadyWarned) {
          ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection " << alias);
          m_undefinedReaders.push_back(alias);
        }
      }
      else {
        std::map<std::string, TMVA::MethodBase*>::iterator itmap2 = m_tmvaMethod.find(alias);
        if((itmap2->second)!=0){
          msvW = pos2->second->EvaluateMVA( itmap2->second ); //"BDT method"
          ATH_MSG_DEBUG("#BTAG# BB weight: "<<m_taggerNameBase<<" "<< msvW);
        }else ATH_MSG_WARNING("#BTAG#  kl==0"); 
      }
    }
    if(m_runModus=="analysis") {
      BTag->setVariable<double>(m_taggerNameBase, "discriminant", msvW);
   }
    return StatusCode::SUCCESS;
  }
  void MultiSVTag::finalizeHistos() {
    /// implementation for Analysis::ITagTool::finalizeHistos
  }

}

