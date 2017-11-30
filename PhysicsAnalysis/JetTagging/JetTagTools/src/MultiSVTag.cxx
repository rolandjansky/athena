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
#include "TMVA/MethodBDT.h"
#include "TMVA/MethodBase.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

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
    declareProperty("taggerName", m_taggerName = "MultiSVbb1");
    declareProperty("xAODBaseName",m_xAODBaseName);
    declareProperty("inputSV0SourceName", m_sv0_infosource = "SV0");
    declareProperty("inputSV1SourceName", m_sv1_infosource = "SV1");
  }

  MultiSVTag::~MultiSVTag() {
  }

  StatusCode MultiSVTag::initialize() {
    // define tagger name:

    //m_taggerNameBase = instanceName2;
    //std::string tmp = "MultiSV1" ;
    m_disableAlgo=false;
    m_useEgammaMethodMultiSV=false;
    m_warnCounter=0;

    m_treeName = "BDT";
    m_varStrName = "variables";
    //m_taggerNameBase = tmp;

    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      return sc;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_calibrationTool);
    }
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib");
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib/"+m_treeName);
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib/"+m_varStrName);
    ATH_MSG_DEBUG("#BTAG# m_taggerNameBase " << m_taggerNameBase);
    m_tmvaReaders.clear();
    m_tmvaMethod.clear();
    m_egammaBDTs.clear();
    return StatusCode::SUCCESS;
  }

  StatusCode MultiSVTag::finalize(){
    if (m_useEgammaMethodMultiSV) {
      for( auto temp: m_egammaBDTs ) if(temp.second) delete temp.second;
    }
    else {
      // delete readers:
      std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
      for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
      std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
      for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;
    }
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

    TMVA::Reader* tmvaReader = nullptr;
    std::map<std::string, TMVA::Reader*>::iterator pos;
    TMVA::MethodBase * kl=0;        std::map<std::string, TMVA::MethodBase*>::iterator it_mb;
    MVAUtils::BDT *bdt=0; std::map<std::string, MVAUtils::BDT*>::iterator it_egammaBDT;
    ATH_MSG_DEBUG("#BTAG# Jet author for MultiSVTag: " << author << ", alias: " << alias );
    /* check if calibration (neural net structure or weights) has to be updated: */
    std::pair<TObject*, bool> calib=m_calibrationTool->retrieveTObject<TObject>(m_taggerNameBase,author,m_taggerNameBase+"Calib");

    bool calibHasChanged = calib.second;
    std::ostringstream iss;
    std::map<std::string, TMVA::MethodBase*>::iterator itmap;
    if(calibHasChanged) {

      ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
      if(!calib.first) {
        ATH_MSG_WARNING("#BTAG# TObject can't be retrieved -> no calibration for "<< m_taggerNameBase );
        return StatusCode::SUCCESS;
      }
      const TString rootClassName=calib.first->ClassName();
      if      (rootClassName=="TDirectoryFile") m_useEgammaMethodMultiSV=true;
      else if (rootClassName=="TList")          m_useEgammaMethodMultiSV=false;//tmva method
      else {
	ATH_MSG_WARNING("#BTAG# Unsupported ROOT class type: "<<rootClassName<<" is retrieved. Disabling algorithm..");
	m_disableAlgo=true;
	return StatusCode::SUCCESS;
      }

      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);
      std::vector<float*>  inputPointers; inputPointers.clear();
      std::vector<std::string> inputVars; inputVars.clear();
      unsigned nConfgVar=0,calibNvars=0; bool badVariableFound=false;

      if (!m_useEgammaMethodMultiSV) {
	ATH_MSG_INFO("#BTAG# Booking TMVA::Reader for "<<m_taggerNameBase);
	std::ostringstream iss; //iss.clear();
	//now the new part istringstream
	TList* list = (TList*)calib.first;
	for(int i=0; i<list->GetSize(); ++i) {
	  TObjString* ss = (TObjString*)list->At(i);
	  std::string sss = ss->String().Data();
	  //KM: if it doesn't find "<" in the string, it starts from non-space character
	  int posi = sss.find('<')!=std::string::npos ? sss.find('<') : sss.find_first_not_of(" ");
	  std::string tmp = sss.erase(0,posi);
	  //std::cout<<tmp<<std::endl;
	  iss << tmp.data();
	  if (tmp.find("<Variable")!=std::string::npos ) {
	    if ( tmp.find("Variables NVar")!=std::string::npos ) {
	      std::string newString=tmp.substr(tmp.find("\"")+1,tmp.find("\" ")-(tmp.find("\"")+1));
	      calibNvars=stoi(newString);
	    }
	    else if ( tmp.find("Variable VarIndex")!=std::string::npos ) {
	      std::string varIndex  =tmp.substr(tmp.find("=\"")+2, tmp.find("\" ")-(tmp.find("=\"")+2));
	      std::string tmpVar  = tmp.erase(0,tmp.find("Expression=\"")+12);
	      std::string varExpress=tmp.substr(0, tmp.find("\""));
	      inputVars.push_back(varExpress);
	    }
	  }
	  // else if (tmp.find("NClass")!=std::string::npos ) {
	  //   std::string newString=tmp.substr(tmp.find("\"")+1,tmp.find("\" ")-(tmp.find("\"")+1));
	  //   nClasses =stoi(newString);
	  // }
	}

	// now configure the TMVAReaders:
	/// check if the reader for this tagger needs update
	tmvaReader = new TMVA::Reader();

	SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,inputPointers);
	ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

	if ( calibNvars!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING("#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << calibNvars << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
	  return StatusCode::SUCCESS;
	}

	//tmvaReader->BookMVA("BDT", xmlFileName);
	TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kBDT, iss.str().data() );
	kl = dynamic_cast<TMVA::MethodBase*>(method);

	// add it or overwrite it in the map of readers:
	pos = m_tmvaReaders.find(alias);
	if(pos!=m_tmvaReaders.end()) {
	  delete pos->second;
	  m_tmvaReaders.erase(pos);
	}
	m_tmvaReaders.insert( std::make_pair( alias, tmvaReader ) );

	it_mb = m_tmvaMethod.find(alias);
	if(it_mb!=m_tmvaMethod.end()) {
	  delete it_mb->second;
	  m_tmvaMethod.erase(it_mb);
	}
	m_tmvaMethod.insert( std::make_pair( alias, kl ) );

	iss.clear();
      }
      else {//if m_useEgammaMethodMultiSV
	ATH_MSG_INFO("#BTAG# Booking MVAUtils::BDT for "<<m_taggerNameBase);

	// TDirectoryFile* f= (TDirectoryFile*)calib.first;
	// TTree *tree = (TTree*) f->Get(treeName.data());
	std::pair<TObject*, bool> calibTree=m_calibrationTool->retrieveTObject<TObject>(m_taggerNameBase,author,m_taggerNameBase+"Calib/"+m_treeName);
	TTree *tree = (TTree*) calibTree.first;

	if (tree) {
	  bdt = new MVAUtils:: BDT(tree);
	}
	else {
	  ATH_MSG_WARNING("#BTAG# No TTree with name: "<<m_treeName<<" exists in the calibration file.. Disabling algorithm.");
	  m_disableAlgo=true;
	  return StatusCode::SUCCESS;
	}

	// TObjArray* toa= (TObjArray*) f->Get(varStrName.data());
	std::pair<TObject*, bool> calibVariables=m_calibrationTool->retrieveTObject<TObject>(m_taggerNameBase,author,m_taggerNameBase+"Calib/"+m_varStrName);
	TObjArray* toa= (TObjArray*) calibVariables.first;
	std::string commaSepVars="";
	if (toa) {
	  TObjString *tos= 0;
	  if (toa->GetEntries()>0) tos= (TObjString*) toa->At(0);
	  commaSepVars=tos->GetString().Data();
	}

	while (commaSepVars.find(",")!=std::string::npos) {
	  inputVars.push_back(commaSepVars.substr(0,commaSepVars.find(","))); calibNvars++;
	  commaSepVars.erase(0,commaSepVars.find(",")+1);
	}
	inputVars.push_back(commaSepVars.substr(0,-1)); calibNvars++;

	SetVariableRefs(inputVars,tmvaReader,nConfgVar,badVariableFound,inputPointers);
	ATH_MSG_DEBUG("#BTAG# tmvaReader= "<<tmvaReader          <<", nConfgVar"<<nConfgVar
		      <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

	if ( calibNvars!=nConfgVar or badVariableFound ) {
	  ATH_MSG_WARNING( "#BTAG# Number of expected variables for MVA: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << calibNvars << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
	  m_disableAlgo=true;
    delete bdt;
	  return StatusCode::SUCCESS;
	}

	bdt->SetPointers(inputPointers);

	it_egammaBDT = m_egammaBDTs.find(alias);
	if(it_egammaBDT!=m_egammaBDTs.end()) {
	  delete it_egammaBDT->second;
	  m_egammaBDTs.erase(it_egammaBDT);
	}
	m_egammaBDTs.insert( std::make_pair( alias, bdt ) );

      }
    }//calib has changed

    //if(!m_calibrationTool->updatedTagger(m_taggerNameBase, alias, m_taggerNameBase+"Calib", name())) {
    //  if(iss.str().size()>0){
      //  m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase,alias, m_taggerNameBase+"Calib", false, name());

    //....
    //the jet
    double jeteta = jetToTag.eta(), jetphi = jetToTag.phi(), jetpt = jetToTag.pt();
    m_jetpt = jetpt;
    ATH_MSG_DEBUG("#BTAG# Jet properties : eta = " << jeteta
                  << " phi = " << jetphi << " pT  = " <<jetpt/GeV);
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

    bool status = true;

    status &= BTag->variable<float>(m_secVxFinderName, "normdist", distnorm);
    status &= BTag->variable<int>(m_secVxFinderName, "nvsec", msv_n);
    std::vector< ElementLink< xAOD::VertexContainer > > msvVertices;
    status &= BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_secVxFinderName, "vertices", msvVertices);
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

      int diffntrkSV1 = -999;
      int SV1ntrk  = 0;
      std::vector< ElementLink< xAOD::VertexContainer > > SV1Vertice;
      status &= BTag->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_sv1_infosource, "vertices", SV1Vertice);
      if (SV1Vertice.size()>0 && SV1Vertice[0].isValid()){
         status &= BTag->taggerInfo(SV1ntrk, xAOD::BTagInfo::SV1_NGTinSvx);
         diffntrkSV1 = all_trks - SV1ntrk;
      }else{ diffntrkSV1 = all_trks;
      }

      int diffntrkSV0 = diffntrkSV1;

      if (!status) {
        ATH_MSG_WARNING("Error retrieving input values; results will be incorrect!");
      }

      m_diffntrkSV0 = diffntrkSV0;
      m_diffntrkSV1 = diffntrkSV1;
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
                               ", diffntrkSV1= "   << m_diffntrkSV1 <<
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
      if(!m_useEgammaMethodMultiSV){
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
	  }else ATH_MSG_WARNING("#BTAG#  kl==0 for alias, author: "<<alias<<" "<<author);
	}
      }
      else {
	it_egammaBDT = m_egammaBDTs.find(alias);
	if(it_egammaBDT==m_egammaBDTs.end()) {
	  int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
	  if(0==alreadyWarned) {
	    ATH_MSG_WARNING("#BTAG# no egammaBDT defined for jet collection alias, author: "<<alias<<" "<<author);
	    m_undefinedReaders.push_back(alias);
	  }
	}
	else{
	  if(it_egammaBDT->second !=0) {
	    msvW = GetClassResponse(it_egammaBDT->second);
	    ATH_MSG_DEBUG("#BTAG# BB weight: "<<m_taggerNameBase<<" "<< msvW);
	  }else ATH_MSG_WARNING("#BTAG# egamma BDT is 0 for alias, author: "<<alias<<" "<<author);
	}
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

  void MultiSVTag::SetVariableRefs(const std::vector<std::string> inputVars, TMVA::Reader* tmvaReader, unsigned &nConfgVar, bool &badVariableFound, std::vector<float*> &inputPointers) {

    if (!m_useEgammaMethodMultiSV) {
      if(!tmvaReader) {
	ATH_MSG_WARNING("#BTAG# tmva method is chosen but tmvaReader==0!!");
	return;
      }
    }

    //std::cout<<"MultiSV input vars: ";
    for (unsigned ivar=0; ivar<inputVars.size(); ivar++) {
      //std::cout<<inputVars.at(ivar)<<", ";
      if      (inputVars.at(ivar)=="pt"               ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_jetpt       ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_jetpt       ); nConfgVar++; }
      else if (inputVars.at(ivar)=="Nvtx"             ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_nvtx        ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_nvtx        ); nConfgVar++; }
      else if (inputVars.at(ivar)=="MaxEfrc"          ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_maxefrc     ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_maxefrc     ); nConfgVar++; }
      else if (inputVars.at(ivar)=="sumMass"          ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_summass     ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_summass     ); nConfgVar++; }
      else if (inputVars.at(ivar)=="totalntrk"        ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_totalntrk   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_totalntrk   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="diffntrkSV0"      ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_diffntrkSV0 ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_diffntrkSV0 ); nConfgVar++; }
      else if (inputVars.at(ivar)=="diffntrkSV1"      ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_diffntrkSV1 ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_diffntrkSV1 ); nConfgVar++; }
      else if (inputVars.at(ivar)=="normDist"         ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_normDist    ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_normDist    ); nConfgVar++; }
      else if (inputVars.at(ivar)=="maxVtxMass"       ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmax_mass   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmax_mass   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="maxSecVtxMass"    ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmx2_mass   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmx2_mass   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="EfrcmaxVtxMass"   ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmax_efrc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmax_efrc   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="EfrcmaxSecVtxMass") { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmx2_efrc   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmx2_efrc   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="dlsmaxVtxMass"    ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmax_dist   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmax_dist   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="dlsmaxSecVtxMass" ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmx2_dist   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmx2_dist   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="dRmaxVtxMassj"    ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmax_DRjet  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmax_DRjet  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="dRmaxSecVtxMassj" ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mmx2_DRjet  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mmx2_DRjet  ); nConfgVar++; }
      else if (inputVars.at(ivar)=="d2Mass12"         ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mx12_2d12   ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mx12_2d12   ); nConfgVar++; }
      else if (inputVars.at(ivar)=="DRMass12"         ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mx12_DR     ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mx12_DR     ); nConfgVar++; }
      else if (inputVars.at(ivar)=="AngleMass12"      ) { m_useEgammaMethodMultiSV ? inputPointers.push_back(&m_mx12_Angle  ) : tmvaReader->AddVariable(inputVars.at(ivar).data(),&m_mx12_Angle  ); nConfgVar++; }
      else {
	ATH_MSG_WARNING( "#BTAG# \""<<inputVars.at(ivar)<<"\" <- This variable found in xml/calib-file does not match to any variable declared in MultiSV... the algorithm will be 'disabled'.");//<<alias<<" "<<author);
	badVariableFound=true;
      }
    }
    //std::cout<<std::endl;

  }

}//end namespace
