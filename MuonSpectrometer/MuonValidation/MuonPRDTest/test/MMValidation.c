/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"

#include "MMValidation.h"
#include "NSWHitsTree.h"
#include "ValidationManager.h"

#include <iostream>


MicroMegasVal::MicroMegasVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name),MicroMegasHitHistograms() {
   SetDirectory(m_step_directory); 
}

MicroMegasVal::~MicroMegasVal()  {}

ValidationStep* MicroMegasVal::hasNextStep(const char* /*stationName*/, int stationEta, int /*stationPhi*/, int /*multilayer*/, int /*layer*/) {
    const char* sd = (stationEta<0)? "C" : "A";
    TString next_name = TString::Format("%s_side%s",this->name(),sd);
    ValidationStep* next = this->Contains(next_name.Data());

    // Create if not exist
    if (next==0) {
        next = MM_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
        this->AddStep(next);
    }

    return next;
}

void MicroMegasVal::ProcessData(NSWHitsTree& tree, const int /*hit_number*/) {
    for (unsigned int nMMHit=0; nMMHit < tree.Hits_MM_n; nMMHit++) {
        if ( hasHistos() ) Fill(tree,nMMHit);
        ValidationStep* next = hasNextStep(tree.Hits_MM_off_stationName->at(nMMHit).c_str(),
                                           tree.Hits_MM_off_stationEta->at(nMMHit),
                                           tree.Hits_MM_off_stationPhi->at(nMMHit),
                                           tree.Hits_MM_off_multiplet->at(nMMHit),
                                           tree.Hits_MM_off_gas_gap->at(nMMHit) );

        if (next != 0) next->ProcessData(tree,nMMHit);
    }
}

bool MicroMegasVal::Check()  { return true; }



// **  MicroMegasSideVal implementation
MicroMegasSideVal::MicroMegasSideVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name), MicroMegasHitHistograms() {
   SetDirectory(m_step_directory); 
}

MicroMegasSideVal::~MicroMegasSideVal()  {}

ValidationStep* MicroMegasSideVal::hasNextStep(const char* stationName, int /*stationEta*/, int stationPhi, int /*multilayer*/, int /*layer*/) {
    const char* ch = (stationName[2]=='S')? "Small" : "Large";
    TString next_name = TString::Format("%s_sector%d_%s",this->name(),stationPhi,ch);
    ValidationStep* next = this->Contains(next_name.Data());

    // Create if not exist
    if (next==0) {
        next = MM_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
        this->AddStep(next);
    }

    return next;
}

void MicroMegasSideVal::ProcessData(NSWHitsTree& tree, const int /*hit_number*/) {
    
    for (unsigned int nMMHit=0; nMMHit < tree.Hits_MM_n; nMMHit++) {
        if ( hasHistos() ) Fill (tree,nMMHit);
        ValidationStep* next = hasNextStep(tree.Hits_MM_off_stationName->at(nMMHit).c_str(),
                                           tree.Hits_MM_off_stationEta->at(nMMHit),
                                           tree.Hits_MM_off_stationPhi->at(nMMHit),
                                           tree.Hits_MM_off_multiplet->at(nMMHit),
                                           tree.Hits_MM_off_gas_gap->at(nMMHit) );

        if (next != 0) next->ProcessData(tree,nMMHit);
    }
}

bool MicroMegasSideVal::Check()  { return true; }



// ** MicroMegasHitSectorVal  implementation
MicroMegasHitSectorVal::MicroMegasHitSectorVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name), MicroMegasHitHistograms() {
   SetDirectory(m_step_directory);
   CreateH(TString::Format("%s_xy_gpos",this->name()).Data());
   CreateH(TString::Format("%s_xy_phiRef_gpos",this->name()).Data(),150,-1500.,1500.,250,0,5000.);
   if ( m_name.Contains("sideA") ) {
       if ( m_name.Contains("Small") ) CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, 7000., 7400.,250,0.,5000.);
       else  CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, 7350., 7750.,250,0.,5000.);
   } else {
       if ( m_name.Contains("Small") ) CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400,-7400.,-7000.,250,0.,5000.);
       else  CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, -7750., -7350.,250,0.,5000.);
   }
   CreateH(TString::Format("%s_xy_det_gpos",this->name()).Data());
}

