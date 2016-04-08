/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESHIJING_H
#define GENERATORMODULESHIJING_H

#include "GeneratorModules/GenModule.h"
#include "CLHEP/Vector/LorentzVector.h"

using std::string;
typedef std::vector<std::string> CommandVector;


/// @brief This code is used to get a BaurMC Monte Carlo event.
///
/// genInitialize() is used to read parameters
/// callGenerator() makes the event
/// genFinalize() writes log files etc
/// fillEvt(GeneratorEvent* evt) passes the event to HepMC
///
/// The output will be stored in the transient event store so it can be passed
/// to the simulation.
///
/// @author Georgios Stavropoulos, October 2002
/// @author Ewelina Lobodzinska (Jan. 2008) - doxygen docu
///
class Baur : public GenModule {
public:
    Baur(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode genInitialize();
    StatusCode callGenerator();
    StatusCode genFinalize();
    StatusCode fillEvt(HepMC::GenEvent*); 

protected:

    // inputs to HIJSET (Baur initialization) ...
    float	m_efrm;
    string 	m_frame;
    string 	m_proj;
    string 	m_targ;
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

    // Limit settings
    double m_partonStoreMinPt;
    double m_vertexOffsetCut;

    // Random numbers seed
    int m_randomseed;
    std::vector<long int> m_seeds;

    // event counter
    int m_events ;

    // Commands to setup Baur
    CommandVector m_InitializeVector;

    void set_user_params	(void);

  /// @todo Remove: the GenModule / AthAlgorithm base class provides built-in TES access
  StoreGateSvc* m_storeGate;

};

#endif










