/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  !!!!!! Problem with calibration constants for mean ToT on the tracks (norm_ ...) !!!!!!!
//
#include "TRT_ToT_Tools/TRT_ToT_dEdx.h"
#include "TRT_ToT_Tools/TRT_ToT_Corrections.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/IChronoStatSvc.h"

#include "TF1.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"

// constructor
TRT_ToT_dEdx::TRT_ToT_dEdx(const std::string& t, const std::string& n, const IInterface* p)
  :
  AthAlgTool(t,n,p),
  m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",n),
  m_isData(true),
  m_isDataSet(false)
{
  declareInterface<ITRT_ToT_dEdx>(this);
  declareProperty("TRTStrawSummarySvc",    m_TRTStrawSummarySvc);

  SetDefaultConfiguration();

  //      m_gasTypeInStraw        = kUnset;
  m_L                                                     = 0.;
  m_timingProfile         = 0;
  m_trtId                                         = 0;
  m_trtman                                        = 0;
}



void TRT_ToT_dEdx::SetDefaultConfiguration()
{
  declareProperty("TRT_dEdx_divideByL",m_divideByL=true);
  declareProperty("TRT_dEdx_useHThits",m_useHThits=true);
  declareProperty("TRT_dEdx_corrected",m_corrected=true);
  declareProperty("TRT_dEdx_whichToTEstimatorAlgo",m_whichToTEstimatorAlgo=kToTLargerIsland);
  declareProperty("TRT_dEdx_useTrackPartWithGasType",m_useTrackPartWithGasType=kUnset);
  declareProperty("TRT_dEdx_toolScenario",m_toolScenario=kAlgReweightTrunkOne);
  declareProperty("TRT_dEdx_applyMimicToXeCorrection",m_applyMimicToXeCorrection=false);
  declareProperty("TRT_dEdx_trackConfig_maxRtrack",m_trackConfig_maxRtrack=1.85);
  declareProperty("TRT_dEdx_trackConfig_minRtrack",m_trackConfig_minRtrack=0.15);
  declareProperty("TRT_dEdx_useZeroRHitCut",m_useZeroRHitCut=true);
}



void TRT_ToT_dEdx::ShowDEDXSetup() const
{
  ATH_MSG_DEBUG("//////////////////////////////////////////////////////////////////");
  ATH_MSG_DEBUG("///              TRT_ToT_Tool setup configuration              ///");
  ATH_MSG_DEBUG(" ");
  ATH_MSG_DEBUG("m_divideByL                     ="<<m_divideByL<<"");
  ATH_MSG_DEBUG("m_useHThits                     ="<<m_useHThits<<"");
  ATH_MSG_DEBUG("m_corrected                     ="<<m_corrected<<"");
  ATH_MSG_DEBUG("m_whichToTEstimatorAlgo         ="<<m_whichToTEstimatorAlgo<<"");
  ATH_MSG_DEBUG("m_useTrackPartWithGasType       ="<<m_useTrackPartWithGasType<<"");
  ATH_MSG_DEBUG("m_toolScenario                  ="<<m_toolScenario<<"");
  ATH_MSG_DEBUG("m_applyMimicToXeCorrection      ="<<m_applyMimicToXeCorrection<<"");
  ATH_MSG_DEBUG(" ");
  ATH_MSG_DEBUG("m_trackConfig_minRtrack         ="<<m_trackConfig_minRtrack<<"");
  ATH_MSG_DEBUG("m_trackConfig_maxRtrack         ="<<m_trackConfig_maxRtrack<<"");
  ATH_MSG_DEBUG("m_useZeroRHitCut                ="<<m_useZeroRHitCut<<"");
  ATH_MSG_DEBUG(" ");
  ATH_MSG_DEBUG("//////////////////////////////////////////////////////////////////");
}



// destructor
TRT_ToT_dEdx::~TRT_ToT_dEdx() {}



// initialize
StatusCode TRT_ToT_dEdx::initialize() 
{

  MsgStream log(msgSvc(), name());
  
  // retrieve TRT-ID helper
  StoreGateSvc* detStore = 0;
  StatusCode sc = service( "DetectorStore", detStore );
  if (sc.isFailure()){
    ATH_MSG_ERROR ( "Could not get DetectorStore" );
    return sc;
  }
  
  sc = detStore->retrieve(m_trtId, "TRT_ID");
  if (sc.isFailure()){
    ATH_MSG_ERROR ( "Could not get TRT_ID helper !" );
    return StatusCode::FAILURE;
  }

  sc = detStore->retrieve(m_trtman, "TRT");
  if (sc.isFailure()){
    ATH_MSG_ERROR ( "Could not get TRT detector manager !" );
    return StatusCode::FAILURE;
  }

  m_timingProfile=0;
  sc = service("ChronoStatSvc", m_timingProfile);
  if ( sc.isFailure() || 0 == m_timingProfile) {
    ATH_MSG_DEBUG ("Can not find ChronoStatSvc name="<<m_timingProfile );
  }
 
  const DataHandle<CondAttrListVec> aptr;
  std::string folderName = {"/TRT/Calib/ToT/ToTVectors"};
  if (StatusCode::SUCCESS == detStore->regFcn(&TRT_ToT_dEdx::update,this,aptr,folderName)){
    ATH_MSG_DEBUG ("Registered callback for ToT");
  }else{
    ATH_MSG_ERROR ("Callback registration failed for /TRT/Calib/ToT/ToTVectors ");
  }

  const DataHandle<CondAttrListCollection> affectedRegionH;
  if (detStore->regFcn(&TRT_ToT_dEdx::update2,this,affectedRegionH,"/TRT/Calib/ToT/ToTValue").isSuccess()){
    ATH_MSG_DEBUG ( "Registered callback for  /TRT/Calib/ToT/ToTValue " );
  }else{
    ATH_MSG_WARNING ( "Cannot register callback for /TRT/Calib/ToT/ToTValue " );
  }

  // Initialize ReadHandleKey
  ATH_CHECK(m_eventInfoKey.initialize());

  //|-TRTStrawSummarySvc     = ServiceHandle('InDetTRTStrawStatusSummarySvc')
  sc = m_TRTStrawSummarySvc.retrieve();
  if (StatusCode::SUCCESS!= sc ){
    ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
    ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
    return sc;
  } else {
    if ( !m_TRTStrawSummarySvc.empty() ) 
      ATH_MSG_INFO ( "Retrieved tool " << m_TRTStrawSummarySvc );
  }

  if (m_useTrackPartWithGasType > EGasType::kUnset || 
      m_useTrackPartWithGasType < EGasType::kXenon) {
    ATH_MSG_ERROR("Property TRT_dEdx_useTrackPartWithGasType has an invalid "
                  << "value: " << m_useTrackPartWithGasType);
    return StatusCode::FAILURE;
  }
      
  //  ShowDEDXSetup();  

  ATH_MSG_INFO("//////////////////////////////////////////////////////////////////");
  ATH_MSG_INFO("///              TRT_ToT_Tool setup configuration              ///");
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("m_divideByL                     ="<<m_divideByL<<"");
  ATH_MSG_INFO("m_useHThits                     ="<<m_useHThits<<"");
  ATH_MSG_INFO("m_corrected                     ="<<m_corrected<<"");
  ATH_MSG_INFO("m_whichToTEstimatorAlgo         ="<<m_whichToTEstimatorAlgo<<"");
  ATH_MSG_INFO("m_useTrackPartWithGasType       ="<<m_useTrackPartWithGasType<<"");
  ATH_MSG_INFO("m_toolScenario                  ="<<m_toolScenario<<"");
  ATH_MSG_INFO("m_applyMimicToXeCorrection      ="<<m_applyMimicToXeCorrection<<"");
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("m_trackConfig_minRtrack         ="<<m_trackConfig_minRtrack<<"");
  ATH_MSG_INFO("m_trackConfig_maxRtrack         ="<<m_trackConfig_maxRtrack<<"");
  ATH_MSG_INFO("m_useZeroRHitCut                ="<<m_useZeroRHitCut<<"");
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("//////////////////////////////////////////////////////////////////");

  ATH_MSG_INFO ( name() << " initialize() successful" );    
  return StatusCode::SUCCESS;
}



StatusCode TRT_ToT_dEdx::finalize() 
{
  MsgStream log(msgSvc(), name());
  ATH_MSG_DEBUG ( "... in finalize() ..." );
  if(m_timingProfile)m_timingProfile->chronoPrint("TRT_ToT_dEdx"); //MJ
  return StatusCode::SUCCESS;
}



