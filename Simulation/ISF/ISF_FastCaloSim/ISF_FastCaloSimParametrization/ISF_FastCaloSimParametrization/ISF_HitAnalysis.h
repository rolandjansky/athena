/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_HIT_ANALYSIS_H
#define ISF_HIT_ANALYSIS_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArElecCalib/ILArfSampl.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

//#####################################
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "FSmap.h"
#include "HepMC/GenParticle.h"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"
#include "FastCaloSim_CaloCell_ID.h"
#include "TrkParameters/TrackParameters.h"
//#####################################


#include <string>
#include <Rtypes.h>
//#include "TH1.h"

/* *************************************************************
 This is a modified copy of Simulation/Tools/CaloHitAnalysis
 Aug 27, 2013 Zdenek Hubacek (CERN)
************************************************************** */

class TileID;
class TileDetDescrManager;
class TTree;
class ITHistSvc;
class TileInfo;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class IGeoModelSvc;

//############################
class ICaloSurfaceBuilder;
class ICaloCoordinateTool;
class IExtrapolateToCaloTool;
class CaloDepthTool;
namespace Trk {
	  class IExtrapolator;
}

class ISF_HitAnalysis : public AthAlgorithm {

 public:

   ISF_HitAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
   ~ISF_HitAnalysis();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();
   virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);
   virtual StatusCode updateMetaData(IOVSVC_CALLBACK_ARGS);

   bool get_calo_etaphi(const Trk::TrackParameters* params_on_surface,CaloCell_ID_FCS::CaloSample sample,double);

 private:

   /** a handle on Store Gate for access to the Event Store */
   //StoreGateSvc* m_storeGate;
   //StoreGateSvc* m_detStore;

   const IGeoModelSvc *m_geoModel;
   const TileInfo *m_tileInfo;
   const LArEM_ID *m_larEmID;
   const LArFCAL_ID *m_larFcalID;
   const LArHEC_ID *m_larHecID;
   const TileID * m_tileID;
   const TileDetDescrManager * m_tileMgr;
   const DataHandle<ILArfSampl>   m_dd_fSampl;
      
   /** Simple variables by Ketevi */
   std::vector<float>* m_hit_x;
   std::vector<float>* m_hit_y;
   std::vector<float>* m_hit_z;
   std::vector<float>* m_hit_energy;
   std::vector<float>* m_hit_time;
   std::vector<Long64_t>* m_hit_identifier;
   std::vector<Long64_t>* m_hit_cellidentifier;
   std::vector<bool>*  m_islarbarrel;
   std::vector<bool>*  m_islarendcap;
   std::vector<bool>*  m_islarhec;
   std::vector<bool>*  m_islarfcal;
   std::vector<bool>*  m_istile;
   std::vector<int>*   m_hit_sampling;
   std::vector<float>* m_hit_samplingfraction;

   std::vector<float>* m_truth_energy;
   std::vector<float>* m_truth_px;
   std::vector<float>* m_truth_py;
   std::vector<float>* m_truth_pz;
   std::vector<int>*   m_truth_pdg;
   std::vector<int>*   m_truth_barcode;
   std::vector<int>*   m_truth_vtxbarcode; //production vertex barcode

   std::vector<Long64_t>* m_cell_identifier;
   std::vector<float>*       m_cell_energy;
   std::vector<int>*         m_cell_sampling;

   std::vector<float>*       m_g4hit_energy;
   std::vector<float>*       m_g4hit_time;
   std::vector<Long64_t>* m_g4hit_identifier;
   std::vector<Long64_t>* m_g4hit_cellidentifier;
   std::vector<float>*       m_g4hit_samplingfraction;
   std::vector<int>*         m_g4hit_sampling;
   //Ok, this won't work, ROOT won't let me save a custom object which it doesn't know about
   //std::vector<zh_matchedcell>* m_matched_cells;

   TTree * m_tree;
   std::string m_ntupleFileName; 
   std::string m_ntupleDirName; 
   std::string m_ntupleTreeName; 
   std::string m_metadataTreeName; 
   std::string m_geoFileName; 
   int m_NtruthParticles;
   ITHistSvc * m_thistSvc;
   const CaloDetDescrManager* m_calo_dd_man;

   //####################################################
   double eta_calo_surf;
   double phi_calo_surf;
   double d_calo_surf;
   double ptruth_eta;
   double ptruth_phi;
   double ptruth_e;
   double ptruth_et;
   double ptruth_pt;
   double ptruth_p;
   int pdgid;
   
   std::vector<std::vector<double> >* m_TTC_entrance_eta;
   std::vector<std::vector<double> >* m_TTC_entrance_phi;
   std::vector<std::vector<double> >* m_TTC_entrance_r;
   std::vector<std::vector<double> >* m_TTC_entrance_z;
   std::vector<std::vector<double> >* m_TTC_back_eta;
   std::vector<std::vector<double> >* m_TTC_back_phi;
   std::vector<std::vector<double> >* m_TTC_back_r;
   std::vector<std::vector<double> >* m_TTC_back_z;
   std::vector<double>* m_TTC_IDCaloBoundary_eta;
   std::vector<double>* m_TTC_IDCaloBoundary_phi;
   std::vector<double>* m_TTC_IDCaloBoundary_r;
   std::vector<double>* m_TTC_IDCaloBoundary_z;
   std::vector<double>* m_TTC_Angle3D;
   std::vector<double>* m_TTC_AngleEta;

   IExtrapolateToCaloTool*      m_etoCalo;
   IExtrapolateToCaloTool*      m_etoCaloEntrance;
   CaloDepthTool*               m_calodepth;
   CaloDepthTool*               m_calodepthEntrance;
   ICaloSurfaceBuilder*           m_calosurf;
   ICaloSurfaceBuilder*           m_calosurf_entrance;
   Trk::IExtrapolator*            m_extrapolator;
   ICaloCoordinateTool*           m_calo_tb_coord;
   std::string                    m_extrapolatorName;
   std::string                    m_extrapolatorInstanceName;
   std::string                    m_calosurf_InstanceName;
   std::string                    m_calosurf_entrance_InstanceName;
   
   CaloCell_ID_FCS::CaloSample sample_calo_surf;
   int                            m_n_surfacelist;
   CaloCell_ID_FCS::CaloSample    m_surfacelist[CaloCell_ID_FCS::MaxSample];
		
   double deta(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   double rzmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   void   minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta) const;
   double rzent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   double rmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   double rent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   double zmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   double zent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
   void   CaloLocalPoint (const Trk::TrackParameters* parm,Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local);
   
   const Trk::TrackParameters*    TrackSeenByCalo(ICaloSurfaceBuilder* m_calosurf,const Trk::TrackParameters* parm, const CaloCell_ID_FCS::CaloSample sample,const double offset, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local);
   const Trk::TrackParameters*    TrackSeenByCalo_ID(const Trk::TrackParameters* parm, Amg::Vector3D* pt_ctb, Amg::Vector3D* pt_local);
   //const Trk::TrackParameters*    TrackSeenByCalo_ID(const Trk::TrackParameters* parm, HepGeom::Point3D<double>* pt_ctb, HepGeom::Point3D<double>* pt_local);
   
   bool   layerCaloOK[CaloCell_ID_FCS::MaxSample];
   double letaCalo[CaloCell_ID_FCS::MaxSample];
   double lphiCalo[CaloCell_ID_FCS::MaxSample];
   double lrCalo[CaloCell_ID_FCS::MaxSample];
   double lzCalo[CaloCell_ID_FCS::MaxSample];
   double dCalo[CaloCell_ID_FCS::MaxSample];
   double distetaCaloBorder[CaloCell_ID_FCS::MaxSample];
   
   bool m_isCaloBarrel[CaloCell_ID_FCS::MaxSample];
   bool isCaloBarrel(CaloCell_ID_FCS::CaloSample sample) const {return m_isCaloBarrel[sample];};
   
   double  min_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
   double  max_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
   FSmap< double , double > rmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
   FSmap< double , double > zmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
   FSmap< double , double > rent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
   FSmap< double , double > zent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
   
   void extrapolate(const HepMC::GenParticle* part);
   void extrapolate_to_ID(const HepMC::GenParticle* part);
   
   HepPDT::ParticleDataTable*     m_particleDataTable;
   
   const Trk::TrackParameters* get_calo_surface(const Trk::Perigee& candidatePerigee,const double charge);
   const Trk::TrackParameters* get_calo_surface_ID(const Trk::Perigee& candidatePerigee,double& eta_ID,double& phi_ID,double& r_ID,double& x_ID,double& y_ID,double& z_ID);
   
   double m_CaloBoundaryR;
   double m_CaloBoundaryZ;
   double m_calomargin;
   
   //###################################################################
 	  

};

#endif // ISF_HIT_ANALYSIS_H

