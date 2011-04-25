/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_IGEOMETRYMANAGERTOOL_H
#define TRKALIGNINTERFACES_IGEOMETRYMANAGERTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "TrkAlignEvent/AlignModuleList.h"
#include "DataModel/DataVector.h"

/**
   @file IGeometryManagerTool.h
   @class IGeometryManagerTool
   
   @brief Interface for tool used to build the AlignModules to be used for alignment.  The implementation is specific to a detector type.

   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

#include <fstream>

class TFile;

static const InterfaceID 
IID_TRKALIGNINTERFACES_IGeometryManagerTool("IGeometryManagerTool",1,0);

namespace Trk {

  class AlignModule;
  class AlignPar;

class IGeometryManagerTool : virtual public IAlgTool {

 public:
  virtual ~IGeometryManagerTool() {}

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  /** build collection of AlignModules for a particular detector type, return number of alignment parameters */
  virtual int ReadGeometry(int solvelevel) = 0;

  virtual void PrintModPositions() = 0;
  virtual void StoreModPositions(std::string filename) = 0;

  /** Sets output ntuple file, to be implemented if needed for detector-specific implementation. */
  virtual void setNtuple(TFile*) {} 

  /** sets the output stream for the logfile */
  virtual void setLogStream(std::ostream * os) { m_logStream = os; }

  /** create align modules and add them to the list */
  virtual void buildGeometry() {};

  /** set alignment parameters for a given module and add them to the list
      the arguments are AlignModule, fullParameterList, activeParameterList */
  virtual void addModuleParameters(Trk::AlignModule * , DataVector< DataVector<Trk::AlignPar> > * , DataVector< DataVector<Trk::AlignPar> > *) {};

  /** get next free IDHash usable for new AlignModule */
  virtual int getNextIDHash() { return m_hashCounter++; };

  /** set first free IDHash usable for new AlignModule */
  virtual void setFirstIDHash(unsigned int idhash) { m_hashCounter=idhash; };

  /** set pointer to module list to which the modules should be added to */
  virtual void setModuleList(Trk::AlignModuleList * list) { m_alignModuleListPtr=list; }

  /** set pointer to vector of hashMaps to which the elements should be added to */
  virtual void setHashMaps(std::vector<Trk::AlignModuleList *> * maps) { m_idHashToAlignModuleMapsPtr=maps; }

  /** check whether the alignment level is allowed */
  virtual bool checkAlignLevel() { return true; }

  /** get alignment level */
  virtual int alignLevel() { return m_alignLevel; }

  /** set alignment level */
  virtual void setAlignLevel(int level) { m_alignLevel = level; }

  /** get alignment level */
  virtual int alignLevelBarrel() { return m_alignLevelBarrel; }

  /** set alignment level */
  virtual void setAlignLevelBarrel(int level) { m_alignLevelBarrel = level; }

  /** check whether the alignment level is allowed */
  virtual bool checkAlignLevelBarrel() { return true; }

  /** get alignment level */
  virtual int alignLevelEndcaps() { return m_alignLevelEndcaps; }

  /** set alignment level */
  virtual void setAlignLevelEndcaps(int level) { m_alignLevelEndcaps = level; }

  /** check whether the alignment level is allowed */
  virtual bool checkAlignLevelEndcaps() { return true; }

 protected:

  /** pointer to module list to which the modules are added */
  Trk::AlignModuleList * m_alignModuleListPtr;

  /** pointer to vector of hashMaps to which the elements are added */
  std::vector<Trk::AlignModuleList *> * m_idHashToAlignModuleMapsPtr;

  int m_hashCounter;  //!< variable for setting the idHash of the AlignModules

  std::ostream * m_logStream; //!< logfile output stream

  int m_alignLevel;         //!< alignment level
  int m_alignLevelBarrel;   //!< alignment level
  int m_alignLevelEndcaps;  //!< alignment level

}; // end class definition
  
inline const InterfaceID& IGeometryManagerTool::interfaceID() {
  return   IID_TRKALIGNINTERFACES_IGeometryManagerTool;
}

} // end namespace
  
#endif // TRKALIGNINTERFACES_IGEOMETRYMANAGERTOOL_H
