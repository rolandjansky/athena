/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOOLBASE_TAU_H
#define TOOLBASE_TAU_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "tauRec/TauCandidateData.h"

/**
 * @brief The base class for all tau tools.
 * 
 * @author Lukasz Janyst
 */

class TauToolBase: public AthAlgTool
{
    public:
        TauToolBase( const std::string &type,
                const std::string &name,
                const IInterface  *parent );
        virtual ~TauToolBase();

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
        virtual StatusCode eventInitialize( TauCandidateData *data );

        //-----------------------------------------------------------------
        //! Execute - called for each tau candidate
        //-----------------------------------------------------------------
        virtual StatusCode execute( TauCandidateData *data );

        //-----------------------------------------------------------------
        //! Cleanup - called for each tau rejected candidate
        //-----------------------------------------------------------------
        virtual void cleanup( TauCandidateData *data );

        //-----------------------------------------------------------------
        //! Event finalizer - called at the end of each event
        //-----------------------------------------------------------------
        virtual StatusCode eventFinalize( TauCandidateData *data );

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

#endif // TOOLBASE_TAU_H
