/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RDOsTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <boost/io/ios_state.hpp>
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimData/SCT_SimHelper.h"
#include <TH2D.h>
#include <TProfile.h>
#include "AthenaKernel/errorcheck.h"

SCT_RDOsTestTool::SCT_RDOsTestTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
  : DigiTestToolBase(type, name, parent),
    m_sctID(nullptr),
    m_numberOfEventsSelected(0),
    m_collection("SCT_RDOs"),
    m_nRDO_Colls(0),
    m_nEmptyRDO_Colls(0),
    m_BarrelEndcap(0),
    m_BarrelLayer(0),
    m_EndcapLayer(0),
    m_SCT_Side(0),
    m_BarrelEtaModule(0),
    m_EndcapEtaModule(0),
    m_BarrelPhiModule(0),
    m_EndcapPhiModule(0),
    m_BarrelStripNumber(0),
    m_EndcapStripNumber(0),
    m_BarrelRDOClusterSize(0),
    m_EndcapRDOClusterSize(0),
    m_BarrelOccupancyByLayer(0),
    m_EndCapA_OccupancyByDisk(0),
    m_EndCapC_OccupancyByDisk(0),
    m_SCT_OccupancyByModuleType(0)
{
  for(unsigned int i(0); i<9; ++i)
    {
      m_h_sct_endcapA_occ_layer[i]=nullptr;
      m_h_sct_endcapC_occ_layer[i]=nullptr;
    }
  for(unsigned int i(0); i<4; ++i) { m_h_sct_barrel_occ_layer[i]=nullptr; }
  for(unsigned int i(0); i<5; ++i) { m_NumberModulesVetoed[i]=0.0; }

}

