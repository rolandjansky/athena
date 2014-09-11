/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDOsTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <boost/io/ios_state.hpp>
//#include "HepMC/GenParticle.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/PixelSimHelper.h"
#include <TH2D.h>
#include <TProfile.h>
#include "AthenaKernel/errorcheck.h"

PixelRDOsTestTool::PixelRDOsTestTool(const std::string& type, 
				     const std::string& name,
				     const IInterface* parent)
  : DigiTestToolBase(type, name, parent),
    m_pixelID(NULL),
    m_numberOfEventsSelected(0),
    m_collection("PixelRDOs"),
    m_nRDO_Colls(0),
    m_nEmptyRDO_Colls(0),
    m_BarrelEndcap(0),
    m_BarrelLayer(0),
    m_EndcapLayer(0),
//     m_PixelSide(0),
    m_BarrelEtaModule(0),
    m_EndcapEtaModule(0),
    m_BarrelPhiModule(0),
    m_EndcapPhiModule(0),
    m_BarrelToT(0),
    m_EndcapToT(0),
    m_BarrelBCID(0),
    m_EndcapBCID(0),
    m_BarrelLVL1A(0),
    m_EndcapLVL1A(0),
    m_BarrelLVL1D(0),
    m_EndcapLVL1D(0),

//     m_BarrelStripNumber(0),
//     m_EndcapStripNumber(0),
//     m_BarrelRDOClusterSize(0),
//     m_EndcapRDOClusterSize(0),
    m_BarrelOccupancyByLayer(0),
    m_EndCapA_OccupancyByDisk(0),
    m_EndCapC_OccupancyByDisk(0)//,
//     m_PixelOccupancyByModuleType(0)
{
  for(unsigned int i(0); i<5; ++i) 
    {
      m_h_pixel_endcapA_occ_layer[i]=NULL;
      m_h_pixel_endcapC_occ_layer[i]=NULL;
    }
  for(unsigned int i(0); i<3; ++i) 
    {
      m_h_pixel_barrel_occ_layer[i]=NULL; 
    }
  //   for(unsigned int i(0); i<5; ++i) { m_NumberModulesVetoed[i]=0.0; }

}

