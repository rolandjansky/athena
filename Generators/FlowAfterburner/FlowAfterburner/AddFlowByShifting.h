/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// File:  Generators/FlowAfterburnber/AddFlowByShifting.h
// Description:
//    This code is used to introduce particle flow 
//    to particles from generated events
//    It works by modifying phi angles of particles
//    according to requested flow type and magnitude
//
//    It takes from SG a container of tracks as input 
//    and registers in SG a new container with modified tracks on output 
//
//    It currently uses Hijing generator specific class HijingEventParams
//    with truth event parameters information
//
// AuthorList:
// Andrzej Olszewski: Initial Code February 2006

#ifndef ADDFLOWBYSHIFTING_H
#define ADDFLOWBYSHIFTING_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include <CLHEP/Random/RandomEngine.h>
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/Relatives.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>




class TGraph;
class StoreGateSvc;
class HijingEventParams;

class AddFlowByShifting:public AthAlgorithm {
public:
        AddFlowByShifting(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

  //functions used for root finding when using the "exact"(and not the "approximate")  method
  static double vn_func           (double x, void *params);
  static double vn_func_derivative(double x, void *params);//currently not used


private:
  double SetParentToRanPhi(HepMC::GenParticlePtr parent);
  double AddFlowToParent(HepMC::GenParticlePtr parent, 
			 const HijingEventParams *hijing_pars);
  void   MoveDescendantsToParent(HepMC::GenParticlePtr parent, double phishift);


  // flow functions to set the vn values
  void (AddFlowByShifting::*m_flow_function)     (double b, double eta, double pt);//function pointer which is set to one of the functions below
  void jjia_minbias_new       (double b, double eta, double pt);
  void jjia_minbias_new_v2only(double b, double eta, double pt);
  void fixed_vn               (double b, double eta, double pt);
  void fixed_v2               (double b, double eta, double pt);
  void jjia_minbias_old       (double b, double eta, double pt);
  void ao_test                (double b, double eta, double pt);
  void custom_vn              (double b, double eta, double pt);
  void p_Pb_cent_eta_indep    (double b, double eta, double pt); //for p_Pb

  TGraph *m_graph_fluc;//TGraph storing the v2_RP/delta Vs b_imp
  void Set_EbE_Fluctuation_Multipliers(HepMC::GenVertexPtr mainvtx, float b);



  StoreGateSvc*    m_sgSvc;
  IAtRndmGenSvc*   p_AtRndmGenSvc;
  CLHEP::HepRandomEngine* p_engine;

  // Setable Properties:-
  std::string m_inkey, m_outkey;

  int   m_ranphi_sw;

  std::string m_flow_function_name;
  std::string m_flow_implementation;
  int   m_flow_implementation_type;
  bool  m_flow_fluctuations;

  int   m_floweta_sw;
  float m_flow_maxeta, m_flow_mineta;

  int   m_flowpt_sw;
  float m_flow_maxpt , m_flow_minpt;

  int   m_flowb_sw;//currently not used 
  float m_custom_v1, m_custom_v2,m_custom_v3, m_custom_v4,m_custom_v5, m_custom_v6;

  int   m_particles_processed;

  //float psi_n[6],v1,v2,v3,v4,v5,v6;
  float m_psi_n[6],m_v_n[6];
  float m_EbE_Multiplier_vn[6];

};

#endif


