// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <math.h>
#include "TrigT1RPCmonitoring/DetailedTW.h"

using namespace std;

#ifdef LVL1_STANDALONE
DetailedTW::DetailedTW(const RPCdata* rpc_data) : BaseObject(Monitoring,"DetailedTW")
#else
DetailedTW::DetailedTW(const RPCdata* rpc_data,const IRPCcablingSvc* cabling) :
    BaseObject(Data,"DetailedTW")
#endif
{
    m_how_many = 0;

    m_global_v.event = 0;
    m_global_v.pt = 0;
    m_global_v.sign = 0;
    m_global_v.check_conf = 0;
    m_global_v.check_high = 0;
    m_global_v.majority_low = 0;
    m_global_v.majority_high = 0;

    m_list_pivot.clear();
    m_list_lowPt.clear() ;
    m_list_highPt.clear();


#ifdef LVL1_STANDALONE
    const CablingRPCBase* cabling = RPCcabling::CablingRPC::instance();
#endif

    if(!cabling)
    {
        DISP << "Creation of Trigger Window  data is not possible: no cabling service!"
             << endl;
        DISP_ERROR;
        return;
    }

    RPCdata::digitList eta = rpc_data->eta_digits_list();
    RPCdata::digitList::const_iterator digi = eta.begin();

    while(digi != eta.end())
    {
        const int station      = (*digi)->decoding().lvl1_station();

        if      (station == 2)  load_pivot_digi(*digi,cabling);
	else if (station == 1)  load_lowPt_digi(*digi,cabling);
        else if (station == 3)  load_highPt_digi(*digi,cabling);

        ++digi;
    }

}

DetailedTW::~DetailedTW()
{
    m_list_pivot.clear();
    m_list_lowPt.clear() ;
    m_list_highPt.clear();
}


#ifdef LVL1_STANDALONE
void DetailedTW::load_pivot_digi(const RPCdigit* digi,const CablingRPCBase* cabling)
#else
void DetailedTW::load_pivot_digi(const RPCdigit* digi,const IRPCcablingSvc* cabling)
#endif
{
    DISP << "DetailedTW::load_pivot_digi -> Pivot digit n. " << m_list_pivot.size() + 1;
    DISP_DEBUG;

    pivot_vars temp_vars;

    // Get logic values from the digit
    unsigned long str_code = digi->decoding().code();
    const int sector       = digi->decoding().logic_sector();
    const ViewType type    = digi->decoding().view();
    const int station      = digi->decoding().lvl1_station();
    const int cabling_code = digi->decoding().cabling_code();

    const CMAparameters::CMAlist list =
                          cabling->give_CMAs(sector,type,station,cabling_code);

    if(list.size() > 1)
    {
        DISP << "DetailedTW::load_pivot_digi:  found more than 1 CMA for a pivot digit!";
        DISP_ERROR;
        return;
    }

    int CMA_eta = list.front()->id().eta_index();
    int CMA_phi = list.front()->id().phi_index();
    int CMA_PAD = list.front()->id().PAD_index();
    int CMA_Ixx = list.front()->id().Ixx_index();

    int global_code;
    int global_connector;
    int prev_strips_conn;
    int foll_strips_conn;
    cabling->give_global_address(str_code,global_code,global_connector,
				       prev_strips_conn,foll_strips_conn);

    // Fill all the vars in struct pivot_vars (defined in DetailedTW.h)
    temp_vars.vp_code    = str_code;
    temp_vars.vp_x       = digi->x();
    temp_vars.vp_y       = digi->y();
    temp_vars.vp_z       = digi->z();

    temp_vars.vp_CMA_eta = CMA_eta;
    temp_vars.vp_CMA_phi = CMA_phi;
    temp_vars.vp_PAD     = CMA_PAD;
    temp_vars.vp_Ixx     = CMA_Ixx;

    temp_vars.pstrip_global  = global_code;

    temp_vars.logic_sector_p = digi->decoding().logic_sector();
    temp_vars.strip_number_p = digi->decoding().strip_number();
    temp_vars.klayer_p       = digi->decoding().rpc_layer() + digi->decoding().lvl1_station()*2;

    // insert the temp_vars into the list
    m_list_pivot.push_back(temp_vars);
    
}


