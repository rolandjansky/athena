/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENEVENTSSELECTORTOOL_H
#define EVENEVENTSSELECTORTOOL_H

/** @file EvenEventsSelectorTool.h
 *  @brief This file contains the class definition for the EvenEventsSelectorTool class.
 *  $Id: EvenEventsSelectorTool.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

class StoreGateSvc;

/** @class EvenEventsSelectorTool
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/
class EvenEventsSelectorTool : public AthAlgTool, virtual public IAthenaSelectorTool {
public: // Constructor and Destructor
   /// Standard Service Constructor
   EvenEventsSelectorTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~EvenEventsSelectorTool();

public:
   /// IAthenaSelectorTool Interface method implementations:
   virtual StatusCode initialize();
   virtual StatusCode postInitialize();
   virtual StatusCode preNext();
   virtual StatusCode postNext();
   virtual StatusCode preFinalize();
   virtual StatusCode finalize();
};

inline StatusCode EvenEventsSelectorTool::initialize() {return StatusCode::SUCCESS;}
inline StatusCode EvenEventsSelectorTool::postInitialize() {return StatusCode::SUCCESS;}
inline StatusCode EvenEventsSelectorTool::preFinalize() {return StatusCode::SUCCESS;}
inline StatusCode EvenEventsSelectorTool::finalize() {return StatusCode::SUCCESS;}

#endif
