/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADFEBMASKER_H
#define LARBADFEBMASKER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/HWIdentifier.h" 
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRecConditions/ILArBadFebMasker.h"

//class StoreGateSvc;

#include "StoreGate/ReadCondHandleKey.h"

class LArBadFebMasker : virtual public ILArBadFebMasker, public AthAlgTool
{
public:

   LArBadFebMasker(const std::string& type, const std::string& name, const IInterface* parent);
   virtual ~LArBadFebMasker();

   virtual StatusCode initialize() override;

   virtual bool febMissing( const HWIdentifier& febId) const override final;
   virtual bool shouldbeMasked(const  HWIdentifier& febId) const override final;
   virtual LArBadFeb febStatus(const  HWIdentifier& febId) const override final;

   virtual bool isMaskingOn() const override final//allow the client code to check in order to optimize
      {return m_doMasking;}

   StatusCode queryInterface(const InterfaceID& riid, void** ppvIf);
   static const InterfaceID& interfaceID();

private:

   typedef LArBadFebEnum::BitWord BitWord;

   LArBadFebMasker();   

   void buildBitMask();
   static const std::vector<std::string>& defaultProblems();

   SG::ReadCondHandleKey<LArBadFebCont> m_bfContKey;
   BitWord                      m_bitMask;           // The list of problems from m_problemWords, in bit form.
   std::vector<std::string>     m_problemWords;      // The list of FEB problems to be masked.
   bool                         m_doMasking;         // Determines whether to do masking.

};

#endif
