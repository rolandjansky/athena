/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STREAMSELECTORTOOL_H
#define STREAMSELECTORTOOL_H

/** @file StreamSelectorTool.h
 *  @brief This file contains the class definition for the StreamSelectorTool class.
 *  $Id: StreamSelectorTool.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

/** @class StreamSelectorTool
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/
class StreamSelectorTool : public AthAlgTool, virtual public IAthenaSelectorTool {
public: // Constructor and Destructor
   /// Standard Service Constructor
   StreamSelectorTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~StreamSelectorTool();

public:
   /// IAthenaSelectorTool Interface method implementations:
   virtual StatusCode initialize();
   virtual StatusCode postInitialize();
   virtual StatusCode preNext() const;
   virtual StatusCode postNext() const;
   virtual StatusCode preFinalize();
   virtual StatusCode finalize();
private:
   StringProperty m_attrListKey{this,"AttributeListKey","Input","Key for attribute list input to be used"};
   //StringArrayProperty m_streamName{this,"SelectedStreams","ALL","Name of stream to be used as a ACCEPT, defaluts to ALL"};
   //StringArrayProperty m_unstreamName{this,"DeselectedStreams","ALL","Name of stream to be used as a VETO, defaluts to ALL"};
   std::vector<std::string> m_streamName;
   std::vector<std::string> m_unstreamName;

   ServiceHandle<ICutFlowSvc> m_cutflow;
   CutIdentifier m_cutid;
};

inline StatusCode StreamSelectorTool::initialize() {return StatusCode::SUCCESS;}
inline StatusCode StreamSelectorTool::preFinalize() {return StatusCode::SUCCESS;}
inline StatusCode StreamSelectorTool::finalize() {return StatusCode::SUCCESS;}

#endif
