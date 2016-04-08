/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////File Config Example/////////////////////////////
//                                                                       //
//   strcpy(GeneralName,"Test__"); ->Prefix of the saved plots           //
//   DataHis->SetNextFile("../trkana.root", "Test", 2);                  //
//                               |-> First File to compare               //
//                                            |-> Name in the legend     //
//                                                  |-> Color            //
//   DataHis->SetLineStyle(2);                                           //
//   DataHis->SetFillStyle(3005);                                        //
//   DataHis->SetLuminosity(1./50.000);                                  //
//                                                                       //
//   DataHis->SetNextFile(....                  ->Next File to compare   //
//   ...                                                                 //
//   ...                                                                 //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

//
// Nice colors:
//  * KGray+2
//  * KRed-4
//  * KOrange+1
//  * KSpring+9
//  * KGreen-3
//  * KCyan+1
//  * KAzure-5
//  * KBlue+2              
//
//////////////////////////////////////////////////////////////////////////
void define_file_config0() 
{
  strcpy(GeneralName,"Test");
  DataHis->SetNextFile("../trkana.root", "Test", 2); 
  DataHis->SetLineStyle(2);
  DataHis->SetFillStyle(3005);
    
}

//////////////////////////////////////////////////////////////////////////
void define_file_config1() 
{
  strcpy(GeneralName,"M8PLUS_COMB");
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/NOM_B1_trkana.root", "Nom BON", kGray+3); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/NOM_B0_trkana.root", "Nom NOB", kGray+1); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/L10_B1_trkana.root", "L10 BON", kRed); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/L10_B0_trkana.root", "L10 NOB", kRed-7); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/L18_B1_trkana.root", "L18 BON", kGreen+1); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/L18_B0_trkana.root", "L18 NOB", kGreen-9); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/L27_B0_trkana.root", "L27 NOB", kCyan); 
  //DataHis->SetNextFile("/l/martis/scratch/M8Plus/Combined/L27_B1_trkana.root", "L27 BON", kBlue); 
  //
  // 
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/Combined/L30_B0_trkana.root", "L30 NOB", kOrange); 
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/Combined/L30_B1_trkana.root", "L30 BON", kRed); 
  //
  DataHis->SetLuminosity("hSTATS",2);
  DataHis->SetEntriesLegend("hit/trk");
}

//////////////////////////////////////////////////////////////////////////
void define_file_config2() 
{
  strcpy(GeneralName,"M8PLUS_COMB");
  DataHis->SetFilesDirectory("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/Combined/");
  // 
  DataHis->SetNextFile("L30_B0_trkana.root", "L30 NOB", kOrange); 
  DataHis->FlagAsBackground();
  DataHis->SetNextFile("L30_B1_trkana.root", "L30 BON", kRed); 
  DataHis->FlagAsBackground();
  //
  DataHis->SetLuminosity("hSTATS",2);
}

//////////////////////////////////////////////////////////////////////////
void define_file_config3() 
{
  strcpy(GeneralName,"M8PLUS_COMB");
  // 
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/Combined/NOM_B0_trkana.root", "14.2.21.1", kOrange); 
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/NOM_B0_trkana.root", "14.4.0", kRed+2); 
  //
  DataHis->SetLuminosity("hSTATS",2);
}

//////////////////////////////////////////////////////////////////////////
void define_file_config4() 
{
  strcpy(GeneralName,"M8PLUS_RELEASES");
  // 
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/Combined/L27_B1_trkana.root", "14.2.21.1", kOrange); 
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L27_B1_trkana.root", "14.4.0", kRed+2); 
  //
  DataHis->SetLuminosity("hSTATS",2);
}

