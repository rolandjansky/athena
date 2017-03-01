/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetReadoutGeometry/InDetDetectorManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "DetDescrConditions/AlignableTransform.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h" 

namespace InDetDD 
{

    InDetDetectorManager::InDetDetectorManager(StoreGateSvc * detStore, const std::string & name)
        : m_alignfoldertype{none},m_detStore(detStore), 
        m_msg(name+"DetectorManager"), 
        m_suppressWarnings(false)
    {
        setName(name);
    }

  // Destructor
    InDetDetectorManager::~InDetDetectorManager() 
        {}


    const Version& InDetDetectorManager::getVersion() const
    {
        return m_version;
    }

    const std::string& InDetDetectorManager::getLayout() const
    {
        return m_version.layout();
    }

    void InDetDetectorManager::setVersion(const Version & version)
    {
        m_version = version;

    // Since default alignments are for final layout, Pixel Rome-Initial
    // layout will result in several (harmless) WARNING message. We suppress these.  
    // Also the SR1 layout produce warnings due to missing parts. We suppress these also.
        m_suppressWarnings = ( (getName() == "Pixel" && 
            (version.tag() == "Pixel-01" || version.tag() == "Pixel-DC2-Initial-00"))
            || version.layout() == "SR1" || version.layout() == "SR1-EndcapC");

    }

    void InDetDetectorManager::addChannel(const std::string & key, int level, FrameType frame)
    {
      //if (msgLvl(MSG::DEBUG)) {
            std::string frameStr = "other";
            if (frame == InDetDD::global) frameStr = "global";
            if (frame == InDetDD::local) frameStr  = "local";
            msg(MSG::INFO) << "Registering alignment channel with key " << key << ", level " << level 
                << ", with frame " << frameStr << "." <<endmsg;
	    //}
        m_keys[key] = LevelInfo(level, frame); 
    }

    void InDetDetectorManager::addFolder(const std::string & key)
    {
        m_folders.insert(key);
    }

    void InDetDetectorManager::addSpecialFolder(const std::string & key)
    {
        m_specialFolders.insert(key);
    }

    void InDetDetectorManager::addGlobalFolder(const std::string & key)
    {
        m_globalFolders.insert(key);
    }

    void InDetDetectorManager::addAlignFolderType(const AlignFolderType  alignfolder)
    {
        m_alignfoldertype = alignfolder;
    }

  // Return the level in the hierarchy (user defined) corresponding to the key.
    const InDetDetectorManager::LevelInfo& InDetDetectorManager::getLevel(const std::string & key) const 
    {
        static LevelInfo invalidLevel;
        std::map<std::string, LevelInfo>::const_iterator iter;
        iter = m_keys.find(key);
        if (iter == m_keys.end()) return invalidLevel;
        return iter->second;
    }

