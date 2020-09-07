/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXtauTOB.cxx  -  
//                              -------------------
//     begin                : 17 01 2020
//     email                : nicholas.andrew.luongo@cern.ch
//  **************************************************************************


#include "L1CaloFEXSim/eFEXtauTOB.h"


LVL1::eFEXtauTOB::eFEXtauTOB():
	m_eta{99999},
	m_phi{99999},
	m_et{99999},
	m_reta_num{99999},
	m_reta_den{99999},
	m_rhad_num{99999},
	m_rhad_den{99999},
	m_wstot_num{99999},
	m_wstot_den{99999},
	m_fpga_id{99999},
	m_efex_id{99999},
	m_seed_und{1},
	m_seed{99999}
{}

unsigned int LVL1::eFEXtauTOB::setEta(unsigned int eta)
{
	m_eta = eta;
	return m_eta;
}

unsigned int LVL1::eFEXtauTOB::setPhi(unsigned int phi)
{
	m_phi = phi;
	return m_phi;
}

unsigned int LVL1::eFEXtauTOB::setEt(unsigned int et)
{
	m_et = et;
	return m_et;
}

unsigned int LVL1::eFEXtauTOB::setBitwiseEt(unsigned int bitwise_et)
{
	m_bitwise_et = bitwise_et;
	return m_bitwise_et;
}

float LVL1::eFEXtauTOB::setIso(unsigned int iso)
{
	m_iso = iso;
	return m_iso;
}

unsigned int LVL1::eFEXtauTOB::setFPGAID(unsigned int fpgaid)
{
	m_fpga_id = fpgaid;
	return m_fpga_id; 
}

unsigned int LVL1::eFEXtauTOB::setEFEXID(unsigned int efexid)
{
	m_efex_id = efexid;
	return m_efex_id; 
}

bool LVL1::eFEXtauTOB::setSeedUnD(bool seedund)
{
	m_seed_und = seedund;
	return m_seed_und;
}

unsigned int LVL1::eFEXtauTOB::setSeed(unsigned int seed)
{
	m_seed = seed;
	return m_seed;
}

unsigned int LVL1::eFEXtauTOB::setRetaNum(unsigned int retanum)
{
	m_reta_num = retanum;
	return m_reta_num;
}

unsigned int LVL1::eFEXtauTOB::setRetaDen(unsigned int retaden)
{
	m_reta_den = retaden;
	return m_reta_den;
}

unsigned int LVL1::eFEXtauTOB::setRhadNum(unsigned int rhadnum)
{
	m_rhad_num = rhadnum;
	return m_rhad_num;
}

unsigned int LVL1::eFEXtauTOB::setRhadDen(unsigned int rhadden)
{
	m_rhad_den = rhadden;
	return m_rhad_den;
}

unsigned int LVL1::eFEXtauTOB::setWstotNum(unsigned int wstot_num)
{
	m_wstot_num = wstot_num;
	return wstot_num;
}

unsigned int LVL1::eFEXtauTOB::setWstotDen(unsigned int wstot_den)
{
	m_wstot_den = wstot_den;
	return wstot_den;
}