MicroMegasHitSectorVal::~MicroMegasHitSectorVal()  {}

ValidationStep* MicroMegasHitSectorVal::hasNextStep(const char* stationName, int stationEta, int /*stationPhi*/, int /*multilayer*/, int /*layer*/) {

    TString next_name = TString::Format("%s_module%s%d",this->name(),stationName,abs(stationEta));
    ValidationStep* next = this->Contains(next_name.Data());

    // Create if not exist
    if (next==0) {
        next = MM_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
       this->AddStep(next);
    }

    return next;
}

void MicroMegasHitSectorVal::ProcessData(NSWHitsTree& tree, const int hit_number) {
    if (hit_number!=-1) {
         if  ( hasHistos() ) Fill(tree,hit_number);
         ValidationStep* next = hasNextStep(tree.Hits_MM_off_stationName->at(hit_number).c_str(),
                                           tree.Hits_MM_off_stationEta->at(hit_number),
                                           tree.Hits_MM_off_stationPhi->at(hit_number),
                                           tree.Hits_MM_off_multiplet->at(hit_number),
                                           tree.Hits_MM_off_gas_gap->at(hit_number) );

        if (next != 0) next->ProcessData(tree,hit_number);
    } else {
        for (unsigned int nMMHit=0; nMMHit < tree.Hits_MM_n; nMMHit++) {
            if  ( hasHistos() ) Fill(tree,nMMHit);
            ValidationStep* next = hasNextStep(tree.Hits_MM_off_stationName->at(nMMHit).c_str(),
                                               tree.Hits_MM_off_stationEta->at(nMMHit),
                                               tree.Hits_MM_off_stationPhi->at(nMMHit),
                                               tree.Hits_MM_off_multiplet->at(nMMHit),
                                               tree.Hits_MM_off_gas_gap->at(nMMHit) );

            if (next != 0) next->ProcessData(tree);
        }
    }
}

bool MicroMegasHitSectorVal::Check()  { return true; }




// ** MicroMegasHitModuleVal  implementation
MicroMegasHitModuleVal::MicroMegasHitModuleVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name),MicroMegasHitHistograms() {
   SetDirectory(m_step_directory); 
   CreateH(TString::Format("%s_xy_gpos",this->name()).Data());
   CreateH(TString::Format("%s_xy_phiRef_gpos",this->name()).Data(),50,-500.,500.);
   if ( m_name.Contains("sideA") ) {
       if ( m_name.Contains("Small") ) CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, 7000., 7400.,250,0.,5000.);
       else CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, 7350., 7750.,250,0.,5000.);
   } else {
       if ( m_name.Contains("Small") ) CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400,-7400.,-7000.,250,0.,5000.);
       else CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, -7750., -7350.,250,0.,5000.);
   }


}

MicroMegasHitModuleVal::~MicroMegasHitModuleVal()  {}

ValidationStep* MicroMegasHitModuleVal::hasNextStep(const char* /*stationName*/, int /*stationEta*/, int /*stationPhi*/, int multilayer, int layer) {

    TString next_name = TString::Format("%s_sub_m%d_l%d",this->name(),multilayer,layer);
    ValidationStep* next = this->Contains(next_name.Data());
    
    // Create if not exist
    if (next==0) {
        next = MM_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
       this->AddStep(next);
    }

    return next;
}

