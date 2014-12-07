/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRec/egammaForwardBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "PathResolver/PathResolver.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/Electron.h"
 
#include <algorithm> 
#include <math.h>


using CLHEP::GeV;


//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
    
egammaForwardBuilder::egammaForwardBuilder(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_timingProfile(0)
{ 
  // Name of Electron Container to be created
  declareProperty("ElectronOutputName",
		  m_electronOutputName="",
		  "Name of Electron Container to be created");

  // Name of the input cluster collection
  declareProperty("TopoClusterName",
		  m_topoClusterName="",
		  "Name of the input cluster collection");

  // Value of the ET cut
  declareProperty("EtCut",
		  m_ETcut=5.*GeV,
		  "Value of the ET cut");

  // Value of the eta cut
  declareProperty("EtaCut",
		  m_etacut=2.5,
		  "Value of the eta cut");

  // Value of the eta cut in forward calorimeter
  declareProperty("EtaFCalCut",
		  m_etafcalcut=3.2,
		  "Value of the eta cut in forward calorimeter");

  // Name of the output EM cluster container
  declareProperty("ClusterContainerName",
		  m_outClusterContainerName="",
		  "Name of the output EM cluster container");

  // 
  declareProperty("FirstENGdensCuts",m_firstENGdensCuts);

  //
  declareProperty("FracMaxCuts",m_fracMaxCuts);

  // Value of cuts on longitudinal shower
  declareProperty("LongMomentCuts",
		  m_longitudinalCuts,
		  "Value of cuts on longitudinal shower");

  // Cut value for second lambda moment
  declareProperty("SecondLambdaCuts",
		  m_secondLambdaCuts,
		  "Cut value for second lambda moment");

  // Cut value for lateral moment
  declareProperty("LatMomentCuts",
		  m_lateralCuts,
		  "Cut value for lateral moment");

  // Cut value for secondR moment
  declareProperty("SecondRCuts",
		  m_secondRCuts,
		  "Cut values for secondR moment");

  // Cut value for Centre lambda moment
  declareProperty("CenterLambdaCuts",
		  m_centerLambdaCuts,
		  "Cut for Centre lambda moment");
  // Name of the object quality tool
  declareProperty("ObjectQualityToolName", 
		  m_ObjectQualityToolName="",
		  "Name of the object quality Tool");


  // Handle of 4-mom Builder
  declareProperty("FourMomBuilderTool",m_fourMomBuilder,
		  "Handle of 4-mom Builder");

} 


// ================================================================
egammaForwardBuilder::~egammaForwardBuilder()
{ 
  //
  // destructor
  //
}

// =================================================================
StatusCode egammaForwardBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing egammaForwardBuilder ");
  
  //for bitwise... 
  m_discrt["FIRST_ENG_DENS"] =  1;
  m_discrt["ENG_FRAC_MAX"]   =  2;
  m_discrt["LONGITUDINAL"]   =  4;
  m_discrt["SECOND_LAMBDA"]  =  8;
  m_discrt["LATERAL"]        = 16;
  m_discrt["SECOND_R"]       = 32;
  m_discrt["CENTER_LAMBDA"]  = 64;
  
  // retrieve object quality tool 
  RetrieveObjectQualityTool();

  m_timingProfile = 0;
  StatusCode sc = service("ChronoStatSvc",m_timingProfile);
  if(sc.isFailure() || m_timingProfile == 0) {
    ATH_MSG_ERROR("Cannot find the ChronoStatSvc " << m_timingProfile);
  }

  // retrieve 4-mom builder:
  if ((sc = RetrieveEMFourMomBuilder()).isFailure()) {
    return sc;
  }

  ATH_MSG_DEBUG("Initialization completed successfully");
  return StatusCode::SUCCESS;
}

// ====================================================================
void egammaForwardBuilder::RetrieveObjectQualityTool()
{
  //
  // retrieve object quality tool
  //

  if (m_ObjectQualityToolName=="") {
    ATH_MSG_INFO("egammaOQFlagsBuilder is disabled  " 
		 << m_ObjectQualityToolName);
    m_objectqualityTool=0;
    return;
  } 

  m_objectqualityTool = ToolHandle<IegammaBaseTool>(m_ObjectQualityToolName);
  if(m_objectqualityTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve " << m_objectqualityTool
		  << ", turning it off");
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_objectqualityTool);

  return;
}

