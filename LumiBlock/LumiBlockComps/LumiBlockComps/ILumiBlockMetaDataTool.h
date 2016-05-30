/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LumiBlockComps_ILumiBlockMetaDataTool_H
#define LumiBlockComps_ILumiBlockMetaDataTool_H

/** @file ILumiBlockMetaDataTool.h
 *  @brief This file contains the class definition for the ILumiBlockMetaDataTool class.
 *  @author Max Baak, Balint Radics, Marjorie Shapiro, 
 *  $Id: ILumiBlockMetaDataTool.h,v 1.0 2009-10-7 19:06:50 mbaak Exp $
 **/

#include "GaudiKernel/IAlgTool.h"
#include "TString.h"

namespace Root {
  class TGRLCollection;
}

/** @class ILumiBlockMetaDataTool
 *  @brief This class provides the interface for MetaDataTools.
 **/
class ILumiBlockMetaDataTool : virtual public IAlgTool {

 public:

   /// Gaudi boilerplate
   static const InterfaceID& interfaceID() {
     static const InterfaceID ILumiBlockMetaDataTool( "ILumiBlockMetaDataTool", 1, 0 );
     return ILumiBlockMetaDataTool;
   }

   /// get grl collection
   virtual const Root::TGRLCollection* getGRLCollection() const = 0 ;
   virtual const TString& getUniqueGRLString() const = 0 ;
   virtual const TString getGRLString( const TString& grlname ) const = 0 ;

 private: // Data

};

#endif