StatusCode SCT_RDOsTestTool::initialize()
{
  // get the SCT ID Helper
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure())
    {
      return StatusCode::FAILURE;
    }

  // initialise pixel or SCT variables:
  m_path+="InDet/SCT/";
  _TH1D(m_nRDO_Colls, "Number_of_SCT_RDO_Collections",1000,0,8000);
  _SET_TITLE(m_nRDO_Colls, "Number of SCT RDO Collections", "Number of SCT RDO Collections", "Frequency");
  _TH1D(m_nEmptyRDO_Colls, "Number_of_Empty_SCT_RDO_Collections",1000,0,8000);
  _SET_TITLE(m_nEmptyRDO_Colls, "Number of Empty SCT RDO Collections", "Number of Empty SCT RDO Collections", "Frequency");
  _TH1D(m_BarrelEndcap, "BarrelEndcap",10,-5.5,4.5);
  _SET_TITLE(m_BarrelEndcap, "SCT barrel/endcap", "SCT barrel/endcap", "Frequency");
  _TH1D(m_BarrelLayer, "BarrelLayer", 10,-0.5,9.5);
  _SET_TITLE(m_BarrelLayer, "SCT barrel layer", "Layer", "Frequency");
  _TH1D(m_EndcapLayer, "EndcapLayer",25,-12.5,12.5);
  _SET_TITLE(m_EndcapLayer, "SCT layer(disk): endcap","SCT layer(disk): endcap", "Frequency");
  _TH1D(m_SCT_Side, "SCT_Side",25,-12.5,12.5);
  _SET_TITLE(m_SCT_Side,"SCT side: endcap/barrel/endcap","SCT side: endcap/barrel/endcap", "Frequency");
  _TH1D(m_BarrelEtaModule, "BarrelEtaModule",20,-10.5,9.5);
  _SET_TITLE(m_BarrelEtaModule,"SCT eta module, barrel","SCT eta module, barrel", "Frequency");
  _TH1D(m_EndcapEtaModule, "EndcapEtaModule",20,-10.5,9.5);
  _SET_TITLE(m_EndcapEtaModule, "SCT eta module, endcap","SCT eta module, endcap", "Frequency");
  _TH1D(m_BarrelPhiModule, "BarrelPhiModule",100,-0.5,99.5);
  _SET_TITLE(m_BarrelPhiModule, "SCT phi module, barrel","SCT phi module, barrel", "Frequency");
  _TH1D(m_EndcapPhiModule, "EndcapPhiModule",100,-0.5,99.5);
  _SET_TITLE(m_EndcapPhiModule, "SCT phi module, endcap","SCT phi module, endcap", "Frequency");
  _TH1D(m_BarrelStripNumber, "BarrelStripNumber",791,-10.5,780.5); //fill for all strips in an RDO cluster
  _SET_TITLE(m_BarrelStripNumber, "SCT strip number, barrel","SCT strip number, barrel", "Frequency");
  _TH1D(m_EndcapStripNumber, "EndcapStripNumber",791,-10.5,780.5); //fill for all strips in an RDO cluster
  _SET_TITLE(m_EndcapStripNumber, "SCT strip number, endcap","SCT strip number, endcap", "Frequency");
  _TH1D(m_BarrelRDOClusterSize, "BarrelRDOClusterSize",26,-0.5,25.5);
  _SET_TITLE(m_BarrelRDOClusterSize, "SCT cluster size, barrel","SCT cluster size, barrel", "Frequency");
  _TH1D(m_EndcapRDOClusterSize, "EndcapRDOClusterSize",26,-0.5,25.5);
  _SET_TITLE(m_EndcapRDOClusterSize, "SCT cluster size, endcap","SCT cluster size, endcap", "Frequency");
  _TH1D(m_BarrelOccupancyByLayer   , "BarrelOccupancyByLayer", 4,0.5,4.5);
  _SET_TITLE(m_BarrelOccupancyByLayer   , "SCT Barrel Occupancy By Layer (%)", "Layer", "SCT Barrel Occupancy [%]");
  _TH2D(m_h_sct_barrel_occ_layer[0], "h_sct_lay0_occupancy", 13, -6.5, 6.5, 32, 0, 32);
  _TH2D(m_h_sct_barrel_occ_layer[1], "h_sct_lay1_occupancy", 13, -6.5, 6.5, 40, 0, 40);
  _TH2D(m_h_sct_barrel_occ_layer[2], "h_sct_lay2_occupancy", 13, -6.5, 6.5, 48, 0, 48);
  _TH2D(m_h_sct_barrel_occ_layer[3], "h_sct_lay3_occupancy", 13, -6.5, 6.5, 56, 0, 56);

  _TH1D(m_EndCapA_OccupancyByDisk, "EndCapA_OccupancyByDisk", 9,0.5,9.5);
  _SET_TITLE(m_EndCapA_OccupancyByDisk, "SCT EndcapA Occupancy By Disk (%)", "Disk", "SCT Endcap A Occupancy [%]");
  _TH2D(m_h_sct_endcapA_occ_layer[0], "h_sct_ecA_dsk0_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[1], "h_sct_ecA_dsk1_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[2], "h_sct_ecA_dsk2_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[3], "h_sct_ecA_dsk3_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[4], "h_sct_ecA_dsk4_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[5], "h_sct_ecA_dsk5_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[6], "h_sct_ecA_dsk6_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[7], "h_sct_ecA_dsk7_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapA_occ_layer[8], "h_sct_ecA_dsk8_occupancy", 1,-0.5,0.5,52,-0.5,51.5);

  _TH1D(m_EndCapC_OccupancyByDisk   , "EndCapC_OccupancyByDisk", 9,0.5,9.5);
  _SET_TITLE(m_EndCapC_OccupancyByDisk, "SCT EndcapC Occupancy By Disk (%)", "Disk", "SCT Endcap C Occupancy [%]");
  _TH2D(m_h_sct_endcapC_occ_layer[0], "h_sct_ecC_dsk0_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[1], "h_sct_ecC_dsk1_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[2], "h_sct_ecC_dsk2_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[3], "h_sct_ecC_dsk3_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[4], "h_sct_ecC_dsk4_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[5], "h_sct_ecC_dsk5_occupancy", 3,-0.5,2.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[6], "h_sct_ecC_dsk6_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[7], "h_sct_ecC_dsk7_occupancy", 2,-0.5,1.5,52,-0.5,51.5);
  _TH2D(m_h_sct_endcapC_occ_layer[8], "h_sct_ecC_dsk8_occupancy", 1,-0.5,0.5,52,-0.5,51.5);

  _TH1D(m_SCT_OccupancyByModuleType, "SCT_OccupancyByModuleType", 5,-0.5,4.5);
  _SET_TITLE(m_SCT_OccupancyByModuleType, "SCT Occupancy By Module Type","Types: Outer(0), Short-Middle(1), Middle(2), Inner(3), Barrel(4)", "Occupancy [%]");

  return StatusCode::SUCCESS;
}

