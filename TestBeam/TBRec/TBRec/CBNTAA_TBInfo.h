/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNTAA_TBInfo_H
#define CBNTAA_TBInfo_H

#include "CBNT_TBRecBase.h"
#include "GaudiKernel/NTuple.h"
#include <vector>

class TBCaloCoordinate;
class TBCondRunParTool;
class CBNTAA_TBInfo : public CBNT_TBRecBase {
public:
	CBNTAA_TBInfo(const std::string &name, ISvcLocator *pSvcLocator);
        ~CBNTAA_TBInfo();
	virtual StatusCode CBNT_initialize();
	virtual StatusCode CBNT_execute();
	virtual StatusCode CBNT_finalize();

private:
	float GetEnergy(float CurrB3, float CurrB4);
	float GetBdl3(float current);
	float GetBdl4(float current);
	float GetErrColl(float coll3_down, float coll3_up, float coll9_down, float coll9_up);
	float GetErrAbsE(float energy);
	float GetErrCurr(float currB3, float currB4);
	float SyncLoss(float energy, float currB3, float currB4);
	float SyncLossErr(float loss);
	

	TBCaloCoordinate* m_coord;
	TBCondRunParTool* m_runpar;
	bool m_DBRead;
	int m_beamener;
	int m_beamtype;
	bool m_computeBeamEnergy;
	bool m_dumpBeamLine;

	//for VLE setup...
	float  m_B8_Bdl; //B8dl of VLE setup B8 magnet
	bool   m_is_VLE; //a flag showing whether the run is a VLE run or HLE run

	//previous values of calculated variables used for caching pruposes...
	int m_prevrunNum;
	float m_prevEnergy;
	float m_preverrAbsEnergy;
	float m_preverrCollimators;
	float m_preverrCurrents;
	float m_prevsycLoss;
	float m_preverrSycLoss;
	float m_prevB3;
	float m_prevB4;
	float m_prevB8;
	//new method added by H.Hakobyan to deal with energy calculation out of execute()..
	void calculateAll(int runNumber,int eventNumber);

    // ntuple variables
    // for test beam
	double    m_runEta_nt;
	long      m_runParticleType_nt;
	long      m_runEnergy_nt;

	std::vector<float>    *m_quad_file;
	std::vector<float>    *m_quad_equip;
	
	std::vector<float>    *m_bend_file;
	std::vector<float>    *m_bend_equip;
	
	std::vector<float>    *m_trim_file;
	std::vector<float>    *m_trim_equip;

	std::vector<float>    *m_coll_file;
	std::vector<float>    *m_coll_equip;

	float      m_energy;
	float      m_errAbsEnergy;
	float      m_errCollimators;
	float      m_errCurrents;
	float      m_sycLoss;
	float      m_errSycLoss;

        StatusCode CBNT_clear();
};

#endif // CBNT_EVENTINFO_H










