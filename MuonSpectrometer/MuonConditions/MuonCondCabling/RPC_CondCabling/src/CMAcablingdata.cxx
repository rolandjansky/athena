/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CMAcablingdata.h"

using namespace RPC_CondCabling;

CMAcablingdata::CMAcablingdata() : 
    BaseObject(Logic,"CMA Cabling Data")
{
    m_type = -1;
    m_station = 0;
    m_number  = -1;
    m_view = Eta;
    m_coverage = AllSectors;
    m_covtag = "";
    reset_data();
}

CMAcablingdata::CMAcablingdata(DBline& data, int type) :
    BaseObject(Logic,"CMA Cabling Data") 
{
    m_type = type;
    m_station = 0;
    m_number = -1;
    m_coverage = AllSectors;

    (++data)("{");
    do
    {
        if(get_data(data))
	{
            if (m_view == Eta)
	    {
	        EtaCMA cma(m_number,m_station,m_type,m_eta_index,m_phi_index,
                        m_lowPt_start_co,m_lowPt_stop_co,m_lowPt_number_co,
			m_highPt_start_co,m_highPt_stop_co,m_highPt_number_co);
                m_etaCMA.push_back(cma);
	    }
	}
        data++;
    }while(!data("}"));
}

CMAcablingdata::~CMAcablingdata()
{
    m_etaCMA.clear();
}

void
CMAcablingdata::reset_data()
{
    m_eta_index = -1;
    m_phi_index = -1;
    
    m_lowPt_start_co  = -1;
    m_lowPt_stop_co   = -1;
    m_lowPt_number_co = -1;

    m_highPt_start_co  = -1;
    m_highPt_stop_co   = -1;
    m_highPt_number_co = -1;

    m_fail = true;
}


bool
CMAcablingdata::confirm_data(ViewType side)
{
    unsigned int div = 247;
    std::string view = (side == Phi)? "phi" : "eta";

    __osstream disp;

    bool have_low_Pt_input  = (m_lowPt_number_co != -1)? true : false;
    bool have_high_Pt_input = (m_highPt_number_co != -1)? true : false;

    if(side == Eta)
    {
        if(m_lowPt_start_co >= m_lowPt_stop_co && have_low_Pt_input)
        {
            disp << "CMA cabling error in CONF data for Sector Type "
             << m_type << ", " << view << " CMA at eta = " << m_eta_index 
             << ",  phi = " << m_phi_index << std::endl
             << " start position for low Pt cabling (" << m_lowPt_start_co
             << ") is greater than stop position (" << m_lowPt_stop_co
             << std::endl;
	    display_error(disp);
            return false;
        }
        if(m_lowPt_stop_co - m_lowPt_start_co + 1 != m_lowPt_number_co &&
                                                     have_low_Pt_input)
        {
            disp << "CMA cabling error in CONF data for Sector Type "
             << m_type << ", " << view << " CMA at eta = " << m_eta_index 
             << ",  phi = " << m_phi_index << std::endl
             << " cabling inconsistence for low Pt <" << m_lowPt_number_co
             << ", " << m_lowPt_start_co << " " << (char)div << " "
	     << m_lowPt_stop_co << ">" << std::endl;
            display_error(disp);
            return false;
        }

        if(m_highPt_start_co >= m_highPt_stop_co && have_high_Pt_input)
        {
            disp << "CMA cabling error in CONF data for Sector Type "
             << m_type << ", " << view << " CMA at eta = " << m_eta_index 
             << ",  phi = " << m_phi_index << std::endl
             << " start position for high Pt cabling ("<<m_highPt_start_co
             << ") is greater than stop position (" << m_highPt_stop_co
             << std::endl;
            display_error(disp);
            return false;
        }
        if(m_highPt_stop_co - m_highPt_start_co + 1 != m_highPt_number_co &&
                                                       have_high_Pt_input)
        {
            disp << "CMA cabling error in CONF data for Sector Type "
                 << m_type << ", " << view << " CMA at eta = " << m_eta_index 
                 << ",  phi = " << m_phi_index << std::endl
                 << " cabling inconsistence for high Pt <"<<m_highPt_number_co
                 << ", " << m_highPt_start_co << " " << (char)div << " "
                 << m_highPt_stop_co << ">" << std::endl;
            display_error(disp);
            return false;
        }
    }

    return true;

}



bool
CMAcablingdata::get_data(DBline& data)
{
    reset_data();

    if(data("eta matrix") >> "eta" >> m_eta_index >> "phi" >> m_phi_index >> 
      ":" >> "low Pt" >> m_lowPt_number_co >> "," >> m_lowPt_start_co >> "-" >>
       m_lowPt_stop_co >> "high Pt" >> m_highPt_number_co >> "," >>
       m_highPt_start_co >> "-" >> m_highPt_stop_co)
    {
        if(m_lowPt_number_co == 0)
	{
	    m_lowPt_number_co = -1;
	    m_lowPt_start_co = -1;
	    m_lowPt_stop_co = -1;
	}
        if(m_highPt_number_co == 0)
	{
	    m_highPt_number_co = -1;
	    m_highPt_start_co = -1;
	    m_highPt_stop_co = -1;
	}
	m_view = Eta;
        m_fail = false;
        if( !confirm_data(Eta)) m_fail = true;
    }

    return !m_fail;
}

std::unique_ptr<EtaCMA>
CMAcablingdata::give_eta_cma()
{    
  if(m_etaCMA.size())
  {
    std::unique_ptr<EtaCMA> CMA( new EtaCMA(m_etaCMA.front()) );
    m_etaCMA.pop_front();
    return CMA;
  }
  return 0;
}

void
CMAcablingdata::Print(std::ostream& stream,bool detail) const
{
    stream << "CMA cabling data ";
    stream << " belonging to sector type " << m_type << std::endl;

    stream << "It contains " << m_etaCMA.size();
    stream << " eta CMAs:" << std::endl;
    ETAlist::const_iterator ei;
    for(ei = m_etaCMA.begin();ei!= m_etaCMA.end();++ei) 
        stream << ShowRequest<EtaCMA>(*ei,detail);
}
