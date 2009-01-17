/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPCmonitoring/TrigEfficiency.h"
#include "TrigT1RPClogic/decodeSL.h"


TrigEfficiency::TrigEfficiency( NTuple::Tuple* ntup,unsigned int max_muon,
                                unsigned int max_roi) : m_ntup(ntup)
{
    m_ntup->addItem("MCTHRU/muon",m_nmuon,0,static_cast<int>(max_muon));
    m_ntup->addItem("MCTHRU/eta",m_nmuon,m_eta);   
    m_ntup->addItem("MCTHRU/phi",m_nmuon,m_phi);  
    m_ntup->addItem("MCTHRU/Px",m_nmuon,m_Px);
    m_ntup->addItem("MCTHRU/Py",m_nmuon,m_Py);
    m_ntup->addItem("MCTHRU/Pz",m_nmuon,m_Pz); 
    m_ntup->addItem("MCTHRU/Vx",m_nmuon,m_Vx);
    m_ntup->addItem("MCTHRU/Vy",m_nmuon,m_Vy);
    m_ntup->addItem("MCTHRU/Vz",m_nmuon,m_Vz);
     
    m_ntup->addItem("ROIS/roi",m_nroi,0,static_cast<int>(max_roi));
    m_ntup->addItem("ROIS/side",m_nroi,m_side);
    m_ntup->addItem("ROIS/sector",m_nroi,m_sector);
    m_ntup->addItem("ROIS/roiId",m_nroi,m_roi);
    m_ntup->addItem("ROIS/thre",m_nroi,m_th);

    m_words.clear();
    m_thru.clear();

    m_max_muon = max_muon;
    m_max_roi  = max_roi;
}

TrigEfficiency::~TrigEfficiency()
{
    m_words.clear();
    m_thru.clear();
}

TrigEfficiency::TrigEfficiency(const TrigEfficiency& efficiency) : 
  m_ntup(efficiency.ntuple()),m_words(efficiency.sl_words()),
  m_thru(efficiency.mcthru()),m_max_muon(efficiency.max_muon()),
  m_max_roi(efficiency.max_roi())
{
}

TrigEfficiency
TrigEfficiency::operator=(const TrigEfficiency& efficiency)
{
    m_words.clear();
    m_words = efficiency.sl_words();
    m_thru = efficiency.mcthru();
    m_ntup = efficiency.ntuple();
    m_max_muon = efficiency.max_muon();
    m_max_roi  = efficiency.max_roi();
    return *this;
}

void
TrigEfficiency::load_SLwords(TrigData data)
{
    if(m_words.size() <= m_max_roi-1) m_words.push_back(data);
}

void
TrigEfficiency::load_MCthru(MontecarloThru thru)
{
    if(m_thru.size() <= m_max_muon-1) m_thru.push_back(thru);
}

void
TrigEfficiency::operator<<(TrigData data)
{
    this->load_SLwords(data);
}

void
TrigEfficiency::operator<<(MontecarloThru thru)
{
    this->load_MCthru(thru);
}

void
TrigEfficiency::write_ntp(INTupleSvc* ntupleSvc)
{
    int count = 0;
    m_nmuon = m_thru.size();
    MCthru::const_iterator mc = m_thru.begin();
    while(mc != m_thru.end())
    {
        m_eta[count] = (*mc).eta;
        m_phi[count] = (*mc).phi;
        m_Px[count]  = (*mc).p_x;
        m_Py[count]  = (*mc).p_y;
        m_Pz[count]  = (*mc).p_z;
        m_Vx[count]  = (*mc).vertex_x;
        m_Vy[count]  = (*mc).vertex_y;
        m_Vz[count]  = (*mc).vertex_z;
 
        ++count;       
	++mc;
    }

    count = 0;
    m_nroi = m_words.size();
    SLwords::const_iterator it = m_words.begin();
    while (it != m_words.end())
    {
        long int subsystem     = ((*it).first > 31)? 1 : 0;
        long int logic_sector  = ((*it).first) %32;

        unsigned long int data_word = (*it).second;

        m_side[count]   = subsystem;
        m_sector[count] = logic_sector;
        m_roi[count]    = TriggerRPC::ROI1(data_word);
        m_th[count]     = TriggerRPC::PT1(data_word);

        // cout << "Input to Efficiency ntup: side=" << subsystem
	//      << ", SL= " << logic_sector
	//      << ", RoI=" << TriggerRPC::ROI1(data_word)
	//      << ", Th=" << TriggerRPC::PT1(data_word)
	//      << ", data word " << hex << data_word
	//      << dec << endl;

        ++count;
	++it;
    }

    ntupleSvc->writeRecord("/NTUPLES/FILE1/RPCTRIG/9");
    m_words.clear();
    m_thru.clear();
}
