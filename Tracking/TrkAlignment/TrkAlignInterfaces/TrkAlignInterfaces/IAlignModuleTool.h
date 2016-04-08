/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_ALIGNMODULETOOL_IH
#define TRKALIGNINTERFACES_ALIGNMODULETOOL_IH

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"

#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"

#include <fstream>

/**
   @file ITrkAlignDBTool.h
   @class ITrkAlignDBTool
   
   @brief Interface for tool use for managing list of AlignModules, 
   detector elements in AlignModules, and AlignPar for both.

   To be implemented for each detector-type separately.

   @author Robert Harrington <roberth@bu.edu>
   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 1/5/08
*/

namespace Trk {
  class Track;
  class AlignTrack;
  class AlignTSOS;
  class RIO_OnTrack;

  static const InterfaceID 
    IID_TRKALIGNINTERFACES_IAlignModuleTool("Trk::IAlignModuleTool",1,0);
  
  
  class IAlignModuleTool: virtual public IAlgTool {
   
  public:
    static const InterfaceID& interfaceID();
    
    virtual ~IAlignModuleTool() {};

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    /** returns type of AlignModuleList (see AlignModuleList.h for more info) */
    AlignModuleListType alignModuleListType() { return (AlignModuleListType)m_alignModuleListType; }

    /** gets detector type for a detector element */
    virtual AlignModule::DetectorType getDetectorType(const TrkDetElementBase* det) const = 0;    

    //    
    // GeometryManagerTool methods
    //

    /** sets AlignModules for a particular detector type */
    virtual void setAlignModules(AlignModuleList* alignmods,
				 std::vector<AlignModuleList*>* idHashToAlignModMap) = 0;
    
    /** sets AlignPar for AlignModules for a particular detector type*/
    virtual void setAlignParLists(DataVector<DataVector<AlignPar> >* fullAlignParList, 
				  DataVector<DataVector<AlignPar> >* alignParList) = 0;
    
    /** set and retrieve flag whether or not the sub-TrkDetElementBase structures are aligned for a given detector type */
    virtual void setSubDetElement(AlignModule::DetectorType detType, bool status) = 0;
    virtual bool subDetElement(AlignModule::DetectorType detType) const = 0;

    //
    // AlignTrackCreator methods
    //

    /** finds AlignModule for a detector element (used by AlignTrackCreator) */
    virtual AlignModule * findAlignModule(const RIO_OnTrack * rio,
                                          AlignModule::DetectorType = AlignModule::NDetectorTypes) const = 0;

    /** finds AlignModule for a detector element */
    virtual AlignModule * findAlignModule(const TrkDetElementBase* det,
                                          AlignModule::DetectorType = AlignModule::NDetectorTypes) const = 0;

    /** allows to set detector specific properties of AlignTSOS, like measurement direction (CSC) or fan-out angle (SCT) */
    virtual void setMeasurementProperties(AlignTSOS *) const {};

    //    
    // MatrixTool methods
    //

    /** number of alignment parameters for all detector types */
    virtual int nAlignParameters() const = 0;
    
    /** converts 2-D AlignParList and AlignModuleList for all detector types
	to single 1-D vectors (used by MatrixTool) */
    virtual const AlignModuleList* alignModules1D() = 0;
    virtual DataVector<AlignPar>*  alignParList1D() = 0;
    

    // DerivCalcTool methods

    /** returns alignment parameters for a single module */
    virtual DataVector<AlignPar>* getAlignPars(const AlignModule* alignModule) const = 0; 

    /** shifts and restores tubes traversed by track in module (used by ShiftingDerivCalcTool) */
    virtual void shiftModule(const AlignModule*, const Track*, 
			     AlignModule::TransformParameters, double) const {}			     
    virtual void restoreModule(const AlignModule*) const {}   


    //
    // AlignDBTool methods
    // 

    /** returns alignment parameters for a detector element, applying AlignModule 
	transforms if necessary */ 
    virtual void fillDetAlignmentParameters(const TrkDetElementBase* det,
					    DataVector<AlignPar>* apVec,
					    AlignModule::DetectorType detType=
					    AlignModule::NDetectorTypes) const = 0;

    /** gets all AlignModules for a particular detector type */
    virtual const AlignModuleList* getAlignModuleCollection() const = 0;


    //
    // FillNtuple methods
    //

    /** returns vector of full AlignPars for an AlignModule */
    virtual DataVector<AlignPar>* getFullAlignPars(const AlignModule* alignModule) const = 0;

    //
    // Other
    //

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

  protected:
    int m_alignModuleListType;  //!< uses Trk enum AlignModuleListType (L1,L2,L3,L1_5,L2_5)

    std::ostream * m_logStream; //!< logfile output stream

  };

  inline const InterfaceID& IAlignModuleTool::interfaceID()
  { return   IID_TRKALIGNINTERFACES_IAlignModuleTool; }
  
} // end namespace
  
#endif // TRKALIGNINTERFACES_ALIGNMODULETOOL_IH
