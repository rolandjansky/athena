//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4HadronPhysicsFTFP_BERT.hh 76703 2013-11-14 10:29:11Z gcosmo $
//
//---------------------------------------------------------------------------
// Author: Alberto Ribon
// Date:   October 2017
//
// Hadron physics for the new physics list FTFP_BERT_BIC_ATL.
// This is a modified version of the FTFP_BERT_ATL physics list for ATLAS,
// which uses Binary Cascade (BIC) for proton and neutron below 5 GeV
// and pions below 1.3 GeV.
//----------------------------------------------------------------------------
//
#ifndef G4HadronPhysicsFTFP_BERT_BIC_ATL_h
#define G4HadronPhysicsFTFP_BERT_BIC_ATL_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4PionBuilder.hh"
#include "G4BertiniPionBuilder.hh"
#include "G4FTFPPionBuilder.hh"
#include "G4BinaryPionBuilder.hh"

#include "G4KaonBuilder.hh"
#include "G4BertiniKaonBuilder.hh"
#include "G4FTFPKaonBuilder.hh"

#include "G4ProtonBuilder.hh"
#include "G4BertiniProtonBuilder.hh"
#include "G4FTFPProtonBuilder.hh"
#include "G4BinaryProtonBuilder.hh"

#include "G4NeutronBuilder.hh"
#include "G4BertiniNeutronBuilder.hh"
#include "G4FTFPNeutronBuilder.hh"
#include "G4BinaryNeutronBuilder.hh"

#include "G4HyperonFTFPBuilder.hh"
#include "G4AntiBarionBuilder.hh"
#include "G4FTFPAntiBarionBuilder.hh"

class G4HadronPhysicsFTFP_BERT_BIC_ATL : public G4VPhysicsConstructor
{
  public: 
    G4HadronPhysicsFTFP_BERT_BIC_ATL(G4int verbose =1);
    G4HadronPhysicsFTFP_BERT_BIC_ATL(const G4String& name, G4bool quasiElastic=false);
    virtual ~G4HadronPhysicsFTFP_BERT_BIC_ATL();

  public: 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  private:
    void CreateModels();
    G4bool QuasiElastic;

    // Simplify handling of TLS data, encapsulate everyhing in a structure
    struct ThreadPrivate { 
      G4NeutronBuilder * theNeutrons;
      G4BertiniNeutronBuilder * theBertiniNeutron;
      G4FTFPNeutronBuilder * theFTFPNeutron;
      G4BinaryNeutronBuilder * theBinaryNeutron;

      G4PionBuilder * thePion;
      G4BertiniPionBuilder * theBertiniPion;
      G4FTFPPionBuilder * theFTFPPion;
      G4BinaryPionBuilder * theBinaryPion;

      G4KaonBuilder * theKaon;
      G4BertiniKaonBuilder * theBertiniKaon;
      G4FTFPKaonBuilder * theFTFPKaon;

      G4ProtonBuilder * thePro;
      G4BertiniProtonBuilder * theBertiniPro;
      G4FTFPProtonBuilder * theFTFPPro;    
      G4BinaryProtonBuilder * theBinaryPro;
    
      G4HyperonFTFPBuilder * theHyperon;
    
      G4AntiBarionBuilder * theAntiBaryon;
      G4FTFPAntiBarionBuilder * theFTFPAntiBaryon;

      G4VCrossSectionDataSet * ChipsKaonMinus;
      G4VCrossSectionDataSet * ChipsKaonPlus;
      G4VCrossSectionDataSet * ChipsKaonZero;
      G4VCrossSectionDataSet * xsNeutronInelasticXS;
      G4VCrossSectionDataSet * xsNeutronCaptureXS;
    };
    static G4ThreadLocal ThreadPrivate* tpdata;
};

#endif

