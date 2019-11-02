// emacs: this is -*- c++ -*-
//
//   IRegSelTool.h        
// 
//    Interface for the new local RegionSelector tool
// 
//
// Copyright (C) 2012-2019 CERN for the benefit of the ATLAS collaboration


#ifndef IREGIONSELECTOR_IREGSELTOOL_H
#define IREGIONSELECTOR_IREGSELTOOL_H

#include "GaudiKernel/IAlgTool.h"


#include "Identifier/IdentifierHash.h"
#include <vector>
#include <stdint.h>
#include "IRegionSelector/IRoiDescriptor.h"
#include "GaudiKernel/IAlgTool.h"


// static const InterfaceID IID_IRegSelTool("IRegSelTool",  , ); 

/**
 * @class IRegSelTool
 * @brief 
 **/

class IRegSelTool : virtual public IAlgTool {

public: 

  /// InterfaceID
  DeclareInterfaceID( IRegSelTool, 1, 0 ); /// ???? 

    
  /// IdentifierHash methods

    
  //! HashIDList interface declaration. %return  list of unique IdentifierHash

  /*!
    \param IRoiDescriptor \c \b roi, the IRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated  %Identifier %Hash numbers.
  */
  virtual void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) = 0;
  

  //! HashIDList interface declaration. %return list of non-repeated IdentifierHash
  /*!
    \param long           \c \b layer, long int to decide which layer within the detector.
    \param IRoiDescriptor \c \b roi, the IRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers.
  */
  virtual void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) = 0; 
   
   
  /// Rob identifier methods methods

  //! ROBIDList interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param IRoiDescriptor \c \b the IRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */

  virtual void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) = 0; 


  //! ROBIDList interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param long   \c \b layer, long int to decide which layer within the detector.
    \param IRoiDescriptor \c \b the IRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */
  
  virtual void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) = 0;   
  
    

// since these methods are all protected, they shouldn;t be in the interface, since 
// folk will not be able to use them - but will leve them im for the time being
//  
// protected:

//   /// Fullscan methods - when not specifying RoI 
//   /// These are protected, since the roidescriptor now has a Fullscan flag, 
//   /// so the normal RoI descriptors should be used to flag this, and then 
//   /// call the full scan methods internally 

//   //! HashIDList interface declaration. %return list of non-repeated IdentifierHash
//   /*!
//     \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers for the complete subdetector.
//   */
//   virtual void HashIDList( std::vector<IdentifierHash>& idlist ) = 0;


//   //! HashIDList interface declaration. %return list of non-repeated IdentifierHash
//   /*!
//     \param long   \c \b layer, long int to decide which layer within the detector.
//     \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers for the complete subdetector.
//   */
//   virtual void HashIDList( long layer, std::vector<IdentifierHash>& idlist ) = 0;


//   /// Again, fullscan methods that return the list for the entire detector sub component
//   /// These are protected, since the RoiDescriptor contains the fullscan flag, and so 
//   /// a full scan instance is accessed by passing in a fullscan RoiDescriptor and we want 
//   /// to discourage any other use pattern 
  
//   //! ROBIDList interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
//   /*!
//     \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers for the complete subdetector. 
//   */

//   virtual void ROBIDList( std::vector<uint32_t>& roblist ) = 0; 


//  //! ROBIDList interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
//   /*!
//     \param long   \c \b layer, long int to decide which layer within the detector.
//     \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
//   */

//   virtual void ROBIDList( long layer, std::vector<uint32_t>& roblist ) = 0; 

};



#endif //  IREGIONSELECTOR_IREGSELTOOL_H
