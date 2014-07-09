/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLMenuLoader
#define TrigConf_XMLMenuLoader

#include "TrigConfStorage/IMenuLoader.h"
#include "TrigConfStorage/XMLLoader.h"
#include "TrigConfL1Data/TriggerItemNode.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 trigger menu configuration*/
   class XMLMenuLoader : virtual public IMenuLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLMenuLoader(XMLStorageMgr& sm, const ptree::value_type* pt) :
         IMenuLoader(), XMLLoader("XMLMenuLoader", sm, pt) {};

      /**@brief destructor*/
      virtual ~XMLMenuLoader(){};

      virtual bool load( Menu& data);

   protected:

      /**@brief builds a trigger item from a given DOM node and attaches
       * it to the item hierarchy. 
       *
       * @param dom_node DOM (XML) node that contains the information
       * for the trigger item
       * 
       * @param parent parent trigger item to which the new one will be
       * linked
       * 
       * @param thr_vec reference to the vector of existing trigger thresholds
       * 
       * Note that this function calls itself recursively to build the
       * hierarchy of trigger items
       *
       * @returns pointer to the newly created @c TriggerItem
       */
      TriggerItemNode* readNode(const value_type& pt, 
                                const std::vector<TriggerThreshold*>& thr_vec);

      /**@brief finds a trigger threshold by name
       *
       * @param name name of the trigger threshold to look for
       *
       * @param thr_vec vector of trigger thresholds to be searched
       *
       * @returns pointer to the @c TriggerThreshold object found or 0
       * if there is non
       */
      TriggerThreshold* findThreshold(const std::string& name, 
                                      const std::vector<TriggerThreshold*>& thr_vec);

   };

}

#endif 

