///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventDensityTool.cxx 
// Implementation file for class EventDensityTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// EventShapeTools includes
#include "EventShapeTools/EventDensityTool.h"

// STL includes

// StoreGate
//#include "StoreGate/StoreGateSvc.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"

#include "xAODEventShape/EventShape.h"
#include "xAODEventShape/EventShapeAuxInfo.h"

#include <tuple>

// xAOD::EventShape::EventDensityID idFromFastJetDef(const fastjet::JetDefinition* jdef,  const fastjet::AreaDefinition* areadef){
  
//   typedef std::tuple<fastjet::JetAlgorithm, double, fastjet::AreaType>  fjdef_t;
//   typedef std::map< fjdef_t , xAOD::EventShape::EventDensityID>         defmap_t;

//   static defmap_t defMap = {
//     { fjdef_t( fastjet::kt_algorithm, 0.4, fastjet::voronoi_area ),  xAOD::EventShape::Kt4Voronoi  },
//     { fjdef_t( fastjet::kt_algorithm, 0.6, fastjet::voronoi_area ),  xAOD::EventShape::Kt6Voronoi  }
//   };

//   defmap_t::iterator it = defMap.find( fjdef_t(jdef->jet_algorithm() , jdef->R() , areadef->area_type() ) );

//   if( it == defMap.end() ) return xAOD::EventShape::UnknownDensity;

//   return it->second;


// }

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
EventDensityTool::EventDensityTool( const std::string& name ) :
  asg::AsgTool(name)
  , m_pjGetter()
  , m_fastJetTool(this)
  , m_evtShapeName("")
  , m_etaRange()
  , m_useAbsEta(true)
  , m_rhoDec  ("")
  , m_sigmaDec("")
  , m_areaDec ("")

{
  //declareInterface< IEventShapeTool >(this);
  //
  // Property declaration
  // 
  declareProperty( "PseudoJetGetter", m_pjGetter );
  declareProperty( "FastJetTool", m_fastJetTool );
  declareProperty( "EventShapeName", m_evtShapeName );

  declareProperty( "EtaRange",m_etaRange);
  declareProperty( "UseAbsoluteEta",m_useAbsEta);
  declareProperty( "UseAreaFourMomentum", m_useAreaFourMom);

  declareProperty("VariableName", m_varBaseName);
}

// Destructor
///////////////
EventDensityTool::~EventDensityTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode EventDensityTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK( m_pjGetter.retrieve() );
  ATH_CHECK( m_fastJetTool.retrieve() );

  const fastjet::AreaDefinition * area = m_fastJetTool->getAreaDefinition();
  if(area == 0 ) {
    ATH_MSG_ERROR(" FastJet MUST be defined with area calculation on !");
    return StatusCode::FAILURE;
  }


  // // get the id under which we'll store the density in the EventShape object
  // // WARNING !! this does ignore the eta range !! TO BE CHANGED !
  // m_shapeId = idFromFastJetDef( m_fastJetTool->getJetDefinition(), area );

  // if( m_shapeId == xAOD::EventShape::UnknownDensity) {
  //   ATH_MSG_ERROR("Unknonw area and jet specification : R="<< m_fastJetTool->getJetDefinition()->R() 
  //                 << " area ="<<m_fastJetTool->getAreaDefinitionType());
  //   return StatusCode::FAILURE;    
  // }

  if(m_etaRange.empty() ) {m_etaRange.push_back(-2.);m_etaRange.push_back(2.);}

  // Fill the EventShape object
  m_rhoDec   = SG::AuxElement::Decorator< float > ( m_varBaseName );
  m_sigmaDec = SG::AuxElement::Decorator< float > ( m_varBaseName+"Sigma" );
  m_areaDec  = SG::AuxElement::Decorator< float > ( m_varBaseName+"Area" );


  return StatusCode::SUCCESS;
}





