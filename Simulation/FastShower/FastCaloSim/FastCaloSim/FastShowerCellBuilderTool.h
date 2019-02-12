/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTSHOWER_CELLBUILDERTOOL_H
#define FASTSHOWER_CELLBUILDERTOOL_H
//
// CellBuilderTool.cxx
//     Building Cells objects from Atlfast
//
// Michael Duehrssen

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "FastCaloSim/BasicCellBuilderTool.h"
//#include "TruthHelper/GenAccessIO.h"
#include "FastSimulationEvent/GenParticleEnergyDepositMap.h"
#include "HepPDT/ParticleDataTable.hh"
#include "TrkExInterfaces/ITimedExtrapolator.h"
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"
#include "FastCaloSimAthenaPool/FastShowerInfoContainer.h"

/*
  #if FastCaloSim_project_release_v1 == 12
  #include "CLHEP/HepPDT/ParticleDataTable.hh"
  #else
  #include "HepPDT/ParticleDataTable.hh"
  #endif
*/
/*
  #include "AtlfastEvent/CollectionDefs.h"

  namespace HepMC_helper{
  class IMCselector;
  }

  namespace FastShower{
  class GridletForger;
  }

  namespace Atlfast {
  class TesIO;
  }
*/

namespace HepMC {
  class GenParticle;
  class GenVertex;
}

namespace Trk {
  class TrackingVolume;
}

class ICaloSurfaceHelper;
class ICaloCoordinateTool;
class CaloDepthTool;
class ParticleEnergyParametrization;
class TShape_Result;
class TDirectory;
class TRandom;

class ICoolHistSvc;
class FastShowerInfoContainer;
class TLateralShapeCorrectionBase;
class TRandom3;

#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/Perigee.h"

class FastShowerCellBuilderTool: public BasicCellBuilderTool
{
public:
  struct Stats {
    double m_E_tot_em = 0;
    double m_E_tot_had = 0;
    double m_Et_tot_em = 0;
    double m_Et_tot_had = 0;

    double m_E_tot_sample[CaloCell_ID_FCS::MaxSample] = {0};
    double m_E_lost_sample[CaloCell_ID_FCS::MaxSample] = {0};
    double m_Et_tot_sample[CaloCell_ID_FCS::MaxSample] = {0};
    double m_Et_lost_sample[CaloCell_ID_FCS::MaxSample] = {0};
  };

  FastShowerCellBuilderTool(
                            const std::string& type,
                            const std::string& name,
                            const IInterface* parent);
  ~FastShowerCellBuilderTool();


  virtual StatusCode initialize() override final;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override final;
  StatusCode setupEvent (const EventContext& ctx,
                         TRandom3& rndm) const;
  StatusCode releaseEvent (Stats& stats) const;
  // the actual simulation code for one particle can run standalone without process(CaloCellContainer* theCellContainer),
  // but setupEvent() should be called before the first particle and releaseEvent() after the last particle
  StatusCode process_particle(CaloCellContainer* theCellContainer, std::vector<Trk::HitInfo>* hitVector,
                              const Amg::Vector3D& initMom,
                              double mass, int pdgId, int barcode,
                              FastShowerInfoContainer* fastShowerInfoContainer,
                              TRandom3& rndm,
                              Stats& stats,
                              const EventContext& ctx) const;

  StatusCode callBack( IOVSVC_CALLBACK_ARGS );

  typedef std::map<int,int> MCdo_simul_state;
  typedef std::vector<const HepMC::GenParticle*> MCparticleCollection ;
  typedef MCparticleCollection::const_iterator MCparticleCollectionCIter ;

private:
  void LoadParametrizationsFromDir(std::string dir);
  void LoadParametrizationsFromFile(TDirectory& infile,MSG::Level level=MSG::INFO);
  StatusCode OpenParamSource(std::string insource);

  SG::ReadHandleKey<McEventCollection> m_mcCollectionKey
    {this, "McLocation", "TruthEvent"};
  std::string                    m_ParticleParametrizationFileName{""};
  std::vector< std::string >     m_AdditionalParticleParametrizationFileNames;

  std::vector< std::string >     m_DB_folder;
  std::vector< int >             m_DB_channel;
  std::vector< std::string >     m_DB_dirname;

  SG::ReadHandleKey<BarcodeEnergyDepositMap> m_MuonEnergyInCaloContainerKey
  { this, "MuonEnergyInCaloContainerName", "FatrasDepositedMuonEnergyInCalo"};
  bool                           m_simul_ID_only{true};
  bool                           m_simul_ID_v14_truth_cuts{false};
  bool                           m_simul_EM_geant_only{false};
  bool                           m_simul_heavy_ions{false};

  ServiceHandle<ICoolHistSvc>    m_coolhistsvc;
  //  const std::string              m_histfolder; // COOL folder to access

  //  HepMC_helper::IMCselector*     m_mcSelector; //selects input particles
  /*
    FastShower::LateralShape*      m_latshape;
    FastShower::LateralShape*      m_latshape_pion;
    FastShower::LongitudinalShape* m_longshape;
  */
  ServiceHandle<IPartPropSvc>    m_partPropSvc;
  ServiceHandle<IAthRNGSvc>      m_rndmSvc;
  ATHRNG::RNGWrapper*            m_randomEngine = nullptr;
  std::string                    m_randomEngineName{"FastCaloSimRnd"};         //!< Name of the random number stream

