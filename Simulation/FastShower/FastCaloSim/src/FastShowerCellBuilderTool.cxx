/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/FastShowerCellBuilderTool.h"
//#include "FastCaloSim/TransportedHelixParticle.h"
#include "FastCaloSim/ParticleEnergyParametrization.h"
#include "FastCaloSim/TShape_Result.h"
#include "FastCaloSim/TLateralShapeCorrection.h"
#include "FastCaloSim/TSplineReweight.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#if FastCaloSim_project_release_v1 == 12
#include "PartPropSvc/PartPropSvc.h"
#include "CLHEP/HepPDT/ParticleData.hh"
#else
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleData.hh"
#endif
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
//#include "TruthHelper/IsGenStable.h"
//#include "TruthHelper/IsGenerator.h"
//#include "TruthHelper/IsGenInteracting.h"
//#include "TruthHelper/IsGenNonInteracting.h"
//#include "TruthHelper/IsGenSimulStable.h"
//#include "FastCaloSim/FastCaloSimIsGenSimulStable.h"
//#include "TruthHelper/IsGenNonInteracting.h"

#include "PathResolver/PathResolver.h"
#include "AthenaKernel/RNGWrapper.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"

//#include "AtlfastAlgs/GlobalEventData.h"
//#include "AtlfastUtils/TesIO.h"
//#include "AtlfastUtils/HepMC_helper/IMCselector.h"
//#include "AtlfastEvent/MCparticleCollection.h"

//extrapolation
#include "CaloDetDescr/CaloDepthTool.h"
//#include "TrkParameters/Perigee.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "CaloTrackingGeometry/ICaloSurfaceHelper.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
//#include "TruthHelper/PileUpType.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkGeometry/TrackingGeometry.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrCondTools/ICoolHistSvc.h"

#include "TROOT.h"
#include "TClass.h"
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TKey.h>
#include <TRandom3.h>
#include <TGraphErrors.h>
#include <TVector3.h>
#include <TFile.h>
#include <TDirectory.h>
#include "TVector2.h"
#include "TMath.h"
#include "TSpline.h"

#include "TruthUtils/HepMCHelpers.h"
#include "GeneratorObjects/McEventCollection.h"

//using namespace Atlfast;
//using namespace FastShower;

#include "FastCaloSimAthenaPool/FastShowerInfo.h"
#include "FastCaloSimAthenaPool/FastShowerInfoContainer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <limits>

typedef std::vector<const HepMC::GenParticle*>  MCparticleCollection ;

//This is a copy of the previous isGenSimulStable which depended on TruthHelper
bool FastCaloSimIsGenSimulStable(const HepMC::GenParticle* p) {
  int status=p->status();
  HepMC::GenVertex* vertex = p->end_vertex();
  // we want to keep primary particle with status==2 but without vertex in HepMC
  int vertex_barcode=-999999;
  if (vertex) vertex_barcode=vertex->barcode();

  return (
          (status%1000 == 1) ||
          (status%1000 == 2 && status > 1000) ||
          (status==2 && vertex_barcode<-200000)
          ) ? true:false;
}



FastShowerCellBuilderTool::FastShowerCellBuilderTool(const std::string& type, const std::string& name, const IInterface* parent)
  : BasicCellBuilderTool(type, name, parent)
  , m_AdditionalParticleParametrizationFileNames(0)
  , m_DB_folder(0)
  , m_DB_channel(0)
  , m_DB_dirname(0)
  , m_coolhistsvc("CoolHistSvc", name)
  , m_partPropSvc("PartPropSvc", name)
  , m_rndmSvc("AthRNGSvc", name)
  , m_extrapolator("")
  , m_caloSurfaceHelper("")
  , m_calo_tb_coord("TBCaloCoordinate")
  , m_sampling_energy_reweighting(CaloCell_ID_FCS::MaxSample,1.0)
  , m_invisibles(0)
{
  const int n_surfacelist=5;
  CaloCell_ID_FCS::CaloSample surfacelist[n_surfacelist]={CaloCell_ID_FCS::PreSamplerB,
                                                          CaloCell_ID_FCS::PreSamplerE,
                                                          CaloCell_ID_FCS::EME1,
                                                          CaloCell_ID_FCS::EME2,
                                                          CaloCell_ID_FCS::FCAL0
  };
  m_n_surfacelist=n_surfacelist;
  for(int i=0;i<n_surfacelist;++i) m_surfacelist[i]=surfacelist[i];

  declareProperty("ParticleParametrizationFileName",m_ParticleParametrizationFileName);
  declareProperty("AdditionalParticleParametrizationFileNames",m_AdditionalParticleParametrizationFileNames);

  declareProperty("CoolHistSvc",                    m_coolhistsvc,          "");
  declareProperty("PartPropSvc",                    m_partPropSvc,          "");
  declareProperty("RandomService",                  m_rndmSvc,              "Name of the random number service");
  declareProperty("RandomStreamName",               m_randomEngineName,     "Name of the random number stream");
  declareProperty("DoSimulWithInnerDetectorTruthOnly",m_simul_ID_only);
  declareProperty("DoSimulWithInnerDetectorV14TruthCuts",m_simul_ID_v14_truth_cuts);
  declareProperty("DoSimulWithEMGeantInteractionsOnly",m_simul_EM_geant_only);
  declareProperty("DoSimulHeavyIonsInCalo",         m_simul_heavy_ions);

  declareProperty("Extrapolator",                   m_extrapolator );
  declareProperty("CaloEntrance",                   m_caloEntranceName );
  declareProperty("CaloSurfaceHelper",              m_caloSurfaceHelper );
  declareProperty("CaloCoordinateTool",             m_calo_tb_coord);

  declareProperty("StoreFastShowerInfo",            m_storeFastShowerInfo);

  declareProperty("DoEnergyInterpolation",          m_jo_interpolate); //ATA: make interpolation optional
  declareProperty("DoNewEnergyEtaSelection",        m_energy_eta_selection); //mwerner: make new selction of EnergyParam optional
  declareProperty("use_Ekin_for_depositions",       m_use_Ekin_for_depositions);//Use the kinetic energy of a particle to as measure of the energie to deposit in the calo

  //declareProperty("spline_reweight_x",       m_spline_reweight_x);
  //declareProperty("spline_reweight_y",       m_spline_reweight_y);

  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i) m_sampling_energy_reweighting[i]=1.0;
  declareProperty("sampling_energy_reweighting",m_sampling_energy_reweighting);

  m_invisibles.push_back(0);
  /*
    m_invisibles.push_back(12);
    m_invisibles.push_back(14);
    m_invisibles.push_back(16);
    m_invisibles.push_back(1000022);
    m_invisibles.push_back(1000039);
  */

  declareProperty("Invisibles",m_invisibles);

  m_ID_cylinder_r.push_back(1070);
  m_ID_cylinder_z.push_back(800);
  m_ID_cylinder_r.push_back(1030);
  m_ID_cylinder_z.push_back(3400);

  declareProperty("ID_cut_off_r",m_ID_cylinder_r);
  declareProperty("ID_cut_off_z",m_ID_cylinder_z);
}


FastShowerCellBuilderTool::~FastShowerCellBuilderTool()
{
  for (auto* el : m_shape_correction ) delete el;
  m_shape_correction.clear();
  for (auto const& m1 : m_map_ParticleShapeParametrizationMap ) {
    for (auto const& m2 : m1.second ) {
      for (auto const& m3 : m2.second ) {
        for (auto* el : m3.second ) {
          delete el;
        }
      }
    }
  }
  for (auto const& m1 : m_map_ParticleEnergyParametrizationMap ) {
    for (auto const& m2 : m1.second ) {
      for (auto const& m3 : m2.second ) {
        delete m3.second;
      }
    }
  }
}

void FastShowerCellBuilderTool::LoadParametrizationsFromDir(std::string dir)
{
  TString curdir=gSystem->pwd();
  TString dirname=dir.c_str();
  TSystemDirectory d(dirname,dirname);
  TList* files=d.GetListOfFiles();
  for(int i=0;i<files->GetSize();++i) if(files->At(i)){
      //    cout<<i<<" : "<<files->At(i)->GetName()<<" = "<<files->At(i)->GetTitle()<<endl;
      TString name=files->At(i)->GetName();
      if(name.Index(".root")==kNPOS) continue;
      name.ReplaceAll(".root","");
      TFile* infile = TFile::Open(files->At(i)->GetName());
      if(infile && infile->IsOpen()) {
        LoadParametrizationsFromFile(*infile);
        infile->Close();
        delete infile;
      }
    }
  gSystem->cd(curdir);
}


class shape_count_info {
public:
  int n;
  double min_eta,max_eta;
  double min_E,max_E;

  shape_count_info():n(0),min_eta(1000),max_eta(-1),min_E(1000000000),max_E(-1) {};
};

bool checkParticleEnergyParametrization(ParticleEnergyParametrization* param, TString& msg)
{
  if(!param) {
    return false;
  }

  /*
  //The parametrizations for param->DistPara(0) and param->DistPara(>m_Ecal_vs_dist->GetNbinsX()) should never be used
  //Could be deleted, but to be safe lets not touch them now extensively...
  ParticleEnergyParametrizationInDistbin* inbin=param->DistPara(0);
  if(inbin) inbin->m_corr.ResizeTo(0,0);
  ParticleEnergyParametrizationInDistbin* inbin=param->DistPara(m_Ecal_vs_dist->GetNbinsX()+1);
  if(inbin) inbin->m_corr.ResizeTo(0,0);
  */

  for(int i=1;i<=param->GetNDistBins();++i) {
    ParticleEnergyParametrizationInDistbin* inbin=param->DistPara(i);
    if(!inbin) {
      msg+=Form("DistPara(%d) not filled. ",i);
      continue;
    }
    for(int j=-2;j<0;++j) {
      if(isnan(inbin->m_mean(j))) {
        inbin->m_corr.ResizeTo(0,0);
        msg+=Form("mean(%d,%d) is nan, fixed by removing correlation matrix, n=%d elements. ",i,j,inbin->m_corr.GetNoElements());
        break;
      }
      if(isnan(inbin->m_RMS(j))) {
        inbin->m_corr.ResizeTo(0,0);
        msg+=Form("RMS(%d,%d) is nan, fixed by removing correlation matrix, n=%d elements. ",i,j,inbin->m_corr.GetNoElements());
        break;
      }
    }
    for(int j=0;j<CaloCell_ID_FCS::MaxSample;++j) {
      if(inbin->m_ElayerProp[j]) {
        if(isnan(inbin->m_mean(j))) {
          msg+=Form("mean(%d,%d) is nan, not fixable! ",i,j);
          return false;
        }
      }
    }
    for(int j=-2;j<CaloCell_ID_FCS::MaxSample;++j) {
      if(inbin->m_corr.GetNoElements()==0) break;
      for(int k=-2;k<CaloCell_ID_FCS::MaxSample;++k) {
        if(isnan(inbin->m_corr(j,k))) {
          inbin->m_corr.ResizeTo(0,0);
          msg+=Form("corr(%d,%d,%d) is nan, fixed by removing correlation matrix, n=%d elements. ",i,j,k,inbin->m_corr.GetNoElements());
          break;
        }
      }
    }
  }
  return true;
}

void FastShowerCellBuilderTool::LoadParametrizationsFromFile(TDirectory& infile,MSG::Level level)
{
  TIterator *iter=infile.GetListOfKeys()->MakeIterator();
  if (!iter) return; // This should really not happen
  iter->Reset();

  std::map< int,shape_count_info >   n_energy,n_shape;
  while(TKey *key=(TKey*)(iter->Next()))
    {
      TClass *cl=gROOT->GetClass(key->GetClassName());
      if(cl->InheritsFrom(TLateralShapeCorrectionBase::Class()))
        {
          TLateralShapeCorrectionBase* obj=(TLateralShapeCorrectionBase*)(key->ReadObj());
          if(obj) {
            m_shape_correction.push_back(obj);
            ATH_MSG_LVL(level," -> Got TLateralShapeCorrectionBase obj "<<obj->GetName()<<":"<<obj->GetTitle()<<"="<<obj->str() );
          }
        }
      if(cl->InheritsFrom(TShape_Result::Class()))
        {
          TShape_Result* obj=(TShape_Result*)(key->ReadObj());
          if(obj) {
            //        cout<<" -> Got obj "<<obj->GetName()<<" : "<<obj->GetTitle()<<endl;

            //ID              Energy             Eta                Dist
            //std::map< int , std::map< double , std::map< double , std::map< double , TShape_Result* > > > > m_map_ParticleShapeParametrizationMap;

            m_map_ParticleShapeParametrizationMap[obj->id()][obj->calosample()][obj->E()].push_back(obj);
            ++n_shape[obj->id()].n;
            n_shape[obj->id()].min_eta=TMath::Min(n_shape[obj->id()].min_eta,obj->eta());
            n_shape[obj->id()].max_eta=TMath::Max(n_shape[obj->id()].max_eta,obj->eta());
            n_shape[obj->id()].min_E  =TMath::Min(n_shape[obj->id()].min_E  ,obj->E()  );
            n_shape[obj->id()].max_E  =TMath::Max(n_shape[obj->id()].max_E  ,obj->E()  );
          }
        }
      if(cl->InheritsFrom(ParticleEnergyParametrization::Class()))
        {
          ParticleEnergyParametrization* obj=(ParticleEnergyParametrization*)(key->ReadObj());
          TString msg;
          if(checkParticleEnergyParametrization(obj,msg)) {
            //        cout<<" -> Got obj "<<obj->GetName()<<" : "<<obj->GetTitle()<<endl;
            obj->SetNoDirectoryHisto();

            //ID              Energy             Eta
            //std::map< int , std::map< double , std::map< double , ParticleEnergyParametrization* > > > m_map_ParticleEnergyParametrizationMap;

            m_map_ParticleEnergyParametrizationMap[obj->id()][obj->E()][obj->eta()]=obj;
            ++n_energy[obj->id()].n;
            n_energy[obj->id()].min_eta=TMath::Min(n_energy[obj->id()].min_eta,obj->eta());
            n_energy[obj->id()].max_eta=TMath::Max(n_energy[obj->id()].max_eta,obj->eta());
            n_energy[obj->id()].min_E  =TMath::Min(n_energy[obj->id()].min_E  ,obj->E()  );
            n_energy[obj->id()].max_E  =TMath::Max(n_energy[obj->id()].max_E  ,obj->E()  );
            if(msg!="") {
              ATH_MSG_WARNING("Could fix some nan in input parametrization "<<obj->GetName()<<" ("<<obj->GetTitle()<<"): "<<msg.Data());
            }
          } else if(obj) {
            ATH_MSG_WARNING("Found nan in input parametrization "<<obj->GetName()<<" ("<<obj->GetTitle()<<"): "<<msg.Data());
          }
        }
    }
  if (iter) delete iter;
  iter=0;

  for(std::map< int,shape_count_info >::iterator i=n_energy.begin();i!=n_energy.end();++i) {
    ATH_MSG_LVL(level,"     Energy parametrization id="<<i->first<<" : "<<i->second.n<<" parametrizations loaded: "<<i->second.min_eta<<"<|eta|<"<<i->second.max_eta<<" ; "<<i->second.min_E<<"<E<"<<i->second.max_E );
  }
  for(std::map< int,shape_count_info >::iterator i=n_shape.begin();i!=n_shape.end();++i) {
    ATH_MSG_LVL(level,"     Shape  parametrization id="<<i->first<<" : "<<i->second.n<<" parametrizations loaded: "<<i->second.min_eta<<"<|eta|<"<<i->second.max_eta<<" ; "<<i->second.min_E<<"<E<"<<i->second.max_E );
  }

}


