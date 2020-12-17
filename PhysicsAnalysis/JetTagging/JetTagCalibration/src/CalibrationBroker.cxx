/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagCalibration/CalibrationBroker.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PoolSvc/IPoolSvc.h"
#include "FileCatalog/IFileCatalog.h"
#include "TH1.h"
#include "TObject.h"
#include "TTree.h"
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>
#include <sys/types.h>


namespace Analysis {

  /**
     @ class CalibrationBroker

     @author laurent.vacavant@cern.ch
  */


  static const InterfaceID IID_ICalibrationBroker("Analysis::CalibrationBroker", 1, 0);

  const unsigned int CalibrationBroker::s_nmax_callbacks = 100;

  CalibrationBroker::CalibrationBroker(const std::string& type, 
				       const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent), 
    m_poolsvc(nullptr),
    m_nrefresh(0),
    m_callBackRegistered(false) {
    declareInterface<CalibrationBroker>(this);
    declareProperty("folderRoot", m_folderRoot = "/GLOBAL/BTagCalib/");
    declareProperty("taggers", m_taggers);
    declareProperty("channels", m_originalChannels);
    declareProperty("channelAliases", m_channelAliases);
    declareProperty("shadowFoldersAndChannels", m_shadowFoldersAndChannels = true);
    declareProperty("singleFolderName", m_singleFolderName = "RUN12");
    declareProperty("recreateHistoMap", m_recreateHistoMap = false);
  }

  CalibrationBroker::~CalibrationBroker() {
  }

  const InterfaceID& CalibrationBroker::interfaceID() {
    return IID_ICalibrationBroker;
  }

  StatusCode CalibrationBroker::initialize() {

    m_histos.clear();
    ATH_MSG_DEBUG( "#BTAG# CalibrationBroker::initialize() called " << name() );
    // List of folders:
    for(uint i=0;i<m_taggers.size();i++) {
      ATH_MSG_DEBUG( "#BTAG# Tagger " << m_taggers[i]);
    }
    // List of channels:
    ATH_MSG_DEBUG( "#BTAG# Original channels " );

    for(uint i=0;i<m_originalChannels.size();i++) {
      ATH_MSG_DEBUG("#BTAG# Channel " << m_originalChannels[i] );
    }
    // Decode channel aliases:
    for(std::vector<std::string>::const_iterator aliasI = m_channelAliases.value().begin(),
	  aliasE = m_channelAliases.value().end(); 
        aliasI != aliasE; aliasI++) {
      const std::string::size_type delim = aliasI->find("->");
      if(delim == std::string::npos) {
        ATH_MSG_ERROR( "#BTAG# Unexpected format in channelAliases: " << (*aliasI));
      } else {
        ATH_MSG_INFO( "#BTAG# Calibration channel alias: " << aliasI->substr(0, delim) << " -> " 
		      << aliasI->substr(delim+2) );
	std::string jetc= aliasI->substr(0, delim);
	std::vector<std::string> jeta = tokenize(aliasI->substr(delim+2), ",");
	m_channelAliasesMultiMap.insert(std::make_pair(jetc, jeta) );
	// Add to list of channels to which aliases will be attached
	// (necessary because getJetAuthor used in taggers does not use
	//  jet collection name but standardised info)
	if (std::find(m_originalChannels.begin(), m_originalChannels.end(),jetc)
	    == m_originalChannels.end()) m_originalChannels.push_back(jetc);
      }
    }
    // Prepare histo maps:
    m_histos.reserve(m_taggers.size());
    for(uint i=0;i<m_taggers.size();i++) {
          std::map<std::string, std::pair<TObject*, bool> > hmap;
          m_histos.push_back(hmap);
	  std::vector<std::string> hvect;
	  m_taggersHists.push_back(hvect);
    }

    // get PoolSvc
    ATH_CHECK(service("PoolSvc",m_poolsvc));

    // Register call back function:
    ATH_CHECK(this->registerCallBack());

    return StatusCode::SUCCESS;
  }

