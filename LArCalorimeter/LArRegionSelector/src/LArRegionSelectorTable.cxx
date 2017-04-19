/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRegionSelector/LArRegionSelectorTable.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include <fstream>

#include "Identifier/IdentifierHash.h" 

#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrRegion.h"

#include "LArRawUtils/LArRoI_Map.h" 
#include "ByteStreamData/RawEvent.h"

#include "RegionSelector/RegionSelectorLUT.h" 
#include "RegionSelector/StoreGateRS_ClassDEF.h" 

#include "eformat/SourceIdentifier.h"

#include "LArIdentifier/LArReadoutModuleService.h"


LArRegionSelectorTable::LArRegionSelectorTable(const std::string& type, 
					       const std::string& name,
					       const IInterface* parent)
  :  AthAlgTool(type,name,parent),

  m_printTable(false),
  m_ttman(0),
  m_TT_ID(0),
  m_roiMap(0),
  m_emLUT(0),
  m_hecLUT(0),
  m_fcalemLUT(0),
  m_fcalhadLUT(0),
  m_roiFileNameEM_0("TrigTowerMapEM_0.txt"),
  m_roiFileNameEM_1("TrigTowerMapEM_1.txt"),
  m_roiFileNameEM_2("TrigTowerMapEM_2.txt"),
  m_roiFileNameEM_3("TrigTowerMapEM_3.txt"),
  m_roiFileNameHEC_0("TrigTowerMapHEC_0.txt"),
  m_roiFileNameHEC_1("TrigTowerMapHEC_1.txt"),
  m_roiFileNameHEC_2("TrigTowerMapHEC_2.txt"),
  m_roiFileNameHEC_3("TrigTowerMapHEC_3.txt"),
  m_roiFileNameFCALem_0("TrigTowerMapFCALem_0.txt"),
  m_roiFileNameFCALhad_0("TrigTowerMapFCALhad_0.txt"),
  m_roiFileNameFCALhad_1("TrigTowerMapFCALhad_1.txt")
{
  declareInterface<IRegionLUT_Creator>(this);
  declareProperty("PrintTable", m_printTable);
}

