/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file CaloTools/src/CaloEstimatedGainTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Estimate gain used to read out a certain energy.
 *
 * Used to be part of ICaloNoiseTool.
 */


#include "CaloEstimatedGainTool.h"
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCell.h"
#include "TileConditions/TileInfo.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "StoreGate/ReadCondHandle.h"


StatusCode CaloEstimatedGainTool::initialize()
{
  m_LowGainThresh[CaloCell_ID::LAREM]    = 3900;//ADC counts in MediumGain 
  m_HighGainThresh[CaloCell_ID::LAREM]   = 1300;//ADC counts in MediumGain
  m_LowGainThresh[CaloCell_ID::LARHEC]   = 2500;//ADC counts in MediumGain 
  m_HighGainThresh[CaloCell_ID::LARHEC]  = 0;//-> high-gain never used for HEC
  m_LowGainThresh[CaloCell_ID::LARFCAL]  = 2000.;//ADC counts
  m_HighGainThresh[CaloCell_ID::LARFCAL] = 1100.;//ADC counts
  m_LowGainThresh[CaloCell_ID::TILE]  = 0.;// unit ?
  m_HighGainThresh[CaloCell_ID::TILE] = 0.;//

  const CaloCell_ID* calocell_id;
  ATH_CHECK( detStore()->retrieve (calocell_id, "CaloCell_ID") );
  m_lar_em_id   = calocell_id->em_idHelper();

  ATH_CHECK( detStore()->retrieve (m_tileInfo, "TileInfo") );

  ATH_CHECK( m_tileIdTransforms.retrieve() );
  ATH_CHECK( m_tileToolEmscale.retrieve() );
  ATH_CHECK( m_tileToolNoiseSample.retrieve() );

  ATH_CHECK( m_adc2mevKey.initialize() );

  return StatusCode::SUCCESS;
}


CaloGain::CaloGain 
CaloEstimatedGainTool::estimatedGain (const EventContext& ctx,
                                      const CaloCell& caloCell,
                                      const Step step) const
{
  return estimatedGain (ctx, caloCell, *caloCell.caloDDE(), step);
}


CaloGain::CaloGain
CaloEstimatedGainTool::estimatedGain (const EventContext& ctx,
                                      const CaloCell& caloCell, 
                                      const CaloDetDescrElement& caloDDE,
                                      const Step step) const
{
  CaloCell_ID::SUBCALO iCalo = caloDDE.getSubCalo();

  if (iCalo == CaloCell_ID::LAREM || 
      iCalo == CaloCell_ID::LARHEC || 
      iCalo == CaloCell_ID::LARFCAL)
  {
    return estimatedLArGain (ctx, iCalo, caloDDE, caloCell.energy(), step);
  }
  else if (iCalo == CaloCell_ID::TILE)
  {
    return estimatedTileGain (ctx, caloCell, caloDDE, step);
  }
  else
  {
    ATH_MSG_WARNING("CaloNoiseTool::estimatedGain  wrong id ! " 
                    << m_lar_em_id->show_to_string (caloDDE.identify()) );
    return CaloGain::INVALIDGAIN;
  } 
}


CaloGain::CaloGain 
CaloEstimatedGainTool::estimatedGain (const EventContext& ctx,
                                      const CaloDetDescrElement& caloDDE,
                                      const float energy,
                                      const Step step) const
{
  CaloCell_ID::SUBCALO iCalo = caloDDE.getSubCalo();

  if (iCalo == CaloCell_ID::LAREM || 
      iCalo == CaloCell_ID::LARHEC || 
      iCalo == CaloCell_ID::LARFCAL)
  {
    return estimatedLArGain (ctx, iCalo, caloDDE, energy, step);
  }
  else if (iCalo == CaloCell_ID::TILE)
  {
    ATH_MSG_WARNING("CaloNoiseTool::estimatedGain  NOT IMPLEMENTED FOR TILE "
                    <<"with these arguments! " 
                    << m_lar_em_id->show_to_string (caloDDE.identify()) );
    return CaloGain::INVALIDGAIN;
  } 
  else
  {
    ATH_MSG_WARNING("CaloNoiseTool::estimatedGain  wrong id ! " 
                    << m_lar_em_id->show_to_string (caloDDE.identify()) );
    return CaloGain::INVALIDGAIN;
  } 
}