  StatusCode CalibrationBroker::registerCallBack() {
    if(!m_callBackRegistered) {
      // -- Register IoV callback function for the COOL folders
      // If folders are shadow folders, register only the single actual folder ALL
      if(m_shadowFoldersAndChannels) {
        const DataHandle<CondAttrListCollection> aptr;
	std::string folder(m_folderRoot); folder+=m_singleFolderName;
        ATH_MSG_DEBUG( "#BTAG# Registering IoV callback for single folder "<< folder);
        if(StatusCode::SUCCESS==detStore()->regFcn(&Analysis::CalibrationBroker::callBack,
	  					   this, aptr, folder)) {
	  ATH_MSG_DEBUG(  "#BTAG# Registered.");
	} else {
           ATH_MSG_ERROR(  "#BTAG# Registration of IoV callback for folder " << folder
               << " failed" );
        }
      } else {
        ATH_MSG_DEBUG(  "#BTAG# The multiple folders DB schema has been deprecated in Run2. Contact Flavour Tagging software team if you get this message.");
      }
      m_callBackRegistered = true;
    }
    return StatusCode::SUCCESS;
  }


  void CalibrationBroker::registerHistogram(const std::string& tagger, const std::string& hname) {
    ATH_MSG_DEBUG(  "#BTAG# registering histogram " << hname << " in " 
		    << ( m_shadowFoldersAndChannels ? "(shadow)" : "" ) << " tagger " << tagger );
    bool registered = false;
    for(uint i=0;i<m_taggers.size();i++) {
      if(tagger==m_taggers[i]) {
	ATH_MSG_DEBUG( "#BTAG# tagger " << tagger << " found in pos " << i
		       << " , registrating " << hname );
	
	m_taggersHists[i].push_back(hname);
	registered = true;
      }
    }
    if(!registered) {
      ATH_MSG_DEBUG( "#BTAG# tagger " << tagger << " not found."
		     << " Registrating of " << hname << " not possible.");
    }
  }


  void CalibrationBroker::printStatus() const {

    ATH_MSG_DEBUG( "#BTAG# Status: #refresh= " << m_nrefresh
		   << ( m_shadowFoldersAndChannels ? " #shadow_" : " #" ) << "taggers= " << m_taggers.size()
		   << ( m_shadowFoldersAndChannels ? " #shadow_" : " #" ) << "channels= " << m_channels.size());
    if( msgLvl(MSG::DEBUG) )msg(MSG::DEBUG) << "#BTAG# Channels: ";
    for(uint i=0;i<m_channels.size();i++) {
      if( msgLvl(MSG::DEBUG) )msg(MSG::DEBUG)<< " " << m_channels[i];
    }
    if( msgLvl(MSG::DEBUG) )msg(MSG::DEBUG)<<endmsg;
    for(uint i=0;i<m_taggers.size();i++) {
      ATH_MSG_DEBUG("#BTAG#         tagger " << m_taggers[i] 
		    << " contains " << m_histos[i].size() << " histograms:" );
      std::map<std::string, std::pair<TObject*, bool> >::const_iterator hI = m_histos[i].begin();
      std::map<std::string, std::pair<TObject*, bool> >::const_iterator hE = m_histos[i].end();
      for( ; hI!=hE; ++hI ) {
        if( msgLvl(MSG::VERBOSE) )msg(MSG::VERBOSE)<< "#BTAG#          -> name: " << (*hI).first;
        if( msgLvl(MSG::VERBOSE) )msg(MSG::VERBOSE)<< " updated: " << ((*hI).second).second;
        if((*hI).second.first) {
	  if(((*hI).second.first)->InheritsFrom("TH1")){
	    if( msgLvl(MSG::VERBOSE) ){
              if (auto th1 = dynamic_cast<TH1*>(((*hI).second).first)) {
                msg(MSG::VERBOSE)<< " entries: " 
                                 << th1->GetEntries();
              }
	    }  
	  } 
        } else {
	  if( msgLvl(MSG::VERBOSE) )msg(MSG::VERBOSE)<< " NOT YET DEFINED";
	}
	if( msgLvl(MSG::VERBOSE) )msg(MSG::VERBOSE)<< endmsg;
      }
    }
  }

  std::pair<TH1*, bool> CalibrationBroker::retrieveHistogram(const std::string& folder, const std::string& channel, const std::string& hname) const {
    return this->retrieveTObject<TH1>(folder,channel,hname);
  }

