/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrigReadNavTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRIGTAUTRIGREADNAVTOOL_H
#define TRIGTAUTRIGREADNAVTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "boost/type_traits/integral_constant.hpp"

#include "StoreGate/StoreGateSvc.h"

#include "EventKernel/INavigable4Momentum.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "boost/type_traits/is_same.hpp"
#include <boost/foreach.hpp>


class Incident;

class TrigMatchTool;

namespace Trig{
  class ChainGroup;
}


namespace TrigTauTool 
{

  static const InterfaceID IID_TrigReadNavTool("TrigTauTool::TrigReadNavTool", 1, 0);

  /** @class TrigReadNavTool 
   * Class for reading trigger navigation using the TrigDecisionTool 
   
      @author  E. Ptacek <eptacek@uoregon.edu>
  */  

  class TrigReadNavTool : public AlgTool, virtual public IIncidentListener
    {
    public:
      TrigReadNavTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~TrigReadNavTool (){
      }
      
      /** access to tool interface */
      static const InterfaceID& interfaceID(){
	return IID_TrigReadNavTool;
      }

       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /**
       * @brief Handle Gaudi incidents.
       * @param inc The incident to handle.
       */
      virtual void handle (const Incident& inc);

      /******************************/
      /*Public Tool functions       */
      /******************************/

      /***********************************************************************
       * return vector of chain names from chain group pattern
       **********************************************************************/
      std::vector<std::string> getChainNames(const std::string& chainPattern);

      /************************************************************************
       * return map of chain names to vectors of roiWords,
       * where roiWords were from RoI's that passed the chain
       *
       * C is the type of the PhysicsAnalysis RoI
       * (roi class from PhysicsAnalysis/AnalysisTrigger...)
       ***********************************************************************/
      std::map<std::string, std::vector<unsigned int> > mapChainsToRoIs(const std::string& chainPattern, std::string label="initialRoI"){
	std::vector< std::string > chainNames = getChainNames(chainPattern);
	return mapChainsToRoIs(chainNames, label);
      }

      std::map<std::string, std::vector<unsigned int> > mapChainsToRoIs(const std::vector<std::string>& chainNames, std::string label="initialRoI");


      /*************************************************************
       * getRelations returns a vector of unique pointers to 
       * objects of type O that are attached through the trigger
       * navigation to p.
       * fromlabel is optional label of trigger object p
       * tolabel is optional label of returned trigger objects
       * only objects that connected to the chains specifed by
       * chainPattern will be tested for match or returned.
       * (If object p is not in specified chains no matches will be 
       *  returned - to match an EF object p to L2 objects
       *  specifiy chainPattern that includes EF and L2)
       *************************************************************/
      template<typename O, typename C>
	std::vector<const O*> getRelations(const std::string& chainPattern, const C* p=0, std::string fromlabel="", std::string tolabel=""){
	m_chainPattern=chainPattern;
	return getRelations<O,C>(p, fromlabel, tolabel);
      }
      template<typename O, typename C>
        std::vector<const O*> getRelations(const C* p, std::string& fromlabel, std::string& tolabel);
      
    protected:
      /** class member version of retrieving MsgStream */
      mutable MsgStream                 m_log;

      /** member variables for algorithm properties: */
      /// Property: Incident service.
      ServiceHandle<IIncidentSvc> m_incSvc;

      // Property: the TrigDecisionTool: used for retrieving chain names from chain pattern
      ToolHandle<Trig::TrigDecisionTool> m_trigDec;

      // Property Chain pattern: pattern string used by the trig decision tool to defines the chain group object, retrieve chain names
      std::string m_chainPattern;

      // Property: Only select objects that fullfill the criteria to pass the trigger for the chains; 
      bool m_onlyPassed;


      /****************************************************************************************
       * class Utils: Utility functions
       ****************************************************************************************/
      class Utils{
      public:
        Utils(){;}
        virtual ~Utils(){;}

        //returns true if two objects have the same pointer
        //checks at compile time that  objects were of same type
        template <typename TO>
          bool isSamePtr(const TO* p, const TO* o){
          if(p==o) return true;
          return false;
        }
      };
      /************************************************************************
       * @Read Nav Tool Dat(DatCache) class; 
       * DatCache and inherited classes contain most of tool functionality
       * these classes contain the maps from objects to chains and objects to 
       * relations(nav based matches) and the functions that create the maps.
       * these classes act as the cache (are deleted when the event ends)
       * 
       ************************************************************************/
      class DatCache : public Utils{
      public:
	DatCache(const ToolHandle<Trig::TrigDecisionTool>* trigDec,
	       const std::string& chainPattern,
	       bool onlyPassed){
	  m_chainGroup=0;
	  m_trigDec=trigDec;
	  m_chainPattern=chainPattern;
	  m_onlyPassed=onlyPassed;
	}
	virtual ~DatCache(){;}

	/// Chain group pointer
	const Trig::ChainGroup* m_chainGroup;

	const ToolHandle<Trig::TrigDecisionTool>* m_trigDec;
	std::string m_chainPattern;
	bool m_onlyPassed;
      };
      /************************************************************************
       * @Read Nav Tool Dat(DatCacheT) class;                                
       *
       * For nav matching
       * type C(from type) is type of object mapped via nav match 
       * to object(s) of type O(to type)                                       
       ************************************************************************/
      template<typename O, typename C>
	class DatCacheT: public DatCache{
	public:
	DatCacheT(const ToolHandle<Trig::TrigDecisionTool>* trigDec,
		  const std::string& chainPattern,
		  bool onlyPassed)
	  : DatCache(trigDec, chainPattern, onlyPassed){m_fromlabel=""; m_tolabel="";}

	~DatCacheT(){;}
       
	/* for obj to obj nav matching */
	void SetObjectLabels(std::string& fromlabel, std::string& tolabel){
	  m_fromlabel=fromlabel;
	  m_tolabel=tolabel;
	}
	std::string m_fromlabel;
        std::string m_tolabel;

	std::map<const C*, std::vector<const O*> > m_objTorelations;
	void fillobjTorelations();
      };
    }; 
} // end of namespace
#include "TrigTauD3PDMaker/TrigReadNavTool.icc"
#endif 
