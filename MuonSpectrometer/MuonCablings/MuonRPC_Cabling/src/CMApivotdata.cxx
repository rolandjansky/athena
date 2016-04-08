/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRPC_Cabling/CMApivotdata.h"

using namespace MuonRPC_Cabling;

CMApivotdata::CMApivotdata() : BaseObject(Logic,"CMA Data")
{
    m_type = -1;
    m_station = 0;
    reset_data();
}

CMApivotdata::CMApivotdata(DBline& data, int type,std::string layout) : 
    BaseObject(Logic,"CMA Data")
{
    m_type = type;
    m_station = 0;
    m_layout = layout;

    (++data)("{");
    do
    {
        if(get_data(data))
	{
            if (m_view == Eta)
	    {
	        EtaCMA cma(m_number,m_station,m_type,m_coverage,
		  	   m_eta_index,m_phi_index,m_PAD_index,m_Ixx_index,
			   m_pivot_station,m_lowPt_station,m_highPt_station,
			   m_start_ch,m_start_st,m_stop_ch,m_stop_st);
                etaCMA.push_back(cma);
	    }
            if (m_view == Phi)
	    {
	        if (m_coverage == EvenSectors )
		{
	            EvenPhiCMA cma(m_number,m_station,m_type,m_coverage,
		  	      m_eta_index,m_phi_index,m_PAD_index,m_Ixx_index,
			      m_pivot_station,m_lowPt_station,m_highPt_station,
			      m_start_ch,m_start_st,m_stop_ch,m_stop_st);
                    evenphiCMA.push_back(cma);
		}
		if(m_coverage == OddSectors )
		{
	            OddPhiCMA cma(m_number,m_station,m_type,m_coverage,
		  	      m_eta_index,m_phi_index,m_PAD_index,m_Ixx_index,
			      m_pivot_station,m_lowPt_station,m_highPt_station,
			      m_start_ch,m_start_st,m_stop_ch,m_stop_st);
                    oddphiCMA.push_back(cma);
		}
	    }
	}
        data++;
    }while(!data("}"));
}

CMApivotdata::~CMApivotdata()
{
    etaCMA.clear();
    evenphiCMA.clear();
    oddphiCMA.clear();
}

void
CMApivotdata::reset_data()
{
    m_view      = NoView;
    m_number    = -1;
    m_coverage  = AllSectors;
    m_covtag    = "";
    m_eta_index = -1;
    m_phi_index = -1;
    m_PAD_index = -1;
    m_Ixx_index = -1;
    m_start_ch  = -1;
    m_start_st  = -1;
    m_stop_ch   = -1;
    m_stop_st   = -1;
    m_pivot_station  = -1;
    m_lowPt_station  = -1;
    m_highPt_station = -1;

    m_fail = true;
}


bool
CMApivotdata::confirm_data(ViewType side)
{
    int start = m_start_ch*100 + m_start_st;
    int stop  = m_stop_ch*100 + m_stop_st;
    std::string view = (side == Phi)? "phi" : "eta";

    __osstream disp;

    if(!CMAidentity::coverage(m_covtag,m_coverage))
    {
        disp << "CMA cabling error into configuration for Sector Type "
             << m_type << ", " <<m_covtag << " " << view << " CMA number "
             << m_number << std::endl
             << " coverage tag ==> " << m_covtag << " <== is not recognized!"
             << std::endl;
        display_error(disp);
        return false;
    }

    if( (side == Eta) & start )
    {
        if(start >= stop)
        {
            disp << "CMA cabling error into configuration for Sector Type "
                 << m_type << ", " << view << " CMA number " << m_number
                 << std::endl
                 << " start position (" << m_start_ch << ":" << m_start_st
                 << ") is greater than stop position (" << m_stop_ch << ":"
                 << m_stop_st << ")" << std::endl;
            display_error(disp);
            return false;
        }
    }
    if(side == Phi && start)
    {
        bool oldPhiSchema = (m_layout=="P03" || m_layout=="H8")? true : false;
        if(start >= stop && m_coverage == EvenSectors && !oldPhiSchema)
        {
            disp << "CMA cabling error into configuration for Sector Type "
             << m_type << ", " << m_covtag << " " << view 
	     << " CMA number " << m_number << std::endl
             << " start position (" << m_start_ch << ":" << m_start_st
             << ") is greater than stop position (" << m_stop_ch << ":"
             << m_stop_st << ")" << std::endl;
            display_error(disp);
            return false;
        }
	if(start <= stop && m_coverage == EvenSectors && oldPhiSchema)
        {
            disp << "CMA cabling error into configuration for Sector Type "
             << m_type << ", " << m_covtag << " " << view
             << " CMA number " << m_number << std::endl
             << " start position (" << m_start_ch << ":" << m_start_st
             << ") is lower than stop position (" << m_stop_ch << ":"
             << m_stop_st << ")" << std::endl;
            display_error(disp);
            return false;
        }
        if(start <= stop && m_coverage == OddSectors)
        {
            disp << "CMA cabling error into configuration for Sector Type "
             << m_type << ", " << m_covtag << " " << view 
	     << " CMA number " << m_number << std::endl
             << " start position (" << m_start_ch << ":" << m_start_st
             << ") is lower than stop position (" << m_stop_ch << ":"
             << m_stop_st << ")" << std::endl;
            display_error(disp);
            return false;
        }
        if(m_start_ch != m_stop_ch)
        {
            disp << "CMA cabling error into configuration for Sector Type "
                 << m_type << ", " << m_covtag << " " << view
	         << " CMA number " << m_number << std::endl
                 << " phi CMA Pivot connected to more than 1 wired or" << std::endl;
            display_error(disp);
            return false;
        }
    }
    return true;
}