  void CalibrationBroker::updateHistogramStatus(const std::string& tagger, const std::string& channel, const std::string& hname, bool status) {

    std::string channelAlias = this->channelAlias(channel);
    ATH_MSG_DEBUG("#BTAG# retrieving " << hname 
		  << " (channel " << channel << " -> " << channelAlias << ") "
      		  << " in tagger " << tagger );
    std::string fname = this->fullHistoName(channelAlias,hname);
    if(status){
      updateHistoStatusTaggerList(tagger, fname);
    }
    for(uint i=0;i<m_taggers.size();i++) {
      if(tagger==m_taggers[i]) {
	ATH_MSG_DEBUG( "#BTAG#    tagger " << tagger << " found in pos " << i
		       << " , retrieving " << fname );
	std::map<std::string, std::pair<TObject*, bool> >::iterator mI = m_histos[i].find(fname);
	std::map<std::string, std::pair<TObject*, bool> >::iterator mE = m_histos[i].end();
	if(mI!=mE) {
	  ATH_MSG_DEBUG("#BTAG#    histo found. Changing status " 
			<< (*mI).second.second << " to " << status );
	  (*mI).second.second = status;
	  break;
	}
      }
    }
    return;
  }


  bool CalibrationBroker::updatedTagger(const std::string& tagger, const std::string& channel,const std::string& hname){

    std::string channelAlias = this->channelAlias(channel);
    ATH_MSG_DEBUG("#BTAG# updatedTagger " << hname 
		  << " (channel " << channel << " -> " << channelAlias << ") "
		  << " in tagger " << tagger );
      
    std::string fname = this->fullHistoName(channelAlias,hname);
    std::string key = tagger+"#"+fname;

    std::map<std::string, std::set<std::string> >::iterator iter = m_folderTaggerUpdateAliases.find(key);
    if(iter == m_folderTaggerUpdateAliases.end()){
      return false;
    }

    return iter->second.count(tagger);

  }



  void CalibrationBroker::updateHistogramStatusPerTagger(const std::string& tagger, const std::string& channel, const std::string& hname, bool status) {


    std::string channelAlias = this->channelAlias(channel);
    ATH_MSG_DEBUG("#BTAG# per tagger update " << hname 
		  << " (channel " << channel << " -> " << channelAlias << ") "
		  << " in tagger " << tagger );
      
    std::string fname = this->fullHistoName(channelAlias,hname);
    std::string key = tagger+"#"+fname;

    if(status){
      std::map<std::string, std::set<std::string> >::iterator iter = m_folderTaggerUpdateAliases.find(key);
      if(iter != m_folderTaggerUpdateAliases.end()){
	iter->second.erase(tagger);
      }
    }
    else{
      m_folderTaggerUpdateAliases[key].insert(tagger);
    }

  }

  void CalibrationBroker::updateHistoStatusTaggerList(const std::string& tagger, const std::string& fname){

    std::string key = tagger+"#"+fname;

    m_folderTaggerUpdateAliases.erase(key);

  }

     
                   
