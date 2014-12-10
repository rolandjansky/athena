/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VectorFillerTool.cxx 628034 2014-11-12 22:18:35Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/VectorFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Object filler tool for a collection of objects, saved as vectors.
 */


#include "VectorFillerTool.h"
#include "AthenaKernel/errorcheck.h"
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
VectorFillerTool::VectorFillerTool (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
  : base_class (type, name, parent),
    m_getter (this),
    m_tree (0)
{
  declareProperty ("Getter",       m_getter,
                   "The ICollectionGetterTool instance.");
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
VectorFillerTool::initialize()
{
  CHECK( VectorFillerToolBase::initialize() );

  // Make sure the block fillers are initialized before the getter,
  // so that it's more likely for the DataVector info to be available.
  CHECK( m_getter.retrieve() );

  // Listen to the EndEvtLoop incident:
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
VectorFillerTool::configureD3PD (IAddVariable* tree)
{
  CHECK( doConfigure (tree, *m_getter) );

  // Try to cast the received object into an ID3PD one:
  m_tree = dynamic_cast< ID3PD* >( tree );

  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode VectorFillerTool::book()
{
  // Be sure aux variables are defined.
  CHECK( m_getter->reset (m_allowMissing) );

  // Configure the metadata object correctly:
  m_metadata.setName( m_objectName );
  m_metadata.setPrefix( m_prefix );
  m_metadata.setContainer( isContainerFiller() );

  CHECK( doBook() );

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
VectorFillerTool::fill()
{
  CHECK( m_getter->reset (m_allowMissing) );
  size_t sz = m_getter->sizeHint (m_allowMissing);

  CHECK( doFill (*m_getter, sz) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Find out if the filler handles containers or single objects
 *
 * This is used by the D3PDReader code generator to know how to
 * create the generated source files exactly.
 */
bool VectorFillerTool::isContainerFiller() const
{
   return true;
}


/**
 * The metadata saving is now done when seeing an end of run incident.
 * This is needed because of MC11b...
 *
 * Basically, we can't do this in finalize(), because the output TTree is
 * no longer accessible by then. So it has to be done just before the
 * event loop is finished.
 */
void VectorFillerTool::handle( const Incident& inc )
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
