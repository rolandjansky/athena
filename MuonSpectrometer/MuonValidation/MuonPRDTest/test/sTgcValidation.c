/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"

#include "sTgcValidation.h"
#include "NSWHitsTree.h"
#include "ValidationManager.h"

#include <iostream>


sTgcVal::sTgcVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name),sTgcHitHistograms() {
   SetDirectory(m_step_directory); 
}

sTgcVal::~sTgcVal()  {}

ValidationStep* sTgcVal::hasNextStep(const char* /*stationName*/, int stationEta, int /*stationPhi*/, int /*multilayer*/, int /*layer*/) {
    const char* sd = (stationEta<0)? "C" : "A";
    TString next_name = TString::Format("%s_side%s",this->name(),sd);
    ValidationStep* next = this->Contains(next_name.Data());

    // Create if not exist
    if (next==0) {
        next = STGC_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
        this->AddStep(next);
    }

    return next;
}

void sTgcVal::ProcessData(NSWHitsTree& tree, const int /*hit_number*/) {
    for (unsigned int nTgcHit=0; nTgcHit < tree.Hits_sTGC_n; nTgcHit++) {
        if ( hasHistos() ) Fill(tree,nTgcHit);
        ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(nTgcHit).c_str(),
                                           tree.Hits_sTGC_off_stationEta->at(nTgcHit),
                                           tree.Hits_sTGC_off_stationPhi->at(nTgcHit),
                                           tree.Hits_sTGC_off_multiplet->at(nTgcHit),
                                           tree.Hits_sTGC_off_gas_gap->at(nTgcHit) );

        if (next != 0) next->ProcessData(tree,nTgcHit);
    }
}

bool sTgcVal::Check()  { return true; }



// **  sTgcSideVal implementation
sTgcSideVal::sTgcSideVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name), sTgcHitHistograms() {
   SetDirectory(m_step_directory); 
}

sTgcSideVal::~sTgcSideVal()  {}

ValidationStep* sTgcSideVal::hasNextStep(const char* stationName, int /*stationEta*/, int stationPhi, int /*multilayer*/, int /*layer*/) {
    const char* ch = (stationName[2]=='S')? "Small" : "Large";
    TString next_name = TString::Format("%s_sector%d_%s",this->name(),stationPhi,ch);
    ValidationStep* next = this->Contains(next_name.Data());

    // Create if not exist
    if (next==0) {
        next = STGC_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
        this->AddStep(next);
    }

    return next;
}

void sTgcSideVal::ProcessData(NSWHitsTree& tree, const int /*hit_number*/) {
    
    for (unsigned int nTgcHit=0; nTgcHit < tree.Hits_sTGC_n; nTgcHit++) {
        if ( hasHistos() ) Fill (tree,nTgcHit);
        ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(nTgcHit).c_str(),
                                           tree.Hits_sTGC_off_stationEta->at(nTgcHit),
                                           tree.Hits_sTGC_off_stationPhi->at(nTgcHit),
                                           tree.Hits_sTGC_off_multiplet->at(nTgcHit),
                                           tree.Hits_sTGC_off_gas_gap->at(nTgcHit) );

        if (next != 0) next->ProcessData(tree,nTgcHit);
    }
}

bool sTgcSideVal::Check()  { return true; }



// ** sTgcHitSectorVal  implementation
sTgcHitSectorVal::sTgcHitSectorVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name), sTgcHitHistograms() {
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

sTgcHitSectorVal::~sTgcHitSectorVal()  {}

ValidationStep* sTgcHitSectorVal::hasNextStep(const char* stationName, int stationEta, int /*stationPhi*/, int /*multilayer*/, int /*layer*/) {

    TString next_name = TString::Format("%s_module%s%d",this->name(),stationName,abs(stationEta));
    ValidationStep* next = this->Contains(next_name.Data());

    // Create if not exist
    if (next==0) {
        next = STGC_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
       this->AddStep(next);
    }

    return next;
}

void sTgcHitSectorVal::ProcessData(NSWHitsTree& tree, const int hit_number) {
    if (hit_number!=-1) {
         if  ( hasHistos() ) Fill(tree,hit_number);
         ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(hit_number).c_str(),
                                           tree.Hits_sTGC_off_stationEta->at(hit_number),
                                           tree.Hits_sTGC_off_stationPhi->at(hit_number),
                                           tree.Hits_sTGC_off_multiplet->at(hit_number),
                                           tree.Hits_sTGC_off_gas_gap->at(hit_number) );

        if (next != 0) next->ProcessData(tree,hit_number);
    } else {
        for (unsigned int nTgcHit=0; nTgcHit < tree.Hits_sTGC_n; nTgcHit++) {
            if  ( hasHistos() ) Fill(tree,nTgcHit);
            ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(nTgcHit).c_str(),
                                               tree.Hits_sTGC_off_stationEta->at(nTgcHit),
                                               tree.Hits_sTGC_off_stationPhi->at(nTgcHit),
                                               tree.Hits_sTGC_off_multiplet->at(nTgcHit),
                                               tree.Hits_sTGC_off_gas_gap->at(nTgcHit) );

            if (next != 0) next->ProcessData(tree);
        }
    }
}

