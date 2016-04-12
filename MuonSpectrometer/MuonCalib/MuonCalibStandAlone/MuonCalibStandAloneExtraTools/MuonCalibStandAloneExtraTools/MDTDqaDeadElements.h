/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author Marco Vanadia mvanadia@cern.ch
//Class based on algorithms introduced by Philipp Fleischmann in GnaMon

#ifndef MUONCALIBSTANDALONEEXTRATOOLS_MDTDQADEADELEMENTS_H
#define MUONCALIBSTANDALONEEXTRATOOLS_MDTDQADEADELEMENTS_H


#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <vector>

//using namespace std;

namespace MuonCalib{

//classes declarations

class Tube;
class Layer;
class Mezzanine;
class Multilayer;
class MDTChamber;
class MDTDqaDeadElementsAnalysis;

//class Tube//////////////////////////////////////////////////////////////////////////////////////

class Tube{

 private:
  int m_status;
  int m_value;
  int m_position;
  int m_layer;
  int m_ml;
  Layer *m_parent_layer;

 public:
  Tube(int,int, Layer*);

  int getStatus();
  int getValue();
  int getLayer();
  int getML();
  int getPosition();
  void setValue(int);
  void setStatus(int);
  void setPosition(int);
  void setLayer(int);
  void setML(int); 

  Layer* getParentLayer();

 };
//end of class Tube////////////////////////////////////////////////////////////////////////////////

//class Layer//////////////////////////////////////////////////////////////////////////////////////

 class Layer{

 private:
   int m_ntubes;
   double m_entries;
   int m_status;
   double m_median;
   double m_median_deviation;
   std::vector<Tube*> m_tubes;

 public:
   Layer();

   Tube* addTube(int,int);
   Tube* getTube(int);

   int getNtubes();
   double getEntries();
   int getStatus();
   double getMedian();
   double getMedianDeviation();

   void setStatus(int);
   void addEntries(double);
 
   void calculateStatistics();
   int getNrealtubes();

 };
//end of class Layer////////////////////////////////////////////////////////////////////////////

//class Mezzanine//////////////////////////////////////////////////////////////////////////////

 class Mezzanine{

 private:
    int m_ntubes;
    double m_entries;
    int m_status;
    double m_median;
    double m_median_deviation;
    std::vector<Tube*> m_tubes;
    int m_70min;
    int m_70max;

 public:
    Mezzanine();

    void addTube(Tube*);
    Tube* getTube(int);

    int getNtubes();
    double getEntries();
    int getStatus();
    double getMedian();
    double getMedianDeviation();
    int get70min();
    int get70max();

    void addEntries(double);
    void setStatus(int);

    void calculateStatistics();

 };

//End of class Mezzanine/////////////////////////////////////////////////////////////////////////////////////////

//class Multilayer///////////////////////////////////////////////////////////////////////////////////////////////

 class Multilayer{

 private:
   int m_nlayers;
   int m_nmezzanines;
   double m_entries;
   int m_status;
   double m_median;
   double m_median_deviation;

   std::vector<Layer*> m_layers;
   std::vector<Mezzanine*> m_mezzanines;

 public:
   Multilayer();

   void addLayer();
   Tube* addTube(int,int,int);
   Layer* getLayer(int);
   Mezzanine* addMezzanine();
   Mezzanine* getMezzanine(int);

   int getNlayers();
   int getNmezzanines();
   double getEntries();
   int getStatus();
   double getMedian();
   double getMedianDeviation();

   void setStatus(int);
   void addEntries(double the_entries);

   void calculateStatistics();
 };

//End of class Multilayer//////////////////////////////////////////////////////////////////////////////////

//Class MDTChamber/////////////////////////////////////////////////////////////////////////////////////////

 class MDTChamber{

 private:
   int m_nmultilayers;

   double m_entries;
   int m_status;
   double m_median;
   double m_median_deviation;
   double m_mean;
   double m_standard_deviation;
   int m_90min;
   int m_90max;
   int m_70;
    
   TString m_name;

   std::vector<Multilayer*> m_multilayers;

 public:

   MDTChamber(TH1*,TString);

   void addMultilayer();
   Multilayer* getMultilayer(int);
   Tube* addTube(int,int,int,int);

   int getNmultilayers();
   double getEntries();
   int getStatus();
   double getMedian();
   double getMedianDeviation();
   double getMean();
   double getStandardDeviation();
   TString getName();
   int get90min();
   int get90max();
   int get70();


   void setStatus(int);
   void addEntries(double);

   void buildMezzanines();
   void calculateStatistics();
   void updateStatistics();

   void Print();

 };

//End of class MDTChamber////////////////////////////////////////////////////////////////////////////////

//class MDTDqaDeadElements///////////////////////////////////////////////////////////////////////////////

 class MDTDqaDeadElements{

 private:
   bool m_verbose;
   bool m_write_report;
   bool m_write_compact_report;
   bool m_do_analysis;
   bool m_print_chambers;
   bool m_write_list_of_dead_tubes;
   bool m_do_noisy;
   std::ofstream m_file_report;
   std::ofstream m_comp_report;
   std::ofstream m_filelistofdeads;
   int m_deadtubes;
   int m_deadmezzanines;
   int m_deadlayers;
   int m_deadmultilayers;
   int m_deadchambers;
   int m_noisytubes;
   int m_lowstatmezzanines;
   int m_lowstatisticschambers;
   int m_lowstatisticsfortubeschambers;
   int m_ntubes;

   int m_dead_tubes_per_sector;
   int m_dead_mezzanines_per_sector;
   int m_dead_layers_per_sector;
   int m_dead_multilayers_per_sector;
   int m_dead_chambers_per_sector;
   int m_lowstat_chambers_per_sector;

 public:
   MDTDqaDeadElements();
   void setVerbose(bool);
   void setWriteReport(bool);
   void setWriteCompactReport(bool);
   void setDoAnalysis(bool);
   void setPrintChambers(bool);
   void setDoNoisy(bool);
   void setWriteListOfDeadTubes(bool);

   void MDTDqaDeadElementsAnalysis(TFile*);

   void fillChamber(MDTChamber*, TH1*);
   void fillDeadMap(MDTChamber*, TH2F*, TH2F*,TH1F*, TDirectory*,TDirectory*);

   void doAnalysis(MDTChamber*);
   void analyseMultilayers(MDTChamber*);
   void analyseLayers(MDTChamber*);
   void analyseMezzanines(MDTChamber*);
   void reanalyseMezzanines(MDTChamber*);
   void analyseTubes(MDTChamber*);

   void PrintListOfDeadTubes(MDTChamber*);

 };

}//namespace MuonCalib

#endif