StatusCode SCT_RDOsTestTool::processEvent() {

  CHECK(this->CheckSDOs());
  const SCT_RDO_Container* p_rdocontainer;
  if(evtStore()->retrieve(p_rdocontainer, m_collection).isFailure())
    {
      return StatusCode::SUCCESS;
    }
  ++m_numberOfEventsSelected;
  int NumberModulesVetoed[5]={0,0,0,0,0};
  unsigned int n_sctColl(0);
  unsigned int n_sctCollNonEmpty(0);

  // loop over RDO collection
  SCT_RDO_Container::const_iterator col_it(p_rdocontainer->begin());
  const SCT_RDO_Container::const_iterator lastCol(p_rdocontainer->end());
  while(col_it!= lastCol)
    {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*col_it);
      if(!SCT_Collection)
        {
          ++col_it;
          continue;    // select only SCT RDO's
        }
      ++n_sctColl;

      if(SCT_Collection->empty())
        {
          ++col_it;
          continue;
        }
      ++n_sctCollNonEmpty;

      bool NoiseOnly(true); // want this to be valid for each module (side in this case)
      int module_type(-1);
      DataVector<SCT_RDORawData>::const_iterator p_rdo(SCT_Collection->begin());
      const DataVector<SCT_RDORawData>::const_iterator endOfRDOs(SCT_Collection->end());
      for( ; p_rdo!=endOfRDOs ; ++p_rdo)
        {
          module_type = -1;
          const Identifier rdoId((*p_rdo)->identify());
          ATH_MSG_VERBOSE ("RDO ID=" << m_sctID->show_to_string( (*p_rdo)->identify())
                           << ", BarrelEndcap =" << m_sctID->barrel_ec(rdoId)
                           << ", LayerWheel =" << m_sctID->layer_disk(rdoId)
                           << ", EtaModule =" << m_sctID->eta_module(rdoId)
                           << ", PhiModule =" << m_sctID->phi_module(rdoId)
                           << ", Side =" << m_sctID->side(rdoId)
                           << ", Strip =" << m_sctID->strip(rdoId)
                           << ", Cluster size =" << (*p_rdo)->getGroupSize()
                           );
          // Fill histograms
          const int sctBarrel(m_sctID->barrel_ec(rdoId));
          const int sctLayer (m_sctID->layer_disk(rdoId));
          const int sctEtaMod(m_sctID->eta_module(rdoId));
          const int sctPhiMod(m_sctID->phi_module(rdoId));
          const int sctSide  (m_sctID->side(rdoId));
          const int clusterSize((*p_rdo)->getGroupSize());
          m_BarrelEndcap->Fill(sctBarrel,1);
          if(sctBarrel==0)
            {
              m_BarrelLayer->Fill(sctLayer,1);
              m_BarrelEtaModule->Fill(sctEtaMod,1);
              m_BarrelPhiModule->Fill(sctPhiMod,1);
              m_SCT_Side->Fill(sctSide,1);
              //Add an entry for all strips represented by each RDO
              const int firstStrip(m_sctID->strip(rdoId));
              for(int clusterStrip(0); clusterStrip<clusterSize; ++clusterStrip)
                {
                  m_BarrelStripNumber->Fill((firstStrip+clusterStrip),1);
                }
              m_BarrelRDOClusterSize->Fill(clusterSize,1);
              //Occupancy
              if(sctLayer>=0) m_h_sct_barrel_occ_layer[sctLayer]->Fill(sctEtaMod,sctPhiMod,clusterSize);
              //Check for SDOs in this module.
              int tempEtaMod(sctEtaMod);
              if(tempEtaMod<0) ++tempEtaMod;
              tempEtaMod = tempEtaMod + 5;        // puts eta module number in range 0-11
              const int index(2*(240*tempEtaMod+60*sctLayer+sctPhiMod)+sctSide);
              if(m_NoBarrelModules.count(index)==1) NoiseOnly = false;
              if(NoiseOnly)
                {
                  module_type=4;
                  m_SCT_OccupancyByModuleType->Fill(module_type,clusterSize);//Barrel       <-> 4
                }
            }
          if(sctBarrel!= 0)
            {
              m_EndcapLayer->Fill(sctLayer,1);
              m_EndcapEtaModule->Fill(sctEtaMod,1);
              m_EndcapPhiModule->Fill(sctPhiMod,1);
              //Add an entry for all strips represented by each RDO
              const int firstStrip(m_sctID->strip(rdoId));
              for(int clusterStrip(0); clusterStrip<clusterSize; ++clusterStrip)
                {
                  m_EndcapStripNumber->Fill((firstStrip+clusterStrip),1);
                }
              m_EndcapRDOClusterSize->Fill(clusterSize,1);

              if(sctBarrel==-2)
                {//Endcap C histograms
                  m_SCT_Side->Fill(-10+sctSide,1);
                  m_EndcapLayer->Fill(-10+sctLayer,1);
                  //Occupancy
                  if(sctLayer>=0)
                    {
                      m_h_sct_endcapC_occ_layer[sctLayer]->Fill(sctEtaMod,sctPhiMod,clusterSize);
                    }

                }
              if(sctBarrel==2)
                {//Endcap A histograms
                  m_SCT_Side->Fill( 10+sctSide,1);
                  m_EndcapLayer->Fill( sctLayer,1);
                  //Occupancy
                  if(sctLayer>=0)
                    {
                      m_h_sct_endcapA_occ_layer[sctLayer]->Fill(sctEtaMod,sctPhiMod,clusterSize);
                    }
                }


              //Module_type  <-> Integer
              //Barrel       <-> 4
              //Inner        <-> 3
              //Middles      <-> 2
              //Short Middles<-> 1
              //Outer        <-> 0

              //TODO loop over SDOs to check this part....
              const int index(2*((1000*(2+sctBarrel))+(60*sctLayer+sctPhiMod))+sctSide);
              if(sctEtaMod==2)
                {
                  module_type = 3; // inner modules
                  if(m_NoInnerModules.count(index)==1) NoiseOnly=false;
                }
              if(sctEtaMod==1)
                {
                  module_type = 2; // middle modules
                  if(sctLayer==7)
                    {
                      module_type = 1; // short middle modules
                      if(m_NoShortMiddleModules.count(index)==1) NoiseOnly=false;
                    }
                  else
                    {
                      if(m_NoMiddleModules.count(index)==1) NoiseOnly=false;
                    }
                }
              if(sctEtaMod==0)
                {
                  module_type = 0; // outer modules
                  if(m_NoOuterModules.count(index)==1) NoiseOnly=false;
                }
              if(NoiseOnly) m_SCT_OccupancyByModuleType->Fill(module_type, clusterSize);

            }

        }
      if(!NoiseOnly)
        {
          if(module_type!=-1) { NumberModulesVetoed[module_type]+=1; }
        }

      ++col_it;
    }
  m_nRDO_Colls->Fill(n_sctColl,1);
  m_nEmptyRDO_Colls->Fill(n_sctColl-n_sctCollNonEmpty,1);
  ATH_MSG_DEBUG ("Found " << n_sctColl << " SCT RDO collections, of which " << n_sctCollNonEmpty << " were non-empty");
  ATH_MSG_DEBUG ("Number of Module sides vetoed from noise plots: Outer(" << NumberModulesVetoed[0] << "), Short Middle(" << NumberModulesVetoed[1]
                 << "), Middle(" << NumberModulesVetoed[2] << "), Inner(" << NumberModulesVetoed[3] << "), Barrel(" << NumberModulesVetoed[4] << ") ");

  return StatusCode::SUCCESS;
}

