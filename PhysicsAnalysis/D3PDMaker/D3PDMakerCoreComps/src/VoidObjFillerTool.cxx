/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VoidObjFillerTool.cxx 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file   D3PDMakerCoreComps/src/VoidObjFillerTool.cxx
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date   Aug, 2009
 * @brief   Object filler tool for tools taking no input.
 */


#include "VoidObjFillerTool.h"
#include "D3PDMakerInterfaces/ID3PD.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
VoidObjFillerTool::VoidObjFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent
				      )
  : base_class (type, name, parent),
    AddVariable (m_prefix, m_blockName),
    m_blockFillers (this),
    m_tree (0)
{
  declareProperty ("BlockFillers", m_blockFillers,
                   "List of contained block filler tools.");
  declareProperty ("Prefix",       m_prefix = "",
                   "Variable name prefix for the contained blocks.");
  declareProperty ("BlockName", m_blockName = this->name(),
                   "The name of this block.");
  declareProperty ("ObjectName", m_objectName = "",
                   "Name of the D3PDObject that created this filler.");
  declareProperty ("SaveMetadata", m_saveMetadata = false,
                   "Set to true to get metadata into the output D3PD "
                   "about the variables created by this tool");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
VoidObjFillerTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_blockFillers.retrieve() );

  // Listen to EndEvent and EndRun incidents:
  ServiceHandle< IIncidentSvc > p_incSvc( "IncidentSvc", name() );
  CHECK( p_incSvc.retrieve() );
  p_incSvc->addListener( this, "EndEvtLoop" );

  return StatusCode::SUCCESS;
}


/**
 * @brief Configure the tool.
 * @param tree The parent D3PD tree.
 *
 * This is called during initialization.
 */
StatusCode
VoidObjFillerTool::configureD3PD (IAddVariable* tree)
{
  CHECK( AddVariable::configureD3PD (tree) );
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->configureD3PD (this, typeid(void)) );

  // Try to cast the received object into an ID3PD one:
  m_tree = dynamic_cast< ID3PD* >( tree );

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode VoidObjFillerTool::book()
{
  // Configure the metadata object correctly:
  m_metadata.setName( m_objectName );
  m_metadata.setPrefix( m_prefix );
  m_metadata.setContainer( isContainerFiller() );

  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->book() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one object.
 *
 * This is called once per event.
 * It loops over the contained block filler tools.
 */ 
StatusCode
VoidObjFillerTool::fill()
{
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->fillUntyped (0) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Find out if the filler handles containers or single objects
 *
 * This is used by the D3PDReader code generator to know how to
 * create the generated source files exactly.
 */
bool VoidObjFillerTool::isContainerFiller() const
{
   return false;
}


/**
 * The metadata saving is now done when seeing an end of run incident.
 * This is needed because of MC11b...
 *
 * Basically, we can't do this in finalize(), because the output TTree is
 * no longer accessible by then. So it has to be done just before the
 * event loop is finished.
 */
void VoidObjFillerTool::handle( const Incident& inc )
{
  if( inc.type() == "EndEvtLoop" ) {
    if( m_tree && m_metadata.variables().size() && m_saveMetadata ) {
      const std::string metadata = m_metadata.toString();
      if( m_tree->addMetadata( m_metadata.metadataName(),
                               &metadata ).isFailure() ) {
        REPORT_MESSAGE( MSG::ERROR )
           << "Couldn't add object metadata information to the output!";
        return;
      }
    }
  }

  return;
}


} // namespace D3PD
