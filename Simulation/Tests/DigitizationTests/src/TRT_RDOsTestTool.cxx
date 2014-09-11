/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_RDOsTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <boost/io/ios_state.hpp>
//#include "HepMC/GenParticle.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/TRT_SimHelper.h"
#include <TH2D.h>
#include <TProfile.h>
//#include "AthenaKernel/errorcheck.h"

TRT_RDOsTestTool::TRT_RDOsTestTool(const std::string& type, 
				   const std::string& name,
				   const IInterface* parent)
  : DigiTestToolBase(type, name, parent),
    m_trtID(NULL),
    m_numberOfEventsSelected(0),
    m_collection("TRT_RDOs"),
    m_nRDO_Colls(0),
    m_nEmptyRDO_Colls(0),
    m_BarrelEndcap(0),
    m_BarrelLayer(0),
    m_EndcapLayer(0),
    m_BarrelStrawLayer(0),
    m_EndcapStrawLayer(0),
    m_BarrelPhiModule(0),
    m_EndcapPhiModule(0),
    m_BarrelStraw(0),
    m_EndcapStraw(0),
    m_BarrelTimeOverThreshold(0),
    m_EndcapTimeOverThreshold(0),
    m_BarrelDriftTimeBin(0),
    m_EndcapDriftTimeBin(0),
    m_BarrelTrailingEdge(0),
    m_EndcapTrailingEdge(0)
{
//   for(unsigned int i(0); i<9; ++i) 
//     {
//       m_h_trt_endcapA_occ_layer[i]=NULL;
//       m_h_trt_endcapC_occ_layer[i]=NULL;
//     }
//   for(unsigned int i(0); i<4; ++i) { m_h_trt_barrel_occ_layer[i]=NULL; }
//   for(unsigned int i(0); i<5; ++i) { m_NumberModulesVetoed[i]=0.0; }

}