StatusCode FastShowerCellBuilderTool::initialize()
{
  ATH_MSG_INFO("Initialisating started");

  ATH_CHECK(BasicCellBuilderTool::initialize());

  ATH_CHECK(m_partPropSvc.retrieve());

  m_particleDataTable = (HepPDT::ParticleDataTable*) m_partPropSvc->PDT();
  if(!m_particleDataTable) {
    ATH_MSG_ERROR("PDG table not found");
    return StatusCode::FAILURE;
  }

  // Random number service
  ATH_CHECK(m_rndmSvc.retrieve());

  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->getEngine(this, m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_ERROR("Could not get random engine '" << m_randomEngineName << "'");
    return StatusCode::FAILURE;
  }

  // Get TimedExtrapolator
  if (m_extrapolator.empty()) {
    ATH_MSG_DEBUG("No Extrapolator specified.");
  }
  else {
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_MSG_DEBUG("Extrapolator retrieved "<< m_extrapolator);
  }

  // Get CaloSurfaceHelper
  ATH_CHECK(m_caloSurfaceHelper.retrieve());

  ATH_CHECK(m_calo_tb_coord.retrieve());
  ATH_MSG_INFO("retrieved " << m_calo_tb_coord.name());

  //#if FastCaloSim_project_release_v1 == 12
  //m_calosurf_middle->setCaloDepth(m_calodepth);
  //#endif

  //#if FastCaloSim_project_release_v1 == 12
  //m_calosurf_entrance->setCaloDepth(m_calodepthEntrance);
  //#endif

  find_phi0();

  ATH_MSG_INFO("========================= Init EM map =============================");
  m_em_map.init(-5,+5,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,100,64);
  m_em_map.setname("EM");

  ATH_MSG_INFO("========================= Init EM fine map ========================");
  m_em_fine_map.init(-2.8,+2.8,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,224,256);
  m_em_fine_map.setname("EM fine");

  ATH_MSG_INFO("========================= Init HAD map ============================");
  m_had_map.init(-5,+5,-M_PI+m_phi0_had,+M_PI+m_phi0_had,100,64);
  m_had_map.setname("HAD");

  ATH_MSG_INFO("========================= Init EM celllist map =============================");
  m_em_celllist_map.init(-5,+5,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,100,64,2,2);
  m_em_celllist_map.setname("EMlist");

  ATH_MSG_INFO("========================= Init celllist maps sample 0 ... "<< CaloCell_ID_FCS::LastSample);
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    //log << MSG::INFO <<  "========================= Init celllist map sample "<<sample<<" =============================" <<endmsg;
    m_celllist_maps[sample].init(-5,+5,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,100,64,3,3);
    m_celllist_maps[sample].setname("samplecelllist");
    //    m_celllist_maps[sample];
  }

  init_all_maps();
  ATH_MSG_INFO("========================= Init volume all maps =========================");
  init_volume(m_em_map);
  init_volume(m_em_fine_map);
  init_volume(m_had_map);

  /*
    if(m_mcLocation=="") {
    log << MSG::INFO <<  "========================= Atlfast::GlobalEventData::Instance() =========================" <<endmsg;
    Atlfast::GlobalEventData* ged = Atlfast::GlobalEventData::Instance();
    m_mcLocation       = ged -> mcLocation();
    }
  */
  ATH_MSG_INFO("McCollection="<< m_mcCollectionKey.key());

  //m_gentesIO = new GenAccessIO();


  // get the CoolHistSvc
  ATH_CHECK(m_coolhistsvc.retrieve());

  /*
    if(m_spline_reweight_x.size()>0 && m_spline_reweight_x.size()==m_spline_reweight_y.size()) {
    TEtaSplineReweight* cor_ele=new TEtaSplineReweight("EtaEleCorrection","EtaEleCorrection");
    TSpline3* sp=new TSpline3("",&m_spline_reweight_x[0],&m_spline_reweight_y[0],m_spline_reweight_x.size(),"b1e1",0,0);
    cor_ele->SetSpline(sp);
    cor_ele->m_pdgid=11;
    cor_ele->m_calosample=2;
    cor_ele->m_min_eta=-1.37;
    cor_ele->m_max_eta=+1.37;
    cor_ele->m_min_p=0;
    cor_ele->m_max_p=20000000;
    cor_ele->m_min_pT=0;
    cor_ele->m_max_pT=20000000;
    m_shape_correction.push_back(cor_ele);

    TEtaSplineReweight* cor_gamma=new TEtaSplineReweight("EtaGammaCorrection","EtaGammaCorrection");
    *cor_gamma=*cor_ele;
    cor_gamma->m_pdgid=22;
    m_shape_correction.push_back(cor_gamma);
    }
  */

  if (OpenParamSource(m_ParticleParametrizationFileName).isFailure()) {
    ATH_MSG_WARNING("Open of "<<m_ParticleParametrizationFileName<<" failed");
  }
  for(unsigned int isource=0;isource<m_AdditionalParticleParametrizationFileNames.size();++isource) {
    if (OpenParamSource(m_AdditionalParticleParametrizationFileNames[isource]).isFailure()) {
      ATH_MSG_WARNING("Open of "<<m_AdditionalParticleParametrizationFileNames[isource]<<" failed");
    }
  }

  for(int i=0;i<CaloCell_ID_FCS::MaxSample;++i) {
    if(fabs(m_sampling_energy_reweighting[i]-1.0)>0.001) {
      ATH_MSG_INFO("Apply sampling reweight factor "<<m_sampling_energy_reweighting[i]<<" to sampling "<<i);
    }
  }

  msg(MSG::INFO) <<"Invisible particles (n="<<m_invisibles.size()<<"): ";
  for(unsigned int i=0;i<m_invisibles.size();++i) {
    if(m_invisibles[i]==0) msg(MSG::INFO)<<"(pdgid=0 -> Use TruthHelper class IsGenNonInteracting), ";
    else msg(MSG::INFO)<<"(pdgid="<<m_invisibles[i]<<"), ";
  }
  msg(MSG::INFO)<<endmsg;

  if(m_simul_ID_only) {
    msg(MSG::INFO) <<"Reject ID truth particle vertices outside cylinder(s): ";
    int nc=TMath::Min(m_ID_cylinder_r.size(),m_ID_cylinder_z.size());
    for(int ic=0;ic<nc;++ic) {
      msg(MSG::INFO)<<"[r="<<m_ID_cylinder_r[ic]<<",z="<<m_ID_cylinder_z[ic]<<"] ";
    }
    msg(MSG::INFO)<<endmsg;
  }

  init_shape_correction();

  ATH_CHECK( m_mcCollectionKey.initialize() );

  if (m_storeFastShowerInfo) {
    ATH_CHECK( m_FastShowerInfoContainerKey.initialize() );
  }
  else {
    m_FastShowerInfoContainerKey = "";
  }

  if (!m_MuonEnergyInCaloContainerKey.key().empty()) {
    ATH_CHECK( m_MuonEnergyInCaloContainerKey.initialize() );
  }

  ATH_MSG_INFO("Initialisating finished");
  return StatusCode::SUCCESS;
}

StatusCode FastShowerCellBuilderTool::OpenParamSource(std::string insource)
{
  if(insource=="") return StatusCode::SUCCESS;

  if(insource.find("DB=")==0) {
    const unsigned int maxdbINFOoutput=2;
    if(m_DB_folder.size()>=maxdbINFOoutput && ( !msgLvl(MSG::DEBUG) ) ) {
      if(m_DB_folder.size()==maxdbINFOoutput) ATH_MSG_INFO("... skipping extra INFO output for further DB registration ...");
    } else {
      ATH_MSG_DEBUG("Register Parametrization from DB : "<< insource);
    }
    insource.erase(0,3);

    std::string::size_type strpos=insource.find(":");
    if(strpos==std::string::npos) {
      ATH_MSG_WARNING("Could not parse string for database entry : "<< insource);
      return StatusCode::SUCCESS;
    }
    std::string cool_folder=insource.substr(0,strpos);
    ATH_MSG_DEBUG("  folder "<< cool_folder);
    insource.erase(0,strpos+1);

    strpos=insource.find(":");
    std::string str_cool_channel=insource.substr(0,strpos);
    if(strpos==std::string::npos) {
      ATH_MSG_WARNING("Could not parse string for database entry "<< insource);
      return StatusCode::SUCCESS;
    }
    char *endptr;
    int cool_channel=strtol(str_cool_channel.c_str(), &endptr, 0);
    if (endptr[0] != '\0') {
      ATH_MSG_ERROR("Could not convert string to int: " << str_cool_channel);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("  channel "<< cool_channel);
    insource.erase(0,strpos+1);

    std::string cool_object=insource;
    ATH_MSG_DEBUG("  object "<< cool_object);

    m_DB_folder.push_back(cool_folder);
    m_DB_channel.push_back(cool_channel);
    m_DB_dirname.push_back(cool_object);

    for(unsigned int icool=0;icool<m_DB_folder.size()-1;++icool) {
      if(cool_folder==m_DB_folder[icool]) {
        ATH_MSG_DEBUG("  IOV callback from FastShowerCellBuilderTool already registered");
        return StatusCode::SUCCESS;
      }
    }

    // register IOV callback function for the COOL folder
    const DataHandle<CondAttrListCollection> aptr;
    if (detStore()->regFcn(&FastShowerCellBuilderTool::callBack,this,aptr,cool_folder).isFailure()) {
      ATH_MSG_WARNING("  Registration of IOV callback failed for folder "<< cool_folder);
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_INFO("  Registered IOV callback from FastShowerCellBuilderTool for folder "<< cool_folder);
    }
  } else {
    std::string ParticleParametrizationFile = PathResolver::find_file(insource, "DATAPATH");
    if ( ParticleParametrizationFile == "" ) {
      ATH_MSG_WARNING("Can't find parametrization file " << insource << " in DATAPATH");
      return StatusCode::SUCCESS;
    }

    TFile* infile=TFile::Open(ParticleParametrizationFile.c_str(),"READ");
    if(infile) {
      //log << MSG::INFO <<  "infile non zero" <<endmsg;
      //infile->ls();
      //log << MSG::INFO <<  "open="<<infile->IsOpen()<<endmsg;
      if(infile->IsOpen()) {
        ATH_MSG_INFO("==== LoadParametrizations from file : "<<ParticleParametrizationFile<<" ====");
        TDirectory* dir;

        dir=(TDirectory*)infile->Get("EnergyResults");
        if(dir) {
          ATH_MSG_INFO("  LoadParametrizations : EnergyResults");
          LoadParametrizationsFromFile(*dir);
        } else {
          ATH_MSG_INFO("  Can't find directory EnergyResults in parametrization file");
        }

        dir=(TDirectory*)infile->Get("ShapeResults");
        if(dir) {
          ATH_MSG_INFO("  LoadParametrizations : ShapeResults");
          LoadParametrizationsFromFile(*dir);
        } else {
          ATH_MSG_INFO("  Can't find directory ShapeResults in parametrization file");
        }

        dir=(TDirectory*)infile->Get("ShapeCorrection");
        if(dir) {
          ATH_MSG_INFO("  LoadParametrizations : ShapeCorrection");
          LoadParametrizationsFromFile(*dir);
        }
      } else {
        ATH_MSG_WARNING("Can't open parametrization file " << ParticleParametrizationFile);
      }
    } else {
      ATH_MSG_WARNING("Can't find parametrization file " << ParticleParametrizationFile);
    }
    if(infile) {
      infile->Close();
      delete infile;
      infile=0;
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode FastShowerCellBuilderTool::callBack( IOVSVC_CALLBACK_ARGS_P( I, keys) )
{
  // printout the list of keys invoked - will normally only be for our
  // histogram folder
  msg(MSG::INFO) << "FastShowerCellBuilderTool callback invoked for I: "<<I<< " folder: ";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) msg(MSG::INFO) << *itr << " ";
  msg(MSG::INFO) << endmsg;
  // check all the keys, if we find the histogram folder, update the pointer

  MSG::Level level=MSG::INFO;
  int nprint=0;

  for (std::list<std::string>::const_iterator itr=keys.begin();itr!=keys.end();++itr) {
    for(unsigned int icool=0;icool<m_DB_folder.size();++icool) {
      if (*itr==m_DB_folder[icool]) {
        TObject* odir;
        if (m_coolhistsvc->getTObject(m_DB_folder[icool],m_DB_channel[icool],m_DB_dirname[icool],odir).isSuccess()) {
          if (odir!=0) {
            TDirectory* dir=(TDirectory*)odir;
            if(nprint>1) {
              if(level==MSG::INFO) ATH_MSG_LVL(level,"==== ...skipping further INFO output for LoadParametrizations from DB ====");
              level=MSG::DEBUG;
            }
            ATH_MSG_LVL(level,"==== LoadParametrizations from DB: "<<m_DB_folder[icool]<<":"<<m_DB_channel[icool]<<":"<<m_DB_dirname[icool]<<" ====");
            LoadParametrizationsFromFile(*dir,level);
            TDirectory* mdir=dir->GetMotherDir();
            if(mdir) {
              ATH_MSG_DEBUG("  removing "<<m_DB_folder[icool]<<":"<<m_DB_channel[icool]<<":"<<m_DB_dirname[icool]<<" from DirList to prevent crash in finalize");
              mdir->GetList()->Remove(dir);
              dir->SetMother(0);
            }
            ++nprint;
          } else {
            ATH_MSG_FATAL("Can't find "<<m_DB_folder[icool]<<":"<<m_DB_channel[icool]<<":"<<m_DB_dirname[icool]<<" in parametrization file, but DB access OK");
            return StatusCode::FAILURE;
          }
        } else {
          ATH_MSG_FATAL("Can't find "<<m_DB_folder[icool]<<":"<<m_DB_channel[icool]<<":"<<m_DB_dirname[icool]<<" in parametrization file, DB access failed");
          return StatusCode::FAILURE;
        }
        m_DB_folder[icool]="DONT_USE_AGAIN";
      }
    }
  }
  return StatusCode::SUCCESS;
}

ParticleEnergyParametrization* FastShowerCellBuilderTool::findElower(int id,double E,double eta) const
{
  t_map_PEP_ID::const_iterator iter_id=m_map_ParticleEnergyParametrizationMap.find(id);
  if(iter_id!=m_map_ParticleEnergyParametrizationMap.end()) {
    ATH_MSG_DEBUG("ID found="<<iter_id->first);
    t_map_PEP_Energy::const_iterator iter_E=iter_id->second.lower_bound(E);
    if(iter_E==iter_id->second.end()) iter_E--;
    if(iter_E!=iter_id->second.end()) {
      if(iter_E->first>=E && iter_E!=iter_id->second.begin()) iter_E--;
      ATH_MSG_DEBUG("E found="<<iter_E->first);
      // first para_eta > fabs_eta  !! might be wrong !!
      double aeta=fabs(eta);
      t_map_PEP_Eta::const_iterator iter_eta=iter_E->second.lower_bound(aeta);

      if(iter_eta!=iter_E->second.begin())  iter_eta--;
      if(m_energy_eta_selection){
        ATH_MSG_DEBUG(" new eta selection for energy paramertization is used ");
        if(iter_eta==iter_E->second.end()) iter_eta--;
        if(iter_eta!=iter_E->second.end()) {

          t_map_PEP_Eta::const_iterator best(iter_eta);
          double deta_best=fabs(best->first - aeta);
          while(iter_eta->first < aeta ) {
            iter_eta++;
            if(iter_eta!=iter_E->second.end()) {
              if(fabs(iter_eta->first-aeta) < deta_best) {
                best=iter_eta;
                deta_best=fabs(best->first-aeta);
              }
            } else break;
          }

          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta found="<<best->first);
            ATH_MSG_DEBUG(best->second->GetName()<<" : "<<best->second->GetTitle());
          }
          return best->second;
        } else {
          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta="<<eta<<" not found : size="<<iter_E->second.size());
            ATH_MSG_DEBUG("begin="<<iter_E->second.begin()->first);
            ATH_MSG_DEBUG("rbegin="<<iter_E->second.rbegin()->first);
          }
          return 0;
        }
      } else {
        if(iter_eta!=iter_E->second.end()) {
          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta found="<<iter_eta->first);
            ATH_MSG_DEBUG(iter_eta->second->GetName()<<" : "<<iter_eta->second->GetTitle());
          }
          return iter_eta->second;
        } else {
          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta="<<eta<<" not found : size="<<iter_E->second.size());
            ATH_MSG_DEBUG("begin="<<iter_E->second.begin()->first);
            ATH_MSG_DEBUG("rbegin="<<iter_E->second.rbegin()->first);
          }
          return 0;
        }
      }
    } else {
      if(msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG("E="<<E<<" not found : size="<<iter_id->second.size());
        ATH_MSG_DEBUG("begin="<<iter_id->second.begin()->first);
        ATH_MSG_DEBUG("rbegin="<<iter_id->second.rbegin()->first);
      }
      return 0;
    }
  } else {
    ATH_MSG_DEBUG("ID "<<id<<" not found");
    return 0;
  }
}

