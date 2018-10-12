/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBXMLWRITER_H
#define TBREC_TBXMLWRITER_H
///////////////////////////////////////////////////////////////////////////////
/// \brief XML writer algorithm for 2004 event display
///
/// The TBXMLWriter is an algorithm invoking writer tools and providing the
/// general frame for an event XML file.
///
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
class TBXMLWriterToolBase;

#include <string>
#include <vector>
#include <map>

class TBXMLWriter : public AthAlgorithm
{
 public:
  
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  /// \brief Algorithm constructor
  TBXMLWriter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBXMLWriter();

  ///////////////////////
  // Algorithm Methods //
  ///////////////////////

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  // tool support
  const std::string&  getFileDir() const { return m_topDirectory; }
  unsigned int getRunNumber()      const { return m_runNumber;    }
  unsigned int getEventNumber()    const { return m_eventNumber;  }
  bool handleBeginRun()            const { return m_beginRun;     }


 private: 

  ////////////////
  // Properties //
  ////////////////

  unsigned int m_outputFrequency;
  unsigned int m_eventCounter;

  std::vector<std::string> m_writerToolNames;
 
  std::string m_topDirectory;

  std::vector<TBXMLWriterToolBase*> m_writerTools;

  ////////////
  // Stores //
  ////////////

  std::map<std::string,unsigned int> m_toolAccept;
  std::map<std::string,unsigned int> m_toolReject;
  std::map<std::string,unsigned int> m_toolInvoke;

  bool         m_beginRun;
  unsigned int m_runNumber;
  unsigned int m_eventNumber;

  ///////////////
  // Functions //
  ///////////////

};
#endif
