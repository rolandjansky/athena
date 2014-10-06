/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#if FastCaloSim_project_release_v1 == 12
  #include "PartPropSvc/PartPropSvc.h"
  #include "CLHEP/HepPDT/ParticleData.hh"
#else  
  #include "GaudiKernel/IPartPropSvc.h"
  #include "HepPDT/ParticleData.hh"
#endif  
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
//#include "TruthHelper/IsGenStable.h"
//#include "TruthHelper/IsGenerator.h"
//#include "TruthHelper/IsGenInteracting.h"
//#include "TruthHelper/IsGenNonInteracting.h"
//#include "TruthHelper/IsGenSimulStable.h"
#include "FastCaloSim/FastCaloSimIsGenSimulStable.h"
#include "TruthHelper/IsGenNonInteracting.h"

#include "PathResolver/PathResolver.h"

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
#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TruthHelper/PileUpType.h"
#include "GeoPrimitives/GeoPrimitives.h"

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

//using namespace Atlfast;
//using namespace FastShower;

#include "FastCaloSimAthenaPool/FastShowerInfo.h"
#include "FastCaloSimAthenaPool/FastShowerInfoContainer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <limits>

FastShowerCellBuilderTool::FastShowerCellBuilderTool(const std::string& type, const std::string& name, const IInterface* parent)
                          :BasicCellBuilderTool(type, name, parent),
                           m_mcLocation("TruthEvent"),
                           m_ParticleParametrizationFileName(""),
                           m_AdditionalParticleParametrizationFileNames(0),
                           m_DB_folder(0),
                           m_DB_channel(0),
                           m_DB_dirname(0),
                           m_MuonEnergyInCaloContainer("FatrasDepositedMuonEnergyInCalo"),
                           m_simul_ID_only(true),
                           m_simul_ID_v14_truth_cuts(false),
                           m_simul_EM_geant_only(false),
                           m_simul_heavy_ions(false),
                           p_coolhistsvc(0),
                           m_rndmSvc("AtDSFMTGenSvc", name),
                           m_randomEngine(0),
                           m_randomEngineName("FastCaloSimRnd"),
                           m_extrapolatorName("Trk::Extrapolator"),   
                           m_extrapolatorInstanceName("Extrapolator"),
                           m_calosurf_middle_InstanceName("CaloSurfaceBuilderMiddle"),
                           m_calosurf_entrance_InstanceName("CaloSurfaceBuilderEntrance"),
                           m_sampling_energy_reweighting(CaloCell_ID_FCS::MaxSample,1.0),
                           m_rndm(0),
                           m_invisibles(0),
                           m_is_init_shape_correction(false)
{ 
  declareInterface<ICaloCellMakerTool>( this );
 
  const int n_surfacelist=5;
  CaloCell_ID_FCS::CaloSample surfacelist[n_surfacelist]={CaloCell_ID_FCS::PreSamplerB,
                                                          CaloCell_ID_FCS::PreSamplerE,
                                                          CaloCell_ID_FCS::EME1,
                                                          CaloCell_ID_FCS::EME2,
                                                          CaloCell_ID_FCS::FCAL0
                                                         };
  m_n_surfacelist=n_surfacelist;
  for(int i=0;i<n_surfacelist;++i) m_surfacelist[i]=surfacelist[i];
  m_rndm=new TRandom3();

  declareProperty("McLocation",m_mcLocation);

  declareProperty("ParticleParametrizationFileName",m_ParticleParametrizationFileName);
  declareProperty("AdditionalParticleParametrizationFileNames",m_AdditionalParticleParametrizationFileNames);
  
  declareProperty("RandomService",                  m_rndmSvc,              "Name of the random number service");
  declareProperty("RandomStreamName",               m_randomEngineName,     "Name of the random number stream");

  declareProperty("MuonEnergyInCaloContainerName",  m_MuonEnergyInCaloContainer);
  
  declareProperty("DoSimulWithInnerDetectorTruthOnly",m_simul_ID_only);
  declareProperty("DoSimulWithInnerDetectorV14TruthCuts",m_simul_ID_v14_truth_cuts);
  declareProperty("DoSimulWithEMGeantInteractionsOnly",m_simul_EM_geant_only);
  declareProperty("DoSimulHeavyIonsInCalo",         m_simul_heavy_ions);

  declareProperty("ExtrapolatorName",               m_extrapolatorName );
  declareProperty("ExtrapolatorInstanceName",       m_extrapolatorInstanceName );

  declareProperty("CalosurfMiddleInstanceName",     m_calosurf_middle_InstanceName);
  declareProperty("CalosurfEntranceInstanceName",   m_calosurf_entrance_InstanceName);
  
  declareProperty("FastShowerInfoContainerKey",     m_FastShowerInfoContainerKey="FastShowerInfoContainer");
  declareProperty("StoreFastShowerInfo",            m_storeFastShowerInfo=false);
  
  declareProperty("DoEnergyInterpolation",          m_jo_interpolate=false); //ATA: make interpolation optional
  declareProperty("DoNewEnergyEtaSelection",        m_energy_eta_selection=false); //mwerner: make new selction of EnergyParam optional 
  declareProperty("use_Ekin_for_depositions",       m_use_Ekin_for_depositions=false);//Use the kinetic energy of a particle to as measure of the energie to deposit in the calo

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
    if(infile->IsOpen()) {
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

void FastShowerCellBuilderTool::LoadParametrizationsFromFile(TDirectory& infile,MSG::Level level)
{
  TIterator *iter=infile.GetListOfKeys()->MakeIterator();
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
      if(obj) {
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
  
  if ( BasicCellBuilderTool::initialize().isFailure() ) {
    return StatusCode::FAILURE;
  }
  
  IPartPropSvc* p_PartPropSvc=0;
  if (service("PartPropSvc",p_PartPropSvc).isFailure() || p_PartPropSvc == 0) {
    ATH_MSG_ERROR("could not find PartPropService");
    return StatusCode::FAILURE;
  }
  
  m_particleDataTable = (HepPDT::ParticleDataTable*) p_PartPropSvc->PDT();
  if(m_particleDataTable == 0){
    ATH_MSG_ERROR("PDG table not found");
    return StatusCode::FAILURE;
  }
  
  // Random number service
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve " << m_rndmSvc);
    return StatusCode::FAILURE;
  }

  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_ERROR("Could not get random engine '" << m_randomEngineName << "'");
    return StatusCode::FAILURE;
  }

  /*
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())
  {
    log << MSG::ERROR
        << "Unable to get pointer to StoreGateSvc"
        << endreq;
    return StatusCode::FAILURE;
  }
  */

  // Retrieve Tools
  IToolSvc* p_toolSvc = 0;
  if ( service("ToolSvc",p_toolSvc).isFailure() ){
    ATH_MSG_ERROR("Cannot find ToolSvc! ");
    return StatusCode::FAILURE;
  }
  else {
    IAlgTool* algTool;


    // Get TrkExtrapolator from ToolService
    if (p_toolSvc->retrieveTool(m_extrapolatorName, m_extrapolatorInstanceName , m_extrapolator).isFailure())
    {
      ATH_MSG_ERROR("Could not find Tool " << m_extrapolatorName << " and create instance " << m_extrapolatorInstanceName <<". Exiting.");
      return StatusCode::FAILURE; 
    } else {
      ATH_MSG_INFO("Successfully created tool  " << m_extrapolatorName << " with instance name " << m_extrapolatorInstanceName);
    }


    // Get the CaloSurface tool and sets the depthtool it should use:
    if (p_toolSvc->retrieveTool("CaloSurfaceBuilder",m_calosurf_middle_InstanceName,m_calosurf_middle,this).isFailure())
    { 
      ATH_MSG_ERROR("Could not find Tool "<< m_calosurf_middle_InstanceName);
      return StatusCode::FAILURE;
    }     

    // Get the CaloSurface tool and sets the depthtool it should use:
    if (p_toolSvc->retrieveTool("CaloSurfaceBuilder",m_calosurf_entrance_InstanceName,m_calosurf_entrance,this).isFailure())
    { 
      ATH_MSG_ERROR("Could not find Tool "<< m_calosurf_entrance_InstanceName);
      return StatusCode::FAILURE;
    }     


    std::string CaloCoordinateTool_name="TBCaloCoordinate";
    ListItem CaloCoordinateTool(CaloCoordinateTool_name);
    if ( p_toolSvc->retrieveTool(CaloCoordinateTool.type(),CaloCoordinateTool.name(), algTool, this).isFailure() ) {
      ATH_MSG_ERROR("Cannot retrieve " << CaloCoordinateTool_name);
      return StatusCode::FAILURE;
    }
    m_calo_tb_coord = dynamic_cast<ICaloCoordinateTool*>(algTool);    
    if ( !m_calo_tb_coord ) {
      ATH_MSG_ERROR("Cannot retrieve " << CaloCoordinateTool_name);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("retrieved " << CaloCoordinateTool_name);
    }

  }

  find_phi0();

  ATH_MSG_INFO("========================= Init EM map =============================");
  em_map.init(-5,+5,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,100,64);
  em_map.setname("EM");

  ATH_MSG_INFO("========================= Init EM fine map ========================");
  em_fine_map.init(-2.8,+2.8,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,224,256);
  em_fine_map.setname("EM fine");

  ATH_MSG_INFO("========================= Init HAD map ============================");
  had_map.init(-5,+5,-M_PI+m_phi0_had,+M_PI+m_phi0_had,100,64);
  had_map.setname("HAD");

  ATH_MSG_INFO("========================= Init EM celllist map =============================");
  em_celllist_map.init(-5,+5,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,100,64,2,2);
  em_celllist_map.setname("EMlist");
  
  ATH_MSG_INFO("========================= Init celllist maps sample 0 ... "<< CaloCell_ID_FCS::LastSample);
  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    //log << MSG::INFO <<  "========================= Init celllist map sample "<<sample<<" =============================" <<endreq;
    celllist_maps[sample].init(-5,+5,-M_PI+m_phi0_em ,+M_PI+m_phi0_em ,100,64,3,3);
    celllist_maps[sample].setname("samplecelllist");
//    celllist_maps[sample];
  }

  init_all_maps();
  ATH_MSG_INFO("========================= Init volume all maps =========================");
  init_volume(em_map);
  init_volume(em_fine_map);
  init_volume(had_map);

/*
  if(m_mcLocation=="") {
    log << MSG::INFO <<  "========================= Atlfast::GlobalEventData::Instance() =========================" <<endreq;
    Atlfast::GlobalEventData* ged = Atlfast::GlobalEventData::Instance();
    m_mcLocation       = ged -> mcLocation();
  }  
*/  
  ATH_MSG_INFO("McCollection="<< m_mcLocation);

  //m_gentesIO = new GenAccessIO();


  // get the CoolHistSvc
  if (service("CoolHistSvc",p_coolhistsvc).isFailure()) {
    ATH_MSG_ERROR("Could not get CoolHistSvc");
    return StatusCode::FAILURE;
  }
  
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
  msg(MSG::INFO)<<endreq;

  if(m_simul_ID_only) {
    msg(MSG::INFO) <<"Reject ID truth particle vertices outside cylinder(s): ";
    int nc=TMath::Min(m_ID_cylinder_r.size(),m_ID_cylinder_z.size());
    for(int ic=0;ic<nc;++ic) {
      msg(MSG::INFO)<<"[r="<<m_ID_cylinder_r[ic]<<",z="<<m_ID_cylinder_z[ic]<<"] ";
    }
    msg(MSG::INFO)<<endreq;
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
    int cool_channel=atoi(str_cool_channel.c_str());
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
      //log << MSG::INFO <<  "infile non zero" <<endreq;
      //infile->ls();
      //log << MSG::INFO <<  "open="<<infile->IsOpen()<<endreq;
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
  msg(MSG::INFO) << endreq;
  // check all the keys, if we find the histogram folder, update the pointer
  
  MSG::Level level=MSG::INFO;
  int nprint=0;

  for (std::list<std::string>::const_iterator itr=keys.begin();itr!=keys.end();++itr) {
    for(unsigned int icool=0;icool<m_DB_folder.size();++icool) {
      if (*itr==m_DB_folder[icool]) {
        TObject* odir;
        if (p_coolhistsvc->getTObject(m_DB_folder[icool],m_DB_channel[icool],m_DB_dirname[icool],odir).isSuccess()) {
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

ParticleEnergyParametrization* FastShowerCellBuilderTool::findElower(int id,double E,double eta)
{
  t_map_PEP_ID::iterator iter_id=m_map_ParticleEnergyParametrizationMap.find(id);
  if(iter_id!=m_map_ParticleEnergyParametrizationMap.end()) {
    ATH_MSG_DEBUG("ID found="<<iter_id->first);
    t_map_PEP_Energy::iterator iter_E=iter_id->second.lower_bound(E);
    if(iter_E==iter_id->second.end()) iter_E--;
    if(iter_E!=iter_id->second.end()) {
      if(iter_E->first>=E && iter_E!=iter_id->second.begin()) iter_E--;
      ATH_MSG_DEBUG("E found="<<iter_E->first);
// first para_eta > fabs_eta  !! might be wrong !! 
      double aeta=fabs(eta);
      t_map_PEP_Eta::iterator iter_eta=iter_E->second.lower_bound(aeta);

      if(iter_eta!=iter_E->second.begin())  iter_eta--;
      if(m_energy_eta_selection){
        ATH_MSG_DEBUG(" new eta selection for energy paramertization is used ");
        if(iter_eta==iter_E->second.end()) iter_eta--;
        if(iter_eta!=iter_E->second.end()) {
          
          t_map_PEP_Eta::iterator best(iter_eta);
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

ParticleEnergyParametrization* FastShowerCellBuilderTool::findEupper(int id,double E,double eta)
{
  t_map_PEP_ID::iterator iter_id=m_map_ParticleEnergyParametrizationMap.find(id);
  if(iter_id!=m_map_ParticleEnergyParametrizationMap.end()) {
    ATH_MSG_DEBUG("ID found="<<iter_id->first);
    t_map_PEP_Energy::iterator iter_E=iter_id->second.lower_bound(E);
    if(iter_E==iter_id->second.end()) iter_E--;
    if(iter_E!=iter_id->second.end()) {
      ATH_MSG_DEBUG("E found="<<iter_E->first);
      double aeta=fabs(eta);
      t_map_PEP_Eta::iterator iter_eta=iter_E->second.lower_bound(aeta);
      if(iter_eta!=iter_E->second.begin())  iter_eta--;
      if(m_energy_eta_selection){
        if(iter_eta==iter_E->second.end()) iter_eta--;
        if(iter_eta!=iter_E->second.end()) {

          t_map_PEP_Eta::iterator best(iter_eta);
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


TShape_Result* FastShowerCellBuilderTool::findShape (int id,int calosample,double E,double eta,double dist,double distrange)
{
  t_map_PSP_ID::iterator iter_id=m_map_ParticleShapeParametrizationMap.find(id);
  if(iter_id!=m_map_ParticleShapeParametrizationMap.end()) {
    ATH_MSG_DEBUG("ID found="<<iter_id->first);
    
    t_map_PSP_calosample::iterator iter_cs=iter_id->second.find(calosample);
    if(iter_cs!=iter_id->second.end()) {
      ATH_MSG_DEBUG("calosample found="<<iter_cs->first);
    
      t_map_PSP_Energy::iterator iter_E=iter_cs->second.lower_bound(E);
      if(iter_E==iter_cs->second.end()) iter_E--;
      double edist=fabs(iter_E->first - E);
      if(iter_E!=iter_cs->second.begin()) {
        t_map_PSP_Energy::iterator iter_Etest=iter_E;
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
        
        for(t_map_PSP_DistEta::iterator iter_disteta=iter_E->second.begin();iter_disteta<iter_E->second.end();++iter_disteta) {
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

const Trk::TrackParameters* FastShowerCellBuilderTool::TrackSeenByCalo(ICaloSurfaceBuilder* m_calosurf,const Trk::TrackParameters* parm, const CaloCell_ID_FCS::CaloSample sample,const double offset, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local)
{
  // Take eta as best guess of track direction and create surface :
  double trketa = parm->eta();
  Trk::Surface* surf = m_calosurf->CreateUserSurface ((CaloCell_ID::CaloSample)sample,offset,trketa);
  if (!surf) return 0;

  const Trk::TrackParameters* param=0;

  if (!parm) {
    ATH_MSG_WARNING("extrapolate TrkParam called with null Trk::TrackParameters*");
    return 0;
  }

  ATH_MSG_DEBUG("Trying to propagate TrackParameters to Surface ... " << (*surf));
  param = m_extrapolator->extrapolate( *parm, *surf,
                                         Trk::alongMomentum,
                                       true, Trk::nonInteracting);
  if (param) {
    ATH_MSG_DEBUG("Propagation successful ");
  } else {
    ATH_MSG_DEBUG("Propagation failed, try custom surface ");

    Trk::Surface* newsurf=0;
    if(isCaloBarrel(sample)) {
      double r;
      if(m_calosurf==m_calosurf_entrance) r=rent(sample,trketa);
       else r=rmid(sample,trketa);
      newsurf=new Trk::CylinderSurface( r , 40000*CLHEP::mm );
    } else { 
      double z;
      if(m_calosurf==m_calosurf_entrance) z=zent(sample,trketa);
       else z=zmid(sample,trketa);
      Amg::Transform3D* shiftedZ=new Amg::Transform3D();
      *shiftedZ=Amg::Translation3D(Amg::Vector3D(0.,0.,z));
      newsurf=new Trk::DiscSurface( shiftedZ , 20*CLHEP::mm , 20000*CLHEP::mm );
    }

    if(newsurf) {
      delete surf;
      surf=newsurf;
      ATH_MSG_DEBUG("Trying to propagate TrackParameters to Surface ... " << (*surf));
      param = m_extrapolator->extrapolate( *parm, *surf,
                                         Trk::alongMomentum,
                                       false, Trk::nonInteracting);
      if (param) {
        ATH_MSG_DEBUG("2nd Propagation successful ");
      }else {
        ATH_MSG_DEBUG("2nd Propagation failed");
      }  
    }
  }  
  
  CaloLocalPoint (param,pt_ctb, pt_local);
  
  delete surf;

  return param;
} 

const Trk::TrackParameters* FastShowerCellBuilderTool::get_calo_surface(const Trk::Perigee& candidatePerigee,const double charge)
{
  const Trk::TrackParameters* params_on_surface=0;

  sample_calo_surf=CaloCell_ID_FCS::MaxSample;
  eta_calo_surf=-999;
  phi_calo_surf=-999;
  d_calo_surf=0;
  double min_calo_surf_dist=1000;
  for(int i=0;i<m_n_surfacelist;++i) {
    CaloCell_ID_FCS::CaloSample sample=m_surfacelist[i];
    double offset = 0.;

    Amg::Vector3D pt_calo_ctb;
    Amg::Vector3D pt_calo_local;

    const Trk::TrackParameters* result=TrackSeenByCalo(m_calosurf_entrance, &candidatePerigee, (CaloCell_ID_FCS::CaloSample)sample, offset, &pt_calo_ctb, &pt_calo_local);

    double etaCalo = pt_calo_local.eta();

    msg(MSG::DEBUG)<<"entrance to calo surface : sample="<<sample<<" eta="<<etaCalo;

    if (result && fabs(etaCalo)<900) {
      double phiCalo = pt_calo_local.phi();
      double distsamp =deta(sample,etaCalo);

      msg(MSG::DEBUG)<<" phi="<<phiCalo<<" dist="<<distsamp;
      if(distsamp<min_calo_surf_dist) {
        if(params_on_surface) delete params_on_surface;
        params_on_surface=result;
        eta_calo_surf=etaCalo;
        phi_calo_surf=phiCalo;
        min_calo_surf_dist=distsamp;
        sample_calo_surf=sample;
        d_calo_surf=rzent(sample,etaCalo);
//          d_calo_surf=m_calodepthEntrance->radius(sample,etaCalo,phiCalo);

        msg(MSG::DEBUG)<<" r/z="<<d_calo_surf;
//          log<<" _r="<<rent(sample,etaCalo);
//          log<<" _z="<<zent(sample,etaCalo);

        if(isCaloBarrel(sample)) d_calo_surf*=cosh(etaCalo);
         else                    d_calo_surf= fabs(d_calo_surf/tanh(etaCalo));

        msg(MSG::DEBUG)<<" d="<<d_calo_surf;

        if(distsamp<0) {
          msg(MSG::DEBUG)<<endreq;
          break;
        }  
      }
      /*
      if ( fabs(charge)>0 ) m_etoCaloEntrance->TrackSeenByCalo( &candidatePerigee, sample, offset, &pt_calo_ctb, &pt_calo_local);
       else m_gtoCaloEntrance->TrackSeenByCalo( &candidatePerigee, sample, offset, &pt_calo_ctb, &pt_calo_local);
      double etaCalo_old = pt_calo_local.eta();
      double phiCalo_old = pt_calo_local.phi();
      if(fabs(etaCalo-etaCalo_old)>0.00001 || fabs(phiCalo-phiCalo_old)>0.00001) {
        log << MSG::WARNING <<"  TTC check sample "<<sample<<" eta=" << etaCalo     << "   phi=" << phiCalo     <<endreq;
        log << MSG::WARNING <<"  TTC custom1      "<<sample<<" eta=" << etaCalo_old << "   phi=" << phiCalo_old <<endreq;
      }
      */
      
    }
    if(result!=params_on_surface && result) delete result;
    msg(MSG::DEBUG)<<endreq;
  }
  
  if(sample_calo_surf==CaloCell_ID_FCS::MaxSample) {
    double maxradius=0;
    if(charge!=0) {                  
      const double const_c=299792458.0;   // Speed of light in m/s
      const double const_bField=2;
      const double const_mm=0.001;        // Unitconversion m -> CLHEP::mm
      const double const_MeV=1000000.0;   // Unitconversion CLHEP::MeV -> CLHEP::eV, pt is assumed in MeV/c, r is given in mm
      double radius = ptruth_pt * const_MeV / ( const_c * const_bField * std::abs(charge) ) / const_mm; 
      maxradius=2*radius; //max detector radius is 2*helix radius
    } else {
      const double const_z_endcap=4700.0; // distance of endcap in mm
      double tanheta=tanh(ptruth_eta);
      maxradius=const_z_endcap*sqrt(1.0/tanheta/tanheta - 1.0);
    }
    if(maxradius>100 && ptruth_pt>400 && fabs(ptruth_eta)<4.5) {
      msg(MSG::WARNING);
    } else {
      msg(MSG::DEBUG);
    }  
    msg()<<"entrance to calo surface not found... " 
         <<" initial par id="<<pdgid<<" e="<<ptruth_e
         <<" pt="<<ptruth_pt
         <<" eta="<<ptruth_eta<<" phi="<<ptruth_phi<<" charge="<<charge
         <<" vx="<<candidatePerigee.position().x()<<" vy="<<candidatePerigee.position().y()<<" vz="<<candidatePerigee.position().z()
         <<" : maxradius="<<maxradius<<" at FCAL/of helix. Skipping particle..."
         <<endreq;
    return 0;   
  } else {
    ATH_MSG_DEBUG("entrance to calo surface : sample="<<sample_calo_surf<<" eta="<<eta_calo_surf<<" phi="<<phi_calo_surf<<" deta="<<min_calo_surf_dist<<" dsurf="<<d_calo_surf);
  }
  
  return params_on_surface;
}

bool FastShowerCellBuilderTool::get_calo_etaphi(const Trk::TrackParameters* params_on_surface,CaloCell_ID_FCS::CaloSample sample){
  double distsamp =deta(sample,eta_calo_surf);
 //        double rzmiddle =m_calodepth->radius((CaloCell_ID::CaloSample)sample,ptruth_eta,ptruth_phi);
  double rzmiddle =rzmid(sample,eta_calo_surf);
  ATH_MSG_DEBUG("in : dist="<<distsamp<<" r/z(middle)="<<rzmiddle<<" =====");

  double offset = 0.;

  Amg::Vector3D pt_calo_ctb;
  Amg::Vector3D pt_calo_local;

  const Trk::TrackParameters* result=TrackSeenByCalo(m_calosurf_middle, params_on_surface, sample, offset, &pt_calo_ctb, &pt_calo_local);

  if (result) {
    delete result;
    result=0;
    double etaCalo = pt_calo_local.eta();
    double phiCalo = pt_calo_local.phi();
    letaCalo[sample]=etaCalo;
    lphiCalo[sample]=phiCalo;
    if(fabs(etaCalo)<900) {
      layerCaloOK[sample]=true;
 //            rzmiddle=m_calodepth->radius((CaloCell_ID::CaloSample)sample,etaCalo,phiCalo);
      rzmiddle=rzmid((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
      distsamp=deta((CaloCell_ID_FCS::CaloSample)sample,etaCalo);
    } else {
      letaCalo[sample]=eta_calo_surf;
      lphiCalo[sample]=phi_calo_surf;
    }

 //          log<< MSG::DEBUG <<" - sample "<<sample<<" eta=" << etaCalo<<" r/z="<<rzmiddle;
 //          log<<" _r="<<rmid((CaloCell_ID::CaloSample)sample,etaCalo);
 //          log<<" _z="<<zmid((CaloCell_ID::CaloSample)sample,etaCalo);

    /*
    if(fabs(etaCalo)<900) {
      if ( fabs(charge)>0 ) m_etoCalo->TrackSeenByCalo( &candidatePerigee, (CaloCell_ID::CaloSample)sample, offset, &pt_calo_ctb, &pt_calo_local);
       else m_gtoCalo->TrackSeenByCalo( &candidatePerigee, (CaloCell_ID::CaloSample)sample, offset, &pt_calo_ctb, &pt_calo_local);
      etaCalo = pt_calo_local.eta();
      phiCalo = pt_calo_local.phi();
      if(fabs(letaCalo[sample]-etaCalo)>0.00001 || fabs(lphiCalo[sample]-phiCalo)>0.00001) {
        log << MSG::WARNING <<"  TTC check sample "<<sample<<" eta=" << letaCalo[sample] << "   phi=" << lphiCalo[sample]<<endreq;
        log << MSG::WARNING <<"  TTC custom1      "<<sample<<" eta=" << etaCalo << "   phi=" << phiCalo<<endreq;
      }
    }
    */

  } else {
    letaCalo[sample]=eta_calo_surf;
    lphiCalo[sample]=phi_calo_surf;
  }

  if(isCaloBarrel((CaloCell_ID_FCS::CaloSample)sample)) rzmiddle*=cosh(letaCalo[sample]);
   else                                             rzmiddle= fabs(rzmiddle/tanh(letaCalo[sample]));

  dCalo[sample]=rzmiddle;
  distetaCaloBorder[sample]=distsamp;

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG)<<"  Final par TTC sample "<<(int)sample;
    if(layerCaloOK[sample]) msg()<<" (good)";
     else msg()<<" (bad)";
    msg()<<" eta=" << letaCalo[sample] << "   phi=" << lphiCalo[sample] <<" dCalo="<<dCalo[sample]<< endreq;
    //        log << MSG::DEBUG <<"Final par HEL     eta "           << peta    << "   phi " << pphi    <<" deta="<<etaCalo-peta<<" dphi="<<phiCalo-pphi<< endreq;
  } 
 
  if(result) {
    delete result;
    result=0;
  }
  
  return layerCaloOK[sample];
}


StatusCode FastShowerCellBuilderTool::process_particle(CaloCellContainer* theCellContainer,const HepMC::GenParticle* part,BarcodeEnergyDepositMap* MuonEnergyMap) 
{
  // Setup of the FastShowerInfo debug containers if requested
  FastShowerInfo *fastshowerinfo(0);
  if( m_storeFastShowerInfo )
  {
    ATH_MSG_DEBUG("Creating FastShowerInfoObject: ");
    fastshowerinfo = new FastShowerInfo();
    fastshowerinfo->Initialize( CaloCell_ID_FCS::MaxSample );
  }
  
  ///////////////////////////
  // Init of basic quantities
  ///////////////////////////

  // Ugly code, ptruth_XYZ are member variables
  ptruth_eta=part->momentum().eta();
  if( fabs(ptruth_eta)>6.0 )
  { 
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::SUCCESS;
  }

  ptruth_phi=part->momentum().phi();
  ptruth_e =part->momentum().e();
  TVector3 truth_direction;  // added (25.5.2009)
  truth_direction.SetPtEtaPhi(1.,ptruth_eta,ptruth_phi);
  TVector3 z_direction;// added (25.5.2009)
  z_direction.SetXYZ(0,0,1);// added (25.5.2009)  // added (25.5.2009)
  double ang_beta = z_direction.Angle(truth_direction);// added (25.5.2009)  
  // Definition of et2 and et according to CLHEP::HepLorentzVector
  //double pt2 = part->momentum().perp2();
  //double et2 = pt2 == 0 ? 0 : ptruth_e*ptruth_e * pt2/(pt2+part->momentum().z()*part->momentum().z());
  //ptruth_et = ptruth_e < 0.0 ? 0.0 : sqrt(et2);
  ptruth_pt=part->momentum().perp();
  ptruth_p=part->momentum().rho();
  pdgid=part->pdg_id();

  // By default assume its a charged hadron==pion
  refid=211;
  double refmass=139.57018; //PDG charged pion mass
  double partmass=part->momentum().m();
  t_map_PEP_ID::iterator iter_id=m_map_ParticleEnergyParametrizationMap.find(11); // Test if a dedicated electron parametrization exists
  if(iter_id!=m_map_ParticleEnergyParametrizationMap.end()) {                     // electron parametrization exists
    if(pdgid==22 || pdgid==111) {
      refid=22;
      refmass=0;
    }  
    if( pdgid==11 || pdgid==-11 ) { 
      refid=11;
      refmass=0.511;
      //log<<MSG::VERBOSE<<" electron parametrization found " << endreq;
    }
  } else {                                                                        // electron parametrization does not exist
     if(pdgid==22 || pdgid==11 || pdgid==-11 || pdgid==111) {
       refid=22;
       refmass=0;
       //if(pdgid==11 || pdgid==-11) log<<MSG::VERBOSE<<" no electron parametrization found: USE PHOTON " << endreq;
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
  
  if(refid==13) {
    if(!MuonEnergyMap) // If the Fatras -> FastCaloSim muon info is not present, don't simulate muons
    {
      if(m_storeFastShowerInfo) delete fastshowerinfo;
      return StatusCode::SUCCESS;
    }
  }  
  
  if(Ein<10) {
    // don't simulate particles below 10MeV
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::SUCCESS;
  }

  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
    layerCaloOK[i]=false;
    distetaCaloBorder[i]=1000;
    dCalo[i]=0;
    letaCalo[i]=lphiCalo[i]=-999;
  }  

  if( m_storeFastShowerInfo )
  {
    // storing the particle information inside the FastShowerInfo object
    fastshowerinfo->SetPtEtaPhiE( ptruth_pt, ptruth_eta, ptruth_phi, ptruth_e );
    fastshowerinfo->SetBarcodeAndPDGId( part->barcode(), pdgid );
  }
        
  // Get charge of particle
  double charge = 0;
  HepPDT::ParticleData* ap = m_particleDataTable->particle( abs( pdgid ) );

  if(!ap){ 
    if(EinT>1000) msg(MSG::WARNING);
     else msg(MSG::DEBUG);
    msg()<<"particle info for pdgid="<<pdgid<<" not found:"<<" e="<<ptruth_e<<" pt="<<ptruth_pt<<" m="<<part->momentum().m()<<" eta="<<ptruth_eta<<", skip particle..."<<endreq;
    
    if(m_storeFastShowerInfo) delete fastshowerinfo;
                    
    return StatusCode::SUCCESS;
  } else {
    charge = ap->charge();
    if(pdgid<0) charge = -charge;
  }

  // Setup truth to calo extrapolation info
  HepMC::GenVertex* pvtx = part->production_vertex();
  Amg::Vector3D pos(pvtx->position().x(),pvtx->position().y(),pvtx->position().z());
  Amg::Vector3D mom(part->momentum().px(),part->momentum().py(),part->momentum().pz());
  Trk::Perigee candidatePerigee(pos,mom,charge,pos);
  
  std::stringstream particle_info_str;
  particle_info_str<<"id="<<pdgid<<" rid="<<refid<<" e="<<ptruth_e<<" Ein="<<Ein<<" pt="<<ptruth_pt<<" p="<<ptruth_p<<" m="<<part->momentum().m()<<" eta="<<ptruth_eta<<" phi="<<ptruth_phi<<"  charge="<<charge;
  
  if(msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("====================================================");
    ATH_MSG_DEBUG("initial "<<particle_info_str.str());
    ATH_MSG_DEBUG("============= Getting Calo Surface =================");
  }  
  
  //////////////////////////////////////
  // Start calo extrapolation
  // First: get entry point into first calo sample
  //////////////////////////////////////
  const Trk::TrackParameters* params_on_surface=get_calo_surface(candidatePerigee,charge);
  if(!params_on_surface) {
    if(m_storeFastShowerInfo) delete fastshowerinfo;
    return StatusCode::FAILURE;
  }
  
  TVector3 surface;
  surface.SetPtEtaPhi(1,eta_calo_surf,phi_calo_surf);
  surface.SetMag(d_calo_surf);

  if(m_storeFastShowerInfo) fastshowerinfo->SetCaloSurface(eta_calo_surf, phi_calo_surf, d_calo_surf);

  // only continue if inside the calo
  if( fabs(eta_calo_surf)<6 ) {
    double distrange=0;
    double Epara_E = Ein; // Basic input energy to be put into the calo without response, used for parametrization lookup

    ParticleEnergyShape p;
    
    //////////////////////////////
    // Process Muon info from Fatras
    //////////////////////////////
    if(refid==13) {
      std::pair<BarcodeEnergyDepositMap::iterator,BarcodeEnergyDepositMap::iterator> range=MuonEnergyMap->equal_range(part->barcode());
      if(range.first==range.second) {
        if(m_storeFastShowerInfo) delete fastshowerinfo;
        return StatusCode::SUCCESS;
      }
      p.fcal_tot=0;
      for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
        p.E_layer[i]=0;
        p.fcal_layer[i]=0;
        letaCalo[i]=eta_calo_surf;
        lphiCalo[i]=phi_calo_surf;
        dCalo[i]=0;
      }
      for(BarcodeEnergyDepositMap::iterator i=range.first;i!=range.second;++i) {
/*
        log << MSG::INFO <<"MUON: id="<<part->pdg_id()<<" stat="<<part->status()<<" bc="<<part->barcode()<<" pt="<<part->momentum().perp()<<" eta="<<part->momentum().eta()<<" phi="<<part->momentum().phi();
        log << " : layer="<<i->second.sample<<" eta="<<i->second.position.eta()<<" phi="<<i->second.position.phi()<<" d="<<i->second.position.rho();
        if(i->second.materialEffects) if(i->second.materialEffects->energyLoss()) log <<" de="<<i->second.materialEffects->energyLoss()->deltaE();
        log << endreq;
*/        
        int sample=i->second.sample;
        double E=0;
        if(i->second.materialEffects) if(i->second.materialEffects->energyLoss()) E=-i->second.materialEffects->energyLoss()->deltaE()/Ein;
        p.E_layer[sample]=E;
        p.E+=p.E_layer[sample];
        letaCalo[sample]=i->second.position.eta();
        lphiCalo[sample]=i->second.position.phi();
        dCalo[sample]=i->second.position.mag();
        p.dist000+=p.E_layer[sample]*dCalo[sample];
      }
      if(p.E<=0) {
        ATH_MSG_DEBUG("particle energy<=0 ");
        ATH_MSG_DEBUG(" - "<<particle_info_str.str()<<" : skip particle...");
        if(m_storeFastShowerInfo) delete fastshowerinfo;
        return StatusCode::FAILURE;
      }
      p.Ecal=p.E;
      p.dist000/=p.E;
      p.dist_in=p.dist000;
    } else {  
    //////////////////////////////
    // Process all non muon particles
    //////////////////////////////
      ParticleEnergyParametrization* Elower=findElower(refid , Ein , eta_calo_surf);
      ParticleEnergyParametrization* Eupper=findEupper(refid , Ein , eta_calo_surf);
      ParticleEnergyParametrization* Epara=0;
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
        Epara->DiceParticle(p,*m_rndm);

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

        // now try to extrpolate to all calo layers, that contain energy
        for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
          if(p.E_layer[sample]<0) p.E_layer[sample]=0;
          if(p.E_layer[sample]>0) {
            ATH_MSG_DEBUG("============= Getting Calo position for sample "<<sample<<" E/Etot="<<p.E_layer[sample]<<" ; in : eta= " << ptruth_eta);
            if( get_calo_etaphi(params_on_surface,(CaloCell_ID_FCS::CaloSample)sample) ) {
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
      
    // If there is energy in the calo
    if(p.E>0) {
     //////////////////////////////////////
     // Main loop over all calorimeter layers
     //////////////////////////////////////
     for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) if(!isnan(p.E_layer[sample])) {
        // Now scale relative energy response to particle energy
        p.E_layer[sample]*=Ein*m_sampling_energy_reweighting[sample];
        ATH_MSG_DEBUG("============= E"<<sample<<"="<<p.E_layer[sample]<<" =============");
        
        double E_orig=p.E_layer[sample];
        double Et_orig=E_orig/cosh(eta_calo_surf);
     // Add energy to lost energy couner, if all deposit goes OK, subtract it again...
        E_lost_sample[sample]+=E_orig;
        Et_lost_sample[sample]+=Et_orig;
        
        if(p.E_layer[sample]>0 && (!isnan(p.E_layer[sample])) && fabs(letaCalo[sample])<5.0) {
          ATH_MSG_DEBUG("E"<<sample<<"="<<p.E_layer[sample]<<" d="<<dCalo[sample]);

          // Calculate estimates for cell energy fluctuations. Should be updated with better numbers
          double smaple_err=0.1;
          if(sample>=CaloCell_ID_FCS::PreSamplerB && sample<=CaloCell_ID_FCS::EME3    ) smaple_err=0.1; //LAr      10%/sqrt(E)
          if(sample>=CaloCell_ID_FCS::HEC0        && sample<=CaloCell_ID_FCS::TileExt2) smaple_err=0.5; //hadronic 50%/sqrt(E) ???
          if(sample>=CaloCell_ID_FCS::FCAL0       && sample<=CaloCell_ID_FCS::FCAL2   ) smaple_err=1.0; //FCAL    100%/sqrt(E) ???
          
          // Find parametrization for the lateral shape distribution in the sample
          TShape_Result* shape;
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
              msg()<<"]"<<endreq;
            }
          } else {
            if( !(sample>=CaloCell_ID_FCS::FCAL0 && sample<=CaloCell_ID_FCS::FCAL2) ) {
              MSG::Level level=MSG::WARNING;
              if(refid==13) level=MSG::DEBUG;
              if(msgLvl(level)) {
                ATH_MSG_LVL(level,"no shape found calosample="<<sample<<" Elayer="<<E_orig);
                ATH_MSG_LVL(level," - "<<particle_info_str.str());
                ATH_MSG_LVL(level," - Ecal/Ein="<<p.Ecal<<" E/Ein="<<p.E<<" E="<<p.E*Ein
                                <<" din="<<p.dist_in<<" dist000="<<p.dist000<<" drec="<<p.dist_rec
                                <<" surface: d="<<surface.Mag()<<" r="<<surface.Perp()<<" z="<<surface.z());
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

                /* Causing to big steps!!!
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
                  msg(MSG::DEBUG)<<" new deta="<<distetaCaloBorder[sample]<<endreq;
                }  
                
                if(distetaCaloBorder[sample]>olddeta) {
                  ATH_MSG_WARNING("repositioned cell impact, but deta increased!!!! stay with the old...");
                  distetaCaloBorder[sample]=olddeta;
                  letaCalo[sample]=oldletaCalo;
                  fcx=oldfcx;
                }  
              } else {
                msg(MSG::DEBUG)<<endreq;
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
          int iphi=celllist_maps[sample].phi_to_index(lookup_lphiCalo);
          int ieta=celllist_maps[sample].eta_to_index(lookup_letaCalo);
          cellinfo_map::cellinfo_vec& vec=celllist_maps[sample].vec(ieta,iphi);
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
                            <<" din="<<p.dist_in<<" dist000="<<p.dist000<<" drec="<<p.dist_rec
                            <<" surface: d="<<surface.Mag()<<" r="<<surface.Perp()<<" z="<<surface.z());
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

//              if(icell<20) log << MSG::DEBUG <<"  ("<<icell<<") ceta="<<cell->eta()<<" cphi="<<cell->phi()<<" E="<<Ecell<<" Einwide="<<Einwide<<endreq;
              if(Einwide<=0) {
                E_theDDE[icell]=0;
                continue;
              } 
              
              if(shape->HasCellFactor() && cell_hit) {
                float disteta=                    ( cell->eta() - cell_hit->eta() )/cell_hit->deta();
                float distphi=TVector2::Phi_mpi_pi( cell->phi() - cell_hit->phi() )/cell_hit->dphi();
                long int idisteta=lroundf(disteta);
                long int idistphi=lroundf(distphi);
                //log << MSG::DEBUG <<"  ("<<icell<<") ceta="<<cell->eta()<<" cphi="<<cell->phi()<<" E="<<Ecell<<" Einwide="<<Einwide<<" deta="<<disteta<<"="<<idisteta<<" dphi="<<distphi<<"="<<idistphi<<endreq;
                
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
              msg(level)<< "calosample "<<sample<<" : no energy dep around truth impact ("<<letaCalo[sample]<<"/"<<fcx<<"/"<<lookup_letaCalo<<";"<<lphiCalo[sample]<<"/"<<fcy<<"/"<<lookup_lphiCalo<<"), E("<<sample<<")="<<E_orig<<", Et("<<sample<<")="<<Et_orig<<endreq;
              msg(level)<< " - "<<particle_info_str.str()<<endreq;
              msg(level)<< " - ";
              if(shape) msg() << level << "parametrization  : "<< shape->GetTitle()<<", ";
              msg() << level << "skip sample..."<<endreq;
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
              log <<endreq;
            }
*/

            double rndfactor=-1;
            while(rndfactor<=0) rndfactor=CLHEP::RandGaussZiggurat::shoot(m_randomEngine,1.0,smaple_err/sqrt(ecell/1000));
            ecell*=rndfactor;
  //          if(ecell<0) ecell=0;
  //          log<<" Esmear="<<ecell<<endreq;
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
  //            log << MSG::VERBOSE << "found calo cell : eta=" <<theCaloCell->caloDDE()->eta()<<" phi="<<theCaloCell->caloDDE()->phi()<<" overlap="<<iter->second<<"old e=" <<theCaloCell->energy()<< " ; new e=" <<theCaloCell->energy()+energy*iter->second<< endreq;
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
            E_tot_em+=elayertot3;
            Et_tot_em+=et_elayertot3;
  //          simul_map_energy[part->barcode()]+=elayertot3;
  //          simul_map_energyEM[part->barcode()]+=elayertot3;
          } else {
            E_tot_had+=elayertot3;
            Et_tot_had+=et_elayertot3;
  //          simul_map_energy[part->barcode()]+=elayertot3;
  //          simul_map_energyHAD[part->barcode()]+=elayertot3;
          }  
          E_tot_sample[sample]+=elayertot3;
          Et_tot_sample[sample]+=et_elayertot3;

          E_lost_sample[sample]-=elayertot3;
          Et_lost_sample[sample]-=et_elayertot3;

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
    }
    
    if(m_storeFastShowerInfo) p.CopyDebugInfo( fastshowerinfo ); // old version: fastshowerinfo->SetParticleEnergyShape( p );
  }
  
  if(params_on_surface) {
    delete params_on_surface;
    params_on_surface=0;
  }

  if(m_storeFastShowerInfo)
  {
    ATH_MSG_DEBUG("Adding FastShowerInfoObject to the container");
    m_FastShowerInfoContainer->push_back(fastshowerinfo);
  }
 
  return StatusCode::SUCCESS;
}


/*
void FastShowerCellBuilderTool::print_par(const HepMC::GenParticle* par,MsgStream& log,int level)
{
  log.width(2*level);
  log<<MSG::INFO<<"";
  log<<"-> id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode();
  log<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<" e="<<par->momentum().e();
  log<<" E="<<simul_sum_energy[par->barcode()]<<" E(EM)="<<simul_sum_energyEM[par->barcode()]<<" E(HAD)="<<simul_sum_energyHAD[par->barcode()]<<endreq;
  HepMC::GenVertex* outver=par->end_vertex();
  if(outver) {
    for(HepMC::GenVertex::particles_out_const_iterator pout=outver->particles_out_const_begin();pout!=outver->particles_out_const_end();++pout) {
      print_par(*pout,log,level+1);
    }
  }
}
*/
/*
void FastShowerCellBuilderTool::sum_par(const HepMC::GenParticle* par,MsgStream& log,std::vector<double>& sums,int level)
{
  sums.resize(3);
  sums[0]=simul_map_energy[par->barcode()];
  sums[1]=simul_map_energyEM[par->barcode()];
  sums[2]=simul_map_energyHAD[par->barcode()];
  
  HepMC::GenVertex* outver=par->end_vertex();
  if(outver) {
    for(HepMC::GenVertex::particles_out_const_iterator pout=outver->particles_out_const_begin();pout!=outver->particles_out_const_end();++pout) {
      std::vector<double> sumpar(3);
      sum_par(*pout,log,sumpar,level+1);
      sums[0]+=sumpar[0];
      sums[1]+=sumpar[1];
      sums[2]+=sumpar[2];
    }
  }

  simul_sum_energy[par->barcode()]=sums[0];
  simul_sum_energyEM[par->barcode()]=sums[1];
  simul_sum_energyHAD[par->barcode()]=sums[2];
}
*/

bool FastShowerCellBuilderTool::Is_ID_Vertex(HepMC::GenVertex* ver)
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

bool FastShowerCellBuilderTool::Is_EM_Vertex(HepMC::GenVertex* ver)
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

FastShowerCellBuilderTool::flag_simul_sate FastShowerCellBuilderTool::Is_below_v14_truth_cuts_Vertex(HepMC::GenVertex* ver)
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
      //log << MSG::DEBUG<<"ver: bc="<<ver->barcode()<<" : loop in MC_recursive_remove_in_particles, returning"<<endreq;
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
  log << MSG::VERBOSE <<"print_MC_info: begin"<< endreq;
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
      log<<endreq;
    }
  }        
  
  log << MSG::VERBOSE <<"print_MC_info: end"<< endreq;
}
*/

void FastShowerCellBuilderTool::MC_remove_out_of_ID(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles)
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

void FastShowerCellBuilderTool::MC_remove_out_of_EM(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles)
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

void FastShowerCellBuilderTool::MC_remove_below_v14_truth_cuts(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles)
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
  if(!m_is_init_shape_correction) {
    init_shape_correction();
    m_is_init_shape_correction=true;
  }
  
  ATH_MSG_DEBUG("Executing start calo size=" <<theCellContainer->size()<<" Event="<<m_nEvent);
  
  if(setupEvent().isFailure() ) {
   ATH_MSG_ERROR("setupEvent() failed");
   return StatusCode::FAILURE;
  }

  FastCaloSimIsGenSimulStable ifs;
  IsGenNonInteracting invisible;
  
  MCparticleCollection particles;
  MCparticleCollection Simulparticles;
  MCdo_simul_state     do_simul_state;

  const McEventCollection* mcCollptr;
  if ( evtStore()->retrieve(mcCollptr, m_mcLocation).isFailure() ) {
     ATH_MSG_ERROR("Could not retrieve McEventCollection");
     return StatusCode::FAILURE;
  }

  // initialize a pileup type helper object
  PileUpType pileupType( mcCollptr );
 
  ATH_MSG_DEBUG("Start getting particles");
 
  // pileupType.signal_particles(particles, isStable);
  pileupType.signal_particles(particles, ifs);
 
  //sc = m_gentesIO->getMC(particles, &ifs, m_mcLocation );
  //if ( sc.isFailure() ) {
  //  log << MSG::ERROR << "getMC from "<<m_mcLocation<<" failed "<< endreq;
  //  return StatusCode::FAILURE;
  //}

  BarcodeEnergyDepositMap* MuonEnergyMap=0;
  if(evtStore()->contains<BarcodeEnergyDepositMap>(m_MuonEnergyInCaloContainer)) {
    if ( evtStore()->retrieve(MuonEnergyMap,m_MuonEnergyInCaloContainer).isFailure() ) {
      ATH_MSG_WARNING("Could not get "<<m_MuonEnergyInCaloContainer<<" from SG ");
    } else {
      ATH_MSG_DEBUG("Got "<<m_MuonEnergyInCaloContainer<<" from SG : size="<<MuonEnergyMap->size());
    }
  } else {
    ATH_MSG_DEBUG("Could not find "<<m_MuonEnergyInCaloContainer<<" in SG ");
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
        //log << MSG::DEBUG <<"INVISIBLE by ARRAY: id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<endreq;
        break;
      }
      if(m_invisibles[i]==0) {
        if(invisible(par)) {
          do_simul_state[par->barcode()]=invisibleTruthHelper;
          //log << MSG::DEBUG <<"INVISIBLE by TruthHelper: id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<endreq;
          break;
        }
      }
    }

    if(abs(par->pdg_id())==13 && MuonEnergyMap) {
      std::pair<BarcodeEnergyDepositMap::iterator,BarcodeEnergyDepositMap::iterator> range=MuonEnergyMap->equal_range(par->barcode());
      if(range.first==range.second) {
        do_simul_state[par->barcode()]=0;
        ATH_MSG_DEBUG("#="<<indpar<<": id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()
                    <<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()
                    << " : no calo energy deposit");
      } else {
        if(msgLvl(MSG::DEBUG)) {
          for(BarcodeEnergyDepositMap::iterator i=range.first;i!=range.second;++i) {
            msg(MSG::DEBUG)<<"#="<<indpar<<": id="<<par->pdg_id()<<" stat="<<par->status()<<" bc="<<par->barcode()
                           <<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()
                           <<" : layer="<<i->second.sample<<" eta="<<i->second.position.eta()<<" phi="<<i->second.position.phi()
                           <<" d="<<i->second.position.mag();
            if(i->second.materialEffects) if(i->second.materialEffects->energyLoss()) msg()<<" de="<<i->second.materialEffects->energyLoss()->deltaE();
            msg()<< endreq;
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
      msg()<<endreq;
    }

    if(do_simul_state[par->barcode()]<=0) {
      continue;
    }
    
    ++indpar;
    
//    if(DoParticleSimul(par,log)) {
    {
//      log<<MSG::DEBUG<<"-> OK for simul"<<endreq;
      Simulparticles.push_back(par);
//      if(Is_Particle_from_pdgid(par,log,15,1)) log<<MSG::INFO<<""<<endreq;
      
//      deflectParticle(particlesAtCal,par);
    }  
  }        
  ATH_MSG_DEBUG("finished finding particles");
  
  if(m_storeFastShowerInfo)
  {
    ATH_MSG_DEBUG("Creating and registering the FastShowerInfoContainer with key " << m_FastShowerInfoContainerKey);
    m_FastShowerInfoContainer = new FastShowerInfoContainer();
    evtStore()->record( m_FastShowerInfoContainer, m_FastShowerInfoContainerKey);
//     log << MSG::DEBUG << m_storeGate->dump() << endreq;
  }
  
  MCparticleCollectionCIter fpart = Simulparticles.begin();
  MCparticleCollectionCIter lpart = Simulparticles.end();

  int stat_npar=0;
  int stat_npar_OK=0;
  int stat_npar_nOK=0;
  for(MCparticleCollectionCIter part=fpart;part<lpart;++part) {
    if(process_particle(theCellContainer,*part,MuonEnergyMap).isFailure()) ++stat_npar_nOK;
     else ++stat_npar_OK;
    ++stat_npar;
  }

/*
  MCparticleCollection all_particles;
  log <<MSG::DEBUG<<"Start getting all particles"<<endreq;
  sc = m_gentesIO->getMC(all_particles, m_mcLocation );

  if ( sc.isFailure() ) {
    log << MSG::ERROR << "getMC from "<<m_mcLocation<<" failed "<< endreq;
    return StatusCode::FAILURE;
  }
  
  log <<MSG::INFO<<"Got all particles n="<<all_particles.size()<<endreq;
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

  if(releaseEvent().isFailure() ) {
   ATH_MSG_ERROR("releaseEvent() failed");
   return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode FastShowerCellBuilderTool::setupEvent()
{
  m_rndmSvc->print(m_randomEngineName);
  unsigned int rseed=0;
  while(rseed==0) {
    rseed=(unsigned int)( CLHEP::RandFlat::shoot(m_randomEngine) * std::numeric_limits<unsigned int>::max() );
  }
  gRandom->SetSeed(rseed);
  m_rndm->SetSeed(rseed);
  
  //if(gRandom) log<<" seed(gRandom="<<gRandom->ClassName()<<")="<<gRandom->GetSeed();
  //if(m_rndm)  log<<" seed(m_rndm="<<m_rndm->ClassName()<<")="<<m_rndm->GetSeed();
  //log<< endreq;
  
  ++m_nEvent;

  E_tot_em=0;
  E_tot_had=0;
  Et_tot_em=0;
  Et_tot_had=0;
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) {
    E_tot_sample[i]=0;
    E_lost_sample[i]=0;
    Et_tot_sample[i]=0;
    Et_lost_sample[i]=0;
  }  

  simul_map_energy.clear();
  simul_map_energyEM.clear();
  simul_map_energyHAD.clear();

  return StatusCode::SUCCESS;
}

StatusCode FastShowerCellBuilderTool::releaseEvent()
{
  ATH_MSG_DEBUG("e="<<E_tot_em+E_tot_had<<" e(EM)="<<E_tot_em<<" e(HAD)="<<E_tot_had
                <<" ; et="<<Et_tot_em+Et_tot_had<<" et(EM)="<<Et_tot_em<<" et(HAD)="<<Et_tot_had);

  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
    if(fabs(Et_lost_sample[sample])<1E-6) Et_lost_sample[sample]=0;
    if(fabs(E_lost_sample[sample])<1E-6) E_lost_sample[sample]=0;
    if(Et_lost_sample[sample]>5000) {
      ATH_MSG_WARNING("  - sample "<<sample<<" : etlost="<<Et_lost_sample[sample]<<" elost="<<E_lost_sample[sample]);
    } else {
      ATH_MSG_DEBUG("  - sample "<<sample<<" : etlost="<<Et_lost_sample[sample]<<" elost="<<E_lost_sample[sample]);
    }  
  }  

  return StatusCode::SUCCESS;
}


/*
void FastShowerCellBuilderTool::deflectParticles(ITransportedParticleCollection &itpc)
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "FastShowerCellBuilderTool deflectParticles()" << endreq;

  // read MC particles from TES
  typedef std::vector<const HepMC::GenParticle*>  MCparticleCollection ;
  typedef std::vector<const HepMC::GenParticle*>::const_iterator  MCparticleCollectionCIter ;

  MCparticleCollection p;
  TesIoStat stat = m_tesIO->getMC( p, m_mcSelector ) ;
  std::string mess;
  mess = stat? "Retrieved MC from TES ":"Failed MC retrieve from TES ";
  log << MSG::DEBUG << mess << p.size()<<endreq;

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
