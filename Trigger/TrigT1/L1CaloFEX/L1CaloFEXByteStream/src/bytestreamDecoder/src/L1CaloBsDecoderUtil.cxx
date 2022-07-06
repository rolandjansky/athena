/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <iomanip>

#include "eformat/ROBFragment.h"

#include "bytestreamDecoder/L1CaloBsDecoderUtil.h"
#include "bytestreamDecoder/L1CaloRdoRodInfo.h"

/*!
 * \class L1CaloBsDecoderUtil
 * Utility methods for bytestream decoder classes for runs 1, 2 and 3.
 */

L1CaloBsDecoderUtil::L1CaloBsDecoderUtil()
{
}

void
L1CaloBsDecoderUtil::decodeRodInfo( const eformat::ROBFragment<const uint32_t*>* rod,
                                    std::list<L1CaloRdoRodInfo>& dat )
{
   // Create the basic object
   int sourceId  = rod->rod_source_id() & 0xffffff;
   int runNumber = rod->rod_run_no();
   int bcNumber  = rod->rod_bc_id();
   int triggerType  = rod->rod_lvl1_trigger_type();
   int detEventType = rod->rod_detev_type();
   int version = rod->rod_version();
//<<   // **FIXME** Hack for testing Run 3: set rod version
//<<   version |= 0x1004;   // **FIXME** Remove this!
   L1CaloRdoRodInfo rdo( sourceId>>16, sourceId&0xffff, runNumber, bcNumber, triggerType, detEventType, version );

   // Add in the Level-1 ID and size
   rdo.setLvl1Id( rod->rod_lvl1_id() );
   rdo.setSize( rod->rod_ndata() );
   
   // Add in the status words
   const uint32_t* status;
   rod->rod_status(status);
   int stat = *status++;
   rdo.setStatus1( stat );
   stat = *status;
   rdo.setStatus2( stat );
   
   // Now add in the module status words in the payload, if not RoI or CTP
   bool hasModuleStatus = true;
   if ( (sourceId>>16) > 0x74 )
      hasModuleStatus = false;
   if ( (sourceId>>16) == 0x73 )
      hasModuleStatus = false;
      
   if ( hasModuleStatus )
   {
      const uint32_t* it_data;
      rod->rod_data( it_data );
      const uint32_t ndata = rod->rod_ndata();
      ++it_data;
      for ( uint32_t i = 1; i < ndata; ++i, ++it_data ) {
         if ( ((*it_data)>>28) == 0xd )
         {
            int idat = (*it_data)&0xfff;
            int module = idat>>8;
            int modStat = rdo.getModuleStatus( module );
            rdo.setModuleStatus( module, modStat | (idat&0xff) );
         }
         if ( ((*it_data)>>28) == 0xf )
         {
            int idat = (*it_data)&0x1ff;
            int module = idat>>8;
            int modStat = rdo.getModuleStatus( module );
            rdo.setModuleStatus( module, modStat | (idat&0xff) );
         }
      }
   }
   
   dat.push_back( rdo );
}
