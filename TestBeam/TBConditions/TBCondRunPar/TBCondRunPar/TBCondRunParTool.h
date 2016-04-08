/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBCONDRUNPAR_TBCONDRUNPARTOOL_H
#define TBCONDRUNPAR_TBCONDRUNPARTOOL_H
// TBCondRunParTool.h
// an AlgTool to provide access to testbeam conditions RunParams information
// via GenericDbTable
// Richard Hawkings, started 2/7/04

// This AlgTool provides the following methods
// <fill me in>

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"

class EventInfo;

class TBCondRunParTool: public AthAlgTool {
 public:
  TBCondRunParTool(const std::string& type, const std::string& name,
		   const IInterface* parent);
  ~TBCondRunParTool();

  static const InterfaceID& interfaceID();
  StatusCode initialize();
  StatusCode finalize();

  // access functions
  int status();
  int run_number();
  int trigger_type();
  int det_mask();
  int beam_type();
  int beam_energy();
  std::string& file_tag();
  std::string& time_SOR();
  // utility functions
  StatusCode getTable(const std::string table,const int irow,
                const void* tblptr,
		int& ncol,int& nrow, std::vector<std::string>& names,
		std::vector<std::string>& rows) const;
  StatusCode extractVal(   
    const std::vector<std::string> names, const std::vector<std::string> rows,
    const std::string name,int& ival) const;
  StatusCode extractVal(   
    const std::vector<std::string> names, const std::vector<std::string> rows,
    const std::string name,float& fval) const;
  StatusCode extractVal(   
    const std::vector<std::string> names, const std::vector<std::string> rows,
    const std::string name,std::string& sval) const;
  void printTable(const std::vector<std::string> names, 
		  const std::vector<std::string> rows) const;
  StatusCode getVal(const std::string folder, const unsigned int chnum, float& fval);

 private:
  bool checkcache();
  bool extractCool(const int run);
  bool extractCoolTest(std::string folder, int run) const;

  const DataHandle<EventInfo> m_eventinfo;
  int m_crun;
  int m_cevent;
  std::string m_runpfolder;
  // cached data
  int m_status;
  int m_run_number;
  int m_trigger_type;
  int m_det_mask;
  int m_beam_type;
  int m_beam_energy;
  std::string m_file_tag;
  std::string m_time_SOR;
};

// inline access functions
inline int TBCondRunParTool::status()
      { checkcache(); return m_status;}
inline int TBCondRunParTool::run_number() 
      { checkcache(); return m_run_number;}
inline int TBCondRunParTool::trigger_type()
      { checkcache(); return m_trigger_type;}
inline int TBCondRunParTool::det_mask()
      { checkcache(); return m_det_mask;}
inline int TBCondRunParTool::beam_type() 
      { checkcache(); return m_beam_type;}
inline int TBCondRunParTool::beam_energy() 
      { checkcache(); return m_beam_energy;}
inline std::string& TBCondRunParTool::file_tag() 
      { checkcache(); return m_file_tag; }
inline std::string& TBCondRunParTool::time_SOR() 
      { checkcache(); return m_time_SOR; }

#endif // TBCONDRUNPAR_TBCONDRUNPARTOOL_H