StatusCode TRT_RDOsTestTool::initialize() 
{
  // get the TRT ID Helper
  if (detStore()->retrieve(m_trtID, "TRT_ID").isFailure()) 
    {
      return StatusCode::FAILURE;
    }
  
  // initialise pixel or TRT variables:
  m_path+="InDet/TRT/";
  _TH1D(m_nRDO_Colls, "Number_of_TRT_RDO_Collections",1000,0,8000);                         
  _SET_TITLE(m_nRDO_Colls, "Number of TRT RDO Collections", "Number of TRT RDO Collections", "Frequency");                         
  _TH1D(m_nEmptyRDO_Colls, "Number_of_Empty_TRT_RDO_Collections",1000,0,8000);                         
  _SET_TITLE(m_nEmptyRDO_Colls, "Number of Empty TRT RDO Collections", "Number of Empty TRT RDO Collections", "Frequency");                         
  _TH1D(m_BarrelEndcap, "BarrelEndcap",7,-3.5,3.5);                         
  _SET_TITLE(m_BarrelEndcap, "TRT barrel/endcap", "TRT barrel/endcap", "Frequency"); 
  _TH1D(m_BarrelPhiModule, "BarrelPhiModule",36,-0.5,35.5);          
  _SET_TITLE(m_BarrelPhiModule, "TRT phi module, barrel","TRT phi module, barrel", "Frequency");
  _TH1D(m_EndcapPhiModule, "EndcapPhiModule",36,-0.5,35.5);          
  _SET_TITLE(m_EndcapPhiModule, "TRT phi module, endcap","TRT phi module, endcap", "Frequency");
  _TH1D(m_BarrelLayer, "BarrelLayer", 5,-0.5,4.5);              
  _SET_TITLE(m_BarrelLayer, "TRT barrel layer", "Layer", "Frequency");
  _TH1D(m_EndcapLayer, "EndcapLayer",31,-15.5,15.5);                       
  _SET_TITLE(m_EndcapLayer, "TRT layer(wheel): endcap","TRT layer(wheel): endcap", "Frequency");
  _TH1D(m_BarrelStrawLayer, "BarrelStrawLayer",61,-30.5,30.5);           
  _SET_TITLE(m_BarrelStrawLayer,"TRT straw layer, barrel","TRT straw layer, barrel", "Frequency");
  _TH1D(m_EndcapStrawLayer, "EndcapStrawLayer",41,-20.5,20.5);           
  _SET_TITLE(m_EndcapStrawLayer, "TRT straw layer, endcap","TRT straw layer, endcap", "Frequency");
  _TH1D(m_BarrelStraw, "BarrelStraw",32,-1.5,30.5); //fill for all strips in an RDO cluster          
  _SET_TITLE(m_BarrelStraw, "TRT straw, barrel","TRT straw, barrel", "Frequency");
  _TH1D(m_EndcapStraw, "EndcapStraw",32,-1.5,30.5); //fill for all strips in an RDO cluster           
  _SET_TITLE(m_EndcapStraw, "TRT straw, endcap","TRT straw, endcap", "Frequency");
  _TH1D(m_BarrelTimeOverThreshold, "BarrelTimeOverThreshold",101,-0.5,100.5);             
  _SET_TITLE(m_BarrelTimeOverThreshold, "TRT Time Over Threshold, barrel","TRT Time Over Threshold, barrel", "Frequency");
  _TH1D(m_EndcapTimeOverThreshold, "EndcapTimeOverThreshold",101,-0.5,100.5);             
  _SET_TITLE(m_EndcapTimeOverThreshold, "TRT Time Over Threshold, endcap","TRT Time Over Threshold, endcap", "Frequency");
  _TH1D(m_BarrelDriftTimeBin, "BarrelDriftTimeBin",36,-5.5,30.5); //fill for all strips in an RDO cluster          
  _SET_TITLE(m_BarrelDriftTimeBin, "TRT Drift Time Bin, barrel","TRT Drift Time Bin, barrel", "Frequency");
  _TH1D(m_EndcapDriftTimeBin, "EndcapDriftTimeBin",36,-5.5,30.5); //fill for all strips in an RDO cluster           
  _SET_TITLE(m_EndcapDriftTimeBin, "TRT Drift Time Bin, endcap","TRT Drift Time Bin, endcap", "Frequency");
  _TH1D(m_BarrelTrailingEdge, "BarrelTrailingEdge",36,-5.5,30.5); //fill for all strips in an RDO cluster          
  _SET_TITLE(m_BarrelTrailingEdge, "TRT Trailing Edge, barrel","TRT Trailing Edge, barrel", "Frequency");
  _TH1D(m_EndcapTrailingEdge, "EndcapTrailingEdge",36,-5.5,30.5); //fill for all strips in an RDO cluster           
  _SET_TITLE(m_EndcapTrailingEdge, "TRT Trailing Edge, endcap","TRT Trailing Edge, endcap", "Frequency");

//   _TH1D(m_BarrelOccupancyByLayer   , "BarrelOccupancyByLayer", 4,0.5,4.5);
//   _SET_TITLE(m_BarrelOccupancyByLayer   , "TRT Barrel Occupancy By Layer (%)", "Layer", "TRT Barrel Occupancy [%]");
//   _TH2D(m_h_trt_barrel_occ_layer[0], "h_trt_lay0_occupancy", 13, -6.5, 6.5, 32, 0, 32);
//   _TH2D(m_h_trt_barrel_occ_layer[1], "h_trt_lay1_occupancy", 13, -6.5, 6.5, 40, 0, 40);
//   _TH2D(m_h_trt_barrel_occ_layer[2], "h_trt_lay2_occupancy", 13, -6.5, 6.5, 48, 0, 48);
//   _TH2D(m_h_trt_barrel_occ_layer[3], "h_trt_lay3_occupancy", 13, -6.5, 6.5, 56, 0, 56);

//   _TH1D(m_EndCapA_OccupancyByDisk, "EndCapA_OccupancyByDisk", 9,0.5,9.5);
//   _SET_TITLE(m_EndCapA_OccupancyByDisk, "TRT EndcapA Occupancy By Disk (%)", "Disk", "TRT Endcap A Occupancy [%]");
//   _TH2D(m_h_trt_endcapA_occ_layer[0], "h_trt_ecA_dsk0_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[1], "h_trt_ecA_dsk1_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[2], "h_trt_ecA_dsk2_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[3], "h_trt_ecA_dsk3_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[4], "h_trt_ecA_dsk4_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[5], "h_trt_ecA_dsk5_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[6], "h_trt_ecA_dsk6_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[7], "h_trt_ecA_dsk7_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapA_occ_layer[8], "h_trt_ecA_dsk8_occupancy", 1,-0.5,0.5,52,-0.5,51.5);

//   _TH1D(m_EndCapC_OccupancyByDisk   , "EndCapC_OccupancyByDisk", 9,0.5,9.5);
//   _SET_TITLE(m_EndCapC_OccupancyByDisk, "TRT EndcapC Occupancy By Disk (%)", "Disk", "TRT Endcap C Occupancy [%]");
//   _TH2D(m_h_trt_endcapC_occ_layer[0], "h_trt_ecC_dsk0_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[1], "h_trt_ecC_dsk1_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[2], "h_trt_ecC_dsk2_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[3], "h_trt_ecC_dsk3_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[4], "h_trt_ecC_dsk4_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[5], "h_trt_ecC_dsk5_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[6], "h_trt_ecC_dsk6_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[7], "h_trt_ecC_dsk7_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
//   _TH2D(m_h_trt_endcapC_occ_layer[8], "h_trt_ecC_dsk8_occupancy", 1,-0.5,0.5,52,-0.5,51.5);

//   _TH1D(m_TRT_OccupancyByModuleType, "TRT_OccupancyByModuleType", 5,-0.5,4.5);
//   _SET_TITLE(m_TRT_OccupancyByModuleType, "TRT Occupancy By Module Type","Types: Outer(0), Short-Middle(1), Middle(2), Inner(3), Barrel(4)", "Occupancy [%]");

  return StatusCode::SUCCESS;
}