ParticleEnergyParametrization* FastShowerCellBuilderTool::findEupper(int id,double E,double eta) const
{
  t_map_PEP_ID::const_iterator iter_id=m_map_ParticleEnergyParametrizationMap.find(id);
  if(iter_id!=m_map_ParticleEnergyParametrizationMap.end()) {
    ATH_MSG_DEBUG("ID found="<<iter_id->first);
    t_map_PEP_Energy::const_iterator iter_E=iter_id->second.lower_bound(E);
    if(iter_E==iter_id->second.end()) iter_E--;
    if(iter_E!=iter_id->second.end()) {
      ATH_MSG_DEBUG("E found="<<iter_E->first);
      double aeta=fabs(eta);
      t_map_PEP_Eta::const_iterator iter_eta=iter_E->second.lower_bound(aeta);
      if(iter_eta!=iter_E->second.begin())  iter_eta--;
      if(m_energy_eta_selection){
        if(iter_eta==iter_E->second.end()) iter_eta--;
        if(iter_eta!=iter_E->second.end()) {

          t_map_PEP_Eta::const_iterator best(iter_eta);
          double deta_best=fabs(best->first - aeta);
          while(iter_eta->first < aeta ) {
            iter_eta++;
            if(iter_eta!=iter_E->second.end()) {
              if(fabs(iter_eta->first-aeta) < deta_best) {
                best=iter_eta;
                deta_best=fabs(best->first-aeta);
              }
            } else break;
          }

          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta found="<<best->first);
            ATH_MSG_DEBUG(best->second->GetName()<<" : "<<best->second->GetTitle());
          }
          return best->second;
        } else {
          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta="<<eta<<" not found : size="<<iter_E->second.size());
            ATH_MSG_DEBUG("begin="<<iter_E->second.begin()->first);
            ATH_MSG_DEBUG("rbegin="<<iter_E->second.rbegin()->first);
          }
          return 0;
        }
      } else {
        if(iter_eta!=iter_E->second.end()) {
          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta found="<<iter_eta->first);
            ATH_MSG_DEBUG(iter_eta->second->GetName()<<" : "<<iter_eta->second->GetTitle());
          }
          return iter_eta->second;
        } else {
          if(msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("eta="<<eta<<" not found : size="<<iter_E->second.size());
            ATH_MSG_DEBUG("begin="<<iter_E->second.begin()->first);
            ATH_MSG_DEBUG("rbegin="<<iter_E->second.rbegin()->first);
          }
          return 0;
        }
      }
    }else {
      if(msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG("E="<<E<<" not found : size="<<iter_id->second.size());
        ATH_MSG_DEBUG("begin="<<iter_id->second.begin()->first);
        ATH_MSG_DEBUG("rbegin="<<iter_id->second.rbegin()->first);
      }
      return 0;
    }
  } else {
    ATH_MSG_DEBUG("ID "<<id<<" not found");
    return 0;
  }
}


const TShape_Result* FastShowerCellBuilderTool::findShape (int id,int calosample,double E,double eta,double dist,double distrange) const
{
  t_map_PSP_ID::const_iterator iter_id=m_map_ParticleShapeParametrizationMap.find(id);
  if(iter_id!=m_map_ParticleShapeParametrizationMap.end()) {
    ATH_MSG_DEBUG("ID found="<<iter_id->first);

    t_map_PSP_calosample::const_iterator iter_cs=iter_id->second.find(calosample);
    if(iter_cs!=iter_id->second.end()) {
      ATH_MSG_DEBUG("calosample found="<<iter_cs->first);

      t_map_PSP_Energy::const_iterator iter_E=iter_cs->second.lower_bound(E);
      if(iter_E==iter_cs->second.end()) iter_E--;
      double edist=fabs(iter_E->first - E);
      if(iter_E!=iter_cs->second.begin()) {
        t_map_PSP_Energy::const_iterator iter_Etest=iter_E;
        iter_Etest--;
        double edisttest=fabs(iter_Etest->first - E);
        if(edisttest<edist) iter_E=iter_Etest;
      }
      if(iter_E!=iter_cs->second.end()) {
        if(msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG("E found="<<iter_E->first);
          ATH_MSG_DEBUG("disteta size="<<iter_E->second.size());
        }

        double bestscore=10000000;
        TShape_Result* best_shape=0;

        for(t_map_PSP_DistEta::const_iterator iter_disteta=iter_E->second.begin();iter_disteta<iter_E->second.end();++iter_disteta) {
          double scoreeta=fabs(((*iter_disteta)->eta()-fabs(eta))/0.2);
          double scoredist=fabs(((*iter_disteta)->meandist()-dist)/distrange);
          double score=scoreeta+scoredist;
          if(!best_shape || score<bestscore) {
            bestscore=score;
            best_shape=(*iter_disteta);
          }
        }

        if(best_shape) {
          ATH_MSG_DEBUG("best parametrization found : "<<best_shape->GetName()<<" = "<<best_shape->GetTitle());
          return best_shape;
        } else {
          ATH_MSG_WARNING("no parametrization found for id="<<id<<" E="<<E<<" eta="<<eta<<" dist="<<dist);
          return 0;
        }
      } else {
        if(msgLvl(MSG::DEBUG)) {
          ATH_MSG_DEBUG("E="<<E<<" not found : size="<<iter_id->second.size());
          ATH_MSG_DEBUG("begin="<<iter_id->second.begin()->first);
          ATH_MSG_DEBUG("rbegin="<<iter_id->second.rbegin()->first);
        }
        return 0;
      }
    } else {
      ATH_MSG_DEBUG("calosample="<<calosample<<" not found");
      return 0;
    }
  } else {
    ATH_MSG_DEBUG("ID found="<<id<<" not found");
    return 0;
  }
}


void FastShowerCellBuilderTool::CaloLocalPoint (const Trk::TrackParameters* parm, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local)
{
  if (!parm) return;
  if (!pt_local) return;

  *pt_ctb = parm->position();

  ATH_MSG_DEBUG( "Impact point in ctb coord : x,y,z= "
                 << pt_ctb->x() << " "
                 << pt_ctb->y() << " " << pt_ctb->z() << " R="
                 << std::sqrt( pt_ctb->x()*pt_ctb->x() + pt_ctb->y()*pt_ctb->y()
                               + pt_ctb->z()*pt_ctb->z())
                 << " eta=" << pt_ctb->eta() << " phi=" << pt_ctb->phi() );

  m_calo_tb_coord->ctb_to_local(*pt_ctb, *pt_local);

  ATH_MSG_DEBUG( "Impact point in local coord : x,y,z= "
                 << pt_local->x() << " "
                 << pt_local->y() << " " << pt_local->z() << " R="
                 << std::sqrt( pt_local->x()*pt_local->x()
                               + pt_local->y()*pt_local->y()
                               + pt_local->z()*pt_local->z())
                 << " eta=" << pt_local->eta() << " phi=" << pt_local->phi() );
}