StatusCode PixelRDOsTestTool::initialize() 
{
  // get the PIXEL ID Helper
  if (detStore()->retrieve(m_pixelID, "PixelID").isFailure()) 
    {
      return StatusCode::FAILURE;
    }
  
  // initialise pixel or PIXEL variables:
  m_path+="InDet/PIXEL/";
  _TH1D(m_nRDO_Colls, "Number_of_PixelRDO_Collections",1000,0,8000);                         
  _SET_TITLE(m_nRDO_Colls, "Number of PIXEL RDO Collections", "Number of PIXEL RDO Collections", "Frequency");                         
  _TH1D(m_nEmptyRDO_Colls, "Number_of_Empty_PixelRDO_Collections",1000,0,8000);                         
  _SET_TITLE(m_nEmptyRDO_Colls, "Number of Empty PIXEL RDO Collections", "Number of Empty PIXEL RDO Collections", "Frequency");                         
  _TH1D(m_BarrelEndcap, "BarrelEndcap",7,-3.5,3.5);                         
  _SET_TITLE(m_BarrelEndcap, "PIXEL barrel/endcap", "PIXEL barrel/endcap", "Frequency"); 
  _TH1D(m_BarrelLayer, "BarrelLayer", 4,-0.5,3.5);              
  _SET_TITLE(m_BarrelLayer, "PIXEL barrel layer", "Layer", "Frequency");
  _TH1D(m_EndcapLayer, "EndcapLayer",25,-12.5,12.5);                       
  _SET_TITLE(m_EndcapLayer, "PIXEL layer(disk): endcap","PIXEL layer(disk): endcap", "Frequency");
//   _TH1D(m_PixelSide, "PixelSide",25,-12.5,12.5);      
//   _SET_TITLE(m_PixelSide,"PIXEL side: endcap/barrel/endcap","PIXEL side: endcap/barrel/endcap", "Frequency");
  _TH1D(m_BarrelEtaModule, "BarrelEtaModule",20,-10.5,9.5);           
  _SET_TITLE(m_BarrelEtaModule,"PIXEL eta module, barrel","PIXEL eta module, barrel", "Frequency");
  _TH1D(m_EndcapEtaModule, "EndcapEtaModule",20,-10.5,9.5);           
  _SET_TITLE(m_EndcapEtaModule, "PIXEL eta module, endcap","PIXEL eta module, endcap", "Frequency");
  _TH1D(m_BarrelPhiModule, "BarrelPhiModule",100,-0.5,99.5);          
  _SET_TITLE(m_BarrelPhiModule, "PIXEL phi module, barrel","PIXEL phi module, barrel", "Frequency");
  _TH1D(m_EndcapPhiModule, "EndcapPhiModule",100,-0.5,99.5);          
  _SET_TITLE(m_EndcapPhiModule, "PIXEL phi module, endcap","PIXEL phi module, endcap", "Frequency");
//   _TH1D(m_BarrelRDOClusterSize, "BarrelRDOClusterSize",26,-0.5,25.5);             
//   _SET_TITLE(m_BarrelRDOClusterSize, "PIXEL cluster size, barrel","PIXEL cluster size, barrel", "Frequency");
//   _TH1D(m_EndcapRDOClusterSize, "EndcapRDOClusterSize",26,-0.5,25.5);             
//   _SET_TITLE(m_EndcapRDOClusterSize, "PIXEL cluster size, endcap","PIXEL cluster size, endcap", "Frequency");

  _TH1D(m_BarrelToT, "BarrelToT",500,-0.5,499.5); 
  _SET_TITLE(m_BarrelToT, "Pixel ToT, barrel","Pixel ToT, barrel", "Frequency");
  _TH1D(m_EndcapToT, "EndcapToT",500,-0.5,499.5); 
  _SET_TITLE(m_EndcapToT, "Pixel ToT, endcap","Pixel ToT, endcap", "Frequency");

  _TH1D(m_BarrelBCID, "BarrelBCID", 5000,-999.5,4000.5); 
  _SET_TITLE(m_BarrelBCID, "Pixel BCID, barrel","Pixel BCID, barrel", "Frequency");
  _TH1D(m_EndcapBCID, "EndcapBCID", 5000,-999.5,4000.5); 
  _SET_TITLE(m_EndcapBCID, "Pixel BCID, endcap","Pixel BCID, endcap", "Frequency");

  _TH1D(m_BarrelLVL1A, "BarrelLVL1A",791,-10.5,780.5); 
  _SET_TITLE(m_BarrelLVL1A, "Pixel LVL1A, barrel","Pixel LVL1A, barrel", "Frequency");
  _TH1D(m_EndcapLVL1A, "EndcapLVL1A",791,-10.5,780.5); 
  _SET_TITLE(m_EndcapLVL1A, "Pixel LVL1A, endcap","Pixel LVL1A, endcap", "Frequency");

  _TH1D(m_BarrelLVL1D, "BarrelLVL1D",791,-10.5,780.5); 
  _SET_TITLE(m_BarrelLVL1D, "Pixel LVL1D, barrel","Pixel LVL1D, barrel", "Frequency");
  _TH1D(m_EndcapLVL1D, "EndcapLVL1D",791,-10.5,780.5); 
  _SET_TITLE(m_EndcapLVL1D, "Pixel LVL1D, endcap","Pixel LVL1D, endcap", "Frequency");

  _TH1D(m_BarrelOccupancyByLayer   , "BarrelOccupancyByLayer", 3,0.5,3.5);
  _SET_TITLE(m_BarrelOccupancyByLayer   , "PIXEL Barrel Occupancy By Layer (%)", "Layer", "PIXEL Barrel Occupancy [%]");
  _TH2D(m_h_pixel_barrel_occ_layer[0], "h_pixel_lay0_occupancy", 13, -6.5, 6.5, 32, 0, 32);
  _TH2D(m_h_pixel_barrel_occ_layer[1], "h_pixel_lay1_occupancy", 13, -6.5, 6.5, 40, 0, 40);
  _TH2D(m_h_pixel_barrel_occ_layer[2], "h_pixel_lay2_occupancy", 13, -6.5, 6.5, 48, 0, 48);
  
  _TH1D(m_EndCapA_OccupancyByDisk, "EndCapA_OccupancyByDisk", 5,0.5,5.5);
  _SET_TITLE(m_EndCapA_OccupancyByDisk, "PIXEL EndcapA Occupancy By Disk (%)", "Disk", "PIXEL Endcap A Occupancy [%]");
  _TH2D(m_h_pixel_endcapA_occ_layer[0], "h_pixel_ecA_dsk0_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapA_occ_layer[1], "h_pixel_ecA_dsk1_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapA_occ_layer[2], "h_pixel_ecA_dsk2_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapA_occ_layer[3], "h_pixel_ecA_dsk3_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapA_occ_layer[4], "h_pixel_ecA_dsk4_occupancy", 3,-0.5,2.5,52,-0.5,51.5);

  _TH1D(m_EndCapC_OccupancyByDisk   , "EndCapC_OccupancyByDisk", 5,0.5,5.5);
  _SET_TITLE(m_EndCapC_OccupancyByDisk, "PIXEL EndcapC Occupancy By Disk (%)", "Disk", "PIXEL Endcap C Occupancy [%]");
  _TH2D(m_h_pixel_endcapC_occ_layer[0], "h_pixel_ecC_dsk0_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapC_occ_layer[1], "h_pixel_ecC_dsk1_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapC_occ_layer[2], "h_pixel_ecC_dsk2_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapC_occ_layer[3], "h_pixel_ecC_dsk3_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_pixel_endcapC_occ_layer[4], "h_pixel_ecC_dsk4_occupancy", 3,-0.5,2.5,52,-0.5,51.5);

//   _TH1D(m_PixelOccupancyByModuleType, "PixelOccupancyByModuleType", 5,-0.5,4.5);
//   _SET_TITLE(m_PixelOccupancyByModuleType, "PIXEL Occupancy By Module Type","Types: Outer(0), Short-Middle(1), Middle(2), Inner(3), Barrel(4)", "Occupancy [%]");

  return StatusCode::SUCCESS;
}

