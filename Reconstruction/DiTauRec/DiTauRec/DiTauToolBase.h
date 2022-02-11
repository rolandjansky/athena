/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_DITAUTOOLBASE_H
#define DITAUREC_DITAUTOOLBASE_H
 
#include <string>
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "DiTauRec/DiTauCandidateData.h"
 
/**
 * @brief The base class for all tau tools.
 * 
 * @author Lukasz Janyst
 */
 
class DiTauToolBase: public AthAlgTool
{
 public:
  DiTauToolBase( const std::string &type,
                 const std::string &name,
                 const IInterface  *parent );
  virtual ~DiTauToolBase();
 
  //-----------------------------------------------------------------
  //! InterfaceID implementation needed for ToolHandle
  //-----------------------------------------------------------------
  static const InterfaceID& interfaceID();
 
  //-----------------------------------------------------------------
  //! Tool initializer
  //-----------------------------------------------------------------
  virtual StatusCode initialize();
 
  //-----------------------------------------------------------------
  //! Event initializer - called at the beginning of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventInitialize( DiTauCandidateData *data );
 
  //-----------------------------------------------------------------
  //! Execute - called for each Ditau candidate
  //-----------------------------------------------------------------
  virtual StatusCode execute( DiTauCandidateData *data,
			      const EventContext& ctx) const;
 
  //-----------------------------------------------------------------
  //! Finalizer
  //-----------------------------------------------------------------
  virtual StatusCode finalize();
 
  //-------------------------------------------------------------
  //! Convenience functions to handle storegate objects
  //-------------------------------------------------------------
  template <class T>
    bool openContainer(T* &container, std::string containerName, bool printFATAL=false);
 
  template <class T>
    bool retrieveTool(T &tool);
 
};
 
#endif // DITAUREC_DITAUTOOLBASE_H
