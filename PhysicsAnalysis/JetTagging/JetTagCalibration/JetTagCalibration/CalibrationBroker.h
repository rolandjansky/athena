/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGCALIBRATION_CALIBRATIONBROKER_H
#define JETTAGCALIBRATION_CALIBRATIONBROKER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "Gaudi/Property.h"
#include "TFile.h"
#include <map>
#include <string>
#include <utility>
#include <set>

class TH1;
class TObject;
class IPoolSvc;

namespace Analysis {

  /**
  @ class CalibrationBroker

  This class retrieves or stores calibration histograms in COOL for the taggers.
  A single COOL folder is defined for all the taggers.
  Each tagger is subdivided in channels, each channel being associated to a jet type
   (Cone4Topo, Kt6, etc).

  Any further substructure (track categories, signal/background) is hidden to COOL 
  and is defined by the actual name of the histogram.

  This class holds the collection of maps between the histogram name and the pointer 
  to the histogram, the latter being kept up-to-date via a callback.
  Each tagger in its initialize() method must register the histograms needed with the 
  registerHistogram() method.

  The geometry tag corresponding to the calibrations, the type of physics events used, 
  the tracking algorithm, etc, are all defined by a DB tag.

  @author laurent.vacavant@cern.ch
  */


class CalibrationBroker : public AthAlgTool {

public:
  CalibrationBroker (const std::string& type,
            const std::string& name, const IInterface* parent);
  virtual ~CalibrationBroker();

  static const InterfaceID& interfaceID();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode registerCallBack();
  void registerHistogram(const std::string& tagger, 
                         const std::string& histoname);
  template <class T> std::pair<T*,bool> retrieveTObject(const std::string& folder, 
                                                 const std::string& channel,
                                                 const std::string& objectname) const;
  std::pair<TH1*,bool> retrieveHistogram(const std::string& tagger, 
                                         const std::string& channel,
                                         const std::string& histoname) const;
  void updateHistogramStatus(const std::string& tagger, 
                             const std::string& channel,
                             const std::string& histoname,
			     bool status);

  std::map<std::string, std::string> m_ossMap;
  inline void storeStr(const std::string& folder, const std::string& channel, const std::string& histoname, std::string oss) {
    std::string key = folder + channel + histoname;
    m_ossMap[key] = oss;//m_ossMap.insert( std::make_pair(key,oss) );
  }
  inline std::string getStr(const std::string& folder, const std::string& channel, const std::string& histoname) {
    std::string key = folder + channel + histoname;
    return m_ossMap[key];
  }


  struct calibMV2{    std::vector<std::string> inputVars;    std::string str;    TObject* obj;  };
  std::map<std::string, calibMV2> m_calibMap;
  inline void storeCalib(const std::string& folder, const std::string& channel, const std::string& histoname,
		       std::vector<std::string> inputVars, std::string str, TObject* obj ) {
    std::string channelAlias = this->channelAlias(channel);
    std::string key = folder + channelAlias + histoname;
    calibMV2 calib;    calib.inputVars = inputVars;    calib.str=str;    calib.obj=obj;
    m_calibMap[key] = calib;
  }
  inline calibMV2 getCalib(const std::string& folder, const std::string& channel, const std::string& histoname) {
    std::string channelAlias = this->channelAlias(channel);
    std::string key = folder + channelAlias + histoname;
    return m_calibMap[key];
  }
  
  void printStatus() const;
  virtual StatusCode callBack( IOVSVC_CALLBACK_ARGS );
  // helper functions:
  std::string channelName(const std::string& fullHistoName) const;
  std::string histoName(const std::string& fullHistoName) const ;
  std::string fullHistoName(const std::string& channel, const std::string& histoName) const;
  std::string channelAlias(const std::string& originalChannel) const;
  void updateHistoStatusTaggerList(const std::string& longfolder, const std::string& fname);
  void updateHistogramStatusPerTagger(const std::string& folder, 
				      const std::string& channel, 
				      const std::string& hname, 
				      bool status);
  bool updatedTagger(const std::string& folder, 
		     const std::string& channel, 
		     const std::string& hname); 

private:

  StatusCode createHistoMap(TFile* file);
  StatusCode objectTDirExists(const std::string& histname, TFile* file) const;
  StatusCode getTObject(const std::string& histname, TFile* file, TObject*& hist) const;
  std::vector<std::string> tokenize(std::string str, std::string delim);

  IPoolSvc* m_poolsvc;
  int m_nrefresh;
  bool m_callBackRegistered;
  static const unsigned int s_nmax_callbacks;

  std::string m_folderRoot;
  std::vector< std::string > m_taggers; 
  std::vector< std::string > m_channels;
  std::vector< std::string > m_originalChannels;
  /* aliases for channels: 
   * to specifiy an alias, enter a string "channelA->channelAA" (or a list of strings)
   * this will force to use channelAA from the DB wherever channelA was mentioned */
  StringArrayProperty m_channelAliases;
  bool m_shadowFoldersAndChannels;
  bool m_recreateHistoMap;
  std::string m_singleFolderName;
  std::map< std::string, std::string > m_channelAliasesMap;
  std::map< std::string, std::vector<std::string> >  m_channelAliasesMultiMap;
  std::vector<std::vector<std::string> > m_taggersHists;
  /* all the histograms associated with a flag to know if they have been updated recently: */
  std::vector< std::map<std::string, std::pair<TObject*, bool> > > m_histos;

  /// if the tagger name is in the set it means that the tagger tool already knows about the update
  std::map<std::string, std::set<std::string> > m_folderTaggerUpdateAliases;

};

}
#include "JetTagCalibration/CalibrationBroker.icc"
#endif // JETTAGCALIBRATION_CALIBRATIONBROKER_H
