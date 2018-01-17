/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4Utilities/TRTVisualization.hh"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"


TRTVisualization* TRTVisualization::s_pVisualization = NULL;


  // Called by GetPointer

TRTVisualization::TRTVisualization(): m_msg("TRTVisualization")
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTVisualization" << endmsg;

  Initialize();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTVisualization done" << endmsg;
}


  // Called by TRTRunAction::BeginOfRunAction

TRTVisualization::~TRTVisualization()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTVisualization" << endmsg;

  s_pVisualization = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTVisualization done" << endmsg;
}


  // Called by TRTVisualization

void TRTVisualization::Initialize()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTVisualization::Initialize" << endmsg;

  m_pVisAttributeRed = new G4VisAttributes(G4Colour(1., 0., 0.));
  m_pVisAttributeRed->SetVisibility(true);

  m_pVisAttributeGreen = new G4VisAttributes(G4Colour(0., 1., 0.));
  m_pVisAttributeGreen->SetVisibility(true);

  m_pVisAttributeBlue = new G4VisAttributes(G4Colour(0., 0., 1.));
  m_pVisAttributeBlue->SetVisibility(true);

  m_pVisAttributeYellow = new G4VisAttributes(G4Colour(1., 1., 0.));
  m_pVisAttributeYellow->SetVisibility(true);

  m_pVisAttributeMagenta = new G4VisAttributes(G4Colour(1., 0., 1.));
  m_pVisAttributeMagenta->SetVisibility(true);

  m_pVisAttributeCyan = new G4VisAttributes(G4Colour(0., 1., 1.));
  m_pVisAttributeCyan->SetVisibility(true);

  m_pVisAttributeBlack = new G4VisAttributes(G4Colour(0., 0., 0.));
  m_pVisAttributeBlack->SetVisibility(true);

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
      pLogicalVolume->SetVisAttributes(m_pVisAttributeRed);
      break;
    case 2:
      pLogicalVolume->SetVisAttributes(m_pVisAttributeGreen);
      break;
    case 3:
      pLogicalVolume->SetVisAttributes(m_pVisAttributeBlue);
      break;
    case 4:
      pLogicalVolume->SetVisAttributes(m_pVisAttributeYellow);
      break;
    case 5:
      pLogicalVolume->SetVisAttributes(m_pVisAttributeMagenta);
      break;
    case 6:
      pLogicalVolume->SetVisAttributes(m_pVisAttributeCyan);
      break;
    case 7:
      pLogicalVolume->SetVisAttributes(m_pVisAttributeBlack);
      break;
    default:
      std::cerr << std::endl;
      std::cerr << "***** TRTVisualization::Visualize *****" << std::endl;
      std::cerr << "  Invalid colour index " << colourIndex << "." << std::endl;
      std::cerr << "  Exit!" << std::endl << std::endl;
      exit(0);
  }
}