StatusCode TRT_RDOsTestTool::processEvent() {

  //CHECK(this->CheckSDOs());
  const TRT_RDO_Container* p_rdocontainer;
  if(evtStore()->retrieve(p_rdocontainer, m_collection).isFailure()) 
    {
      return StatusCode::SUCCESS;
    } 
  ++m_numberOfEventsSelected;
  //int NumberModulesVetoed[5]={0,0,0,0,0};
  unsigned int n_trtColl(0);
  unsigned int n_trtCollNonEmpty(0);

  // loop over RDO collection
  TRT_RDO_Container::const_iterator col_it(p_rdocontainer->begin());
  const TRT_RDO_Container::const_iterator lastCol(p_rdocontainer->end());
  while(col_it!= lastCol) 
    {
      const InDetRawDataCollection<TRT_RDORawData>* TRT_Collection(*col_it);
      if(!TRT_Collection) 
	{
	  ++col_it;
	  continue;    // select only TRT RDO's
	}
      ++n_trtColl;
      
      if(TRT_Collection->empty()) 
	{
	  ++col_it;
	  continue;
	}
      ++n_trtCollNonEmpty;

      //bool NoiseOnly(true); // want this to be valid for each module (side in this case)
      //int module_type(-1);
      DataVector<TRT_RDORawData>::const_iterator p_rdo(TRT_Collection->begin());  
      const DataVector<TRT_RDORawData>::const_iterator endOfRDOs(TRT_Collection->end());
      for( ; p_rdo!=endOfRDOs ; ++p_rdo) 
	{
	  //module_type = -1;
	  const Identifier rdoId((*p_rdo)->identify());
	  ATH_MSG_VERBOSE ("RDO ID=" << m_trtID->show_to_string( (*p_rdo)->identify())
			   << ", BarrelEndcap =" << m_trtID->barrel_ec(rdoId) 
			   << ", PhiModule =" << m_trtID->phi_module(rdoId) 
			   << ", LayerWheel =" << m_trtID->layer_or_wheel(rdoId) 
			   << ", StrawLayer =" << m_trtID->straw_layer(rdoId) 
			   << ", Straw =" << m_trtID->straw(rdoId) 
			   << ", timeOverThreshold =" << (*p_rdo)->timeOverThreshold()
			   << ", HighLevel =" << (*p_rdo)->highLevel()
			   << ", driftTimeBin =" << (*p_rdo)->driftTimeBin()
			   << ", trailingEdge =" << trailingEdge(*p_rdo)
			   << ", firstBinHigh =" << firstBinHigh(*p_rdo)
			   << ", lastBinHigh =" << lastBinHigh(*p_rdo)
			   );
	  // Fill histograms
	  const int trtBarrel(m_trtID->barrel_ec(rdoId));
	  const int trtPhiMod(m_trtID->phi_module(rdoId));
	  const int trtLayer (m_trtID->layer_or_wheel(rdoId));
	  const int trtStrawLayer(m_trtID->straw_layer(rdoId));
	  const int trtStraw  (m_trtID->straw(rdoId));
	  const double trtTimeOverThreshold((*p_rdo)->timeOverThreshold());
	  const int trtDriftTimeBin((*p_rdo)->driftTimeBin());
	  const int trtTrailingEdge(trailingEdge(*p_rdo));
	  //const bool trtHighLevel((*p_rdo)->highLevel()); //FIXME need to create appropriate histogram
	  //const bool trtFirstBinHigh(firstBinHigh(*p_rdo)); //FIXME need to create appropriate histogram
	  //const bool trtLastBinHigh (lastBinHigh (*p_rdo)); //FIXME need to create appropriate histogram
	  
	  m_BarrelEndcap->Fill(trtBarrel,1);
	  if(abs(trtBarrel)==1) 
	    {
	      m_BarrelPhiModule->Fill(trtPhiMod,1);
	      m_BarrelLayer->Fill(trtLayer,1);
	      m_BarrelStrawLayer->Fill(trtStrawLayer,1);
	      m_BarrelStraw->Fill(trtStraw,1);
	      m_BarrelTimeOverThreshold->Fill(trtTimeOverThreshold,1);
	      m_BarrelDriftTimeBin->Fill(trtDriftTimeBin,1);
	      m_BarrelTrailingEdge->Fill(trtTrailingEdge,1);
// 	      //Occupancy
// 	      if(trtLayer>=0) m_h_trt_barrel_occ_layer[trtLayer]->Fill(trtEtaMod,trtPhiMod,trtTimeOverThreshold);
// 	      //Check for SDOs in this module.
// 	      int tempEtaMod(trtEtaMod);
// 	      if(tempEtaMod<0) ++tempEtaMod;
// 	      tempEtaMod = tempEtaMod + 5;        // puts straw layer number in range 0-11
// 	      const int index(2*(240*tempEtaMod+60*trtLayer+trtPhiMod)+trtSide);
// 	      if(NoBarrelModules.count(index)==1) NoiseOnly = false;
// 	      if(NoiseOnly)
// 		{
// 		  module_type=4;
// 		  m_TRT_OccupancyByModuleType->Fill(module_type,trtTimeOverThreshold);//Barrel       <-> 4
// 		}
	    }
	  if(abs(trtBarrel)== 2) 
	    {
	      m_EndcapPhiModule->Fill(trtPhiMod,1);
	      m_EndcapLayer->Fill(trtLayer,1);
	      m_EndcapStrawLayer->Fill(trtStrawLayer,1);
	      m_EndcapStraw->Fill(trtStraw,1);
	      m_EndcapTimeOverThreshold->Fill(trtTimeOverThreshold,1);
	      m_EndcapDriftTimeBin->Fill(trtDriftTimeBin,1);
	      m_EndcapTrailingEdge->Fill(trtTrailingEdge,1);

// 	      if(trtBarrel==-2) 
// 		{//Endcap C histograms
// 		  m_TRT_Side->Fill(-10+trtSide,1);
// 		  m_EndcapLayer->Fill(-10+trtLayer,1);
// 		  //Occupancy
// 		  if(trtLayer>=0) 
// 		    {
// 		      m_h_trt_endcapC_occ_layer[trtLayer]->Fill(trtEtaMod,trtPhiMod,trtTimeOverThreshold);
// 		    }
          
// 		}
// 	      if(trtBarrel==2) 
// 		{//Endcap A histograms
// 		  m_TRT_Side->Fill( 10+trtSide,1);
// 		  m_EndcapLayer->Fill( trtLayer,1);
// 		  //Occupancy
// 		  if(trtLayer>=0) 
// 		    {
// 		      m_h_trt_endcapA_occ_layer[trtLayer]->Fill(trtEtaMod,trtPhiMod,trtTimeOverThreshold);
// 		    }
// 		}


// 	      //Module_type  <-> Integer
// 	      //Barrel       <-> 4
// 	      //Inner        <-> 3
// 	      //Middles      <-> 2
// 	      //Short Middles<-> 1
// 	      //Outer        <-> 0

// 	      //TODO loop over SDOs to check this part....
//  	      const int index(2*((1000*(2+trtBarrel))+(60*trtLayer+trtPhiMod))+trtSide);
//  	      if(trtEtaMod==2) 
//  	      	{
//  	      	  module_type = 3; // inner modules
//  	      	  if(NoInnerModules.count(index)==1) NoiseOnly=false;
//  	      	}
//  	      if(trtEtaMod==1)
//  	      	{
//  	      	  module_type = 2; // middle modules
//  	      	  if(trtLayer==7) 
//  	      	    {
//  	      	      module_type = 1; // short middle modules
//  	      	      if(NoShortMiddleModules.count(index)==1) NoiseOnly=false;
//  	      	    }
//  	      	  else
//  	      	    {
//  	      	      if(NoMiddleModules.count(index)==1) NoiseOnly=false;
//  	      	    }
//  	      	}
//  	      if(trtEtaMod==0) 
//  	      	{
//  	      	  module_type = 0; // outer modules
//  	      	  if(NoOuterModules.count(index)==1) NoiseOnly=false;
//  	      	}
//  	      if(NoiseOnly) m_TRT_OccupancyByModuleType->Fill(module_type, trtTimeOverThreshold);
        
	    }

	}
//       if(!NoiseOnly) 
// 	{
// 	  if(module_type!=-1) { NumberModulesVetoed[module_type]+=1; }
// 	}
      
      ++col_it;
    }
  m_nRDO_Colls->Fill(n_trtColl,1);
  m_nEmptyRDO_Colls->Fill(n_trtColl-n_trtCollNonEmpty,1);
  ATH_MSG_DEBUG ("Found " << n_trtColl << " TRT RDO collections, of which " << n_trtCollNonEmpty << " were non-empty"); 
//   ATH_MSG_DEBUG ("Number of Module sides vetoed from noise plots: Outer(" << NumberModulesVetoed[0] << "), Short Middle(" << NumberModulesVetoed[1] 
// 		 << "), Middle(" << NumberModulesVetoed[2] << "), Inner(" << NumberModulesVetoed[3] << "), Barrel(" << NumberModulesVetoed[4] << ") "); 

  return StatusCode::SUCCESS;
}

