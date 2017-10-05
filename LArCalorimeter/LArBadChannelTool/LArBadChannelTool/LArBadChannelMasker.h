/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelMasker_H
#define LArBadChannelMasker_H

/* ----------------------------------------------------

Tool Name:     LArBadChannelMasker
Author:        Ryan Taylor

Description:   

   The LArBadChannelMasker tool is a very simple and 
   generic interface to the LArBadChanTool, and is used 
   to check whether cells have any of the problems specified
   by the user in the 'ProblemsToMask' property. 

---------------------------------------------------- */

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
   //#include "CaloIdentifier/CaloGain.h"                   //included by ILArBadChannelMasker.h
#include "LArRecConditions/ILArBadChanTool.h"
   //#include "Identifier/HWIdentifier.h"                   //included by ILArBadChanTool.h
   //#include "LArRecConditions/LArBadChannel.h"            //included by ILArBadChanTool.h
      //#include "LArRecConditions/LArBadChannelEnum.h"        //included by LArBadChannel.h
      //#include "LArRecConditions/LArBadChanBitPacking.h"     //included by LArBadChannel.h

//class StoreGateSvc;

class LArBadChannelMasker : virtual public ILArBadChannelMasker, public AthAlgTool
{
public:

   LArBadChannelMasker(const std::string& type, const std::string& name, const IInterface* parent);
   virtual ~LArBadChannelMasker();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   virtual bool cellShouldBeMasked(const HWIdentifier& hardwareId, const int gain=CaloGain::UNKNOWNGAIN) const;
   virtual bool cellShouldBeMasked(const Identifier& offlineId, const int gain=CaloGain::UNKNOWNGAIN) const;
   virtual bool cellShouldBeMaskedFEB(const HWIdentifier& FEBid, const int channelNumber, const int gain=CaloGain::UNKNOWNGAIN) const;

   virtual bool isMaskingOn() const //allow the client code to check in order to optimize
      {return m_doMasking;}

   StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);
   static const InterfaceID& interfaceID();

private:

   typedef LArBadChannelEnum::BitWord BitWord;

   LArBadChannelMasker();   
   LArBadChannelMasker(const LArBadChannelMasker&); //needs addRef() if implemented?

   void buildBitMask();
   static const std::vector<std::string>& defaultProblems();

   bool statusShouldBeMasked(const LArBadChannel& cellStatus, const int gain=CaloGain::UNKNOWNGAIN) const;

   //virtual StatusCode testCallBack(IOVSVC_CALLBACK_ARGS);
   //StoreGateSvc*                m_detStore;

   ToolHandle<ILArBadChanTool>  m_badChanToolHandle; // The tool for accessing bad-channel information.
   ILArBadChanTool*             m_badChanTool;       // Cache a pointer to save significant time.

   static LArBadChanBitPacking  s_bitPacking;        // A helper for bit operations, etc.
   std::vector<std::string>     m_problemWords;      // The list of bad-channel problems to be masked.
   BitWord                      m_bitMask;           // The list of problems from m_problemWords, in bit form.
   bool                         m_doMasking;         // Determines whether to do masking.

   // Bitmasks for determining the gain-dependence of problems.
   const BitWord                m_highGainMask;
   const BitWord                m_mediumGainMask;
   const BitWord                m_lowGainMask;
};

inline bool LArBadChannelMasker::statusShouldBeMasked(const LArBadChannel& cellStatus, const int gain) const
{ 
   // The majority of cells are good, so this saves a little bit of time.
   if(cellStatus.good())
      return false;

   ATH_MSG_VERBOSE( "gain: " << gain << " bitMask: 0x" << MSG::hex << m_bitMask 
      << " cellStatus: 0x" << cellStatus.packedData() << MSG::dec ); 

   if (gain==CaloGain::LARHIGHGAIN) 
      return (m_bitMask & cellStatus.packedData() & m_highGainMask) != 0;
   else if (gain==CaloGain::LARMEDIUMGAIN) 
      return (m_bitMask & cellStatus.packedData() & m_mediumGainMask) != 0;
   else if (gain==CaloGain::LARLOWGAIN) 
      return (m_bitMask & cellStatus.packedData() & m_lowGainMask) != 0;
   else 
      return (m_bitMask & cellStatus.packedData()) != 0;
}


#endif
