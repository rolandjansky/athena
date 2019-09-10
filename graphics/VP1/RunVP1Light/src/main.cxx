/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Author: Riccardo.Maria.Bianchi@cern.ch, Apr 2017
// Update: Apr 2019

#include "RunVP1Light/VP1LightRun.h"

// Qt includes
#include <QSettings>
#include <QString>
#include <QDir>
#include <QDebug>
// #include <QCoreApplication>
#include <QCommandLineParser>

// C++ includes
#include <iostream>
#include <stdlib.h>
#include <fstream>

int main(int argc, char** argv)
{
  QStringList arguments;
  for (int i = 0; i<=argc; i++){
    arguments << argv[i];
  }

  QCommandLineParser parser;

  // An database directory option
  QCommandLineOption databaseDirectoryOption(QStringList() << "d" << "database-directory", "Use geometry database <file>.", "default");
  parser.addOption(databaseDirectoryOption);

  // An AOD directory option
  QCommandLineOption AODDirectoryOption(QStringList() << "x" << "xaod-directory", "Use xAOD file <file>.", "default");
  parser.addOption(AODDirectoryOption);

  // Print help
  QCommandLineOption helpOption(QStringList() << "h" << "help", "Show help.");
  parser.addOption(helpOption);

  // Process the command line arguments given by the user
  parser.process(arguments);

  bool helpIsSet = parser.isSet(helpOption);
  bool dbIsSet = parser.isSet(databaseDirectoryOption);
  bool AODIsSet = parser.isSet(AODDirectoryOption);
  QString dbDir = parser.value(databaseDirectoryOption);
  QString aodDir = parser.value(AODDirectoryOption);

  //If help option is set, display help and exit VP1Light
  if(helpIsSet){
    qInfo() << "Usage";
    qInfo() << " ";
    qInfo() << "  vp1light [options]";
    qInfo() << " ";
    qInfo() << "Add path to geometry database or AOD file as options. Alternatively they can be set inside VP1Light.";
    qInfo() << " ";
    qInfo() << "Options:";
    qInfo() << "  -h, --help                               = Show help.";
    qInfo() << "  -d, --database-directory <path-to-DB>    = Specify geometry database file.";
    qInfo() << "  -x, --xaod-directory <path-to-xAOD>        = Specify xAOD file.";
    return 0;
  }

  // Save settings
  QSettings settings("ATLAS", "VP1Light");
  if(dbIsSet){
    settings.setValue("db/path", dbDir);
    settings.setValue("db/dbByEnv", "1");
  }
  else{
    settings.setValue("db/dbByEnv", "");
  }
  if(AODIsSet){
    settings.setValue("aod/path", aodDir);
    settings.setValue("aod/aodByEnv", "1");
  }
  else{
    settings.setValue("aod/aodByEnv", "");
  }

  // Disable expert settings by default
  if(settings.value("ExpertSettings/notFirstStart").toString().isEmpty()){
    settings.setValue("ExpertSettings/notFirstStart","1");
    settings.setValue("ExpertSettings/enableExpertSettings","");
  }


  VP1LightRun vp1light("my first VP1Light test");

  vp1light.initialize();
  while ( vp1light.execute());
}