StatusCode TRT_RDOsTestTool::CheckSDOs() {

//   NoOuterModules.clear();
//   NoShortMiddleModules.clear();
//   NoMiddleModules.clear();
//   NoInnerModules.clear();
//   NoBarrelModules.clear();

//   const InDetSimDataCollection* simDataMapTRT(NULL);
//   if(evtStore()->retrieve(simDataMapTRT,"TRT_SDO_Map").isFailure()) {
//     ATH_MSG_WARNING ("Could not find TRT SDO container");
//     return StatusCode::SUCCESS;
//   }

//   boost::io::ios_all_saver ias( std::cout );

//   std::cout << "  B  L  P  E  S  Str      Status     Evt   Trk      Chg"
//             << std::endl;

//   InDetSimDataCollection::const_iterator itSDO(simDataMapTRT->begin());
//   const InDetSimDataCollection::const_iterator endOfSDOs(simDataMapTRT->end());
//   for(; itSDO!=endOfSDOs; ++itSDO) 
//     {
//       const Identifier sdoId((*itSDO).first);
//       const int trtBarrel(m_trtID->barrel_ec(sdoId));
//       const int trtLayer (m_trtID->layer_disk(sdoId));
//       const int trtEtaMod(m_trtID->eta_module(sdoId));
//       const int trtPhiMod(m_trtID->phi_module(sdoId));
//       const int trtSide  (m_trtID->side(sdoId));
//       //const int trtStrip (m_trtID->strip(sdoId));

//       if(trtBarrel==0)
// 	{
// 	  int tempEtaMod(trtEtaMod);
// 	  if(tempEtaMod<0) ++tempEtaMod;
// 	  tempEtaMod = tempEtaMod + 5;        // puts straw layer number in range 0-11
// 	  const int index(2*(240*tempEtaMod+60*trtLayer+trtPhiMod)+trtSide);
// 	  NoBarrelModules[index]=1;
// 	}
//       if(trtBarrel!= 0) 
// 	{
// 	  //Module_type  <-> Integer
// 	  //Barrel       <-> 4
// 	  //Inner        <-> 3
// 	  //Middles      <-> 2
// 	  //Short Middles<-> 1
// 	  //Outer        <-> 0
// 	  const int index(2*((1000*(2+trtBarrel))+(60*trtLayer+trtPhiMod))+trtSide);
// 	  if(trtEtaMod==2) 
// 	    {
// 	      NoInnerModules[index]=1;
// 	    }
// 	  if(trtEtaMod==1)
// 	    {
// 	      if(trtLayer==7) 
// 		{
// 		  NoShortMiddleModules[index]=1;
// 		}
// 	      else
// 		{
// 		  NoMiddleModules[index]=1;
// 		}
// 	    }
// 	  if(trtEtaMod==0) 
// 	    {
// 	      NoOuterModules[index]=1;
// 	    }
// 	}

//       if(msgLvl(MSG::VERBOSE))
// 	{
// 	  const InDetSimData& sdo((*itSDO).second);
// 	  ATH_MSG_VERBOSE (" TRT SDO first=" << m_trtID->show_to_string(sdoId) << ", word=" << sdo.word());
// 	  const std::vector< std::pair<HepMcParticleLink,float> >& deposits = sdo.getdeposits();

// 	  // Loop over deposits for this SDO
// 	  std::vector< std::pair<HepMcParticleLink,float> >::const_iterator itDep(deposits.begin());
// 	  const std::vector< std::pair<HepMcParticleLink,float> >::const_iterator endOfDeposits(deposits.end());
// 	  for( ; itDep!=endOfDeposits; ++itDep) 
// 	    {
// 	      const HepMcParticleLink& particleLink = (*itDep).first;
// 	      const float charge = (*itDep).second;
// 	      ATH_MSG_VERBOSE (" event index=" << particleLink.eventIndex() << " barcode=" << particleLink.barcode() << ", charge=" << charge);
      
// 	      std::cout << std::setw(3)  << m_trtID->barrel_ec(sdoId)
// 			<< std::setw(3)  << m_trtID->layer_disk(sdoId)
// 			<< std::setw(3)  << m_trtID->phi_module(sdoId)
// 			<< std::setw(3)  << m_trtID->eta_module(sdoId)
// 			<< std::setw(3)  << m_trtID->side(sdoId)
// 			<< std::setw(5)  << m_trtID->strip(sdoId)
// 			<< std::hex << std::showbase
// 			<< std::setw(9)  << sdo.word()
// 			<< std::dec << std::noshowbase
// 			<< std::setw(2)  << TRT_SimHelper::isNoise(sdo)
// 			<< std::setw(2)  << TRT_SimHelper::isBelowThreshold(sdo)
// 			<< std::setw(2)  << TRT_SimHelper::isDisabled(sdo)
// 			<< std::setw(5)  << particleLink.eventIndex()
// 			<< std::setw(7)  << particleLink.barcode()
// 			<< std::fixed << std::showpoint << std::setprecision(1)
// 			<< std::setw(10) << charge
// 			<< std::endl;
// 	      ias.restore();
// 	    }
// 	}
//     }

//   m_NumberModulesVetoed[0] += NoOuterModules.size();
//   m_NumberModulesVetoed[1] += NoShortMiddleModules.size();
//   m_NumberModulesVetoed[2] += NoMiddleModules.size();
//   m_NumberModulesVetoed[3] += NoInnerModules.size();
//   m_NumberModulesVetoed[4] += NoBarrelModules.size();
//   ATH_MSG_DEBUG ("Number of Module sides with True Track Hits: Outer(" << NoOuterModules.size() << "), Short Middle(" << NoShortMiddleModules.size() 
// 		 << "), Middle(" << NoMiddleModules.size() << "), Inner(" << NoInnerModules.size() << "), Barrel(" << NoBarrelModules.size() << ") "); 

  return StatusCode::SUCCESS;
}