StatusCode PixelRDOsTestTool::processEvent() {

  CHECK(this->CheckSDOs());
  const PixelRDO_Container* p_rdocontainer;
  if(evtStore()->retrieve(p_rdocontainer, m_collection).isFailure()) 
    {
      return StatusCode::SUCCESS;
    } 
  //int NumberModulesVetoed[5]={0,0,0,0,0};
  unsigned int n_pixelColl(0);
  unsigned int n_pixelCollNonEmpty(0);

  // loop over RDO collection
  PixelRDO_Container::const_iterator col_it(p_rdocontainer->begin());
  const PixelRDO_Container::const_iterator lastCol(p_rdocontainer->end());
  while(col_it!= lastCol) 
    {
      const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*col_it);
      if(!PixelCollection) 
	{
	  ++col_it;
	  continue;    // select only Pixel RDO's
	}
      ++n_pixelColl;
      
      if(PixelCollection->empty()) 
	{
	  ++col_it;
	  continue;
	}
      ++n_pixelCollNonEmpty;

      //bool NoiseOnly(true); // want this to be valid for each module (side in this case)
      //int module_type(-1);
      DataVector<PixelRDORawData>::const_iterator p_rdo(PixelCollection->begin());  
      const DataVector<PixelRDORawData>::const_iterator endOfRDOs(PixelCollection->end());
      for( ; p_rdo!=endOfRDOs ; ++p_rdo) 
	{
	  //module_type = -1;
	  const Identifier rdoId((*p_rdo)->identify());
	  ATH_MSG_VERBOSE ("RDO ID=" << m_pixelID->show_to_string( (*p_rdo)->identify())
			   << ", BarrelEndcap =" << m_pixelID->barrel_ec(rdoId) 
			   << ", LayerWheel =" << m_pixelID->layer_disk(rdoId) 
			   << ", EtaModule =" << m_pixelID->eta_module(rdoId) 
			   << ", PhiModule =" << m_pixelID->phi_module(rdoId) 
			   );
	  // Fill histograms
	  const int pixelBarrel(m_pixelID->barrel_ec(rdoId));
	  const int pixelLayer (m_pixelID->layer_disk(rdoId));
	  const int pixelEtaMod(m_pixelID->eta_module(rdoId));
	  const int pixelPhiMod(m_pixelID->phi_module(rdoId));
	  m_BarrelEndcap->Fill(pixelBarrel,1);
	  if(pixelBarrel==0) 
	    {
	      m_BarrelLayer->Fill(pixelLayer,1);
	      m_BarrelEtaModule->Fill(pixelEtaMod,1);
	      m_BarrelPhiModule->Fill(pixelPhiMod,1);

	      m_BarrelToT->Fill((*p_rdo)->getToT(),1);
	      m_BarrelBCID->Fill((*p_rdo)->getBCID(),1);
	      m_BarrelLVL1A->Fill((*p_rdo)->getLVL1A(),1);
	      m_BarrelLVL1D->Fill((*p_rdo)->getLVL1ID(),1);

	      //Occupancy
	      if(pixelLayer>=0) m_h_pixel_barrel_occ_layer[pixelLayer]->Fill(pixelEtaMod,pixelPhiMod,1);
	      //Check for SDOs in this module.
	      // int tempEtaMod(pixelEtaMod);
	      // if(tempEtaMod<0) ++tempEtaMod;
	      // tempEtaMod = tempEtaMod + 5;        // puts eta module number in range 0-11
	      // const int index(2*(240*tempEtaMod+60*pixelLayer+pixelPhiMod)+pixelSide);
	      // if(NoBarrelModules.count(index)==1) NoiseOnly = false;
	      // if(NoiseOnly)
	      // {
	      //  module_type=4;
	      //  m_PixelOccupancyByModuleType->Fill(module_type,clusterSize);//Barrel       <-> 4
	      // }
	    }
	  if(pixelBarrel!= 0) 
	    {
	      m_EndcapLayer->Fill(pixelLayer,1);
	      m_EndcapEtaModule->Fill(pixelEtaMod,1);
	      m_EndcapPhiModule->Fill(pixelPhiMod,1);
	      m_EndcapToT->Fill((*p_rdo)->getToT(),1);
	      m_EndcapBCID->Fill((*p_rdo)->getBCID(),1);
	      m_EndcapLVL1A->Fill((*p_rdo)->getLVL1A(),1);
	      m_EndcapLVL1D->Fill((*p_rdo)->getLVL1ID(),1);

	      if(pixelBarrel==-2) 
		{//Endcap C histograms
		  //m_PixelSide->Fill(-10+pixelSide,1);
		  m_EndcapLayer->Fill(-10+pixelLayer,1);
		  //Occupancy
		  if(pixelLayer>=0) 
		    {
		      m_h_pixel_endcapC_occ_layer[pixelLayer]->Fill(pixelEtaMod,pixelPhiMod,1);
		    }
		}
	      if(pixelBarrel==2) 
		{//Endcap A histograms
		  //m_PixelSide->Fill( 10+pixelSide,1);
		  m_EndcapLayer->Fill( pixelLayer,1);
		  //Occupancy
		  if(pixelLayer>=0) 
		    {
		      m_h_pixel_endcapA_occ_layer[pixelLayer]->Fill(pixelEtaMod,pixelPhiMod,1);
		    }
		}


	      //Module_type  <-> Integer
	      //Barrel       <-> 4
	      //Inner        <-> 3
	      //Middles      <-> 2
	      //Short Middles<-> 1
	      //Outer        <-> 0

	      //TODO loop over SDOs to check this part....
 	      // const int index(2*((1000*(2+pixelBarrel))+(60*pixelLayer+pixelPhiMod))+pixelSide);
 	      // if(pixelEtaMod==2) 
 	      // 	{
 	      // 	  module_type = 3; // inner modules
 	      // 	  if(NoInnerModules.count(index)==1) NoiseOnly=false;
 	      // 	}
 	      // if(pixelEtaMod==1)
 	      // 	{
 	      // 	  module_type = 2; // middle modules
 	      // 	  if(pixelLayer==7) 
 	      // 	    {
 	      // 	      module_type = 1; // short middle modules
 	      // 	      if(NoShortMiddleModules.count(index)==1) NoiseOnly=false;
 	      // 	    }
 	      // 	  else
 	      // 	    {
 	      // 	      if(NoMiddleModules.count(index)==1) NoiseOnly=false;
 	      // 	    }
 	      // 	}
 	      // if(pixelEtaMod==0) 
 	      // 	{
 	      // 	  module_type = 0; // outer modules
 	      // 	  if(NoOuterModules.count(index)==1) NoiseOnly=false;
 	      // 	}
	      // if(NoiseOnly) m_PixelOccupancyByModuleType->Fill(module_type, clusterSize);
        
	    }

	}
//       if(!NoiseOnly) 
// 	{
// 	  if(module_type!=-1) { NumberModulesVetoed[module_type]+=1; }
// 	}
      
      ++col_it;
    }
  m_nRDO_Colls->Fill(n_pixelColl,1);
  m_nEmptyRDO_Colls->Fill(n_pixelColl-n_pixelCollNonEmpty,1);
  //ATH_MSG_DEBUG ("Found " << n_pixelColl << " PIXEL RDO collections, of which " << n_pixelCollNonEmpty << " were non-empty"); 
  //ATH_MSG_DEBUG ("Number of Module sides vetoed from noise plots: Outer(" << NumberModulesVetoed[0] << "), Short Middle(" << NumberModulesVetoed[1] 
  //		 << "), Middle(" << NumberModulesVetoed[2] << "), Inner(" << NumberModulesVetoed[3] << "), Barrel(" << NumberModulesVetoed[4] << ") "); 

  return StatusCode::SUCCESS;
}

