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
  ATH_CHECK( m_key.initialize() );

  // create a BeamSpotCondition from default values and store it

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

  if (!m_useDB) return StatusCode::SUCCESS;

  SG::ReadCondHandle<AthenaAttributeList> rawData { m_key };
  
  const AthenaAttributeList* raw { *rawData };
  if ( raw == nullptr )
  {
    ATH_MSG_ERROR("Beam Spot data for key " << m_key.fullKey() << " not found");
    return StatusCode::FAILURE;
  }

  int status { (*raw)["status"].data<int>() };
  float posX { (*raw)["posX"].data<float>() };
  float posY { (*raw)["posY"].data<float>() };
  float posZ { (*raw)["posZ"].data<float>() };
  float sigmaX { (*raw)["sigmaX"].data<float>() };
  float sigmaY { (*raw)["sigmaY"].data<float>() };
  float sigmaZ { (*raw)["sigmaZ"].data<float>() };
  float tiltX { (*raw)["tiltX"].data<float>() };
  float tiltY { (*raw)["tiltY"].data<float>() };
  float sigmaXY { m_sigmaXY };
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

  return StatusCode::SUCCESS;
}

