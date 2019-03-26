/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTToTCondAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListSpecification.h"


TRTToTCondAlg::TRTToTCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator),
    m_condSvc("CondSvc",name)
{}
TRTToTCondAlg::~TRTToTCondAlg(){}

StatusCode TRTToTCondAlg::initialize()
{

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Read key
  ATH_CHECK( m_VecReadKey.initialize() );
  ATH_CHECK( m_ValReadKey.initialize() );

  // Register write handle
  ATH_CHECK( m_WriteKey.initialize() );

  if (m_condSvc->regHandle(this, m_WriteKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_WriteKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TRTToTCondAlg::execute() 
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________

  SG::WriteCondHandle<TRTDedxcorrection> writeHandle{m_WriteKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }



  //  // ____________ Construct new Write Cond Object  ____________
  //  std::unique_ptr<TRTDedxcorrection> Dedxcorrection{std::make_unique<TRTDedxcorrection>()};
  TRTDedxcorrection* Dedxcorrection = new TRTDedxcorrection; 

  // ____________ Compute the Write Cond Object (Dedxcorrections)  ____________
  SG::ReadCondHandle<CondAttrListVec> readVecHandle{m_VecReadKey};
  const CondAttrListVec* channel_values{*readVecHandle};
  if(channel_values==nullptr) {
      ATH_MSG_ERROR(" Problem reading TRT/Calib/ToT/ToTVectors cond object");
      delete Dedxcorrection;
      return StatusCode::FAILURE;
  }
  if(StatusCode::SUCCESS != update1( *Dedxcorrection, channel_values  )) {
     ATH_MSG_ERROR ("Problem filling Dedxcorrection.");
     delete Dedxcorrection;
     return StatusCode::FAILURE;     
  }

  SG::ReadCondHandle<CondAttrListCollection> readValHandle{m_ValReadKey};
  const CondAttrListCollection* attrListColl{*readValHandle};
  if(attrListColl==nullptr) {
      ATH_MSG_ERROR(" Problem reading TRT/Calib/ToT/ToTValue cond object");
      delete Dedxcorrection;
      return StatusCode::FAILURE;
  }
  if(StatusCode::SUCCESS != update2( *Dedxcorrection, attrListColl  )) {
     ATH_MSG_ERROR ("Problem filling Dedxcorrection.");
     delete Dedxcorrection;
     return StatusCode::FAILURE;     
  }

 
  //__________ Assign range of Dedxcorrection to that of the ReadHandle___________ 
  EventIDRange rangeW;

  if(!readVecHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readVecHandle.key());
        delete Dedxcorrection;
        return StatusCode::FAILURE;
  }

  // Record  CDO
  if(writeHandle.record(rangeW,Dedxcorrection).isFailure()) {
    ATH_MSG_ERROR("Could not record " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    delete Dedxcorrection;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode TRTToTCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode TRTToTCondAlg::update1( TRTDedxcorrection& Dedxcorrection, const CondAttrListVec* channel_values){

  std::vector<std::string>  dict_names = {"para_end_corrRZLXe","para_end_corrRZ_Xe","para_end_mimicToXeXe",
    "para_long_corrRZLXe","para_long_corrRZ_Xe","para_long_mimicToXeXe","para_short_corrRZLXe",
    "para_short_corrRZ_Xe","para_short_mimicToXeXe","resolution_Xe","resolution_e_Xe","para_end_corrRZLAr",
    "para_end_corrRZ_Ar","para_end_mimicToXeAr","para_long_corrRZLAr","para_long_corrRZ_Ar",
    "para_long_mimicToXeAr","para_short_corrRZLAr","para_short_corrRZ_Ar","para_short_mimicToXeAr",
    "resolution_Ar","resolution_e_Ar","para_end_corrRZLKr","para_end_corrRZ_Kr","para_end_mimicToXeKr",
    "para_long_corrRZLKr","para_long_corrRZ_Kr","para_long_mimicToXeKr","para_short_corrRZLKr",
    "para_short_corrRZ_Kr","para_short_mimicToXeKr","resolution_Kr","resolution_e_Kr"};

  std::map<std::string,std::vector<float> > result_dict;
  int dataBaseType = kNewDB;
  ATH_MSG_DEBUG("update():: dict_names[]="<<dict_names.size()<<", channel_values[]="<<channel_values->size()<<"");
  if(channel_values->size()<19695) dataBaseType = kOldDB; 

  if(dataBaseType==kNewDB) {
        
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
                        
      update_New(Dedxcorrection, result_dict);
      ATH_MSG_DEBUG ("update():: Reading new database is done!");

      return StatusCode::SUCCESS;
                
  } else if(dataBaseType==kOldDB) {
        ATH_MSG_WARNING ("update():: Old COOL database tag!");

        std::vector<std::string>  dict_names_old = {"resolution","resolution_e","para_long_corrRZ_MC",
             "para_short_corrRZ_MC","para_end_corrRZ_MC","para_long_corrRZL_MC",
             "para_short_corrRZL_MC","para_end_corrRZL_MC"};
        
        CondAttrListVec::const_iterator first_channel = channel_values->begin();
        CondAttrListVec::const_iterator last_channel  = channel_values->end();

        unsigned int current_channel = 0;
        std::vector<float> current_array_values = {};

        for (; first_channel != last_channel; ++first_channel) {
            if (current_channel != first_channel->first) {
                result_dict[dict_names_old[current_channel]] = current_array_values;
                current_channel = first_channel->first;      
                current_array_values.clear();
             }
             current_array_values.push_back(first_channel->second["array_value"].data<float>());             
        }
                        
        result_dict[dict_names_old[current_channel]] = current_array_values;

        update_Old(Dedxcorrection, result_dict);
        ATH_MSG_DEBUG ("update():: Reading old database is done!");

        return StatusCode::SUCCESS;
  }
        return StatusCode::FAILURE;
}

void TRTToTCondAlg::update_New(TRTDedxcorrection & Dedxcorrection, std::map<std::string,std::vector<float> > &result_dict) {
  //      fill Xenon +++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection.resolution[0][ind]=result_dict["resolution_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection.resolution_e[0][ind]=result_dict["resolution_e_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection.para_long_corrRZ_MC[0][ind]=result_dict["para_long_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection.para_short_corrRZ_MC[0][ind]=result_dict["para_short_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection.para_long_corrRZL_MC[0][ind]=result_dict["para_long_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection.para_short_corrRZL_MC[0][ind]=result_dict["para_short_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection.para_end_corrRZL_MC[0][ind]=result_dict["para_end_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection.para_long_corrRZ[0][ind]=result_dict["para_long_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection.para_short_corrRZ[0][ind]=result_dict["para_short_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection.para_long_corrRZL_DATA[0][ind]=result_dict["para_long_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection.para_short_corrRZL_DATA[0][ind]=result_dict["para_short_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection.para_end_corrRZL_DATA[0][ind]=result_dict["para_end_corrRZLXe"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection.para_end_corrRZ[0][ind]=result_dict["para_end_corrRZ_Xe"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection.para_end_corrRZ_MC[0][ind]=result_dict["para_end_corrRZ_Xe"][ind];
  }

        
                
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection.para_end_mimicToXe_DATA[0][ind]=result_dict["para_end_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection.para_end_mimicToXe_MC[0][ind]=result_dict["para_end_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection.para_short_mimicToXe_DATA[0][ind]=result_dict["para_short_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection.para_short_mimicToXe_MC[0][ind]=result_dict["para_short_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection.para_long_mimicToXe_DATA[0][ind]=result_dict["para_long_mimicToXeXe"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection.para_long_mimicToXe_MC[0][ind]=result_dict["para_long_mimicToXeXe"][ind];
  }

  //      fill Argon +++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection.resolution[1][ind]=result_dict["resolution_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection.resolution_e[1][ind]=result_dict["resolution_e_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection.para_long_corrRZ_MC[1][ind]=result_dict["para_long_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection.para_short_corrRZ_MC[1][ind]=result_dict["para_short_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection.para_long_corrRZL_MC[1][ind]=result_dict["para_long_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection.para_short_corrRZL_MC[1][ind]=result_dict["para_short_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection.para_end_corrRZL_MC[1][ind]=result_dict["para_end_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection.para_long_corrRZ[1][ind]=result_dict["para_long_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection.para_short_corrRZ[1][ind]=result_dict["para_short_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection.para_long_corrRZL_DATA[1][ind]=result_dict["para_long_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection.para_short_corrRZL_DATA[1][ind]=result_dict["para_short_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection.para_end_corrRZL_DATA[1][ind]=result_dict["para_end_corrRZLAr"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection.para_end_corrRZ[1][ind]=result_dict["para_end_corrRZ_Ar"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection.para_end_corrRZ_MC[1][ind]=result_dict["para_end_corrRZ_Ar"][ind];
  }

        
                
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection.para_end_mimicToXe_DATA[1][ind]=result_dict["para_end_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection.para_end_mimicToXe_MC[1][ind]=result_dict["para_end_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection.para_short_mimicToXe_DATA[1][ind]=result_dict["para_short_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection.para_short_mimicToXe_MC[1][ind]=result_dict["para_short_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection.para_long_mimicToXe_DATA[1][ind]=result_dict["para_long_mimicToXeAr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection.para_long_mimicToXe_MC[1][ind]=result_dict["para_long_mimicToXeAr"][ind];
  }

  //      fill Krypton +++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection.resolution[2][ind]=result_dict["resolution_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 4; ++ind) {
    Dedxcorrection.resolution_e[2][ind]=result_dict["resolution_e_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection.para_long_corrRZ_MC[2][ind]=result_dict["para_long_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection.para_short_corrRZ_MC[2][ind]=result_dict["para_short_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection.para_long_corrRZL_MC[2][ind]=result_dict["para_long_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection.para_short_corrRZL_MC[2][ind]=result_dict["para_short_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection.para_end_corrRZL_MC[2][ind]=result_dict["para_end_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 3240; ++ind) {
    Dedxcorrection.para_long_corrRZ[2][ind]=result_dict["para_long_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 216; ++ind) {
    Dedxcorrection.para_short_corrRZ[2][ind]=result_dict["para_short_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 630; ++ind) {
    Dedxcorrection.para_long_corrRZL_DATA[2][ind]=result_dict["para_long_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 63; ++ind) {
    Dedxcorrection.para_short_corrRZL_DATA[2][ind]=result_dict["para_short_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 252; ++ind) {
    Dedxcorrection.para_end_corrRZL_DATA[2][ind]=result_dict["para_end_corrRZLKr"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection.para_end_corrRZ[2][ind]=result_dict["para_end_corrRZ_Kr"][ind];
  }

  for (unsigned int ind=0; ind < 336; ++ind) {
    Dedxcorrection.para_end_corrRZ_MC[2][ind]=result_dict["para_end_corrRZ_Kr"][ind];
  }

        
                
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection.para_end_mimicToXe_DATA[2][ind]=result_dict["para_end_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 560; ++ind) {
    Dedxcorrection.para_end_mimicToXe_MC[2][ind]=result_dict["para_end_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection.para_short_mimicToXe_DATA[2][ind]=result_dict["para_short_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 180; ++ind) {
    Dedxcorrection.para_short_mimicToXe_MC[2][ind]=result_dict["para_short_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection.para_long_mimicToXe_DATA[2][ind]=result_dict["para_long_mimicToXeKr"][ind];
  }
  for (unsigned int ind=0; ind < 1800; ++ind) {
    Dedxcorrection.para_long_mimicToXe_MC[2][ind]=result_dict["para_long_mimicToXeKr"][ind];
  }
}



void TRTToTCondAlg::update_Old(TRTDedxcorrection & Dedxcorrection, std::map<std::string,std::vector<float> > &result_dict) {
  for(int gasType = 0; gasType<3; gasType++) { // loop over gas types
    for (unsigned int ind=0; ind < 4; ++ind) {
      Dedxcorrection.resolution[gasType][ind]=result_dict["resolution"][ind];
    }

    for (unsigned int ind=0; ind < 4; ++ind) {
      Dedxcorrection.resolution_e[gasType][ind]=result_dict["resolution_e"][ind];
    }

    for (unsigned int ind=0; ind < 3240; ++ind) {
      Dedxcorrection.para_long_corrRZ_MC[gasType][ind]=result_dict["para_long_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 216; ++ind) {
      Dedxcorrection.para_short_corrRZ_MC[gasType][ind]=result_dict["para_short_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 630; ++ind) {
      Dedxcorrection.para_long_corrRZL_MC[gasType][ind]=result_dict["para_long_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 63; ++ind) {
      Dedxcorrection.para_short_corrRZL_MC[gasType][ind]=result_dict["para_short_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 252; ++ind) {
      Dedxcorrection.para_end_corrRZL_MC[gasType][ind]=result_dict["para_end_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 3240; ++ind) {
      Dedxcorrection.para_long_corrRZ[gasType][ind]=result_dict["para_long_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 216; ++ind) {
      Dedxcorrection.para_short_corrRZ[gasType][ind]=result_dict["para_short_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 630; ++ind) {
      Dedxcorrection.para_long_corrRZL_DATA[gasType][ind]=result_dict["para_long_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 63; ++ind) {
      Dedxcorrection.para_short_corrRZL_DATA[gasType][ind]=result_dict["para_short_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 252; ++ind) {
      Dedxcorrection.para_end_corrRZL_DATA[gasType][ind]=result_dict["para_end_corrRZL_MC"][ind];
    }

    for (unsigned int ind=0; ind < 336; ++ind) {
      Dedxcorrection.para_end_corrRZ[gasType][ind]=result_dict["para_end_corrRZ_MC"][ind];
    }

    for (unsigned int ind=0; ind < 336; ++ind) {
      Dedxcorrection.para_end_corrRZ_MC[gasType][ind]=result_dict["para_end_corrRZ_MC"][ind];
    }

    // Setting aditional corrections
    for (unsigned int ind=0; ind < 560; ++ind) {
      Dedxcorrection.para_end_mimicToXe_MC[gasType][ind]   = 1.;
      Dedxcorrection.para_end_mimicToXe_DATA[gasType][ind] = 1.;
    }

    for (unsigned int ind=0; ind < 180; ++ind) {
      Dedxcorrection.para_short_mimicToXe_MC[gasType][ind]   = 1.;
      Dedxcorrection.para_short_mimicToXe_DATA[gasType][ind] = 1.;
    }

    for (unsigned int ind=0; ind < 1800; ++ind) {
      Dedxcorrection.para_long_mimicToXe_MC[gasType][ind]   = 1.;
      Dedxcorrection.para_long_mimicToXe_DATA[gasType][ind] = 1.;
    }
  }
}

StatusCode TRTToTCondAlg::update2(TRTDedxcorrection& Dedxcorrection, const CondAttrListCollection* attrListColl ){

  int dataBaseType = kNewDB;
  if(attrListColl->size() < 2) dataBaseType = kOldDB;

  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();

  if(dataBaseType==kNewDB) {
     for (int index=0; first != last; ++first,++index) {
        const coral::AttributeList& attrList = (*first).second;
        Dedxcorrection.paraL_dEdx_p1[index] = attrList["paraL_dEdx_p1"].data<float>();
        Dedxcorrection.paraL_dEdx_p2[index] = attrList["paraL_dEdx_p2"].data<float>();
        Dedxcorrection.paraL_dEdx_p3[index] = attrList["paraL_dEdx_p3"].data<float>();
        Dedxcorrection.paraL_dEdx_p4[index] = attrList["paraL_dEdx_p4"].data<float>();
        Dedxcorrection.paraL_dEdx_p5[index] = attrList["paraL_dEdx_p5"].data<float>();

        Dedxcorrection.para_dEdx_p1[index] = attrList["para_dEdx_p1"].data<float>();
        Dedxcorrection.para_dEdx_p2[index] = attrList["para_dEdx_p2"].data<float>();
        Dedxcorrection.para_dEdx_p3[index] = attrList["para_dEdx_p3"].data<float>();
        Dedxcorrection.para_dEdx_p4[index] = attrList["para_dEdx_p4"].data<float>();
        Dedxcorrection.para_dEdx_p5[index] = attrList["para_dEdx_p5"].data<float>();
                                  
        Dedxcorrection.norm_offset_data[index] = attrList["norm_offset_data"].data<float>();
        Dedxcorrection.norm_slope_tot[index] = attrList["norm_slope_tot"].data<float>();  
        Dedxcorrection.norm_slope_totl[index] = attrList["norm_slope_totl"].data<float>(); 
        Dedxcorrection.norm_offset_tot[index] = attrList["norm_offset_tot"].data<float>(); 
        Dedxcorrection.norm_offset_totl[index] = attrList["norm_offset_totl"].data<float>();           
        Dedxcorrection.norm_nzero[index]=attrList["norm_nzero"].data<int>();
     }
  } else {
     ATH_MSG_WARNING ("update2():: Old COOL database tag!");
     // return update2_Old();
     for (; first != last; ++first) {  
        const coral::AttributeList& attrList = (*first).second;
        for(int gasType=0; gasType<3; gasType++) {
           Dedxcorrection.paraL_dEdx_p1[gasType] = attrList["paraL_dEdx_p1"].data<float>();
           Dedxcorrection.paraL_dEdx_p2[gasType] = attrList["paraL_dEdx_p2"].data<float>();
           Dedxcorrection.paraL_dEdx_p3[gasType] = attrList["paraL_dEdx_p3"].data<float>();
           Dedxcorrection.paraL_dEdx_p4[gasType] = attrList["paraL_dEdx_p4"].data<float>();
           Dedxcorrection.paraL_dEdx_p5[gasType] = attrList["paraL_dEdx_p5"].data<float>();

           Dedxcorrection.para_dEdx_p1[gasType] = attrList["para_dEdx_p1"].data<float>();
           Dedxcorrection.para_dEdx_p2[gasType] = attrList["para_dEdx_p2"].data<float>();
           Dedxcorrection.para_dEdx_p3[gasType] = attrList["para_dEdx_p3"].data<float>();
           Dedxcorrection.para_dEdx_p4[gasType] = attrList["para_dEdx_p4"].data<float>();
           Dedxcorrection.para_dEdx_p5[gasType] = attrList["para_dEdx_p5"].data<float>();

           Dedxcorrection.norm_offset_data[gasType] = attrList["norm_offset_data"].data<float>();
           Dedxcorrection.norm_slope_tot[gasType] = attrList["norm_slope_tot"].data<float>();  
           Dedxcorrection.norm_slope_totl[gasType] = attrList["norm_slope_totl"].data<float>(); 
           Dedxcorrection.norm_offset_tot[gasType] = attrList["norm_offset_tot"].data<float>(); 
           Dedxcorrection.norm_offset_totl[gasType] = attrList["norm_offset_totl"].data<float>();         
           Dedxcorrection.norm_nzero[gasType]=attrList["norm_nzero"].data<int>(); 
        }
     }
  }

  return StatusCode::SUCCESS;
}


