/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Detector description conversion service package
 -----------------------------------------------
 Copyright (C) 2002 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: IdDictTest.h,v 1.25 2005-04-28 17:07:11 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IDDICTTEST_IDDICTTEST_H
# define IDDICTTEST_IDDICTTEST_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/Algorithm.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class Identifier;
class ExpandedIdentifier;
class Range;
class MultiRange;
class IdDictManager;
class LArIdManager;
class CaloIdManager;
class IdDictDictionary;
class SiliconID;
class CaloCell_ID;
class CaloDM_ID;
class CaloLVL1_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArMiniFCAL_ID;
class LArOnlineID;
class PixelID;
class SCT_ID;
class TRT_ID;
class StoreGateSvc;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/********************************************************************

Algorithm for testing the Identifier dictionary for all subdetectors

********************************************************************/

class IdDictTest : public Algorithm
{

 public:

  IdDictTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~IdDictTest();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

    void 	tab 		(size_t level) const;

    // test for atlas id:
    StatusCode 	testAtlasDetectorID(void) const;
    StatusCode	test_indet_ids 	   (void) const;
    StatusCode	test_calos_ids 	   (void) const;
    StatusCode	test_laronline_ids (void) const;
    StatusCode	test_muons_ids 	   (void) const;
    StatusCode	print_ids	   (void);

    // Methods to get a valid id for dead mat, lvl1 and muon subdets - for checks
    Identifier 	lvl1() const;
    Identifier 	lar_dm() const;
    Identifier 	tile_dm() const;
    Identifier 	mdt() const;
    Identifier 	csc() const;
    Identifier 	rpc() const;
    Identifier 	tgc() const;
    
    BooleanProperty             m_printFlag;
    BooleanProperty             m_atlasFlag;
    BooleanProperty             m_indetFlag;
    BooleanProperty             m_pixelFlag;
    BooleanProperty             m_sctFlag;
    BooleanProperty             m_trtFlag;
    BooleanProperty             m_calosFlag;
    BooleanProperty             m_caloTTFlag;
    BooleanProperty             m_emBarrelFlag;
    BooleanProperty             m_emEndcapFlag;
    BooleanProperty             m_hecFlag;
    BooleanProperty             m_fcalFlag;
    BooleanProperty             m_miniFcalFlag;
    BooleanProperty             m_laronlineFlag;
    BooleanProperty             m_tileFlag;
    BooleanProperty             m_muonsFlag;
    BooleanProperty             m_cscFlag;
    BooleanProperty             m_mdtFlag;
    BooleanProperty             m_rpcFlag;
    BooleanProperty             m_tgcFlag;
    const IdDictManager*        m_idDictMgr;
    const PixelID*              m_pixIDHelper;
    const SCT_ID*               m_sctIDHelper;
    const TRT_ID*               m_trtIDHelper;
    const SiliconID*            m_siliconID;
    const LArIdManager*         m_larIdMgr;
    const CaloIdManager*        m_caloIdMgr;
    const CaloCell_ID*          m_caloCellHelper;
    const CaloDM_ID*            m_caloDMHelper;
    const CaloLVL1_ID*          m_lvl1Helper;
    const LArEM_ID*             m_emHelper;
    const LArHEC_ID*            m_hecHelper;
    const LArFCAL_ID*           m_fcalHelper;
    const LArMiniFCAL_ID*       m_miniFcalHelper;
    const LArOnlineID*          m_laronlineHelper;
    IdDictDictionary*           m_muon_dict;
    IdDictDictionary*           m_calo_dict;  
    bool                        m_tested_ids;
    StoreGateSvc*               m_detStore;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // IDDICTTEST_IDDICTTEST_H
