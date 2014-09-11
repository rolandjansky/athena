/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
///    Author: A. Wildauer
///    CERN, January 26, 2005
///
/////////////////////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/CombinerTool.h"
#include "JetEvent/JetTagInfoBase.h"
//#include "JetEvent/Jet.h"
#include "xAODJet/Jet.h"

namespace Analysis
{

CombinerTool::CombinerTool( const std::string& t, const std::string& n, const IInterface* p ) :
        AthAlgTool( t, n, p )
{
    declareInterface<ICombinerTool>( this );
}

CombinerTool::~CombinerTool()
{}

StatusCode CombinerTool::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode CombinerTool::finalize()
{
    return StatusCode::SUCCESS;
}

/** OLD APPROACH
Produce the likelihood of one tagger
*/
// std::vector<double> CombinerTool::simpleCombine(const JetTagInfoBase* iTagInfo) const
// {
//     std::vector<double> likelihood;
//     // just to make sure
//     if (iTagInfo==0) {
//         ATH_MSG_ERROR("#BTAG# Never give a NULL pointer to JetTagTools/CombinerTool::simpleCombine(...)! Fix it! I segfault!");
//     }
//     if (iTagInfo->isValid())
//     {
//         likelihood = iTagInfo->tagLikelihood();

//         /** sum the likelihood vector for normalization */
//         double sum( 0. );
//         for ( std::vector<double>::iterator lhItr = likelihood.begin();
//                 lhItr != likelihood.end(); ++lhItr )
//         {
//             sum += ( *lhItr );
//         }

//         for ( std::vector<double>::iterator lhItr = likelihood.begin();
//                 lhItr != likelihood.end(); ++lhItr )
//         {
//             ( *lhItr ) /= sum;
//         }
//     }
//     else
//     {
//         // if not valid return -1. as sig and bkg
//         likelihood.push_back(-1.);
//         likelihood.push_back(-1.);
//     }
//     return likelihood;
// }

/** SIMPLER INTERFACE THAN old approach above
Produce the likelihood of one tagger
*/
std::vector<double> CombinerTool::simpleCombine(const xAOD::Jet& particleJet, const std::string& infoKey) const
{
  std::vector<double> likelihood;
  if (particleJet.tagInfo(infoKey) == 0)
  {
    // no combine possible so we also dont know how many event types (normally 2: sig and bkg)
    likelihood.push_back(-1.);
    likelihood.push_back(-1.);
    return likelihood;
  }
  else
  {
    return simpleCombine(particleJet.tagInfo(infoKey));
  }
}

/**
Combine the likelihoods from a list of taggers given in the combineTheseTaggers vector<string>
*/
std::vector<double> CombinerTool::simpleCombine( const xAOD::Jet& particleJet,
        const std::vector<std::string>& combineTheseTaggers ) const
{
    bool combine(true);
    std::vector<double> likelihood;
    /// if no info object is there there is nothing to combine ...
    if ( particleJet.jetTagInfoVector().size() == 0 )
    {
        ATH_MSG_WARNING("#BTAG# JetTagInfoBase vector is ZERO. Nothing to combine!");
        combine = false;
    }

    /// if TruthInfo is the only object in the info map no combine is possible
    if ( particleJet.tagInfo("TruthInfo") != 0 && particleJet.jetTagInfoVector().size() == 1 )
    {
        ATH_MSG_DEBUG("#BTAG# The only object in ITagInfo vector is TruthInfo. No combine possible.");
        combine = false;
    }

    /** The size of the likelihood vector is equal to the number of histogram files, i.e. the size of
    the m_mapOfAllHistos vector in the LikelihoodTool (2 in case of one background and one signal).
    Since the combiner tool shall not need to retrieve the likelihood tool this info has to be taken
    from somewhere else. The size of the likelihood verctor of each ITagInfo object (except that of
    the "TruthInfo" one) has also the size of the number of input files it gets it from m_mapOfAllHistos)
    and hence we can take that on to resize *this likelihood vector.
    */
    if (combine)
    {
        bool firstValidTagger(true);
        int count(0);
        for ( std::vector<std::string>::const_iterator strItr = combineTheseTaggers.begin(); strItr != combineTheseTaggers.end(); strItr++ )
        {
            ATH_MSG_VERBOSE("#BTAG# Combining likelihood from these taggers " << *strItr);
            const JetTagInfoBase* infoObject(particleJet.tagInfo(*strItr));
            if ( infoObject != 0 )
            {
                if (infoObject->isValid())
                {
                  if (firstValidTagger)
                  {
                    likelihood.resize( infoObject->tagLikelihood().size() );
                    for ( std::vector<double>::iterator itr = likelihood.begin() ;
                            itr != likelihood.end() ; ++itr )
                    {
                        ( *itr ) = 1.;
                    }
                    firstValidTagger=false;
                  }
                    count++;
                    const std::vector<double>& tmpVector = infoObject->tagLikelihood();

                    unsigned int numInputFiles( 0 );
                    for ( std::vector<double>::const_iterator tmpItr = tmpVector.begin();
                            tmpItr != tmpVector.end(); ++tmpItr )
                    {
                      if (likelihood.size()>=numInputFiles+1)
                      {
                        likelihood[ numInputFiles ] *= tmpVector[ numInputFiles ];
                      }
                      numInputFiles++;
                    }
                }
                else
                    ATH_MSG_DEBUG("#BTAG# TagTool " << *strItr << " does exist but is not valid.");
            }
            else
                ATH_MSG_DEBUG("#BTAG# TagTool " << *strItr << " does not exist.");
        }

        /** if no InfoObjects of the requested types (combineTheseTaggers) has been found return -1. */
        if (count == 0)
        {
          // no combine possible so we also dont know how many event types (normally 2: sig and bkg)
          likelihood.clear();
          likelihood.resize(0); // also resize because i am not 100% sure what clear() does to a resized vector :-)
	  likelihood.push_back(-1.);
          likelihood.push_back(-1.);
        }
        /** sum the likelihood vector for normalization */
        else if (count > 0)
        {
            double sum( 0. );
            for ( std::vector<double>::iterator lhItr = likelihood.begin();
                    lhItr != likelihood.end(); ++lhItr )
            {
                sum += ( *lhItr );
            }

            for ( std::vector<double>::iterator lhItr = likelihood.begin();
                    lhItr != likelihood.end(); ++lhItr )
            {
                ( *lhItr ) /= sum;
            }
        }
    }
    else
    {
        // no combine possible so we also dont know how many event types (normally 2: sig and bkg)
        likelihood.push_back(-1.);
        likelihood.push_back(-1.);
    }
    return likelihood;
}

}
