/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/ReadTgcDigit.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

const int maxColl = 1600;
const int maxDig  = 4096;

/////////////////////////////////////////////////////////////////////////////

ReadTgcDigit::ReadTgcDigit(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), 
    m_ntuplePtr(0), 
    m_activeStore("ActiveStoreSvc", name)
{
  // Declare the properties
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteTgcNtuple", m_tgcNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcDigit::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_activeStore.retrieve() );
  ATH_CHECK( m_idHelperSvc.retrieve() );

  if (!m_tgcNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  // add items
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCncoll",   m_nColl, 0, maxColl) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCndig",    m_nDig,  0, maxDig) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCstation", m_nDig,  m_stationName) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCeta",     m_nDig,  m_stationEta) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCphi",     m_nDig,  m_stationPhi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCgasgap",  m_nDig,  m_gasGap) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCisstrip", m_nDig,  m_isStrip) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCchannel", m_nDig,  m_channel) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcdig/TGCbctag",   m_nDig,  m_bcTag) );
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );

  // get TGC digit container
  const std::string key = "TGC_DIGITS";
  const TgcDigitContainer* tgc_container = nullptr;
  ATH_CHECK( (*m_activeStore)->retrieve(tgc_container, key) );
 
  ATH_MSG_DEBUG("****** tgc->size() : " << tgc_container->size() );

  if (!m_tgcNtuple) return StatusCode::SUCCESS; 

  m_nColl = 0;
  m_nDig = 0;

  // loop over collections
  for (TgcDigitContainer::const_iterator c = tgc_container->begin(); 
       c != tgc_container->end(); ++c)
    {
      if ( (*c)->size() == 0 ) continue;

      // loop over digits
      std::vector<Identifier> vID;
      for (TgcDigitCollection::const_iterator dig = (*c)->begin(); 
	   dig != (*c)->end(); ++dig) 
	{
	  Identifier id = (*dig)->identify();
          uint16_t bctag  = (*dig)->bcTag();

	  // ignore duplicated hits
	  bool dupl=false;
	  std::vector<Identifier>::const_iterator it_vID  = vID.begin();	  
	  std::vector<Identifier>::const_iterator it_vIDE = vID.end();
	  for (; it_vID != it_vIDE; ++it_vID)
	    {
	      if ((*it_vID)==id)
		{ 
		  dupl=true;
		  break;
		}
	    }

	  if (dupl) continue;
	  
	  vID.push_back(id);
	  
          ATH_MSG_DEBUG( "Digit number " << m_nDig  );

	  // ID information
	  m_stationName[m_nDig] = m_idHelperSvc->tgcIdHelper().stationName(id);
	  m_stationEta [m_nDig] = m_idHelperSvc->tgcIdHelper().stationEta(id);
	  m_stationPhi [m_nDig] = m_idHelperSvc->tgcIdHelper().stationPhi(id);
	  m_gasGap     [m_nDig] = m_idHelperSvc->tgcIdHelper().gasGap(id);
	  m_isStrip    [m_nDig] = m_idHelperSvc->tgcIdHelper().isStrip(id);
	  m_channel    [m_nDig] = m_idHelperSvc->tgcIdHelper().channel(id); 
	  m_bcTag      [m_nDig] = bctag;

	  ATH_MSG_DEBUG( MSG::hex
                         << " N_" << m_idHelperSvc->tgcIdHelper().stationName(id)
                         << " E_" << m_idHelperSvc->tgcIdHelper().stationEta(id)
                         << " P_" << m_idHelperSvc->tgcIdHelper().stationPhi(id)
                         << " G_" << m_idHelperSvc->tgcIdHelper().gasGap(id)
                         << " C_" << m_idHelperSvc->tgcIdHelper().channel(id) );

	  ++m_nDig;
	}
      ++m_nColl;
  }

  ATH_MSG_DEBUG( "execute() completed" );
  return StatusCode::SUCCESS;
}

StatusCode ReadTgcDigit::accessNtuple()
{

  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleSvc(), m_NtupleLocID );

  if (static_cast<int>(nt))
    {
      m_ntuplePtr=nt;
      ATH_MSG_INFO( "Ntuple " << m_NtupleLocID 
                    << " reaccessed! "  );
    } 
  else
    {
      ATH_MSG_FATAL( "Cannot reaccess " << m_NtupleLocID  );
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}


