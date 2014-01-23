/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCVALIDATION_H
#define STGCVALIDATION_H


#include "ValidationStep.h"



class NSWHitsTree;

class sTgcHitHistograms {

    private:
    TDirectory* m_histo_directory;


    public:
    sTgcHitHistograms();
    ~sTgcHitHistograms();

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


class sTgcHitSubStructureVal: public ValidationStep, public sTgcHitHistograms {

    public:
    sTgcHitSubStructureVal( TDirectory* current_dir, const char* name );
    ~sTgcHitSubStructureVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree,const int hit_number=-1);

    bool Check();
};


class sTgcHitModuleVal: public ValidationStep, public sTgcHitHistograms {

    public:
    sTgcHitModuleVal( TDirectory* current_dir, const char* name );
    ~sTgcHitModuleVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree,const int hit_number=-1);

    bool Check();
};


class sTgcHitSectorVal: public ValidationStep, public sTgcHitHistograms {

    public:
    sTgcHitSectorVal( TDirectory* current_dir, const char* name );
    ~sTgcHitSectorVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree,const int hit_number=-1);

    bool Check();
};


class sTgcSideVal: public ValidationStep, public sTgcHitHistograms {

    public:
    sTgcSideVal( TDirectory* current_dir, const char* name );
    ~sTgcSideVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree, const int hit_number=-1);

    bool Check();
};



class sTgcVal: public ValidationStep, public sTgcHitHistograms {

    public:
    sTgcVal( TDirectory* current_dir, const char* name );
    ~sTgcVal();

    ValidationStep* hasNextStep(const char* stationName, int stationEta, int stationPhi, int multilayer, int layer);
    void ProcessData(NSWHitsTree& tree, const int hit_number=-1);

    bool Check();
};



ValidationStep* STGC_STEP_FACTORY(const char* name, TDirectory* dir = 0);

#endif