bool
FastShowerCellBuilderTool::get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector,
                                           CaloCell_ID_FCS::CaloSample sample,
                                           double eta_calo_surf,
                                           double phi_calo_surf,
                                           bool layerCaloOK[CaloCell_ID_FCS::MaxSample],
                                           double letaCalo[CaloCell_ID_FCS::MaxSample],
                                           double lphiCalo[CaloCell_ID_FCS::MaxSample],
                                           double dCalo[CaloCell_ID_FCS::MaxSample],
                                           double distetaCaloBorder[CaloCell_ID_FCS::MaxSample]) const
{
  layerCaloOK[sample]=false;
  letaCalo[sample]=eta_calo_surf;
  lphiCalo[sample]=phi_calo_surf;
  double distsamp =deta(sample,eta_calo_surf);
  double rzmiddle =rzmid(sample,eta_calo_surf);
  double hitdist=0;
  bool best_found=false;
  double best_target=0;

  std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
  while ( it!= hitVector->end() && it->detID != (3000+sample) ) { it++;}
  //while ((*it).detID != (3000+sample) && it < hitVector->end() )  it++;

  if (it!=hitVector->end()) {
    Amg::Vector3D hitPos1 = (*it).trackParms->position();
    int sid1=(*it).detID;
    int sid2=-1;
    Amg::Vector3D hitPos;
    Amg::Vector3D hitPos2;

    std::vector<Trk::HitInfo>::iterator itnext = it;
    ++itnext;
    if(itnext!=hitVector->end()) {
      hitPos2 = (*itnext).trackParms->position();
      sid2=(*itnext).detID;
      double eta_avg=0.5*(hitPos1.eta()+hitPos2.eta());
      double t;
      if(isCaloBarrel(sample)) {
        double r=rmid(sample,eta_avg);
        double r1=hitPos1.perp();
        double r2=hitPos2.perp();
        t=(r-r1)/(r2-r1);
        best_target=r;
      } else {
        double z=zmid(sample,eta_avg);
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
        best_target=z;
      }
      hitPos=t*hitPos2+(1-t)*hitPos1;
    } else {
      hitPos=hitPos1;
      hitPos2=hitPos1;
    }
    double etaCalo = hitPos.eta();
    double phiCalo = hitPos.phi();
    letaCalo[sample]=etaCalo;
    lphiCalo[sample]=phiCalo;
    hitdist=hitPos.mag();
    layerCaloOK[sample]=true;
    rzmiddle=rzmid((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
    distsamp=deta((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
    best_found=true;
    ATH_MSG_DEBUG(" extrapol with layer hit: id="<<sid1<<" -> "<<sid2<<
                  " target r/z="<<best_target<<
                  " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                  " re="<<hitPos.perp()<<" ze="<<hitPos[Amg::z]
                  );
  }
  if(!best_found) {
    it = hitVector->begin();
    double best_dist=0.5;
    bool best_inside=false;
    int best_id1,best_id2;
    Amg::Vector3D best_hitPos=(*it).trackParms->position();
    while (it < hitVector->end()-1 ) {
      Amg::Vector3D hitPos1 = (*it).trackParms->position();
      int sid1=(*it).detID;
      it++;
      if ((hitPos1-it->trackParms->position()).mag()<0.001 ) it++; // ST exit and entry to the next layer may coincide
      if (it==hitVector->end()) break;
      Amg::Vector3D hitPos2 = (*it).trackParms->position();
      int sid2=(*it).detID;
      double eta_avg=0.5*(hitPos1.eta()+hitPos2.eta());
      double t;
      double tmp_target=0;
      double r1=hitPos1.perp();
      double r2=hitPos2.perp();
      if(isCaloBarrel(sample) && std::abs(r1-r2) > 1e-3) {
        double r=rmid(sample,eta_avg);
        t=(r-r1)/(r2-r1);
        tmp_target=r;
      } else {
        double z=zmid(sample,eta_avg);
        double z1=hitPos1[Amg::z];
        double z2=hitPos2[Amg::z];
        t=(z-z1)/(z2-z1);
        tmp_target=z;
      }
      Amg::Vector3D hitPos=t*hitPos2+(1-t)*hitPos1;
      double dist=TMath::Min(TMath::Abs(t-0),TMath::Abs(t-1));
      bool inside=false;
      if(t>=0 && t<=1) inside=true;
      if(!best_found || inside) {
        if(!best_inside || dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          best_target=tmp_target;
        }
      } else {
        if(!best_inside && dist<best_dist) {
          best_dist=dist;
          best_hitPos=hitPos;
          best_inside=inside;
          best_found=true;
          best_id1=sid1;
          best_id2=sid2;
          best_target=tmp_target;
        }
      }
      ATH_MSG_VERBOSE(" extrapol without layer hit: id="<<sid1<<" -> "<<sid2<<" dist="<<dist<<" mindist="<<best_dist<<
                      " t="<<t<<" best_inside="<<best_inside<<" target r/z="<<tmp_target<<
                      " r1="<<hitPos1.perp()<<" z1="<<hitPos1[Amg::z]<<" r2="<<hitPos2.perp()<<" z2="<<hitPos2[Amg::z]<<
                      " re="<<hitPos.perp()<<" ze="<<hitPos[Amg::z]<<
                      " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                      );
      if(best_found) {
        letaCalo[sample]=best_hitPos.eta();
        lphiCalo[sample]=best_hitPos.phi();
        hitdist=best_hitPos.mag();
        rzmiddle=rzmid((CaloCell_ID_FCS::CaloSample)sample,letaCalo[sample]);
        distsamp=deta((CaloCell_ID_FCS::CaloSample)sample,letaCalo[sample]);
        layerCaloOK[sample]=true;
      }
    }
    if(best_found) {
      ATH_MSG_DEBUG(" extrapol without layer hit: id="<<best_id1<<" -> "<<best_id2<<" mindist="<<best_dist<<
                    " best_inside="<<best_inside<<" target r/z="<<best_target<<
                    " rb="<<best_hitPos.perp()<<" zb="<<best_hitPos[Amg::z]
                    );
    }
  }

  if(isCaloBarrel((CaloCell_ID_FCS::CaloSample)sample)) rzmiddle*=cosh(letaCalo[sample]);
  else                                                  rzmiddle= fabs(rzmiddle/tanh(letaCalo[sample]));

  dCalo[sample]=rzmiddle;
  distetaCaloBorder[sample]=distsamp;

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG)<<"  Final par TTC sample "<<(int)sample;
    if(layerCaloOK[sample]) msg()<<" (good)";
    else msg()<<" (bad)";
    msg()<<" eta=" << letaCalo[sample] << "   phi=" << lphiCalo[sample] <<" dCalo="<<dCalo[sample]<<" dist(hit)="<<hitdist<< endmsg;
  }

  return layerCaloOK[sample];
}

bool
FastShowerCellBuilderTool::get_calo_surface(std::vector<Trk::HitInfo>* hitVector,
                                            double& eta_calo_surf,
                                            double& phi_calo_surf,
                                            double& d_calo_surf) const
{
  CaloCell_ID_FCS::CaloSample sample_calo_surf=CaloCell_ID_FCS::noSample;
  eta_calo_surf=-999;
  phi_calo_surf=-999;
  d_calo_surf=0;
  double min_calo_surf_dist=1000;

  for(int i=0;i<m_n_surfacelist;++i) {
    CaloCell_ID_FCS::CaloSample sample=m_surfacelist[i];
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while (it != hitVector->end() && it->detID != (3000+sample) )  { it++;}
    if(it==hitVector->end()) continue;
    Amg::Vector3D hitPos = (*it).trackParms->position();

    //double offset = 0.;
    double etaCalo = hitPos.eta();

    msg(MSG::DEBUG)<<"test: entrance to calo surface : sample="<<sample<<" eta="<<etaCalo;

    if (fabs(etaCalo)<900) {
      double phiCalo = hitPos.phi();
      double distsamp =deta(sample,etaCalo);

      msg(MSG::DEBUG)<<" phi="<<phiCalo<<" dist="<<distsamp;
      if(distsamp<min_calo_surf_dist) {
        eta_calo_surf=etaCalo;
        phi_calo_surf=phiCalo;
        min_calo_surf_dist=distsamp;
        sample_calo_surf=sample;
        d_calo_surf=rzent(sample,etaCalo);
        msg(MSG::DEBUG)<<" r/z="<<d_calo_surf;
        if(isCaloBarrel(sample)) d_calo_surf*=cosh(etaCalo);
        else                     d_calo_surf= fabs(d_calo_surf/tanh(etaCalo));
        msg(MSG::DEBUG)<<" d="<<d_calo_surf;
        if(distsamp<0) {
          msg(MSG::DEBUG)<<endmsg;
          break;
        }
      }
      msg(MSG::DEBUG)<<endmsg;
    } else {
      msg(MSG::DEBUG)<<": eta > 900, not using this"<<endmsg;
    }
  }

  if(sample_calo_surf==CaloCell_ID_FCS::noSample) {
    // first intersection with sensitive calo layer
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while ( it < hitVector->end() && (*it).detID != 3 ) { it++;}   // to be updated
    if (it==hitVector->end())   {  // no calo intersection, abort
      return false;
    }
    Amg::Vector3D surface_hitPos = (*it).trackParms->position();
    eta_calo_surf=surface_hitPos.eta();
    phi_calo_surf=surface_hitPos.phi();
    d_calo_surf=surface_hitPos.mag();

    double pT=(*it).trackParms->momentum().perp();
    if(TMath::Abs(eta_calo_surf)>4.9 || pT<500 || (TMath::Abs(eta_calo_surf)>4 && pT<1000) ) {
      ATH_MSG_DEBUG("only entrance to calo entrance layer found, no surface : eta="<<eta_calo_surf<<" phi="<<phi_calo_surf<<" d="<<d_calo_surf<<" pT="<<pT);
    } else {
      ATH_MSG_WARNING("only entrance to calo entrance layer found, no surface : eta="<<eta_calo_surf<<" phi="<<phi_calo_surf<<" d="<<d_calo_surf<<" pT="<<pT);
    }
  } else {
    ATH_MSG_DEBUG("entrance to calo surface : sample="<<sample_calo_surf<<" eta="<<eta_calo_surf<<" phi="<<phi_calo_surf<<" deta="<<min_calo_surf_dist<<" dsurf="<<d_calo_surf);
  }

  return true;
}

StatusCode
FastShowerCellBuilderTool::process_particle(CaloCellContainer* theCellContainer,
                                            std::vector<Trk::HitInfo>* hitVector,
                                            Amg::Vector3D initMom,
                                            double mass,
                                            int pdgid,
                                            int barcode,
                                            FastShowerInfoContainer* fastShowerInfoContainer,
                                            TRandom3& rndm,
                                            Stats& stats,
                                            const EventContext& ctx) const
{
  // no intersections with Calo layers found : abort;
  if(!hitVector || !hitVector->size())  {
    ATH_MSG_DEBUG(" Calo hit vector empty: aborting particle processing ");
    return StatusCode::FAILURE;
  }

  // Setup of the FastShowerInfo debug containers if requested
  FastShowerInfo *fastshowerinfo(0);
  if( m_storeFastShowerInfo ) {
    ATH_MSG_DEBUG("Creating FastShowerInfoObject: ");
    fastshowerinfo = new FastShowerInfo();
    fastshowerinfo->Initialize( CaloCell_ID_FCS::MaxSample );
  }

  ///////////////////////////
  // Init of basic quantities
  ///////////////////////////

  double ptruth_eta=initMom.eta();
  if( fabs(ptruth_eta)>6.0 ) {
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::SUCCESS;
  }

  double ptruth_phi=initMom.phi();
  double ptruth_e = sqrt(initMom.mag2()+mass*mass);

  TVector3 truth_direction;  // added (25.5.2009)
  truth_direction.SetPtEtaPhi(1.,ptruth_eta,ptruth_phi);
  TVector3 z_direction;// added (25.5.2009)
  z_direction.SetXYZ(0,0,1);// added (25.5.2009)  // added (25.5.2009)
  double ang_beta = z_direction.Angle(truth_direction);// added (25.5.2009)
  // Definition of et2 and et according to CLHEP::HepLorentzVector
  //double pt2 = part->momentum().perp2();
  //double et2 = pt2 == 0 ? 0 : ptruth_e*ptruth_e * pt2/(pt2+part->momentum().z()*part->momentum().z());
  //ptruth_et = ptruth_e < 0.0 ? 0.0 : sqrt(et2);
  double ptruth_pt=initMom.perp();
  double ptruth_p=initMom.mag();

  // By default assume its a charged hadron==pion
  int refid=211;
  double refmass=139.57018; //PDG charged pion mass
  double partmass=mass;
  t_map_PEP_ID::const_iterator iter_id=m_map_ParticleEnergyParametrizationMap.find(11); // Test if a dedicated electron parametrization exists
  if(iter_id!=m_map_ParticleEnergyParametrizationMap.end()) {                     // electron parametrization exists
    if(pdgid==22 || pdgid==111) {
      refid=22;
      refmass=0;
    }
    if( pdgid==11 || pdgid==-11 ) {
      refid=11;
      refmass=0.511;
      //log<<MSG::VERBOSE<<" electron parametrization found " << endmsg;
    }
  } else {                                                                        // electron parametrization does not exist
    if(pdgid==22 || pdgid==11 || pdgid==-11 || pdgid==111) {
      refid=22;
      refmass=0;
      //if(pdgid==11 || pdgid==-11) log<<MSG::VERBOSE<<" no electron parametrization found: USE PHOTON " << endmsg;
    }
  }
  if(pdgid==13 || pdgid==-13) {
    refid=13;
    refmass=105.658367; //PDG mass
  }
  if(pdgid==111) {
    // there shouldn't be stable pi0, but sometimes they are in the truth info, because of G4 cuts.
    // Treat them as massless to deposit the full energy: pi0->gamma gamma
    partmass=0;
  }

  // Default use the total particle energy as base quantity to deposit into the calo
  double Ein=ptruth_e;
  if(m_use_Ekin_for_depositions) { // alternatively use only the kinetic energy
    double Ekin=Ein - partmass; // kinetic energy of incoming particle
    Ein=Ekin+refmass; //the parametrization is done in bins of E, so go back from Ekin to the E used in the input reference
  }
  double EinT=Ein * ptruth_pt/ptruth_p; // only needed to trigger debug output

  if(Ein<10) {
    // don't simulate particles below 10MeV
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::SUCCESS;
  }

  bool   layerCaloOK[CaloCell_ID_FCS::MaxSample];
  double letaCalo[CaloCell_ID_FCS::MaxSample];
  double lphiCalo[CaloCell_ID_FCS::MaxSample];
  double dCalo[CaloCell_ID_FCS::MaxSample];
  double distetaCaloBorder[CaloCell_ID_FCS::MaxSample];
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
    layerCaloOK[i]=false;
    distetaCaloBorder[i]=1000;
    dCalo[i]=0;
    letaCalo[i]=lphiCalo[i]=-999;
  }

  if( m_storeFastShowerInfo ) {
    // storing the particle information inside the FastShowerInfo object
    fastshowerinfo->SetPtEtaPhiE( ptruth_pt, ptruth_eta, ptruth_phi, ptruth_e );
    fastshowerinfo->SetBarcodeAndPDGId( barcode, pdgid );
  }

  std::stringstream particle_info_str;
  particle_info_str<<"id="<<pdgid<<" rid="<<refid<<" e="<<ptruth_e<<" Ein="<<Ein<<" EinT="<<EinT<<" pt="<<ptruth_pt<<" p="<<ptruth_p<<" m="<<mass<<" eta="<<ptruth_eta<<" phi="<<ptruth_phi;

  if(msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("====================================================");
    ATH_MSG_DEBUG("initial "<<particle_info_str.str());
    ATH_MSG_DEBUG("====================================================");
  }

  //////////////////////////////////////
  // Start calo extrapolation
  // First: get entry point into first calo sample
  //////////////////////////////////////

  if(msgLvl(MSG::DEBUG)) {
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while (it < hitVector->end() )  {
      int sample=(*it).detID;
      Amg::Vector3D hitPos = (*it).trackParms->position();
      ATH_MSG_DEBUG(" HIT: layer="<<sample-3000<<" eta="<<hitPos.eta()<<" phi="<<hitPos.phi()<<" d="<<hitPos.mag());
      it++;
    }
  }

  double d_calo_surf;
  double eta_calo_surf;
  double phi_calo_surf;
  if(!get_calo_surface(hitVector,
                       eta_calo_surf,
                       phi_calo_surf,
                       d_calo_surf))
  {
    if(TMath::Abs(ptruth_eta)>5 || EinT<500) {
      ATH_MSG_DEBUG("Calo hit vector does not contain calo layer entry: aborting processing particle "<<particle_info_str.str());
    } else {
      ATH_MSG_WARNING("Calo hit vector does not contain calo layer entry: aborting processing particle "<<particle_info_str.str());
    }
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::FAILURE;
  }

  TVector3 surface;
  surface.SetPtEtaPhi(1,eta_calo_surf,phi_calo_surf);
  surface.SetMag(d_calo_surf);

  if(m_storeFastShowerInfo) fastshowerinfo->SetCaloSurface(eta_calo_surf, phi_calo_surf, d_calo_surf);

  // only continue if inside the calo
  if( fabs(eta_calo_surf)> 6 ) {
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::SUCCESS;
  }

  double distrange=0;
  double Epara_E = Ein; // Basic input energy to be put into the calo without response, used for parametrization lookup

  ParticleEnergyShape p;

  //////////////////////////////
  // Process Muon info from Fatras
  //////////////////////////////
  if(abs(pdgid)==13) {
    // std::pair<BarcodeEnergyDepositMap::iterator,BarcodeEnergyDepositMap::iterator> range=MuonEnergyMap->equal_range(part->barcode());
    p.fcal_tot=0;
    for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
      p.E_layer[i]=0;
      p.fcal_layer[i]=0;
      letaCalo[i]=-999;
      lphiCalo[i]=-999;
      dCalo[i]=0;
    }
    // loop over intersection
    std::vector<Trk::HitInfo>::iterator it = hitVector->begin();
    while (it < hitVector->end() && (*it).detID != -3 )  { it++;}   // to be updated
    if(it!=hitVector->end()) {
      Amg::Vector3D hitPos = (*it).trackParms->position();
      CaloCell_ID_FCS::CaloSample sample = (CaloCell_ID_FCS::CaloSample)((*it).detID-3000);    // to be updated
      double pLast = (*it).trackParms->momentum().mag();
      double pCurr = pLast;
      double dCurr = hitPos.mag();
      while ( it < hitVector->end()-1 )  {
        // step to the layer exit to evaluate the deposit
        it++;
        pCurr = (*it).trackParms->momentum().mag();
        double edeposit = pLast - pCurr; // to be updated! Includes dead material energy loss in calo deposit
        hitPos = (*it).trackParms->position();
        if (sample>=0 && sample<=CaloCell_ID_FCS::LastSample) {  // save layer deposit as relative fraction of the momentum, in the middle
          double E = edeposit/Ein;
          p.E_layer[sample]=E;
          p.E+=p.E_layer[sample];
          letaCalo[sample]=hitPos.eta();
          lphiCalo[sample]=hitPos.phi();
          dCalo[sample]=0.5*(dCurr+hitPos.mag());
          p.dist000+=p.E_layer[sample]*dCalo[sample];

          ATH_MSG_DEBUG("muon deposit: sampe="<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << letaCalo[sample] <<" dCalo="<<dCalo[sample]);
        }
        sample = (CaloCell_ID_FCS::CaloSample)((*it).detID-3000);         // to be updated
        dCurr = hitPos.mag();
        pLast = pCurr;
      }

      p.Ecal=p.E;
      p.dist000/=p.E;
      p.dist_in=p.dist000;
    }
  } else {
    //////////////////////////////
    // Process all non muon particles
    //////////////////////////////
    const ParticleEnergyParametrization* Elower=findElower(refid , Ein , eta_calo_surf);
    const ParticleEnergyParametrization* Eupper=findEupper(refid , Ein , eta_calo_surf);
    const ParticleEnergyParametrization* Epara=0;
    if(Elower) {
      ATH_MSG_DEBUG("lower : "<< Elower->GetTitle()<< " lower E: " << Elower->E());
      Epara=Elower;
    }
    if(Eupper) {
      ATH_MSG_DEBUG("upper : "<< Eupper->GetTitle()<< " upper E: " << Eupper->E());
      Epara=Eupper;
    }
    if(Elower && Eupper) {
      ATH_MSG_DEBUG("lower : "<< Elower->GetTitle()<< " lower E: " << Elower->E()<< " ; upper : "<< Eupper->GetTitle()<< " upper E: " << Eupper->E());
      /* interpolate */
      if (m_jo_interpolate && Eupper->E()>Elower->E()) {
        double ran = gRandom->Rndm();
        double wt;
        if(Ein>1.0e-6) wt = TMath::Log(Ein/Eupper->E())/TMath::Log(Elower->E()/Eupper->E()); //protect against E=0
        else wt=1.0;
        if(ran<wt) Epara = Elower;
        else Epara = Eupper;
      } else {
        if( fabs(Elower->E()-Ein) < fabs(Eupper->E()-Ein) ){
          Epara=Elower;
        } else{
          Epara=Eupper;
        }
      }
      /* interpolate */
    }
    if(Epara) {
      Epara_E = Epara->E();
      ATH_MSG_DEBUG("take  : "<< Epara->GetTitle());
      Epara->DiceParticle(p,rndm);

      if(m_storeFastShowerInfo) Epara->CopyDebugInfo(fastshowerinfo); // old version: fastshowerinfo->SetParticleEnergyParam( *Epara );

      if(p.E<=0) {
        MSG::Level level=MSG::WARNING;
        if(Ein<2000) level=MSG::DEBUG;
        if(EinT<2000) level=MSG::DEBUG;
        ATH_MSG_LVL(level,"particle energy<=0 ");
        ATH_MSG_LVL(level," - "<<particle_info_str.str()<< " parametrization  : "<< Epara->GetTitle()<<" : skip particle...");

        if(m_storeFastShowerInfo) delete fastshowerinfo;

        if(p.E<0) return StatusCode::FAILURE;
        return StatusCode::SUCCESS;
      }

      if(p.E>=3 && p.E*Ein>2000) {
        ATH_MSG_WARNING("particle energy/truth="<<p.E);
        ATH_MSG_WARNING(" - "<<particle_info_str.str());
        ATH_MSG_WARNING(" parametrization  : "<< Epara->GetTitle());
        ATH_MSG_WARNING(" - p.E="<<p.E<< "p.Ecal="<<p.Ecal<<" p.fcal_tot="<<p.fcal_tot);
        for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
          ATH_MSG_WARNING(" - sample "<<sample<<" E/Etot="<<p.E_layer[sample] <<" fcal="<<p.fcal_layer[sample]<<" w="<<Epara->weight(sample));
        }

        if(m_storeFastShowerInfo) delete fastshowerinfo;

        ATH_MSG_WARNING(" - skip particle...");
        return StatusCode::FAILURE;
      }

      /*
      // now loop over intersections and match with the parametrized deposit ( by default in the middle )
      double dCurr = hitPos.mag();
      while ( it < hitVector->end()-1 )  {
      it++;
      hitPos = (*it).trackParms->position();
      if (sample>0) {  // save layer deposit as relative fraction of the momentum ?
      if(p.E_layer[sample]<0) p.E_layer[sample]=0;
      if(p.E_layer[sample]>0) {
      dCalo[sample] = 0.5*( dCurr + hitPos.mag() );
      p.dist000+=p.E_layer[sample]*hitPos.mag();
      if(dCalo[sample]<0.01 && p.E_layer[sample]>0) {
      ATH_MSG_WARNING("Calo position for sample "<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << hitPos.eta() <<" dCalo="<<dCalo[sample]);
      }
      }
      }
      dCurr = hitPos.mag();
      sample = (CaloCell_ID_FCS::CaloSample)((*it).detID-3000);         // to be updated
      }

      p.dist000/=p.E;
      distrange=fabs((Epara->GetDistMax()-Epara->GetDistMin())/Epara->GetNDistBins());

      ATH_MSG_DEBUG("Ein="<<Ein<<" Ecal/Ein="<<p.Ecal<<" E/Ein="<<p.E<<" E="<<p.E*Ein
      <<" din="<<p.dist_in<<" dist000="<<p.dist000<<" drec="<<p.dist_rec
      <<" dmin="<<Epara->GetDistMin()<<" dmax="<<Epara->GetDistMax()<<" nd="<<Epara->GetNDistBins()<<" drange="<<distrange);
      */


      // now try to extrapolate to all calo layers, that contain energy
      for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
        if(p.E_layer[sample]<0) p.E_layer[sample]=0;
        if(p.E_layer[sample]>0) {
          ATH_MSG_DEBUG("============= Getting Calo position for sample "<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << ptruth_eta);
          if( get_calo_etaphi(hitVector,(CaloCell_ID_FCS::CaloSample)sample,
                              eta_calo_surf,
                              phi_calo_surf,
                              layerCaloOK,
                              letaCalo,
                              lphiCalo,
                              dCalo,
                              distetaCaloBorder) )
          {
            p.dist000+=p.E_layer[sample]*dCalo[sample];
            if(dCalo[sample]<0.01 && p.E_layer[sample]>0) {
              ATH_MSG_WARNING("Calo position for sample "<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << ptruth_eta <<" dCalo="<<dCalo[sample]);
            }
          }
        }
      }
      p.dist000/=p.E;
      distrange=fabs((Epara->GetDistMax()-Epara->GetDistMin())/Epara->GetNDistBins());

      ATH_MSG_DEBUG("Ein="<<Ein<<" Ecal/Ein="<<p.Ecal<<" E/Ein="<<p.E<<" E="<<p.E*Ein
                    <<" din="<<p.dist_in<<" dist000="<<p.dist000<<" drec="<<p.dist_rec
                    <<" surface: d="<<surface.Mag()<<" r="<<surface.Perp()<<" z="<<surface.z()
                    <<" dmin="<<Epara->GetDistMin()<<" dmax="<<Epara->GetDistMax()<<" nd="<<Epara->GetNDistBins()<<" drange="<<distrange);


    }
  }

  p.dist_rec=p.dist000-surface.Mag();

  // If there is no energy in the calo, abort
  if(p.E<=0) {
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::FAILURE;
  }

  //////////////////////////////////////
  // Main loop over all calorimeter layers
  //////////////////////////////////////
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) if(!isnan(p.E_layer[sample])) {
      // Now scale relative energy response to particle energy
      p.E_layer[sample]*=Ein*m_sampling_energy_reweighting[sample];
      ATH_MSG_DEBUG("============= E"<<sample<<"="<<p.E_layer[sample]<<" =============");

      double E_orig=p.E_layer[sample];
      double Et_orig=E_orig/cosh(eta_calo_surf);
      // Add energy to lost energy counter.  If all deposit goes OK, subtract it again...
      stats.m_E_lost_sample[sample]+=E_orig;
      stats.m_Et_lost_sample[sample]+=Et_orig;

      if(p.E_layer[sample]>0 && (!isnan(p.E_layer[sample])) && fabs(letaCalo[sample])<5.0) {
        ATH_MSG_DEBUG("E"<<sample<<"="<<p.E_layer[sample]<<" d="<<dCalo[sample]);

        // Calculate estimates for cell energy fluctuations. Should be updated with better numbers
        double smaple_err=0.1;
        if(sample>=CaloCell_ID_FCS::PreSamplerB && sample<=CaloCell_ID_FCS::EME3    ) smaple_err=0.1; //LAr      10%/sqrt(E)
        if(sample>=CaloCell_ID_FCS::HEC0        && sample<=CaloCell_ID_FCS::TileExt2) smaple_err=0.5; //hadronic 50%/sqrt(E) ???
        if(sample>=CaloCell_ID_FCS::FCAL0       && sample<=CaloCell_ID_FCS::FCAL2   ) smaple_err=1.0; //FCAL    100%/sqrt(E) ???

        // Find parametrization for the lateral shape distribution in the sample
        const TShape_Result* shape;
        if(m_jo_interpolate) {
          shape=findShape( refid , sample , Epara_E , letaCalo[sample] , p.dist_in , distrange);
        } else {
          shape=findShape( refid , sample , Ein     , letaCalo[sample] , p.dist_in , distrange);
        }
        if(shape) {
          if(msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG)<<"found shape : "<<shape->GetName()<<" dmin="<<shape->distmin()<<"dmax="<<shape->distmax()<<" corr=[";
            for(unsigned int icorr=0;icorr<shape->m_correction.size();++icorr) {
              if(icorr>0) msg()<<";";
              msg()<<shape->m_correction[icorr]->GetName();
            }
            msg()<<"]"<<endmsg;
          }
        } else {
          if( !(sample>=CaloCell_ID_FCS::FCAL0 && sample<=CaloCell_ID_FCS::FCAL2) ) {
            MSG::Level level=MSG::WARNING;
            if(refid==13) level=MSG::DEBUG;
            if(msgLvl(level)) {
              ATH_MSG_LVL(level,"no shape found calosample="<<sample<<" Elayer="<<E_orig);
              ATH_MSG_LVL(level," - "<<particle_info_str.str());
              ATH_MSG_LVL(level," - Ecal/Ein="<<p.Ecal<<" E/Ein="<<p.E<<" E="<<p.E*Ein
                          <<" din="<<p.dist_in<<" dist000="<<p.dist000<<" drec="<<p.dist_rec);
            }
          }
        }

        ATH_MSG_DEBUG("  letaCalo="<<letaCalo[sample]<<" lphiCalo="<<lphiCalo[sample]);
        double fcx=letaCalo[sample];
        double fcy=lphiCalo[sample];
        double direction_factor=0.0;
        double distfactor = 0.0;
        //double distsign = 1.;

        TVector3 truth;  // added (25.5.2009)
        truth.SetPtEtaPhi(1.,letaCalo[sample],lphiCalo[sample]);// added (25.5.2009)
        double ang_alpha = truth.Angle(truth_direction);  // added (25.5.2009)
        double ang_gamma = z_direction.Angle(truth);// added (25.5.2009)
        int sign =1;// added (25.5.2009)
        if(TMath::Abs(ang_beta) > TMath::Abs(ang_gamma)){
          sign= -1;// added (25.5.2009)
          ang_beta = TMath::Pi() - ang_beta;
        }
        direction_factor = (TMath::Sin(ang_alpha)/TMath::Sin(ang_beta));

        double lookup_letaCalo=fcx;
        double lookup_lphiCalo=fcy;

        /*          direction_factor = 0;  */
        if(shape!=0) {
          distfactor = 2*(p.dist_in-shape->distmin())/(shape->distmax()-shape->distmin())-1;
          if(distfactor > 1.) distfactor =1.;
          if(distfactor < -1.) distfactor =-1.;
          //if(distfactor < 0.) distsign = -1.;
          // calculate position of shower in calo
          fcx=shape->eta_center(letaCalo[sample]/*,distsign*distfactor*/,direction_factor*sign);
          fcy=shape->phi_center(lphiCalo[sample]);
          lookup_letaCalo=fcx;
          lookup_lphiCalo=fcy;
          distetaCaloBorder[sample]=deta((CaloCell_ID_FCS::CaloSample)sample,fcx);
          double mineta,maxeta;
          minmaxeta((CaloCell_ID_FCS::CaloSample)sample,fcx,mineta,maxeta);

          // correct shower position in calo, if it is outside the calo layer boundaries
          // TODO: Apply relocation of shape to overlap with active calo also if no shape function is found
          if(distetaCaloBorder[sample]>0) {
            double bordereta=maxeta;
            if(fcx<mineta) bordereta=mineta;
            lookup_letaCalo=bordereta;
            double eta_jakobi=shape->eta_jakobi_factor(bordereta);
            //TODO: dCalo[sample] should not be taken, better dcalo at mineta/maxeta
            double cutoff_eta=shape->cutoff_eta()/(eta_jakobi*dCalo[sample]);
            double newetaCaloBorder=cutoff_eta/2;
            if(msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG)<<"  fcx="<<fcx<<" fcy="<<fcy<<" mineta="<<mineta<<" maxeta="<<maxeta<<" deta_border="
                             <<distetaCaloBorder[sample]<<" bordereta="<<bordereta<<" eta_jakobi="<<eta_jakobi
                             <<" cutoff_eta [mm]="<<shape->cutoff_eta()<<" dcalo="<<dCalo[sample]
                             <<" shapesize="<<cutoff_eta<<" aim deta_border="<<newetaCaloBorder;
            }
            if(distetaCaloBorder[sample]>newetaCaloBorder) {
              double olddeta=distetaCaloBorder[sample];
              double oldletaCalo=letaCalo[sample];
              double oldfcx=fcx;
              double delta=distetaCaloBorder[sample]-newetaCaloBorder;

              if(fcx<mineta) {
                fcx+=delta;
              } else if(fcx>maxeta) {
                fcx-=delta;
              }
              letaCalo[sample]=fcx;

              /* Causing too big steps!!!
                 if(fcx<mineta) {
                 while(fcx<mineta){
                 letaCalo[sample]+=delta;
                 fcx=shape->eta_center(letaCalo[sample],direction_factor*sign);
                 }
                 } else if(fcx>maxeta) {
                 while(fcx>maxeta){
                 letaCalo[sample]-=delta;
                 fcx=shape->eta_center(letaCalo[sample],direction_factor*sign);
                 }
                 }

                 fcx=shape->eta_center(letaCalo[sample],direction_factor*sign);
              */

              distetaCaloBorder[sample]=deta((CaloCell_ID_FCS::CaloSample)sample,fcx);
              if(msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG)<<" new deta="<<distetaCaloBorder[sample]<<endmsg;
              }

              if(distetaCaloBorder[sample]>olddeta) {
                ATH_MSG_WARNING("repositioned cell impact, but deta increased!!!! stay with the old...");
                distetaCaloBorder[sample]=olddeta;
                letaCalo[sample]=oldletaCalo;
                fcx=oldfcx;
              }
            } else {
              msg(MSG::DEBUG)<<endmsg;
            }
          } else {
            ATH_MSG_DEBUG("  fcx="<<fcx<<" fcy="<<fcy<<" mineta="<<mineta<<" maxeta="<<maxeta<<" deta="<<distetaCaloBorder[sample]);
          }

          if(m_storeFastShowerInfo) shape->SetDebugInfo(sample, fastshowerinfo); // old version: fastshowerinfo->SetTShapeResult( sample, *shape );

        }

        if(m_storeFastShowerInfo) fastshowerinfo->SetCaloInfo(sample, fcx, fcy, letaCalo[sample], lphiCalo[sample]);

        if(fabs(lookup_letaCalo)>5.0) {
          lookup_letaCalo*=4.99/fabs(lookup_letaCalo);
        }

        // Ugly code: does a fast lookup which cells should be considered to be filled with energy
        int iphi=m_celllist_maps[sample].phi_to_index(lookup_lphiCalo);
        int ieta=m_celllist_maps[sample].eta_to_index(lookup_letaCalo);
        const cellinfo_map::cellinfo_vec& vec=m_celllist_maps[sample].vec(ieta,iphi);
        int n_cells=vec.size();

        ATH_MSG_DEBUG("  n_cells=" <<n_cells);

        std::vector< const CaloDetDescrElement* > theDDE;
        std::vector< double > E_theDDE;
        theDDE.reserve(n_cells);
        E_theDDE.reserve(n_cells);

        for(int icell=0;icell<n_cells;++icell) {
          const CaloDetDescrElement* newcell=vec[icell].first;
          theDDE[icell]=newcell;
          E_theDDE[icell]=0;
        }

        double elayertot=0;
        int ibestcell=-1;
        double bestdist=100000000000.0;
        CLHEP::Hep3Vector truthimpact;
        truthimpact.setREtaPhi(1,fcx,fcy);
        truthimpact.setMag(dCalo[sample]);

        if(shape==0) {
          // If no shape is found, find the hit cell and deposit all energy in the hit cell
          for(int icell=0;icell<n_cells;++icell) {
            const CaloDetDescrElement* cell=theDDE[icell];
            double dist;
            if( sample>=CaloCell_ID_FCS::FCAL0 && sample<=CaloCell_ID_FCS::FCAL2 ) {
              double distx=( cell->x() - truthimpact.x() )/cell->dx();
              double disty=( cell->y() - truthimpact.y() )/cell->dy();
              dist=sqrt( distx*distx + disty*disty );
            } else {
              double disteta=( cell->eta() - truthimpact.eta() )/cell->deta();
              double distphi=( cell->phi() - truthimpact.phi() )/cell->dphi();
              dist=sqrt( disteta*disteta + distphi*distphi );
            }
            if(dist<bestdist || ibestcell<0) {
              bestdist=dist;
              ibestcell=icell;
            }
          }
          if(ibestcell>=0) {
            double subetot=1;
            E_theDDE[ibestcell]=subetot;
            elayertot+=subetot;

            const CaloDetDescrElement* cell=theDDE[ibestcell];
            if( !(sample>=CaloCell_ID_FCS::FCAL0 && sample<=CaloCell_ID_FCS::FCAL2) && refid!=13) {
              ATH_MSG_WARNING("best cell found for calosample "<<sample<<" eta="<<letaCalo[sample]<<" phi="<<lphiCalo[sample]
                              <<" ceta="<<cell->eta()<<" cphi="<<cell->phi()
                              <<" cdeta="<<cell->deta()<<" cdphi="<<cell->dphi()<<" bd="<<bestdist);
            } else {
              ATH_MSG_DEBUG  ("best cell found for calosample "<<sample<<" eta="<<letaCalo[sample]<<" phi="<<lphiCalo[sample]
                              <<" ceta="<<cell->eta()<<" cphi="<<cell->phi()
                              <<" cdeta="<<cell->deta()<<" cdphi="<<cell->dphi()<<" bd="<<bestdist);
            }

          }
        } else {
          //if a shape is found, find the hit cell and do eta position fine correction
          for(int icell=0;icell<n_cells;++icell) {
            const CaloDetDescrElement* cell=theDDE[icell];
            double disteta=( cell->eta() - truthimpact.eta() )/cell->deta();
            double distphi=TVector2::Phi_mpi_pi( cell->phi() - truthimpact.phi() )/cell->dphi();
            double dist=sqrt( disteta*disteta + distphi*distphi );
            if(dist<bestdist) {
              bestdist=dist;
              ibestcell=icell;
            }
          }
          double fcx_fine_corr=fcx ;
          double cellpos=0 ;
          if(ibestcell < 0 ) {
            ATH_MSG_WARNING("NO BEST CELL FOUND ");
            ATH_MSG_WARNING(" - calosample="<<sample<<" Elayer="<<E_orig);
            ATH_MSG_WARNING(" - id="<<pdgid<<" eta="<<letaCalo[sample]<<" phi="<<lphiCalo[sample]);
            ATH_MSG_WARNING(" - Ein="<<Ein<<" Ecal/Ein="<<p.Ecal<<" E/Ein="<<p.E<<" E="<<p.E*Ein
                            <<" din="<<p.dist_in<<" dist000="<<p.dist000<<" drec="<<p.dist_rec);
            ATH_MSG_WARNING(" - fcx="<<fcx<<" fcy="<<fcy);

          }
          const CaloDetDescrElement* cell_hit=0;
          if(ibestcell >= 0){
            cell_hit= theDDE[ibestcell];
            cellpos = (fcx - cell_hit->eta() + cell_hit->deta()/2 )/ cell_hit->deta();
            cellpos = cellpos - floor(cellpos);
            while(cellpos < 0) cellpos =cellpos+ 1.;
            while(cellpos > 1) cellpos =cellpos -1.;
            fcx_fine_corr = fcx_fine_corr  +shape->reldeta_dist()*sin(cellpos*2*TMath::Pi())  ;

            ATH_MSG_DEBUG("  fcx_fine="<<fcx_fine_corr<<" fcx="<<fcx<<" direction_factor*sign="<<direction_factor*sign);
          }

          // 1st loop over all cells and calculate the relative energy content in each cell
          for(int icell=0;icell<n_cells;++icell) {
            const CaloDetDescrElement* cell=theDDE[icell];

            double Einwide;
            double Ecell=shape->CellIntegralEtaPhi(*cell,letaCalo[sample],lphiCalo[sample],Einwide,fcx_fine_corr,fcy,direction_factor*sign);

            //              if(icell<20) log << MSG::DEBUG <<"  ("<<icell<<") ceta="<<cell->eta()<<" cphi="<<cell->phi()<<" E="<<Ecell<<" Einwide="<<Einwide<<endmsg;
            if(Einwide<=0) {
              E_theDDE[icell]=0;
              continue;
            }

            if(shape->HasCellFactor() && cell_hit) {
              float disteta=                    ( cell->eta() - cell_hit->eta() )/cell_hit->deta();
              float distphi=TVector2::Phi_mpi_pi( cell->phi() - cell_hit->phi() )/cell_hit->dphi();
              long int idisteta=lroundf(disteta);
              long int idistphi=lroundf(distphi);
              //log << MSG::DEBUG <<"  ("<<icell<<") ceta="<<cell->eta()<<" cphi="<<cell->phi()<<" E="<<Ecell<<" Einwide="<<Einwide<<" deta="<<disteta<<"="<<idisteta<<" dphi="<<distphi<<"="<<idistphi<<endmsg;

              for(unsigned int icorr=0;icorr<shape->m_correction.size();++icorr) {
                if(shape->m_correction[icorr]->HasCellFactor()) {
                  Ecell*=shape->m_correction[icorr]->cellfactor(idisteta,idistphi);
                }
              }
            }

            double subetot=Ecell;

            E_theDDE[icell]=subetot;
            elayertot+=subetot;

            if(m_storeFastShowerInfo) fastshowerinfo->AddCellSubETot( sample, subetot, cell->identify().get_identifier32().get_compact());
          }
          if(ibestcell>=0) {
            elayertot-= E_theDDE[ibestcell];
            E_theDDE[ibestcell]*=(1+ shape->reletascale()*fabs(direction_factor));
            elayertot+=E_theDDE[ibestcell];
            const CaloDetDescrElement* cell=theDDE[ibestcell];
            double eta_jakobi=TMath::Abs( 2.0*TMath::Exp(-cell->eta())/(1.0+TMath::Exp(-2*cell->eta())) );
            //  double phi_dist2r=1.0/TMath::CosH(fcx);
            double phi_dist2r=1.0;
            double dist000=TMath::Sqrt(cell->r()*cell->r()+cell->z()*cell->z());
            double celldx=cell->deta()*eta_jakobi*dist000;
            double celldy=cell->dphi()*phi_dist2r*cell->r();
            ATH_MSG_DEBUG("center cell found for calosample "<<sample<<" eta="<<letaCalo[sample]<<" phi="<<lphiCalo[sample]
                          <<" ceta="<<cell->eta()<<" cphi="<<cell->phi()<<" bd="<<bestdist
                          <<" cdeta="<<cell->deta()<<" cdphi="<<cell->dphi()<<" cdx="<<celldx<<" cdy="<<celldy<<" r="<<cell->r()<<" d="<<dist000);
          }
        }

        if(elayertot<=0) {
          MSG::Level level=MSG::DEBUG;
          if(Et_orig>500) level=MSG::WARNING;
          if(msgLvl(level)) {
            msg(level)<< "calosample "<<sample<<" : no energy dep around truth impact ("<<letaCalo[sample]<<"/"<<fcx<<"/"<<lookup_letaCalo<<";"<<lphiCalo[sample]<<"/"<<fcy<<"/"<<lookup_lphiCalo<<"), E("<<sample<<")="<<E_orig<<", Et("<<sample<<")="<<Et_orig<<endmsg;
            msg(level)<< " - "<<particle_info_str.str()<<endmsg;
            msg(level)<< " - ";
            if(shape) msg() << level << "parametrization  : "<< shape->GetTitle()<<", ";
            msg() << level << "skip sample..."<<endmsg;
          }
          continue;
        }

        if(shape) {
          ATH_MSG_DEBUG(shape->GetTitle()<<": cutoff="<<shape->cutoff_eta()<<" dmin="<<shape->distmin()<<" dmax="<<shape->distmax());
        }

        ATH_MSG_DEBUG("sample "<<sample<<" etot="<<p.E_layer[sample]<<" elayertot="<<elayertot);
        double elayertot2=0;

        // 2nd loop over all cells: renormalize to total energy in layer and apply cell fluctuations
        for(int icell=0;icell<n_cells;++icell) {
          //          const CaloDetDescrElement* cell=theDDE[i][icell];
          double ecell=E_theDDE[icell]*p.E_layer[sample]/elayertot;
          if(ecell<=0) continue;
          //          log<<MSG::DEBUG<<"layer "<<i<<" cell eta="<<cell->eta()<<" phi="<<cell->phi()<<" Eorg="<<ecell;

          /*
            if(ecell/p.E_layer[sample]>0.01) {
            const CaloDetDescrElement* cell=theDDE[icell];
            log << MSG::DEBUG <<" ceta="<<cell->eta()<<" cphi="<<cell->phi()<<" E="<<ecell;
            if(ibestcell) {
            const CaloDetDescrElement* bestcell=theDDE[ibestcell];
            log <<" dhit :deta="<<cell->eta()-bestcell->eta()<<" dphi="<<TVector2::Phi_mpi_pi( cell->phi() - bestcell->phi() );
            }
            log <<endmsg;
            }
          */

          CLHEP::HepRandomEngine* engine = m_randomEngine->getEngine (ctx);
          double rndfactor=-1;
          while(rndfactor<=0) rndfactor=CLHEP::RandGaussZiggurat::shoot(engine,1.0,smaple_err/sqrt(ecell/1000));
          ecell*=rndfactor;
          //          if(ecell<0) ecell=0;
          //          log<<" Esmear="<<ecell<<endmsg;
          elayertot2+=ecell;
          E_theDDE[icell]=ecell;

          if(m_storeFastShowerInfo) fastshowerinfo->AddCellEErrorCorrected(sample, ecell );
        }
        double elayertot3=0;
        // 3rd loop over all cells: renormalize to total energy in layer again and deposit energy in the calo
        for(int icell=0;icell<n_cells;++icell) {
          const CaloDetDescrElement* cell=theDDE[icell];
          double ecell=E_theDDE[icell]*p.E_layer[sample]/elayertot2;
          if(ecell==0) continue;
          elayertot3+=ecell;

          // is there a other way to get a non const pointer?
          CaloCell* theCaloCell=(CaloCell*)(theCellContainer->findCell(cell->calo_hash()));
          if(theCaloCell) {
            //            log << MSG::VERBOSE << "found calo cell : eta=" <<theCaloCell->caloDDE()->eta()<<" phi="<<theCaloCell->caloDDE()->phi()<<" overlap="<<iter->second<<"old e=" <<theCaloCell->energy()<< " ; new e=" <<theCaloCell->energy()+energy*iter->second<< endmsg;
            theCaloCell->setEnergy(theCaloCell->energy()+ecell);

            if(m_storeFastShowerInfo) fastshowerinfo->AddCellEFinal(sample, ecell );
          } else {
            ATH_MSG_WARNING("det_elm found eta=" <<cell->eta()<<" phi="<<cell->phi()<<" hash="<<cell->calo_hash()
                            << " : e=" <<ecell<< " not filled!!! doing nothing!!!");
            //            theCaloCell=new CaloCell(cell,ecell,0,1,CaloGain::UNKNOWNGAIN);
            //            theCellContainer->push_back(theCaloCell);

            if(m_storeFastShowerInfo) fastshowerinfo->AddCellEFinal(sample);
          }
        }

        if(fabs(E_orig - elayertot3)>0.1) {
          ATH_MSG_ERROR("calosample "<<sample<<" : energy not fully deposited, E("<<sample<<")="<<E_orig<<", Et("<<sample<<")="<<Et_orig<<", deposit="<<elayertot3);
          ATH_MSG_ERROR(" - "<<particle_info_str.str());
          if(shape) ATH_MSG_ERROR("parametrization  : "<< shape->GetTitle());
        }

        double et_elayertot3=elayertot3/cosh(eta_calo_surf);

        if(sample>=CaloCell_ID_FCS::PreSamplerB && sample<=CaloCell_ID_FCS::EME3) {
          stats.m_E_tot_em+=elayertot3;
          stats.m_Et_tot_em+=et_elayertot3;
        } else {
          stats.m_E_tot_had+=elayertot3;
          stats.m_Et_tot_had+=et_elayertot3;
        }
        stats.m_E_tot_sample[sample]+=elayertot3;
        stats.m_Et_tot_sample[sample]+=et_elayertot3;

        stats.m_E_lost_sample[sample]-=elayertot3;
        stats.m_Et_lost_sample[sample]-=et_elayertot3;

        ATH_MSG_DEBUG("sample "<<sample<<" etot="<<p.E_layer[sample]<<" e1="<<elayertot<<" e2="<<elayertot2<<" e3="<<elayertot3);

        if(m_storeFastShowerInfo) {
          if(ibestcell>=0) {
            fastshowerinfo->SetLayerInfo( sample, elayertot, elayertot2, elayertot3, theDDE[ibestcell]->eta(), theDDE[ibestcell]->phi(), (unsigned int)theDDE[ibestcell]->calo_hash() );
          } else {
            fastshowerinfo->SetLayerInfo( sample, elayertot, elayertot2, elayertot3 );
          }
        }
      } else {
        MSG::Level level=MSG::DEBUG;
        bool is_debug=false;
        if(fabs(letaCalo[sample])>5 && fabs(letaCalo[sample])<10) is_debug=true;
        if(fabs(ptruth_eta)>4.9) is_debug=true;
        if(Et_orig>100 && (!is_debug) ) level=MSG::WARNING;
        if(Et_orig>1) {
          ATH_MSG_LVL(level,"calosample "<<sample<<" : no eta on calo found for truth impact ("<<letaCalo[sample]<<","<<lphiCalo[sample]<<"), E("<<sample<<")="<<E_orig<<", Et("<<sample<<")="<<Et_orig);
          ATH_MSG_LVL(level," - "<<particle_info_str.str());
          ATH_MSG_LVL(level," - skip sample...");
        }
      }
    }

  if(m_storeFastShowerInfo && fastShowerInfoContainer) {
    p.CopyDebugInfo( fastshowerinfo ); // old version: fastshowerinfo->SetParticleEnergyShape( p );

    ATH_MSG_DEBUG("Adding FastShowerInfoObject to the container");
    fastShowerInfoContainer->push_back(fastshowerinfo);
  }

  return StatusCode::SUCCESS;
}


