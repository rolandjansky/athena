/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTOOLS_COOLSTRFILESVC_H
#define DETDESCRCONDTOOLS_COOLSTRFILESVC_H
// CoolStrFileSvc.h, concrete implementation of CoolStrFileSvc
// Richard Hawkings, started 14/11/05

#include "DetDescrCondTools/ICoolStrFileSvc.h"
#include "AthenaBaseComps/AthService.h"

#include "StoreGate/StoreGateSvc.h"
class CoolStrFileSvc : public virtual ICoolStrFileSvc, public virtual AthService
{
  template <class TYPE> class SvcFactory;

 public:
  CoolStrFileSvc(const std::string& name, ISvcLocator* svc);
  virtual ~CoolStrFileSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  virtual const InterfaceID& type() const;

  virtual StatusCode getString(const std::string& folder, const int chan, 
			       std::string& data) const;

  virtual StatusCode getFile(const std::string& folder, const int chan,
			     const std::string& file="") const;

  virtual StatusCode putFile(const std::string& folder, const std::string& 
			     filename, const int chan, const int tech) const;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

 private:
  StoreGateSvc* p_detstore;

  StatusCode putData(const std::string& folder, 
        const std::string& filename,const int chan,const int tech, 
		     const std::string& data) const;

  StatusCode getData(const std::string& folder, 
		   const int chan, std::string& file,std::string& data) const;
};

#endif // DETDESCRCONDTOOLS_COOLSTRFILESVC_H
