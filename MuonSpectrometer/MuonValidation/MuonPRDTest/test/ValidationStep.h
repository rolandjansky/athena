/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALIDATIONSTEP_H
#define VALIDATIONSTEP_H

#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TDirectory.h"

#include <stdint.h>
#include <vector>


using namespace std;

class NSWHitsTree;

class ValidationStep {

    public:

    ValidationStep( TDirectory* current_dir, const char* name );
    ~ValidationStep();

    const char* name() const;
    TDirectory* StepDirectory() const;

    void AddStep(ValidationStep* next);

    virtual void ProcessData(NSWHitsTree& tree, const int element_number=-1) = 0;


    protected:
    ValidationStep* Contains(const char* name);

    virtual ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int Multilayer, int Layer) = 0;

    virtual bool Check() = 0;

    TString     m_name;
    TDirectory* m_step_directory;

    vector< ValidationStep* > m_next_steps;
};


#endif