    StatusCode InDetDetectorManager::align( IOVSVC_CALLBACK_ARGS_P(I,keys) ) const
    {
        (void) I; // avoid warning about unused parameter 

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AlignmentCallback called " << endmsg;

        if (!getIdHelper()) return StatusCode::SUCCESS;

        bool alignmentChange = false;
	const AlignInfo &aligninfo = AlignInfo(m_alignfoldertype);

        // If dummy arguments
        if (keys.empty()) {


            // New global aligment folders should be processed first
            for (std::set<std::string>::const_iterator iterFolders = m_globalFolders.begin();
            iterFolders != m_globalFolders.end();
            ++iterFolders) {
                try {
                    bool status = processGlobalAlignmentContainer(*iterFolders);
                    alignmentChange = (alignmentChange || status);
                } catch(std::runtime_error& err) {
                    // keys are empty when running simualtion. It is normal for detector specific aligments not to exist.
		    msg(MSG::FATAL) << err.what() << endmsg;
		    return StatusCode::FAILURE;
                }
            }

            // Regular alignments. Loop through folder keys. Normally only one.
            for (std::set<std::string>::const_iterator iterFolders = m_folders.begin();
            iterFolders != m_folders.end();
            ++iterFolders) {
                try {
                    bool status = processAlignmentContainer(*iterFolders);
                    alignmentChange = (alignmentChange || status);
                }
                catch(std::runtime_error& err) {
                    // alignments should always exist so we return fatal if we could not process the alignment for this key
                    msg(MSG::FATAL) << err.what() << endmsg;
                    return StatusCode::FAILURE;
                }
            }  
            // Detector specific aligments
            for (std::set<std::string>::const_iterator iterFolders = m_specialFolders.begin();
            iterFolders != m_specialFolders.end();
            ++iterFolders) {
                try {
		  bool status = processSpecialAlignment(*iterFolders, aligninfo.AlignFolder());
                    alignmentChange = (alignmentChange || status);
                } catch(std::runtime_error& err) {
                    // keys are empty when running simualtion. It is normal for detector specific aligments not to exist.
                    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << err.what() << endmsg;
                    // We continue as detector specific aligments don't always exist.
                }
            }

        } else {
            // Loop over all the keys.
            for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {

                const std::string & key = *itr;

                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Processing call back key  " << key << endmsg;      

		if ( m_globalFolders.find(key) != m_globalFolders.end() ) { 

                    try {
                        // New global alignemnts
                        bool status = processGlobalAlignmentContainer(key);
                        alignmentChange = (alignmentChange || status);
                    } catch(std::runtime_error& err) {
                        // alignments should always exist so we return fatal if we could not process the alignment for this key
                        msg(MSG::FATAL) << err.what() << endmsg;
                        return StatusCode::FAILURE;
                    }

		} else if ( m_folders.find(key) != m_folders.end() ) { 

                    try {
                        // Regular alignemnts
                        bool status = processAlignmentContainer(key);
                        alignmentChange = (alignmentChange || status);
                    } catch(std::runtime_error& err) {
                        // alignments should always exist so we return fatal if we could not process the alignment for this key
                        msg(MSG::FATAL) << err.what() << endmsg;
                        return StatusCode::FAILURE;
                    }

                } else if ( m_specialFolders.find(key) !=  m_specialFolders.end() ) {
                    try {
                        // Detector specific alignments
		      bool status = processSpecialAlignment(key, aligninfo.AlignFolder());
                        alignmentChange = (alignmentChange || status);
                    } 
                    catch(std::runtime_error& err) {
                        // Should always exist if the folder was requested so we return fatal if we could not process the alignment for this key
                        msg(MSG::FATAL) << err.what() << endmsg;
                        return StatusCode::FAILURE;
                    }
                } else {
                    // Should not be any other keys specified in call back.
                    msg(MSG::ERROR) << "Unrecognized key in call back." << endmsg;
                    return  StatusCode::SUCCESS;
                }
            }
        }

    // We invalidate all the elements if at least one alignment changed.
        if (alignmentChange) {
            invalidateAll();
        }    

        return StatusCode::SUCCESS;
    }


    bool InDetDetectorManager::processAlignmentContainer(const std::string & key) const
    {
        bool alignmentChange = false;

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Dealing with key as container" << endmsg;
        const AlignableTransformContainer* container;
        if (StatusCode::SUCCESS!=m_detStore->retrieve(container, key)) {        
            msg(MSG::ERROR) << "Cannot find AlignableTransformContainer for key " 
                << key << " - no misalignment" << endmsg;
            // This should not occur in normal situations so we force job to abort.
            throw std::runtime_error("Unable to apply Inner Detector alignments");
        }
        // Check if container is empty - this can occur if it is an invalid IOV.
        if (container->empty()) {
            msg(MSG::ERROR) << "AlignableTransformContainer for key " 
                << key << " is empty. Probably due to out of range IOV" << endmsg;
            // This should not occur in normal situations so we force job to abort.
            throw std::runtime_error("Unable to apply Inner Detector alignments.");
        }
        // loop over all the AlignableTransform objects in the collection
        for (DataVector<AlignableTransform>::const_iterator pat=container->begin();
        pat!=container->end();++pat) {
	  
            bool status = processKey((*pat)->tag(),*pat);
            alignmentChange = (alignmentChange || status);
        }
        return alignmentChange;
    } 

