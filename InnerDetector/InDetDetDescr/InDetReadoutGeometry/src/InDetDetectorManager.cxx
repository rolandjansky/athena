/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetReadoutGeometry/InDetDetectorManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "DetDescrConditions/AlignableTransform.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <map>

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
        std::string frameStr = "other";
        if (frame == InDetDD::global) frameStr = "global";
        if (frame == InDetDD::local) frameStr  = "local";
        ATH_MSG_INFO("Registering alignment channel with key " << key << ", level " << level 
                     << ", with frame " << frameStr << ".");
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
        std::map<std::string, LevelInfo>::const_iterator iter;
        iter = m_keys.find(key);
        if (iter == m_keys.end()) return s_invalidLevel;
        return iter->second;
    }

    StatusCode InDetDetectorManager::align( IOVSVC_CALLBACK_ARGS_P(I,keys) ) const
    {

        (void) I; // avoid warning about unused parameter 

        ATH_MSG_DEBUG("AlignmentCallback called ");

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
                  ATH_MSG_FATAL(err.what());
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
                    ATH_MSG_FATAL(err.what());
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
                    ATH_MSG_INFO(err.what());
                    // We continue as detector specific aligments don't always exist.
                }
            }

        } else {
            // Loop over all the keys.
            for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {

                const std::string & key = *itr;

                ATH_MSG_DEBUG(" Processing call back key  " << key);

                if ( m_globalFolders.find(key) != m_globalFolders.end() ) { 

                    try {
                        // New global alignemnts
                        bool status = processGlobalAlignmentContainer(key);
                        alignmentChange = (alignmentChange || status);
                    } catch(std::runtime_error& err) {
                        // alignments should always exist so we return fatal if we could not process the alignment for this key
                        ATH_MSG_FATAL(err.what());
                        return StatusCode::FAILURE;
                    }

                } else if ( m_folders.find(key) != m_folders.end() ) { 

                    try {
                        // Regular alignemnts
                        bool status = processAlignmentContainer(key);
                        alignmentChange = (alignmentChange || status);
                    } catch(std::runtime_error& err) {
                        // alignments should always exist so we return fatal if we could not process the alignment for this key
                        ATH_MSG_FATAL(err.what());
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
                        ATH_MSG_FATAL(err.what());
                        return StatusCode::FAILURE;
                    }
                } else {
                    // Should not be any other keys specified in call back.
                    ATH_MSG_ERROR("Unrecognized key in call back.");
                    return  StatusCode::RECOVERABLE;
                }
            }
        }

    // We invalidate all the elements if at least one alignment changed.
        if (alignmentChange) {
            invalidateAll();
        }    

        return StatusCode::SUCCESS;
    }

    StatusCode InDetDetectorManager::align(const RawAlignmentObjects& alignObjects, GeoVAlignmentStore* alignStore) const
    {

        ATH_MSG_DEBUG("align() called from an alignment CondAlg");
        if (!getIdHelper()) return StatusCode::SUCCESS; // To Do: is it really a success?

        bool alignmentChange = false;
        //      const AlignInfo &aligninfo = AlignInfo(m_alignfoldertype);
      
        for(const auto& alignObj : alignObjects) {
            const std::string& key = alignObj.first;

            ATH_MSG_DEBUG(" Processing folder  " << key);

            if(m_globalFolders.find(key)!=m_globalFolders.end()) {  
                try {
                    // New global alignemnts
                    const CondAttrListCollection* obj = static_cast<const CondAttrListCollection*>(alignObj.second);
                    bool status = processGlobalAlignmentContainer(key,obj,alignStore);
                    alignmentChange = (alignmentChange || status);
                } catch(std::runtime_error& err) {
                    // alignments should always exist so we return fatal if we could not process the alignment for this key
                    ATH_MSG_FATAL(err.what());
                    return StatusCode::FAILURE;
                }
            } 
            else if(m_folders.find(key)!=m_folders.end()) { 
                try {
                    // Regular alignemnts
                    const AlignableTransformContainer* container = static_cast<const AlignableTransformContainer*>(alignObj.second);
                    bool status = processAlignmentContainer(container,alignStore);
                    alignmentChange = (alignmentChange || status);
                } catch(std::runtime_error& err) {
                    // alignments should always exist so we return fatal if we could not process the alignment for this key
                    ATH_MSG_FATAL(err.what());
                    return StatusCode::FAILURE;
                }
            } 
            else if(m_specialFolders.find(key)!=m_specialFolders.end()) {
                // To Do: do we really need this?

                /*
                  try {
                  // Detector specific alignments
                  bool status = processSpecialAlignment(key, aligninfo.AlignFolder(),alignStore);
                  alignmentChange = (alignmentChange || status);
                  } 
                  catch(std::runtime_error& err) {
                  // Should always exist if the folder was requested so we return fatal if we could not process the alignment for this key
                  ATH_MSG_FATAL(err.what());
                  return StatusCode::FAILURE;
                  }
                */
            } 
            else {
                // Should not be any other keys specified in call back.
                ATH_MSG_ERROR("Unrecognized folder name.");
                return StatusCode::RECOVERABLE;
            }
        }
        // To Do: custom caching is not going to work in MT
        /*
          if(alignmentChange) invalidateAll(); 
        */

        return StatusCode::SUCCESS;      
    }

    bool InDetDetectorManager::processAlignmentContainer(const std::string & key) const
    {
        bool alignmentChange = false;

        ATH_MSG_DEBUG("Dealing with key as container");
        const AlignableTransformContainer* container;
        if (StatusCode::SUCCESS!=m_detStore->retrieve(container, key)) {        
            ATH_MSG_ERROR("Cannot find AlignableTransformContainer for key " 
                          << key << " - no misalignment");
            // This should not occur in normal situations so we force job to abort.
            throw std::runtime_error("Unable to apply Inner Detector alignments");
        }
        // Check if container is empty - this can occur if it is an invalid IOV.
        if (container->empty()) {
            ATH_MSG_ERROR("AlignableTransformContainer for key " 
                          << key << " is empty. Probably due to out of range IOV");
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

    bool InDetDetectorManager::processAlignmentContainer(const AlignableTransformContainer* container, GeoVAlignmentStore* alignStore) const
    {
        bool alignmentChange = false;

        // Check if container is empty - this can occur if it is an invalid IOV.
        if (container->empty()) {
            ATH_MSG_ERROR("AlignableTransformContainer " 
                          << " is empty. Probably due to out of range IOV"); // To Do: add key to this printout for making it more informative
            // This should not occur in normal situations so we force job to abort.
            throw std::runtime_error("Unable to apply Inner Detector alignments.");
        }
        // loop over all the AlignableTransform objects in the collection
        // use only the last ones.
        // /Indet/AlignL3/SCTEA9 appear repeatedly in tags of the /Indet/AlignL3 folder
        std::map<const std::string, const AlignableTransform*> stringToTransform;
        for (DataVector<AlignableTransform>::const_iterator pat=container->begin();
             pat!=container->end();++pat) {
            stringToTransform[(*pat)->tag()] = *pat;
        }
        for (std::pair<const std::string, const AlignableTransform*> value: stringToTransform) {
            bool status = processKey(value.first, value.second, alignStore);
            alignmentChange = (alignmentChange || status);
        }
        return alignmentChange;
    }

    bool InDetDetectorManager::processKey(const std::string key,
                                          const AlignableTransform* transformCollection,
                                          GeoVAlignmentStore* alignStore) const 
    {  
        bool alignmentChange = false;

        // From the key determine what level in hierarchy we are dealing with.
        // returns -1 if unrecognized.  
        const LevelInfo & levelInfo = getLevel(key);
        if (levelInfo.isValid()) {
            ATH_MSG_VERBOSE("Processing channel: " << key);
        } else {
            ATH_MSG_DEBUG("Channel " << key << " not registered in this manager");
        }
        // return silently if unrecognised - this can happen in container mode
        // when a single container holds transforms for both pixel and SCT
        if (!levelInfo.isValid() ) return false;

        //Loop over the effected nodes.
        for (AlignableTransform::AlignTransMem_citr trans_iter = transformCollection->begin(); 
             trans_iter != transformCollection->end(); 
             ++trans_iter) {
            ATH_MSG_DEBUG( "Get alignment for identifier " 
                           << getIdHelper()->show_to_string(trans_iter->identify())  
                           << " at level " << levelInfo.level());

            // The delta in the conditions DB is not necessarily the same as what is needed in the
            // alignable transform. At the moment we support global frame, local frame or an alternative frame
            // The setAlignableTransformDelta method takes care of this correction - this is CLHEP <--> Amg interfaced
            bool status = setAlignableTransformDelta(levelInfo.level(), 
                                                     trans_iter->identify(),
                                                     Amg::CLHEPTransformToEigen(trans_iter->transform()),
                                                     levelInfo.frame(),
                                                     alignStore);

            alignmentChange = (alignmentChange || status);

            if (!status) {
                if (!identifierBelongs(trans_iter->identify())) {
                    // Its probably OK. Eg /Indet/Align/ID contains alse pixel and sct ids.
                    ATH_MSG_DEBUG("Cannot set AlignableTransform for identifier."
                                  << " Probably OK if its /Indet/Align/ID folder. "  
                                  << getIdHelper()->show_to_string(trans_iter->identify())  
                                  << " at level " << levelInfo.level());
                } else {
                    if (m_suppressWarnings) {
                        ATH_MSG_DEBUG("WARNING: Cannot set AlignableTransform for identifier  " 
                                      << getIdHelper()->show_to_string(trans_iter->identify())  
                                      << " at level " << levelInfo.level());
                    } else {
                        ATH_MSG_WARNING("Cannot set AlignableTransform for identifier  " 
                                        << getIdHelper()->show_to_string(trans_iter->identify())  
                                        << " at level " << levelInfo.level());
                        ATH_MSG_WARNING("Subsequent WARNINGS will be printed at DEBUG level.");
                        m_suppressWarnings = true; 
                    }
                }
            }  
        }
        return alignmentChange;
    }

  // We provide a default implementation of any detector specific alignment.
    bool InDetDetectorManager::processGlobalAlignmentContainer(const std::string & key,
                                                               const CondAttrListCollection* obj,
                                                               GeoVAlignmentStore* alignStore) const
    {
      bool alignmentChange = false;

      ATH_MSG_DEBUG("processing GlobalAlignmentContainer with key:  " << key);
      // From the key determine what level in hierarchy we are dealing with.                                                                                   
      // returns -1 if unrecognized.                                                                                                                           
      const LevelInfo & levelInfo = getLevel(key);
      if (levelInfo.isValid()) {
          ATH_MSG_VERBOSE("Processing channel: " << key);
      } else {
          ATH_MSG_DEBUG("Channel " << key << " not registered in this manager");
      }
      // return silently if unrecognised - this can happen in container mode                                                                                   
      // when a single container holds transforms for both pixel and SCT                                                                                       
      if (!levelInfo.isValid() ) return false;
        
      // Within detector specific code
      bool status = processGlobalAlignment(key, levelInfo.level(), levelInfo.frame(), obj, alignStore);
      
      alignmentChange = (alignmentChange || status);

      return alignmentChange;

    }
  
  // We provide a default implementation of any detector specific alignment.                                                                                 
    bool InDetDetectorManager::processGlobalAlignment(const std::string &, int /*level*/, FrameType /*frame*/,
                                                      const CondAttrListCollection* /*obj*/, GeoVAlignmentStore* /*alignStore*/) const
    {
        return false;
    }


  // We provide a default implementation of any detector specific alignment.
    bool InDetDetectorManager::processSpecialAlignment(const std::string &, InDetDD::AlignFolderType) const
    {
        return false;
    }

    const InDetDetectorManager::LevelInfo InDetDetectorManager::s_invalidLevel;

} // namespace InDetDD
