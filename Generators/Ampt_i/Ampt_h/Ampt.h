/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */

#ifndef GENERATORMODULESAMPT_H
#define GENERATORMODULESAMPT_H

#include "GeneratorModules/GenModule.h"
//#include "Tauola_i/Atlas_HEPEVT.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "Ampt_h/HiMain1.h"
#include "Ampt_h/HiMain2.h"
#include "Ampt_h/HiParnt.h"
#include "Ampt_h/Hijdat.h"
#include "Ampt_h/HiStrng.h"
#include "Ampt_h/HijCrdn.h"
#include "Ampt_h/Ludat1.h"
#include "Ampt_h/Input1.h"
#include "Ampt_h/Input2.h"
#include "Ampt_h/Coal.h"
#include "Ampt_h/Anim.h"
#include "Ampt_h/Rndm3.h"
#include "Ampt_h/Resdcy.h"
#include "Ampt_h/Phidcy.h"
#include "Ampt_h/Cmsflag.h"
#include "Ampt_h/Phihj.h"
#include "Ampt_h/Para2.h"
#include "Ampt_h/Para7.h"
#include "Ampt_h/Para8.h"
#include "Ampt_h/Embed.h"
#include "Ampt_h/Xyembed.h"
#include "Ampt_h/Rndf77.h"
#include "Ampt_h/Arprnt.h"
#include "Ampt_h/Arevt.h"
#include "Ampt_h/Smearz.h"
#include "Ampt_h/Snn.h"
#include "Ampt_h/Hbt.h"
#include "Ampt_h/Atlas_HEPEVT.h"

#include "HepMC/IO_GenEvent.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/GenEvent.h"

#include <iostream>
#include <fstream>
#include <memory>

//using std::string;

//typedef std::vector<std::string> CommandVector;

class StoreGateSvc;
class HijingEventParams;

/**
@class  Generators/Ampt.h

@brief This code is used to get a AMPT Monte Carlo event.
    genInitialize() is used to read parameters
    callGenerator() makes the event
    genFinalize() writes log files etc.
    fillEvt(GeneratorEvent* evt) passes the event to HepMC
    The output will be stored in the transient event store so it can be
    passed to the simulation.

@author         Alexandre (Sasha) Lebedev, July 2018
                
*/

class Ampt : public GenModule {

public:

