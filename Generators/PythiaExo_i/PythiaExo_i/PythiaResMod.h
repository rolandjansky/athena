/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESPYTHIARESMOD_H
#define GENERATORMODULESPYTHIARESMOD_H

#include "GeneratorModules/GenModule.h"

#include "PythiaExo_i/Pydat1.h"
#include "PythiaExo_i/Pydat2.h"
#include "PythiaExo_i/Pydat3.h"
#include "PythiaExo_i/Pysubs.h"
#include "PythiaExo_i/Pypars.h"
#include "PythiaExo_i/Pydatr.h"
#include "PythiaExo_i/Pymssm.h"
#include "PythiaExo_i/Pypued.h"
#include "PythiaExo_i/Pypevwt.h"
#include "PythiaExo_i/Pymsrv.h"
#include "PythiaExo_i/Pyint1.h"
#include "PythiaExo_i/Pyint2.h"
#include "PythiaExo_i/Pyint5.h"
#include "PythiaExo_i/Pytcsm.h"

#include "AcerMC_i/AcerMC_acset.h"
#include "Tauola_i/Atlas_HEPEVT.h"

/// @todo Not in a header file!
using std::string;

/// @todo Inside class (these are being declared as global symbols)
typedef std::vector<std::string> CommandVector;
typedef std::vector<std::string> PygiveCommandVector;

// Forward declaration of RNG service interface
class IAtRndmGenSvc;


/***
@brief ~ carbon copy of Pythia.h for ResMod class 
PythiaModifies/pysgex.F modifications introduced for W' and Z' [ISUB 141,142]
   set PythiaResMod.UseResMod=1 to use it
***/


class PythiaResMod : public GenModule {
public:

  /// @name Construction / destruction
  //@{
  /// Standard constructor
  PythiaResMod(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PythiaResMod();
  //@}


  /// @name Event loop methods
  //@{
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);
  //@}


  /// @name Tune switches
  /// @brief ATLAS tune and parameters implementation, introduced for MC09/MC08. See atlasTune.cxx
  //@{

  /// Set tune via function call 
  StatusCode setPythiaTune();

  /// Use current ATLAS tune 
  StatusCode atlasTune();

  //@}


  /// @ name Accessors to PYTHIA COMMONS
  //@{
  Pydat1& pydat1();
  Pydat2& pydat2();
  Pydat3& pydat3();
  Pysubs& pysubs();
  Pypars& pypars();
  Pydatr& pydatr();
  Pymssm& pymssm();
  Pypued& pypued();
  Pypevwt& pypevwt();
  Pymsrv& pymsrv();
  Pyint1& pyint1();
  Pyint2& pyint2();
  Pyint5& pyint5();
  Pytcsm& pytcsm();
  //@}

  /// Accessor for external processes
  AcerMC_acset& acermc_acset();

  // Pointer to AtRndmGenSvc
  static IAtRndmGenSvc* p_AtRndmGenSvc;
  static std::string pythia_stream;


protected:

  // Settable Properties

  /// @name Inputs to @c pyinit
  //@{
  std::string m_frame;
  std::string m_external; //to specify the specific process when m_frame is set to user
  std::string m_beam;
  std::string m_target;
  double m_win;
  int m_UseResMod;
  //@}


  /// Tune setting, assumed form is  m_Default_Tune_Name=TUNETYPE_TUNEINDEX
  std::string m_Tune_Name;  

  /// PYGIVE argument as C string
  const char* m_pygive_cstr;
  const char* m_pygive_init_cstr;
  const char* m_pygive_gen_cstr;

  /// @name Various input parameters
  //@{
  int  m_msel;
  int  m_firstlistevent;
  int  m_lastlistevent;
  int  m_eventlistlevel;
  std::vector<int>  m_pystatlistlevel;
  int  m_initlistlevel;
  int  m_randomseed;
  std::string  m_envval1;
  int  m_fortout;
  //@}

  /// @name Random file numbers, offsets, etc.
  //@{
  std::string  m_RndmFileName;
  int	m_RndmFileNumber;
  int	m_RndmSwitch;
  int	m_RndmFirstEvent;
  int	m_RndmSkipEvents;
  int	m_RndmMOVE;
  int	m_RndmFileLength;
  //@}

  /// RNG seeds
  std::vector<long int> m_seeds;

  /// Event counter
  int m_events;

