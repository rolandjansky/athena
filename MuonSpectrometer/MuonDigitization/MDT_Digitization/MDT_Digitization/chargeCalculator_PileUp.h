/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DIGITIZATION_CHARGECALCULATOR2_H
#define MDT_DIGITIZATION_CHARGECALCULATOR2_H
/*-----------------------------------------------
  
Created 20-8-2011 by Oleg.Bulekov@cern.ch
Function chargeCalculator returns the value of electric charge for Qball particle.
The information about charge coded in the pdgid of Qball particle is used.
It is supposed that the first digit of decimal number of charge corresponds 
to the third digit of decimal number of the  pdgid.
(ex. Qball pdgid=10000200 corresponds to Q=2 )

-----------------------------------------------*/

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "PathResolver/PathResolver.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/MdtTubeCalibContainer.h"

#include "TrkDetDescrUtils/GeometryStatics.h"

#include <vector>
#include <sstream>


#include <iostream>

// SB
#include "AtlasHepMC/GenParticle.h" 
//

double chargeCalculator_PileUp(const MDTSimHit& hit, unsigned short eventId=0){

    const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
    const HepMcParticleLink::PositionFlag idxFlag = (eventId==0) ? HepMcParticleLink::IS_POSITION: HepMcParticleLink::IS_INDEX;
    const HepMcParticleLink trkParticle(hit.trackNumber(),eventId,evColl,idxFlag);
    const HepMC::GenParticle* genParticle = trkParticle.cptr(); 
	double qcharge=1.;
    if (genParticle){
		int particleEncoding = genParticle->pdg_id();
//      std::cout << "SB: pdgId=" << particleEncoding <<std::endl;
		if (((int)(abs(particleEncoding)/10000000) == 1) && ((int)(abs(particleEncoding)/100000)==100)) {
			qcharge = ((abs(particleEncoding) / 100000.0) - 100.0) * 1000.0;
		if (particleEncoding < 0.0) qcharge = -qcharge;
//		std::cout << "SB: BINGO! Qball: qcharge=" << qcharge <<std::endl;	
		}
                else if(((int)(abs(particleEncoding)/10000000) == 2) && ((int)(abs(particleEncoding)/100000)==200)) {
                        qcharge = (double)((abs(particleEncoding) / 1000) % 100) / (double)((abs(particleEncoding) / 10) % 100);
                        if (particleEncoding < 0.0) qcharge = -qcharge;
                }
    } else {
//      std::cout << "SB: genParticle=0 " <<std::endl;
    }

return qcharge;
}

#endif
