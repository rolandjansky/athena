/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEPULSESIMULATOR_TILESAMPLEGENERATOR_H
#define TILEPULSESIMULATOR_TILESAMPLEGENERATOR_H

#include "TObject.h"


class TilePulseShape;
class TileSampleBuffer;
class TRandom3;
class TF1;

class TileSampleGenerator: public TObject {
public:
	TileSampleGenerator();
	TileSampleGenerator(TilePulseShape* ps, TileSampleBuffer* buf, bool dd_DEBUG = false);
	virtual ~TileSampleGenerator();

	inline void setPulseShape(TilePulseShape* ps) {
		m_ps = ps;
	}
	void fillSamples(double t0, double pedestal, double amplitude1, double amplitude2, TF1* pdf, bool addNoise, double itOffset = 0, double otOffset = 50);
	void fillNSamples(double t0, double pedestal, double amp_it, std::vector<float> amp_pu, TF1* pdf, bool addNoise, double itOffset = 0, int nSamples = 7, int nPul = 21);
	void fill7SamplesQIE(float amp_it, float *amp_pu); //The function calculates charges for 7 intervals with each interval of 25 ns

private:
	TilePulseShape* m_ps;
	TileSampleBuffer* m_buf;
	bool m_DEBUG;
	TRandom3* m_rndm;

public:

};

#endif // TILEPULSESIMULATOR_TILESAMPLEGENERATOR_H
