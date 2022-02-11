/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadFebMasker.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArRecConditions/LArBadFebBitPacking.h"


LArBadFebMasker::LArBadFebMasker(const std::string& type, 
      const std::string& name, const IInterface* parent) :
  base_class(type, name, parent), 
  m_bfContKey("LArBadFeb"),
  m_bitMask(0), 
  m_problemWords(defaultProblems()),
  m_doMasking(false)
{
  declareProperty("BFKey",m_bfContKey,"Key of the BadFebContainer in the conditions store");
  declareProperty("ProblemsToMask", m_problemWords, "List of FEB problems to be masked.");
  declareProperty("DoMasking", m_doMasking, "Flag to turn FEB masking on or off.");
}

LArBadFebMasker::~LArBadFebMasker()
{}

StatusCode LArBadFebMasker::initialize() 
{
   ATH_MSG_DEBUG ( "in initialize()" );

   ATH_CHECK(m_bfContKey.initialize(m_doMasking));

   if(!m_doMasking) return StatusCode::SUCCESS; //Do nothing
     
   buildBitMask();

   LArBadFeb tempBF(m_bitMask);    //consider overloading the function
   LArBadFebBitPacking bf_bitpack;
   ATH_MSG_INFO ( "Cell masking is ON. The following problems will be masked: " 
                  << bf_bitpack.stringStatus(tempBF) );
   
   return StatusCode::SUCCESS;
}

bool LArBadFebMasker::febMissing(const HWIdentifier& febId) const{

   if(!m_doMasking) return false;

   SG::ReadCondHandle<LArBadFebCont> bfContHdl{m_bfContKey};
   const LArBadFebCont* bfCont{*bfContHdl};
   return  (0x1 << LArBadFebEnum::ProblemType::deadAllBit) & (bfCont->status(febId)).packedData();
}

bool LArBadFebMasker::shouldbeMasked(const HWIdentifier& febId) const{

   if(!m_doMasking) return false;

   SG::ReadCondHandle<LArBadFebCont> bfContHdl{m_bfContKey};
   const LArBadFebCont* bfCont{*bfContHdl};
   return  m_bitMask & (bfCont->status(febId)).packedData();
}

LArBadFeb LArBadFebMasker::febStatus(const HWIdentifier& febId) const{

   SG::ReadCondHandle<LArBadFebCont> bfContHdl{m_bfContKey};
   const LArBadFebCont* bfCont{*bfContHdl};
   return  bfCont->status(febId);
}

void LArBadFebMasker::buildBitMask()
{
  m_bitMask = 0;

  LArBadFebBitPacking bf_bitPacking;
  for(std::vector<std::string>::const_iterator it = m_problemWords.begin(); 
    it != m_problemWords.end(); ++it)
  {
    if(bf_bitPacking.setBit(*it,m_bitMask,true))
      ATH_MSG_DEBUG("The problem flag '"<< (*it) << "' was set.");
    else
      ATH_MSG_WARNING ( "The problem flag '" << (*it) << "' was not recognized." );
  }
}

const std::vector<std::string>& LArBadFebMasker::defaultProblems()
{
  static std::vector<std::string> defaults;
  if(defaults.size() == 0)
  {
    defaults.reserve(1);
    defaults.push_back("deadAllBit");
  }
  return defaults;
}

