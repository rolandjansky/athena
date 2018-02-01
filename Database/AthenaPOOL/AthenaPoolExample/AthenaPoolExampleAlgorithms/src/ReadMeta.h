/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_READMETA_H
#define ATHENAPOOLEXAMPLEALGORITHMS_READMETA_H

/** @file ReadMeta.h
 *  @brief This file contains the class definition for the ReadMeta class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadMeta.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPoolKernel/IMetaDataTool.h"

class StoreGateSvc;

namespace AthPoolEx {

/** @class AthPoolEx::ReadMeta
 *  @brief This class provides an example for reading in file meta data objects from Pool.
 **/
class ReadMeta : public AthAlgTool, virtual public IMetaDataTool, virtual public IIncidentListener {
public: // Constructor and Destructor
   /// Standard Service Constructor
   ReadMeta(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~ReadMeta();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   /// Function called when a new input file is opened
   virtual StatusCode beginInputFile();

   /// Function called when the currently open input file got completely
   /// processed
   virtual StatusCode endInputFile() {return StatusCode::SUCCESS;}

   /// Function called when the tool should write out its metadata
   virtual StatusCode metaDataStop() {return StatusCode::SUCCESS;}

   /// Incident service handle listening for BeginInputFile and EndInputFile.
   void handle(const Incident& incident);

private:
   ServiceHandle<StoreGateSvc> m_pMetaDataStore;
   ServiceHandle<StoreGateSvc> m_pInputStore;
};

} // end AthPoolEx namespace

#endif
