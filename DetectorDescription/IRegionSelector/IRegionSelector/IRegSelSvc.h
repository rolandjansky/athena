// emacs: this is -*- c++ -*-
//
//   IRegSelSvc.h        
// 
//    Interface for the RegionSelector (RegSelSvc) service                  
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: IRegSelSvc.h,v 1.1.1.1 2008-11-19 15:33:35 sutt Exp $

#ifndef IREGSELSVC_H
#define IREGSELSVC_H
#include "GaudiKernel/IInterface.h"
#include "Identifier/IdentifierHash.h"
#include <vector>
#include <stdint.h>
#include "IRegionSelector/RegSelEnums.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "IRegionSelector/IRoiDescriptor.h"

//static const InterfaceID IID_IRegSelSvc("IRegSelSvc", 1 , 0);

class IRegSelSvc: virtual public IInterface {
 public:
  /// Destructor
  virtual ~IRegSelSvc(){}
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { 
    static const InterfaceID _IID("IRegSelSvc", 1 , 0);
    return _IID; 
  }
  //static const InterfaceID& interfaceID() { return IID_IRegSelSvc; }
  
  
  /// IdentifierHash methods
  
  
  //! DetHashIDList interface declaration. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID          \c \b detectorID, an enum to define subdetectors
    \param IRoiDescriptor \c \b roi, the IRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, 
			      const IRoiDescriptor& roi, 
			      std::vector<IdentifierHash>& IDList) = 0;
  


  //! DetHashIDList interface declaration. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID          \c \b detectorID, an enum to define subdetectors
    \param long           \c \b layer, long int to decide which layer within the detector.
    \param IRoiDescriptor \c \b roi, the IRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, 
			      long layer,
			      const IRoiDescriptor& roi, 
			      std::vector<IdentifierHash>& IDList) = 0;



  /// Fullscan methods - when not specifying RoI 
  /// Technically, these should be made private, sinec the roidescriptor now has a Fullscan flag, 
  /// so the normal RoI descriptors should be used to flag this, and then call the full scan methods
  /// internally 

  //! DetHashIDList interface declaration. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID   \c \b detectorID, an enum to define subdetectors
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers for the complete subdetector.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, 
			      std::vector<IdentifierHash>& IDList) = 0;


  //! DetHashIDList interface declaration. %A list of non-repeated IdentifierHash is returned
  /*!
    \param DETID  \c \b detectorID, an enum to define subdetectors
    \param long   \c \b layer, long int to decide which layer within the detector.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers for the complete subdetector.
    \sa DetROBIDList(), DetROBIDListUint()
  */
  virtual void DetHashIDList( DETID detectorID, 
			      long layer,
			      std::vector<IdentifierHash>& IDList) = 0;




  
  /// Rob identifier methods methods


  //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID          \c \b detectorID, an enum to define subdetectors
    \param IRoiDescriptor \c \b the IRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */

  virtual void DetROBIDListUint(DETID detectorID, 
				const IRoiDescriptor& roi, 
				std::vector<uint32_t>& outputROBIDList) = 0; 


  //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID  \c \b detectorID, an enum to define subdetectors
    \param long   \c \b layer, long int to decide which layer within the detector.
    \param IRoiDescriptor \c \b the IRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */

  virtual void DetROBIDListUint(DETID detectorID, 
				long layer,  
				const IRoiDescriptor& roi, 
				std::vector<uint32_t>& outputROBIDList) = 0; 


  //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID \c \b detectorID, an enum to define subdetectors
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers for the complete subdetector. 
  */

  virtual void DetROBIDListUint(DETID detectorID, 
				std::vector<uint32_t>& outputROBIDList) = 0; 

	
 //! DetROBIDListUint interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param DETID  \c \b detectorID, an enum to define subdetectors
    \param long   \c \b layer, long int to decide which layer within the detector.
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */

  virtual void DetROBIDListUint(DETID detectorID, 
				long layer, 
				std::vector<uint32_t>& outputROBIDList) = 0; 



  /** beginRun is when all the tables are initialized */
  virtual StatusCode handle(IOVSVC_CALLBACK_ARGS) = 0;




  /// NB: This should be removed
  //! GetEtaPhi interface declaration. This interface can be used by all subdetectors. %Given a IdentifierHash number eta and phi ranges are returned
  /*!
    \param DETID \c \b detectorID, an enum to define subdetectors
    \param IdentifierHash \c \b hashId, Offline %Identifier %Hash number    
    \return double \c \b etaMin, a double argument that represents eta minimum value in a range.
    \return double \c \b etaMax, a double argument that represents eta maximum value in a range.
    \return double \c \b phiMin, a double argument that represents phi minimum value in a range.
    \return double \c \b phiMax, a double argument that represents phi maximum value in a range.
  */
  virtual void GetEtaPhi(DETID detectorID, IdentifierHash hashId, 
			 double *etaMin, double *etaMax,
			 double *phiMin, double *phiMax ) = 0;

  /** return the stored roi extend in z at the beamline **/
  virtual double getDeltaZ() const = 0;

  /** Method to Disable LAr ROBs */
  virtual void DisableMissingROBs(const std::vector<uint32_t>& vec) = 0;


};

#endif
