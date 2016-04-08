/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NeuralNetwork.h 693248 2015-09-04 17:52:57Z wsfreund $
#ifndef RINGERSELECTORTOOLS_PROCEDURES_NEURALNETWORK_H
#define RINGERSELECTORTOOLS_PROCEDURES_NEURALNETWORK_H

//#undef NDEBUG

// STL includes:
#include <vector>

// Local includes:
#include "RingerSelectorTools/procedures/IRingerProcedure.h"

// Local tool includes:
#include "RingerSelectorTools/tools/RingerIOVarDepObj.h"
#include "RingerSelectorTools/tools/RedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/final.h"
#include "RingerSelectorTools/tools/cxx/override.h"

namespace Ringer {


namespace Discrimination {

/**
 *  @class NNFeedForward
 *  @brief Provides Neural Network FeedForward
 *
 *  @author: Dhiana Deva Cavalcanti Rocha <dhiana.deva@gmail.com> (LPS/UFRJ)
 *  @author: Werner S. Freund <wsfreund@cern.ch> (LPS/UFRJ)
 *                (revised implementation)
 *
 *  @date: Jan 2015
 **/
class NNFeedForward : virtual public IDiscriminator, 
                      public RedirectMsgStream
{

  RINGER_IO_VARDEP_BASE( NNFeedForward )

  public:
    /**
     * @brief Main ctor
     **/
    NNFeedForward(const std::vector<unsigned int> &nodes, 
       const std::vector<float> &weights, 
       const std::vector<float> &bias);

    NNFeedForward(): m_nLayers(0),
                     m_wM(nullptr),
                     m_bM(nullptr),
                     m_mM(nullptr){;}
    /**
     * @brief Propagates input through neural network
     **/
    virtual void execute(const std::vector<float> &input,
        std::vector<float> &output) const ATH_RINGER_OVERRIDE ATH_RINGER_FINAL;

    void changeArchiteture(const std::vector<unsigned int> &nodes, 
        const std::vector<float> &weights, 
        const std::vector<float> &bias);

    /**
     * @brief Main dtor
     **/
    ~NNFeedForward();

  protected:
    /**
     * Release bias memory
     **/
    void releaseBias();

    /**
     * Release weights memory
     **/
    void releaseWeights();

    /**
     * Release multiplication matrix memory
     **/
    void releaseMMatrix();

    /**
     * Get m_wM and m_bM on std::vector unique dimension representation:
     **/
    void getWeigthsAndBias(std::vector<float> &weights,
        std::vector<float> &bias) const;

  private:

    /// @brief Neural network nodes size description
    std::vector<unsigned int> m_nodes;
    /// @brief Neural network number of layers
    size_t m_nLayers;
    /// @brief Neural network weights
    float ***m_wM;
    /// @brief Neural network bias
    float **m_bM;
    /// @brief Temporary multiplication matrix
    float **m_mM;

    /** Define it as a Root TObjebt, disable I/O */
    //ClassDef(NNFeedForward,0)
};

RINGER_DEFINE_PROCEDURE_DEFAULT_METHODS( NNFeedForward )

/**
 *  @class NNFeedForwardVarDep
 *  @brief Provides Neural Network FeedForward mechanism.
 *
 *  @author: Werner S. Freund <wsfreund@cern.ch> (LPS/UFRJ)
 *  @author: Dhiana Deva Cavalcanti Rocha <dhiana.deva@gmail.com> (LPS/UFRJ)
 *
 *  @date: Jan 2015
 **/
class NNFeedForwardVarDep : virtual public IDiscriminatorVarDep,
                            public RingerIOVarDepObj < NNFeedForwardVarDep >,
                            public NNFeedForward
{

  RINGER_IO_VARDEP_OBJ(NNFeedForwardVarDep, NNFeedForward)

  public:

    /**
     * Main ctor
     **/
    NNFeedForwardVarDep(const std::vector<unsigned int> &nodes, 
        const std::vector<float> &weights, 
        const std::vector<float> &bias):
      NNFeedForward(nodes,weights,bias){;}

    /**
     * Empty ctor
     **/
    NNFeedForwardVarDep(){;}

    /** Define it as a Root TObjebt, set version 1 */
    //ClassDef(NNFeedForwardVarDep,1)
};

} // namespace Discrimination
} // namespace Ringer

#endif // RINGERSELECTORTOOLS_NEURALNETWORK_H