CaloGain::CaloGain 
CaloEstimatedGainTool::estimatedLArGain (const EventContext& ctx,
                                         const CaloCell_ID::SUBCALO iCalo,
                                         const CaloDetDescrElement& caloDDE,
                                         const float energy, 
                                         const Step step) const
{
  //returns the gain of a cell according to the energy 
  //(converts in ADC counts in MediumGain and uses thresholds in ADC counts 
  //to determine the gain)

  //  the function could be called before (step=0, RawChannels) 
  //  or after  (step=1, Cells) LArG3Escale

  CaloGain::CaloGain igain=CaloGain::INVALIDGAIN; 

  //--EM/HEC--
  if( iCalo == CaloCell_ID::LAREM ||
      iCalo == CaloCell_ID::LARHEC ||
      iCalo == CaloCell_ID::LARFCAL )
  {     
    //We choose the gain in applying thresholds on the energy 
    //converted in ADC counts in MediumGain (index "1" of adc2mev(id,1)).
    //Indeed, thresholds in ADC counts are defined with respect 
    //to the MediumGain.
    //
    //              1300              3900 
    // ---------------|----------------|-------------> ADC counts in MediumGain
    //    HighGain  <---  MediumGain  --->  LowGain

    float adc=0.;
    
    //Cells (with E scale and weights from LArG3Escale)
    if (step == Step::CELLS) 
      adc = energy / (adc2mev (ctx, caloDDE, CaloGain::LARMEDIUMGAIN)) + 1000;
    //RawChannels    
    else if (step == Step::RAWCHANNELS)
    {
      adc = energy / (adc2mev (ctx, caloDDE, CaloGain::LARMEDIUMGAIN)) + 1000;
    }
    else 
    {
      ATH_MSG_WARNING( "CaloNoiseTool::estimatedGain   wrong step" );
    }  

    if (adc < m_HighGainThresh[iCalo])     igain = CaloGain::LARHIGHGAIN;  
    else if (adc > m_LowGainThresh[iCalo]) igain = CaloGain::LARLOWGAIN; 
    else                                   igain = CaloGain::LARMEDIUMGAIN;  
  }

  return igain;
}


CaloGain::CaloGain
CaloEstimatedGainTool::estimatedTileGain(const EventContext& ctx,
                                         const CaloCell& caloCell,
                                         const CaloDetDescrElement& caloDDE,
                                         const Step /*step*/) const
{
  const TileCell& tileCell = dynamic_cast<const TileCell&> (caloCell);

  // threshold (1023 counts) is the same for all channels
  double threshold = m_tileInfo->ADCmax();

  // xxx
  static TileHWID const * const tileHWID = TileCablingService::getInstance()->getTileHWID();
  static const IdContext chContext = tileHWID->channel_context();
  
  HWIdentifier hwid1;
  tileHWID->get_id (caloDDE.onl1(), hwid1, &chContext ); // pmt id
  hwid1 = tileHWID->adc_id (hwid1, TileHWID::HIGHGAIN); // high gain ADC id
  
  unsigned int drawerIdx1(0), channel1(0), adc1(0);
  m_tileIdTransforms->getIndices (hwid1, drawerIdx1, channel1, adc1);

  // first PMT, convert energy to ADC counts
  double amplitude1 = tileCell.ene1();
  amplitude1 /= m_tileToolEmscale->channelCalib (drawerIdx1, channel1, adc1, 1.0,
                                                 TileRawChannelUnit::ADCcounts,
                                                 TileRawChannelUnit::MegaElectronVolts);

  double pedestal1 = m_tileToolNoiseSample->getPed (drawerIdx1, channel1, adc1, TileRawChannelUnit::ADCcounts, ctx);

  int igain1;

  if (amplitude1 + pedestal1 < threshold ) {
    igain1 = TileID::HIGHGAIN;
  } else {
    igain1 = TileID::LOWGAIN;
  }

  // second PMT, if it exists
  if (caloDDE.onl2() !=  TileID::NOT_VALID_HASH ) {

    HWIdentifier hwid2;
    tileHWID->get_id( caloDDE.onl2(), hwid2, &chContext ); // pmt id
    hwid2 = tileHWID->adc_id (hwid2, TileHWID::HIGHGAIN); // high gain ADC id

    unsigned int drawerIdx2(0), channel2(0), adc2(0);
    m_tileIdTransforms->getIndices (hwid2, drawerIdx2, channel2, adc2);

    // first PMT, convert energy to ADC counts
    double amplitude2 = tileCell.ene2();
    amplitude2 /= m_tileToolEmscale->channelCalib (drawerIdx2, channel2, adc2, 1.0,
                                                   TileRawChannelUnit::ADCcounts,
                                                   TileRawChannelUnit::MegaElectronVolts);

    double pedestal2 = m_tileToolNoiseSample->getPed (drawerIdx2, channel2, adc2, TileRawChannelUnit::ADCcounts, ctx);

    if (amplitude2 + pedestal2 < threshold) {
      // igain2 high
      return igain1 == TileID::LOWGAIN ? CaloGain::TILEHIGHLOW : CaloGain::TILEHIGHHIGH;
    } else {
      // igain2 low
      return igain1 == TileID::LOWGAIN ? CaloGain::TILELOWLOW : CaloGain::TILEHIGHLOW;
    }
    // nb. Having HIGHLOW in two places seems wrong, but that's what the
    // original code was doing.
  } 

  // igain2 doesn't exist.
  return igain1 == TileID::LOWGAIN ? CaloGain::TILEONELOW : CaloGain::TILEONEHIGH;
}


float CaloEstimatedGainTool::adc2mev (const EventContext& ctx,
                                      const CaloDetDescrElement& caloDDE,
                                      CaloGain::CaloGain gain) const
{
  SG::ReadCondHandle adc2mev (m_adc2mevKey, ctx);
  const auto& p = adc2mev->ADC2MEV (caloDDE.identify(), gain);
  if (p.size() >= 2) {
    return p[1];
  }
  return 0;
}
