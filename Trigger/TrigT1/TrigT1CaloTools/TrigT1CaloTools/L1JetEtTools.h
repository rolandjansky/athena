///////////////////////////////////////////////////////////////////
// L1JetEtTools.h, (c) Alan Watson
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef LVL1L1JETETTOOLS_H
#define LVL1L1JETETTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1JetEtTools.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"

namespace LVL1 
{

  //using namespace TrigConf;

  /** @class L1JetEtTools

      This is a tool to reconstruct the L1 JetEt trigger sums
      and the thresholds passed & expected RoI word.
      Used for offline monitoring and trigger reconstruction.

      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  class L1JetEtTools : virtual public IL1JetEtTools, public AthAlgTool
    {
    public:
      
      L1JetEtTools(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~L1JetEtTools ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      /** Compute JetEt sum from a range of different input data */
      virtual unsigned int jetEtSum(const DataVector<JetROI>* rois);
      virtual unsigned int jetEtSum(const CMMJetHitsCollection* sums);
      virtual unsigned int jetEtSum(const std::vector<Jet_ROI>& rois);
      virtual unsigned int jetEtSum(const std::vector<unsigned int>& multiplicities,
                                    const std::vector<int>& weights);

      /** Calculate RoI word for given jetEt value */
      virtual unsigned int jetEtRoIWord(unsigned int jetEt);

      /** Fill multiplicity vectors from a range of different input data */
      virtual void fillMultiplicities(const DataVector<JetROI>* rois, std::vector< unsigned int >& multiplicities);
      virtual void fillMultiplicities(const CMMJetHitsCollection* sums, std::vector< unsigned int >& multiplicities);
      virtual void fillMultiplicities(const std::vector< Jet_ROI >& rois, std::vector< unsigned int >& multiplicities);
                  
    private:

      /** pointer to LVL1ConfigSvc */
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
           
    }; 
} // end of namespace

#endif 