bool FastShowerCellBuilderTool::Is_ID_Vertex(HepMC::GenVertex* ver) const
{
  if(ver) {
    double inr=ver->position().perp();
    double inz=fabs(ver->position().z());
    int nc=TMath::Min(m_ID_cylinder_r.size(),m_ID_cylinder_z.size());
    for(int ic=0;ic<nc;++ic) {
      if(inz<=m_ID_cylinder_z[ic] && inr<=m_ID_cylinder_r[ic]) return true;
    }
    return false;
  } else {
    return false;
  }
}

bool FastShowerCellBuilderTool::Is_EM_Vertex(HepMC::GenVertex* ver) const
{
  if(ver) {
    for(HepMC::GenVertex::particles_in_const_iterator pin=ver->particles_in_const_begin();pin!=ver->particles_in_const_end();++pin) {
      const HepMC::GenParticle* par=*pin;
      int absid=abs(par->pdg_id());
      if(!(absid==11 || absid==22)) return false;
    }
    for(HepMC::GenVertex::particles_out_const_iterator pout=ver->particles_out_const_begin();pout!=ver->particles_out_const_end();++pout) {
      const HepMC::GenParticle* par=*pout;
      int absid=abs(par->pdg_id());
      if(!(absid==11 || absid==22)) return false;
    }
  } else {
    return false;
  }
  return true;
}

