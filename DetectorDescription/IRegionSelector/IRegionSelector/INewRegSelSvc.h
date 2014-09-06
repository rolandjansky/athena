// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   IRegSelSvc.h        
// 
//    Interface for the RegionSelector (NewRegSelSvc) service                  
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: INewRegSelSvc.h,v 1.1.1.1 2008-11-19 15:33:35 sutt Exp $

#ifndef INEWREGSELSVC_H
#define INEWREGSELSVC_H
#include "GaudiKernel/IInterface.h"
#include "Identifier/IdentifierHash.h"
#include <vector>
#include <stdint.h>
#include "IRegionSelector/RegSelEnums.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "IRegionSelector/INewRoiDescriptor.h"

//static const InterfaceID IID_INewRegSelSvc("INewRegSelSvc", 1 , 0);

class INewRegSelSvc: virtual public IInterface {
public:
  /// Destructor
  virtual ~INewRegSelSvc(){}
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { 
    static const InterfaceID _IID("INewRegSelSvc", 1 , 0);
    return _IID; 
  }
  //static const InterfaceID& interfaceID() { return IID_INewRegSelSvc; }
  
  /// Interface provided by this INewRegSelSvc service



  //! DetHashIDList interface declaration. This interface can be used by all the ID subdetectors. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID  \c \b detectorID, an enum to define subdetectors
    \param INewRoiDescriptor \c \b roi, the INewRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, 
			      const INewRoiDescriptor& roi, 
			      std::vector<IdentifierHash>& IDList) = 0;



  //! DetHashIDList interface declaration. This interface can be used by all the ID subdetectors. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID  \c \b detectorID, an enum to define subdetectors
    \param long  \c \b layer, long int to decide which layer within the detector.
    \param INewRoiDescriptor \c \b roi, the INewRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, long layer,
			      const INewRoiDescriptor& roi, 
			      std::vector<IdentifierHash>& IDList) = 0;


  //! DetHashIDList interface declaration. This interface can be used by all the ID subdetectors. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID  \c \b detectorID, an enum to define subdetectors
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers for the complete subdetector.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, std::vector<IdentifierHash>& IDList) = 0;
  


  //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID \c \b detectorID, an enum to define subdetectors
    \param INewRoiDescriptor \c \b the INewRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */
  virtual void DetROBIDListUint(DETID detectorID, 
				const INewRoiDescriptor& roi, 
				std::vector<uint32_t>& outputROBIDList) = 0; 
  


  //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID \c \b detectorID, an enum to define subdetectors
    \param long   \c \b layer, long int to decide which layer within the detector.
    \param INewRoiDescriptor \c \b the INewRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */
  virtual void DetROBIDListUint(DETID detectorID, long layer, 
				const INewRoiDescriptor& roi, 
				std::vector<uint32_t>& outputROBIDList) = 0; 

  
  
  //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID \c \b detectorID, an enum to define subdetectors
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers for the complete subdetector. 
  */
  virtual void DetROBIDListUint(DETID detectorID, std::vector<uint32_t>& outputROBIDList) = 0; 
  


  /** beginRun is when all the tables are initialized */
  virtual StatusCode handle(IOVSVC_CALLBACK_ARGS) = 0;

  /** Method to Disable LAr ROBs */
  //   virtual void DisableMissingROBs(const std::vector<uint32_t>& vec) = 0;

};

#endif
