/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepRecorder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/MaterialStepRecorder.h"
#include "TrkGeometry/MaterialStep.h"
#include <iostream>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "SimHelpers/ServiceAccessor.h"
#include "StoreGate/StoreGateSvc.h"
#include "G4Step.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include <climits>
#include <cmath>

namespace G4UA{


  MaterialStepRecorder::MaterialStepRecorder():
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"MaterialStepRecorder"),
    m_evtStore("StoreGateSvc/StoreGateSvc","MaterialStepRecorder"), //FIXME name should be passed in via a Config struct rather than hardcoded.
    m_detStore("StoreGateSvc/DetectorStore","MaterialStepRecorder"), //FIXME name should be passed in via a Config struct rather than hardcoded.
    m_matStepCollection(nullptr),
    m_matStepCollectionName("MaterialStepRecords"), //FIXME should be passed in via a Config struct rather than hardcoded.
    m_recordComposition(true), //FIXME should be passed in via a Config struct rather than hardcoded.
    m_totalNbOfAtoms(0),
    m_totalSteps(0),
    m_eventID(0),
    m_elementTable(nullptr),
    m_elementTableName("ElementTable"),
    m_runElementTable(nullptr)
  {
  }

  void MaterialStepRecorder::BeginOfEventAction(const G4Event*){


    ATH_MSG_DEBUG(" BeginOfEventAction");

    // create a new Collection
    m_matStepCollection = new Trk::MaterialStepCollection;

    //    m_eventStepLength = 0;

  }

  void MaterialStepRecorder::EndOfEventAction(const G4Event*){

    ATH_MSG_DEBUG(" EndOfEventAction");

    ++m_eventID;
    // write the Collection to StoreGate
    if(m_evtStore->record(m_matStepCollection, m_matStepCollectionName, false).isFailure())
      {
        ATH_MSG_ERROR("cannot record step collection to StoreGate");
      }
    // write out the ElementTable of this event
    if ( m_evtStore->record(m_elementTable, m_elementTableName, false).isFailure() )
      {
        ATH_MSG_ERROR("EndOfEventAction : recording ElementTable in StoreGate FAILED");
        delete m_elementTable;
      }
    m_elementTable = nullptr;

  }

  void MaterialStepRecorder::BeginOfRunAction(const G4Run*){

    ATH_MSG_DEBUG(" BeginOfRunAction");

    // initialize
    m_totalSteps = 0;
    m_eventID = 0;

  }

  void MaterialStepRecorder::UserSteppingAction(const G4Step* aStep)
  {
    // kill secondaries
    if (aStep->GetTrack()->GetParentID()) {
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
      return;
    }

    // ElementTable preparation
    if (m_recordComposition && !m_elementTable) {
      m_elementTable = new Trk::ElementTable();
      m_runElementTable = new Trk::ElementTable();
    }

    // the material information
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    // G4LogicalVolume
    G4LogicalVolume *lv= touchHist ? touchHist->GetVolume()->GetLogicalVolume() : 0;
    G4Material *mat    = lv ? lv->GetMaterial() : 0;

    std::vector<unsigned char> elements;
    std::vector<unsigned char> fractions;

    // log the information // cut off air
    if (mat && mat->GetRadlen() < 200000.) {

      ++m_totalSteps;

      // the step information
      double steplength     = aStep->GetStepLength();

      // the position information
      G4ThreeVector pos     = aStep->GetPreStepPoint()->GetPosition();

      double X0             = mat->GetRadlen();
      double L0             = mat->GetNuclearInterLength();
      double A              = 0.;
      double Z              = 0.;
      //        double totAtoms       = 0.;

      double rho            = mat->GetDensity()*CLHEP::mm3/CLHEP::gram;

      // get the number of Elements
      size_t                 elNumber = mat->GetNumberOfElements();
      const G4ElementVector* elVector = mat->GetElementVector();
      double                 totAtoms = mat->GetTotNbOfAtomsPerVolume();

      // reserve the right number of elements
      elements.reserve(elNumber);
      fractions.reserve(elNumber);

      if (1 == elNumber) {

        A   = mat->GetA()/CLHEP::gram;
        Z   = mat->GetZ();

        unsigned int Zint = (unsigned int)Z;
        // the element and fraction vector
        elements.push_back(static_cast<unsigned char>(Z));
        fractions.push_back(UCHAR_MAX);
        // record the Element
        if (m_recordComposition && !m_elementTable->contains(Zint)){
          // the element Material
          Trk::Material elMat(X0,L0,A,Z,rho);
          G4String      elName =  (*elVector)[0]->GetName();
          // add it to the table
          m_elementTable->addElement(elMat, elName);
          m_runElementTable->addElement(elMat,elName);
        }


      } else {

        const G4double*   atVector = mat->GetVecNbOfAtomsPerVolume();
        double totalFrac           = 0.;
        double totalFracChar       = 0.;

        for (size_t iel = 0; iel < elNumber; ++iel) {

          G4Element*  currentEl  = (*elVector)[iel];
          double      currentNum = atVector ? atVector[iel] : 1.;
          double      relNbAtoms = currentNum/totAtoms;

          double currentZ = currentEl->GetZ();

          A += relNbAtoms*currentEl->GetA()/CLHEP::gram;
          Z += relNbAtoms*currentEl->GetZ();
          unsigned int Zint = (unsigned int)(currentEl->GetZ());

          // the element and fraction vector
          elements.push_back(int(currentZ));
          // calculate the fraction with a accuracy of 1/256.
          unsigned int relNbAtomsChar = (unsigned int)(relNbAtoms*(UCHAR_MAX));
          relNbAtomsChar = relNbAtomsChar > UCHAR_MAX ? UCHAR_MAX : relNbAtomsChar;

          // smaller components than 0.5 % are automatically ignored
          totalFrac     += relNbAtoms;
          if (relNbAtomsChar) {
            totalFracChar += double(relNbAtomsChar)/double(1.*UCHAR_MAX);
            fractions.push_back(relNbAtomsChar);
            // record composition
            if (m_recordComposition && !m_elementTable->contains(Zint)){
              double curA          =  currentEl->GetA()/CLHEP::gram;
              double curZ          =  currentEl->GetZ();
              // approximate formulas for X0 and L0
              // X0 from : PH-EP-Tech-Note-2010-013 g/cm3 -> g/mm3
              // L0 from :
              double curX0         =  1432.8*curA/(curZ*(curZ+1)*(11.319-std::log(curZ)));
              double curL0         =  0.;
              double curRho        =  rho*relNbAtoms;
              Trk::Material elMat(curX0,curL0,curA,curZ,curRho);
              G4String      elName =  currentEl->GetName();
              // add it to the table
              m_elementTable->addElement(elMat, elName);
              m_runElementTable->addElement(elMat,elName);
            }
          }
        }
        if ((totalFrac-1.)*(totalFrac-1.) > 10e-4 )
          ATH_MSG_DEBUG("Total fractions do not add up to one at INPUT (" << totalFrac << ") !");
      }

      // is it a Geantino?
      if (aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() == 0) {
        if (m_recordComposition)
          m_matStepCollection->push_back(new Trk::MaterialStep(pos.x(), pos.y(), pos.z(), steplength, X0, L0, A, Z, rho, elements, fractions));
        else
          m_matStepCollection->push_back(new Trk::MaterialStep(pos.x(), pos.y(), pos.z(), steplength, X0, L0, A, Z, rho));
      }
    }
    return;
  }


} // namespace G4UA
