/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gROOT->LoadMacro("../src/MeanAndRMS.h+");
  gROOT->LoadMacro("Identifier/Identifier.h+");
  gROOT->LoadMacro("CaloDetDescr/CaloDetDescrElement.h+");
  gROOT->LoadMacro("../src/CaloGeometry.cxx+");
  gROOT->LoadMacro("CaloGeometryFromFile.cxx+");

  // Warning: cell lookup in the FCal is not working yet!
  CaloGeometryFromFile* geo=new CaloGeometryFromFile();
  geo->LoadGeometryFromFile("ATLAS-GEO-20-00-01.root","ATLAS-GEO-20-00-01");
  //CaloGeometry::m_debug_identity=3179554531063103488;
  //geo->Validate();
  
  const CaloDetDescrElement* cell;
  cell=geo->getDDE(2,0.24,0.24); //This is not working yet for the FCal!!!
  cout<<"Found cell id="<<cell->identify()<<" sample="<<cell->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<endl;
  
  Long64_t cellid64(3179554531063103488);
  Identifier cellid(cellid64);
  cell=geo->getDDE(cellid); //This is working also for the FCal
  cout<<"Found cell id="<<cell->identify()<<" sample="<<cell->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<endl;
      
}

