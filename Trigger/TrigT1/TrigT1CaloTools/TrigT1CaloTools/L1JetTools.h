/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JetTools.h, 
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JETTOOLS_H
#define LVL1L1JETTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloUtils/JetInputKey.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1 
{

  /** @class L1JetTools

      This is a tool to reconstruct the L1 Jet trigger sums
      for a particular RoI location from the stored JetInputs.
      Used for offline monitoring and trigger reconstruction.

      Useage: L1JetTools->mapTowers() // set up event for analysis
              L1JetTools->formSums(eta, phi) // for each RoI
              L1JetTools->ET8x8() // etc. Return values
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1JetTools : virtual public IL1JetTools, public AthAlgTool
    {
    public:
      typedef DataVector<JetElement>          JECollection ;
      
      L1JetTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1JetTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Convert user-supplied JetElements to map of JetInputs. Can specify which time slice of data to use */
      virtual void mapJetInputs(const DataVector<JetElement>* jes, std::map<int, JetInput*>* elements, int slice = -1) ;

      /** Return vector of RoI objects derived from user-specified inputs. JetElements can be multi-slice */
      virtual void findRoIs(const std::map<int, JetInput*>* elements, DataVector<JetAlgorithm>* rois) ;
      virtual void findRoIs(const DataVector<JetElement>* jes, DataVector<JetAlgorithm>* rois, int slice = -1) ;

      /** Return RoI object for specified location */
      virtual JetAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) ;
                  
      /** Form jet cluster sums for a given RoI location */
      virtual void formSums(double RoIeta, double RoIphi,
                            const std::map<int, JetInput*>* elements) ;
      virtual void formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) ;
      /** Methods to return the cluster ET values */
      virtual int ET4x4() const ;
      virtual int ET6x6() const ;
      virtual int ET8x8() const ;
      virtual bool isEtMax() const ;
      virtual unsigned int Hits() const ;
      virtual unsigned int RoIWord() const ;
            
    private:
      
      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

      /** Utility for decoding RoI words */
      JEPRoIDecoder m_conv;
      
      /** member variables for algorithm properties: */
     JetAlgorithm* m_RoI;
           
    }; 
} // end of namespace

#endif 