StatusCode egammaForwardBuilder::RetrieveEMFourMomBuilder()
{
  //
 // retrieve EMFourMomBuilder tool
 //
  
  if (m_fourMomBuilder.empty()) {
    ATH_MSG_ERROR("EMFourMomBuilder is empty");
    return StatusCode::FAILURE;
  }
  
  
  if(m_fourMomBuilder.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_fourMomBuilder);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG("Retrieved Tool "<<m_fourMomBuilder);
  
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode egammaForwardBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode egammaForwardBuilder::execute()
{
  //
  // athena execute method
  //

  ATH_MSG_DEBUG("Executing egammaForwardBuilder ");

  // create an egamma container and register it
  xAOD::ElectronContainer* xaodFrwd = new xAOD::ElectronContainer();
  CHECK( evtStore()->record( xaodFrwd, m_electronOutputName ) );
  xAOD::ElectronAuxContainer* auxFrwd = new xAOD::ElectronAuxContainer();
  CHECK( evtStore()->record( auxFrwd, m_electronOutputName + "Aux." ) );
  xaodFrwd->setStore( auxFrwd );
  ATH_MSG_DEBUG( "Recorded Electrons with key: " << m_electronOutputName );


  //cluster
  xAOD::CaloClusterContainer* outClusterContainer = new xAOD::CaloClusterContainer();
  CHECK( evtStore()->record( outClusterContainer, m_outClusterContainerName ) );
  xAOD::CaloClusterAuxContainer* auxCaloCluster = new xAOD::CaloClusterAuxContainer();
  CHECK( evtStore()->record( auxCaloCluster, m_outClusterContainerName + "Aux." ) );
  outClusterContainer->setStore( auxCaloCluster );



  //Topo cluster Container
  const xAOD::CaloClusterContainer* cluster;
  StatusCode sc = evtStore()->retrieve(cluster, m_topoClusterName);
  if(sc.isFailure()) {
    ATH_MSG_WARNING("egammaForwardBuilder::Could not retrieve Cluster container"); 
    return StatusCode::SUCCESS;
  }



  //loop over cluster container
  xAOD::CaloClusterContainer::const_iterator  clus_begin = cluster->begin();
  xAOD::CaloClusterContainer::const_iterator  clus_end   = cluster->end();

  for (; clus_begin!=clus_end; clus_begin++) {
   
    //Preselectcion cuts
    if((*clus_begin)->et() < m_ETcut||
       fabs((*clus_begin)->eta())<m_etacut)
      continue;
 
    //Save it in the electroncontainer 
    xAOD::Electron* el = new xAOD::Electron();
    xaodFrwd->push_back(el);

    el->setAuthor( xAOD::EgammaParameters::AuthorFwdElectron );

     
    xAOD::CaloCluster *newcluster = new xAOD::CaloCluster(**clus_begin);
    outClusterContainer->push_back(newcluster);
    
    int index = outClusterContainer->size() - 1;
    
    ElementLink<xAOD::CaloClusterContainer> newclusterElementLink;
    newclusterElementLink.toIndexedElement( *outClusterContainer, index  );

    std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
    linksToClusters.push_back(newclusterElementLink);
    el->setCaloClusterLinks(linksToClusters);    

    //do  Four Momentum
    CHECK(m_fourMomBuilder->execute(el));
    
    // do object quality 
    CHECK( ExecObjectQualityTool(el) );
    
    const long frwdElectronTight = 126;
    const long frwdElectronLoose = 104;

    //Egamma ID
    long IsEM = isEMForward( newcluster );
    ATH_MSG_VERBOSE(" the IsEM calculated "<<IsEM);

    const bool isLoose = (frwdElectronLoose & IsEM) == 0;
    const bool isTight = (frwdElectronTight & IsEM) == 0;
    el->setPassSelection(isLoose, "Loose");
    el->setPassSelection(isTight, "Tight");

  }

  
//   //lock the ElectronForward collection
//   sc = evtStore()->setConst(xaodFrwd);
//   if ( sc.isFailure() ) {
//     ATH_MSG_WARNING("cannot set electron_container to const ");
//   }
  
  //outClusterContainer->print();
  ATH_MSG_VERBOSE("egammaForward execute completed successfully");
  
  return StatusCode::SUCCESS;
}  
  
// ================================================
long egammaForwardBuilder::isEMForward(const xAOD::CaloCluster* cluster)
{
  //
  // apply cuts and fills isem variable
  //

  double firstENGdens=-999.;
  double lateral=-999.;
  double secondR=-999.;
  double centerLambda=-999.;
  double fracMax=-999.;
  double longitudinal=-999.;
  double secondLambda=-999.;

  // loop on moments
  
  if( !cluster->retrieveMoment( xAOD::CaloCluster::FIRST_ENG_DENS,firstENGdens) )
    firstENGdens = -999;
 
  if( !cluster->retrieveMoment( xAOD::CaloCluster::ENG_FRAC_MAX, fracMax) )
    fracMax = -999;
  
  if( !cluster->retrieveMoment( xAOD::CaloCluster::LONGITUDINAL, longitudinal) )
    longitudinal = -999;
  
  if( !cluster->retrieveMoment( xAOD::CaloCluster::SECOND_LAMBDA,secondLambda) )
    secondLambda = -999;
  
  if( !cluster->retrieveMoment( xAOD::CaloCluster::LATERAL, lateral) )
    lateral =-999;
  
  if( !cluster->retrieveMoment( xAOD::CaloCluster::SECOND_R, secondR) )
    secondR =-999;
  
  if( !cluster->retrieveMoment( xAOD::CaloCluster::CENTER_LAMBDA, centerLambda) )
    centerLambda = -999;

  
  long _EM_Pass = 0;
  long m_Frwd_ID;

  //cuts in the EMEC and FCal can be differents
  int index;
  if(fabs(cluster->eta())<m_etafcalcut)
    index = 0;
  else
    index = 1;

  if(!(firstENGdens > m_firstENGdensCuts[index]))
    _EM_Pass |= m_discrt["FIRST_ENG_DENS"];
  m_Frwd_ID=_EM_Pass;

    
  if(!(fracMax > m_fracMaxCuts[index]))
    _EM_Pass |= m_discrt["ENG_FRAC_MAX"];
  m_Frwd_ID=_EM_Pass;


  if(!(longitudinal < m_longitudinalCuts[index]))
    _EM_Pass |= m_discrt["LONGITUDINAL"];
  m_Frwd_ID=_EM_Pass;


  if(!(secondLambda < m_secondLambdaCuts[index]))
    _EM_Pass |= m_discrt["SECOND_LAMBDA"];
  m_Frwd_ID=_EM_Pass;


  if(!(lateral < m_lateralCuts[index]))
    _EM_Pass |= m_discrt["LATERAL"];
  m_Frwd_ID=_EM_Pass;


  if(!(secondR < m_secondRCuts[index]))
    _EM_Pass |= m_discrt["SECOND_R"];
  m_Frwd_ID=_EM_Pass;
  
  //if((centerLambda < m_centerLambdaCuts[index]))
  if(!(centerLambda < m_centerLambdaCuts[index]))
    _EM_Pass |= m_discrt["CENTER_LAMBDA"];
  m_Frwd_ID=_EM_Pass;

  
  return m_Frwd_ID;
}

// ===========================================================
StatusCode egammaForwardBuilder::ExecObjectQualityTool(xAOD::Egamma *eg)
{
  //
  // execution of the object quality tools
  //

  // protection in case tool is not available
  // return success as algorithm may be able to run without it 
  // in degraded mode

  if (m_ObjectQualityToolName=="") return StatusCode::SUCCESS;

  // setup chrono for this tool
  std::string chronoName=this->name()+"_"+m_objectqualityTool->name() ;
  if(m_timingProfile) m_timingProfile->chronoStart(chronoName);

  // execute the tool
  StatusCode sc = m_objectqualityTool->execute(eg);
  if ( sc.isFailure() ) {
    ATH_MSG_DEBUG("failure returned by object quality tool"); 
  }

  if(m_timingProfile) m_timingProfile->chronoStop(chronoName);

  return sc;
}
