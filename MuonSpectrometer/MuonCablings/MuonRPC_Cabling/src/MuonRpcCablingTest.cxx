/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRPC_Cabling/MuonRpcCablingTest.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "RPCcablingInterface/RpcPadIdHash.h"

#include <fstream>

MuonRpcCablingTest::MuonRpcCablingTest(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm            ( name, pSvcLocator ),
    m_cablingSvc            ( NULL ),
    m_padHashIdHelper       ( NULL ),
    m_selSideStr            ( "" ),
    m_firstLastChannel      ( false )
{
    declareProperty("selectedSide",    m_selSideStr="");
    declareProperty("selectedSectors", m_selSectorVec);
    declareProperty("selectedRois",    m_selRoiVec);    
    declareProperty("selectedPads",    m_selPadVec);    
    declareProperty("testOnlyFirstLastCMChannel",m_firstLastChannel=false);
    m_first = true;
}

StatusCode
MuonRpcCablingTest::initialize()
{
    StatusCode status = StatusCode::SUCCESS;

    ATH_MSG_INFO("In Initialize");
    m_first = true;

    ATH_CHECK(m_idHelperSvc.retrieve());

    // get RPC cablingSvc
    const IRPCcablingServerSvc* RpcCabGet = nullptr;
    ATH_CHECK(service("RPCcablingServerSvc", RpcCabGet));
  
    ATH_CHECK(RpcCabGet->giveCabling(m_cablingSvc));

    return status;
    
}


StatusCode
MuonRpcCablingTest::execute()
{
    if (!m_first) return StatusCode::SUCCESS;
    m_first = false;
    std::ofstream fout("RPC_Mapping.dump");
    
    ATH_MSG_INFO("Executing");

    m_padHashIdHelper = m_cablingSvc->padHashFunction();
    
    
    const CablingRPCBase* cabling = m_cablingSvc->getRPCCabling();
    if (!cabling) 
    {
        ATH_MSG_ERROR(" CablingRPCBase not retrieven !!!!!!");
        return StatusCode::FAILURE;
    }

    IdContext rpcModuleContext = m_idHelperSvc->rpcIdHelper().module_context();

    // this is the right thing to do !!!!!!!
    unsigned int hash_max = m_padHashIdHelper->max();
    ATH_MSG_INFO("From padHashIdHelper hash_max = "<<hash_max);
    
    
    Identifier pad_idId;
    unsigned short int pad_id;
    unsigned int       logic_sector;
    Identifier PadId, PadId_previous;

    PadId_previous = (int) 0;

    unsigned short int iSubSysMin=0;
    unsigned short int iSubSysMax=1;
    if (m_selSideStr!="") 
    {
	if  (m_selSideStr=="A")  iSubSysMin=1;
	else if (m_selSideStr!="C") 
	{

	    ATH_MSG_WARNING("Unknown side chosen - selected <"<<m_selSideStr<<"> while available are A and C");
	}
	else  iSubSysMax = 0;	
    }
    
    std::vector<unsigned short int> iSecVec;
    if (!m_selSectorVec.empty())
    {
	ATH_MSG_INFO("# of selected Sectors is "<<m_selSectorVec.size());
	int ic = 0;
	for (std::vector<int>::const_iterator is=m_selSectorVec.begin(); is!=m_selSectorVec.end(); ++is) 
	{
	    iSecVec.push_back(*is);
	    ATH_MSG_INFO("Sector Vec content is --- "<<*is<<" at element "<<ic);
	    ++ic;
	}
	
    }	
    else 
    {
	for (unsigned short int is=0; is < 32; ++is) 
	{
	    iSecVec.push_back(is);
	}
    }

    std::vector<unsigned short int> iRoiVec;
    if (!m_selRoiVec.empty())
    {
	ATH_MSG_INFO("# of selected Roi is "<<m_selRoiVec.size());
	int ic=0;
	for (std::vector<int>::const_iterator is=m_selRoiVec.begin(); is!=m_selRoiVec.end(); ++is) 
	{
	    iRoiVec.push_back(*is);
	    ATH_MSG_INFO("Roi Vec content is --- "<<*is<<" at element "<<ic);
	    ++ic;
	}
	
    }	
    else 
    {
	ATH_MSG_INFO("No Roi Selected ");
	int ic=0;
	for (unsigned short int is=0; is < 32; ++is) 
	{
	    iRoiVec.push_back(is);
	    ATH_MSG_DEBUG("Roi Vec content is --- "<<is<<" at element "<<ic);
	    ++ic;
	}
    }
    unsigned short int iSubSysId=iSubSysMin;
    for (; iSubSysId<iSubSysMax+1; ++iSubSysId)
    {
	ATH_MSG_INFO("New SubSysID = " << iSubSysId);
        for (std::vector<unsigned short int>::const_iterator is=iSecVec.begin(); is!=iSecVec.end(); ++is)
        {
	    unsigned short int iSectorId = *is;
	    ATH_MSG_INFO("New SectorID = " << iSectorId);
	    
            for (std::vector<unsigned short int>::const_iterator ir=iRoiVec.begin(); ir!=iRoiVec.end(); ++ir)
            {
		unsigned short int iRoiNumber = *ir;
		ATH_MSG_DEBUG("New RoiID = " << iRoiNumber);
		
                ATH_MSG_DEBUG("SubSysID  = " << iSubSysId << " SectorID = " << iSectorId << " RoiNumber =" << iRoiNumber);
                if (cabling->give_PAD_address(iSubSysId,iSectorId,iRoiNumber,logic_sector,pad_id,PadId))
                {
		    pad_idId = pad_id;
                    ATH_MSG_DEBUG("pad_id, PadId ------------------------ "
				 <<  (int)pad_idId.get_identifier32().get_compact() <<" "<< PadId<<" "<<m_idHelperSvc->rpcIdHelper().show_to_string(PadId));
                    
                    if (PadId_previous == PadId)
                    {
                        ATH_MSG_DEBUG("same pad as before ");
                    }
                    else
                    {
                        IdentifierHash padHash = (*m_padHashIdHelper)(PadId);
                        ATH_MSG_DEBUG("Associated pad-Hash (via the hash-function) = "<<(int)padHash);
                        if (padHash>hash_max) ATH_MSG_ERROR("Computed hashId > hash_max = "<<hash_max);

                        for (unsigned short int CMAId=0; CMAId<8; ++CMAId)
                        {
                            for (unsigned short ijk=0; ijk<6; ++ijk)
                            {
				for (unsigned short Channel=0; Channel<32;)
				{	
				    std::list<Identifier> myOfflineList = 
					m_cablingSvc->give_strip_id   (iSubSysId,
                                                   iSectorId,
                                                   pad_idId.get_identifier32().get_compact(),
                                                   CMAId	,
                                                   ijk,
                                                   Channel);
                                    
				    ATH_MSG_DEBUG("ijk = "<<ijk<<" channel = "<<Channel<<" list size is "<<myOfflineList.size());				    
				    fout<<"Online "<<iSubSysId<<"."<<iSectorId<<"."<<pad_idId.get_identifier32().get_compact()<<"."<<CMAId<<"."<<ijk<<"."<<Channel
					<<" \t "<<myOfflineList.size()<<" \t ";
				    ATH_MSG_DEBUG("Online "<<iSubSysId<<"."<<iSectorId<<"."<<pad_idId.get_identifier32().get_compact()<<"."<<CMAId<<"."<<ijk<<"."<<Channel
						  <<" \t "<<myOfflineList.size()<<" \t ");
				    if (!myOfflineList.empty()) 
				    {
					for (std::list<Identifier>::const_iterator it=myOfflineList.begin(); it!=myOfflineList.end(); ++it)
					{
					    ATH_MSG_DEBUG("ijk = "<<ijk<<" channel = "<<Channel<<" id offline = "<<m_idHelperSvc->rpcIdHelper().show_to_string(*it));
					    fout<<m_idHelperSvc->rpcIdHelper().show_to_string(*it)<<" ";
					    ATH_MSG_DEBUG(m_idHelperSvc->rpcIdHelper().show_to_string(*it)<<" ");
					}
				    }
				    fout<<std::endl;
				    if (m_firstLastChannel) Channel+=31;
				    else ++Channel;
				}				
                            }
                        }
                    }
                    PadId_previous =  PadId;                                            
                }
                else 
                    ATH_MSG_DEBUG("No pad for this side/sector/roi ");
            }
        }
    }
    fout.close();


    std::ofstream fout2("RPCPadParameters.dump");
    // M.Corradi, test of Pad setup
    fout2 << "DUMP OF PAD PARAMETERS" << std::endl;
    fout2 << "======================" << std::endl;
    fout2 << "s  p  a  m  f  t  t  t" << std::endl;
    fout2 << "e  a  n  a  e  h  h  h" << std::endl;
    fout2 << "c  d  d  s  e  0  1  2" << std::endl;
    fout2 << "         k  t         " << std::endl;
    for (unsigned short int isl=0; isl<64; isl++){
        for (unsigned short int it=0; it<8; it++){
            
            bool eta_and_phi, feet;
            unsigned short int cma_mask,feet_th0,feet_th1,feet_th2;
            
            if (
                 m_cablingSvc->give_Pad_Parameters(isl,it,feet,eta_and_phi,
                                                  cma_mask,feet_th0,
                                                  feet_th1, feet_th2)
                ){
                fout2 << isl << "  "
                     << it << "  "
                     << eta_and_phi << "  "
                     << cma_mask << "  "
                     << feet << "  " 
                     << feet_th0 << "  "
                     << feet_th1 << "  "
                     << feet_th2 << std::endl;
            } else {
                ATH_MSG_DEBUG("No pad parameters found");
            }        
        }
    }
    fout2.close();
    

    return StatusCode::SUCCESS;
}
