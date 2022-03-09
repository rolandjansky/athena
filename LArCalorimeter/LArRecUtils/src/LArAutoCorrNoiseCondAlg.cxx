/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArAutoCorrNoiseCondAlg.h"

#include "LArElecCalib/LArConditionsException.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "GaudiKernel/EventIDRange.h"

LArAutoCorrNoiseCondAlg::LArAutoCorrNoiseCondAlg( const std::string& name, ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator )
    , m_LArOnOffIdMappingObjKey( "LArOnOffIdMap" )
    , m_LArMCSymObjKey( "LArMCSym" )
    , m_LArAutoCorrObjKey( "LArAutoCorrSym" )
    , m_LArAutoCorrNoiseObjKey( "LArAutoCorrNoise" )
    , m_isSuperCell( false )
    , m_MCsym( true )
    , m_nSampl( 4 )
{
  declareProperty( "LArOnOffIdMappingObjKey", m_LArOnOffIdMappingObjKey, "Key to read LArOnOffIdMapping object" );
  declareProperty( "LArMCSymObjKey", m_LArMCSymObjKey, "Key to read LArMCSym object" );
  declareProperty( "LArAutoCorrObjKey", m_LArAutoCorrObjKey, "Key to read LArAutoCorr object" );
  declareProperty( "LArAutoCorrNoiseObjKey", m_LArAutoCorrNoiseObjKey, "Key to write LArAutoCorrNoise object" );
  declareProperty( "isSuperCell", m_isSuperCell );
  declareProperty( "MCsym", m_MCsym );
  declareProperty( "nSampl", m_nSampl );
}

LArAutoCorrNoiseCondAlg::~LArAutoCorrNoiseCondAlg() {}

StatusCode LArAutoCorrNoiseCondAlg::initialize()
{
  ATH_MSG_DEBUG( "initialize " << name() );

  // ReadCondHandle initialization
  ATH_CHECK( m_LArOnOffIdMappingObjKey.initialize() );
  ATH_CHECK( m_LArMCSymObjKey.initialize() );
  ATH_CHECK( m_LArAutoCorrObjKey.initialize() );

  ATH_CHECK( m_LArAutoCorrNoiseObjKey.initialize() );

  // Number of gains
  m_nGains = ( m_isSuperCell ) ? 1 : 3;

  return StatusCode::SUCCESS;
}

