/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECEVENT_LARCELLCONTAINER_H
#define LARRECEVENT_LARCELLCONTAINER_H


/** 
    @class LArCellContainer
    @brief Container Class for LArCell (obsolete)

    This is a data object, containing a collection of 
    CaloCell Objects.  Note that LArCell inherits from CaloCell
    and LArCellContainer inherits from CaloCellContainer.
    While retrieving the collection from the TDS - the
    default iterator while iterating over this collection is
    therefore CaloCell. Derived iterators are provided to
    return the pointer to LArCell is necessary.


    @author Hong Ma
    @author Srini Rajagopalan
*/

/*
CREATED:  Dec. 15, 1999

Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Nov. 03, 2000   (HM)
         Follow new LArCell Contructor (include time, quality)

Updated: Dec. 02, 2000    (M. Wielers, P. Loch)
         Extended to work for FCAL

Updated: Jan. 2, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

Update:  May 10, 2001 (HMA) 
	 add CLID 
	 convert to GaudiKernel
	 
Updated: April 11, 2002 (HMA) 
	Moved to LArCalorimeter/LArRecEvent 

Updated: June 24, 2003 (DR) 
        Add optional Ownership switch in constructor for use with DataPool 
********************************************************************/

#include "DataModel/DataVector.h" 
#include "CLIDSvc/CLASS_DEF.h"
#include "LArRecEvent/LArCell.h"

class LArCellContainer : public DataVector<LArCell>
{

 public:

  /** @brief Main constructor */
  LArCellContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) : DataVector<LArCell>(ownPolicy){ };

  /**  @brief destructor */  
  virtual ~LArCellContainer() { };

  /** @brief dump  */
  void print();
  
};

CLASS_DEF(LArCellContainer, 2731, 1)

#endif