// callback for DB for arrays
StatusCode TRT_ToT_dEdx::update(int& /*i*/ , std::list<std::string>& /*l*/) 
{
  StoreGateSvc* detStore = 0;
  StatusCode sc = service( "DetectorStore", detStore );

  std::vector<std::string>  dict_names = {"para_end_corrRZLXe","para_end_corrRZ_Xe","para_end_mimicToXeXe","para_long_corrRZLXe","para_long_corrRZ_Xe","para_long_mimicToXeXe","para_short_corrRZLXe","para_short_corrRZ_Xe","para_short_mimicToXeXe","resolution_Xe","resolution_e_Xe","para_end_corrRZLAr","para_end_corrRZ_Ar","para_end_mimicToXeAr","para_long_corrRZLAr","para_long_corrRZ_Ar","para_long_mimicToXeAr","para_short_corrRZLAr","para_short_corrRZ_Ar","para_short_mimicToXeAr","resolution_Ar","resolution_e_Ar","para_end_corrRZLKr","para_end_corrRZ_Kr","para_end_mimicToXeKr","para_long_corrRZLKr","para_long_corrRZ_Kr","para_long_mimicToXeKr","para_short_corrRZLKr","para_short_corrRZ_Kr","para_short_mimicToXeKr","resolution_Kr","resolution_e_Kr"};
  std::map<std::string,std::vector<float> > result_dict;

  const DataHandle<CondAttrListVec> channel_values;

  if (StatusCode::SUCCESS == detStore->retrieve(channel_values, "/TRT/Calib/ToT/ToTVectors" ))
    {
      int dataBaseType = kNewDB;
      ATH_MSG_DEBUG("update():: dict_names[]="<<dict_names.size()<<", channel_values[]="<<channel_values->size()<<"");
      if(channel_values->size()<19695) 
        dataBaseType = kOldDB; 

      if(dataBaseType==kNewDB) 
        {
          CondAttrListVec::const_iterator first_channel = channel_values->begin();
          CondAttrListVec::const_iterator last_channel  = channel_values->end();

          unsigned int current_channel = 0;
          std::vector<float> current_array_values = {};

          for (; first_channel != last_channel; ++first_channel) {
            if (current_channel != first_channel->first){
              result_dict[dict_names[current_channel]] = current_array_values;
              current_channel = first_channel->first;      
              current_array_values.clear();
            }
            current_array_values.push_back(first_channel->second["array_value"].data<float>());             
          }
                        
          result_dict[dict_names[current_channel]] = current_array_values;
                        
          update_New(result_dict);
          ATH_MSG_DEBUG ("update():: Reading new database is done!");

          return StatusCode::SUCCESS;
                
        } else 
        if(dataBaseType==kOldDB) 
          {
            ATH_MSG_WARNING ("update():: Old COOL database tag!");

            std::vector<std::string>  dict_names_old = {"resolution","resolution_e","para_long_corrRZ_MC","para_short_corrRZ_MC","para_end_corrRZ_MC","para_long_corrRZL_MC","para_short_corrRZL_MC","para_end_corrRZL_MC"};
        
            CondAttrListVec::const_iterator first_channel = channel_values->begin();
            CondAttrListVec::const_iterator last_channel  = channel_values->end();

            unsigned int current_channel = 0;
            std::vector<float> current_array_values = {};

            for (; first_channel != last_channel; ++first_channel) 
              {
                if (current_channel != first_channel->first)
                  {
                    result_dict[dict_names_old[current_channel]] = current_array_values;
                    current_channel = first_channel->first;      
                    current_array_values.clear();
                  }
                current_array_values.push_back(first_channel->second["array_value"].data<float>());             
              }
                        
            result_dict[dict_names_old[current_channel]] = current_array_values;

            update_Old(result_dict);
            ATH_MSG_DEBUG ("update():: Reading old database is done!");

            return StatusCode::SUCCESS;
          }
        else {
          ATH_MSG_ERROR ("Problem reading condDB object. dataBaseType="<<dataBaseType<<"");
          return StatusCode::FAILURE;
        }
    }
  else {
    ATH_MSG_ERROR ("Problem reading condDB object. -");
    return StatusCode::FAILURE;
  }
}



