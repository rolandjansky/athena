/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMCNVSVC_H
#define BYTESTREAMCNVSVC_BYTESTREAMCNVSVC_H

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include <map>

class ByteStreamOutputSvc;
class FullEventAssemblerBase;

/** @class ByteStreamCnvSvc
    @brief Gaudi COnversion Service class for ByteStream Persistency

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
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Implements ConversionSvc's connectOutput
   virtual StatusCode connectOutput(const std::string& t, const std::string& mode);
   virtual StatusCode connectOutput(const std::string& t);

   /// Implements ConversionSvc's commitOutput
   virtual StatusCode commitOutput(const std::string& outputConnection, bool b);

   /// @brief Access to FullEventAssembler
   template <class T> StatusCode getFullEventAssembler(T*&t, std::string nm);

protected:
   /// Write the FEA to RawEvent.
   void writeFEA();

private:
   /// name of the service
   std::string m_ioSvcName;

   /// list of service names
   Gaudi::Property<std::vector<std::string>> m_ioSvcNameList;

   /// Services for writing output
   std::map<std::string, ByteStreamOutputSvc*> m_ioSvcMap;

   /// flags for Simulation EventType
   Gaudi::Property<bool> m_isSimulation;
   /// flags for TestBeam EventType
   Gaudi::Property<bool> m_isTestbeam;
   /// flags for Calibration EventType
   Gaudi::Property<bool> m_isCalibration;
   /// flags for getting Detector Mask from COOL
   Gaudi::Property<bool> m_getDetectorMask;

   /// user type
   std::string m_userType;

   /// @brief common FEA, indexed by string key
   std::map<std::string, FullEventAssemblerBase*> m_feaMap;
};

// Implementation of template method:
template <class T> StatusCode ByteStreamCnvSvc::getFullEventAssembler(T*& t, std::string nm) {
   std::map<std::string, FullEventAssemblerBase*>::const_iterator it = m_feaMap.find(nm);
   if (it != m_feaMap.end()) {
      T* p = dynamic_cast<T*>((*it).second);
      if (p == 0) {
         ATH_MSG_WARNING(" Key = " << nm << " exists, but of different type");
         return(StatusCode::FAILURE);
      }
      t = p;
      return(StatusCode::SUCCESS);
   }

   // reach here if key does not exist
   t = new T();
   m_feaMap[nm] = t;
   return(StatusCode::SUCCESS);
}

#endif