FastShowerCellBuilderTool::flag_simul_sate FastShowerCellBuilderTool::Is_below_v14_truth_cuts_Vertex(HepMC::GenVertex* ver) const
{
  if(ver) {
    int nin=0;
    int id_in=0;
    const HepMC::GenParticle* par_in=0;
    for(HepMC::GenVertex::particles_in_const_iterator pin=ver->particles_in_const_begin();pin!=ver->particles_in_const_end();++pin) {
      par_in=*pin;
      id_in=par_in->pdg_id();
      ++nin;
    }
    if(nin!=1) return zero_state;
    int nout=0;
    int nele=0;
    int ngamma=0;
    int nother=0;
    for(HepMC::GenVertex::particles_out_const_iterator pout=ver->particles_out_const_begin();pout!=ver->particles_out_const_end();++pout) {
      const HepMC::GenParticle* par=*pout;
      int absid=abs(par->pdg_id());
      if(absid==22) {
        ++ngamma;
      } else {
        if(absid==11) ++nele;
        else ++nother;
      }
      ++nout;
    }
    if(abs(id_in)==11 && nout<=2 && nele==1 && nother==0) {
      //Bremsstrahlung, but bremsphoton might not be stored
      if(par_in->momentum().e()<500) return v14_truth_brems;
    }
    if(id_in==22 && nout<=2 && nele>=1 && ngamma==0 && nother==0) {
      //photon conversion to e+e-, but one e might not be stored
      if(par_in->momentum().e()<500) return v14_truth_conv;
    }
  } else {
    return zero_state;
  }
  return zero_state;
}