    bool InDetDetectorManager::processKey(const std::string key, const AlignableTransform* transformCollection) const 
    {  
        bool alignmentChange = false;

        // From the key determine what level in hierarchy we are dealing with.
        // returns -1 if unrecognized.  
        const LevelInfo & levelInfo = getLevel(key);
        if (msgLvl(MSG::DEBUG)) {
            if (levelInfo.isValid()) {
                if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Processing channel: " << key << endmsg;
            } else {
                msg(MSG::DEBUG) << "Channel " << key << " not registered in this manager" << endmsg;
            }
        }
        // return silently if unrecognised - this can happen in container mode
        // when a single container holds transforms for both pixel and SCT
        if (!levelInfo.isValid() ) return false;


        //Loop over the effected nodes.
        for (AlignableTransform::AlignTransMem_citr trans_iter = transformCollection->begin(); 
        trans_iter != transformCollection->end(); 
        ++trans_iter) {
            if (msgLvl(MSG::DEBUG)) {
	      msg(MSG::DEBUG) << "Get alignment for identifier " 
			     << getIdHelper()->show_to_string(trans_iter->identify())  
			     << " at level " << levelInfo.level() << endmsg;

            }
            // The delta in the conditions DB is not necessarily the same as what is needed in the
            // alignable transform. At the moment we support global frame, local frame or an alternative frame
            // The setAlignableTransformDelta method takes care of this correction - this is CLHEP <--> Amg interfaced
            bool status = setAlignableTransformDelta(levelInfo.level(), 
                                                     trans_iter->identify(),
						     Amg::CLHEPTransformToEigen(trans_iter->transform()),
                                                     levelInfo.frame());

            alignmentChange = (alignmentChange || status);

            if (!status) {
                if (!identifierBelongs(trans_iter->identify())) {
                    // Its probably OK. Eg /Indet/Align/ID contains alse pixel and sct ids.
                    if (msgLvl(MSG::DEBUG)) {
                        msg(MSG::DEBUG) << "Cannot set AlignableTransform for identifier."
                            << " Probably OK if its /Indet/Align/ID folder. "  
                            << getIdHelper()->show_to_string(trans_iter->identify())  
                            << " at level " << levelInfo.level() << endmsg;
                    }
                } else {
                    if (m_suppressWarnings) {
                        if (msgLvl(MSG::DEBUG)) {	      
                            msg(MSG::DEBUG) << "WARNING: Cannot set AlignableTransform for identifier  " 
                                << getIdHelper()->show_to_string(trans_iter->identify())  
                                << " at level " << levelInfo.level() << endmsg;
                        }
                    } else {
                        msg(MSG::WARNING) << "Cannot set AlignableTransform for identifier  " 
                            << getIdHelper()->show_to_string(trans_iter->identify())  
                            << " at level " << levelInfo.level() << endmsg;
                        msg(MSG::WARNING) << "Subsequent WARNINGS will be printed at DEBUG level."  << endmsg;
                        m_suppressWarnings = true; 
                    }
                }
            }  
        }
        return alignmentChange;
    }

  // We provide a default implementation of any detector specific alignment.
    bool InDetDetectorManager::processGlobalAlignmentContainer(const std::string & key) const
    {
      bool alignmentChange = false;

      msg(MSG::DEBUG) << "processing GlobalAlignmentContainer with key:  " << key  << endmsg;
      // From the key determine what level in hierarchy we are dealing with.                                                                                   
      // returns -1 if unrecognized.                                                                                                                           
      const LevelInfo & levelInfo = getLevel(key);
      if (msgLvl(MSG::DEBUG)) {
	if (levelInfo.isValid()) {
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Processing channel: " << key << endmsg;
	} else {
	  msg(MSG::DEBUG) << "Channel " << key << " not registered in this manager" << endmsg;
	}
      }
      // return silently if unrecognised - this can happen in container mode                                                                                   
      // when a single container holds transforms for both pixel and SCT                                                                                       
      if (!levelInfo.isValid() ) return false;
        
      // Within detector specific code
      bool status = processGlobalAlignment(key, levelInfo.level(), levelInfo.frame());
      
      alignmentChange = (alignmentChange || status);

      return alignmentChange;

    }
  
  // We provide a default implementation of any detector specific alignment.                                                                                 
    bool InDetDetectorManager::processGlobalAlignment(const std::string &, int /*level*/, FrameType /*frame*/) const
    {
        return false;
    }


  // We provide a default implementation of any detector specific alignment.
    bool InDetDetectorManager::processSpecialAlignment(const std::string &, InDetDD::AlignFolderType) const
    {
        return false;
    }


} // namespace InDetDD
