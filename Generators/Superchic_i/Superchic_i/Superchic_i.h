/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
//
// File:  Generators/Superchic_i.h
// Description:
//    This code is used to get a Starlight Monte Carlo event.
//    genInitialize() is used to read parameters
//    callGenerator() makes the event
//    genFinalize() writes log files etc
//    fillEvt(GeneratorEvent* evt) passes the event to HepMC
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
//
// AuthorList:
//         Prabhakar Palni, 29 August 2018

#ifndef GENERATORMODULESSUPERCHIC_H
#define GENERATORMODULESSUPERCHIC_H

#include "GeneratorModules/GenModule.h"
#include "Superchic_i/Atlas_HEPEVT_modified.h"

using std::string;
typedef std::vector<std::string> CommandVector;

class StoreGateSvc;
// ---------------------

class Superchic_i:public GenModule {
public:
  Superchic_i(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Superchic_i();

  virtual StatusCode	genInitialize	();
  virtual StatusCode 	callGenerator	();
  virtual StatusCode 	genFinalize	();
  virtual StatusCode 	fillEvt		(HepMC::GenEvent* evt);

protected:

  // event counter
  int m_events ;

  // inputs parameter (SuperChic initialization)

  std::string m_configFileName;

  std::string m_rts;
  int m_isurv;
  std::string m_intag;
  std::string m_PDFname;
  unsigned int m_PDFmember;

  //pdfinfo
  HepMC::PdfInfo m_pdfinfo;
  // inputs parameter (events generation)
  unsigned int m_proc;
  std::string m_beam;
  std::string m_outtg;
  std::string m_sfaci;
  std::string m_diff;
  std::string m_an;
  std::string m_az;
  std::string m_rz;
  std::string m_dz;
  std::string m_rn;
  std::string m_dn;
  std::string m_ionqcd;
  unsigned int m_ncall;
  unsigned int m_itmx;
  std::string m_prec;
  unsigned int m_ncall1;
  unsigned int m_inccall;
  unsigned int m_itend;
  int m_iseed;
  //int m_s2int;

  std::string m_genunw;
  int m_nev;
  std::string m_erec;
  std::string m_readwt;

  std::string m_wtmax;
  std::string m_ymin;
  std::string m_ymax;
  std::string m_mmin;
  std::string m_mmax;
  std::string m_gencuts;
  std::string m_scorr;
  std::string m_fwidth;
  std::string m_ptxmax;
  std::string m_acoabmax;
  std::string m_ptamin;
  std::string m_ptbmin;
  std::string m_ptcmin;
  std::string m_ptdmin;
  std::string m_ptemin;
  std::string m_ptfmin;

  std::string m_etaamin;
  std::string m_etabmin;
  std::string m_etacmin;
  std::string m_etadmin;
  std::string m_etaemin;
  std::string m_etafmin;
  std::string m_etaamax;
  std::string m_etabmax;
  std::string m_etacmax;
  std::string m_etadmax;
  std::string m_etaemax;
  std::string m_etafmax;

  std::string m_rjet;
  std::string m_jalg;

  std::string m_m2b;
  int m_pdgid1;
  int m_pdgid2;

  std::string m_malp;
  std::string m_gax;
  std::string m_alpt;

  std::string m_mpol;
  std::string m_mmon;
  std::string m_gamm;
  std::string m_mcharg;
  std::string m_mneut;

  // Random numbers seed
  std::vector<long int> m_seeds;

  // Commands to setup superchic
  CommandVector m_InitializeVector;

  bool set_user_params();
  bool prepare_params_file();

  // I/O to HEPEVT

  static Atlas_HEPEVT_modified* s_atlas_HEPEVT;


};

#endif
