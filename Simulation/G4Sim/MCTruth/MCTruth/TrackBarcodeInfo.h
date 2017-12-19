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
        TrackBarcodeInfo(int bc, const ISF::ISFParticle* baseIsp=0);
	int GetParticleBarcode() const;
	const ISF::ISFParticle *GetBaseISFParticle() const;
	void SetBaseISFParticle(const ISF::ISFParticle*);
	void SetReturnedToISF(bool returned);
	bool GetReturnedToISF() const;

private:
	const ISF::ISFParticle *m_theBaseISFParticle;
	int m_barcode;
	bool m_returnedToISF;
};


#endif // TrackBarcodeInfo_H
