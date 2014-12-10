/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ObjFillerTool.cxx 631247 2014-11-26 16:25:26Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/ObjFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Object filler tool for a single object.
 */


#include "ObjFillerTool.h"
#include "GaudiKernel/System.h"
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
ObjFillerTool::ObjFillerTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent)
  : base_class (type, name, parent),
    AddVariable (m_prefix, m_blockName),
    m_getter (this),
    m_blockFillers (this),
    m_tree (0)
{
  declareProperty ("Getter",       m_getter,
                   "The IObjGetterTool instance.");
  declareProperty ("BlockFillers", m_blockFillers,
                   "List of contained block filler tools.");
  declareProperty ("Prefix",       m_prefix = "",
                   "Variable name prefix for the contained blocks.");
  declareProperty ("BlockName", m_blockName = this->name(),
                   "The name of this block.");
  declareProperty ("ObjectName", m_objectName = "",
                   "Name of the D3PDObject that created this filler.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, then it is not considered an error for the "
                   "requested input object to be missing.");
  declareProperty ("SaveMetadata", m_saveMetadata = false,
                   "Set to true to get metadata into the output D3PD "
                   "about the variables created by this tool");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode
ObjFillerTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_getter.retrieve() );
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
ObjFillerTool::configureD3PD (IAddVariable* tree)
{
  // The type that the getter will provide.
  const std::type_info& ti = m_getter->typeinfo();

  CHECK( AddVariable::configureD3PD (tree) );
  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->configureD3PD (this, ti) );

  // Try to cast the received object into an ID3PD one:
  m_tree = dynamic_cast< ID3PD* >( tree );

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode ObjFillerTool::book()
{
  // Be sure aux variables are defined.
  (void)m_getter->getUntyped (m_allowMissing);

  // Configure the metadata object correctly:
  m_metadata.setPrefix( m_prefix );
  m_metadata.setName( m_objectName );
  m_metadata.setContainer( isContainerFiller() );

  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->book() );
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one object.
 *
 * This is called once per event.  It should:
 *   - Fetch the input.
 *   - If it's a container, loop over the contents.
 *   - Loop over the contained block filler tools.
 */ 
StatusCode
ObjFillerTool::fill()
{
  const void* obj = m_getter->getUntyped (m_allowMissing);
  if (!obj) {
    if (m_allowMissing)
      return StatusCode::SUCCESS;
    REPORT_MESSAGE(MSG::ERROR) << "Can't get input object.";
    return StatusCode::FAILURE;
  }

  for (size_t i = 0; i < m_blockFillers.size(); i++)
    CHECK( m_blockFillers[i]->fillUntyped (obj) );

  m_getter->releaseObjectUntyped (obj);

  return StatusCode::SUCCESS;
}


/**
 * @brief Find out if the filler handles containers or single objects
 *
 * This is used by the D3PDReader code generator to know how to
 * create the generated source files exactly.
 */
bool ObjFillerTool::isContainerFiller() const
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
void ObjFillerTool::handle( const Incident& inc )
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