StatusCode PixelRDOsTestTool::CheckSDOs() {
  //Fill this in once more info is available
  return StatusCode::SUCCESS;
}

double PixelRDOsTestTool::GetBarrelOccupancy(const TH2* hist, const int layer, const double basescale, double& error)
{
  double occ(0.0);
  double scale(1.0);
  if(layer==0) scale = basescale*22.*7. ;//nphi * neta
  if(layer==1) scale = basescale*40.*7. ;//nphi * neta
  if(layer==2) scale = basescale*46.*7. ;//nphi * neta
  for(int x =1; x<=hist->GetNbinsX(); ++x)
    {
      for(int y =1; y<=hist->GetNbinsY(); ++y)
    {
      double content = hist->GetBinContent(x,y);
      if(content>0.) occ+= content;
    }
    }
  error = std::pow(occ,0.5);
  ATH_MSG_VERBOSE ("Barrel Layer: "<<layer<<" Occupancy = "<<occ<<" +/- "<<error<<" Scale = "<<scale);
  occ/=scale; error/=scale; 
  occ*=100.; error*=100.; //scale to a percentage 
  ATH_MSG_VERBOSE ("Barrel Layer: "<<layer<<" Percentage Occupancy = "<<occ<<" +/- "<<error);
  return occ;

}

