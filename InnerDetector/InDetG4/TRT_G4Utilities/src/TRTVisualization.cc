/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTVisualization.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"


TRTVisualization* TRTVisualization::pVisualization = NULL;


  // Called by GetPointer

TRTVisualization::TRTVisualization(): m_msg("TRTVisualization")
{
  TRTParameters* pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTVisualization" << endmsg;

  Initialize();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTVisualization done" << endmsg;
}


  // Called by TRTRunAction::BeginOfRunAction

TRTVisualization::~TRTVisualization()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTVisualization" << endmsg;

  pVisualization = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTVisualization done" << endmsg;
}


  // Called by TRTVisualization

void TRTVisualization::Initialize()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTVisualization::Initialize" << endmsg;

  pVisAttributeRed = new G4VisAttributes(G4Colour(1., 0., 0.));
  pVisAttributeRed->SetVisibility(true);

  pVisAttributeGreen = new G4VisAttributes(G4Colour(0., 1., 0.));
  pVisAttributeGreen->SetVisibility(true);

  pVisAttributeBlue = new G4VisAttributes(G4Colour(0., 0., 1.));
  pVisAttributeBlue->SetVisibility(true);

  pVisAttributeYellow = new G4VisAttributes(G4Colour(1., 1., 0.));
  pVisAttributeYellow->SetVisibility(true);

  pVisAttributeMagenta = new G4VisAttributes(G4Colour(1., 0., 1.));
  pVisAttributeMagenta->SetVisibility(true);

  pVisAttributeCyan = new G4VisAttributes(G4Colour(0., 1., 1.));
  pVisAttributeCyan->SetVisibility(true);

  pVisAttributeBlack = new G4VisAttributes(G4Colour(0., 0., 0.));
  pVisAttributeBlack->SetVisibility(true);

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTVisualization::Initialize done" << endmsg;
}


  // Called on demand

void TRTVisualization::Visualize(G4LogicalVolume* pLogicalVolume,
  int colourIndex)
{
  switch(colourIndex)
  {
    case 0:
      pLogicalVolume->SetVisAttributes(G4VisAttributes::Invisible);
      break;
    case 1:
      pLogicalVolume->SetVisAttributes(pVisAttributeRed);
      break;
    case 2:
      pLogicalVolume->SetVisAttributes(pVisAttributeGreen);
      break;
    case 3:
      pLogicalVolume->SetVisAttributes(pVisAttributeBlue);
      break;
    case 4:
      pLogicalVolume->SetVisAttributes(pVisAttributeYellow);
      break;
    case 5:
      pLogicalVolume->SetVisAttributes(pVisAttributeMagenta);
      break;
    case 6:
      pLogicalVolume->SetVisAttributes(pVisAttributeCyan);
      break;
    case 7:
      pLogicalVolume->SetVisAttributes(pVisAttributeBlack);
      break;
    default:
      std::cerr << std::endl;
      std::cerr << "***** TRTVisualization::Visualize *****" << std::endl;
      std::cerr << "  Invalid colour index " << colourIndex << "." << std::endl;
      std::cerr << "  Exit!" << std::endl << std::endl;
      exit(0);
  }
}
