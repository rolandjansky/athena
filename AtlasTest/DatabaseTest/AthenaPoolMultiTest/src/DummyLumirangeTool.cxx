/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DummyLumirangeTool.cxx
 *
 * @brief AthAlgorithm which takes references provided by RegStream 
 * finds the appropriate CORAL object in storegate and stores 
 * them in a POOL collection. 
 *
 * $Id: DummyLumirangeTool.cxx,v 1.2 2009-05-01 13:41:51 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

//#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "SGTools/DataProxy.h"

// Framework include files
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/GaudiException.h"

#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <ctime>

#include "DummyLumirangeTool.h"

// Standard Constructor
DummyLumirangeTool::DummyLumirangeTool(const std::string& name,
                                             ISvcLocator* pSvcLocator)
    :
    AthAlgorithm(name,pSvcLocator),
    m_storeName("StoreGateSvc/MetaDataStore"),
    m_lumigran(2),
    m_firstrun(0),
    m_rangesize(40),
    m_blocksperrun(200),
    m_lumitot(1000)
{
    declareProperty("StoreName",    m_storeName, "Store to find input metadata during finalize");
    declareProperty("Granularity",  m_lumigran,  "skip size");
    declareProperty("FirstRun",     m_firstrun,  "beginning run number");
    declareProperty("RangeSize",    m_rangesize, "number of lumi/range" );
    declareProperty("BlocksPerRun", m_blocksperrun, "number of lumiblocks till increment run");
    declareProperty("LumiTot",      m_lumitot, "total number of lumiblocks to write");
}

// initialize data writer
StatusCode 
DummyLumirangeTool::initialize() 
{
   ATH_MSG_DEBUG("In initialize ");


   Root::TGoodRunsList grl;
   int lumicount = 0;
   //int runcount = 1;
   int ilumiblock = 1;
   int irun = m_firstrun;
   while (lumicount < m_lumitot) {
      //ATH_MSG_DEBUG("lumicount = " << lumicount);
      //ATH_MSG_DEBUG("Entering rangesize loop");
      for (int j=0; j<m_rangesize; ++j) {
         ATH_MSG_DEBUG("Adding " << irun <<", " << ilumiblock << " for " << j);
         grl.AddRunLumiBlock(irun,ilumiblock);
         if (ilumiblock%m_rangesize/m_lumigran==0) {
            ATH_MSG_DEBUG("hit granularity boundary");
            ilumiblock += m_lumigran;
         }
         ++lumicount;
         ++ilumiblock;
      }
      if (lumicount%m_blocksperrun ==0) {
         irun += 2;
         ilumiblock = 1;
      }
   }

   Root::TGoodRunsListWriter blue;
   blue.SetGoodRunsList(grl);

   return AthAlgorithm::initialize();
}

// initialize data writer
StatusCode 
DummyLumirangeTool::execute() 
{
    ATH_MSG_DEBUG("In execute ");
    return StatusCode::SUCCESS;
}


// terminate data writer
StatusCode 
DummyLumirangeTool::finalize() 
{
    ATH_MSG_DEBUG("In finalize ");
    return AthAlgorithm::finalize();
}