void TRT_ToT_dEdx::update_New(std::map<std::string,std::vector<float> > &result_dict) 
{
  //      fill Xenon +++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection::resolution[0][ind]=result_dict["resolution_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection::resolution_e[0][ind]=result_dict["resolution_e_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ_MC[0][ind]=result_dict["para_long_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection::para_short_corrRZ_MC[0][ind]=result_dict["para_short_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection::para_long_corrRZL_MC[0][ind]=result_dict["para_long_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection::para_short_corrRZL_MC[0][ind]=result_dict["para_short_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection::para_end_corrRZL_MC[0][ind]=result_dict["para_end_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ[0][ind]=result_dict["para_long_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection::para_short_corrRZ[0][ind]=result_dict["para_short_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection::para_long_corrRZL_DATA[0][ind]=result_dict["para_long_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection::para_short_corrRZL_DATA[0][ind]=result_dict["para_short_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection::para_end_corrRZL_DATA[0][ind]=result_dict["para_end_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection::para_end_corrRZ[0][ind]=result_dict["para_end_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection::para_end_corrRZ_MC[0][ind]=result_dict["para_end_corrRZ_Xe"][ind];
  }

        
                
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection::para_end_mimicToXe_DATA[0][ind]=result_dict["para_end_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection::para_end_mimicToXe_MC[0][ind]=result_dict["para_end_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection::para_short_mimicToXe_DATA[0][ind]=result_dict["para_short_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection::para_short_mimicToXe_MC[0][ind]=result_dict["para_short_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection::para_long_mimicToXe_DATA[0][ind]=result_dict["para_long_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection::para_long_mimicToXe_MC[0][ind]=result_dict["para_long_mimicToXeXe"][ind];
  }

  //      fill Argon +++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection::resolution[1][ind]=result_dict["resolution_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection::resolution_e[1][ind]=result_dict["resolution_e_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ_MC[1][ind]=result_dict["para_long_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection::para_short_corrRZ_MC[1][ind]=result_dict["para_short_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection::para_long_corrRZL_MC[1][ind]=result_dict["para_long_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection::para_short_corrRZL_MC[1][ind]=result_dict["para_short_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection::para_end_corrRZL_MC[1][ind]=result_dict["para_end_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ[1][ind]=result_dict["para_long_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection::para_short_corrRZ[1][ind]=result_dict["para_short_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection::para_long_corrRZL_DATA[1][ind]=result_dict["para_long_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection::para_short_corrRZL_DATA[1][ind]=result_dict["para_short_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection::para_end_corrRZL_DATA[1][ind]=result_dict["para_end_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection::para_end_corrRZ[1][ind]=result_dict["para_end_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection::para_end_corrRZ_MC[1][ind]=result_dict["para_end_corrRZ_Ar"][ind];
  }

        
                
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection::para_end_mimicToXe_DATA[1][ind]=result_dict["para_end_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection::para_end_mimicToXe_MC[1][ind]=result_dict["para_end_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection::para_short_mimicToXe_DATA[1][ind]=result_dict["para_short_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection::para_short_mimicToXe_MC[1][ind]=result_dict["para_short_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection::para_long_mimicToXe_DATA[1][ind]=result_dict["para_long_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection::para_long_mimicToXe_MC[1][ind]=result_dict["para_long_mimicToXeAr"][ind];
  }

  //      fill Krypton +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection::resolution[2][ind]=result_dict["resolution_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection::resolution_e[2][ind]=result_dict["resolution_e_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ_MC[2][ind]=result_dict["para_long_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection::para_short_corrRZ_MC[2][ind]=result_dict["para_short_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection::para_long_corrRZL_MC[2][ind]=result_dict["para_long_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection::para_short_corrRZL_MC[2][ind]=result_dict["para_short_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection::para_end_corrRZL_MC[2][ind]=result_dict["para_end_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection::para_long_corrRZ[2][ind]=result_dict["para_long_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection::para_short_corrRZ[2][ind]=result_dict["para_short_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection::para_long_corrRZL_DATA[2][ind]=result_dict["para_long_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection::para_short_corrRZL_DATA[2][ind]=result_dict["para_short_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection::para_end_corrRZL_DATA[2][ind]=result_dict["para_end_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection::para_end_corrRZ[2][ind]=result_dict["para_end_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection::para_end_corrRZ_MC[2][ind]=result_dict["para_end_corrRZ_Kr"][ind];
  }

        
                
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection::para_end_mimicToXe_DATA[2][ind]=result_dict["para_end_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection::para_end_mimicToXe_MC[2][ind]=result_dict["para_end_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection::para_short_mimicToXe_DATA[2][ind]=result_dict["para_short_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection::para_short_mimicToXe_MC[2][ind]=result_dict["para_short_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection::para_long_mimicToXe_DATA[2][ind]=result_dict["para_long_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection::para_long_mimicToXe_MC[2][ind]=result_dict["para_long_mimicToXeKr"][ind];
  }
}



void TRT_ToT_dEdx::update_Old(std::map<std::string,std::vector<float> > &result_dict)
{
  for(int gasType = 0; gasType<3; gasType++) { // loop over gas types
    for (unsigned int ind=0; ind < 4; ++ind) {
      Dedxcorrection::resolution[gasType][ind]=result_dict["resolution"][ind];
    }

    for (unsigned int ind=0; ind < 4; ++ind) {
      Dedxcorrection::resolution_e[gasType][ind]=result_dict["resolution_e"][ind];
    }

    for (unsigned int ind=0; ind < 3240; ++ind) {
      Dedxcorrection::para_long_corrRZ_MC[gasType][ind]=result_dict["para_long_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 216; ++ind) {
      Dedxcorrection::para_short_corrRZ_MC[gasType][ind]=result_dict["para_short_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 630; ++ind) {
      Dedxcorrection::para_long_corrRZL_MC[gasType][ind]=result_dict["para_long_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 63; ++ind) {
      Dedxcorrection::para_short_corrRZL_MC[gasType][ind]=result_dict["para_short_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 252; ++ind) {
      Dedxcorrection::para_end_corrRZL_MC[gasType][ind]=result_dict["para_end_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 3240; ++ind) {
      Dedxcorrection::para_long_corrRZ[gasType][ind]=result_dict["para_long_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 216; ++ind) {
      Dedxcorrection::para_short_corrRZ[gasType][ind]=result_dict["para_short_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 630; ++ind) {
      Dedxcorrection::para_long_corrRZL_DATA[gasType][ind]=result_dict["para_long_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 63; ++ind) {
      Dedxcorrection::para_short_corrRZL_DATA[gasType][ind]=result_dict["para_short_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 252; ++ind) {
      Dedxcorrection::para_end_corrRZL_DATA[gasType][ind]=result_dict["para_end_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 336; ++ind) {
      Dedxcorrection::para_end_corrRZ[gasType][ind]=result_dict["para_end_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 336; ++ind) {
      Dedxcorrection::para_end_corrRZ_MC[gasType][ind]=result_dict["para_end_corrRZ_MC"][ind];
    }

    // Setting aditional corrections
    for (unsigned int ind=0; ind < 560; ++ind) {
      Dedxcorrection::para_end_mimicToXe_MC[gasType][ind]   = 1.;
      Dedxcorrection::para_end_mimicToXe_DATA[gasType][ind] = 1.;
    }

    for (unsigned int ind=0; ind < 180; ++ind) {
      Dedxcorrection::para_short_mimicToXe_MC[gasType][ind]   = 1.;
      Dedxcorrection::para_short_mimicToXe_DATA[gasType][ind] = 1.;
    }

    for (unsigned int ind=0; ind < 1800; ++ind) {
      Dedxcorrection::para_long_mimicToXe_MC[gasType][ind]   = 1.;
      Dedxcorrection::para_long_mimicToXe_DATA[gasType][ind] = 1.;
    }
  }
}



// callback for DB for scalar values
StatusCode TRT_ToT_dEdx::update2(int& /*i*/, std::list<std::string>& /*l*/ )
{
  const CondAttrListCollection* attrListColl = 0;
  StoreGateSvc* detStore = 0;
  StatusCode sc = service( "DetectorStore", detStore );

  if (StatusCode::SUCCESS == detStore->retrieve(attrListColl, "/TRT/Calib/ToT/ToTValue" ))
    {
      int dataBaseType = kNewDB;
      if(attrListColl->size() < 2) dataBaseType = kOldDB;

      CondAttrListCollection::const_iterator first = attrListColl->begin();
      CondAttrListCollection::const_iterator last  = attrListColl->end();

      if(dataBaseType==kNewDB) 
        {
          for (int index=0; first != last; ++first,++index) 
            {
              const coral::AttributeList& attrList = (*first).second;
              Dedxcorrection::paraL_dEdx_p1[index] = attrList["paraL_dEdx_p1"].data<float>();
              Dedxcorrection::paraL_dEdx_p2[index] = attrList["paraL_dEdx_p2"].data<float>();
              Dedxcorrection::paraL_dEdx_p3[index] = attrList["paraL_dEdx_p3"].data<float>();
              Dedxcorrection::paraL_dEdx_p4[index] = attrList["paraL_dEdx_p4"].data<float>();
              Dedxcorrection::paraL_dEdx_p5[index] = attrList["paraL_dEdx_p5"].data<float>();

              Dedxcorrection::para_dEdx_p1[index] = attrList["para_dEdx_p1"].data<float>();
              Dedxcorrection::para_dEdx_p2[index] = attrList["para_dEdx_p2"].data<float>();
              Dedxcorrection::para_dEdx_p3[index] = attrList["para_dEdx_p3"].data<float>();
              Dedxcorrection::para_dEdx_p4[index] = attrList["para_dEdx_p4"].data<float>();
              Dedxcorrection::para_dEdx_p5[index] = attrList["para_dEdx_p5"].data<float>();
                                  
              Dedxcorrection::norm_offset_data[index] = attrList["norm_offset_data"].data<float>();
              Dedxcorrection::norm_slope_tot[index] = attrList["norm_slope_tot"].data<float>();  
              Dedxcorrection::norm_slope_totl[index] = attrList["norm_slope_totl"].data<float>(); 
              Dedxcorrection::norm_offset_tot[index] = attrList["norm_offset_tot"].data<float>(); 
              Dedxcorrection::norm_offset_totl[index] = attrList["norm_offset_totl"].data<float>();           
              Dedxcorrection::norm_nzero[index]=attrList["norm_nzero"].data<int>();
            }
        } 
      else 
        {
          ATH_MSG_WARNING ("update2():: Old COOL database tag!");
          // return update2_Old();
          for (; first != last; ++first) {  
            const coral::AttributeList& attrList = (*first).second;
            for(int gasType=0; gasType<3; gasType++)
              {
                Dedxcorrection::paraL_dEdx_p1[gasType] = attrList["paraL_dEdx_p1"].data<float>();
                Dedxcorrection::paraL_dEdx_p2[gasType] = attrList["paraL_dEdx_p2"].data<float>();
                Dedxcorrection::paraL_dEdx_p3[gasType] = attrList["paraL_dEdx_p3"].data<float>();
                Dedxcorrection::paraL_dEdx_p4[gasType] = attrList["paraL_dEdx_p4"].data<float>();
                Dedxcorrection::paraL_dEdx_p5[gasType] = attrList["paraL_dEdx_p5"].data<float>();

                Dedxcorrection::para_dEdx_p1[gasType] = attrList["para_dEdx_p1"].data<float>();
                Dedxcorrection::para_dEdx_p2[gasType] = attrList["para_dEdx_p2"].data<float>();
                Dedxcorrection::para_dEdx_p3[gasType] = attrList["para_dEdx_p3"].data<float>();
                Dedxcorrection::para_dEdx_p4[gasType] = attrList["para_dEdx_p4"].data<float>();
                Dedxcorrection::para_dEdx_p5[gasType] = attrList["para_dEdx_p5"].data<float>();

                Dedxcorrection::norm_offset_data[gasType] = attrList["norm_offset_data"].data<float>();
                Dedxcorrection::norm_slope_tot[gasType] = attrList["norm_slope_tot"].data<float>();  
                Dedxcorrection::norm_slope_totl[gasType] = attrList["norm_slope_totl"].data<float>(); 
                Dedxcorrection::norm_offset_tot[gasType] = attrList["norm_offset_tot"].data<float>(); 
                Dedxcorrection::norm_offset_totl[gasType] = attrList["norm_offset_totl"].data<float>();         
                Dedxcorrection::norm_nzero[gasType]=attrList["norm_nzero"].data<int>(); 
              }
          }
        }
    } else {
    ATH_MSG_ERROR ("Problem reading condDB object. -");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}



bool TRT_ToT_dEdx::isGood_Hit(const Trk::TrackStateOnSurface *itr) const
{
  return isGood_Hit(itr, m_divideByL, m_useHThits);
}

bool TRT_ToT_dEdx::isGood_Hit(const Trk::TrackStateOnSurface *itr, bool divideByL, 
                              bool useHThits) const
{
  const Trk::MeasurementBase* trkM = itr->measurementOnTrack();
  if (!trkM)  return false;   
  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trkM);  
  if (!driftcircle) return false;

  const Trk::TrackParameters* trkP = itr->trackParameters();
  if(trkP==0)return false; 
  double Trt_Rtrack = fabs(trkP->parameters()[Trk::locR]);
  double Trt_RHit = fabs(driftcircle->localParameters()[Trk::driftRadius]);
  double Trt_HitTheta = trkP->parameters()[Trk::theta];
  double Trt_HitPhi = trkP->parameters()[Trk::phi];
  double error2 = 2*driftcircle->localCovariance()(Trk::driftRadius,Trk::driftRadius);
  double distance2 = (Trt_Rtrack - Trt_RHit)*(Trt_Rtrack - Trt_RHit);
  Identifier DCId = driftcircle->identify();
  int HitPart =  m_trtId->barrel_ec(DCId);
  const InDetDD::TRT_BaseElement* element = m_trtman->getElement(DCId);
  double strawphi = element->center(DCId).phi();

  if ( m_useZeroRHitCut && Trt_RHit==0) return false;                                     // tube hit
  if ( (Trt_Rtrack >= m_trackConfig_maxRtrack) || (Trt_Rtrack <= m_trackConfig_minRtrack) )return false;    // drift radius close to wire or wall
  if (distance2 > error2) return false; // Select precision hit only

  if (std::abs(HitPart)==1) { //Barrel
    m_L = 2*sqrt(4-Trt_Rtrack*Trt_Rtrack)*1./fabs(sin(Trt_HitTheta));
  }
  else
    if (std::abs(HitPart)==2) { //EndCap
      m_L = 2*sqrt(4-Trt_Rtrack*Trt_Rtrack)*1./sqrt(1-sin(Trt_HitTheta)*sin(Trt_HitTheta)*cos(Trt_HitPhi-strawphi)*cos(Trt_HitPhi-strawphi));
    }
    else {
      ATH_MSG_FATAL ("std::abs(HitPart)= " << std::abs(HitPart) << ". Must be 1(Barrel) or 2(Endcap)");
      throw std::exception();
    }

  if(divideByL)
    if ( m_L < 1.7 ) return false; // Length in the straw
  if(!useHThits){
    int TrtHl = driftcircle->highLevel();
    if (TrtHl==1) return false; 
  }

  if(m_useTrackPartWithGasType!=kUnset) // don't preselect hits
    {
      if(m_useTrackPartWithGasType!=gasTypeInStraw(itr))
        return false;
    }

  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToT(BitPattern);

  if(ToT==0) return false; // If ToT for this hit equal 0, skip it.
  
  return true;
}



double TRT_ToT_dEdx::dEdx(const Trk::Track* track) const
{
  return dEdx(track, m_divideByL, m_useHThits, m_corrected);
}


bool TRT_ToT_dEdx::isData() const {
  if (m_isDataSet) {
    return m_isData;
  } else {
   
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
    if (!eventInfo.isValid()){
      REPORT_MESSAGE(MSG::FATAL) << "Cannot retrieve EventInfo";
      return false;
    }

    // check if data or MC 
    m_isData = true;
    if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
      m_isData=false;
    }

    m_isDataSet = true;
    return m_isData;
  }
}



double TRT_ToT_dEdx::dEdx(const Trk::Track* track, bool divideByL, bool useHThits, bool corrected) const
{
  ATH_MSG_DEBUG("dEdx()");

  double nVtx=-1.;

  // Event information 
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  if (!eventInfo.isValid()){
    REPORT_MESSAGE(MSG::FATAL) << "Cannot retrieve EventInfo";
    return 0;
  }

  //    Average interactions per crossing for the current BCID
  double mu = -1.;
  mu = eventInfo->averageInteractionsPerCrossing();
  if(isData()) {
    nVtx = 1.3129 + 0.716194*mu + (-0.00475074)*mu*mu;
  }
  else
    nVtx = 1.0897 + 0.748287*mu + (-0.00421788)*mu*mu;


  if (!track) {
    return 0;
  }
  const DataVector<const Trk::TrackStateOnSurface>* vtsos = track->trackStateOnSurfaces(); 
  if (!vtsos) {
    return 0;
  }
  
  EGasType gasType;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itr  = vtsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itre = vtsos->end();  

  if(m_toolScenario==kAlgStandard || m_toolScenario==kAlgScalingToXe)
    {
      std::vector<double> vecToT;
      double ToTsum = 0;

      for ( ; itr!=itre ; ++itr) {
        if ( isGood_Hit((*itr),divideByL,useHThits)) {
          vecToT.push_back(correctToT_corrRZ(*itr,divideByL,corrected));
        }
      } 
          
      sort(vecToT.begin(), vecToT.end());
      int nhits = (int)vecToT.size();

      int ntrunk = 1;
      if(divideByL) nhits-=ntrunk;

      // Boost speed
      if(nhits<1)return 0.0;

      for (int i = 0; i < nhits;i++){
        ToTsum+=vecToT.at(i);
      } 
      ToTsum*=correctNormalization(divideByL, m_isData, nVtx);

      return ToTsum/nhits;
    }
  else
    if(m_toolScenario==kAlgReweight || m_toolScenario==kAlgReweightTrunkOne)
      {
        std::vector<double> vecToT_Xe;
        std::vector<double> vecToT_Ar;
        std::vector<double> vecToT_Kr;

        int gasTypeForCorrectNormalization = m_useTrackPartWithGasType;
        if(m_useTrackPartWithGasType!=kUnset)
          {
            ATH_MSG_WARNING("dEdX_Estimator():: Using m_toolScenario="<<m_toolScenario<<" scenario m_useTrackPartWithGasType is set to"<<m_useTrackPartWithGasType<<", but kUnset is requiered. Setting that to kUnset. Check you tool configuration.");
            m_useTrackPartWithGasType=kUnset;
          }

        for ( ; itr!=itre ; ++itr) {
          if ( isGood_Hit((*itr),divideByL,useHThits)) {
            gasType=gasTypeInStraw(*itr);
            if(gasType==kXenon)
              vecToT_Xe.push_back(correctToT_corrRZ(*itr, divideByL, corrected));
            else
              if(gasType==kArgon)
                vecToT_Ar.push_back(correctToT_corrRZ(*itr, divideByL, corrected));
              else
                if(gasType==kKrypton)
                  vecToT_Kr.push_back(correctToT_corrRZ(*itr, divideByL, corrected));
                else
                  ATH_MSG_ERROR("dEdX_Estimator():: During scenario kAlgReweight variable gasTypeInStraw got value kUnset.");
          }
        } 

        sort(vecToT_Xe.begin(), vecToT_Xe.end());
        sort(vecToT_Ar.begin(), vecToT_Ar.end());
        sort(vecToT_Kr.begin(), vecToT_Kr.end());

        int nhitsXe = (int)vecToT_Xe.size();
        int nhitsAr = (int)vecToT_Ar.size();
        int nhitsKr = (int)vecToT_Kr.size();


        int ntrunk = 1;
        if(divideByL)
          {
            if(m_toolScenario==kAlgReweight){
              if(nhitsXe>0) nhitsXe-=ntrunk;
              if(nhitsAr>0) nhitsAr-=ntrunk;
              if(nhitsKr>0) nhitsKr-=ntrunk;
            }
            else // kAlgReweightTrunkOne
              {
                int trunkGas = kUnset;
                double maxToT = 0.;
                if(nhitsXe>0 && vecToT_Xe.at(nhitsXe-1)>maxToT) trunkGas = kXenon;
                if(nhitsAr>0 && vecToT_Ar.at(nhitsAr-1)>maxToT) trunkGas = kArgon;
                if(nhitsKr>0 && vecToT_Kr.at(nhitsKr-1)>maxToT) trunkGas = kKrypton;

                if(trunkGas==kXenon)   nhitsXe-=ntrunk;
                else
                  if(trunkGas==kArgon)   nhitsAr-=ntrunk;
                  else
                    if(trunkGas==kKrypton) nhitsKr-=ntrunk;
              }
          }

        m_useTrackPartWithGasType = gasTypeForCorrectNormalization;

        // Boost speed.
        int nhits  = nhitsXe + nhitsAr + nhitsKr;
        if(nhits<1) return 0.0;

        double ToTsumXe = 0;
        double ToTsumAr = 0;
        double ToTsumKr = 0;
        for (int i = 0; i < nhitsXe;i++){
          ToTsumXe+=vecToT_Xe.at(i);
        } 
        for (int i = 0; i < nhitsAr;i++){
          ToTsumAr+=vecToT_Ar.at(i);
        } 
        for (int i = 0; i < nhitsKr;i++){
          ToTsumKr+=vecToT_Kr.at(i);
        } 

        if(nhitsXe>0) ToTsumXe/=nhitsXe;
        else ToTsumXe = 0;
        if(nhitsAr>0) ToTsumAr/=nhitsAr;
        else ToTsumAr = 0;
        if(nhitsKr>0) ToTsumKr/=nhitsKr;
        else ToTsumKr = 0;


        double ToTsum = ToTsumXe*nhitsXe + ToTsumAr*nhitsAr + ToTsumKr*nhitsKr;

        ToTsum*=correctNormalization(divideByL, m_isData, nVtx);

        return ToTsum/nhits;
      }
  

  ATH_MSG_ERROR("dEdX_Estimator():: m_toolScenario has wrong value "<<m_toolScenario<<"");
  return 0.;
}

double TRT_ToT_dEdx::usedHits(const Trk::Track* track) const
{
  return usedHits(track, m_divideByL, m_useHThits);
}

double TRT_ToT_dEdx::usedHits(const Trk::Track* track, bool divideByL, bool useHThits) const
{
  ATH_MSG_DEBUG("usedHits()");
  EGasType gasType = kUnset;

  if (!track) {
    return 0;
  }
  const DataVector<const Trk::TrackStateOnSurface>* vtsos = track->trackStateOnSurfaces(); 
  if (!vtsos) {
    return 0;
  }

  DataVector<const Trk::TrackStateOnSurface>::const_iterator itr  = vtsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itre = vtsos->end();  
  

  if(m_toolScenario==kAlgStandard || m_toolScenario==kAlgScalingToXe)
    {
      int nhits =0;

      for ( ; itr!=itre ; ++itr) {
        if ( isGood_Hit((*itr),divideByL,useHThits)) {
          nhits++;
        }
      } 

      int ntrunk = 1;
      if(divideByL) nhits-=ntrunk;

      return nhits;
    }
  else
    if(m_toolScenario==kAlgReweight || m_toolScenario==kAlgReweightTrunkOne)
      {
        int nhits = 0;
        int nhitsXe = 0;
        int nhitsAr = 0;
        int nhitsKr = 0;

        int gasTypeForCorrectNormalization = m_useTrackPartWithGasType;
        if(m_useTrackPartWithGasType!=kUnset)
          {
            ATH_MSG_WARNING("usedHits_Estimator():: Using m_toolScenario="<<m_toolScenario<<" scenario m_useTrackPartWithGasType is set to "<<m_useTrackPartWithGasType<<", but kUnset is required. Setting that to kUnset. Check you tool configuration.");
            m_useTrackPartWithGasType=kUnset;
          }

        for ( ; itr!=itre ; ++itr) {
          if ( isGood_Hit((*itr),divideByL,useHThits)) {
            gasType=gasTypeInStraw(*itr);
            if(gasType==kXenon)
              nhitsXe++;
            else
              if(gasType==kArgon)
                nhitsAr++;
              else
                if(gasType==kKrypton)
                  nhitsKr++;
                else
                  ATH_MSG_ERROR("usedHits_Estimator():: During scenario kAlgReweight variable gasTypeInStraw got value kUnset.");
          }
        } 

        m_useTrackPartWithGasType = gasTypeForCorrectNormalization;

        int ntrunk = 1;
        if(divideByL) {
          if(m_toolScenario==kAlgReweight){
            if(nhitsXe>0) nhitsXe-=ntrunk;
            if(nhitsAr>0) nhitsAr-=ntrunk;
            if(nhitsKr>0) nhitsKr-=ntrunk;
          }
          else { // kAlgReweightTrunkOne
            if(nhitsXe>0 || nhitsAr>0 || nhitsKr>0)
              nhitsXe -= ntrunk;
          }
        }

        nhits  = nhitsXe + nhitsAr + nhitsKr;

        return nhits;
      }

  ATH_MSG_ERROR("usedHits_Estimator():: m_toolScenario has wrong value "<<m_toolScenario<<"");
  return 0;
}



double TRT_ToT_dEdx::getProb(const Trk::TrackStateOnSurface *itr, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits) const
{
  return getProb(itr, dEdx_obs, pTrk, hypothesis, nUsedHits, m_divideByL);
}



double TRT_ToT_dEdx::getProb(const Trk::TrackStateOnSurface *itr, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool divideByL) const
{
  EGasType gasType = gasTypeInStraw(itr);
  return getProb(gasType, dEdx_obs, pTrk, hypothesis, nUsedHits, divideByL);
}
        

double TRT_ToT_dEdx::getProb(EGasType gasType, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool divideByL) const
{
        
  ATH_MSG_DEBUG("getProb():: gasTypeInStraw = "<<gasType<<"");

  if(gasType==kUnset)
    {
      ATH_MSG_DEBUG("getProb():: gasTypeInStraw set kUnset that is not allowed! Use gasTypeInStraw(*itr) to get gas type info for that hit first!");
      ATH_MSG_DEBUG("getProb():: Now gasTypeInStraw sets to kXenon.");
      gasType = kXenon;
    }

  double dEdx_pred = predictdEdx(gasType, pTrk, hypothesis, divideByL); 
  if(dEdx_pred==0)return 0.0;
  if(hypothesis==Trk::electron){
    // correction for pTrk in [MeV]
    double factor = 1;
    double correct = 1+factor*(0.045*log10(pTrk)+0.885-1);
    dEdx_pred= dEdx_pred/correct;
  }

  double Resolution = Dedxcorrection::resolution[gasType][0]+Dedxcorrection::resolution[gasType][1]*(nUsedHits+0.5)+Dedxcorrection::resolution[gasType][2]*(nUsedHits+0.5)*(nUsedHits+0.5)+Dedxcorrection::resolution[gasType][3]*(nUsedHits+0.5)*(nUsedHits+0.5)*(nUsedHits+0.5);
  if(hypothesis==Trk::electron){
    Resolution = Dedxcorrection::resolution_e[gasType][0]+Dedxcorrection::resolution_e[gasType][1]*(nUsedHits+0.5)+Dedxcorrection::resolution_e[gasType][2]*(nUsedHits+0.5)*(nUsedHits+0.5)+Dedxcorrection::resolution_e[gasType][3]*(nUsedHits+0.5)*(nUsedHits+0.5)*(nUsedHits+0.5);
  }

  double prob =exp( -0.5 * ( ( ( dEdx_obs - dEdx_pred ) / (Resolution*dEdx_pred) ) * 
                             ( ( dEdx_obs - dEdx_pred ) / (Resolution*dEdx_pred) ) ))  ; 

  ATH_MSG_DEBUG("getProb():: return "<<prob<<"");
  return prob;
}


double TRT_ToT_dEdx::getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits) const {

  return getTest(dEdx_obs, pTrk, hypothesis, antihypothesis, nUsedHits, m_divideByL);

}

double TRT_ToT_dEdx::getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits, bool divideByL) const
{
  ATH_MSG_DEBUG("getTest()");

  EGasType gasType = kUnset;
        
  if ( dEdx_obs<=0. || pTrk<=0. || nUsedHits<=0 ) return 0.5;
  
  double Pone = getProb(gasType, dEdx_obs,pTrk,hypothesis,nUsedHits, divideByL);
  double Ptwo = getProb(gasType, dEdx_obs,pTrk,antihypothesis,nUsedHits, divideByL);
  if( (Pone+Ptwo) != 0){
    ATH_MSG_DEBUG("getTest():: return "<<Pone/(Pone+Ptwo)<<"");
    return Pone/(Pone+Ptwo);
  }else
    return 0.5;
}



double TRT_ToT_dEdx::predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis) const
{
  return predictdEdx(itr, pTrk, hypothesis, m_divideByL);
}



double TRT_ToT_dEdx::predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis, bool divideByL) const
{
  EGasType gasType = gasTypeInStraw(itr);
  return predictdEdx(gasType, pTrk, hypothesis, divideByL);
}

double TRT_ToT_dEdx::predictdEdx(EGasType gasType, const double pTrk, Trk::ParticleHypothesis hypothesis, bool divideByL) const
{

  ATH_MSG_DEBUG("predictdEdx(): gasTypeInStraw = "<<gasType<<"");

  if(gasType==kUnset)
    {
      ATH_MSG_DEBUG("predictdEdx():: gasTypeInStraw set kUnset that is not allowed! Use gasTypeInStraw(*itr) to get gas type info for that hit first!");
      ATH_MSG_DEBUG("predictdEdx():: Now gasTypeInStraw sets to kXenon.");
      gasType = kXenon;
    }

  double mass = m_particlemasses.mass[hypothesis];

  double betaGamma = pTrk/mass;
  /** @todo why is it possible that a 20 MeV particle reaches this point? (see Savannah bug 94644) */
  // low momentum particle can create floating point error 
  // do we need the check in the log parameter in addition? will create CPU increase
  // do we want to throw an assertion here?
  if(pTrk<100)return 0; 
  if(divideByL){    
    if(Dedxcorrection::paraL_dEdx_p3[gasType]+1./( std::pow( betaGamma, Dedxcorrection::paraL_dEdx_p5[gasType]))<=0) return 0;
    return Dedxcorrection::paraL_dEdx_p1[gasType]/std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::paraL_dEdx_p4[gasType])  * 
      (Dedxcorrection::paraL_dEdx_p2[gasType] - std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::paraL_dEdx_p4[gasType] ) 
       - log(Dedxcorrection::paraL_dEdx_p3[gasType]+1./( std::pow( betaGamma, Dedxcorrection::paraL_dEdx_p5[gasType]) ) ) );
  }else {
    if(Dedxcorrection::para_dEdx_p3[gasType]+1./( std::pow( betaGamma, Dedxcorrection::para_dEdx_p5[gasType]) )<=0)return 0; 
    return Dedxcorrection::para_dEdx_p1[gasType]/std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::para_dEdx_p4[gasType])  * 
      (Dedxcorrection::para_dEdx_p2[gasType] - std::pow( sqrt( (betaGamma*betaGamma)/(1.+(betaGamma*betaGamma)) ), Dedxcorrection::para_dEdx_p4[gasType] ) 
       - log(Dedxcorrection::para_dEdx_p3[gasType]+1./( std::pow( betaGamma, Dedxcorrection::para_dEdx_p5[gasType]) ) ) );
  }
  //return 0;  
}



double TRT_ToT_dEdx::mass(const Trk::TrackStateOnSurface *itr, const double pTrk, double dEdx ) const
{
  EGasType gasType = gasTypeInStraw(itr);

  ATH_MSG_DEBUG("mass(): gasTypeInStraw = "<<gasType<<"");

  if(gasType==kUnset)
    {
      ATH_MSG_WARNING("mass():: gasTypeInStraw set kUnset that is not allowed! Use gasTypeInStraw(*itr) to get gas type info for that hit first!");
      ATH_MSG_WARNING("mass():: Now gasTypeInStraw sets to kXenon.");
      gasType = kXenon;
    }
  // only for testing purposes!!!!
  // note that dE/dx has to be corrected on track level first before value can be transferred to mass
  // this has to be tuned on user level
  /** @todo make failsafe */
  static const double bg_min = 0.001;
  static const double bg_max = 3;   // maximal allowed bg
  
  static const std::string blumRolandiFunction = "( [0]/sqrt( (x*x/([5]*[5]))/(1.+(x*x/([5]*[5]))) )^[3] ) * ([1] - sqrt( (x*x/([5]*[5]))/(1.+(x*x/([5]*[5]))) )^[3] - log([2]+1./((x/[5])^[4]) ) )";
  
  TF1 blumRolandi( "BR", blumRolandiFunction.c_str(), 0.7, 100000);

  blumRolandi.SetParameters(Dedxcorrection::para_dEdx_p1[gasType],Dedxcorrection::para_dEdx_p2[gasType],Dedxcorrection::para_dEdx_p3[gasType],Dedxcorrection::para_dEdx_p4[gasType],Dedxcorrection::para_dEdx_p5[gasType], 1. ); 
  //blumRolandi.SetParameters(&Dedxcorrection::para_dEdx_BB);
  double betaGamma = blumRolandi.GetX(dEdx, bg_min, bg_max); 
  
  ATH_MSG_DEBUG("mass():: return "<<pTrk/betaGamma<<"");
        
  return pTrk/betaGamma;
}



/* returns gas type for given straw */
ITRT_ToT_dEdx::EGasType TRT_ToT_dEdx::gasTypeInStraw(const Trk::TrackStateOnSurface *itr) const
{
  const Trk::MeasurementBase* trkM = itr->measurementOnTrack();
  if (!trkM)  return kUnset;   
  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trkM);  
  if (!driftcircle) return kUnset;

  return gasTypeInStraw(driftcircle);
}



ITRT_ToT_dEdx::EGasType TRT_ToT_dEdx::gasTypeInStraw(const InDet::TRT_DriftCircleOnTrack *driftcircle) const
{
  Identifier DCid = driftcircle->identify();  
  
  // getStatusHT returns enum {Undefined, Dead, Good, Xenon, Argon, Krypton, EmulatedArgon, EmulatedKrypton}.
  // Our representation of 'GasType' is 0:Xenon, 1:Argon, 2:Krypton
  EGasType GasType=kUnset; // kUnset is default
  if (!m_TRTStrawSummarySvc.empty()) {
    int stat = m_TRTStrawSummarySvc->getStatusHT(DCid);
    if       ( stat==2 || stat==3 ) { GasType = kXenon; } // Xe
    else if  ( stat==1 || stat==4 ) { GasType = kArgon; } // Ar
    else if  ( stat==5 )            { GasType = kKrypton; } // Kr
    else if  ( stat==6 )            { GasType = kArgon; } // Emulated Ar
    else if  ( stat==7 )            { GasType = kKrypton; } // Emulated Kr
    else { ATH_MSG_FATAL ("getStatusHT = " << stat << ", must be 'Good(2)||Xenon(3)' or 'Dead(1)||Argon(4)' or 'Krypton(5)' or 'EmulatedArgon(6)' or 'EmulatedKr(7)'!");
      throw std::exception();
    }
  }

  return GasType;
}



double TRT_ToT_dEdx::getToT(unsigned int BitPattern) const
{
  if(m_whichToTEstimatorAlgo==kToTLargerIsland) 
    return getToTlargerIsland(BitPattern);
  else
    if(m_whichToTEstimatorAlgo==kToTHighOccupancy) 
      return getToTHighOccupancy(BitPattern);
    else
      if(m_whichToTEstimatorAlgo==kToTHighOccupancySmart)
        return getToTHighOccupancySmart(BitPattern);

  ATH_MSG_FATAL("getToT():: No ToT estimator case for m_whichToTEstimatorAlgo"<<m_whichToTEstimatorAlgo<<"");
  throw std::exception();
}


/////////////////////////////////
// Corrections
/////////////////////////////////

double TRT_ToT_dEdx::correctNormalization(bool divideLength,bool scaledata, double nVtx) const
{
  EGasType gasType = static_cast<EGasType> (m_useTrackPartWithGasType);
  if(m_useTrackPartWithGasType==kUnset)
    gasType=kXenon;
  if(nVtx<=0)nVtx=Dedxcorrection::norm_nzero[gasType];
  double slope = Dedxcorrection::norm_slope_tot[gasType];
  double offset = Dedxcorrection::norm_offset_tot[gasType];
  if(divideLength){
    slope = Dedxcorrection::norm_slope_tot[gasType];
    offset = Dedxcorrection::norm_offset_tot[gasType];
  } 
  double shift = Dedxcorrection::norm_offset_data[gasType];
  if(!scaledata)shift = 0;
  return (slope*Dedxcorrection::norm_nzero[gasType]+offset)/(slope*nVtx+offset+shift);
}



double TRT_ToT_dEdx::correctToT_corrRZ(const Trk::TrackStateOnSurface *itr) const
{

  return correctToT_corrRZ(itr, m_divideByL, m_corrected);

}

double TRT_ToT_dEdx::correctToT_corrRZ(const Trk::TrackStateOnSurface *itr, bool divideByL, bool corrected) const
{
  const Trk::MeasurementBase* trkM = itr->measurementOnTrack();
  const Trk::TrackParameters* trkP = itr->trackParameters();
  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trkM);

  if (!driftcircle) return 0;
  if (driftcircle->prepRawData()==0) return 0;

  Identifier DCId = driftcircle->identify();
  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToT(BitPattern);
  if(ToT==0) {
    ATH_MSG_WARNING("correctToT_corrRZ(const Trk::TrackStateOnSurface *itr):: ToT="<<ToT<<". We must cut that hit in isGood_Hit() !");
    return 0;
  }
  int HitPart =  m_trtId->barrel_ec(DCId);
  int StrawLayer = m_trtId->straw_layer(DCId);
  int Layer = m_trtId->layer_or_wheel(DCId);
  double HitRtrack = fabs(trkP->parameters()[Trk::locR]);
  EGasType gasType = gasTypeInStraw(itr);  
  if(gasType==kUnset) {
    ATH_MSG_ERROR("correctToT_corrRZ(const Trk::TrackStateOnSurface *itr):: Gas type in straw is kUnset! Return ToT = 0");
    return 0;
  }  
  
  if(m_applyMimicToXeCorrection==true || m_toolScenario==kAlgScalingToXe)
    {
      if(gasType!=kXenon) // mimic to Xenon ToT, so we skip Xenon hits
        {     
          double mimicCorr = 0.;
          if (abs(HitPart)==1) // Barrel
            mimicCorr=mimicToXeHit_Barrel(gasType, HitRtrack, Layer, StrawLayer);
          else // End-cap
            mimicCorr=mimicToXeHit_Endcap(gasType, HitRtrack, Layer, HitPart); 

          if(mimicCorr==0.)
            {
              ATH_MSG_FATAL("correctToT_corrRZ():: mimicCorr = "<<mimicCorr<<"");
              throw std::exception();
            }
          ToT/=mimicCorr;

          if(m_whichToTEstimatorAlgo==kToTLargerIsland){
            if(ToT<3.125) ToT = 3.125;
            if(ToT>75)    ToT = 75;
          }else{
            if(ToT<3.125) ToT = 3.125;
            if(ToT>62.5)  ToT = 62.5;
          }

          gasType=kXenon; // After mimic correction we work with that hit as Xenon hit.
        }  
    }
 
  if(divideByL) ToT = ToT/m_L;
  if(!corrected) return ToT;
  /* else correct */
           
  double HitZ = driftcircle->globalPosition().z();
  double trackx =  driftcircle->globalPosition().x();
  double tracky =  driftcircle->globalPosition().y();
  double HitRpos = sqrt(trackx*trackx+tracky*tracky);
  
  /** @todo implement possiblity to set the scaling factor run-by-run from database, 
      should probably be done later on track- level */
  double ToTmip = 1;
  double valToT = 0;
  if(divideByL){
    if (abs(HitPart)==1) // Barrel
      valToT = fitFuncBarrel_corrRZL(gasType, HitRtrack,HitZ, Layer, StrawLayer);
    else // End-cap
      valToT = fitFuncEndcap_corrRZL(gasType, HitRtrack,HitRpos,Layer, HitZ>0?1:(HitZ<0?-1:0));   
  }else{
    if (abs(HitPart)==1) // Barrel
      valToT = fitFuncBarrel_corrRZ(gasType, HitRtrack,HitZ, Layer, StrawLayer);
    else // End-cap
      valToT = fitFuncEndcap_corrRZ(gasType, HitRtrack,HitRpos,Layer, HitZ>0?1:(HitZ<0?-1:0));
  } 

  return ToTmip*ToT/valToT;
}



double TRT_ToT_dEdx::correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool isdata) const {

  return correctToT_corrRZL(trkP, driftcircle, HitPart, Layer, StrawLayer, isdata,
                            m_useHThits);
}

double TRT_ToT_dEdx::correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool isdata, bool useHThits) const
{
  if (isdata != isData()) {
    ATH_MSG_ERROR("TRT_ToT_dEdx::correctToT_corrRZL called with isData = " << isdata
                  << " but data type is " << m_isData << ". Ignoring!");
  }

        
  if(trkP==0)return false; 
  if (!driftcircle) return false;
  if (driftcircle->prepRawData()==0) return 0;
  double HitRtrack = fabs(trkP->parameters()[Trk::locR]);
  double Trt_RHit = fabs(driftcircle->localParameters()[Trk::driftRadius]);
  if ( m_useZeroRHitCut && Trt_RHit==0) return false;                                     // tube hit
  if ( (HitRtrack >= m_trackConfig_maxRtrack) || (HitRtrack <= m_trackConfig_minRtrack) )return false;    // drift radius close to wire or wall
  if(!useHThits){
    int TrtHl = driftcircle->highLevel();
    if (TrtHl==1) return false; 
  }

  EGasType gasType = gasTypeInStraw(driftcircle);  
  if(m_useTrackPartWithGasType!=kUnset) // don't preselect hits
    {
      if(m_useTrackPartWithGasType!=gasType)
        return false;
    }

  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToT(BitPattern);
  if(ToT==0) return false; // If ToT for this hit equal 0, skip it.

  if(m_applyMimicToXeCorrection==true || m_toolScenario==kAlgScalingToXe)
    {
      if(gasType!=kXenon) // mimic to Xenon ToT, so we skip Xenon hits
        {     
          if (abs(HitPart)==1) // Barrel
            ToT/=mimicToXeHit_Barrel(gasType, HitRtrack, Layer, StrawLayer);
          else // End-cap
            ToT/=mimicToXeHit_Endcap(gasType, HitRtrack, Layer, HitPart); 

          if(m_whichToTEstimatorAlgo==kToTLargerIsland){
            if(ToT<3.125) ToT = 3.125;
            if(ToT>75)    ToT = 75;
          }else{
            if(ToT<3.125) ToT = 3.125;
            if(ToT>62.5)  ToT = 62.5;
          }

          gasType=kXenon; // After mimic correction we work with that hit as Xenon hit.
        }  
    }

  ToT = ToT/m_L;

  const Amg::Vector3D& gp = driftcircle->globalPosition();
  double HitR = sqrt( gp.x() * gp.x() + gp.y() * gp.y() );
  double HitZ = gp.z();
  double ToTmip = 1;
  double valToT = 1;

  if (abs(HitPart)==1) // Barrel
    valToT = fitFuncBarrel_corrRZL(gasType, HitRtrack,HitZ, Layer, StrawLayer);
  else // End-cap
    valToT = fitFuncEndcap_corrRZL(gasType, HitRtrack ,HitR,Layer, HitZ>0?1:(HitZ<0?-1:0));
  return ToTmip*ToT/valToT;
}



double TRT_ToT_dEdx::correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool isdata) const
{
  return correctToT_corrRZ(trkP, driftcircle, HitPart, Layer, StrawLayer,  isdata, 
                           m_useHThits);
}

double TRT_ToT_dEdx::correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool isdata, bool useHThits) const
{

  if (isdata != isData()) {
    ATH_MSG_ERROR("TRT_ToT_dEdx::correctToT_corrRZ called with isData = " << isdata
                  << " but data type is " << m_isData << ". Ignoring!");
  }

  if(trkP==0)return false; 
  if (!driftcircle) return false;
  if (driftcircle->prepRawData()==0) return 0;
  double HitRtrack = fabs(trkP->parameters()[Trk::locR]);
  double Trt_RHit = fabs(driftcircle->localParameters()[Trk::driftRadius]);
  if ( m_useZeroRHitCut && Trt_RHit==0) return false;                                     // tube hit
  if ( (HitRtrack >= m_trackConfig_maxRtrack) || (HitRtrack <= m_trackConfig_minRtrack) )return false;    // drift radius close to wire or wall
  if(!useHThits){
    int TrtHl = driftcircle->highLevel();
    if (TrtHl==1) return false; 
  }            

  EGasType gasType = gasTypeInStraw(driftcircle);  
  if(m_useTrackPartWithGasType!=kUnset) // don't preselect hits
    {
      if(m_useTrackPartWithGasType!=gasType)
        return false;
    }

  unsigned int BitPattern = driftcircle->prepRawData()->getWord();
  double ToT = getToT(BitPattern);
  if(ToT==0) return false; // If ToT for this hit equal 0, skip it.
  
  if(m_applyMimicToXeCorrection==true || m_toolScenario==kAlgScalingToXe)
    {
      if(gasType!=kXenon) // mimic to Xenon ToT, so we skip Xenon hits
        {     
          if (abs(HitPart)==1) // Barrel
            ToT/=mimicToXeHit_Barrel(gasType, HitRtrack, Layer, StrawLayer);
          else // End-cap
            ToT/=mimicToXeHit_Endcap(gasType, HitRtrack, Layer, HitPart); 

          if(m_whichToTEstimatorAlgo==kToTLargerIsland){
            if(ToT<3.125) ToT = 3.125;
            if(ToT>75)    ToT = 75;
          }else{
            if(ToT<3.125) ToT = 3.125;
            if(ToT>62.5)  ToT = 62.5;
          }

          gasType=kXenon; // After mimic correction we work with that hit as Xenon hit.
        }  
    }

  const Amg::Vector3D& gp = driftcircle->globalPosition();
  double HitR = sqrt( gp.x() * gp.x() + gp.y() * gp.y() );
  double HitZ = gp.z();
  double ToTmip = 1;
  double valToT = 1;

  if (abs(HitPart)==1) // Barrel
    valToT = fitFuncBarrel_corrRZ(gasType, HitRtrack,HitZ, Layer, StrawLayer);
  else // End-cap
    valToT = fitFuncEndcap_corrRZ(gasType, HitRtrack ,HitR,Layer, HitZ>0?1:(HitZ<0?-1:0));
  return ToTmip*ToT/valToT;
}