  StatusCode CalibrationBroker::createHistoMap(TFile* pfile  ){

    ATH_MSG_DEBUG("#BTAG# in createHistoMap" );

    m_channels.clear();
    m_channelAliasesMap.clear();
    std::vector< std::string > mappedAlias; 

    for(unsigned int j=0; j<m_originalChannels.size(); ++j){
      m_channels.push_back(m_originalChannels[j]);
    }

    std::string folder(m_folderRoot); folder+=m_singleFolderName;

    for(uint i=0;i<m_taggers.size();++i) {
      std::string tagger = m_taggers[i];

      for(unsigned int j=0; j<m_originalChannels.size(); ++j){

        /// get all aliases
        std::map<std::string, std::vector<std::string> >::iterator ialiaslist 
          = m_channelAliasesMultiMap.find(m_originalChannels[j]);
	if(ialiaslist == m_channelAliasesMultiMap.end()){
	  ATH_MSG_DEBUG( "#BTAG#  no alias for original channel" << m_originalChannels[j] );
	  if(!objectTDirExists(tagger+"/"+m_originalChannels[j], pfile)){
	    ATH_MSG_WARNING( "#BTAG# no calibration for jet collection " << m_originalChannels[j]
			   << " consider using aliases " );
	  }
	  continue;
	}
	std::vector<std::string> aliaslist = ialiaslist->second;
	if(aliaslist.size() == 1){
	  if("none" == aliaslist[0]){
	    ATH_MSG_DEBUG("#BTAG#  no alias for original channel" << m_originalChannels[j]);
				
            if(objectTDirExists(tagger+"/"+m_originalChannels[j], pfile)){
	      ATH_MSG_WARNING( "#BTAG# no calibration for jet collection " << m_originalChannels[j]
				     << " consider using aliases " );
            }
            continue;
	  }
	}

	bool foundalias=false;

	for(unsigned int k=0; k<aliaslist.size(); ++k){

	  std::string aliasentry = aliaslist[k];
	  if("none" == aliasentry){
	    ATH_MSG_DEBUG("#BTAG# first alias entry is none - replace with original channel" 
				<< m_originalChannels[j] );
            aliasentry= m_originalChannels[j];
	  }
	  /// now see if the jet collection exists in db
	  std::string hFullName(tagger); 
	  hFullName+="/"; hFullName+=aliasentry; 
          // Check if jet collection already in channel alias map
          if (std::count(mappedAlias.begin(), mappedAlias.end(), aliasentry) > 0) {
	    ATH_MSG_INFO( "#BTAG# found alias entry in Map " << aliasentry );
	    ATH_MSG_INFO( "#BTAG# found alias entry folder " << folder<< " and " << m_singleFolderName);
            m_channelAliasesMap.insert(std::make_pair(m_originalChannels[j],aliasentry));
            foundalias=true;
            break;
          }
          else {
            if (objectTDirExists(hFullName, pfile)) {
	      ATH_MSG_INFO( "#BTAG# found alias entry in DB " << aliasentry );
	      ATH_MSG_INFO( "#BTAG# found alias entry folder " << folder<< " and " << m_singleFolderName);
              if("none"!=aliaslist[k]){
		std::vector<std::string>::const_iterator pos = find(m_channels.begin(), 
								m_channels.end(), aliasentry);
		if(pos==m_channels.end()) {
		  ATH_MSG_DEBUG("#BTAG# Alias is pointing to undefined channel: " <<  aliasentry
				    << ". Adding it to channel list.");
		  m_channels.push_back(aliasentry);
	        }
	        m_channelAliasesMap.insert(std::make_pair(m_originalChannels[j],aliasentry));
                mappedAlias.push_back(aliasentry);
	      }
	      foundalias=true;
	      break;
	    }
	    else{
	      ATH_MSG_INFO( "#BTAG# no alias entry " << aliasentry 
		       << " trying next alias ");
	    }
	  }
	}
	if(!foundalias){
	  ATH_MSG_WARNING( "#BTAG# none of the aliases exist for jet collection " 
			 << m_originalChannels[j]);
	}

      }
      break ; /// check alias for the first tagger. same jet collections for all taggers for now

    }

    ATH_MSG_INFO( "#BTAG# final registered channels " );
    for(uint i=0;i<m_channels.size();++i) {
      ATH_MSG_INFO( "#BTAG# Channel " << m_channels[i] );
    }

    ATH_MSG_INFO( "#BTAG# final registered aliases " );
    std::map<std::string,std::string>::iterator iter = m_channelAliasesMap.begin();
    for(;iter!=m_channelAliasesMap.end();++iter) {
      ATH_MSG_INFO( "#BTAG# Channel alias " << iter->first <<"->"<<iter->second );
    }


    // now that we know which alias to use register the histograms

    ATH_MSG_DEBUG( "#BTAG# m_taggers size " << m_taggers.size());
    for(unsigned int i=0; i<m_taggers.size(); ++i){

      std::vector<std::string> histnames = m_taggersHists[i];
      for(unsigned int h=0; h<histnames.size(); ++h){

	std::string hname = histnames[h];
	ATH_MSG_DEBUG( "#BTAG# registrating tagger "<< m_taggers[i] <<" with " 
		       << m_channels.size() << " histograms (1 per channel): ");
	for(uint j=0;j<mappedAlias.size();j++) {
          std::string fname = this->fullHistoName(mappedAlias[j],hname);
          TObject* dummy = nullptr;
          m_histos[i].insert(std::make_pair(fname, std::make_pair(dummy,false)));
        }
	/*for(uint j=0;j<m_channels.size();j++) {
	  if(this->channelAlias(m_channels[j])==m_channels[j]) { // skip aliased channels
	    std::string fname = this->fullHistoName(m_channels[j],hname);
	    TObject* dummy = nullptr;
	    m_histos[i].insert(std::make_pair(fname, std::make_pair(dummy, false)));
	  } else {
	    ATH_MSG_DEBUG( "#BTAG# " << m_channels[j] << " is aliased to " << this->channelAlias(m_channels[j]) 
			   << " : not registering twice histogram " << hname );
	  }
	}*/
      }
    }

    return StatusCode::SUCCESS;

  }