#ifdef LVL1_STANDALONE
void DetailedTW::load_lowPt_digi(const RPCdigit* digi,const CablingRPCBase* cabling)
#else
void DetailedTW::load_lowPt_digi(const RPCdigit* digi,const IRPCcablingSvc* cabling)
#endif
{
    DISP << "DetailedTW::load_lowPt_digi -> Low Pt digit n. " << m_list_lowPt.size() + 1;
    DISP_DEBUG;

    lowPt_vars temp_vars;

    // Get logic values from the digit
    unsigned long str_code         = digi->decoding().code();

    int global_code;
    int global_connector;
    int prev_strips_conn;
    int foll_strips_conn;
    cabling->give_global_address(str_code,global_code,global_connector,
				       prev_strips_conn,foll_strips_conn);

    // Fill all vars in struct conf_vars (defined in DetailedTW.h)
    temp_vars.cp_code = str_code;
    temp_vars.cp_x    = digi->x();
    temp_vars.cp_y    = digi->y();
    temp_vars.cp_z    = digi->z();
    temp_vars.cstrip_global  = global_code; //local strip number instead of global!
    temp_vars.logic_sector_c = digi->decoding().logic_sector();
    temp_vars.strip_number_c = digi->decoding().strip_number();
    temp_vars.klayer_c = digi->decoding().rpc_layer() + digi->decoding().lvl1_station()*2;

    // insert the temp_vars into the list
    m_list_lowPt.push_back(temp_vars);
}

#ifdef LVL1_STANDALONE
void DetailedTW::load_highPt_digi(const RPCdigit* digi,const CablingRPCBase* cabling)
#else
void DetailedTW::load_highPt_digi(const RPCdigit* digi,const IRPCcablingSvc* cabling)
#endif
{
    DISP << "DetailedTW::load_highPt_digi -> High Pt digit n. " << m_list_highPt.size() + 1;
    DISP_DEBUG;

    highPt_vars temp_vars;

    // Get logic values from the digit
    unsigned long str_code         = digi->decoding().code();

    int global_code;
    int global_connector;
    int prev_strips_conn;
    int foll_strips_conn;
    cabling->give_global_address(str_code,global_code,global_connector,
				       prev_strips_conn,foll_strips_conn);

    // Fill all vars in struct conf_vars (defined in DetailedTW.h)
    temp_vars.hp_x =    digi->x();
    temp_vars.hp_y =    digi->y();
    temp_vars.hp_z =    digi->z();
    temp_vars.hstrip_global  = global_code; //local strip number instead of global!
    temp_vars.logic_sector_h = digi->decoding().logic_sector();
    temp_vars.strip_number_h = digi->decoding().strip_number();
    temp_vars.klayer_h = digi->decoding().rpc_layer() + digi->decoding().lvl1_station()*2;

    // insert the temp_vars into the list
    m_list_highPt.push_back(temp_vars);
}

