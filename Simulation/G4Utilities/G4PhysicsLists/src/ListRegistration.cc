#include "FadsPhysics/PhysicsListEntryT.h"
#include "G4HadronQElasticPhysics.hh"


#include "ExN01PhysicsList.hh"
static FADS::PhysicsListEntryT<ExN01PhysicsList> physlist_ExN01("ExN01");


#include "QGSP_BERT.hh"
static FADS::PhysicsListEntryT<QGSP_BERT> physlist_QGSP_BERT("QGSP_BERT");

#include "QGSP_BERT_CHIPS.hh"
static FADS::PhysicsListEntryT<QGSP_BERT_CHIPS> physlist_QGSP_BERT_CHIPS("QGSP_BERT_CHIPS");

#include "QGSP_FTFP_BERT.hh"
static FADS::PhysicsListEntryT<QGSP_FTFP_BERT> physlist_QGSP_FTFP_BERT("QGSP_FTFP_BERT");

#include "QGSP_BERT_HP.hh"
static FADS::PhysicsListEntryT<QGSP_BERT_HP> physlist_QGSP_BERT_HP("QGSP_BERT_HP");

#include "QGSP_BERT_TRV.hh"
static FADS::PhysicsListEntryT<QGSP_BERT_TRV> physlist_QGSP_BERT_TRV("QGSP_BERT_TRV");

#include "QGSP_BERT_EMV.hh"
static FADS::PhysicsListEntryT<QGSP_BERT_EMV> physlist_QGSP_BERT_EMV("QGSP_BERT_EMV");

#include "QGSP_BERT_EMX.hh"
static FADS::PhysicsListEntryT<QGSP_BERT_EMX> physlist_QGSP_BERT_EMX("QGSP_BERT_EMX");

#include "QGSP_BERT_NOLEP.hh"
static FADS::PhysicsListEntryT<QGSP_BERT_NOLEP> physlist_QGSP_BERT_NOLEP("QGSP_BERT_NOLEP");


#include "QGS_BIC.hh"
static FADS::PhysicsListEntryT<QGS_BIC> physlist_QGS_BIC("QGS_BIC");

#include "QGSP_BIC.hh"
static FADS::PhysicsListEntryT<QGSP_BIC> physlist_QGSP_BIC("QGSP_BIC");

#include "QGSP_BIC_HP.hh"
static FADS::PhysicsListEntryT<QGSP_BIC_HP> physlist_QGSP_BIC_HP("QGSP_BIC_HP");

#include "QGSP_BIC_EMY.hh"
static FADS::PhysicsListEntryT<QGSP_BIC_EMY> physlist_QGSP_BIC_EMY("QGSP_BIC_EMY");


#include "QGSC_BERT.hh"
static FADS::PhysicsListEntryT<QGSC_BERT> physlist_QGSC_BERT("QGSC_BERT");

#include "QGSC_CHIPS.hh"
static FADS::PhysicsListEntryT<QGSC_CHIPS> physlist_QGSC_CHIPS("QGSC_CHIPS");


#include "FTFP_BERT.hh"
static FADS::PhysicsListEntryT<FTFP_BERT> physlist_FTFP_BERT("FTFP_BERT");

#include "FTFP_BERT_TRV.hh"
static FADS::PhysicsListEntryT<FTFP_BERT_TRV> physlist_FTFP_BERT_TRV("FTFP_BERT_TRV");

#include "FTFP_BERT_EMV.hh"
static FADS::PhysicsListEntryT<FTFP_BERT_EMV> physlist_FTFP_BERT_EMV("FTFP_BERT_EMV");

#include "FTFP_BERT_EMX.hh"
static FADS::PhysicsListEntryT<FTFP_BERT_EMX> physlist_FTFP_BERT_EMX("FTFP_BERT_EMX");

#include "FTF_BIC.hh"
static FADS::PhysicsListEntryT<FTF_BIC> physlist_FTF_BIC("FTF_BIC");


#include "QGSP.hh"
static FADS::PhysicsListEntryT<QGSP> physlist_QGSP("QGSP");

#include "LHEP.hh"
static FADS::PhysicsListEntryT<LHEP> physlist_LHEP("LHEP");

#include "LHEP_EMV.hh"
static FADS::PhysicsListEntryT<LHEP_EMV> physlist_LHEP_EMV("LHEP_EMV");

#include "CHIPS.hh"
static FADS::PhysicsListEntryT<CHIPS> physlist_CHIPS("CHIPS");

#include "QBBC.hh"
static FADS::PhysicsListEntryT<QBBC> physlist_QBBC("QBBC");

#include "Shielding.hh"
static FADS::PhysicsListEntryT<Shielding> physlist_Shielding("Shielding");

#include "FadsPhysics/FactoryMadeList.h"
static FADS::FactoryMadeList physlist_FTFP_BERT_EMY("FTFP_BERT_EMY");
static FADS::FactoryMadeList physlist_FTFP_BERT_EMZ("FTFP_BERT_EMZ");

// Physics lists for hadronic variations - requires geant4.9.6.patch03.atlas06
static FADS::FactoryMadeList physlist_FTFP_BERT_BIC("FTFP_BERT_BIC");
static FADS::FactoryMadeList physlist_FTFP_BERT_rescattering("FTFP_BERT_rescattering");
static FADS::FactoryMadeList physlist_FTFP_BERT_chipsXS("FTFP_BERT_chipsXS");
static FADS::FactoryMadeList physlist_FTFP_BERT_lowEneutrons("FTFP_BERT_lowEneutrons");
static FADS::FactoryMadeList physlist_FTFP_BERT_noDiffraction("FTFP_BERT_noDiffraction");
static FADS::FactoryMadeList physlist_FTFP_BERT_newElastic("FTFP_BERT_newElastic");

#include "FTFP_BERT_ATL.hh"
static FADS::PhysicsListEntryT<FTFP_BERT_ATL> physlist_FTFP_BERT_ATL("FTFP_BERT_ATL");
