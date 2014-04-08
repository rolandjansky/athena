/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CREATEAANTFROMLUMIBLOCKCOLLECTION_H
#define CREATEAANTFROMLUMIBLOCKCOLLECTION_H

// *****************************************************************
//
// Algorithm:  CreateAANTFromLumiBlockCollection.hh
// Author: Balint Radics <radbal@cern.ch>
// Created: May 2009
// Description:
//     This algorithm is used to create an LumiBlockCollection_p1
//     in an output Root file using THistSvc
//
// *********************************************************************


// LumiBlock and IOV includes
#include "AthenaKernel/IOVTime.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockRange_p1.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p1.h"

// STL includes
#include <vector>
#include <stdint.h>

// Framework includes
#include "GaudiKernel/ITHistSvc.h" 
#include "GaudiKernel/Algorithm.h"


// ROOT includes
#include "TTree.h"

class StoreGateSvc;

class CreateAANTFromLumiBlockCollection:public Algorithm {
public:
  CreateAANTFromLumiBlockCollection (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode clear();
  StatusCode finalize();

private:

  StringProperty  m_LBColl_name;

  StoreGateSvc* m_storeGate;    //cache the StoreGateSvc ptr for efficiency
  StoreGateSvc* m_metaStore;    //cache the StoreGateSvc ptr for efficiency
  ITHistSvc * tHistSvc;

  LumiBlockCollection*  m_tempLBColl;

  LumiBlockCollectionCnv_p1 m_lbc_conv;
  LumiBlockRangeCnv_p1 m_lbr_conv;
  
  LumiBlockCollection_p1 *m_lbc;
  LumiBlockRange_p1 *m_lbr;

  std::vector<double> * m_vectord;

  TTree * MetaDataTree;
  TTree * UserDataTree;

};

#endif // end of CREATEAANTFROMLUMIBLOCKCOLLECTION_H