double TRT_RDOsTestTool::GetBarrelOccupancy(const TH2* hist, const int layer, const double basescale, double& error)
{
  double occ(0.0);
  double scale(1.0);
  if(layer==0) scale = basescale*32.*12. ;//nphi * neta
  if(layer==1) scale = basescale*40.*12. ;//nphi * neta
  if(layer==2) scale = basescale*48.*12. ;//nphi * neta
  if(layer==3) scale = basescale*56.*12. ;//nphi * neta
  for(int x =1; x<=hist->GetNbinsX(); ++x)
    {
      for(int y =1; y<=hist->GetNbinsY(); ++y)
    {
      double content = hist->GetBinContent(x,y);
      if(content>0.) occ+= content;
    }
    }
  error = pow(occ,0.5);
  ATH_MSG_VERBOSE ("Barrel Layer: "<<layer<<" Occupancy = "<<occ<<" +/- "<<error<<" Scale = "<<scale);
  occ/=scale; error/=scale; 
  occ*=100.; error*=100.; //scale to a percentage 
  ATH_MSG_VERBOSE ("Barrel Layer: "<<layer<<" Percentage Occupancy = "<<occ<<" +/- "<<error);
  return occ;

}

double TRT_RDOsTestTool::GetEndcapOccupancy(const TH2* hist, const int disk, const double basescale, double& error)
{
  double occ(0.0);
  double scale(1.0);
  double nmodules(1.0);
  if(disk==0) nmodules = 92. ; // etamod = [0,1]
  if(disk>0 && disk<6) nmodules = 132. ; // etamod = [0,2]
  if(disk==6 || disk==7) nmodules = 92. ; // etamod = [0,1]
  if(disk==8) nmodules = 52.; // etamod = [0]
  scale = basescale*nmodules;
  for(int x =1; x<=hist->GetNbinsX(); ++x)
    {
      for(int y =1; y<=hist->GetNbinsY(); ++y)
	{
	  double content = hist->GetBinContent(x,y);
	  if(content>0.) occ+= content;
	}
    }
  error = pow(occ,0.5);
  ATH_MSG_VERBOSE ("Endcap Disk: "<<disk<<" No. modules = "<<nmodules<<" Occupancy = "<<occ<<" +/- "<<error<<" Scale = "<<scale);
  occ/=scale;  error/=scale;
  occ*=100.; error*=100.; //scale to a percentage 
  ATH_MSG_VERBOSE ("Endcap Disk: "<<disk<<" No. modules = "<<nmodules<<" Percentage Occupancy = "<<occ<<" +/- "<<error);
  return occ;

}