  StatusCode CalibrationBroker::objectTDirExists(const std::string& histname, TFile * pfile) const {
    
    ATH_MSG_DEBUG("#BTAG# in objectTDirExists" );

     // now read the histogram into memory
     ATH_MSG_DEBUG("Getting object "+histname+" from file");
     TObject* hist = nullptr;
     pfile->GetObject(histname.c_str(),hist);
     if (hist==nullptr) {
       ATH_MSG_DEBUG("#BTAG# Could not load TObject " << histname);
       return StatusCode::FAILURE;
     }

     return StatusCode::SUCCESS;
  }

  StatusCode CalibrationBroker::getTObject(const std::string& histname, TFile * pfile, TObject*& hist) const {
     // now read the histogram into memory
     ATH_MSG_DEBUG("Getting object "+histname+" from file");
     pfile->GetObject(histname.c_str(),hist);
     if (hist==nullptr) {
       ATH_MSG_DEBUG("#BTAG# Could not load TObject " << histname);
       return StatusCode::FAILURE;
     }
     else {
       // make this histogram unassociated with the TFile, so file can be closed
       // only for histogram objects, others do not get associated
       // TTrees have special treatment 
       TH1* ihist=dynamic_cast<TH1*>(hist);
       if (ihist!=nullptr) ihist->SetDirectory(0);
       // if it is a TDirectory, also need special treatment to unassociate parent
       TDirectory* idir=dynamic_cast<TDirectory*>(hist);
       if (idir!=nullptr) {
         TDirectory* mdir=idir->GetMotherDir();
         if (mdir!=nullptr) {
           ATH_MSG_DEBUG("Disconnecting TDirectory "+histname+" from parent");
           mdir->GetList()->Remove(idir);
           idir->SetMother(0);
         } else {
           ATH_MSG_WARNING("Could not get MotherDir for TDirectory "+histname);
         }
       }
     }

     return StatusCode::SUCCESS;
   }

