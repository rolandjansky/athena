/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MDTCOOLSTRSVC_H
#define MUONCONDSVC_MDTCOOLSTRSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"

#include "MuonCondInterface/MdtICoolStrSvc.h"
#include "GaudiKernel/Service.h"
namespace MuonCalib {
class MdtCoolStrSvc : public AthService, virtual MdtICoolStrSvc
{
  friend class SvcFactory<MdtCoolStrSvc>;   
  //  template <class TYPE> class SvcFactory;

 public:
  MdtCoolStrSvc(const std::string& name, ISvcLocator* svc);
  virtual ~MdtCoolStrSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  virtual const InterfaceID& type() const;

  virtual StatusCode getString(const std::string& folder, const int chan, 
			       std::string& data) const;

  virtual StatusCode getFile(const std::string& folder, const int chan,
			     const std::string& file="") const;

   virtual StatusCode putFileT0(const std::string& folder, const std::string& 
			     filename, const int chan, const int tech) const;

   virtual StatusCode putFileRT(const std::string& folder, const std::string&
                             filename, const int chan, const int tech) const;

   virtual StatusCode putFileAlignCorr(const std::string& folder, 
				       const std::string& filename, 
				       const int chan, 
				       const int tech) const;
   virtual StatusCode putAligFromFile(const std::string& folder, 
				       const std::string& filename, 
				       const int chan, 
				       const int tech) const;

   virtual StatusCode putFileTube(const std::string& folder, const std::string&
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
}
#endif // MUONCONDSVC_MDTCOOLSTRSVC_H