    Ampt(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~Ampt();

    virtual StatusCode  genInitialize   ();
    virtual StatusCode  callGenerator   ();
    virtual StatusCode  genFinalize     ();
    virtual StatusCode  fillEvt         (HepMC::GenEvent* evt);
    virtual CLHEP::HepLorentzVector randomizeVertex();

protected:

  StatusCode  fillEvtAmpt         (HepMC::GenEvent* evt);
  StatusCode  fillEvtHijing       (HepMC::GenEvent* evt);

// Files for Ampt-style text output of events
  std::ofstream outfile1;
  std::ofstream outfile2;

// AMPT input parameters
    int         m_nevnt; // number of events to generate
    float       m_efrm; // sqrt(S_NN) in GeV if FRAME is CMS, default 2560.
    std::string      m_frame8; // default "CMS"
    std::string      m_proj8;  // default "A"
    std::string      m_targ8; // default "A"
    std::string      m_frame; // default "CMS"
    std::string      m_proj;  // default "A"
    std::string      m_targ; // default "A"
    int         m_iap; // (projectile A number)
    int         m_iat; // (target A number)
    int         m_izp; // (projectile Z number)
    int         m_izt; // (target Z number)
    float       m_bmin; // mininum impact parameter in fm
    float       m_bmax; // maximum impact parameter in fm
    int         m_isoft; // ISOFT (D=1): select Default AMPT or String Melting(see below)
    int         m_ntmax; //  number of timesteps (D=150), see below
    float       m_dt; //  timestep in fm (hadron cascade time = DT*NTMAX) (D=0.2)
    float       m_parj41; // parameter a in Lund symmetric splitting function
    float       m_parj42; // parameter b in Lund symmetric splitting function
    int         m_ipop; //(D=1,yes;0,no) flag for popcorn mechanism(netbaryon stopping)
    float       m_parj5; // to control BMBbar vs BBbar in popcorn (D=1.0)
    int         m_ihpr2_6; // shadowing flag in Hijing (Default=1,yes; 0,no)
    int         m_ihpr2_4; // quenching flag in Hijing (D=0,no; 1,yes)
    float       m_hipr1_14; // quenching rate -dE/dx (GeV/fm) in case quenching flag=1
    float       m_hipr1_8; // p0 cutoff in HIJING for minijet productions (D=2.0)
    float       m_xmu; // parton screening mass in fm^(-1) (D=2.265d0), see below
    int         m_izpc; // IZPC: (D=0 forward-angle parton scatterings; 100,isotropic)
    double      m_alpha; // alpha in parton cascade (D=0.33d0), see parton screening mass
    double      m_dpcoal; // dpcoal in GeV
    double      m_drcoal; // drcoal in fm
    int         m_ihjsed; // take HIJING seed from m_hjseed (D=0)or at runtime(11)
//    int         m_nseed; // D=13150909        ! random seed for HIJING
    int         m_iseedp;  // random seed for parton cascade
    int         m_iksdcy;  // flag for K0s weak decays (D=0,no; 1,yes)
    int         m_iphidcy;   // flag for phi decays at end of hadron cascade (D=1,yes; 0,no)
    int         m_ipi0dcy;   // flag for pi0 decays at end of hadron cascade (D=0,no; 1,yes)
    int         m_ioscar;  //  optional OSCAR output (D=0,no; 1,yes; 2&3,more parton info)
    int         m_idpert; // flag for perturbative deuteron calculation (D=0,no; 1or2,yes)
    int         m_npertd; // integer factor for perturbative deuterons(>=1 & <=10000)
    int         m_idxsec; // choice of cross section assumptions for deuteron reactions
    float       m_pttrig; // Pt in GeV: generate events with >=1 minijet above this value
    int         m_maxmiss; // maxmiss (D=1000): maximum # of tries to repeat a HIJING event
    int         m_ihpr2_2; // flag to turn off initial and final state radiation (D=3)
    int         m_ihpr2_5; //  flag to turn off Kt kick (D=1)
    int         m_iembed; // flag to turn on quark pair embedding (D=0,no; 1,yes)
    float       m_pxqembd; // Initial Px value (GeV) of the embedded quark (u or d)
    float       m_pyqembd; // Initial Py value (GeV) of the embedded quark (u or d)
    float       m_xembd; // Initial x value (fm) of the embedded back-to-back q/qbar
    float       m_yembd; // Initial y value (fm) of the embedded back-to-back q/qbar
    int         m_nsembd; // nsembd(D=0) to embed a back-to-back q/qbar pair into each event
    float       m_psembd; // psembd (in GeV) to embed a back-to-back q/qbar pair into each event
    float       m_tmaxembd; // tmaxembd (in radian) to embed a back-to-back q/qbar pair into each event
    int         m_ishadow; // Flag to enable users to modify nuclear shadowing (D=0,no; 1,yes)
    double      m_dshadow; // Factor used to modify nuclear shadowing
    int         m_iphirp; // Flag for random orientation of reaction plane (D=0,no; 1,yes)

    // random number seeds
    //int  m_randomseed;
    std::vector<long int> m_seeds;

    // event vertex randomization
    bool m_sel;  // Constant vertex shifting on or off
    float m_x;
    float m_y;
    float m_z;
    bool m_rand; // Randomize for beam gas
    bool m_wide; // True allows particles with x,y distributions

    // event counter
    int m_events;

// Accessors to COMMON blocks
    HiMain1	m_himain1;
    HiMain2	m_himain2;
    HiStrng	m_histrng;
    HijCrdn	m_hijcrdn;
    HiParnt	m_hiparnt;
    Hijdat	m_hijdat;
    Ludat1	m_ludat1;
    Input1      m_input1;
    Input2      m_input2;
    Anim        m_anim;
    Coal        m_coal;
    Embed       m_embed;
    Xyembed     m_xyembed;
    Resdcy      m_resdcy;
    Cmsflag     m_cmsflag;
    Phihj       m_phihj;
    Phidcy      m_phidcy;
    Rndm3       m_rndm3;
    Para2       m_para2;
    Para7       m_para7;
    Para8       m_para8;
    Rndf77      m_rndf77;
    Arprnt      m_arprnt;
    Arevt       m_arevt;
    Smearz      m_smearz;
    Snn         m_snn;
    Hbt         m_hbt;

/// I/O to HEPEVT common
//    void store_Atlas_HEPEVT();
//    static Atlas_HEPEVT* s_atlas_HEPEVT;

/// Access to event store
    StoreGateSvc* m_storeGate;

/// Pointer to object with hijing event parameters
    HijingEventParams* m_event_params;

/// HepMC I/O
    std::auto_ptr<HepMC::IO_GenEvent> m_hepmcio;
    //HepMC::IO_GenEvent* m_hepmcio;

  bool m_writeAmpt;
  bool m_writeHepMC;
  bool m_DumpHijing;

};

#endif