StatusCode TRT_RDOsTestTool::finalize() 
{
//   double basescale(static_cast<double>(m_numberOfEventsSelected));
//   basescale*=1536.0; // nstrips per _module_
//   m_BarrelOccupancyByLayer->Sumw2();
//   m_BarrelOccupancyByLayer->SetMarkerStyle(20);
//   double error(0.0);
//   for (int i(0); i<4 ; ++i)
//     {
//       m_BarrelOccupancyByLayer->SetBinContent(i+1,GetBarrelOccupancy(m_h_trt_barrel_occ_layer[i],i,basescale, error));
//       m_BarrelOccupancyByLayer->SetBinError(i+1,error);
//     }

//   m_EndCapA_OccupancyByDisk->Sumw2();
//   m_EndCapA_OccupancyByDisk->SetMarkerStyle(20);
//   m_EndCapC_OccupancyByDisk->Sumw2();
//   m_EndCapC_OccupancyByDisk->SetMarkerStyle(20);
//   for (int i(0); i<9; ++i)
//     {
//       m_EndCapA_OccupancyByDisk->SetBinContent(i+1,GetEndcapOccupancy(m_h_trt_endcapA_occ_layer[i],i,basescale, error));
//       m_EndCapA_OccupancyByDisk->SetBinError(i+1,error);
//       m_EndCapC_OccupancyByDisk->SetBinContent(i+1,GetEndcapOccupancy(m_h_trt_endcapC_occ_layer[i],i,basescale, error));
//       m_EndCapC_OccupancyByDisk->SetBinError(i+1,error);
//     }

//   //Module_type  <-> Integer
//   //Barrel       <-> 4
//   //Inner        <-> 3
//   //Middles      <-> 2
//   //Short Middles<-> 1
//   //Outer        <-> 0
//   //No. Outer Modules        = 2*9*52 in total
//   //No. Short Middle Modules = 2*1*40 in total
//   //No. Middle Modules       = 2*7*40 in total
//   //No. Inner Modules        = 2*5*40 in total
//   //No. Barrel Modules        =12*176 in total
//   double No_Modules[5] = {936.0, 80.0, 560.0, 400.0, 2112.0};
//   m_TRT_OccupancyByModuleType->Sumw2();
//   m_TRT_OccupancyByModuleType->SetMarkerStyle(20);
//   for(int i(0); i<5 ; ++i)
//     {
//       double Occupancy(m_TRT_OccupancyByModuleType->GetBinContent(i+1));
//       double Error(std::pow(Occupancy,0.5));
//       double scale(basescale*No_Modules[i]);
//       // correct for the number of strips not used because module side had a track hit
//       scale -= m_NumberModulesVetoed[i]*768.0; //as one side only

//       Occupancy/=scale; Error/=scale;
//       Occupancy*=100.0; Error*=100.0;
//       m_TRT_OccupancyByModuleType->SetBinContent(i+1,Occupancy);
//       m_TRT_OccupancyByModuleType->SetBinError(i+1,Error);
//     }
//   //       ATH_MSG_ERROR("No. Inner Modules Seen = " << NoInnerModules.size());
//   //       ATH_MSG_ERROR("No. Middle Modules Seen = " << NoMiddleModules.size());
//   //       ATH_MSG_ERROR("No. Short Middle Modules Seen = " << NoShortMiddleModules.size());
//   //       ATH_MSG_ERROR("No. Outer Modules Seen = " << NoOuterModules.size());
  return StatusCode::SUCCESS;
}

 
int TRT_RDOsTestTool::trailingEdge(const TRT_RDORawData* r) const 
{
  // trailingEdge available from TRT_LoLumRawData
  //
  int val(0);
  const TRT_LoLumRawData* l = dynamic_cast<const TRT_LoLumRawData*>(r);
  if (l) val = l->trailingEdge();
  return val;
}

bool TRT_RDOsTestTool::firstBinHigh(const TRT_RDORawData* r) const 
{
  bool val(false);
  const TRT_LoLumRawData* l = dynamic_cast<const TRT_LoLumRawData*>(r);
  if (l) val = l->firstBinHigh();
  return val;
}

bool TRT_RDOsTestTool::lastBinHigh(const TRT_RDORawData* r) const 
{
  bool val(false);
  const TRT_LoLumRawData* l = dynamic_cast<const TRT_LoLumRawData*>(r);
  if (l) val = l->lastBinHigh();
  return val;
}

