/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* author : Hong Ma */

/* date of creation : Aug 17, 2003 */


#ifndef LARFAKEHITPRODUCER_H
#define LARFAKEHITPRODUCER_H

#include "LArSim/LArHitProducer.h"

class LArFakeHitProducer : public LArHitProducer {

public:

    /** usual ATHENA constructor of an algorithm*/
    LArFakeHitProducer(const std::string & name, ISvcLocator * pSvcLocator);

    /** Destructor */
    virtual ~LArFakeHitProducer() { }

    /**
     * Method in which the hits are produced <br><br> This method must be redefined in the doughter <br>
     * specializations of this class, to accommodate the <br> specific code
     */
    virtual StatusCode hitConstruction();

private:

};
#endif    //LARFAKEHITPRODUCER_H