typedef FastShowerCellBuilderTool::MCdo_simul_state MCdo_simul_state;
typedef FastShowerCellBuilderTool::MCparticleCollection MCparticleCollection ;
typedef FastShowerCellBuilderTool::MCparticleCollectionCIter MCparticleCollectionCIter ;

void MC_init_particle_simul_state(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles)
{
  MCparticleCollectionCIter ip;
  for(ip=particles.begin();ip<particles.end();++ip){
    const HepMC::GenParticle* par=*ip;
    if(par) {
      do_simul_state[par->barcode()]=1;
      HepMC::GenVertex* outver=par->end_vertex();
      HepMC::GenVertex* inver =par->production_vertex();
      if(outver) {
        do_simul_state[outver->barcode()]=1;
      }
      if(inver) {
        do_simul_state[inver->barcode()]=1;
      }
    }
  }
}

void MC_recursive_remove_out_particles(MCdo_simul_state& do_simul_state,HepMC::GenVertex* ver,FastShowerCellBuilderTool::flag_simul_sate simul_state)
{
  if(ver) {
    //    if(do_simul_state[ver->barcode()]<=0) return;
    do_simul_state[ver->barcode()]=simul_state;
    for(HepMC::GenVertex::particles_out_const_iterator pout=ver->particles_out_const_begin();pout!=ver->particles_out_const_end();++pout) {
      const HepMC::GenParticle* par=*pout;
      do_simul_state[par->barcode()]=simul_state;
      HepMC::GenVertex* outver=par->end_vertex();
      if(outver) MC_recursive_remove_out_particles(do_simul_state,outver,simul_state);
    }
  }
}

void MC_recursive_remove_in_particles(MCdo_simul_state& do_simul_state,HepMC::GenVertex* ver,FastShowerCellBuilderTool::flag_simul_sate simul_state)
{
  if(ver) {
    if(do_simul_state[ver->barcode()]==simul_state) {
      //log << MSG::DEBUG<<"ver: bc="<<ver->barcode()<<" : loop in MC_recursive_remove_in_particles, returning"<<endmsg;
      return;
    }
    do_simul_state[ver->barcode()]=simul_state;
    for(HepMC::GenVertex::particles_in_const_iterator pin=ver->particles_in_const_begin();pin!=ver->particles_in_const_end();++pin) {
      const HepMC::GenParticle* par=*pin;
      do_simul_state[par->barcode()]=simul_state;
      HepMC::GenVertex* inver=par->production_vertex();
      if(inver) MC_recursive_remove_in_particles(do_simul_state,inver,simul_state);
    }
  }
}

/*
  void print_MC_info(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles,MsgStream& log)
  {
  log << MSG::VERBOSE <<"print_MC_info: begin"<< endmsg;
  MCparticleCollectionCIter ip;
  for(ip=particles.begin();ip<particles.end();++ip){
  const HepMC::GenParticle* par=*ip;

  if(log.level()<=MSG::DEBUG) {
  std::string reason="---";
  if(do_simul_state[par->barcode()]<=0) {
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::out_of_ID) reason="-ID";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::non_EM_vertex) reason="-EM";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::heavy_ion) reason="-HI";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::pdg_id_unkown) reason="-PI";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::invisibleArray) reason="-IA";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::invisibleTruthHelper) reason="-IT";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::mother_particle) reason="-MO";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::v14_truth_brems) reason="-BR";
  if(do_simul_state[par->barcode()]==FastShowerCellBuilderTool::v14_truth_conv) reason="-CO";
  } else {
  reason="+OK";
  }
  log << MSG::DEBUG<<reason;

  log <<": "<<"bc="<<par->barcode()<<" id="<<par->pdg_id()<<" stat="<<par->status()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi();
  HepMC::GenVertex*  inver =par->production_vertex();
  HepMC::GenVertex* outver =par->end_vertex();
  if(inver) {
  double inr=inver->position().perp();
  double inz=inver->position().z();
  log<<" ; r="<<inr<<" z="<<inz<<" phi="<<inver->position().phi()<<" ; ";
  bool sep=false;
  for(HepMC::GenVertex::particles_in_const_iterator pin=inver->particles_in_const_begin();pin!=inver->particles_in_const_end();++pin) {
  const HepMC::GenParticle* invpar=*pin;
  if(invpar) {
  if(sep) log<<",";
  log<<invpar->barcode();
  if(do_simul_state[invpar->barcode()]<=0) log<<"-";
  else log<<"+";
  sep=true;
  }
  }
  }
  log<<"->"<<par->barcode();
  if(outver) {
  log<<"->";
  bool sep=false;
  for(HepMC::GenVertex::particles_out_const_iterator pout=outver->particles_out_const_begin();pout!=outver->particles_out_const_end();++pout) {
  const HepMC::GenParticle* outpar=*pout;
  if(outpar) {
  if(sep) log<<",";
  log<<outpar->barcode();
  if(do_simul_state[outpar->barcode()]<=0) log<<"-";
  else log<<"+";
  sep=true;
  }
  }
  }
  log<<endmsg;
  }
  }

  log << MSG::VERBOSE <<"print_MC_info: end"<< endmsg;
  }
*/

void FastShowerCellBuilderTool::MC_remove_out_of_ID(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles) const
{
  MCparticleCollectionCIter ip;
  for(ip=particles.begin();ip<particles.end();++ip){
    const HepMC::GenParticle* par=*ip;
    if(par) {
      if(do_simul_state[par->barcode()]<=0) continue;

      HepMC::GenVertex* inver =par->production_vertex();
      if(inver) {
        if(!Is_ID_Vertex(inver)) {
          int nin=0;
          for(HepMC::GenVertex::particles_in_const_iterator pin=inver->particles_in_const_begin();pin!=inver->particles_in_const_end();++pin) {
            const HepMC::GenParticle* invpar=*pin;
            if(invpar) {
              if(do_simul_state[invpar->barcode()]>0) {
                ++nin;
                break;
              }
            }
          }

          if(nin>0) {
            MC_recursive_remove_out_particles(do_simul_state,inver,out_of_ID);
          }
        }
      }
    }
  }
}

void FastShowerCellBuilderTool::MC_remove_out_of_EM(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles) const
{
  MCparticleCollectionCIter ip;
  for(ip=particles.begin();ip<particles.end();++ip){
    const HepMC::GenParticle* par=*ip;
    if(par) {
      if(do_simul_state[par->barcode()]<=0) continue;

      HepMC::GenVertex* outver =par->end_vertex();
      if(outver) {
        if(outver->barcode()>=-200000) continue;

        if(!Is_EM_Vertex(outver)) {
          MC_recursive_remove_out_particles(do_simul_state,outver,non_EM_vertex);
        }
      }
    }
  }
}

void FastShowerCellBuilderTool::MC_remove_below_v14_truth_cuts(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles) const
{
  MCparticleCollectionCIter ip;
  for(ip=particles.begin();ip<particles.end();++ip){
    const HepMC::GenParticle* par=*ip;
    if(par) {
      if(do_simul_state[par->barcode()]<=0) continue;

      HepMC::GenVertex* outver =par->end_vertex();
      if(outver) {
        if(outver->barcode()>=-200000) continue;

        flag_simul_sate reason=Is_below_v14_truth_cuts_Vertex(outver);
        if(reason<0) {
          MC_recursive_remove_out_particles(do_simul_state,outver,reason);
        }
      }
    }
  }
}

void MC_remove_decay_to_simul(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles)
{
  MCparticleCollectionCIter ip;
  for(ip=particles.begin();ip<particles.end();++ip){
    const HepMC::GenParticle* par=*ip;
    if(par) {
      if(do_simul_state[par->barcode()]<=0) continue;

      HepMC::GenVertex* inver =par->production_vertex();
      if(inver) {
        MC_recursive_remove_in_particles(do_simul_state,inver,FastShowerCellBuilderTool::mother_particle);
      }
    }
  }
}

void FastShowerCellBuilderTool::init_shape_correction()
{
  if(m_shape_correction.size()==0) return;

  ATH_MSG_INFO("Assigning shape correction functions...");
  for(t_map_PSP_ID::iterator iter_id=m_map_ParticleShapeParametrizationMap.begin();
      iter_id!=m_map_ParticleShapeParametrizationMap.end();
      ++iter_id) {
    for(t_map_PSP_calosample::iterator iter_cs=iter_id->second.begin();
        iter_cs!=iter_id->second.end();
        ++iter_cs) {
      for(t_map_PSP_Energy::iterator iter_E=iter_cs->second.begin();
          iter_E!=iter_cs->second.end();
          ++iter_E) {
        int n_assign=0;
        for(t_map_PSP_DistEta::iterator iter_disteta=iter_E->second.begin();
            iter_disteta<iter_E->second.end();
            ++iter_disteta) {
          for(t_shape_correction::iterator corr=m_shape_correction.begin();
              corr<m_shape_correction.end();
              ++corr) {
            if((*corr)->is_match((*iter_disteta)->id(),
                                 (*iter_disteta)->calosample(),
                                 (*iter_disteta)->E(),
                                 (*iter_disteta)->eta(),
                                 (*iter_disteta)->meandist())) {
              (*iter_disteta)->AddShapeCorrection(*corr);
              ++n_assign;
            }
          }
        }
        if(n_assign>0) {
          ATH_MSG_DEBUG("  ID="<<iter_id->first<< " calosample="<<iter_cs->first<< " E="<<iter_E->first
                        <<" : #shape corrections="<<n_assign);
        }
      }
    }
  }
}