void MicroMegasHitModuleVal::ProcessData(NSWHitsTree& tree,const int hit_number) {
    if (hit_number!=-1) {
        if ( hasHistos() ) Fill(tree,hit_number);
        ValidationStep* next = hasNextStep(tree.Hits_MM_off_stationName->at(hit_number).c_str(),
                                           tree.Hits_MM_off_stationEta->at(hit_number),
                                           tree.Hits_MM_off_stationPhi->at(hit_number),
                                           tree.Hits_MM_off_multiplet->at(hit_number),
                                           tree.Hits_MM_off_gas_gap->at(hit_number) );

        if (next != 0) next->ProcessData(tree,hit_number);
    } else {
        for (unsigned int nMMHit=0; nMMHit < tree.Hits_MM_n; nMMHit++) {
            if ( hasHistos() ) Fill(tree,nMMHit);
            ValidationStep* next = hasNextStep(tree.Hits_MM_off_stationName->at(nMMHit).c_str(),
                                           tree.Hits_MM_off_stationEta->at(nMMHit),
                                           tree.Hits_MM_off_stationPhi->at(nMMHit),
                                           tree.Hits_MM_off_multiplet->at(nMMHit),
                                           tree.Hits_MM_off_gas_gap->at(nMMHit) );

            if (next != 0) next->ProcessData(tree);
        }
    }
}

bool MicroMegasHitModuleVal::Check()  { return true; }



// ** MicroMegasHitSubStructureVal  implementation
MicroMegasHitSubStructureVal::MicroMegasHitSubStructureVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name),MicroMegasHitHistograms() {
   SetDirectory(m_step_directory); 
   CreateH(TString::Format("%s_xy_gpos",this->name()).Data());
   CreateH(TString::Format("%s_xy_phiRef_gpos",this->name()).Data(),50,-500.,500.);
   if ( m_name.Contains("sideA") ) {
       if ( m_name.Contains("Small") ) CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, 7000., 7400.,250,0.,5000.);
       else  CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, 7350., 7750.,250,0.,5000.);
   } else {
       if ( m_name.Contains("Small") ) CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400,-7400.,-7000.,250,0.,5000.);
       else  CreateH(TString::Format("%s_rz_gpos",this->name()).Data(),400, -7750., -7350.,250,0.,5000.);
   }


}

MicroMegasHitSubStructureVal::~MicroMegasHitSubStructureVal()  {}

ValidationStep* MicroMegasHitSubStructureVal::hasNextStep(const char* /*stationName*/, int /*stationEta*/, int /*stationPhi*/, int /*multilayer*/, int /*layer*/) {
    return 0;
}

void MicroMegasHitSubStructureVal::ProcessData(NSWHitsTree& tree,const int hit_number) {
    if (hit_number!=-1) {
        if ( hasHistos() ) Fill(tree,hit_number);
    } else {
        for (unsigned int nMMHit=0; nMMHit < tree.Hits_MM_n; nMMHit++) {
            if ( hasHistos() ) Fill(tree,nMMHit);
        }
    }
}

bool MicroMegasHitSubStructureVal::Check()  { return true; }



// **  Histograms for the MicroMegas hit validation
MicroMegasHitHistograms::MicroMegasHitHistograms() : m_has_histos(false),m_xy_gpos(0),m_xy_phiRef_gpos(0),m_rz_gpos(0),m_xy_det_gpos(0) {}
MicroMegasHitHistograms::~MicroMegasHitHistograms() {}

void MicroMegasHitHistograms::SetDirectory(TDirectory* dir) {
    m_histo_directory = dir;
}

bool MicroMegasHitHistograms::hasHistos() const { return m_has_histos; }

