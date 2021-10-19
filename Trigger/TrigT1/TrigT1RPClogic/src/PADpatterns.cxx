/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1RPClogic/PADpatterns.h"


PADpatterns::PADpatterns(int sector, int pad,unsigned long int debug) : 
    RPCtrigDataObject(0,"PAD patterns"),m_sector(sector),m_pad_id(pad),
    m_debug(debug) 
{
}

PADpatterns::PADpatterns(const PADpatterns& PAD) : 
    RPCtrigDataObject(PAD.number(),PAD.name())
{
    m_sector = PAD.sector();
    m_pad_id = PAD.pad_id();
    m_debug  = PAD.debug();
    m_cma_patterns = PAD.cma_patterns();
}

PADpatterns
PADpatterns::operator=(const PADpatterns& PAD)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(PAD);
    m_sector = PAD.sector();
    m_pad_id = PAD.pad_id();
    m_debug  = PAD.debug();
    m_cma_patterns.clear();
    m_cma_patterns = PAD.cma_patterns();
    return *this;
}

bool
PADpatterns::operator==(const PADpatterns& patterns) const
{
    if(m_sector == patterns.sector())
    {
        if (m_pad_id == patterns.pad_id() ) return true;
    }
    return false;
}

bool
PADpatterns::operator!=(const PADpatterns& patterns) const
{
    return !(*this == patterns);
}


bool
PADpatterns::operator<(const PADpatterns& patterns) const
{
    if(m_sector < patterns.sector())
    {
     if (m_pad_id < patterns.pad_id() ) return true;
    }
    return false;
}

void
PADpatterns::load_cma_patterns(CMApatterns* patterns)
{
    m_cma_patterns.push_back(patterns);
}


PADpatterns& 
PADpatterns::operator<<(CMApatterns* patterns)
{
    this->load_cma_patterns(patterns);
    return *this;
}

Pad*
PADpatterns::give_pad(const RpcCablingCondData* readCdo)
{
  if (!m_pad)
  {
    int subsystem    = (m_sector<32)? 0 : 1;
    int logic_sector = m_sector%32;

    CMAdata::PatternsList::iterator cma = m_cma_patterns.begin();

    // M.Corradi 26/2/2010 check if old cabling from cma data !
    bool oldSimulation = false;
    if (cma!=m_cma_patterns.end()) {        
      if ( (*cma)->cma_parameters().conf_type() != CMAparameters::Atlas){
        oldSimulation = true;
      }
    }

    m_pad = std::make_unique<Pad>(0,0,m_debug,subsystem,logic_sector,m_pad_id,1,oldSimulation);

    //M.Corradi 8/1/2015 get Pad configuration Parameters 
    bool  eta_and_phi, feet_on;
    unsigned short int cma_mask, feet_th0, feet_th1, feet_th2 ;

    if (readCdo->give_Pad_Parameters((unsigned short int)m_sector,
          (unsigned short int)m_pad_id,
          feet_on,eta_and_phi,cma_mask,
          feet_th0,feet_th1,feet_th2)){
      m_pad->setFeetOn(feet_on);
      m_pad->setFeetThresholds(0,feet_th0);
      m_pad->setFeetThresholds(1,feet_th1);
      m_pad->setFeetThresholds(2,feet_th2);
      // m_pad->display(1);
      // MASK AND eta_and_phi to be implemented

    }else {
      std::runtime_error("Could not retrieve Pad Parameters m_sector, m_pad_id = " +
                         std::to_string(m_sector) + ", " + std::to_string(m_pad_id));
    }


    while(cma != m_cma_patterns.end())
    {
      Matrix*  low_pt_matrix = (*cma)->give_low_pt_matrix();
      Matrix* high_pt_matrix = (*cma)->give_high_pt_matrix();

      CMAidentity id  = (*cma)->cma_parameters().id();
      int matrix_view = (id.type() == Phi)? 1 : 0;
      int matrix_id   = id.Ixx_index();

      for (int bunch=0;bunch< NOBXS;++bunch)
      {
        m_pad->load(0,matrix_view,matrix_id,bunch,
            low_pt_matrix->getOutputThres(bunch),
            low_pt_matrix->getOutputOverl(bunch));

        m_pad->load(1,matrix_view,matrix_id,bunch,
            high_pt_matrix->getOutputThres(bunch),
            high_pt_matrix->getOutputOverl(bunch));
      } 
      ++cma;
    }

    m_pad->execute();
  }
  return m_pad.get();
}

void
PADpatterns::Print(std::ostream& stream,bool detail) const
{
    stream << name() << " (Id =  " << m_pad_id << ")  in sector logic " 
           << m_sector << std::endl;
    stream << "It contains n. " << m_cma_patterns.size() 
	   << " CMAs";

    if(detail)
    {
        stream << " :" << std::endl;
        CMAdata::PatternsList::const_iterator cma = m_cma_patterns.begin();
        while (cma != m_cma_patterns.end())
        {
	    (*cma)->Print(stream,false);
	    ++cma;
        }
    }
    else stream << "." << std::endl;
}