StatusCode LArAutoCorrNoiseCondAlg::execute()
{

  // WriteHandle setup
  SG::WriteCondHandle<LArAutoCorrNoise> writeHandle( m_LArAutoCorrNoiseObjKey );
  if ( writeHandle.isValid() ) {
    ATH_MSG_DEBUG( "CondHandle " << writeHandle.fullKey() << " is already valid." );
    return StatusCode::SUCCESS;
  }

  // Identifier helper
  const LArOnlineID_Base* larOnlineID = nullptr;
  if ( m_isSuperCell ) {
    const LArOnline_SuperCellID* scidhelper;
    ATH_CHECK( detStore()->retrieve( scidhelper, "LArOnline_SuperCellID" ) );
    larOnlineID = scidhelper; // cast to base-class
  } else {                    // regular cells
    const LArOnlineID* idhelper;
    ATH_CHECK( detStore()->retrieve( idhelper, "LArOnlineID" ) );
    larOnlineID = idhelper; // cast to base-class
  }
  // Mapping helper
  const LArOnOffIdMapping*              larOnOffIdMapping = nullptr;
  SG::ReadCondHandle<LArOnOffIdMapping> larOnOffIdMappingHdl{m_LArOnOffIdMappingObjKey};
  larOnOffIdMapping = *larOnOffIdMappingHdl;
  if ( larOnOffIdMapping == nullptr ) {
    ATH_MSG_ERROR( "Failed to retrieve LArOnOffIdMapping object" );
  }
  // MC symmetrization helper
  const LArMCSym*              larMCsym = nullptr;
  if ( !m_isSuperCell && m_MCsym ) {
  SG::ReadCondHandle<LArMCSym> larMCsymHdl{m_LArMCSymObjKey};
  larMCsym = *larMCsymHdl;
  if ( larMCsym == nullptr ) {
    ATH_MSG_ERROR( "Failed to retrieve LArMCSym object" );
    return StatusCode::FAILURE;
  }
  }

  // Get pointers to inputs
  // Retrieve validity ranges and determine their intersection
  EventIDRange rangeAutoCorr;

  SG::ReadCondHandle<ILArAutoCorr> AutoCorrHdl{m_LArAutoCorrObjKey};
  const ILArAutoCorr*              larAutoCorr{*AutoCorrHdl};
  if ( !AutoCorrHdl.range( rangeAutoCorr ) ) {
    ATH_MSG_ERROR( "Failed to retrieve validity range for " << AutoCorrHdl.key() );
  }

  std::vector<std::map<HWIdentifier, std::vector<float>>> terms( m_nGains );

  std::vector<HWIdentifier>::const_iterator it, it_e;
  if ( !m_isSuperCell && m_MCsym ) {
    it   = larMCsym->symIds().begin();
    it_e = larMCsym->symIds().end();
  } else {
    it   = larOnlineID->channel_begin();
    it_e = larOnlineID->channel_end();
  }

  int count{0};

  for ( ; it != it_e; ++it ) {
    count++;
    const HWIdentifier id = *it;

    if ( larOnOffIdMapping->isOnlineConnected( id ) ) {
      // the Shape is a function of gain
      for ( unsigned int igain = 0; igain < m_nGains; igain++ ) {
        const ILArAutoCorr::AutoCorrRef_t AC = larAutoCorr->autoCorr( id, igain );

        std::vector<float> vTerms;
        const int          size = m_nSampl * m_nSampl;
        vTerms.resize( size );

        // compute sqrt to covariance matrix
        for ( int j = 0; j < m_nSampl; j++ ) {
          // a) diagonal terms
          double ck = 0.;
          for ( int k = 0; k <= j - 1; k++ ) {
            int index = j * m_nSampl + k;
            ck        = ck + vTerms[index] * vTerms[index];
          }
          //     diagonal term of autocorr matrix = 1
          vTerms[j * m_nSampl + j] = sqrt( std::fabs( 1. - ck ) );
          // b) off-diagonal terms
          for ( int i = j + 1; i < m_nSampl; i++ ) {
            double ck = 0;
            for ( int k = 0; k <= j - 1; k++ ) {
              int index1 = i * m_nSampl + k;
              int index2 = j * m_nSampl + k;
              ck         = ck + vTerms[index1] * vTerms[index2];
            }
            int          index    = i * m_nSampl + j;
            int          index2   = j * m_nSampl + j;
            unsigned int index3   = ( i - j ) - 1;
            double       AutoCorr = 0.;
            if ( index3 < ( AC.size() ) ) AutoCorr = AC[index3];
            vTerms[index] = ( AutoCorr - ck ) / vTerms[index2];
          }
        }

        terms[igain][id] = vTerms;

      } //(loop on gains)

    } else // unconnected
      for ( unsigned int igain = 0; igain < m_nGains; igain++ ) {
        terms[igain][id] = std::vector<float>( 1, 0. );
      }
  }

  ATH_MSG_INFO( "LArAutoCorrNoise: Ncell " << ( ( m_MCsym ) ? "w/ MCsym" : "w/o MCsym: " ) << count );
  ATH_MSG_DEBUG( "LArAutoCorrNoise: end of loop over cells" );

  // Make output object
  std::unique_ptr<LArAutoCorrNoise> larAutoCorrNoise =
      std::make_unique<LArAutoCorrNoise>( std::move( terms ), larMCsym );

  // Write
  ATH_CHECK( writeHandle.record( rangeAutoCorr, larAutoCorrNoise.release() ) );

  return StatusCode::SUCCESS;
}
