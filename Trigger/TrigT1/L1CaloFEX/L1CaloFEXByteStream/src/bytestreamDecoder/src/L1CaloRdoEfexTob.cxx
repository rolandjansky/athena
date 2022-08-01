/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "bytestreamDecoder/L1CaloRdoEfexTob.h"

#include "channelMappings/EfexCellMapping.h"

#include <iomanip>
#include <iostream>

//>>#include "channelMappings/CpmTowerMapping.h"   // Use Efex mapping!!

/*!
  \class L1CaloRdoEfexTob
  L1CaloRdoFexTob subclass for eFEX EM/Tau TOBs or XTOBs.
  These may come from eFEX or Ph1Topo inputs.
  The available flags are different in these two cases.
  The RDO object has two 32 bit words: value and flag.
  The TOBs and XTOBs differ mainly in the number of bits
  used for the Et value (12 vs 16). We store the Et in
  the value word and everything else in the flag keeping
  all the fields in their location in the hardware format.
  
  The id, fibre and tobSeq values were used in Run 2 for topo
  and for distinguishing different sources of the same TOB
  (eg here from FEX readout vs Ph1Topo readout perhaps).
 */


L1CaloRdoEfexTob::L1CaloRdoEfexTob( int crate, int module, int eta, int phi, int numSlices,
                                        TobType tobType, TobSource source, int id, int fibre, int tobSeq ) 
: L1CaloRdoFexTob( crate, module, eta, phi, numSlices, tobType, source, id, fibre, tobSeq )
{
   int fpga = (eta-1)/4;
   if ( fpga < 0 ) 
      fpga = 0;
   if ( fpga > 3 )
      fpga = 3;
   int localeta = eta-4*fpga;
   EfexCellMapping::roiType roi = EfexCellMapping::Roi;
   EfexCellMapping map( roi, crate, module, fpga, localeta, phi );
   setRegion( map.getDetectorRegion() );
}

void
L1CaloRdoEfexTob::infoSpecific( ) const
{
   std::cout << "      Isolation:  " << std::hex;
   for ( size_t s = 0 ; s < numSlices() ; ++s )
   {
      std::cout << std::setw(2) << getIsol( s ) << " ";
   }
   std::cout << std::dec << std::endl;
}

/*
bool 
L1CaloRdoEfexTob::getOverflow( size_t slice ) const
{
   return ( getFlag( slice ) & 0x10 );  // **FIXME**
}

bool 
L1CaloRdoEfexTob::getOverflow( ) const
{
   return getOverflow( getL1aPos() );
}
*/

/*!
  Return 12 bit (TOB) or 16 bit (XTOB) cluster Et (from the RDO value).
 */
int 
L1CaloRdoEfexTob::getClusterEt( size_t slice ) const
{
   return ( getValue( slice ) & 0xffff );
}

int 
L1CaloRdoEfexTob::getClusterEt( ) const
{
   return getClusterEt( getL1aPos() );
}

/*!
  Return three 2 bit isolation hits (packed into bits 18-23 of the RDO flag).
 */
int 
L1CaloRdoEfexTob::getIsol( size_t slice ) const
{
   return ( ( getFlag( slice ) >> 18 ) & 0x3f );
}

int 
L1CaloRdoEfexTob::getIsol( ) const
{
   return getIsol( getL1aPos() );
}
