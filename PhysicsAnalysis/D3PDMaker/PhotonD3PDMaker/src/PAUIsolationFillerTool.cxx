/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PAUIsolationFillerTool.h
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date January, 2011
 * @brief Fill PAU related variables
 */


#include "PAUIsolationFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "PhotonAnalysisUtils/PAUegamma.h"

using namespace PAU;

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PAUIsolationFillerTool::PAUIsolationFillerTool (const std::string& type,
						const std::string& name,
						const IInterface* parent)
  : BlockFillerTool<PAU::egamma> (type, name, parent)
{
  // Avoid coverity warnings.
  book().ignore();
  m_ED_median_new = 0;
  m_ED_sigma_new = 0;
  m_ED_Njets_new = 0;

  declareProperty("Fill_EtCone_pt_corrected", m_fill_EtCone_pt_corrected);
  declareProperty("Fill_EtCone_ED_corrected", m_fill_EtCone_ED_corrected);
  declareProperty("Fill_EtCone_corrected"   , m_fill_EtCone_corrected   );
  declareProperty("Fill_EtCone_ED_corrected_new", m_fill_EtCone_ED_corrected_new);
  declareProperty("Fill_EtCone_corrected_new"   , m_fill_EtCone_corrected_new   );

  declareProperty("Fill_EtCone_cells35"     , m_fill_EtCone_cells35     );
  declareProperty("Fill_EtCone_cells55"     , m_fill_EtCone_cells55     );

  declareProperty("Fill_EtConeIso"          , m_fill_EtConeIso          );
  declareProperty("Fill_EtConeIso_cells35"  , m_fill_EtConeIso_cells35  );
  declareProperty("Fill_EtConeIso_cells55"  , m_fill_EtConeIso_cells55  );

  // set up some relationships
  m_fill_to_value_map[&m_fill_EtCone_pt_corrected] = &m_EtCone_pt_corrected;
  m_fill_to_value_map[&m_fill_EtCone_ED_corrected] = &m_EtCone_ED_corrected;
  m_fill_to_value_map[&m_fill_EtCone_corrected]    = &m_EtCone_corrected   ;
  m_fill_to_value_map[&m_fill_EtCone_ED_corrected_new] = &m_EtCone_ED_corrected_new;
  m_fill_to_value_map[&m_fill_EtCone_corrected_new]    = &m_EtCone_corrected_new   ;

  m_fill_to_value_map[&m_fill_EtCone_cells35]      = &m_EtCone_cells35     ;
  m_fill_to_value_map[&m_fill_EtCone_cells55]      = &m_EtCone_cells55     ;

  m_fill_to_value_map[&m_fill_EtConeIso]           = &m_EtConeIso          ;
  m_fill_to_value_map[&m_fill_EtConeIso_cells35]   = &m_EtConeIso_cells35  ;
  m_fill_to_value_map[&m_fill_EtConeIso_cells55]   = &m_EtConeIso_cells55  ;

  // initialize some default names
  m_fill_to_name_map[&m_fill_EtCone_pt_corrected] = "Etcone%d_pt_corrected";
  m_fill_to_name_map[&m_fill_EtCone_ED_corrected] = "Etcone%d_ED_corrected";
  m_fill_to_name_map[&m_fill_EtCone_corrected]    = "Etcone%d_corrected"   ;
  m_fill_to_name_map[&m_fill_EtCone_ED_corrected_new] = "Etcone%d_ED_corrected_new";
  m_fill_to_name_map[&m_fill_EtCone_corrected_new]    = "Etcone%d_corrected_new"   ;
		    						    
  m_fill_to_name_map[&m_fill_EtCone_cells35]      = "cells35_Etcone%d"     ;
  m_fill_to_name_map[&m_fill_EtCone_cells55]      = "cells55_Etcone%d"     ;
		    						    
  m_fill_to_name_map[&m_fill_EtConeIso]           = "Etcone%dIso"          ;
  m_fill_to_name_map[&m_fill_EtConeIso_cells35]   = "cells35_Etcone%dIso"  ;
  m_fill_to_name_map[&m_fill_EtConeIso_cells55]   = "cells55_Etcone%dIso"  ;

  // initialize some default acceptable values
  m_EtCone_pt_corrected[10] = NULL;
  m_EtCone_pt_corrected[15] = NULL;
  m_EtCone_pt_corrected[20] = NULL;
  m_EtCone_pt_corrected[25] = NULL;
  m_EtCone_pt_corrected[30] = NULL;
  m_EtCone_pt_corrected[35] = NULL;
  m_EtCone_pt_corrected[40] = NULL;

  m_EtCone_ED_corrected[10] = NULL;
  m_EtCone_ED_corrected[15] = NULL;
  m_EtCone_ED_corrected[20] = NULL;
  m_EtCone_ED_corrected[25] = NULL;
  m_EtCone_ED_corrected[30] = NULL;
  m_EtCone_ED_corrected[35] = NULL;
  m_EtCone_ED_corrected[40] = NULL;

  m_EtCone_corrected[10] = NULL;
  m_EtCone_corrected[15] = NULL;
  m_EtCone_corrected[20] = NULL;
  m_EtCone_corrected[25] = NULL;
  m_EtCone_corrected[30] = NULL;
  m_EtCone_corrected[35] = NULL;
  m_EtCone_corrected[40] = NULL;

  m_EtCone_ED_corrected_new[10] = NULL;
  m_EtCone_ED_corrected_new[15] = NULL;
  m_EtCone_ED_corrected_new[20] = NULL;
  m_EtCone_ED_corrected_new[25] = NULL;
  m_EtCone_ED_corrected_new[30] = NULL;
  m_EtCone_ED_corrected_new[35] = NULL;
  m_EtCone_ED_corrected_new[40] = NULL;

  m_EtCone_corrected_new[10] = NULL;
  m_EtCone_corrected_new[15] = NULL;
  m_EtCone_corrected_new[20] = NULL;
  m_EtCone_corrected_new[25] = NULL;
  m_EtCone_corrected_new[30] = NULL;
  m_EtCone_corrected_new[35] = NULL;
  m_EtCone_corrected_new[40] = NULL;

  m_EtCone_cells35[20] = NULL;
  m_EtCone_cells35[30] = NULL;
  m_EtCone_cells35[40] = NULL;

  m_EtCone_cells55[20] = NULL;
  m_EtCone_cells55[30] = NULL;
  m_EtCone_cells55[40] = NULL;

  m_EtConeIso[20] = NULL;
  m_EtConeIso[30] = NULL;
  m_EtConeIso[40] = NULL;

  m_EtConeIso_cells35[20] = NULL;
  m_EtConeIso_cells35[30] = NULL;
  m_EtConeIso_cells35[40] = NULL;

  m_EtConeIso_cells55[20] = NULL;
  m_EtConeIso_cells55[30] = NULL;
  m_EtConeIso_cells55[40] = NULL;


  // collect all of the maps for later
  m_all_vecs.push_back(&m_fill_EtCone_pt_corrected);
  m_all_vecs.push_back(&m_fill_EtCone_ED_corrected);
  m_all_vecs.push_back(&m_fill_EtCone_corrected);
  m_all_vecs.push_back(&m_fill_EtCone_ED_corrected_new);
  m_all_vecs.push_back(&m_fill_EtCone_corrected_new);

  m_all_vecs.push_back(&m_fill_EtCone_cells35);
  m_all_vecs.push_back(&m_fill_EtCone_cells55);

  m_all_vecs.push_back(&m_fill_EtConeIso);
  m_all_vecs.push_back(&m_fill_EtConeIso_cells35);
  m_all_vecs.push_back(&m_fill_EtConeIso_cells55);
}