bool
CMApivotdata::get_data(DBline& data)
{
    reset_data();

    if(data("eta matrix") >> m_number >> ":" >>
      "eta" >> m_eta_index >> "phi" >> m_phi_index >>
      "PAD" >> m_PAD_index >> "Idx" >> m_Ixx_index >>
       m_pivot_station >> m_lowPt_station >> m_highPt_station >>
      m_start_ch >> ":" >> m_start_st >> "-" >> m_stop_ch >> ":" >> m_stop_st )
    {
        //if( !confirm_data(Eta)) return false;
	m_view = Eta;
        m_fail = false;
        if( !confirm_data(Eta)) m_fail = true; 
    }

    if(data("phi matrix") >> m_covtag >> m_number >> ":" >>
      "eta" >> m_eta_index >> "phi" >> m_phi_index >>
      "PAD" >> m_PAD_index >> "Idx" >> m_Ixx_index >>
       m_pivot_station >> m_lowPt_station >> m_highPt_station >>
      m_start_ch >> ":" >> m_start_st >> "-" >> m_stop_ch >> ":" >> m_stop_st )
    {
        //if( !confirm_data(Phi)) return false;
	m_view = Phi;
        m_fail = false;
        if( !confirm_data(Phi)) m_fail = true;
    }

    return !m_fail;
}

EtaCMA*
CMApivotdata::give_eta_cma()
{    
    if(etaCMA.size())
    {
        EtaCMA* CMA = new EtaCMA(etaCMA.front());
	etaCMA.pop_front();
	return CMA;
    }
    return 0;
}

EvenPhiCMA*
CMApivotdata::give_evenphi_cma()
{    
    if(evenphiCMA.size())
    {
        EvenPhiCMA* CMA = new EvenPhiCMA(evenphiCMA.front());
	evenphiCMA.pop_front();
	return CMA;
    }
    return 0;
}

OddPhiCMA*
CMApivotdata::give_oddphi_cma()
{    
    if(oddphiCMA.size())
    {
        OddPhiCMA* CMA = new OddPhiCMA(oddphiCMA.front());
	oddphiCMA.pop_front();
	return CMA;
    }
    return 0;
}

void
CMApivotdata::Print(std::ostream& stream, bool detail) const
{
    stream << "CMA pivot segmentation";
    stream << " belonging to sector type " << m_type << std::endl;

    stream << "It contains " << etaCMA.size();
    stream << " eta CMAs:" << std::endl;
    ETAlist::const_iterator ei;
    for(ei = etaCMA.begin();ei!=etaCMA.end();++ei) 
        stream << ShowRequest<EtaCMA>(*ei,detail);

    stream << "It contains " << evenphiCMA.size();
    stream << "even phi CMAs:" << std::endl;
    EvenPHIlist::const_iterator ev;
    for(ev=evenphiCMA.begin();ev!=evenphiCMA.end();++ev) 
        stream << ShowRequest<EvenPhiCMA>(*ev,detail);
 
    stream << "It contains " << oddphiCMA.size();
    stream << "odd phi CMAs:" << std::endl;
    OddPHIlist::const_iterator od;
    for(od=oddphiCMA.begin();od!=oddphiCMA.end();++od) 
        stream << ShowRequest<OddPhiCMA>(*od,detail);

}
