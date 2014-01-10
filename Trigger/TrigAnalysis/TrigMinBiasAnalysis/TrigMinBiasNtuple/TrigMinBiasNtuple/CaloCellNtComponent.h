/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_CaloCellNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_CaloCellNTCOMPONENT_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

class TileID;
class CaloCellContainer;

namespace NtComponent {
  class CaloCellNtComponent: public NtupleComponent {
  public:
    CaloCellNtComponent(NtupleAlgorithm *mainAlg,
			NtComponentParameters parameters);
    virtual ~CaloCellNtComponent();
    
    StatusCode setupServices(void);
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_CaloCellContName;

    /** For access to the tile test beam identifiers */
    const TileID* m_tileID;

	// to filter cells by noise significance
	//ToolHandle<ICaloNoiseTool> m_caloNoiseTool;

	bool m_useTile;
	bool m_useLarg;
	int m_caloDetail;

    // NTUPLE variables
    int m_ncell;
    std::vector<float>* m_energy;
    std::vector<float>* m_time;
    std::vector<float>* m_eta;
    std::vector<float>* m_phi;
    std::vector<float>* m_quality;
    std::vector<int>* m_type;
    std::vector<int>* m_side;
    std::vector<int>* m_module;
    std::vector<int>* m_tower;
    std::vector<int>* m_sample;
    std::vector<float>* m_x;
    std::vector<float>* m_y;
    std::vector<float>* m_z;
  };
}

#endif