double TRT_ToT_dEdx::fitFuncBarrel_corrRZ(EGasType gasType, double driftRadius,double zPosition, int Layer, int StrawLayer) const
{
  if(Layer==0 && StrawLayer<9)
    return fitFuncBarrelShort_corrRZ(gasType, driftRadius,zPosition, StrawLayer);
  else
    return fitFuncBarrelLong_corrRZ(gasType, driftRadius,zPosition,Layer, StrawLayer);
  //return 0;
}



double TRT_ToT_dEdx::fitFuncEndcap_corrRZ(EGasType gasType, double driftRadius,double radialPosition, int Layer, int sign) const 
{
  /**
   * T(r,R) = T0(r)+ a(r)*R
   */
  double T0 =  fitFuncPol_corrRZ(gasType, 0,driftRadius,Layer,0,sign,2);
  double a  =  fitFuncPol_corrRZ(gasType, 1,driftRadius,Layer,0,sign,2);
  return T0+a*radialPosition;
}



double TRT_ToT_dEdx::fitFuncBarrelLong_corrRZ(EGasType gasType, double driftRadius,double zPosition, int Layer, int StrawLayer) const
{
  /**
   *                   |z|       /|z| - l  \
   * T(r,z) = T0(r) +   ---  exp|----------|
   *                    v(r)     \  s(r)   /
   */ 
  double z = fabs(zPosition);
  int sign=1;
  if(zPosition<0)sign=-1;
  double l = 704.6;
  // define set of parameters for negative and positive z positions
  double T0 =  fitFuncPol_corrRZ(gasType, 0,driftRadius,Layer,StrawLayer,sign,0);
  double  v =  fitFuncPol_corrRZ(gasType, 1,driftRadius,Layer,StrawLayer,sign,0);
  double  s =  fitFuncPol_corrRZ(gasType, 2,driftRadius,Layer,StrawLayer,sign,0);
  return T0+z/v*exp((z-l)/s);
}



