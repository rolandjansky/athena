///////////////////////////////////////////////////////////////////
// L1JEMJetTools.h, (c) Alan Watson
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JEMJETTOOLS_H
#define LVL1L1JEMJETTOOLS_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigT1CaloToolInterfaces/IL1JEMJetTools.h"
#include "TrigT1CaloEvent/JetInput.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/JetInputKey.h"
#include "TrigT1CaloUtils/JEMJetAlgorithm.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

class AtlasDetectorID;
class Identifier;

namespace LVL1 
{

  /** @class L1JEMJetTools

      This is a tool to reconstruct the L1 Jet trigger sums
      for a particular RoI location from the stored JetInputs.
      Used for offline monitoring and trigger reconstruction.
      
      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1JEMJetTools : virtual public IL1JEMJetTools, public AlgTool
    {
    public:
      typedef DataVector<JetElement>          JECollection ;
      
      L1JEMJetTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1JEMJetTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Convert user-supplied JetElements to map of JetInputs. Can specify which time slice of data to use */
      virtual void mapJetInputs(const DataVector<JetElement>* jes, std::map<int, JetInput*>* elements, int slice = -1) ;

      /** Return vector of TOB RoI objects derived from user-specified inputs. JetElements can be multi-slice */
      virtual void findRoIs(const std::map<int, JetInput*>* elements, DataVector<JEMTobRoI>* rois) ;
      virtual void findRoIs(const DataVector<JetElement>* jes, DataVector<JEMTobRoI>* rois, int slice = -1) ;

      /** Return vector of RoI objects derived from user-specified inputs. JetElements can be multi-slice */
      virtual void findRoIs(const std::map<int, JetInput*>* elements, DataVector<JEMJetAlgorithm>* rois) ;
      virtual void findRoIs(const DataVector<JetElement>* jes, DataVector<JEMJetAlgorithm>* rois, int slice = -1) ;
      
      /** Form JEM results for specified crate/module using user-supplied map of input towers
          Adds to DataVector of JEMTobRoI and returns backplane data words*/
      virtual void findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                  DataVector<JEMTobRoI>* rois, std::vector<unsigned int>& jetCMXData);

      /** Return RoI object for specified location */
      virtual JEMJetAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) ;
                  
      /** Form jet cluster sums for a given RoI location */
      virtual void formSums(double RoIeta, double RoIphi,
                            const std::map<int, JetInput*>* elements) ;
      virtual void formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) ;
      /** Methods to return the cluster ET values */
      virtual int ET4x4() const ;
      virtual int ET6x6() const ;
      virtual int ET8x8() const ;
      virtual int ETLarge() const ;
      virtual int ETSmall() const ;
      virtual bool isEtMax() const ;
      virtual bool isRoI() const ;
            
    private:
      
      /** class member version of retrieving MsgStream */
      mutable MsgStream    m_log;
      int                  m_outputlevel;
      
      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;

      /** Utility for decoding RoI words */
      JEPRoIDecoder m_conv;
      
      /** member variables for algorithm properties: */
     JEMJetAlgorithm* m_RoI;
           
    }; 
} // end of namespace

#endif 
