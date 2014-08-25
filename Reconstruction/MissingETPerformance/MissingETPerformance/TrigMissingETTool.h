/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMISSINGET_TOOL_H 
#define TRIGMISSINGET_TOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

class ITHistSvc;
class MsgStream;
class StoreGateSvc;

class MissingETData;

class LVL1_ROI;
class TrigMissingETContainer;

class TH1F;
class TH2F;
class TH1;
class TH2;

static const InterfaceID IID_TrigMissingETTool("TrigMissingETTool", 1, 0);

class TrigMissingETTool : public AthAlgTool {

public:

  TrigMissingETTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_TrigMissingETTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~TrigMissingETTool( );

  /** a handle on Store Gate for access to the Event Store */
  StoreGateSvc* m_storeGate;

 private:
  std::string m_folderName;

  void setDir(const std::string&);
  std::string& getDir();
  TH1* hist(const std::string&);
  TH2* hist2(const std::string&);
  StatusCode addHistogram(TH1 *);
  StatusCode addHistogram(TH2 *);
  
  StatusCode trigMissingETPlots(MissingETData *);

  StatusCode bookL1hists();
  StatusCode fillL1hists(const LVL1_ROI *);
  
  StatusCode bookL2hists();
  StatusCode fillL2hists(const TrigMissingETContainer*);
  
  StatusCode bookEFhists();
  StatusCode fillEFhists(const TrigMissingETContainer* , const std::string& );


  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  std::string m_curdir, m_name;

};

#endif // TRIGMISSINGET_TOOL_H 
