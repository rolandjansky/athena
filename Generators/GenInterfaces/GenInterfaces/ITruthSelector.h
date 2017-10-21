/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 tool interface to obtain barcode(s) for charged tracks produced or
 reconstructable inside the Inner Detector acceptance.
 ***************************************************************************/

#ifndef TRUTHTOOLS_ITRUTHSELECTOR_H
#define TRUTHTOOLS_ITRUTHSELECTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <map>
#include <vector>
#include "GaudiKernel/IAlgTool.h"

class McEventCollection;

/** Interface ID for ITruthSelector*/  
static const InterfaceID IID_ITruthSelector("ITruthSelector", 1, 0);
  
/**@class ITruthSelector

Abstract base class for TruthSelector AlgTool


@author Alan.Poppleton@cern.ch
*/

class ITruthSelector: virtual public IAlgTool
{
public:
 
    /**Virtual destructor*/
    virtual ~ITruthSelector() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITruthSelector; }
  
    /**ITruthSelector interface:
       map to provide user code-independence between old G3 and G4 kine/barcode labelling schemes */
    virtual const std::map<int,int>&	indetKineMap(void) = 0;
      
    /**ITruthSelector interface:
       select muons with production vertex inside InDet envelope
       developement needed to provide equivalent electron functionality */
    virtual const std::vector<int>&	indetMuons(double minPt) = 0;
     
    /**ITruthSelector interface:
       access McEventCollection (auto-switch between G3, DC2 and current naming) */
    virtual const McEventCollection*	mcEventCollection(void) = 0;
     
    /**ITruthSelector interface:
       jobOption control of the primary/secondary definitions below
       vector of barcodes for primary and close-in secondary tracks that should be reconstructed */
    virtual const std::vector<int>&	reconstructablePrimaries(double minPt) = 0;
        
    /**ITruthSelector interface:
       vector of barcodes for secondary tracks that should be reconstructed */
    virtual const std::vector<int>&	reconstructableSecondaries(double minPt) = 0;

};
   
#endif // TRUTHTOOLS_ITRUTHSELECTOR_H

