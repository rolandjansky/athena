#include "../TriggerTowerMaker.h"
#include "../Run2TriggerTowerMaker.h"
#include "../CPMTowerMaker.h"
#include "../JetElementMaker.h"
#include "../Run2CPMTowerMaker.h"
#include "../Run2JetElementMaker.h"
#include "../MergeSlices.h"
#include "../Tester.h"
#include "../ReprocessTriggerTowers.h"
#include "../CPMSim.h"
#include "../JEMJetSim.h"
#include "../JEMEnergySim.h"
#include "../RoIROD.h"
#include "../CPCMX.h"
#include "../JetCMX.h"
#include "../EnergyCMX.h"
#include "../OverlayRun2TriggerTowerMaker.h"
#include "../TransientDatabaseOverride.h"
#include "../OverlayTTL1.h"


using namespace LVL1;

DECLARE_COMPONENT( TriggerTowerMaker )
DECLARE_COMPONENT( Run2TriggerTowerMaker )
DECLARE_COMPONENT( CPMTowerMaker )
DECLARE_COMPONENT( JetElementMaker )
DECLARE_COMPONENT( Run2CPMTowerMaker )
DECLARE_COMPONENT( Run2JetElementMaker )
DECLARE_COMPONENT( MergeSlices )
DECLARE_COMPONENT( Tester )
DECLARE_COMPONENT( ReprocessTriggerTowers )
DECLARE_COMPONENT( CPMSim )
DECLARE_COMPONENT( JEMJetSim )
DECLARE_COMPONENT( JEMEnergySim )
DECLARE_COMPONENT( RoIROD )
DECLARE_COMPONENT( CPCMX )
DECLARE_COMPONENT( JetCMX )
DECLARE_COMPONENT( EnergyCMX )
DECLARE_COMPONENT( TransientDatabaseOverride )
DECLARE_COMPONENT( OverlayRun2TriggerTowerMaker )
DECLARE_COMPONENT( OverlayTTL1 )