StatusCode FastShowerCellBuilderTool::process(CaloCellContainer* theCellContainer)
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ATH_MSG_DEBUG("Executing start calo size=" <<theCellContainer->size()<<" Event="<<ctx.evt());

  TRandom3 rndm;
  if(setupEvent(ctx, rndm).isFailure() ) {
    ATH_MSG_ERROR("setupEvent() failed");
    return StatusCode::FAILURE;
  }

  //FastCaloSimIsGenSimulStable ifs;
  //TruthHelper::IsGenNonInteracting invisible;

  MCparticleCollection particles;
  MCparticleCollection Simulparticles;
  MCdo_simul_state     do_simul_state;

  SG::ReadHandle<McEventCollection> mcCollptr (m_mcCollectionKey, ctx);

  // initialize a pileup type helper object
  //PileUpType pileupType( mcCollptr );

  ATH_MSG_DEBUG("Start getting particles");

  // pileupType.signal_particles(particles, isStable);
  //pileupType.signal_particles(particles, ifs);
  //ZH 28.07.2014 Try using TruthUtils instead:

  if (mcCollptr->size() >0)
    {
      HepMC::GenEvent::particle_const_iterator istart = mcCollptr->at(0)->particles_begin();
      HepMC::GenEvent::particle_const_iterator iend   = mcCollptr->at(0)->particles_end();
      for ( ; istart!= iend; ++istart)
        {
          //std::cout <<" ("<< FastCaloSimIsGenSimulStable(*istart)<<"/"<<(*istart)->barcode()<<","<<(*istart)->status()<<"/"<<ifs(*istart)<<") ";
          particles.push_back(*istart);
        }
      //std::cout <<std::endl;
    }
  particles = MC::filter_keep(particles, FastCaloSimIsGenSimulStable);


  //sc = m_gentesIO->getMC(particles, &ifs, m_mcLocation );
  //if ( sc.isFailure() ) {
  //  log << MSG::ERROR << "getMC from "<<m_mcLocation<<" failed "<< endmsg;
  //  return StatusCode::FAILURE;
  //}

  const BarcodeEnergyDepositMap* MuonEnergyMap=0;
  if (!m_MuonEnergyInCaloContainerKey.key().empty()) {
    SG::ReadHandle<BarcodeEnergyDepositMap> h (m_MuonEnergyInCaloContainerKey, ctx);
    // FIXME: Fix configuration so as not to request this object if it does not exist.
    if (h.isValid()) {
      MuonEnergyMap = h.cptr();
    }
    else {
      ATH_MSG_DEBUG("Could not find "<<m_MuonEnergyInCaloContainerKey.key()<<" in SG ");
    }
  }

  MC_init_particle_simul_state(do_simul_state,particles);
  if(m_simul_ID_only) MC_remove_out_of_ID(do_simul_state,particles);
  if(m_simul_ID_v14_truth_cuts) MC_remove_below_v14_truth_cuts(do_simul_state,particles);
  if(m_simul_EM_geant_only) MC_remove_out_of_EM(do_simul_state,particles);
  MC_remove_decay_to_simul(do_simul_state,particles);

  MCparticleCollectionCIter ip;
  int indpar=0;
  ATH_MSG_DEBUG("start finding partilces n="<<particles.size());

  for(ip=particles.begin();ip<particles.end();++ip){
    const HepMC::GenParticle* par=*ip;
    if(par->pdg_id()==0) do_simul_state[par->barcode()]=pdg_id_unkown;
    // Do not treat nuclei
    if((!m_simul_heavy_ions) && par->pdg_id() > 1000000000) do_simul_state[par->barcode()]=heavy_ion;

    for(unsigned int i=0;i<m_invisibles.size();++i) {
      if(abs(par->pdg_id())==m_invisibles[i]) {
        do_simul_state[par->barcode()]=invisibleArray;
        //log << MSG::DEBUG <<"INVISIBLE by ARRAY: id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<endmsg;
        break;
      }
      if(m_invisibles[i]==0) {
        if(MC::isNonInteracting(par)) {
          do_simul_state[par->barcode()]=invisibleTruthHelper;
          //log << MSG::DEBUG <<"INVISIBLE by TruthHelper: id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<endmsg;
          break;
        }
      }
    }

    if(abs(par->pdg_id())==13 && MuonEnergyMap) {
      std::pair<BarcodeEnergyDepositMap::const_iterator,BarcodeEnergyDepositMap::const_iterator> range=MuonEnergyMap->equal_range(par->barcode());
      if(range.first==range.second) {
        do_simul_state[par->barcode()]=0;
        ATH_MSG_DEBUG("#="<<indpar<<": id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()
                      <<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()
                      << " : no calo energy deposit");
      } else {
        if(msgLvl(MSG::DEBUG)) {
          for(BarcodeEnergyDepositMap::const_iterator i=range.first;i!=range.second;++i) {
            msg(MSG::DEBUG)<<"#="<<indpar<<": id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()
                           <<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()
                           <<" : layer="<<i->second.sample<<" eta="<<i->second.position.eta()<<" phi="<<i->second.position.phi()
                           <<" d="<<i->second.position.mag();
            if(i->second.materialEffects) if(i->second.materialEffects->energyLoss()) msg()<<" de="<<i->second.materialEffects->energyLoss()->deltaE();
            msg()<< endmsg;
          }
        }
      }
    }

    if(msgLvl(MSG::DEBUG)) {
      std::string reason="---";
      if(do_simul_state[par->barcode()]<=0) {
        if(do_simul_state[par->barcode()]==out_of_ID) reason="-ID";
        if(do_simul_state[par->barcode()]==non_EM_vertex) reason="-EM";
        if(do_simul_state[par->barcode()]==heavy_ion) reason="-HI";
        if(do_simul_state[par->barcode()]==pdg_id_unkown) reason="-PI";
        if(do_simul_state[par->barcode()]==invisibleArray) reason="-IA";
        if(do_simul_state[par->barcode()]==invisibleTruthHelper) reason="-IT";
        if(do_simul_state[par->barcode()]==mother_particle) reason="-MO";
        if(do_simul_state[par->barcode()]==v14_truth_brems) reason="-BR";
        if(do_simul_state[par->barcode()]==v14_truth_conv) reason="-CO";
      } else {
        reason="+OK";
      }
      msg(MSG::DEBUG)<<reason;

      msg()<<indpar<<": id="<<par->pdg_id()<<" stat="<<par->status()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi();
      HepMC::GenVertex*  inver =par->production_vertex();
      HepMC::GenVertex* outver =par->end_vertex();
      if(inver) {
        double inr=inver->position().perp();
        double inz=inver->position().z();
        msg()<<" ; bc="<<inver->barcode()<<" r="<<inr<<" z="<<inz<<" phi="<<inver->position().phi()<<" ; ";
        bool sep=false;
        for(HepMC::GenVertex::particles_in_const_iterator pin=inver->particles_in_const_begin();pin!=inver->particles_in_const_end();++pin) {
          const HepMC::GenParticle* invpar=*pin;
          if(invpar) {
            if(sep) msg()<<",";
            msg()<<invpar->barcode();
            sep=true;
          }
        }
      }
      msg()<<"->"<<par->barcode();
      if(outver) {
        msg()<<"->";
        bool sep=false;
        for(HepMC::GenVertex::particles_out_const_iterator pout=outver->particles_out_const_begin();pout!=outver->particles_out_const_end();++pout) {
          const HepMC::GenParticle* outpar=*pout;
          if(outpar) {
            if(sep) msg()<<",";
            msg()<<outpar->barcode();
            sep=true;
          }
        }
      }
      msg()<<endmsg;
    }

    if(do_simul_state[par->barcode()]<=0) {
      continue;
    }

    ++indpar;

    //    if(DoParticleSimul(par,log)) {
    {
      //      log<<MSG::DEBUG<<"-> OK for simul"<<endmsg;
      Simulparticles.push_back(par);
      //      if(Is_Particle_from_pdgid(par,log,15,1)) log<<MSG::INFO<<""<<endmsg;

      //      deflectParticle(particlesAtCal,par);
    }
  }
  ATH_MSG_DEBUG("finished finding particles");

  std::unique_ptr<FastShowerInfoContainer> fastShowerInfoContainer;
  if(m_storeFastShowerInfo)
    {
      fastShowerInfoContainer = std::make_unique<FastShowerInfoContainer>();
    }

  MCparticleCollectionCIter fpart = Simulparticles.begin();
  MCparticleCollectionCIter lpart = Simulparticles.end();

  Stats stats;
  int stat_npar=0;
  int stat_npar_OK=0;
  int stat_npar_nOK=0;
  for(MCparticleCollectionCIter part=fpart;part<lpart;++part) {

    std::vector<Trk::HitInfo>* hitVector = caloHits(*(*part));

    Amg::Vector3D mom((*part)->momentum().x(),(*part)->momentum().y(),(*part)->momentum().z());

    if(process_particle(theCellContainer,
                        hitVector,
                        mom,
                        (*part)->generated_mass(),
                        (*part)->pdg_id(),
                        (*part)->barcode(),
                        fastShowerInfoContainer.get(),
                        rndm,
                        stats,
                        ctx))
    {
      ++stat_npar_nOK;
    } else {
      ++stat_npar_OK;
    }

    for(std::vector<Trk::HitInfo>::iterator it = hitVector->begin();it < hitVector->end();++it)  {
      if((*it).trackParms) {
        delete (*it).trackParms;
        (*it).trackParms=0;
      }
    }
    delete hitVector;

    ++stat_npar;
  }
  //std::cout <<"ZH Processed: "<<stat_npar<<"(ok: "<<stat_npar_nOK<<")"<<std::endl;
  /*
    MCparticleCollection all_particles;
    log <<MSG::DEBUG<<"Start getting all particles"<<endmsg;
    sc = m_gentesIO->getMC(all_particles, m_mcLocation );

    if ( sc.isFailure() ) {
    log << MSG::ERROR << "getMC from "<<m_mcLocation<<" failed "<< endmsg;
    return StatusCode::FAILURE;
    }

    log <<MSG::INFO<<"Got all particles n="<<all_particles.size()<<endmsg;
    for(ip=all_particles.begin(); ip<all_particles.end(); ++ip){
    const HepMC::GenParticle* par=*ip;
    if(abs(par->pdg_id())!=15) continue;
    std::vector<double> sums(3);
    sums[0]=sums[1]=sums[2]=0;
    sum_par(par,log,sums,0);
    print_par(par,log,0);
    }
  */

  ATH_MSG_DEBUG("Executing finished calo size=" <<theCellContainer->size()<<"; "<<stat_npar<<" particle(s), "<<stat_npar_OK<<" with sc=SUCCESS");

  if(m_storeFastShowerInfo)
  {
    ATH_MSG_DEBUG("Registering the FastShowerInfoContainer with key " << m_FastShowerInfoContainerKey);
    SG::WriteHandle<FastShowerInfoContainer> showerHandle (m_FastShowerInfoContainerKey, ctx);
    ATH_CHECK( showerHandle.record (std::move (fastShowerInfoContainer)) );
  }

  if(releaseEvent(stats).isFailure() ) {
    ATH_MSG_ERROR("releaseEvent() failed");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode FastShowerCellBuilderTool::setupEvent (const EventContext& ctx,
                                                  TRandom3& rndm) const
{
  m_rndmSvc->printEngineState(this,m_randomEngineName);
  unsigned int rseed=0;
  CLHEP::HepRandomEngine* engine = m_randomEngine->getEngine(ctx);
  while(rseed==0) {
    rseed=(unsigned int)( CLHEP::RandFlat::shoot(engine) * std::numeric_limits<unsigned int>::max() );
  }
  rndm.SetSeed(rseed);

  //log<<" seed(rndm="<<rndm.ClassName()<<")="<<rndm.GetSeed();
  //log<< endmsg;

  return StatusCode::SUCCESS;
}

StatusCode FastShowerCellBuilderTool::releaseEvent (Stats& stats) const
{
  ATH_MSG_DEBUG("Event summary e="<<stats.m_E_tot_em+stats.m_E_tot_had<<" e(EM)="<<stats.m_E_tot_em<<" e(HAD)="<<stats.m_E_tot_had
                <<" ; et="<<stats.m_Et_tot_em+stats.m_Et_tot_had<<" et(EM)="<<stats.m_Et_tot_em<<" et(HAD)="<<stats.m_Et_tot_had);

  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    if(fabs(stats.m_Et_lost_sample[sample])<1E-6) stats.m_Et_lost_sample[sample]=0;
    if(fabs(stats.m_E_lost_sample[sample])<1E-6) stats.m_E_lost_sample[sample]=0;
    if(stats.m_Et_lost_sample[sample]>5000) {
      ATH_MSG_WARNING("  Event summary sample "<<sample<<" : etlost="<<stats.m_Et_lost_sample[sample]<<" elost="<<stats.m_E_lost_sample[sample]);
    } else {
      ATH_MSG_DEBUG("  Event summary sample "<<sample<<" : etlost="<<stats.m_Et_lost_sample[sample]<<" elost="<<stats.m_E_lost_sample[sample]);
    }
  }

  return StatusCode::SUCCESS;
}


/*
  void FastShowerCellBuilderTool::deflectParticles(ITransportedParticleCollection &itpc)
  {
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "FastShowerCellBuilderTool deflectParticles()" << endmsg;

  // read MC particles from TES
  typedef std::vector<const HepMC::GenParticle*>  MCparticleCollection ;
  typedef std::vector<const HepMC::GenParticle*>::const_iterator  MCparticleCollectionCIter ;

  MCparticleCollection p;
  TesIoStat stat = m_tesIO->getMC( p, m_mcSelector ) ;
  std::string mess;
  mess = stat? "Retrieved MC from TES ":"Failed MC retrieve from TES ";
  log << MSG::DEBUG << mess << p.size()<<endmsg;

  MCparticleCollectionCIter ip= p.begin();
  for(; ip<p.end(); ++ip){
  deflectParticle(itpc,*ip);
  }
  }

  void FastShowerCellBuilderTool::deflectParticle(Atlfast::ITransportedParticleCollection &itpc,const HepMC::GenParticle* par)
  {
  ITransportedParticle *itp = new TransportedHelixParticle(par);
  itp->deflect();
  itpc.push_back(itp);
  }

*/

std::vector<Trk::HitInfo>* FastShowerCellBuilderTool::caloHits(const HepMC::GenParticle& part) const
{
  // Start calo extrapolation
  ATH_MSG_DEBUG ("[ fastCaloSim transport ] processing particle "<<part.pdg_id() );

  std::vector<Trk::HitInfo>*     hitVector =  new std::vector<Trk::HitInfo>;

  int     pdgId    = part.pdg_id();
  double  charge   = HepPDT::ParticleID(pdgId).charge();

  // particle Hypothesis for the extrapolation

  Trk::ParticleHypothesis pHypothesis = m_pdgToParticleHypothesis.convert(pdgId,charge);

  ATH_MSG_DEBUG ("particle hypothesis "<< pHypothesis );

  // geantinos not handled by PdgToParticleHypothesis - fix there
  if ( pdgId == 999 ) pHypothesis = Trk::geantino;

  HepMC::GenVertex *vtx = part.production_vertex();
  Amg::Vector3D pos(0.,0.,0.);    // default

  if (vtx) {
    //const HepMC::ThreeVector vtxPos(vtx->point3d());
    pos = Amg::Vector3D( vtx->point3d().x(),vtx->point3d().y(), vtx->point3d().z());
  }

  ATH_MSG_DEBUG( "[ fastCaloSim transport ] starting transport from position "<< pos );

  Amg::Vector3D mom(part.momentum().x(),part.momentum().y(),part.momentum().z());

  // input parameters : curvilinear parameters
  Trk::CurvilinearParameters inputPar(pos,mom,charge);

  // stable vs. unstable check : ADAPT for FASTCALOSIM
  //double freepath = ( !m_particleDecayHelper.empty()) ? m_particleDecayHelper->freePath(isp) : - 1.;
  double freepath = -1.;
  //ATH_MSG_VERBOSE( "[ fatras transport ] Particle free path : " << freepath);
  // path limit -> time limit  ( TODO : extract life-time directly from decay helper )
  double tDec = freepath > 0. ? freepath : -1.;
  int decayProc = 0;

  /* uncomment if unstable particles used by FastCaloSim
  // beta calculated here for further use in validation
  double mass = m_particleMasses.mass[pHypothesis];
  double mom = isp.momentum().mag();
  double beta = mom/sqrt(mom*mom+mass*mass);

  if ( tDec>0.) {
  tDec = tDec/beta/CLHEP::c_light + isp.timeStamp();
  decayProc = 201;
  }
  */

  Trk::TimeLimit timeLim(tDec,0.,decayProc);        // TODO: set vertex time info

  // prompt decay ( uncomment if unstable particles used )
  //if ( freepath>0. && freepath<0.01 ) {
  //  if (!m_particleDecayHelper.empty()) {
  //    ATH_MSG_VERBOSE( "[ fatras transport ] Decay is triggered for input particle.");
  //    m_particleDecayHelper->decay(isp);
  //  }
  //  return 0;
  //}

  // presample interactions - ADAPT FOR FASTCALOSIM
  Trk::PathLimit pathLim(-1.,0);
  //if (absPdg!=999 && pHypothesis<99) pathLim = m_samplingTool->sampleProcess(mom,isp.charge(),pHypothesis);

  Trk::GeometrySignature nextGeoID=Trk::Calo;

  // first extrapolation to reach the ID boundary

  // get CaloEntrance if not done already
  if (!m_caloEntrance) {
    m_caloEntrance = m_extrapolator->trackingGeometry()->trackingVolume(m_caloEntranceName);
    if (!m_caloEntrance)  ATH_MSG_INFO("CaloEntrance not found ");
  }

  const Trk::TrackParameters* caloEntry = 0;

  if (m_caloEntrance && m_caloEntrance->inside(pos,0.001) &&
      !m_extrapolator->trackingGeometry()->atVolumeBoundary(pos,m_caloEntrance,0.001)) {

    std::vector<Trk::HitInfo>*     dummyHitVector = 0;
    if ( charge==0 ) {

      caloEntry = m_extrapolator->transportNeutralsWithPathLimit(inputPar,pathLim,timeLim,
                                                                 Trk::alongMomentum,pHypothesis,dummyHitVector,nextGeoID,m_caloEntrance);

    } else {

      caloEntry = m_extrapolator->extrapolateWithPathLimit(inputPar,pathLim,timeLim,
                                                           Trk::alongMomentum,pHypothesis,dummyHitVector,nextGeoID,m_caloEntrance);
    }
  } else caloEntry=&inputPar;

  if ( caloEntry ) {

    const Trk::TrackParameters* eParameters = 0;

    // save Calo entry hit (fallback info)
    hitVector->push_back(Trk::HitInfo(caloEntry->clone(),timeLim.time,nextGeoID,0.));

    ATH_MSG_DEBUG( "[ fastCaloSim transport ] starting Calo transport from position "<< pos );

    if ( charge==0 ) {

      eParameters = m_extrapolator->transportNeutralsWithPathLimit(*caloEntry,pathLim,timeLim,
                                                                   Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);
    } else {

      eParameters = m_extrapolator->extrapolateWithPathLimit(*caloEntry,pathLim,timeLim,
                                                             Trk::alongMomentum,pHypothesis,hitVector,nextGeoID);
    }

    // save Calo exit hit (fallback info)
    if (eParameters) hitVector->push_back(Trk::HitInfo(eParameters,timeLim.time,nextGeoID,0.));

    //delete eParameters;   // HitInfo took ownership

  }

  return hitVector;

}