  //CaloDepthTool*                 m_calodepth;
  //CaloDepthTool*                 m_calodepthEntrance;

  /** The Extrapolator setup */
  ToolHandle<Trk::ITimedExtrapolator>   m_extrapolator; //public tool

  ToolHandle<ICaloSurfaceHelper>   m_caloSurfaceHelper; //public tool
  ToolHandle<ICaloCoordinateTool>  m_calo_tb_coord; //public tool

  bool                           m_jo_interpolate{false}; //ATA: make marjorie's iterpolation optional
  bool                           m_energy_eta_selection{false};//mwerner: make new selection of EnergyParam optional
  bool                           m_use_Ekin_for_depositions{false};//Use the kinetic energy of a particle to as measure of the energie to deposit in the calo

  std::vector< float >           m_sampling_energy_reweighting;

  void                           CaloLocalPoint (const Trk::TrackParameters* parm, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local);

  int                            m_n_surfacelist{5};
  CaloCell_ID_FCS::CaloSample    m_surfacelist[CaloCell_ID_FCS::MaxSample];

  HepPDT::ParticleDataTable*     m_particleDataTable{};

  std::vector< int >             m_invisibles;

  //  TGraphErrors*                  geometry[CaloCell_ID_FCS::MaxSample][3];

  ParticleEnergyParametrization* findElower(int id,double E,double eta) const;
  ParticleEnergyParametrization* findEupper(int id,double E,double eta) const;
  const TShape_Result* findShape (int id,int calosample,double E,double eta,double dist,double distrange) const;

  //void sum_par(const HepMC::GenParticle* par,MsgStream& log,std::vector<double>& sums,int level=0);
  //void print_par(const HepMC::GenParticle* par,MsgStream& log,int level=0);


public:
  enum flag_simul_sate {
    zero_state=0,
    out_of_ID=-1,
    non_EM_vertex=-2,
    heavy_ion=-3,
    pdg_id_unkown=-4,
    invisibleArray=-5,
    invisibleTruthHelper=-6,
    mother_particle=-7,
    v14_truth_brems=-8,
    v14_truth_conv=-9
  };

private:
  //std::vector< double > m_spline_reweight_x;
  //std::vector< double > m_spline_reweight_y;

  void init_shape_correction();
  typedef std::vector< TLateralShapeCorrectionBase* > t_shape_correction;
  t_shape_correction m_shape_correction;

  // extrapolation through Calo
  std::vector<Trk::HitInfo>* caloHits(const HepMC::GenParticle& part ) const;

  bool Is_ID_Vertex(HepMC::GenVertex* ver) const;
  std::vector< double >          m_ID_cylinder_r;
  std::vector< double >          m_ID_cylinder_z;
  bool Is_EM_Vertex(HepMC::GenVertex* ver) const;
  flag_simul_sate Is_below_v14_truth_cuts_Vertex(HepMC::GenVertex* ver) const;
  void MC_remove_out_of_ID(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles) const;
  void MC_remove_out_of_EM(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles) const;
  void MC_remove_below_v14_truth_cuts(MCdo_simul_state& do_simul_state,const MCparticleCollection& particles) const;

  //ID              Energy             Eta
  typedef std::map< double , ParticleEnergyParametrization* > t_map_PEP_Eta;
  typedef std::map< double , t_map_PEP_Eta > t_map_PEP_Energy;
  typedef std::map< int    , t_map_PEP_Energy > t_map_PEP_ID;
  t_map_PEP_ID m_map_ParticleEnergyParametrizationMap;

  //ID              Energy             Eta                Dist
  //  typedef std::map< double , TShape_Result* > t_map_PSP_Eta;
  typedef std::vector< TShape_Result* > t_map_PSP_DistEta;
  typedef std::map< double , t_map_PSP_DistEta > t_map_PSP_Energy;
  typedef std::map< int    , t_map_PSP_Energy > t_map_PSP_calosample;
  typedef std::map< int    , t_map_PSP_calosample > t_map_PSP_ID;
  t_map_PSP_ID m_map_ParticleShapeParametrizationMap;


public:
  bool get_calo_etaphi(std::vector<Trk::HitInfo>* hitVector,
                       CaloCell_ID_FCS::CaloSample sample,
                       double eta_calo_surf,
                       double phi_calo_surf,
                       bool layerCaloOK[CaloCell_ID_FCS::MaxSample],
                       double letaCalo[CaloCell_ID_FCS::MaxSample],
                       double lphiCalo[CaloCell_ID_FCS::MaxSample],
                       double dCalo[CaloCell_ID_FCS::MaxSample],
                       double distetaCaloBorder[CaloCell_ID_FCS::MaxSample]) const;

  bool get_calo_surface(std::vector<Trk::HitInfo>* hitVector,
                        double& eta_calo_surf,
                        double& phi_calo_surf,
                        double& d_calo_surf) const;

private:
  SG::WriteHandleKey<FastShowerInfoContainer>  m_FastShowerInfoContainerKey
  { this, "FastShowerInfoContainerKey", "FastShowerInfoContainer" };
  bool                     m_storeFastShowerInfo{false};
  Trk::PdgToParticleHypothesis        m_pdgToParticleHypothesis;
  mutable const Trk::TrackingVolume*  m_caloEntrance{};
  std::string                         m_caloEntranceName{"InDet::Containers::InnerDetector"};
};

#endif
