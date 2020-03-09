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



/**
 * @class IRegSelTool
 * @brief 
 **/

class IRegSelTool : virtual public IAlgTool {

public: 

  /// InterfaceID
  DeclareInterfaceID( IRegSelTool, 1, 0 ); 

    
  /// IdentifierHash methods

    
  //! HashIDList interface declaration. %return  list of unique IdentifierHash

  /*!
    \param IRoiDescriptor \c \b roi, the IRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated  %Identifier %Hash numbers.
  */
  virtual void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const = 0;
  

  //! HashIDList interface declaration. %return list of non-repeated IdentifierHash
  /*!
    \param long           \c \b layer, long int to decide which layer within the detector.
    \param IRoiDescriptor \c \b roi, the IRoiDescriptor for the roi, all enabled elements in the roi are found.
    \return std::vector<IdentifierHash> which is a list of non-repeated Offline %Identifier %Hash numbers.
  */
  virtual void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const = 0; 
   
   
  /// Rob identifier methods methods

  //! ROBIDList interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param IRoiDescriptor \c \b the IRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */

  virtual void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const = 0; 


  //! ROBIDList interface declaration. This interface can be used by the ID subdetectors. %A list of non-repeated ROBIDs (uint_32_t) is returned by a reference.
  /*!
    \param long   \c \b layer, long int to decide which layer within the detector.
    \param IRoiDescriptor \c \b the IRoiDescriptor for the roi, all enabled elements in the roi are found. 
    \return std::vector<uint32_t> which is a list of non-repeated ROBID numbers.
  */
  
  virtual void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const = 0;   
   
};



#endif //  IREGIONSELECTOR_IREGSELTOOL_H