double TRT_ToT_dEdx::fitFuncBarrelShort_corrRZ(EGasType gasType, double driftRadius,double zPosition, int StrawLayer) const
{
  /**
   *  T(r,z) = T0(r)+ b(r)*|z|
   */
  double z = fabs(zPosition);
  int sign=1;
  if(zPosition<0)sign=-1;
  double T0 = fitFuncPol_corrRZ(gasType, 0,driftRadius,0,StrawLayer,sign,1);
  double b  = fitFuncPol_corrRZ(gasType, 1,driftRadius,0,StrawLayer,sign,1);
  return T0+b*z; 
}


double TRT_ToT_dEdx::fitFuncPol_corrRZ(EGasType gasType, int parameter, double driftRadius, int Layer, int Strawlayer, int sign, int set) const
{
  
  double a = 0;
  double b = 0;
  double c = 0;
  double d = 0;
  double e = 0;
  double f = 0;
  double r = driftRadius;
  int offset = 0;
  if(isData()){
    if(set==0){ // long straws in barrel
      //int layerindex = Layer*30+Strawlayer;
      //int parId=0;
      //parId=0;
      //if(sign>0)parId=1620;  // FIXME: parId is not used
      a = Dedxcorrection::para_long_corrRZ[gasType][(6*parameter+0)*30*3+Layer*30+Strawlayer+offset];
      b = Dedxcorrection::para_long_corrRZ[gasType][(6*parameter+1)*30*3+Layer*30+Strawlayer+offset];
      c = Dedxcorrection::para_long_corrRZ[gasType][(6*parameter+2)*30*3+Layer*30+Strawlayer+offset];
      d = Dedxcorrection::para_long_corrRZ[gasType][(6*parameter+3)*30*3+Layer*30+Strawlayer+offset];
      e = Dedxcorrection::para_long_corrRZ[gasType][(6*parameter+4)*30*3+Layer*30+Strawlayer+offset];
      f = Dedxcorrection::para_long_corrRZ[gasType][(6*parameter+5)*30*3+Layer*30+Strawlayer+offset];
     
    }else if (set ==1) { // short straws in barrel
      if(sign > 0) offset+=108;
      a = Dedxcorrection::para_short_corrRZ[gasType][(6*parameter+0)*9+Layer+offset];
      b = Dedxcorrection::para_short_corrRZ[gasType][(6*parameter+1)*9+Layer+offset];
      c = Dedxcorrection::para_short_corrRZ[gasType][(6*parameter+2)*9+Layer+offset];
      d = Dedxcorrection::para_short_corrRZ[gasType][(6*parameter+3)*9+Layer+offset];
      e = Dedxcorrection::para_short_corrRZ[gasType][(6*parameter+4)*9+Layer+offset];
      f = Dedxcorrection::para_short_corrRZ[gasType][(6*parameter+5)*9+Layer+offset];
    }else{  // straws in endcap
      if(sign >0) Layer+=14;
      a = Dedxcorrection::para_end_corrRZ[gasType][(6*parameter+0)*28+Layer];
      b = Dedxcorrection::para_end_corrRZ[gasType][(6*parameter+1)*28+Layer];
      c = Dedxcorrection::para_end_corrRZ[gasType][(6*parameter+2)*28+Layer];
      d = Dedxcorrection::para_end_corrRZ[gasType][(6*parameter+3)*28+Layer];
      e = Dedxcorrection::para_end_corrRZ[gasType][(6*parameter+4)*28+Layer];
      f = Dedxcorrection::para_end_corrRZ[gasType][(6*parameter+5)*28+Layer];
    }
  }else{
    if(set==0){ // long straws in barrel
      if(sign > 0) offset=1620;
      a = Dedxcorrection::para_long_corrRZ_MC[gasType][(6*parameter+0)*30*3+Layer*30+Strawlayer+offset];
      b = Dedxcorrection::para_long_corrRZ_MC[gasType][(6*parameter+1)*30*3+Layer*30+Strawlayer+offset];
      c = Dedxcorrection::para_long_corrRZ_MC[gasType][(6*parameter+2)*30*3+Layer*30+Strawlayer+offset];
      d = Dedxcorrection::para_long_corrRZ_MC[gasType][(6*parameter+3)*30*3+Layer*30+Strawlayer+offset];
      e = Dedxcorrection::para_long_corrRZ_MC[gasType][(6*parameter+4)*30*3+Layer*30+Strawlayer+offset];
      f = Dedxcorrection::para_long_corrRZ_MC[gasType][(6*parameter+5)*30*3+Layer*30+Strawlayer+offset];
    }else if (set ==1) { // short straws in barrel
      if(sign > 0) offset+=108;
      a = Dedxcorrection::para_short_corrRZ_MC[gasType][(6*parameter+0)*9+Layer+offset];
      b = Dedxcorrection::para_short_corrRZ_MC[gasType][(6*parameter+1)*9+Layer+offset];
      c = Dedxcorrection::para_short_corrRZ_MC[gasType][(6*parameter+2)*9+Layer+offset];
      d = Dedxcorrection::para_short_corrRZ_MC[gasType][(6*parameter+3)*9+Layer+offset];
      e = Dedxcorrection::para_short_corrRZ_MC[gasType][(6*parameter+4)*9+Layer+offset];
      f = Dedxcorrection::para_short_corrRZ_MC[gasType][(6*parameter+5)*9+Layer+offset];
    }else{  // straws in endcap
      if(sign >0) Layer+=14;
      a = Dedxcorrection::para_end_corrRZ_MC[gasType][(6*parameter+0)*28+Layer];
      b = Dedxcorrection::para_end_corrRZ_MC[gasType][(6*parameter+1)*28+Layer];
      c = Dedxcorrection::para_end_corrRZ_MC[gasType][(6*parameter+2)*28+Layer];
      d = Dedxcorrection::para_end_corrRZ_MC[gasType][(6*parameter+3)*28+Layer];
      e = Dedxcorrection::para_end_corrRZ_MC[gasType][(6*parameter+4)*28+Layer];
      f = Dedxcorrection::para_end_corrRZ_MC[gasType][(6*parameter+5)*28+Layer];
    }    
  }
  return a+b*r+c*r*r+d*r*r*r+e*r*r*r*r+f*r*r*r*r*r;
}


