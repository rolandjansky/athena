// BeamSpot_ConditionsAlgs includes
#include "BeamSpotCondAlg.h"
#include "CoralBase/AttributeListException.h"

BeamSpotCondAlg::BeamSpotCondAlg( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm( name, pSvcLocator )
{ }


BeamSpotCondAlg::~BeamSpotCondAlg() {}


StatusCode BeamSpotCondAlg::initialize() 
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_condSvc.retrieve() );

  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  if ( m_condSvc->regHandle( this, m_writeKey ).isFailure() )
  {
    ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode BeamSpotCondAlg::finalize() 
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode BeamSpotCondAlg::execute() 
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::WriteCondHandle<InDet::BeamSpotData> writeHandle { m_writeKey };
  if ( writeHandle.isValid() )
  {
    writeHandle.updateStore();
    ATH_MSG_DEBUG( "CondHandle " << writeHandle.fullKey() << " is already valid " <<
		   "Forcing update of Store contents" );
    return StatusCode::SUCCESS;
  }

  SG::ReadCondHandle<AthenaAttributeList> readHandle { m_readKey };
  
  const AthenaAttributeList* raw { *readHandle };
  if ( raw == nullptr )
  {
    ATH_MSG_ERROR("Beam Spot data for key " << m_readKey.fullKey() << " not found");
    return StatusCode::FAILURE;
  }
  
  int   status  { m_status };
  float posX    { m_posX };
  float posY    { m_posY };
  float posZ    { m_posZ };
  float sigmaX  { m_sigmaX };
  float sigmaY  { m_sigmaY };
  float sigmaZ  { m_sigmaZ };
  float tiltX   { m_tiltX };
  float tiltY   { m_tiltY };
  float sigmaXY { m_sigmaXY };

  if (m_useDB)
  {
    status = (*raw)["status"].data<int>();
    posX = (*raw)["posX"].data<float>();
    posY = (*raw)["posY"].data<float>();
    posZ = (*raw)["posZ"].data<float>();
    sigmaX = (*raw)["sigmaX"].data<float>();
    sigmaY = (*raw)["sigmaY"].data<float>();
    sigmaZ = (*raw)["sigmaZ"].data<float>();
    tiltX = (*raw)["tiltX"].data<float>();
    tiltY = (*raw)["tiltY"].data<float>();
    try
    {
      sigmaXY = (*raw)["sigmaXY"].data<float>();
    }
    catch (coral::AttributeListException& e)
    {
      ATH_MSG_DEBUG( "No sigmaXY value present in database, using default value of " << sigmaXY );
    }

    ATH_MSG_INFO( "Read from condDB"
	       << " status " << status
	       << " pos (" << posX << "," << posY << "," << posZ << ")"
	       << " sigma (" << sigmaX << "," << sigmaY << "," << sigmaZ << ")"
	       << " tilt (" << tiltX << "," << tiltY << ")"
	       << " sigmaXY " << sigmaXY );
  }

  EventIDRange rangeW;
  if ( !readHandle.range(rangeW) )
  {
    ATH_MSG_ERROR( "Failed to retrieve validity range for " << readHandle.key() );
    return StatusCode::FAILURE;
  }

  InDet::BeamSpotData* writeCdo = new InDet::BeamSpotData( status, posX, posY, posZ,
							   sigmaX, sigmaY, sigmaZ,
							   tiltX, tiltY, sigmaXY );
  
  if ( writeHandle.record( rangeW, writeCdo ).isFailure() )
  {
    ATH_MSG_ERROR( "Could not record InDet::BeamSpotData " << writeHandle.key() <<
		   " with EventRange " << rangeW << " into conditions store." );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "Recorded new InDet::BeamSpotData to " << writeHandle.key() << " with range " << rangeW <<
		" into conditions store." );

  return StatusCode::SUCCESS;
}

