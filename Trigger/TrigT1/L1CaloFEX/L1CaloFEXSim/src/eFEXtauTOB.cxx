/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	m_rcore_core{99999},
	m_rcore_env{99999},
	m_rhad_core{99999},
	m_rhad_env{99999},
	m_fpga_id{99999},
	m_efex_id{99999},
	m_seed_und{1},
	m_seed{99999},
	m_tobword{0}
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

unsigned int LVL1::eFEXtauTOB::seteFEXID(unsigned int efexid)
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

unsigned int LVL1::eFEXtauTOB::setRcoreCore(unsigned int rcorecore)
{
	m_rcore_core = rcorecore;
	return m_rcore_core;
}

unsigned int LVL1::eFEXtauTOB::setRcoreEnv(unsigned int rcoreenv)
{
	m_rcore_env = rcoreenv;
	return m_rcore_env;
}

unsigned int LVL1::eFEXtauTOB::setRhadCore(unsigned int rhadcore)
{
	m_rhad_core = rhadcore;
	return m_rhad_core;
}

unsigned int LVL1::eFEXtauTOB::setRhadEnv(unsigned int rhadenv)
{
	m_rhad_env = rhadenv;
	return m_rhad_env;
}

uint32_t LVL1::eFEXtauTOB::setTobword(uint32_t tobword) {
  m_tobword = tobword;
  return m_tobword;
}
