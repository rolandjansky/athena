/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EVENTSVC_H
#define ANP_EVENTSVC_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : EventSvc
 * @Author : Rustem Ospanov
 *
 * @Brief  : This singleton class reads data description from xml file
 *           and provides dataset information for current input file.
 * 
 **********************************************************************************/

/*
  EventSvc is singleton service which provides event weight. This service
  requires RunModule for initialization and counting of events.

   - read list of available datasets from XML configuration file
   - count number of events per dataset
   - set active datasets as datasets with non-zero number of events
   - total xsec is computed in one of three ways:
      1) Using name of dataset which cross-section is total cross-section
      2) Set directly in Registry configuration
      3) If above two methods do not produce postive total cross-section
         then use sum of cross-sections of all active datasets
   - divide dataset importance weight by generator efficiency
   - each event is normalized so that dataset weighted event count 
     is equal to dataset xsec/total xsec

  TODO: this code works only if all input files are read. When a specific
        number of events is requested, need to rescale weights accordingly.
*/

// C/C++
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Anp
{
  class Registry;
  class RunModule;

  class EventSvc {
  public:
    
    static EventSvc& Instance();

    double GetWeight() const { return fCurrWeight; }
    double GetNEvents() const { return fNEvents; }
    std::vector<std::string> GetDataset() const { return fDatasets; }
    
    bool IsValid() const { return fValid; }

  public:

    struct DSInfo
    {
      DSInfo() : 
	geff(1.0), weight(1.0), xsec(0.0), total_weight(0.0), nevent(0), index(0) {}
      
      // Properties read from XML
      std::string name;
      std::string path;
      std::string parent;
      std::string project;
      std::string version;
      std::string menu;
      double      geff;
      double      weight;
      double      xsec;
      
      // Computed variables
      double      total_weight;
      long        nevent;
      unsigned    index;

      bool Match(const std::string &key) const;
    };

  private:
    
    EventSvc();
    ~EventSvc();
    
    EventSvc(const EventSvc &);
    const EventSvc& operator=(const EventSvc &);
    
    void AddDataset(const std::string &ds, long nevent);

    void SetDataset(const std::string &ds);

    void Config(const Registry &reg);
       
    bool Init();

    unsigned GetDirectoryIndex(const std::string &path) const;
    
    void ParseXML(const std::string &path, const std::string &norm);
    
    friend class RunModule;

  private:

    double                        fCurrWeight;  // current weight
    double                        fTotalWeight; // total weighted event count
    double                        fNEvents;     // total cross-section for normalization
    double                        fTotalXsec;   // total cross-section for normalization
    double                        fTotalLumi;   // total luminosity for normalization

    bool                          fDebug;       // debug flag
    bool                          fWarning;     // warning flag
    bool                          fValid;       // configuration was successful

    DSInfo                       *fCurrDS;      // current dataset
    DSInfo                       *fNormDS;      // normalization dataset
    std::vector<DSInfo *>         fActvVec;     // information for active datasets
    std::vector<DSInfo *>         fInfoVec;     // information for loaded datasets

    std::set<std::string>         fSkipSet;     // List of unknown/skipped datasets
    std::vector<std::string>      fDatasets;    // Datasets from python configuration
   }; 
}

#endif

