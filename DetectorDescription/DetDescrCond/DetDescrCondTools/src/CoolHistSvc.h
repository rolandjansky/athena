/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTOOLS_COOLHISTSVC_H
#define DETDESCRCONDTOOLS_COOLHISTSVC_H
// CoolHistSvc.h - service to manage reference histograms under COOL control
// Richard Hawkings, started 22/1/07

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"
#include "DetDescrCondTools/ICoolHistSvc.h"

class StoreGateSvc;
class IPoolSvc;
class TFile;
class TDirectory;

class CoolHistSvc : public virtual ICoolHistSvc, 
		    public virtual IIncidentListener,
		    public virtual AthService {
  template <class TYPE> class SvcFactory;

 public:
  CoolHistSvc(const std::string& name, ISvcLocator* svc);
  virtual ~CoolHistSvc();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  virtual const InterfaceID& type() const;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual void handle(const Incident& inc);

  // methods to get references to 1,2,3D histograms under COOL control
  virtual StatusCode getHist(const std::string& folder, 
			     const unsigned int channel,
			     const std::string& histname,TH1*& hist);
  virtual StatusCode getHist(const std::string& folder, 
			     const unsigned int channel,
			     const std::string& histname,TH2*& hist);
  virtual StatusCode getHist(const std::string& folder, 
			     const unsigned int channel,
			     const std::string& histname,TH3*& hist);
  virtual StatusCode getTObject(const std::string& folder, 
			        const unsigned int channel,
			        const std::string& histname,TObject*& hist);
  virtual bool objectExists(const std::string& folder, 
			    const unsigned int channel,
			    const std::string& histname);

  virtual StatusCode getHist(const std::string& folder, 
			     const std::string& channel,
			     const std::string& histname,TH1*& hist);
  virtual StatusCode getHist(const std::string& folder, 
			     const std::string& channel,
			     const std::string& histname,TH2*& hist);
  virtual StatusCode getHist(const std::string& folder, 
			     const std::string& channel,
			     const std::string& histname,TH3*& hist);
  virtual StatusCode getTObject(const std::string& folder, 
			        const std::string& channel,
			        const std::string& histname,TObject*& hist);
  virtual bool objectExists(const std::string& folder, 
			    const std::string& channel,
			    const std::string& histname);

 private:
  StatusCode getHist_i(const std::string& folder, const unsigned int channel,
		       const std::string& channelName,
		       const std::string& histname,TObject*& hist,
		       const bool printerr=true);
  TFile* getFile(const std::string& guid);
  std::string getHistKey(const std::string& folder, const int channel,
			 const std::string& histname);
  void closeFiles();

  unsigned int m_par_maxfiles; // maximum number of files to have open
  StoreGateSvc* p_detstore;
  IPoolSvc* m_poolsvc;

  // list of cached histograms and associated file GUIDs
  typedef std::map<std::string,std::pair<std::string, TObject*> > HistKeyMap;
  HistKeyMap m_histkeymap;
  // list of open files with GUID
  typedef std::map<std::string, TFile*> FileMap;
  FileMap m_filemap;

  // directory restore copied from THistSvc
  class GlobalDirectoryRestore {
  public:
    GlobalDirectoryRestore();
    ~GlobalDirectoryRestore();
  private:
    TDirectory* m_gd;
    TFile* m_gf;
    int m_ge;
  };
};

#endif // DETDESCRCONDTOOLS_COOLHISTSVC_H
