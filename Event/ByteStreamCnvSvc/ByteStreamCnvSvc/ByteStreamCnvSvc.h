/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMCNVSVC_H
#define BYTESTREAMCNVSVC_BYTESTREAMCNVSVC_H

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/ServiceHandle.h"

#include <map>

class ByteStreamOutputSvc;
class FullEventAssemblerBase;

/** @class ByteStreamCnvSvc
    @brief Gaudi Conversion Service class for ByteStream Persistency

    This class is responsible for converting data object to and from BS format
    It inherits from ByteStreamCnvSvcBase, which is used by HLT in online.

    When reading the ByteStream data, the ByteStream converters are called, which in turn call
    IRODDataProviderSvc to retrieve the raw data.

    When writing the object data to ByteStream, an FullEventFragment is assembled
    from lower level fragments using FullEventAssembler, and writen out to BS in commitOutput
    method through ByteStreamOutputSvc.
*/

class ByteStreamCnvSvc : public ByteStreamCnvSvcBase/*, virtual public IService*/ {

public:
   /// Standard Constructor
   ByteStreamCnvSvc(const std::string& name, ISvcLocator* svc);

   /// Standard Destructor
   virtual ~ByteStreamCnvSvc();

   /// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   /// Implements ConversionSvc's connectOutput
   virtual StatusCode connectOutput(const std::string& t, const std::string& mode) override;
   virtual StatusCode connectOutput(const std::string& t) override;

   /// Implements ConversionSvc's commitOutput
   virtual StatusCode commitOutput(const std::string& outputConnection, bool b) override;

   /// Implementation of IByteStreamEventAccess: Get RawEvent
   virtual RawEventWrite* getRawEvent() override
   {
     return m_slots->m_rawEventWrite.get();
   }

  /// @brief Access to FullEventAssembler
   template <class T> StatusCode getFullEventAssembler(T*&t, const std::string& nm);

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
  //@}
  /// Should rather be in IByteStreamSvc.h if we had one
  static const InterfaceID& interfaceID();

protected:
   RawEventWrite* setRawEvent (std::unique_ptr<RawEventWrite> rawEventWrite);

private:
   /// name of the service
   std::string m_ioSvcName;

   /// list of service names
   Gaudi::Property<std::vector<std::string>> m_ioSvcNameList{ this, "ByteStreamOutputSvcList", {}, "", "OrderedSet<T>"};
   
   /// Services for writing output
   std::map<std::string, ByteStreamOutputSvc*> m_ioSvcMap;

   /// Event store.
   ServiceHandle<StoreGateSvc> m_evtStore;

   /// user type
   std::string m_userType;

   /// @brief common FEA, indexed by string key
   using FEAPtr_t = std::unique_ptr<FullEventAssemblerBase>;
   using FEAMap_t = std::map<std::string,  FEAPtr_t>;

   /// Slot-specific state.
   struct SlotData
   {
     std::unique_ptr<RawEventWrite> m_rawEventWrite;
     FEAMap_t m_feaMap;
     std::vector<uint32_t> m_tagBuff;
     std::vector<uint32_t> m_l1Buff;
     std::vector<uint32_t> m_l2Buff;
     std::vector<uint32_t> m_efBuff;

     void clear()
     {
       m_rawEventWrite.reset();
       m_feaMap.clear();
       m_tagBuff.clear();
       m_l1Buff.clear();
       m_l2Buff.clear();
       m_efBuff.clear();
     }
   };
   SG::SlotSpecificObj<SlotData> m_slots;

   /// Write the FEA to RawEvent.
   void writeFEA (SlotData& slot);
};

// Implementation of template method:
template <class T> StatusCode ByteStreamCnvSvc::getFullEventAssembler(T*& t, const std::string& nm)
{
   const EventContext& ctx = Gaudi::Hive::currentContext();
   FEAMap_t& feaMap = m_slots.get (ctx)->m_feaMap;
   FEAPtr_t& fea = feaMap[nm];
   if (fea) {
      T* p = dynamic_cast<T*>(fea.get());
      if (p == 0) {
         ATH_MSG_WARNING(" Key = " << nm << " exists, but of different type");
         return(StatusCode::FAILURE);
      }
      t = p;
      return(StatusCode::SUCCESS);
   }

   // reach here if key does not exist
   auto ptr = std::make_unique<T>();
   t = ptr.get();
   fea = std::move (ptr);
   return(StatusCode::SUCCESS);
}

#endif
