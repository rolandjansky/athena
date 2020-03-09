/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Alexander Madsen
/// @author Nils Krumnack



#ifndef EVENT_LOOP_GRID_DRIVER_H
#define EVENT_LOOP_GRID_DRIVER_H

#include "EventLoop/Driver.h"

namespace SH {
  class SampleGrid;
  class MetaObject;
}

namespace EL {

  std::string getRootCoreConfig ();

  /// \brief a \ref Driver to submit jobs via ganga

  class GridDriver final : public Driver {

  public:

    GridDriver ();

    void testInvariant () const;

    static void status(const std::string& location);

    static void status();

    static void kill(const std::string& location);

    static void killAll();

    static std::string listActive();

    static void reset();

    static bool startServer();

    static bool stopServer();

    std::string outputSampleName;
    bool express;
    bool mergeOutput;
    std::string destSE;
    std::string site;
    std::string cloud;
    int memory;
    int maxCpuCount;
    int nFiles;
    int nFilesPerJob;
    int nJobs;
    int nMinorRetries;
    int nMajorRetries;
    float nGBPerJob;
    std::string workDir;
    std::string extFile;         //To be made obsolete, please do not use!
    std::string excludedSite;
    std::string rootVer; 
    std::string cmtConfig; 

    static std::string gangaLogFile;

    //These fields are currently not supported
    //std::string excludeFile;
    //bool noSubmit;              //Should maybe enable this again
    //int maxNFilesPerJob;        //no clear way to do this in ganga
    //int maxFileSize;            //no clear way to do this in ganga
    //std::string spaceToken;     //users can only output to scratchdisk
    //std::string useChirpServer; //ganga has this possibility
    //bool enableRetries;         //done by tasks 

    //std::string downloadIncludeFileMask;
    //std::string downloadExcludeFileMask;

    //These functions are kept for backwards compatibility.
    //They should be considered obsolete and will be removed in a future version.
    void gather(const std::string location) const;
    SH::SampleGrid* createSampleFromDQ2(const std::string& dataset) const;

  protected:
    virtual ::StatusCode
    doManagerStep (Detail::ManagerData& data) const override;

  private:
    ::StatusCode doRetrieve (Detail::ManagerData& data) const;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(EL::GridDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
