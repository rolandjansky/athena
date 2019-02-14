/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOOLBASE_DITAU_H
 #define TOOLBASE_DITAU_H
 
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
         //! Cleanup - called for each Ditau rejected candidate
         //-----------------------------------------------------------------
         virtual void cleanup( DiTauCandidateData *data );
 
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
 
 #endif // TOOLBASE_DITAU_H