/**
 * @brief Book variables for this block.
 */
StatusCode PAUIsolationFillerTool::book()
{
  for(unsigned int j=0; j<m_all_vecs.size(); j++){
    std::vector<float>* fill_vec = m_all_vecs[j];
    std::map<int, float*>* value_map = m_fill_to_value_map[fill_vec];
    std::string var_name_template = m_fill_to_name_map[fill_vec];

    for(unsigned int i=0; i<fill_vec->size(); i++){
      int radius = 0;
      float float_radius = fill_vec->at(i);
      if(float_radius < 9) radius = (int)(float_radius*10);
      else                 radius = (int)(float_radius);
      
      if(value_map->find((int)radius) == value_map->end()) continue;
      
      char varname[100];
      snprintf(varname, 100, var_name_template.c_str(), (int)radius);
      std::string vn(varname);
      CHECK( addVariable( vn, value_map->at(radius) ) );
    }
  }

  CHECK ( addVariable( "ED_median", m_ED_median ) );
  CHECK ( addVariable( "ED_sigma" , m_ED_sigma  ) );
  CHECK ( addVariable( "ED_Njets" , m_ED_Njets  ) );

  if(m_fill_EtCone_corrected_new.size()  || m_fill_EtCone_ED_corrected_new.size()){
    CHECK ( addVariable( "ED_median_new", m_ED_median_new ) );
    CHECK ( addVariable( "ED_sigma_new" , m_ED_sigma_new  ) );
    CHECK ( addVariable( "ED_Njets_new" , m_ED_Njets_new  ) );
  }

  CHECK ( addVariable( "transmin_etcone40" , m_transmin_etcone40  ) );
  CHECK ( addVariable( "transmax_etcone40" , m_transmax_etcone40  ) );
  CHECK ( addVariable( "transmin_etcone100", m_transmin_etcone100 ) );
  CHECK ( addVariable( "transmax_etcone100", m_transmax_etcone100 ) );

  CHECK ( addVariable( "transmin", m_transmin ) );
  CHECK ( addVariable( "transmax", m_transmax ) );

  CHECK ( addVariable( "transmin_loweta", m_transmin_loweta ) );
  CHECK ( addVariable( "transmax_loweta", m_transmax_loweta ) );
  
  return StatusCode::SUCCESS;
}


