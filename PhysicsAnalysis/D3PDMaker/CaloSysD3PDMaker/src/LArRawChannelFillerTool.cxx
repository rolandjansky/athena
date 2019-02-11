/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   LArRawChannelFillerTool.cxx
 * Author: Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 * 
 * Created on May 30, 2012
 */

#include "LArRawChannelFillerTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

    LArRawChannelFillerTool::LArRawChannelFillerTool(const std::string& type,
                const std::string& name,
                const IInterface* parent):Base(type, name,parent)
    {
      book().ignore();  // Avoid coverity warnings.
    }

    LArRawChannelFillerTool::~LArRawChannelFillerTool() {
    }


    /**
     * @brief Standard Gaudi initialize method.
     */
    StatusCode LArRawChannelFillerTool::initialize()
    {
      CHECK( m_cablingKey.initialize()  );
      return StatusCode::SUCCESS;
    }

    StatusCode LArRawChannelFillerTool::book(){
        CHECK(addVariable("offlineID",m_offId));
        CHECK(addVariable("onlineID",m_onlId));
        CHECK(addVariable("energy",m_energy));
        CHECK(addVariable("time",m_time));
        CHECK(addVariable("quality",m_quality));
        return StatusCode::SUCCESS;
    }

    StatusCode LArRawChannelFillerTool::fill(const LArRawChannel& p){
        ATH_MSG_DEBUG( " in LArRawChannelFillerTool::fill()"  );
	
        const LArRawChannel* larR=&p;
	if ( larR ) {
                SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
                const LArOnOffIdMapping* cabling{*cablingHdl};
                if(!cabling){
                   ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
                   return StatusCode::FAILURE;
                }
		const Identifier id=cabling->cnvToIdentifier(larR->identify());
		*m_offId = id.get_identifier32().get_compact();
		*m_onlId = larR->identify().get_identifier32().get_compact();
		*m_energy = larR->energy();
		*m_time = larR->time()  ;
		*m_quality = larR->quality( );

	}
        return StatusCode::SUCCESS;
    }

}