double TRT_ToT_dEdx::fitFuncEndcap_corrRZL(EGasType gasType, double driftRadius,double radialPosition, int Layer, int sign) const 
{
  /*
   * T(r,R) = T0(r)+ a(r)*R
   */

  double r = fabs(driftRadius);
  double a,b,c,d,e,f,g,h,i;  
  if(sign >0) Layer+=14;
  if(isData()){
    a = Dedxcorrection::para_end_corrRZL_DATA[gasType][(0)*28+Layer];
    b = Dedxcorrection::para_end_corrRZL_DATA[gasType][(1)*28+Layer];
    c = Dedxcorrection::para_end_corrRZL_DATA[gasType][(2)*28+Layer];
    d = Dedxcorrection::para_end_corrRZL_DATA[gasType][(3)*28+Layer];
    e = Dedxcorrection::para_end_corrRZL_DATA[gasType][(4)*28+Layer];
    f = Dedxcorrection::para_end_corrRZL_DATA[gasType][(5)*28+Layer];  
    g = Dedxcorrection::para_end_corrRZL_DATA[gasType][(6)*28+Layer];  
    h = Dedxcorrection::para_end_corrRZL_DATA[gasType][(7)*28+Layer];  
    i = Dedxcorrection::para_end_corrRZL_DATA[gasType][(8)*28+Layer];  
  }else{
    a = Dedxcorrection::para_end_corrRZL_MC[gasType][(0)*28+Layer];
    b = Dedxcorrection::para_end_corrRZL_MC[gasType][(1)*28+Layer];
    c = Dedxcorrection::para_end_corrRZL_MC[gasType][(2)*28+Layer];
    d = Dedxcorrection::para_end_corrRZL_MC[gasType][(3)*28+Layer];
    e = Dedxcorrection::para_end_corrRZL_MC[gasType][(4)*28+Layer];
    f = Dedxcorrection::para_end_corrRZL_MC[gasType][(5)*28+Layer];  
    g = Dedxcorrection::para_end_corrRZL_MC[gasType][(6)*28+Layer];  
    h = Dedxcorrection::para_end_corrRZL_MC[gasType][(7)*28+Layer];  
    i = Dedxcorrection::para_end_corrRZL_MC[gasType][(8)*28+Layer]; 
  } 

  double T1    = b*r+c*r*r+d*r*r*r+e*r*r*r*r+f*r*r*r*r*r;
  double slope = g+h*r+i*r*r;
  double T0    = a;
  
  return T0+T1+slope*radialPosition;
}


