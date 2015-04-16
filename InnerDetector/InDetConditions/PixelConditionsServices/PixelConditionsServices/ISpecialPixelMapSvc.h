/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_ISPECIALPIXELMAPSERVICE_H
#define PIXELCONDITIONSSERVICES_ISPECIALPIXELMAPSERVICE_H


#include "GaudiKernel/IInterface.h" 
#include "AthenaKernel/IOVSvcDefs.h" 

#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "PixelConditionsData/SpecialPixelMap.h"



/** 
 * Abstract interface for SpecialPixelMapSvc
 *
 * Creates and manages a set of special pixel map objects in the detector store. If not explicitly switched
 * off in the job options, a callback funtion is registered that creates the special pixel map objects in the
 * detector store and automatically keeps them up to date for the current run/lumi block. By default all
 * special pixel maps are merged into one map which is stored in place of the first map in the list of
 * StoreGate keys.
 *
 * In addition to the callback function the following functions are provided: A special pixel map object can
 * be created explicitly with the create method. This method can be configured to either use the data from
 * the offline COOL database or from text files. The contents of maps for selected modules can be printed to
 * the screen with print(). The number of maps managed can be retrieved with getNPixelMaps(), and the maps
 * can be retrieved with getPixelMap(unsigned int index). 
 *
 * ruwiedel@physik.uni-bonn.de 
 *
 */


class ISpecialPixelMapSvc: virtual public IInterface{
			   
public: 
  virtual ~ISpecialPixelMapSvc(){}

  static const InterfaceID& interfaceID();

  /** automatically creates and updates pixel map objects in the detector store for the current run/lumi block */
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) = 0; //!< callback function registered in initialize()

  virtual StatusCode createFromTextfiles(bool fillMissing = true) const = 0;
  //!< create special pixel map objects using data read from text files (for details see the README file in the doc directory)
  virtual StatusCode createFromDetectorStore(const std::string condAttrListCollectionKey, const std::string pixelMapKey,
					     bool fillMissing = true ) const = 0;
  //!< create special pixel map from CondAttrListCollection at key
  virtual StatusCode createFromDetectorStore(bool fillMissing = true) const = 0;
  //!< create special pixel map objects using data from the offline COOL database
  virtual StatusCode create() const = 0;
  //!< create special pixel map objects using the data source specified in the job options
  virtual StatusCode print() const = 0;
  //!< print pixel maps for the modules specified in the job options to the screen
  
  virtual unsigned int getNPixelMaps() const = 0;
  //!< returns the number of special pixel map objects managed by the SpecialPixelMapSvc
  virtual const DetectorSpecialPixelMap *getPixelMap(unsigned int index) const = 0;
  //!< returns a const pointer to the index'th special pixel map

  virtual unsigned int getChips(const unsigned int & index) const = 0;
  //!< returns the number of chips and type of module as 10*mchips+i 

  virtual StatusCode registerCondAttrListCollection(const DetectorSpecialPixelMap* spm) const = 0;
  //!< convert spm to CondAttrListCollection and register in the detector store
  //!< used when preparing database write
}; 

inline const InterfaceID& ISpecialPixelMapSvc::interfaceID(){
  static const InterfaceID IID_ISpecialPixelMapSvc("ISpecialPixelMapSvc", 1, 0); 
  return IID_ISpecialPixelMapSvc;
}

#endif 