bool sTgcHitSectorVal::Check()  { return true; }




// ** sTgcHitModuleVal  implementation
sTgcHitModuleVal::sTgcHitModuleVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name),sTgcHitHistograms() {
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

sTgcHitModuleVal::~sTgcHitModuleVal()  {}

ValidationStep* sTgcHitModuleVal::hasNextStep(const char* /*stationName*/, int /*stationEta*/, int /*stationPhi*/, int multilayer, int layer) {

    TString next_name = TString::Format("%s_sub_m%d_l%d",this->name(),multilayer,layer);
    ValidationStep* next = this->Contains(next_name.Data());
    
    // Create if not exist
    if (next==0) {
        next = STGC_STEP_FACTORY(next_name,this->StepDirectory());
        next = ValidationManager::instance()->insert(next);
       this->AddStep(next);
    }

    return next;
}

void sTgcHitModuleVal::ProcessData(NSWHitsTree& tree,const int hit_number) {
    if (hit_number!=-1) {
        if ( hasHistos() ) Fill(tree,hit_number);
        ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(hit_number).c_str(),
                                           tree.Hits_sTGC_off_stationEta->at(hit_number),
                                           tree.Hits_sTGC_off_stationPhi->at(hit_number),
                                           tree.Hits_sTGC_off_multiplet->at(hit_number),
                                           tree.Hits_sTGC_off_gas_gap->at(hit_number) );

        if (next != 0) next->ProcessData(tree,hit_number);
    } else {
        for (unsigned int nTgcHit=0; nTgcHit < tree.Hits_sTGC_n; nTgcHit++) {
            if ( hasHistos() ) Fill(tree,nTgcHit);
            ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(nTgcHit).c_str(),
                                           tree.Hits_sTGC_off_stationEta->at(nTgcHit),
                                           tree.Hits_sTGC_off_stationPhi->at(nTgcHit),
                                           tree.Hits_sTGC_off_multiplet->at(nTgcHit),
                                           tree.Hits_sTGC_off_gas_gap->at(nTgcHit) );

            if (next != 0) next->ProcessData(tree);
        }
    }
}

bool sTgcHitModuleVal::Check()  { return true; }



// ** sTgcHitSubStructureVal  implementation
sTgcHitSubStructureVal::sTgcHitSubStructureVal( TDirectory* current_dir, const char* name ) : ValidationStep (current_dir,name),sTgcHitHistograms() {
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

sTgcHitSubStructureVal::~sTgcHitSubStructureVal()  {}

ValidationStep* sTgcHitSubStructureVal::hasNextStep(const char* /*stationName*/, int /*stationEta*/, int /*stationPhi*/, int /*multilayer*/, int /*layer*/) {
    return 0;
}

void sTgcHitSubStructureVal::ProcessData(NSWHitsTree& tree,const int hit_number) {
    if (hit_number!=-1) {
        if ( hasHistos() ) Fill(tree,hit_number);
        //ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(hit_number).c_str(),
        //                                   tree.Hits_sTGC_off_stationEta->at(hit_number),
        //                                   tree.Hits_sTGC_off_stationPhi->at(hit_number),
        //                                   tree.Hits_sTGC_off_multiplet->at(hit_number),
        //                                   tree.Hits_sTGC_off_gas_gap->at(hit_number) );

        //if (next != 0) next->ProcessData(tree,hit_number);
    } else {
        for (unsigned int nTgcHit=0; nTgcHit < tree.Hits_sTGC_n; nTgcHit++) {
            if ( hasHistos() ) Fill(tree,nTgcHit);
            //ValidationStep* next = hasNextStep(tree.Hits_sTGC_off_stationName->at(nTgcHit).c_str(),
            //                               tree.Hits_sTGC_off_stationEta->at(nTgcHit),
            //                               tree.Hits_sTGC_off_stationPhi->at(nTgcHit),
            //                               tree.Hits_sTGC_off_multiplet->at(nTgcHit),
            //                               tree.Hits_sTGC_off_gas_gap->at(nTgcHit) );

            //if (next != 0) next->ProcessData(tree);
        }
    }
}

bool sTgcHitSubStructureVal::Check()  { return true; }



// **  Histograms for the sTgc hit validation
sTgcHitHistograms::sTgcHitHistograms() : m_has_histos(false),m_xy_gpos(0),m_xy_phiRef_gpos(0),m_rz_gpos(0),m_xy_det_gpos(0) {}
sTgcHitHistograms::~sTgcHitHistograms() {}

void sTgcHitHistograms::SetDirectory(TDirectory* dir) {
    m_histo_directory = dir;
}

bool sTgcHitHistograms::hasHistos() const { return m_has_histos; }

TH2F* sTgcHitHistograms::CreateH(const char* name, int binx, double xmin, double xmax, int biny, double ymin, double ymax) {
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
        std::cout << "sTgcHitHistogams::CreateH , histo name " << name << " not recognized! Likely it will crash ..." << std::endl;
    }
    return 0;
}