StatusCode SCT_RDOsTestTool::CheckSDOs() {

  m_NoOuterModules.clear();
  m_NoShortMiddleModules.clear();
  m_NoMiddleModules.clear();
  m_NoInnerModules.clear();
  m_NoBarrelModules.clear();

  const InDetSimDataCollection* simDataMapSCT(nullptr);
  if(evtStore()->retrieve(simDataMapSCT,"SCT_SDO_Map").isFailure()) {
    ATH_MSG_WARNING ("Could not find SCT SDO container");
    return StatusCode::SUCCESS;
  }

  boost::io::ios_all_saver ias( std::cout );

  if(msgLvl(MSG::VERBOSE))
    {
      std::cout << "  B  L  P  E  S  Str      Status     Evt   Trk      Chg"
                << std::endl;
    }
  InDetSimDataCollection::const_iterator itSDO(simDataMapSCT->begin());
  const InDetSimDataCollection::const_iterator endOfSDOs(simDataMapSCT->end());
  for(; itSDO!=endOfSDOs; ++itSDO)
    {
      const Identifier sdoId((*itSDO).first);
      const int sctBarrel(m_sctID->barrel_ec(sdoId));
      const int sctLayer (m_sctID->layer_disk(sdoId));
      const int sctEtaMod(m_sctID->eta_module(sdoId));
      const int sctPhiMod(m_sctID->phi_module(sdoId));
      const int sctSide  (m_sctID->side(sdoId));
      //const int sctStrip (m_sctID->strip(sdoId));

      if(sctBarrel==0)
        {
          int tempEtaMod(sctEtaMod);
          if(tempEtaMod<0) ++tempEtaMod;
          tempEtaMod = tempEtaMod + 5;        // puts eta module number in range 0-11
          const int index(2*(240*tempEtaMod+60*sctLayer+sctPhiMod)+sctSide);
          m_NoBarrelModules[index]=1;
        }
      if(sctBarrel!= 0)
        {
          //Module_type  <-> Integer
          //Barrel       <-> 4
          //Inner        <-> 3
          //Middles      <-> 2
          //Short Middles<-> 1
          //Outer        <-> 0
          const int index(2*((1000*(2+sctBarrel))+(60*sctLayer+sctPhiMod))+sctSide);
          if(sctEtaMod==2)
            {
              m_NoInnerModules[index]=1;
            }
          if(sctEtaMod==1)
            {
              if(sctLayer==7)
                {
                  m_NoShortMiddleModules[index]=1;
                }
              else
                {
                  m_NoMiddleModules[index]=1;
                }
            }
          if(sctEtaMod==0)
            {
              m_NoOuterModules[index]=1;
            }
        }

      if(msgLvl(MSG::VERBOSE))
        {
          const InDetSimData& sdo((*itSDO).second);
          ATH_MSG_VERBOSE (" SCT SDO first=" << m_sctID->show_to_string(sdoId) << ", word=" << sdo.word());
          const std::vector< std::pair<HepMcParticleLink,float> >& deposits = sdo.getdeposits();

          // Loop over deposits for this SDO
          std::vector< std::pair<HepMcParticleLink,float> >::const_iterator itDep(deposits.begin());
          const std::vector< std::pair<HepMcParticleLink,float> >::const_iterator endOfDeposits(deposits.end());
          for( ; itDep!=endOfDeposits; ++itDep)
            {
              const HepMcParticleLink& particleLink = (*itDep).first;
              const float charge = (*itDep).second;
              ATH_MSG_VERBOSE (" event index=" << particleLink.eventIndex() << " barcode=" << particleLink.barcode() << ", charge=" << charge);

              std::cout << std::setw(3)  << m_sctID->barrel_ec(sdoId)
                        << std::setw(3)  << m_sctID->layer_disk(sdoId)
                        << std::setw(3)  << m_sctID->phi_module(sdoId)
                        << std::setw(3)  << m_sctID->eta_module(sdoId)
                        << std::setw(3)  << m_sctID->side(sdoId)
                        << std::setw(5)  << m_sctID->strip(sdoId)
                        << std::hex << std::showbase
                        << std::setw(9)  << sdo.word()
                        << std::dec << std::noshowbase
                        << std::setw(2)  << SCT_SimHelper::isNoise(sdo)
                        << std::setw(2)  << SCT_SimHelper::isBelowThreshold(sdo)
                        << std::setw(2)  << SCT_SimHelper::isDisabled(sdo)
                        << std::setw(5)  << particleLink.eventIndex()
                        << std::setw(7)  << particleLink.barcode()
                        << std::fixed << std::showpoint << std::setprecision(1)
                        << std::setw(10) << charge
                        << std::endl;
              ias.restore();
            }
        }
    }

  m_NumberModulesVetoed[0] += m_NoOuterModules.size();
  m_NumberModulesVetoed[1] += m_NoShortMiddleModules.size();
  m_NumberModulesVetoed[2] += m_NoMiddleModules.size();
  m_NumberModulesVetoed[3] += m_NoInnerModules.size();
  m_NumberModulesVetoed[4] += m_NoBarrelModules.size();
  ATH_MSG_DEBUG ("Number of Module sides with True Track Hits: Outer(" << m_NoOuterModules.size() << "), Short Middle(" << m_NoShortMiddleModules.size()
                 << "), Middle(" << m_NoMiddleModules.size() << "), Inner(" << m_NoInnerModules.size() << "), Barrel(" << m_NoBarrelModules.size() << ") ");

  return StatusCode::SUCCESS;
}