//////////////////////////////////////////////////////////////////////////
void define_file_config5() 
{
  strcpy(GeneralName,"M8PLUS_14.4");
  DataHis->SetFilesDirectory("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/");
  if (1==0) {
    // 
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/NOM_B0_trkana.root", "NOM NOB", kGray+2); 
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/NOM_B1_trkana.root", "NOM BON", kBlack); 
    //
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L27_B0_trkana.root", "L27 NOB", kAzure+8); 
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L27_B1_trkana.root", "L27 BON", kBlue); 
    //
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L30_B0_trkana.root", "Iter8 NOB", kOrange); 
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L30_B1_trkana.root", "Iter8 BON", kRed+2); 
    //
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L31_B0_trkana.root", "Iter9 NOB", kSpring-9); 
    //DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L31_B1_trkana.root", "Iter9 BON", kGreen+1); 
    //
    DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L32_B0_trkana.root", "B Off", kOrange); 
    DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/L32_B1_trkana.root", "B On", kRed+2); 
    //
  }
  if (1==0) {
    DataHis->SetNextFile("NOM_BA_trkana.root", "Nominal", kGray+2); 
    DataHis->SetNextFile("L27_BA_trkana.root", "Stave Align.", kAzure+8); 
    DataHis->SetNextFile("L32_BA_trkana.root", "Module Align.", kRed+1); 
  }
  if (1==0) {
    DataHis->SetNextFile("L32_B0_trkana.root", "B OFF", kOrange+7); 
    DataHis->SetNextFile("L32_B1_trkana.root", "B ON", kRed+3); 
  }
  if (1==1) {
    DataHis->SetNextFile("MCP_B1_trkana.root", "MC Perfect", kGray+2); 
    DataHis->SetFillStyle(3002);
    DataHis->SetNextFile("L32_BA_trkana.root", "Module Align.", kRed+1); 
    DataHis->SetMarkerStyle(20);
  }
  
  DataHis->SetLuminosity("hSTATS",1);
}

//////////////////////////////////////////////////////////////////////////
void define_file_config6() 
{
  strcpy(GeneralName,"BEN");
  //
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BFieldON/L24_trkana.root", "Aligned", kBlack );
  DataHis->SetNextFile("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BFieldON/NOM_trkana.root", "Nominal", kRed); 
  //
  DataHis->SetLuminosity("hSTATS",2);
  DataHis->SetYTitle("hits/track");
  DataHis->SetOptTitle(kFALSE);
}

//////////////////////////////////////////////////////////////////////////
void define_file_config7() 
{
  strcpy(GeneralName,"Claude_LHCC");
  DataHis->SetFilesDirectory("/home/martis/projectes/atlas/alineament/ntuples/M8Plus/BonBoff_14.4/");
  // 
  if (1==0) {
    DataHis->SetNextFile("NOM_BA_trkana.root", "Nominal", kGray+3); 
    DataHis->SetNextFile("L27_BA_trkana.root", "Stave Alignment", kBlue-4); 
    DataHis->SetNextFile("L30_BA_trkana.root", "Module Alignment", kRed+1); 
  }
  //
  if (1==1) {
    DataHis->SetNextFile("NOM_B1_trkana.root", "Nominal", kRed+1); 
    DataHis->SetNextFile("L27_B1_trkana.root", "Stave Alignment", kBlue-4); 
    DataHis->SetNextFile("L30_B1_trkana.root", "Module Alignment", kGray+3); 
  }
  //
  DataHis->SetLuminosity("hSTATS",2);
}

//////////////////////////////////////////////////////////////////////////
void define_file_config8() 
{
  strcpy(GeneralName,"tops");
  DataHis->SetFilesDirectory("/home/martis/projectes/atlas/tophy/ntuples");

  DataHis->SetNextFile("topview_myntuple.root", "Signal", kBlack); 
  DataHis->SetFillStyle(1);

  DataHis->SetNextFile("topview_myntuple.root", "Comb background", kBlack); 
  DataHis->SetFillStyle(3002);
}
   
//////////////////////////////////////////////////////////////////////////
void define_file_config9() 
{
}

//////////////////////////////////////////////////////////////////////////
void define_file_config10() 
{
}

//////////////////////////////////////////////////////////////////////////
void define_file_config11() 
{
}

//////////////////////////////////////////////////////////////////////////
void define_file_config12() 
{
}

//////////////////////////////////////////////////////////////////////////
void define_file_config13() 
{
}

//////////////////////////////////////////////////////////////////////////
void define_file_config14() 
{
}

//////////////////////////////////////////////////////////////////////////
void define_file_config15() 
{
}