double PixelRDOsTestTool::GetEndcapOccupancy(const TH2* hist, const int disk, const double basescale, double& error)
{
  double occ(0.0);
  double scale(1.0);
  double nmodules(1.0);
  if(disk<3) nmodules = 54. ;
  else nmodules = 66. ;
  scale = basescale*nmodules;
  for(int x =1; x<=hist->GetNbinsX(); ++x)
    {
      for(int y =1; y<=hist->GetNbinsY(); ++y)
	{
	  double content = hist->GetBinContent(x,y);
	  if(content>0.) occ+= content;
	}
    }
  error = std::pow(occ,0.5);
  ATH_MSG_VERBOSE ("Endcap Disk: "<<disk<<" No. modules = "<<nmodules<<" Occupancy = "<<occ<<" +/- "<<error<<" Scale = "<<scale);
  occ/=scale;  error/=scale;
  occ*=100.; error*=100.; //scale to a percentage 
  ATH_MSG_VERBOSE ("Endcap Disk: "<<disk<<" No. modules = "<<nmodules<<" Percentage Occupancy = "<<occ<<" +/- "<<error);
  return occ;

}

StatusCode PixelRDOsTestTool::finalize() 
{
  double basescale(static_cast<double>(m_numberOfEventsSelected));
  basescale*=328.0*144.0; // npixels per _module_ (for all except barrel layer 0);
  m_BarrelOccupancyByLayer->Sumw2();
  m_BarrelOccupancyByLayer->SetMarkerStyle(20);
  double error(0.0);
  for (int i(0); i<3 ; ++i)
    {
      const double scale = (i==0) ? static_cast<double>(m_numberOfEventsSelected)*320.0*192.0: basescale;
      m_BarrelOccupancyByLayer->SetBinContent(i+1,GetBarrelOccupancy(m_h_pixel_barrel_occ_layer[i],i,scale, error));
      m_BarrelOccupancyByLayer->SetBinError(i+1,error);
    }

  m_EndCapA_OccupancyByDisk->Sumw2();
  m_EndCapA_OccupancyByDisk->SetMarkerStyle(20);
  m_EndCapC_OccupancyByDisk->Sumw2();
  m_EndCapC_OccupancyByDisk->SetMarkerStyle(20);
  for (int i(0); i<5; ++i)
    {
      m_EndCapA_OccupancyByDisk->SetBinContent(i+1,GetEndcapOccupancy(m_h_pixel_endcapA_occ_layer[i],i,basescale, error));
      m_EndCapA_OccupancyByDisk->SetBinError(i+1,error);
      m_EndCapC_OccupancyByDisk->SetBinContent(i+1,GetEndcapOccupancy(m_h_pixel_endcapC_occ_layer[i],i,basescale, error));
      m_EndCapC_OccupancyByDisk->SetBinError(i+1,error);
    }

  return StatusCode::SUCCESS;
}