StatusCode EventDensityTool::fillEventShape() const {
  
  ATH_MSG_DEBUG("fillEventShape() ");

  const xAOD::EventShape * evtShape;
  if( evtStore()->contains<xAOD::EventShape>(m_evtShapeName) ){
    ATH_MSG_DEBUG("fillEventShape()  : evtStore contains "<< m_evtShapeName);
    // need to const_cast as long as we don't have a policy for decoration
    //    evtShape = const_cast<xAOD::EventShape*>(evtStore()->retrieve<xAOD::EventShape>(m_evtShapeName) );
    ATH_CHECK(evtStore()->retrieve<xAOD::EventShape>(evtShape, m_evtShapeName) );
  }else {
    // build and record
    xAOD::EventShape *evtShape_tmp = new xAOD::EventShape();
    xAOD::EventShapeAuxInfo* aux = new xAOD::EventShapeAuxInfo();
    evtShape_tmp->setStore( aux );
    ATH_CHECK( evtStore()->record( aux, m_evtShapeName + "Aux." ) );
    ATH_CHECK( evtStore()->record( evtShape_tmp, m_evtShapeName ) );
    ATH_MSG_DEBUG(" Built a new EventShape object "<< m_evtShapeName);
    evtShape = evtShape_tmp;
  }

  ATH_MSG_DEBUG("fillEventShape() -> evtShape object : "<< m_evtShapeName);

  const PseudoJetVector * pjVect = m_pjGetter->get();

  if(!pjVect) {
    ATH_MSG_ERROR( "Couldn't retrieve pseudojets from "<<m_pjGetter->name() );
    return StatusCode::FAILURE;
  }
  
  return fillEventShape( *pjVect, evtShape);
}


StatusCode EventDensityTool::fillEventShape(const xAOD::IParticleContainer*, const xAOD::EventShape *evtShape) const {
 
  const PseudoJetVector * pjVect = m_pjGetter->get();
 
  if(!pjVect) {
    ATH_MSG_ERROR( "Couldn't retrieve pseudojets from "<<m_pjGetter->name() );
    return StatusCode::FAILURE;
    /// TODO : instead build a PseudoJetVector from the input IParticleContainer
  }
  
  return fillEventShape( *pjVect, evtShape);
}
  



StatusCode EventDensityTool::fillEventShape(const PseudoJetVector &pjVect, const xAOD::EventShape *evtShape) const {

  ATH_MSG_DEBUG("fillEventShape(PseudoJetVector, Evtshape) " << &pjVect << " , "<< evtShape);
  PseudoJetVector jets;
  ATH_CHECK( m_fastJetTool->execute( pjVect, jets) );
  
  
  /// guaranteed valid pointer since checked in initialize()
  const fastjet::ClusterSequenceArea* pSeq = 
    dynamic_cast<const fastjet::ClusterSequenceArea*>(m_fastJetTool->clusterSequence());

  ATH_MSG_DEBUG("    fillEventShape : built sequence  "<< pSeq);
  if(! pjVect.empty() ) ATH_MSG_DEBUG("calculating rho ... InputCluster " << pjVect.size() << "  pt= "<< pjVect.front().pt() << "  "<< pjVect.back().pt());

  fastjet::Selector sel; 
  if(m_useAbsEta)
    sel  = fastjet::SelectorAbsRapRange(m_etaRange[0], m_etaRange[1] );
  else
    sel  = fastjet::SelectorRapRange(m_etaRange[0], m_etaRange[1] );


  double rho, sigma, area;
  pSeq->get_median_rho_and_sigma(sel,
                                 m_useAreaFourMom,
                                 rho,
                                 sigma,
                                 area);

  
  if( ! m_rhoDec.isAvailable(*evtShape) ) {
    // write only if present
    m_rhoDec( *evtShape ) = rho;
    m_sigmaDec( *evtShape ) = sigma;
    m_areaDec( *evtShape ) = area;
    ATH_MSG_DEBUG(" Filled EventShape object with "<< m_varBaseName << " = "<<rho);
  }else {
    ATH_MSG_WARNING(" EventShape object  "<< m_varBaseName << " already existing ! not overwriting !");
  }

  // typedef xAOD::EventShape::EventDensityID DensityID;
  // bool setOk = true;
  // setOk = setOk && evtShape->setDensity( m_shapeId, rho );
  // setOk = setOk && evtShape->setDensity( (DensityID) (m_shapeId+1), sigma);
  // setOk = setOk && evtShape->setDensity( (DensityID) (m_shapeId+2), area);
  // if(setOk){
  //   ATH_MSG_DEBUG(" Filled EventShape object "<< rho);
  //   return StatusCode::SUCCESS;
  // }
  // ATH_MSG_ERROR( "Could not set one ")
  return StatusCode::SUCCESS;
}

