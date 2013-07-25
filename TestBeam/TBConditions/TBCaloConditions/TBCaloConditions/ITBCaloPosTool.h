/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBCALOCONDITIONS_ITBCALOPOSTOOL
# define TBCALOCONDITIONS_ITBCALOPOSTOOL

#include "GaudiKernel/IAlgTool.h"

/** 
 ** Class ITBCaloPosTool
 ** 
 ** This is an interface for tool used access the Calorimeter position
 ** in Combined TB

   Sept 28, 2004   Hong Ma 
	

 **/


static const InterfaceID IID_IITBCaloPosTool("IITBCaloPosTool", 1 , 0); 
 
class ITBCaloPosTool : virtual public IAlgTool 
{

public:    
  
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IITBCaloPosTool; }


    ///  access eta value 
    virtual double  eta () = 0 ;

    ///  access eta value 
    virtual double  theta () = 0 ;

    ///  access eta value 
    virtual double  z () = 0 ;

    ///  access delta value 
    virtual double  delta () = 0 ;


};



#endif
