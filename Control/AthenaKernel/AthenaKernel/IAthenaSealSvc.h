/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Athena Kernel package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IAthenaSealSvc.h,v 1.5 2008-06-02 07:49:43 yyao Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAKERNEL_IATHENASEALSVC_H
# define ATHENAKERNEL_IATHENASEALSVC_H


#include "GaudiKernel/IInterface.h"

// Declaration of the interface ID ( interface id, major version, minor version) 


/** @class IAthenaSealSvc

 * @brief   The interface implemented by the AthenaSealSvc service. This is
 *    used by clients to check the seal dictionary.
              
 *   @author RD Schaffer
 */  
class IAthenaSealSvc : virtual public IInterface {
public:
    /** Check dictionary for a class 
      - dictionary filler must have been loaded, either by the
        "system" or explicitly with "load" below
      @param typeName - type/name of class to check
    */
    virtual StatusCode checkClass(const std::string& typeName) const = 0;


    /** Dynamically load a dictionary filler
	@param dictName - name of dict filler lib, e.g. MyClassesDict
    */
    virtual StatusCode loadDictFiller(const std::string& dictName) const = 0;

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { 
      static const InterfaceID iid("IAthenaSealSvc", 1 , 0); 
      return iid;
    }
};




#endif // ATHENAKERNEL_IATHENASEALSVC_H