void sTgcHitHistograms::Fill(NSWHitsTree& tree,const int TgcHit_number) {
    //int hit_start = (TgcHit_number==-1)? 0                : TgcHit_number;
    //int hit_stop  = (TgcHit_number==-1)? tree.Hits_sTGC_n : TgcHit_number+1;

    //for (unsigned int nTgcHit=hit_start; nTgcHit < hit_stop; nTgcHit++) {
    if (m_xy_gpos) m_xy_gpos->Fill(tree.Hits_sTGC_hitGlobalPositionX->at(TgcHit_number),tree.Hits_sTGC_hitGlobalPositionY->at(TgcHit_number));
    if (m_xy_det_gpos) m_xy_det_gpos->Fill(tree.Hits_sTGC_detector_globalPositionX->at(TgcHit_number),tree.Hits_sTGC_detector_globalPositionY->at(TgcHit_number));
    if (m_xy_phiRef_gpos) {
        double theta = M_PI/2. - tree.Hits_sTGC_detector_globalPositionP->at(TgcHit_number);
        double x_ref = tree.Hits_sTGC_hitGlobalPositionX->at(TgcHit_number)*cos(theta) - tree.Hits_sTGC_hitGlobalPositionY->at(TgcHit_number)*sin(theta);
        double y_ref = tree.Hits_sTGC_hitGlobalPositionX->at(TgcHit_number)*sin(theta) + tree.Hits_sTGC_hitGlobalPositionY->at(TgcHit_number)*cos(theta);
        m_xy_phiRef_gpos->Fill(x_ref,y_ref);
    }
    if (m_rz_gpos) {
        double theta = M_PI/2. - tree.Hits_sTGC_detector_globalPositionP->at(TgcHit_number);
        //double x_ref = tree.Hits_sTGC_hitGlobalPositionX->at(TgcHit_number)*cos(theta) - tree.Hits_sTGC_hitGlobalPositionY->at(TgcHit_number)*sin(theta);
        double y_ref = tree.Hits_sTGC_hitGlobalPositionX->at(TgcHit_number)*sin(theta) + tree.Hits_sTGC_hitGlobalPositionY->at(TgcHit_number)*cos(theta);
        m_rz_gpos->Fill(tree.Hits_sTGC_hitGlobalPositionZ->at(TgcHit_number),y_ref);
    }

    //}
}


// **  Step factory implementation
ValidationStep* STGC_STEP_FACTORY(const char* name, TDirectory* dir) {
    if (dir==0) dir = gDirectory;
    TString Name(name);
    if ( strcmp(name,"sTgcHit")==0  )                       return (new sTgcVal(dir,name));
    if ( Name.Length() < 14 && Name.Contains("side") )      return (new sTgcSideVal(dir,name));
    if ( Name.Length() < 29 && Name.Contains("sector") )    return (new sTgcHitSectorVal(dir,name));
    if ( Name.Length() < 41 && Name.Contains("module") )    return (new sTgcHitModuleVal(dir,name));
    if ( Name.Length() > 46 && Name.Contains("sub_m") )     return (new sTgcHitSubStructureVal(dir,name));

    std::cout << "STEP_FACTORY: setp " << name << " not known to factory. No ValidationStep produced." << std::endl;
    return 0;
}   
// **  End of Step factory implementation

