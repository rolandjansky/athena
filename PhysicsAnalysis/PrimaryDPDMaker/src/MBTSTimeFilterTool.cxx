/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Author: Max Baak
// Merger of : TrigT2MinBias/src/T2MbtsFex.cxx, TrigMinBiasNtuple/src/MbtsContainerNtComponent.cxx

#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "TileIdentifier/TileTBID.h"
#include "TileEvent/TileContainer.h"


MBTSTimeFilterTool::MBTSTimeFilterTool( const std::string& type, const std::string& name, const IInterface* parent ) 
 : AthAlgTool( type, name, parent )
 , m_warningPrinted(false)
{
  declareInterface<MBTSTimeFilterTool>( this );

  declareProperty( "MBTSContainerName", m_mbtsContainerName = "MBTSContainer" );
  declareProperty( "ChargeThreshold",   m_chargethreshold = 60.0/222.0 );  // Value in pC, from T2MbtsFex.cxx
  declareProperty( "MinHitsPerSide",    m_minhitsperside = 2 ); 
  declareProperty( "MaxTimeDifference", m_maxtimediff = 10.0 ); 
}


MBTSTimeFilterTool::~MBTSTimeFilterTool()
{
}


StatusCode
MBTSTimeFilterTool::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("initialize()");

  // Get identifier for the MBTS
  sc = detStore()->retrieve(m_tileTBID);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Unable to retrieve TileTBID helper from DetectorStore.");
    return sc;
  }

  ATH_MSG_DEBUG ("initialize() successful");

  return sc;
}


StatusCode
MBTSTimeFilterTool::getTimeDifference(bool& passCut, double& timeDiff, double& timeA, double&timeC, int& countA, int& countC)
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("execute()");

  const TileCellContainer *tileCellCnt = 0;

  sc = evtStore()->retrieve(tileCellCnt, m_mbtsContainerName);
  if( sc.isFailure()  || !tileCellCnt ) {
    if (!m_warningPrinted) {
      ATH_MSG_WARNING (m_mbtsContainerName <<" requested but not found.");
      m_warningPrinted = true;
    }
    return StatusCode::SUCCESS;
  }

  timeA = 0.;
  timeC = 0.;
  countA = 0;
  countC = 0;
  double charge = 0;
  double eta = 0.;

  TileCellContainer::const_iterator itr = tileCellCnt->begin();
  TileCellContainer::const_iterator itr_end = tileCellCnt->end();

  int count=0;
  for(; itr != itr_end; ++itr, ++count) {

    // check charge passes threshold
    charge = (*itr)->energy();
    ATH_MSG_DEBUG ("Energy =" << charge << "pC");
    if(charge < m_chargethreshold) continue;

    // only endcap 
    eta = (*itr)->eta();
    ATH_MSG_DEBUG ("Eta =" << eta);
    if ( std::fabs(eta) < 1.5 ) continue;

    Identifier id=(*itr)->ID();
    // cache type, module and channel
    // MBTS Id type is  "side"  +/- 1
    int type_id = m_tileTBID->type(id);
    // MBTS Id module is "phi"  0-7
    int module_id = m_tileTBID->module(id);
    // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
    int channel_id = m_tileTBID->channel(id);

    // Catch errors
    if( abs(type_id) != 1 ){
      ATH_MSG_WARNING ("MBTS identifier type is out of range.");
      continue;
    }
    if( channel_id < 0 || channel_id > 1 ){
      ATH_MSG_WARNING ("MBTS identifier channel is out of range.");
      continue;
    }
    if( module_id < 0 || module_id > 7 ){
      ATH_MSG_WARNING ("MBTS identifier module is out of range.");
      continue;
    }

    if (type_id > 0)  {
      timeA +=  (*itr)->time();
      countA++ ; 
    } else {     
      timeC +=  (*itr)->time();
      countC++;
    }
  }

  if(count>32)
    ATH_MSG_WARNING ("There shoule be <=32 MBTS TileCells in " << m_mbtsContainerName << ".  " << count << " were found.");

  if ( countA > 0 ) timeA = timeA/((double)(countA));
  if ( countC > 0 ) timeC = timeC/((double)(countC));

  timeDiff = 999.; // Or whatever default you want
  if ( countA >= m_minhitsperside && countC >= m_minhitsperside ) { timeDiff = (timeA - timeC); }
  passCut = ( std::fabs(timeDiff) < m_maxtimediff);

  ATH_MSG_DEBUG ("execute() successful");

  return sc;
}


StatusCode
MBTSTimeFilterTool::finalize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_DEBUG ("finalize() successful");

  return sc;
}

