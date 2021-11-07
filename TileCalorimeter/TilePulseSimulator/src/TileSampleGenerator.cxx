/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TilePulseSimulator/TileSampleGenerator.h"
#include "TilePulseSimulator/TilePulseShape.h"
#include "TilePulseSimulator/TileSampleBuffer.h"

#include "TArrayD.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include <iostream>
#include <cstdio>

using namespace std;

//
//_______________________________________________________________
TileSampleGenerator::TileSampleGenerator() :
		m_ps(0), m_buf(0), m_DEBUG(false), m_rndm(nullptr) {

}

//
//_______________________________________________________________
TileSampleGenerator::TileSampleGenerator(TilePulseShape* ps, TileSampleBuffer* buf, bool dd_DEBUG) :
		m_ps(ps), m_buf(buf), m_DEBUG(dd_DEBUG) {
	m_rndm = new TRandom3();
}

//
//_______________________________________________________________
TileSampleGenerator::~TileSampleGenerator() {
	delete m_rndm;
}

//
//_______________________________________________________________
void TileSampleGenerator::fillSamples(double t0, double pedestal, double amplitude1, double amplitude2, TF1* pdf, bool addNoise, double itOffset, double otOffset) {
	for (unsigned int i = 0; i < m_buf->size(); ++i) {
		double x1 = m_buf->getTime(i) - t0 + itOffset;
		double y1 = m_ps->eval(x1) * amplitude1;
		double x2 = m_buf->getTime(i) - t0 + otOffset;
		double y2 = m_ps->eval(x2) * amplitude2;
		double y = y1 + y2 + pedestal;

		double noise(0), val(y);
		if (addNoise) {
			noise = pdf->GetRandom();
			val += noise;
		}

		m_buf->setValueNoise(i, (noise));
		m_buf->setValue(i, val);
	}

	return;
}

//
//________________________________________________________
void TileSampleGenerator::fillNSamples(double t0, double pedestal, double amp_it, vector<float> amp_pu, TF1* pdf, bool addNoise, double itOffset, int nSamples, int nPul) {

    std::unique_ptr<TileSampleBuffer> bufall(new TileSampleBuffer(nPul, -25*((nPul-1)/2), 25.));

	if(m_DEBUG){
		cout << "Pileup pulses:" << std::endl;
		for (std::vector<float>::const_iterator i = amp_pu.begin(); i != amp_pu.end(); ++i)
    		std::cout << *i << ' ';
		std::cout << std::endl;
	}
	

	for (int i = 0; i < nSamples; i++) { //Loop over output samples
		double tin, amp_it_out;
		int nPul = amp_pu.size();
		vector<int> t(nPul);
		vector<float> amp_pu_out(nPul);
		int x = (nPul-1)/2 + nSamples;  
		double amp_total = pedestal;

		if (m_DEBUG)
			cout << "sample to compute: " << i << "  " << amp_total << std::endl;

		for (int j = 0; j < nPul ; j++) { //Loop over PU pulses

			t[j] = bufall->getTime(x + i - j) - t0;

			if (m_DEBUG)
				cout << "pileupsample to compute " << j << std::endl;
			if (m_DEBUG)
				cout << "                time in " << i << " " << j << " " << (x + i - j) << " " << " buf " << bufall->getTime(x + i - j) << "  time_out " << t[j] << std::endl;

			if (t[j] < -((nSamples-1)/2)*25. || t[j] > ((nSamples+1)/2)*25.)
				continue; //Limits of the PulseShape
			amp_pu_out[j] = m_ps->eval(t[j]) * amp_pu.at(j); // PU Contribution
			amp_total += amp_pu_out[j];

			if (m_DEBUG)
				cout << "                amp_pu  " << amp_pu.at(j) << "  ps  " << m_ps->eval(t[j]) << "  amp_out " << amp_pu_out[j] << std::endl;
			if (m_DEBUG)
				cout << "                amp_total " << amp_total << std::endl;
		}

		tin = m_buf->getTime(i) - t0 + itOffset;
		amp_it_out = m_ps->eval(tin) * amp_it; //Contribution from In-time Pulse
		amp_total += amp_it_out;

		if (m_DEBUG)
			cout << "      INTIME    amp_it  " << amp_it << "  ps  " << m_ps->eval(tin) << "  amp_it_out " << amp_it_out << std::endl;
		if (m_DEBUG)
			std::cout << "                amp_total " << amp_total << std::endl;

		double noise(0), val(amp_total);
		if (addNoise) {
			noise = pdf->GetRandom();
			val += noise;
		}
		if (m_DEBUG)
			cout << "                                     FINAL     " << amp_total << std::endl;
		m_buf->setValueNoise(i, (noise));
		m_buf->setValue(i, val);
	}

	return;
}

//
//________________________________________________________
// Simulation of the PMT pulse shapes for QIE FEB
// Please e-mail your questions to alexander.paramonov@cern.ch
//________________________________________________________
void TileSampleGenerator::fill7SamplesQIE(float amp_it, float *amp_pu) { //float t0, addNoise

	//amp_it = energy/charge of the in-time pulse in fC

	//amp_pu charge deposited by out-of-time interactions assuming the interactions are every 25 ns
	//the amplitudes are in fC

	//The in-time signal produces charges in amp_total[3] and amp_total[4]; ~80% of charge is added to amp_total[3]

	float amp_total[7] = { 0, 0, 0, 0, 0, 0, 0 };
	float tail_Q = 0;
	const float Q_1pe = 17.; //Average charge per a photo-electron
	const float tail_prob = 0.18; //Probability for a photo-electron to have 25<t<75 ns

	for (int i = 0; i < 7; i++) { //Loop over output samples

		//if (m_DEBUG)
		//	std::cout << "sample to compute: " << i << "  " << amp_total << std::endl;

		if (i != 3) { //out-of-time pileup
			tail_Q = Q_1pe * m_rndm->Binomial((int) (amp_pu[i] / Q_1pe), tail_prob);

			amp_total[i] += amp_pu[i] - tail_Q;
			if (i < 6)
				amp_total[i + 1] += tail_Q;

		} else { //in-time pulse

			tail_Q = Q_1pe * m_rndm->Binomial((int) amp_it / Q_1pe, tail_prob);

			amp_total[3] += amp_it - tail_Q;
			amp_total[4] += tail_Q;
		}

		/*
		 double noise(0), val(amp_total);
		 if (addNoise) {
		 noise = pdf->GetRandom();
		 amp_total[i] += noise;
		 }

		 if (m_DEBUG)
		 std::cout << "                                     FINAL     " << amp_total << std::endl;
		 */
	}

	for (unsigned int i = 0; i < 7; i++) { //Loop over output samples
		m_buf->setValueNoise(i, 0);
		m_buf->setValue(i, amp_total[i]);
	}

	return;
}