double TRT_ToT_dEdx::fitFuncBarrel_corrRZL(EGasType gasType, double driftRadius,double zPosition, int Layer, int Strawlayer) const 
{
  /*
   * T(r,z) = T0(r)+ b(r)*z*z 
   */

  double a,b,c,d,e,f,g;  

  if(Layer==0 && Strawlayer<9){ // short straws
    if(isData()){
      a = Dedxcorrection::para_short_corrRZL_DATA[gasType][(0)*9+Strawlayer];
      b = Dedxcorrection::para_short_corrRZL_DATA[gasType][(1)*9+Strawlayer];
      c = Dedxcorrection::para_short_corrRZL_DATA[gasType][(2)*9+Strawlayer];
      d = Dedxcorrection::para_short_corrRZL_DATA[gasType][(3)*9+Strawlayer];
      e = Dedxcorrection::para_short_corrRZL_DATA[gasType][(4)*9+Strawlayer];
      f = Dedxcorrection::para_short_corrRZL_DATA[gasType][(5)*9+Strawlayer];
      g = Dedxcorrection::para_short_corrRZL_DATA[gasType][(6)*9+Strawlayer];
    }else{
      a = Dedxcorrection::para_short_corrRZL_MC[gasType][(0)*9+Strawlayer];
      b = Dedxcorrection::para_short_corrRZL_MC[gasType][(1)*9+Strawlayer];
      c = Dedxcorrection::para_short_corrRZL_MC[gasType][(2)*9+Strawlayer];
      d = Dedxcorrection::para_short_corrRZL_MC[gasType][(3)*9+Strawlayer];
      e = Dedxcorrection::para_short_corrRZL_MC[gasType][(4)*9+Strawlayer];
      f = Dedxcorrection::para_short_corrRZL_MC[gasType][(5)*9+Strawlayer];
      g = Dedxcorrection::para_short_corrRZL_MC[gasType][(6)*9+Strawlayer];
    }
    
  }else{
    if(isData()){
      a = Dedxcorrection::para_long_corrRZL_DATA[gasType][(0)*30*3+Layer*30+Strawlayer];
      b = Dedxcorrection::para_long_corrRZL_DATA[gasType][(1)*30*3+Layer*30+Strawlayer];
      c = Dedxcorrection::para_long_corrRZL_DATA[gasType][(2)*30*3+Layer*30+Strawlayer];
      d = Dedxcorrection::para_long_corrRZL_DATA[gasType][(3)*30*3+Layer*30+Strawlayer];
      e = Dedxcorrection::para_long_corrRZL_DATA[gasType][(4)*30*3+Layer*30+Strawlayer];
      f = Dedxcorrection::para_long_corrRZL_DATA[gasType][(5)*30*3+Layer*30+Strawlayer];
      g = Dedxcorrection::para_long_corrRZL_DATA[gasType][(6)*30*3+Layer*30+Strawlayer];
    }else{
      a = Dedxcorrection::para_long_corrRZL_MC[gasType][(0)*30*3+Layer*30+Strawlayer];
      b = Dedxcorrection::para_long_corrRZL_MC[gasType][(1)*30*3+Layer*30+Strawlayer];
      c = Dedxcorrection::para_long_corrRZL_MC[gasType][(2)*30*3+Layer*30+Strawlayer];
      d = Dedxcorrection::para_long_corrRZL_MC[gasType][(3)*30*3+Layer*30+Strawlayer];
      e = Dedxcorrection::para_long_corrRZL_MC[gasType][(4)*30*3+Layer*30+Strawlayer];
      f = Dedxcorrection::para_long_corrRZL_MC[gasType][(5)*30*3+Layer*30+Strawlayer];
      g = Dedxcorrection::para_long_corrRZL_MC[gasType][(6)*30*3+Layer*30+Strawlayer];
    }
  }
  double z = fabs(zPosition);
  double r = fabs(driftRadius);
  double T0neg=a;
  double T0pos=b;  
  double T1 = exp(-c*r*r)+d*r;
  double slope = e*r+f*r*r+g*r*r*r;  
  double result;
  result = T0neg+T1+slope*z;
  if(zPosition>0)result = T0pos+T1+slope*z;

  return result;
}



