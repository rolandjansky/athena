/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESHIJING_H
#define GENERATORMODULESHIJING_H

#include "GeneratorModules/GenModule.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "Hijing_i/HiParnt.h"
#include "Hijing_i/RanSeed.h"
#include "Hijing_i/HiMain1.h"
#include "Hijing_i/HiMain2.h"
#include "Hijing_i/HijJet1.h"
#include "Hijing_i/HijJet2.h"
#include "Hijing_i/HijJet4.h"
#include "Hijing_i/HiStrng.h"
#include "Hijing_i/HijCrdn.h"

typedef std::vector<std::string> CommandVector;

// new to store hijing event parameters
// ------------------------------------
class StoreGateSvc;
class HijingEventParams;
// ---------------------
/**
@class  Generators/Hijing.h

@brief This code is used to get a HIJING Monte Carlo event.
    genInitialize() is used to read parameters
    callGenerator() makes the event
    genFinalize() writes log files etc
    fillEvt(GeneratorEvent* evt) passes the event to HepMC

    The output will be stored in the transient event store so it can be
    passed to the simulation.

@author         Georgios Stavropoulos, October 2002
                
                Ewelina Lobodzinska (Jan. 2008) - doxygen docu
*/

class Hijing:public GenModule {
public:
    Hijing(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~Hijing();
    
    virtual StatusCode	genInitialize	();
    virtual StatusCode 	callGenerator	();
    virtual StatusCode 	genFinalize	();
    virtual StatusCode 	fillEvt		(HepMC::GenEvent* evt);
    virtual CLHEP::HepLorentzVector randomizeVertex();

protected:

    // inputs to HIJSET (Hijing initialization) ...
    float	m_efrm;
    std::string 	m_frame;
    std::string 	m_proj;
    std::string 	m_targ;
    int 	m_iap;
    int 	m_iat;
    int 	m_izp;
    int 	m_izt;
    // ... and HIJING (event generation)
    float	m_bmin;
    float	m_bmax;

    // Vertex shifting
    
    float m_x;
    float m_y;
    float m_z;
    bool m_sel; // Vertex shifting on or off
    bool m_spec; // TRUE  will keep specator information (default)
    
    //Set internal randomization of vertices for beam gas gen (on or off).
    bool m_rand;
    bool m_wide; // True allows particles with x,y distributions
    bool m_prand; //BPK mirror event randomly

    // Limit settings
    double m_partonStoreMinPt;
    double m_vertexOffsetCut;
 
    // Random numbers seed
    int  m_randomseed;
    std::vector<long int> m_seeds;

    // event counter
    int m_events ;

    // Commands to setup hijing
    CommandVector m_InitializeVector;

     // Accessor to HIJING Options and parameters COMMON
    HiParnt	m_hiparnt;

    // Accessor to HIJING Random number seed COMMON
    RanSeed	m_ranseed;
    
    // Accessors to HIJING Event information COMMONS
    HiMain1	m_himain1;
    HiMain2	m_himain2;
    HijJet1	m_hijjet1;
    HijJet2	m_hijjet2;
    HijJet4	m_hijjet4;
    HiStrng	m_histrng;
    HijCrdn	m_hijcrdn;
    
    void       	set_user_params	(void);

  // new to store hijing event parameters
  // ------------------------------------
  /// Access to event store
  StoreGateSvc* m_storeGate;
  
  /// Pointer to object with hijing event parameters
  HijingEventParams* m_event_params;
  //--------------------------------
};

#endif










