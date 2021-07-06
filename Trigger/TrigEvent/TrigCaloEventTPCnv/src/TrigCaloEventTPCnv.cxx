/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p1.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p4.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p5.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p5.h"

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_p1.h"

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainerCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p2.h"

#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p2.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_tlp1.h" 
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_tlp1.h" 
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p1.h"

#include "TrigCaloEventTPCnv/TrigT2Jet_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p1.h"

#include "TrigCaloEventTPCnv/TrigT2JetContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_p3.h"

#include "TrigCaloEventTPCnv/RingerRingsContainer_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainer_p1.h"
#include "TrigCaloEventTPCnv/RingerRings_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRings_p1.h"
#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_p2.h"
#include "TrigCaloEventTPCnv/RingerRingsContainer_p2.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p2.h"
#include "TrigCaloEventTPCnv/RingerRings_p2.h"

#include "TrigCaloEventTPCnv/TrigEMClusterContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_tlp2.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_tlp2.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigCaloCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p3.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_p4.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p4.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p4.h"

#include "TrigCaloEventTPCnv/TrigCaloCluster_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterContainerCnv_p3.h"

#include "TrigCaloEventTPCnv/TrigT2ZdcSignals_p1.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsContainerCnv_p1.h"

#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutput_p1.h"

#include "TrigCaloEventTPCnv/TrigRNNOutputContainerCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p2.h"

DECLARE_TPCNV_FACTORY(RingerRingsCnv_tlp1,
                      RingerRings,
                      RingerRings_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(RingerRingsContainerCnv_tlp1,
                      RingerRingsContainer,
                      RingerRingsContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(RingerRingsContainerCnv_p2,
                      RingerRingsContainer,
                      RingerRingsContainer_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigEMClusterCnv_tlp1,
                      TrigEMCluster,
                      TrigEMCluster_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigEMClusterCnv_tlp2,
                      TrigEMCluster,
                      TrigEMCluster_tlp2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigEMClusterContainerCnv_tlp1,
                      TrigEMClusterContainer,
                      TrigEMClusterContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigEMClusterContainerCnv_p3,
                      TrigEMClusterContainer,
                      TrigEMClusterContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigEMClusterContainerCnv_p4,
                      TrigEMClusterContainer,
                      TrigEMClusterContainer_p4,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TrigCaloClusterContainerCnv_p3,
                      TrigCaloClusterContainer,
                      TrigCaloClusterContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigT2ZdcSignalsContainerCnv_p1,
                      TrigT2ZdcSignalsContainer,
                      TrigT2ZdcSignalsContainer_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigRNNOutputCnv_tlp1,
                      TrigRNNOutput,
                      TrigRNNOutput_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigRNNOutputContainerCnv_tlp1,
                      TrigRNNOutputContainer,
                      TrigRNNOutputContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigRNNOutputContainerCnv_p2,
                      TrigRNNOutputContainer,
                      TrigRNNOutputContainer_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigT2JetCnv_tlp1,
                      TrigT2Jet,
                      TrigT2Jet_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigT2JetContainerCnv_tlp1,
                      TrigT2JetContainer,
                      TrigT2JetContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigT2JetContainerCnv_p3,
                      TrigT2JetContainer,
                      TrigT2JetContainer_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigT2MbtsBitsCnv_p1,
                      TrigT2MbtsBits,
                      TrigT2MbtsBits_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigT2MbtsBitsCnv_p2,
                      TrigT2MbtsBits,
                      TrigT2MbtsBits_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigT2MbtsBitsContainerCnv_tlp1,
                      TrigT2MbtsBitsContainer,
                      TrigT2MbtsBitsContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigT2MbtsBitsContainerCnv_p3,
                      TrigT2MbtsBitsContainer,
                      TrigT2MbtsBitsContainer_p3,
                      Athena::TPCnvVers::Current)
                      
// DECLARE_TPCNV_FACTORY(TrigTauClusterCnv_tlp1,
//                       TrigTauCluster,
//                       TrigTauCluster_tlp1,
//                       Athena::TPCnvVers::Old)
                      
// DECLARE_TPCNV_FACTORY(TrigTauClusterCnv_tlp2,
//                       TrigTauCluster,
//                       TrigTauCluster_tlp2,
//                       Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTauClusterContainerCnv_tlp1,
                      TrigTauClusterContainer,
                      TrigTauClusterContainer_tlp1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTauClusterContainerCnv_p2,
                      TrigTauClusterContainer,
                      TrigTauClusterContainer_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTauClusterContainerCnv_p3,
                      TrigTauClusterContainer,
                      TrigTauClusterContainer_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigTauClusterContainerCnv_p4,
                      TrigTauClusterContainer,
                      TrigTauClusterContainer_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigTauClusterContainerCnv_p5,
                      TrigTauClusterContainer,
                      TrigTauClusterContainer_p5,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigTauClusterDetailsContainerCnv_tlp1,
                      TrigTauClusterDetailsContainer,
                      TrigTauClusterDetailsContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigTauClusterDetailsContainerCnv_p2,
                      TrigTauClusterDetailsContainer,
                      TrigTauClusterDetailsContainer_p2,
                      Athena::TPCnvVers::Current)
