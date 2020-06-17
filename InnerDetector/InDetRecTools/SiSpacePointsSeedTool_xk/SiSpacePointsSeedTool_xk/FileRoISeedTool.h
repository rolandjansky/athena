/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for Z-window RoI from an input file
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedTool_xk_FileRoISeedTool_h
#define SiSpacePointsSeedTool_xk_FileRoISeedTool_h

#include "InDetRecToolInterfaces/IZWindowRoISeedTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <vector>


namespace InDet {

  class FileRoISeedTool : 
   virtual public IZWindowRoISeedTool ,public AthAlgTool
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    FileRoISeedTool(const std::string&,const std::string&,const IInterface*);
    virtual ~FileRoISeedTool();
    virtual StatusCode               initialize();
    virtual StatusCode               finalize  ();

    /** Compute RoI */
    virtual std::vector<ZWindow> getRoIs();

  protected:

    /**    @name Disallow default instantiation, copy, assignment **/
    FileRoISeedTool() = delete;
    FileRoISeedTool(const FileRoISeedTool&) = delete;
    FileRoISeedTool &operator=(const FileRoISeedTool&) = delete;

    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
 
    std::string m_filename; //name of file
    float m_z0Window; /// width of z0 window
    
  }; // FileRoISeedTool
} //InDet namespace

#endif // SiSpacePointsSeedMaker_FileRoISeedTool

