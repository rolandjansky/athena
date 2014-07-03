/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackBarcodeInfo_H
#define TrackBarcodeInfo_H

#include "VTrackInformation.h"

namespace ISF {
  class ISFParticle;
}

class TrackBarcodeInfo: public VTrackInformation {
public:
        TrackBarcodeInfo(int bc, const ISF::ISFParticle* isp=0);
	int GetParticleBarcode() const;
	const ISF::ISFParticle *GetISFParticle() const;
	void SetISFParticle(const ISF::ISFParticle*);

private:
	const ISF::ISFParticle *theISFParticle;
	int barcode;
};


#endif // TrackBarcodeInfo_H