  StatusCode CalibrationBroker::callBack( IOVSVC_CALLBACK_ARGS_P(/*I*/,keys) ) {
 
    // printout the list of keys invoked - will normally only be for our
    // histogram folder
    ATH_MSG_DEBUG ( "#BTAG# CalibrationBroker callback invoked for keys:");


    if(m_shadowFoldersAndChannels) {
      std::string folder(m_folderRoot); folder+=m_singleFolderName;
      // check all the keys, if we find the histogram folder, update the pointers
      for(std::list<std::string>::const_iterator itr=keys.begin();
          itr!=keys.end();++itr) {
          ATH_MSG_DEBUG( "#BTAG# Key : " << (*itr));
          if((*itr)==folder) {
            ATH_MSG_DEBUG( "#BTAG# Key and actual folder match: " << (*itr));

            // Get the GUID
            const CondAttrListCollection* atrcol = nullptr;
            if (StatusCode::SUCCESS!=detStore()->retrieve(atrcol,folder)) {
              ATH_MSG_DEBUG("#BTAG# Cannot retrieve CondAttrListCollection for " << folder);
              return StatusCode::FAILURE;
            }
            unsigned int channel=1; //Always 1 in old version with CoolHistSvc
            CondAttrListCollection::const_iterator citr = atrcol->chanAttrListPair(channel);
            if (citr==atrcol->end()) {
              ATH_MSG_WARNING("#BTAG# Cannot find valid reference for " << folder << " channel " << channel);
              return StatusCode::FAILURE;
            }

            const std::string coolguid=citr->second["fileGUID"].data<std::string>();
            ATH_MSG_DEBUG("#BTAG# Folder key "+folder+" has current file GUID "+coolguid);
    
            // Open the file
            std::string pfname, tech;
            m_poolsvc->catalog()->getFirstPFN(coolguid, pfname, tech );
            TFile* pfile = TFile::Open(pfname.c_str(),"READ");
            if (pfile==nullptr || !pfile->IsOpen()) {
              delete pfile;
              ATH_MSG_WARNING("Problems opening input file "+pfname);
              return StatusCode::FAILURE;
            }

            if(0 == m_nrefresh || m_recreateHistoMap){
              StatusCode sc = createHistoMap(pfile);
              if(sc != StatusCode::SUCCESS){
	        /// do nothing for the moment
              }
            }

            m_nrefresh++;
          // check all the keys, if we find the histogram folder, update the pointers
		for(uint i=0;i<m_taggers.size();i++) {
	    std::string tagger = m_taggers[i];
	    std::map<std::string, std::pair<TObject*, bool> >::iterator mI = m_histos[i].begin();
	    std::map<std::string, std::pair<TObject*, bool> >::iterator mE = m_histos[i].end();
	    for(; mI!=mE; ++mI) {
	      std::string fname = (*mI).first;
	      ATH_MSG_DEBUG( "#BTAG# Retrieving " << tagger <<":"<< fname );
	      std::string channel = this->channelName(fname);
	      std::string hname = this->histoName(fname);
	      std::string hFullName(tagger); 
	      hFullName+="/"; hFullName+=channel; 
	      hFullName+="/"; hFullName+=hname;
	      ATH_MSG_DEBUG( "#BTAG#     histo name in physical file= " << hFullName );
	      TObject* hPointer = nullptr;
              if (getTObject(hFullName, pfile, hPointer)) {
              

		    if(hPointer) {
		      ATH_MSG_DEBUG( "#BTAG# Cached pointer to histogram: " << hPointer);
		      const TString rootClassName=hPointer->ClassName();
		      if (rootClassName=="TTree") {
                         ((TTree*)hPointer)->LoadBaskets();
                         ((TTree*)hPointer)->SetDirectory(0); 
		      }
                      (*mI).second.first = hPointer;
		      (*mI).second.second = true;
		      updateHistoStatusTaggerList(m_taggers[i],fname);
		    } else {
		      ATH_MSG_ERROR( "#BTAG# Could not cache pointer to histogram " << fname );
		    }
	      } else {
		ATH_MSG_WARNING("#BTAG# error: histogram "<<hFullName
				<<" does not exist - you are probably using an old database tag");
	      }
	    } //end loop histos
        } // end loop taggers
        
        // close the file
        pfile->Close();
        delete pfile;

        }// loop on keys
      }
    }
    else {
      ATH_MSG_DEBUG(  "#BTAG# The multiple folders DB schema has been deprecated in Run2. Contact Flavour Tagging software team if you get this message.");
    }
    return StatusCode::SUCCESS;
  }

  StatusCode CalibrationBroker::finalize() { 
    ATH_MSG_DEBUG("#BTAG# CalibrationBroker::finalize() called " << name() );
    return StatusCode::SUCCESS;
  }


  std::string CalibrationBroker::channelName(const std::string& fname) const {
    const std::string delim("#");
    std::string::size_type sPos;
    sPos = fname.find_first_of(delim);
    std::string channel = fname.substr(0,sPos);
    return channel;
  }

  std::string CalibrationBroker::histoName(const std::string& fname) const {
    const std::string delim("#");
    std::string::size_type sPos;
    sPos = fname.find_first_of(delim);
    std::string hname = fname.substr(sPos+1);
    return hname;
  }

  std::string CalibrationBroker::fullHistoName(const std::string& channel, const std::string& histoName) const {
    std::string fname = channel + "#" + histoName;
    return fname;
  }
  
  std::string CalibrationBroker::channelAlias(const std::string& originalChannel) const {
    std::string alias = originalChannel;
    std::map< std::string, std::string>::const_iterator pos;
    pos = m_channelAliasesMap.find(originalChannel);
    if (pos != m_channelAliasesMap.end()) alias = pos->second;
    return alias;
  }

  std::vector<std::string> CalibrationBroker::tokenize(std::string str, std::string delim){
    std::vector<std::string> tokens;
    std::string::size_type sPos, sEnd, sLen;
    // if str starts with a character in delim, do you want an empty string in tokens?
    // sPos = 0; // if answer is yes
    sPos = str.find_first_not_of(delim);  // if answer is no
    while(sPos != std::string::npos){
      sEnd = str.find_first_of(delim, sPos);
      if(sEnd == std::string::npos) sEnd = str.length();
      sLen = sEnd - sPos;
      std::string token = str.substr(sPos, sLen);
      tokens.push_back(token);
      sPos = str.find_first_not_of(delim, sEnd);
    }
    return tokens;
}

}