  /// @name Common blocks
  //@{
  /// Subprocess switches
  Pysubs m_pysubs;
  /// Pydat1 data, methods 
  Pydat1 m_pydat1;  
  /// Pypars data, methods 
  Pypars m_pypars;
  /// Pydat2 data, methods 
  Pydat2 m_pydat2;
  /// Pydat3 data, methods 
  Pydat3 m_pydat3;  
  /// Pydatr access for random numbers (only set the 1st seed just now
  Pydatr m_pydatr;
  /// Pyint1 access for data
  Pyint1 m_pyint1;
  /// Pyint2 access for data
  Pyint2 m_pyint2;	
  /// Pyint5 access for data
  Pyint5 m_pyint5;	
  /// SUSY access
  Pymssm m_pymssm; 
  /// UED access
  Pypued m_pypued;
  // Event weight access
  Pypevwt m_pypevwt; 
  /// RPV
  Pymsrv m_pymsrv; 
  /// PYTCSM access
  Pytcsm m_pytcsm; 
  //@}

  /// External process
  int m_ExternalProcess; 
  AcerMC_acset m_acermc_acset;

//sgluon
  bool m_addParticle;

  // Commands to setup pythia
  CommandVector m_pythiaCommandVector;
  // Commands to setup pythia / make param. querries - via PYGIVE:
  // pass parameters or querry parameter value before PYINIT
  PygiveCommandVector m_PygiveCommandVector;
  // querry parameter value after PYINIT e.g. to check if initializaton resets the value
  PygiveCommandVector m_Param_Query_AfterInit;
  // querry parameter value after PYEVNT e.g. to get event-specific param. value
  PygiveCommandVector m_Param_Query_AfterGen;

  /// @deprecated ???
  bool m_AtlasDefaults;

  /// @name Tunes implemented as switches
  /// @deprecated Kept for backward compatibility -- strongly prefer to use Tune_Name flag 
  //@{
  /** use ATLAS PYTHIA technical settings [default:true]
      eventRecord, errors*/
  bool m_useAtlasPythiaRecomm, m_Default_useAtlasPythiaRecomm; 
  /**use ATLAS PYTHIA parameters [default:true]
      masses, dec. widths, etc. */
  bool m_useAtlasPythiaCommon,m_Default_useAtlasPythiaCommon; 
  /** complete mc09 tune Pythia parameters set [default:true]*/
  bool m_useAtlasPythiaTune09,m_Default_useAtlasPythiaTune09;
  /**complete mc08 tune Pythia parameters set [default:false]
     if set to true overrides mc09 tune, 
     mc09 should be set to false in order to use mc08 only */
  bool m_useAtlasPythiaTune08,m_Default_useAtlasPythiaTune08;
  /** use no ATLAS PYTHIA settings, apart from pynit inputs [default:false]*/
  bool m_useNoAtlasPythiaParam,m_Default_useNoAtlasPythiaParam;


  ///call pytune directly in case any parameters set py the tune are to be overwritten from jO
  //@{
  int m_direct_call_to_pytune;
  //@}

  /// The susy file name
  std::string m_read_Filesusy; 

  //-----------------------------------------------------------------------------

  /// @name Other class members (not properties)
  //@{

  // more tune-related vars:
  //m_Tune_Index  see AtlasTune.cxx for catalogue
  //m_Default_Tune_Name=TUNETYPE_TUNEINDEX
  //so that (after parsing in Pythia.cxx): m_Tune_Type=TUNETYPE and m_Tune_Index=TUNEINDEX
  int  m_Tune_Index;
  std::string m_Tune_Type;
  // needed in tune setting for protection against tune-seting inf. loop
  int m_atlasTune_loop_prot;
  //tune to be assumed by default and in cases of invalid TUNETYPE or TUNEINDEX when TUNETYPE=="ATLAS"
  std::string m_Default_Tune_Name, m_Default_Tune_Type; int m_Default_Tune_Index;
  // dummy starting tune string
  std::string m_Starting_Tune_Name, m_Starting_Tune_Type; int m_Starting_Tune_Index;
  // backward compatibility with early 08/09 jO when tunes were impl. as switches:
  // to be updated if MC08 and MC09 tunes Tune_Type or Tune_Index are changed 
  std::string m_mc0809_Tune_Type;
  int m_mc09_Tune_Index;
  int m_mc08_Tune_Index;
  // 

  //@}

  // multiply x-section by a num. factor
  // add both in the MetaData print-out 
  double m_Default_xsscale, m_xsscale; 

  // print out manually set x-section from the MetaData 
  double m_Default_SetCrossSection, m_SetCrossSection;

  /// Method to Manage the random seed IO from/to a file
  void RandomNumberIO();

  /// I/O to HEPEVT
  void store_Atlas_HEPEVT();

  /// TAUOLA HEPEVT
  static Atlas_HEPEVT* s_atlas_HEPEVT;
};


#endif
