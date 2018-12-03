/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelMasker.h"
//#include "StoreGate/StoreGateSvc.h"

LArBadChanBitPacking LArBadChannelMasker::s_bitPacking;  //init static member

LArBadChannelMasker::LArBadChannelMasker(const std::string& type, 
      const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent), 
  m_badChanToolHandle("LArBadChanTool"), 
  m_badChanTool(0),
  m_problemWords(defaultProblems()),
  m_bitMask(0), 
  m_doMasking(false),
  m_highGainMask(s_bitPacking.highGainMask()), 
  m_mediumGainMask(s_bitPacking.mediumGainMask()), 
  m_lowGainMask(s_bitPacking.lowGainMask())
{
  declareInterface<ILArBadChannelMasker>(this);
  declareProperty("TheLArBadChanTool", m_badChanToolHandle, "Public, shared LArBadChanTool.");
  declareProperty("ProblemsToMask", m_problemWords, "List of channel problems to be masked.");
  declareProperty("DoMasking", m_doMasking, "Flag to turn cell masking on or off.");
}

LArBadChannelMasker::~LArBadChannelMasker()
{}

StatusCode LArBadChannelMasker::initialize() 
{
   ATH_MSG_DEBUG ( "in initialize()" );

   if(!m_doMasking)
   {
      ATH_MSG_INFO ( "Cell masking is OFF." );
      return StatusCode::SUCCESS;       // Do nothing.
   }

   buildBitMask();

   if(m_badChanToolHandle.retrieve().isFailure())
   {
      ATH_MSG_ERROR ( m_badChanToolHandle.propertyName() 
                      << ": Failed to retrieve tool " << m_badChanToolHandle );
      return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG ( "Successfully retrieved a " << m_badChanToolHandle.type() );

   // Please don't do this unless you know you really need to.
   m_badChanTool = &(*m_badChanToolHandle);   
   m_badChanTool->addRef();


   LArBadChannel tempBC(m_bitMask);    //consider overloading the function
   ATH_MSG_INFO ( "Cell masking is ON. The following problems will be masked: " 
                  << s_bitPacking.stringStatus(tempBC) );
   
   return StatusCode::SUCCESS;
}

StatusCode LArBadChannelMasker::finalize()
{
  if(m_badChanTool)
    m_badChanTool->release();
  m_badChanToolHandle.release().ignore(); //check but ignore the StatusCode
  return StatusCode::SUCCESS;
}

bool LArBadChannelMasker::cellShouldBeMasked(const Identifier& offlineId, const int gain) const
{
   return m_doMasking && statusShouldBeMasked(m_badChanTool->offlineStatus(offlineId),gain);
}

bool LArBadChannelMasker::cellShouldBeMasked(const HWIdentifier& hardwareId, const int gain) const
{
   return m_doMasking && statusShouldBeMasked(m_badChanTool->status(hardwareId),gain);
}

bool LArBadChannelMasker::cellShouldBeMaskedFEB(const HWIdentifier& FEBid, const int channelNumber, const int gain) const
{
   return m_doMasking && statusShouldBeMasked(m_badChanTool->status(FEBid, channelNumber), gain);
}

void LArBadChannelMasker::buildBitMask()
{
  m_bitMask = 0;

  for(std::vector<std::string>::const_iterator it = m_problemWords.begin(); 
    it != m_problemWords.end(); ++it)
  {
    std::pair<bool, LArBadChannelEnum::ProblemType> result = s_bitPacking.enumName(*it);
    if(result.first)
      s_bitPacking.setBit(result.second, m_bitMask);
    else
      ATH_MSG_WARNING ( "The problem flag '" << (*it) << "' was not recognized." );
  }
}

const std::vector<std::string>& LArBadChannelMasker::defaultProblems()
{
  static std::vector<std::string> defaults;
  if(defaults.size() == 0)
  {
    defaults.reserve(12);
    defaults.push_back("highNoiseLG");
    defaults.push_back("highNoiseMG");
    defaults.push_back("highNoiseHG");
    defaults.push_back("unstableNoiseLG");
    defaults.push_back("unstableNoiseMG");
    defaults.push_back("unstableNoiseHG");
    defaults.push_back("unstable");
    defaults.push_back("deadReadout");  // Dead channels can have strange 
    defaults.push_back("deadPhys");     // conditions and anomalous energies.
    defaults.push_back("problematicForUnknownReason");
    defaults.push_back("deadSCACell");
    defaults.push_back("badFirstSample");
//    defaults.push_back("distorted"); // Should be not too serious, calibration is okay.
//    defaults.push_back("deadCalib"); // Should be recoverable in DSPs?
  }
  return defaults;
}

/*
StatusCode LArBadChannelMasker::testCallBack(IOVSVC_CALLBACK_ARGS) 	 
{  //Could monitor DB updates, or implement a check of whether the database was read. 	 
   //Maybe use a one-shot incident listener instead?
   log << MSG::DEBUG << "In LArBadChannelMasker::testCallBack" << endmsg; 	 
   return StatusCode::SUCCESS; 	 
}*/

StatusCode LArBadChannelMasker::queryInterface(const InterfaceID& riid, void** ppvIf )
{
   if(riid == ILArBadChannelMasker::interfaceID())
   {
      *ppvIf = static_cast<ILArBadChannelMasker*>(this);
      addRef();
      return StatusCode::SUCCESS;
   }
   else if(riid == interfaceID())
   {
      *ppvIf = this;
      addRef();
      return StatusCode::SUCCESS;
   }
   else return AthAlgTool::queryInterface( riid, ppvIf );
}

const InterfaceID& LArBadChannelMasker::interfaceID() 
{
  static const InterfaceID id("LArBadChannelMasker", 1 , 0);
  return id; 
}