TH2F* MicroMegasHitHistograms::CreateH(const char* name, int binx, double xmin, double xmax, int biny, double ymin, double ymax) {
    TString Name(name);
    if ( Name.Contains("xy_gpos") ) {
        m_xy_gpos = new TH2F(name,"x-y hit global position",binx,xmin,xmax,biny,ymin,ymax);
        m_xy_gpos->SetDirectory(m_histo_directory);
        m_histo_2d.push_back(m_xy_gpos);
        m_has_histos = true;
        return m_xy_gpos;
    } else if ( Name.Contains("xy_phiRef_gpos") ) {
        m_xy_phiRef_gpos = new TH2F(name,"x-y reference hit global position",binx,xmin,xmax,biny,ymin,ymax);
        m_xy_phiRef_gpos->SetDirectory(m_histo_directory);
        m_histo_2d.push_back(m_xy_phiRef_gpos);
        m_has_histos = true;
        return m_xy_phiRef_gpos;
    } else if ( Name.Contains("rz_gpos") ) {
        m_rz_gpos = new TH2F(name,"r-z hit global position",binx,xmin,xmax,biny,ymin,ymax);
        m_rz_gpos->SetDirectory(m_histo_directory);
        m_histo_2d.push_back(m_rz_gpos);
        m_has_histos = true;
        return m_rz_gpos;
    } else if ( Name.Contains("xy_det_gpos") ) {
        m_xy_det_gpos = new TH2F(name,"x-y detector global position",binx,xmin,xmax,biny,ymin,ymax);
        m_xy_det_gpos->SetDirectory(m_histo_directory);
        m_histo_2d.push_back( m_xy_det_gpos );
        m_has_histos = true;
    } else {
        std::cout << "MicroMegasHitHistogams::CreateH , histo name " << name << " not recognized! Likely it will crash ..." << std::endl;
    }
    return 0;
}

void MicroMegasHitHistograms::Fill(NSWHitsTree& tree,const int MMHit_number) {
    if (m_xy_gpos) m_xy_gpos->Fill(tree.Hits_MM_hitGlobalPositionX->at(MMHit_number),tree.Hits_MM_hitGlobalPositionY->at(MMHit_number));
    if (m_xy_det_gpos) m_xy_det_gpos->Fill(tree.Hits_MM_detector_globalPositionX->at(MMHit_number),tree.Hits_MM_detector_globalPositionY->at(MMHit_number));
    if (m_xy_phiRef_gpos) {
        double theta = M_PI/2. - tree.Hits_MM_detector_globalPositionP->at(MMHit_number);
        double x_ref = tree.Hits_MM_hitGlobalPositionX->at(MMHit_number)*cos(theta) - tree.Hits_MM_hitGlobalPositionY->at(MMHit_number)*sin(theta);
        double y_ref = tree.Hits_MM_hitGlobalPositionX->at(MMHit_number)*sin(theta) + tree.Hits_MM_hitGlobalPositionY->at(MMHit_number)*cos(theta);
        m_xy_phiRef_gpos->Fill(x_ref,y_ref);
    }
    if (m_rz_gpos) {
        double theta = M_PI/2. - tree.Hits_MM_detector_globalPositionP->at(MMHit_number);
        double y_ref = tree.Hits_MM_hitGlobalPositionX->at(MMHit_number)*sin(theta) + tree.Hits_MM_hitGlobalPositionY->at(MMHit_number)*cos(theta);
        m_rz_gpos->Fill(tree.Hits_MM_hitGlobalPositionZ->at(MMHit_number),y_ref);
    }
}


// **  Step factory implementation
ValidationStep* MM_STEP_FACTORY(const char* name, TDirectory* dir) {
    if (dir==0) dir = gDirectory;
    TString Name(name);
    if ( strcmp(name,"MMHit")==0  )                       return (new MicroMegasVal(dir,name));
    if ( Name.Length() < 12 && Name.Contains("side") )      return (new MicroMegasSideVal(dir,name));
    if ( Name.Length() < 27 && Name.Contains("sector") )    return (new MicroMegasHitSectorVal(dir,name));
    if ( Name.Length() < 39 && Name.Contains("module") )    return (new MicroMegasHitModuleVal(dir,name));
    if ( Name.Length() > 44 && Name.Contains("sub_m") )     return (new MicroMegasHitSubStructureVal(dir,name));

    std::cout << "STEP_FACTORY: setp " << name << " not known to factory. No ValidationStep produced." << std::endl;
    return 0;
}   
// **  End of Step factory implementation