double TRT_ToT_dEdx::getToTlargerIsland(unsigned int BitPattern) const 
{
  unsigned long mask = 0x02000000;
  unsigned int best_length = 0;
  unsigned int current_length = 0;
  unsigned int k = 0;

  //shift bitmask to the right until end;
  while (true) {
    if (BitPattern & mask) {
      ++current_length;
    }
    else {
      // remember longest island
      if (current_length > best_length)
        best_length = current_length;
      current_length = 0;
    }
    if (!mask)
      break;
    assert(k < 24);
    mask >>= 1;
    if (k == 7 || k == 15)
      mask >>= 1;
    ++k;
  }
  assert(k == 24);
  return best_length*3.125; 
}



double TRT_ToT_dEdx::getToTonly1bits(unsigned int BitPattern) const 
{
  /********  Islands ********/
  unsigned  mask2 = 0x02000000;
  int k2;
  int i_island = 0;
  std::vector<int> StartIsland;
  std::vector<int> EndIsland;
  std::vector<int> LengthIsland;
  bool StartedIsland = false;
  bool EndedIsland = true;
  for(k2=0;k2<24;++k2) {
    if ( !(BitPattern & mask2) ) {
      if (StartedIsland) {
        EndIsland.push_back(k2-1);
        LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
        i_island = i_island+1;
        EndedIsland = true;
        StartedIsland = false;
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    } else {
      if (EndedIsland) {
        StartIsland.push_back(k2);
        StartedIsland = true;
        EndedIsland = false;
      }
      if (k2==23) {
        EndIsland.push_back(k2);
        LengthIsland.push_back(EndIsland[i_island]-StartIsland[i_island]+1);
      }
      mask2>>=1;
      if (k2==7 || k2==15) mask2>>=1;
    }
  }
  
  // ToT using only bits set to 1 (remove all 0)
  double ToT_only1bits = 0;
  for (unsigned int m=0 ; m<StartIsland.size() ; m++) {
    ToT_only1bits = ToT_only1bits + LengthIsland[m]*3.125;
  }
  
  return ToT_only1bits;
}


double TRT_ToT_dEdx::getToTHighOccupancy(unsigned int BitPattern) const 
{
  int LE = DriftTimeBin_v2(BitPattern);
  int TE = TrailingEdge_v2(BitPattern);
  if ( (0 == LE) || (24 == LE) || (24 == TE) || (0 == TE) || (23 == LE) )
    return 0;

  return (double) (TE-LE+1)*3.125; 
}



int TRT_ToT_dEdx::DriftTimeBin_v2(unsigned int BitPattern) const
{
  unsigned  mask = 0x02000000;
  unsigned  word_LE = BitPattern>>6;
  word_LE = word_LE<<6;
 
  mask >>=1;
  bool SawZero = false;
  int i;
  for(i=1;i<18;++i)
    { 
      if      (  (word_LE & mask) && SawZero) break;
      else if ( !(word_LE & mask) ) SawZero = true; 
      mask>>=1;
      if(i==7 || i==15) mask>>=1;
    }
  if(i==18) i=0;
  return i;
}  



int TRT_ToT_dEdx::TrailingEdge_v2(unsigned int BitPattern) const
{
  unsigned mask = 0x00000001;
  unsigned mask_word = 0x0001fff0;
  unsigned word_TE = BitPattern & mask_word;
  //bool SawZero=false;
  bool SawZero=true;
  int i;
  for (i = 0; i < 24; ++i)
    {
      if ( (word_TE & mask) && SawZero )
        break;
      else if ( !(word_TE & mask) )
        SawZero = true;

      mask <<= 1;
      if (i == 7 || i == 15)
        mask <<= 1;
    }
 
  if ( 24 == i )
    return i;

  return (23 - i);
}



double TRT_ToT_dEdx::getToTHighOccupancySmart(unsigned int BitPattern) const 
{
  int LE = DriftTimeBin_v2(BitPattern);
  int TE = TrailingEdge_v3(BitPattern);
  if ( (0 == LE) || (24 == LE) || (24 == TE) || (0 == TE) || (23 == LE) )
    return 0;

  return (double) (TE-LE+1)*3.125; 
}



int TRT_ToT_dEdx::TrailingEdge_v3(unsigned int BitPattern) const
{
  unsigned mask = 0x00000001;
  unsigned mask_word = 0x0001fff0; // 11111111 1 11110000   
  unsigned mask_last_bit =0x10; //10000
  
  unsigned word_TE = BitPattern & mask_word;
  
  bool SawZero=true;
  bool SawZero1=false;
  bool SawZero2=false;
  bool SawUnit1=false;
  int i=0;
  int j=0;
  int k=0;
  
  if(word_TE & mask_last_bit) 
    {
  
      for (j = 0; j < 11; ++j)
        {
          mask_last_bit=mask_last_bit<<1;
                
          if(j==3) mask_last_bit=mask_last_bit<<1;
                
          if ( !(word_TE & mask_last_bit) )
            {
              SawZero2 = true;
              break;                  
            }
        }
        
      if(SawZero2 == false) return 19;

      if(SawZero2 == true){
        for (k = j+1; k < 11; ++k)
          {
            mask_last_bit=mask_last_bit<<1;

            if(k==3) mask_last_bit=mask_last_bit<<1;

            if ( word_TE & mask_last_bit )
              {
                SawUnit1 = true;
                break;                                  
              }
          } 
      }
        
      if(SawUnit1 == false && SawZero2 == true) return 19;
        
    }
  
  //+++++++++++++++++++++++++++++++++++++
  
  for (i = 0; i < 24; ++i)
    {
      if(!(word_TE & mask) && i>3)
        {
          SawZero1 = true;
        }
            
      if(SawZero1)
        {  
          if ( (word_TE & mask) && SawZero )
            break;
          else if ( !(word_TE & mask) )
            SawZero = true;
        }
      mask <<= 1;
      if (i == 7 || i == 15)
        mask <<= 1;
    }
 
  if ( 24 == i )
    return i;

  return (23 - i);
}

 
double TRT_ToT_dEdx::mimicToXeHit_Endcap(EGasType gasType, double driftRadius, int Layer, int sign) const
{
  double r = fabs(driftRadius); 
  double a; 

  float rCopy = r;
  rCopy*=10;
  int rBin = int(rCopy);
  if(rBin<0 || rBin>19)
    {
      ATH_MSG_FATAL("mimicToXeHit_Endcap():: rBin = "<<rBin<<" and out of range [0,19].");
      throw std::exception();
    }

  int side = 0; // A side
  if(sign <0) side =1; // C side
  if(isData())
    a = Dedxcorrection::para_end_mimicToXe_DATA[gasType][(side*14+Layer)*20+(rBin)];
  else
    a = Dedxcorrection::para_end_mimicToXe_MC[gasType][(side*14+Layer)*20+(rBin)];

  ATH_MSG_DEBUG("mimicToXeHit_Endcap():: isData = " << isData() << " gasTypeInStraw = " << gasType
                << " side = " << side << " Layer = " << Layer << " rBin = " << rBin <<" BINPOS = " << (side*14+Layer)*20+(rBin) 
                << " a = " << a << "" );

  return a;
}

double TRT_ToT_dEdx::mimicToXeHit_Barrel(EGasType gasType, double driftRadius, int Layer, int Strawlayer) const 
{
  double r = fabs(driftRadius); 
  double a;  

  float rCopy = r;
  rCopy*=10;
  int rBin = int(rCopy);
  if(rBin<0 || rBin>19)
    {
      ATH_MSG_FATAL("mimicToXeHit_Barrel():: rBin = "<<rBin<<" and out of range [0,19].");
      throw std::exception();
    }

  if(Layer==0 && Strawlayer<9){ // short straws
    if(isData())
      a = Dedxcorrection::para_short_mimicToXe_DATA[gasType][Strawlayer*20+(rBin)];
    else
      a = Dedxcorrection::para_short_mimicToXe_MC[gasType][Strawlayer*20+(rBin)];
  }else{
    if(isData())
      a = Dedxcorrection::para_long_mimicToXe_DATA[gasType][Layer*30*20+Strawlayer*20+(rBin)];
    else
      a = Dedxcorrection::para_long_mimicToXe_MC[gasType][Layer*30*20+Strawlayer*20+(rBin)];
  }

  ATH_MSG_DEBUG("mimicToXeHit_Barrel():: isData = " << isData() << " Layer = " << Layer << " Strawlayer = " << Strawlayer << " rBin = " << rBin << " a = " << a << "" );

  return a;
}
