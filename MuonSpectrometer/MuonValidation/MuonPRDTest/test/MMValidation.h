/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMVALIDATION_H
#define MMVALIDATION_H


#include "ValidationStep.h"



class NSWHitsTree;

class MicroMegasHitHistograms {

    private:
    TDirectory* m_histo_directory;


    public:
    MicroMegasHitHistograms();
    ~MicroMegasHitHistograms();

    void SetDirectory(TDirectory* dir);
    bool hasHistos() const;

    // Factory for the validation histogram; handle the creation of both 1D and 2D histograms
    TH2F* CreateH(const char* name, int binx=500, double xmin=-5000., double xmax=5000., int biny=500, double ymin=-5000., double ymax=5000.);

    // Fill method
    void Fill(NSWHitsTree& tree,const int TgcHit_number=-1);

    bool m_has_histos;

    // define validation histograms
    TH2F* m_xy_gpos;
    TH2F* m_xy_phiRef_gpos;
    TH2F* m_rz_gpos;
    TH2F* m_xy_det_gpos;

    // collect pointer of all histograms
    vector<TH1F*> m_histo_1d;
    vector<TH2F*> m_histo_2d;
};


class MicroMegasHitSubStructureVal: public ValidationStep, public MicroMegasHitHistograms {

    public:
    MicroMegasHitSubStructureVal( TDirectory* current_dir, const char* name );
    ~MicroMegasHitSubStructureVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree,const int hit_number=-1);

    bool Check();
};


class MicroMegasHitModuleVal: public ValidationStep, public MicroMegasHitHistograms {

    public:
    MicroMegasHitModuleVal( TDirectory* current_dir, const char* name );
    ~MicroMegasHitModuleVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree,const int hit_number=-1);

    bool Check();
};


class MicroMegasHitSectorVal: public ValidationStep, public MicroMegasHitHistograms {

    public:
    MicroMegasHitSectorVal( TDirectory* current_dir, const char* name );
    ~MicroMegasHitSectorVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree,const int hit_number=-1);

    bool Check();
};


class MicroMegasSideVal: public ValidationStep, public MicroMegasHitHistograms {

    public:
    MicroMegasSideVal( TDirectory* current_dir, const char* name );
    ~MicroMegasSideVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree, const int hit_number=-1);

    bool Check();
};



class MicroMegasVal: public ValidationStep, public MicroMegasHitHistograms {

    public:
    MicroMegasVal( TDirectory* current_dir, const char* name );
    ~MicroMegasVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree, const int hit_number=-1);

    bool Check();
};



ValidationStep* MM_STEP_FACTORY(const char* name, TDirectory* dir = 0);

#endif