void FillIso(float* fillme, float val){
  if(fillme != NULL) *fillme = val;
}

/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode PAUIsolationFillerTool::fill (const PAU::egamma& p)
{

  // initialize some default acceptable values
  FillIso( m_EtCone_pt_corrected[10], p.etcone10_pt_corrected );
  FillIso( m_EtCone_pt_corrected[15], p.etcone15_pt_corrected );
  FillIso( m_EtCone_pt_corrected[20], p.etcone20_pt_corrected );
  FillIso( m_EtCone_pt_corrected[25], p.etcone25_pt_corrected );
  FillIso( m_EtCone_pt_corrected[30], p.etcone30_pt_corrected );
  FillIso( m_EtCone_pt_corrected[35], p.etcone35_pt_corrected );
  FillIso( m_EtCone_pt_corrected[40], p.etcone40_pt_corrected );

  FillIso( m_EtCone_ED_corrected[10], p.etcone10_ED_corrected );
  FillIso( m_EtCone_ED_corrected[15], p.etcone15_ED_corrected );
  FillIso( m_EtCone_ED_corrected[20], p.etcone20_ED_corrected );
  FillIso( m_EtCone_ED_corrected[25], p.etcone25_ED_corrected );
  FillIso( m_EtCone_ED_corrected[30], p.etcone30_ED_corrected );
  FillIso( m_EtCone_ED_corrected[35], p.etcone35_ED_corrected );
  FillIso( m_EtCone_ED_corrected[40], p.etcone40_ED_corrected );

  FillIso( m_EtCone_corrected[10], p.etcone10_corrected );
  FillIso( m_EtCone_corrected[15], p.etcone15_corrected );
  FillIso( m_EtCone_corrected[20], p.etcone20_corrected );
  FillIso( m_EtCone_corrected[25], p.etcone25_corrected );
  FillIso( m_EtCone_corrected[30], p.etcone30_corrected );
  FillIso( m_EtCone_corrected[35], p.etcone35_corrected );
  FillIso( m_EtCone_corrected[40], p.etcone40_corrected );

  FillIso( m_EtCone_ED_corrected_new[10], p.etcone10_ED_corrected_new );
  FillIso( m_EtCone_ED_corrected_new[15], p.etcone15_ED_corrected_new );
  FillIso( m_EtCone_ED_corrected_new[20], p.etcone20_ED_corrected_new );
  FillIso( m_EtCone_ED_corrected_new[25], p.etcone25_ED_corrected_new );
  FillIso( m_EtCone_ED_corrected_new[30], p.etcone30_ED_corrected_new );
  FillIso( m_EtCone_ED_corrected_new[35], p.etcone35_ED_corrected_new );
  FillIso( m_EtCone_ED_corrected_new[40], p.etcone40_ED_corrected_new );

  FillIso( m_EtCone_corrected_new[10], p.etcone10_corrected_new );
  FillIso( m_EtCone_corrected_new[15], p.etcone15_corrected_new );
  FillIso( m_EtCone_corrected_new[20], p.etcone20_corrected_new );
  FillIso( m_EtCone_corrected_new[25], p.etcone25_corrected_new );
  FillIso( m_EtCone_corrected_new[30], p.etcone30_corrected_new );
  FillIso( m_EtCone_corrected_new[35], p.etcone35_corrected_new );
  FillIso( m_EtCone_corrected_new[40], p.etcone40_corrected_new );

  FillIso( m_EtCone_cells35[20], p.cells35etcone20 );
  FillIso( m_EtCone_cells35[30], p.cells35etcone30 );
  FillIso( m_EtCone_cells35[40], p.cells35etcone40 );
			   					     
  FillIso( m_EtCone_cells55[20], p.cells55etcone20 );
  FillIso( m_EtCone_cells55[30], p.cells55etcone30 );
  FillIso( m_EtCone_cells55[40], p.cells55etcone40 );

  FillIso( m_EtConeIso[20], p.etcone20iso );
  FillIso( m_EtConeIso[30], p.etcone30iso );
  FillIso( m_EtConeIso[40], p.etcone40iso );

  FillIso( m_EtConeIso_cells35[20], p.cells35etcone20iso );
  FillIso( m_EtConeIso_cells35[30], p.cells35etcone30iso );
  FillIso( m_EtConeIso_cells35[40], p.cells35etcone40iso );

  FillIso( m_EtConeIso_cells55[20], p.cells55etcone20iso );
  FillIso( m_EtConeIso_cells55[30], p.cells55etcone30iso );
  FillIso( m_EtConeIso_cells55[40], p.cells55etcone40iso );

  * m_ED_median		   = p.ED_median	  ;
  * m_ED_sigma		   = p.ED_sigma	          ;
  * m_ED_Njets		   = p.ED_Njets	          ;

  if(m_fill_EtCone_corrected_new.size()  || m_fill_EtCone_ED_corrected_new.size()){
    * m_ED_median_new		   = p.ED_median_new	  ;
    * m_ED_sigma_new		   = p.ED_sigma_new	          ;
    * m_ED_Njets_new		   = p.ED_Njets_new	          ;
  }

  * m_transmin_etcone40	   = p.transmin_etcone40  ;
  * m_transmax_etcone40	   = p.transmax_etcone40  ;
  * m_transmin_etcone100   = p.transmin_etcone100 ;
  * m_transmax_etcone100   = p.transmax_etcone100 ;
  * m_transmin		   = p.transmin	          ;
  * m_transmax		   = p.transmax	          ;
  * m_transmin_loweta	   = p.transmin_loweta    ;
  * m_transmax_loweta      = p.transmax_loweta    ;

  return StatusCode::SUCCESS;
}


} // namespace D3PD