void DetailedTW::make_ntupla(int event,float pt,float sign,int /*id_ntp*/,int maj_low, int maj_high){

    cout<<" Enter Mk_ntupla "<<endl;

    // fill vars in struct global_vars (defined in DetailedTW.h)
    m_global_v.event = event;
    m_global_v.pt    = pt;
    m_global_v.sign  = sign;
    m_global_v.check_conf = 99999;
    m_global_v.check_high = 99999;
    m_global_v.majority_low  = maj_low;
    m_global_v.majority_high = maj_high;

    //HCDIR_W("//PAWC/T1MB"," ");

    // int numb_var_global = 7;
    // int numb_var_pivot  = 12;
    // int numb_var_conf   = 8;
    // int numb_var_high   = 7;
    const int numb_var_tot    = 34;

    float vars[numb_var_tot];

    for (int ind=0;ind<numb_var_tot;ind++)
    {
        vars[ind]=99999.; // reset vars
    }

    vars[0]  = m_global_v.event;
    vars[1]  = m_global_v.pt;
    vars[2]  = m_global_v.sign;
    vars[32] = m_global_v.majority_low;
    vars[33] = m_global_v.majority_high;

    list_pivot_vars::const_iterator pv = m_list_pivot.begin();
    while (pv != m_list_pivot.end() )
    {
        DISP << " For this event there are " << m_list_pivot.size() 
	     << " digit into the pivot plane.";
        DISP_DEBUG;


	vars[3]  = (*pv).vp_code;
	vars[4]  = (*pv).vp_x;
	vars[5]  = (*pv).vp_y;
	vars[6]  = (*pv).vp_z;
	vars[7]  = (*pv).vp_CMA_eta;
	vars[8]  = (*pv).vp_CMA_phi;
	vars[9]  = (*pv).vp_PAD;
	vars[10] = (*pv).vp_Ixx;
        vars[11] = (*pv).pstrip_global;
        vars[12] = (*pv).logic_sector_p;
	vars[13] = (*pv).strip_number_p;
	vars[14] = (*pv).klayer_p;

	if (m_list_lowPt.size() == 0)           // ZERO DIGI IN LOW PT PLANE
        {
	    m_global_v.check_conf = 0;
	    vars[15]= m_global_v.check_conf;      // check_conf=0

	    if (m_list_highPt.size() == 0)      // ZERO DIGI IN HIGH PT PLANE
            {
	        m_global_v.check_high = 0;
	        vars[24] = m_global_v.check_high; // check_high=0

	        DISP << "fill ntupla case YNN";
                DISP_DEBUG;

	        for (int ind=0;ind<numb_var_tot;ind++)
                {
	            DISP << "VAR # " << ind << " = " << vars[ind]; // reset vars
	        }

	        // HFN_W(id_ntp,vars);
	    }
            else
	    {   //                     AT LEAST 1 DIGIT IN HIGH PLANE!!!
                list_highPt_vars::const_iterator hg = m_list_highPt.begin();
                while (hg != m_list_highPt.end() )
                {
		    m_global_v.check_high = 1;

		    vars[24] = m_global_v.check_high; //check_high=1
		    vars[25] = (*hg).hp_x;
		    vars[26] = (*hg).hp_y;
		    vars[27] = (*hg).hp_z;
		    vars[28] = (*hg).hstrip_global;
		    vars[29] = (*hg).logic_sector_h;
		    vars[30] = (*hg).strip_number_h;
		    vars[31] = (*hg).klayer_h;

                    DISP << "fill ntupla case NYY";
                    DISP_DEBUG;

		    for (int ind=0;ind<numb_var_tot;ind++)
                    {
		        DISP << "VAR # " << ind << " = " << vars[ind];
                        DISP_DEBUG;
		    }
		    
                    hg++;

		    //HFN_W(id_ntp,vars);

	        }//end of  while (index_digi_high<lista_high
	    }///end of if (lista_high.count ! 

	}
	else
        {  // AT LEAST 1 DIGIT IN CONF PLANE !!!!
	    list_lowPt_vars::const_iterator lw = m_list_lowPt.begin();
            while (lw != m_list_lowPt.end() )
            {
	        m_global_v.check_conf = 1;

	        vars[15] = m_global_v.check_conf;            //check_conf=1
	        vars[16] = (*lw).cp_code;
	        vars[17] = (*lw).cp_x;
	        vars[18] = (*lw).cp_y;
	        vars[19] = (*lw).cp_z;
	        vars[20] = (*lw).cstrip_global;
	        vars[21] = (*lw).logic_sector_c;
	        vars[22] = (*lw).strip_number_c;
	        vars[23] = (*lw).klayer_c;

	        if (m_list_highPt.size() == 0)
                {           // ZERO DIGI IN HIGH PT PLANE
	            m_global_v.check_high = 0;

	            vars[24] = m_global_v.check_high;      // check_high=0

	            DISP << "fill ntupla case YYN";
		    DISP_DEBUG;

		    for (int ind=0;ind<numb_var_tot;ind++)
                    {
		        DISP << "VAR # " << ind << " = " << vars[ind];
			DISP_DEBUG; 
		    }

	            //HFN_W(id_ntp,vars);
	        }
                else
	        {   //                     AT LEAST 1 DIGIT IN HIGH PLANE!!!
                    list_highPt_vars::const_iterator hg = m_list_highPt.begin();
                    while (hg != m_list_highPt.end() )
                    {
		        m_global_v.check_high = 1;

		        vars[24] = m_global_v.check_high; //check_high=1
		        vars[25] = (*hg).hp_x;
		        vars[26] = (*hg).hp_y;
		        vars[27] = (*hg).hp_z;
		        vars[28] = (*hg).hstrip_global;
		        vars[29] = (*hg).logic_sector_h;
		        vars[30] = (*hg).strip_number_h;
		        vars[31] = (*hg).klayer_h;

                        DISP << "fill ntupla case YYY";
                        DISP_DEBUG;

		        for (int ind=0;ind<numb_var_tot;ind++)
                        {
		            DISP << "VAR # " << ind << " = " << vars[ind];
                            DISP_DEBUG;
		        }
			
                        hg++;

		        // HFN_W(id_ntp,vars);

		    }//end of  while (index_digi_high<lista_high
	        }///end of if (lista_high.count !

	        lw++;;

	    }//end of  while (index_digi_conf<lista_conf
	} //end of else (lista_conferma.count !

	pv++;

    } //end of if pivot_list

    DISP << "exit from make ntupla";
    DISP_ERROR;

}//end of  void DetailedTW::make_ntupla(int event)