double SCT_RDOsTestTool::GetBarrelOccupancy(const TH2* hist, const int layer, const double basescale, double& error)
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

double SCT_RDOsTestTool::GetEndcapOccupancy(const TH2* hist, const int disk, const double basescale, double& error)
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

StatusCode SCT_RDOsTestTool::finalize()
{
  double basescale(static_cast<double>(m_numberOfEventsSelected));
  basescale*=1536.0; // nstrips per _module_
  m_BarrelOccupancyByLayer->Sumw2();
  m_BarrelOccupancyByLayer->SetMarkerStyle(20);
  double error(0.0);
  for (int i(0); i<4 ; ++i)
    {
      m_BarrelOccupancyByLayer->SetBinContent(i+1,GetBarrelOccupancy(m_h_sct_barrel_occ_layer[i],i,basescale, error));
      m_BarrelOccupancyByLayer->SetBinError(i+1,error);
    }

  m_EndCapA_OccupancyByDisk->Sumw2();
  m_EndCapA_OccupancyByDisk->SetMarkerStyle(20);
  m_EndCapC_OccupancyByDisk->Sumw2();
  m_EndCapC_OccupancyByDisk->SetMarkerStyle(20);
  for (int i(0); i<9; ++i)
    {
      m_EndCapA_OccupancyByDisk->SetBinContent(i+1,GetEndcapOccupancy(m_h_sct_endcapA_occ_layer[i],i,basescale, error));
      m_EndCapA_OccupancyByDisk->SetBinError(i+1,error);
      m_EndCapC_OccupancyByDisk->SetBinContent(i+1,GetEndcapOccupancy(m_h_sct_endcapC_occ_layer[i],i,basescale, error));
      m_EndCapC_OccupancyByDisk->SetBinError(i+1,error);
    }

  //Module_type  <-> Integer
  //Barrel       <-> 4
  //Inner        <-> 3
  //Middles      <-> 2
  //Short Middles<-> 1
  //Outer        <-> 0
  //No. Outer Modules        = 2*9*52 in total
  //No. Short Middle Modules = 2*1*40 in total
  //No. Middle Modules       = 2*7*40 in total
  //No. Inner Modules        = 2*5*40 in total
  //No. Barrel Modules        =12*176 in total
  double No_Modules[5] = {936.0, 80.0, 560.0, 400.0, 2112.0};
  m_SCT_OccupancyByModuleType->Sumw2();
  m_SCT_OccupancyByModuleType->SetMarkerStyle(20);
  for(int i(0); i<5 ; ++i)
    {
      double Occupancy(m_SCT_OccupancyByModuleType->GetBinContent(i+1));
      double Error(std::pow(Occupancy,0.5));
      double scale(basescale*No_Modules[i]);
      // correct for the number of strips not used because module side had a track hit
      scale -= m_NumberModulesVetoed[i]*768.0; //as one side only

      Occupancy/=scale; Error/=scale;
      Occupancy*=100.0; Error*=100.0;
      m_SCT_OccupancyByModuleType->SetBinContent(i+1,Occupancy);
      m_SCT_OccupancyByModuleType->SetBinError(i+1,Error);
    }
  //       ATH_MSG_ERROR("No. Inner Modules Seen = " << m_NoInnerModules.size());
  //       ATH_MSG_ERROR("No. Middle Modules Seen = " << m_NoMiddleModules.size());
  //       ATH_MSG_ERROR("No. Short Middle Modules Seen = " << m_NoShortMiddleModules.size());
  //       ATH_MSG_ERROR("No. Outer Modules Seen = " << m_NoOuterModules.size());
  return StatusCode::SUCCESS;
}