LArRegionSelectorTable::~LArRegionSelectorTable()
{
  // tables are stored in storegate so nothing to delete.
} 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArRegionSelectorTable::initialize(){

  ATH_MSG_INFO ( "initialize()" );

  ATH_MSG_INFO ( "Tool Properties" );
  ATH_MSG_INFO ( " Print Table:      " 
                 << ((m_printTable) ? "true" : "false") );

  if (m_printTable)
    ATH_MSG_INFO ( " Output Files:      " 
                   << m_roiFileNameEM_0 << "," << m_roiFileNameEM_1 << "," 
                   << m_roiFileNameEM_2 << "," << m_roiFileNameEM_3 << "," 
                   << m_roiFileNameHEC_0 << "," << m_roiFileNameHEC_1 << "," 
                   << m_roiFileNameHEC_2 << "," << m_roiFileNameHEC_3 << "," 
                   << m_roiFileNameFCALem_0 << "," 
                   << m_roiFileNameFCALhad_0 << "," << m_roiFileNameFCALhad_1 );

  ATH_CHECK( detStore()->retrieve(m_ttman) );
  ATH_MSG_DEBUG ( "CaloTTMgr Manager found" );

  ATH_CHECK( detStore()->retrieve(m_TT_ID) );
  ATH_MSG_DEBUG ( "CaloLVL1_ID helper found" );

  // Retrieve needed tools: LArRoI_Map, LArCablingSvc 

  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service( "ToolSvc",toolSvc) );
  ATH_MSG_DEBUG ( " Successfully retrieved ToolSvc "	);

  ATH_CHECK ( toolSvc->retrieveTool("LArRoI_Map", m_roiMap ) );
  fillMaps();

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArRegionSelectorTable::execute() {
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode LArRegionSelectorTable::finalize() {

  /* do we need this ?
  if (m_printTable) {
    m_fileout.close();
  }
  */

  ATH_MSG_INFO ( "finalize()" );
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

// Get the lookup table.
const RegionSelectorLUT * 
LArRegionSelectorTable::getLUT(std::string subSyst) const
{

  if(subSyst=="EM") {
    return m_emLUT;
  }
  else if(subSyst=="HEC") {
    return m_hecLUT;
  }
  else if(subSyst=="FCALEM") {
    return m_fcalemLUT;
  }
  else if(subSyst=="FCALHAD") {
    return m_fcalhadLUT;
  }
  else {
    ATH_MSG_WARNING ( "getLUT called with wrong argument, returning EM by default" );
    ATH_MSG_WARNING ( "valid list of arguments is EM HEC FCALEM FCALHAD" );
    return m_emLUT;
  }

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void
LArRegionSelectorTable::fillMaps()
{
  // cut and past of LArRoI_Map::print() written by H. Ma
  // except that RegionSelectorLUT.h are filled as well
  // ( 1 for EM, 1 for HEC, 2 for FCAL ) :

  // split TT map into files, EM and HAD separately. 
  // Create RegionSelectorLUT pointers for LAr
  // and dump the eta phi region for each TT 

  LArReadoutModuleService larReadoutModuleSvc;

  // check on the first map if job not already done by s.o. else
  std::string larKey = "LArRegionSelectorLUT_EM";
  StatusCode sc = detStore()->contains< RegionSelectorLUT >(larKey);
  if (sc == StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " RegionSelectorLUT " << larKey 
                    << " already exists => do nothing " );
    return;
  }


  int ncount=0; 
  int ncount1=0; 

  RegionSelectorLUT* ttLut= 0;
  RegionSelectorLUT* ttLutEM= new RegionSelectorLUT ( 12544 );
  RegionSelectorLUT* ttLutHEC= new RegionSelectorLUT ( 5632 );
  RegionSelectorLUT* ttLutFCALem= new RegionSelectorLUT ( 128 );
  RegionSelectorLUT* ttLutFCALhad= new RegionSelectorLUT ( 256 );

  std::ofstream* ttmap=0;
  std::vector<std::ofstream*> ttmapEM; 
  std::vector<std::ofstream*> ttmapHEC; 
  std::vector<std::ofstream*> ttmapFCALem; 
  std::vector<std::ofstream*> ttmapFCALhad; 

  if(m_printTable) {

    ttmapEM.push_back(new std::ofstream(m_roiFileNameEM_0.c_str())) ;
    ttmapEM.push_back(new std::ofstream(m_roiFileNameEM_1.c_str())) ;
    ttmapEM.push_back(new std::ofstream(m_roiFileNameEM_2.c_str())) ;
    ttmapEM.push_back(new std::ofstream(m_roiFileNameEM_3.c_str())) ;
    ttmapHEC.push_back(new std::ofstream(m_roiFileNameHEC_0.c_str())) ;
    ttmapHEC.push_back(new std::ofstream(m_roiFileNameHEC_1.c_str())) ;
    ttmapHEC.push_back(new std::ofstream(m_roiFileNameHEC_2.c_str())) ;
    ttmapHEC.push_back(new std::ofstream(m_roiFileNameHEC_3.c_str())) ;
    ttmapFCALem.push_back(new std::ofstream(m_roiFileNameFCALem_0.c_str())) ;
    ttmapFCALhad.push_back(new std::ofstream(m_roiFileNameFCALhad_0.c_str())) ;
    ttmapFCALhad.push_back(new std::ofstream(m_roiFileNameFCALhad_1.c_str())) ;

    /* do we need this ? Hong didn't
      m_fileout.open(m_roiFileNameEM_0.c_str());
    */

    *ttmapEM[0] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		<<std::endl ;
    *ttmapEM[1] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		<<std::endl ;
    *ttmapEM[2] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		<<std::endl ;
    *ttmapEM[3] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		<<std::endl ;
    *ttmapHEC[0] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		 <<std::endl ;
    *ttmapHEC[1] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		 <<std::endl ;
    *ttmapHEC[2] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		 <<std::endl ;
    *ttmapHEC[3] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		 <<std::endl ;
    *ttmapFCALem[0] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		  <<std::endl ;
    *ttmapFCALhad[0] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		  <<std::endl ;
    *ttmapFCALhad[1] <<" TT-ID , HASH, SAMP, LAYER, ETAMIN, ETAMAX, PHIMIN, PHIMAX, ROBIDs "
		  <<std::endl ;
  }

  CaloTTDescrManager::calo_region_const_iterator it2 =
    m_ttman->calo_region_begin();	
  CaloTTDescrManager::calo_region_const_iterator it2_end =
    m_ttman->calo_region_end();	

  for(; it2!=it2_end;++it2){
    const CaloTTDescrRegion* region = (*it2); 
    
    Identifier lay_id = region->identify(); 
    int sam = m_TT_ID->sampling(lay_id);
    int layer = m_TT_ID->layer(lay_id);
    bool isFcal = m_TT_ID->is_fcal(lay_id); 	

    float e1,e2,p1,p2 ;
    float dEta = region->deta() ; 
    float dPhi = region->dphi() ; 
    
    e1 = region->eta()-dEta/2. ; 
    e2 = e1+dEta; 
    
    p1 = region->phi()-dPhi/2. ; 
    p2 = p1+dPhi; 

    if(sam==0) {
      // EM
      if(isFcal) {
	if(layer == 0) {
	  if(m_printTable)	  ttmap = ttmapFCALem[layer]; 
	  ttLut = ttLutFCALem; 
	}
      } else {
	if(m_printTable)	ttmap = ttmapEM[layer]; 
	ttLut = ttLutEM; 
      }
    } else {
      // HAD
      if(isFcal) { 
	 if(m_printTable)      ttmap = ttmapFCALhad[layer]; 
	ttLut = ttLutFCALhad; 
      } else {
	 if(m_printTable)      ttmap = ttmapHEC[layer]; 
	ttLut = ttLutHEC; 
      }
    } 

    IdentifierHash hashid;	
    IdContext IdCntx = m_TT_ID->layer_context(); 
    m_TT_ID->get_hash(lay_id, hashid, &IdCntx ) ; 

    // quite special treatment for FCALHAD
    if ( isFcal && ( sam != 0 ) ){
    if ( m_TT_ID->pos_neg_z(lay_id) ==+1 ) { e1=3.6; e2=5.2;}
            else { e1=-5.2;e2=-3.6;};
    int pseudolayer = m_TT_ID->eta(lay_id);
    if ( layer == 0 ) {
            if ( pseudolayer < 2 ) layer=0; else layer = 1;
            if ( layer == 0 ){
                    if ( m_TT_ID->pos_neg_z(lay_id) ==+1 ) { e1=3.2; e2=3.6;}
                    else { e1=-3.6;e2=-3.2;};
            }
    } else {
            if ( pseudolayer < 2 ) layer=2; else layer = 3;
            if ( layer == 2 ){
                    if ( m_TT_ID->pos_neg_z(lay_id) ==+1 ) { e1=3.2; e2=3.6;}
                    else { e1=-3.6;e2=-3.2;};
            }
    }
    } // end of isFcal hadronic

    // get the RODID for this TT	
    const std::vector<HWIdentifier>& vROBID = m_roiMap->CollectionID((unsigned int)hashid);
    // skip the TT that does not have readout, i.e., invalid.
    if(vROBID.size() > 0) { 

      if(vROBID.size() > 1) {
        ATH_MSG_DEBUG ( "More than 1  (" << vROBID.size()
                        << ") ROBID for this TT, sampling, layer, etamin, phimin= " 
                        << m_TT_ID->show_to_string(lay_id) << " " 
                        << sam << " " << layer << " " << e1 << " " << p1
                        );
      }
      //	     *ttmap << m_TT_ID->show_to_string(lay_id) << " " << hashid 
      if(m_printTable && ttmap)
        *ttmap << lay_id.getString() << " " << hashid 
               << "  " << sam <<  "  "
               << "  " << layer <<  "  "
               <<e1 << "  " 
               <<e2 << "  "
               <<p1 << "  "
               <<p2 << " "; 
      
      std::vector<HWIdentifier>::const_iterator it 
	= vROBID.begin(); 
      std::vector<HWIdentifier>::const_iterator it_end 
	= vROBID.end(); 

      for(;it!=it_end;++it)	{
	  
	const HWIdentifier mId = (*it);
	eformat::SubDetector detid = (eformat::SubDetector) larReadoutModuleSvc.subDet(mId);
	uint16_t m = larReadoutModuleSvc.rodFragId(mId);
	//EventFormat::SourceID sid = EventFormat::SourceID(detid,type,m);
	eformat::helper::SourceIdentifier sid(detid,m);
	uint32_t rod_id =  sid.code();
	
	if(m_printTable && ttmap)	  *ttmap << std::hex<< rod_id << " " ; 
	
	ttLut->additem((double)e1,(double)e2,(double)p1,(double)p2,
		       sam,layer,(int)hashid,(int)rod_id);
	ATH_MSG_VERBOSE ( ncount << " filled LUT with following info"
                          << " hashid,sampling, layer, etamin, phimin= " 
                          << m_TT_ID->show_to_string(lay_id) << " " 
                          << hashid << " "
                          << sam << " " << layer << " " << e1 << " " << p1 );
	
        ++ncount; 
      }
      
      if(m_printTable && ttmap)      *ttmap<<std::dec<<std::endl; 
      ++ncount1; 
    } else {
      // no ROBID for this TT. not abnormal (not all layers exist in all TTs)
      ATH_MSG_DEBUG ( "No ROBID for this TT, sign, sampling, layer, etamin, phimin= " 
                      << m_TT_ID->show_to_string(lay_id) << " " << sam << " " << layer << " " << e1 << " " << p1
                      );
    }
    
  } // end loop on "regions"

  ATH_MSG_INFO (" Total number of records = "<<ncount);
  ATH_MSG_INFO (" Total number of TTs = "<<ncount1);


  // save pointer as member.
  m_emLUT = ttLutEM;
  m_hecLUT = ttLutHEC;
  m_fcalemLUT = ttLutFCALem;
  m_fcalhadLUT = ttLutFCALhad;

  larKey = "LArRegionSelectorLUT_EM";
  if (recordMap(ttLutEM,larKey).isFailure()) {
    ATH_MSG_ERROR("Cannot record " << larKey);
    return;
  }

  larKey = "LArRegionSelectorLUT_HEC";
  if (recordMap(ttLutHEC,larKey).isFailure()) {
    ATH_MSG_ERROR("Cannot record " << larKey);
    return;
  }

  larKey = "LArRegionSelectorLUT_FCALem";
  if (recordMap(ttLutFCALem,larKey).isFailure()) {
    ATH_MSG_ERROR("Cannot record " << larKey);
    return;
  }

  larKey = "LArRegionSelectorLUT_FCALhad";
  if (recordMap(ttLutFCALhad,larKey).isFailure()) {
    ATH_MSG_ERROR("Cannot record " << larKey);
    return;
  }

  if(m_printTable) {
    delete ttmapEM[0];
    delete ttmapEM[1];
    delete ttmapEM[2];
    delete ttmapEM[3];
  
    delete ttmapHEC[0];
    delete ttmapHEC[1];
    delete ttmapHEC[2];
    delete ttmapHEC[3];
    
    delete ttmapFCALem[0];
    delete ttmapFCALhad[0];
    delete ttmapFCALhad[1];
  }

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
LArRegionSelectorTable::recordMap(RegionSelectorLUT* larRSlut, std::string larKey) 
{
  static const bool SETCONST(false);
  StatusCode sc = detStore()->contains< RegionSelectorLUT >(larKey);
  if (sc == StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " RegionSelectorLUT " << larKey 
                    << " already exists " );
  } else {
    ATH_CHECK( detStore()->record(larRSlut, larKey,SETCONST) );
    ATH_MSG_INFO ( "LArRegionSelectorLUT successfully saved in detector Store" 
                   << larKey );
  }
  return StatusCode::SUCCESS;
}
