/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVAUtils_BDT_H
#define MVAUtils_BDT_H

#include<vector>
#include "TString.h"
#include "MVAUtils/Node.h"

class TTree;

namespace MVAUtils 
{
  /** Simplified Boosted Decision Tree, based on TMVA::DecisionTree.
   * Holds a forest (vector of top nodes of each decision tree) and a
   * constant offset (not always used).  The response is given by in
   * various ways, e.g. offset + the sum of the response of each tree,
   * or 2.0/(1.0+exp(-2.0*sum))-1, with no offset. Also, there's a special
   * return when the BDTs are trained with multiple classes.
   *
   * Can be constructed from TMVA::MethodBDT or a TTree. Each entry
   * of the TTree represents a binary tree and each element of the
   * vectors stored in the TTree represent a node.
   *
   * The response can be evaluated from a vector of floats or a vector
   * of pointers (to avoid creating vectors at each call) which can be
   * stored internally (m_pointers).
   **/
  class BDT
  {
  public:

    BDT(float offset,
        float sumWeights,
        const std::vector<Node::index_t>& forest,
        const std::vector<float>& weights,
        const std::vector<Node>& nodes): 
      m_offset(offset),
      m_sumWeights(sumWeights),
      m_forest(forest),
      m_weights(weights),
      m_nodes(nodes){
      }

    BDT(TTree *tree);

    /*
     * disable everything else and default the dtor
     * */
    BDT() = delete;
    BDT (const BDT&) = delete;
    BDT& operator=(const BDT&) = delete;
    BDT (BDT&&) = delete;
    BDT& operator=(BDT&&) = delete;
    ~BDT()=default; 

    /** return the number of trees in the forest */
    unsigned int GetNTrees() const { return m_forest.size(); }

    /** return the offset that is added to the response */
    float GetOffset() const { return m_offset; }

    /** these return offset + sum */
    float GetResponse(const std::vector<float>& values) const;
    float GetResponse(const std::vector<float*>& pointers) const;
    float GetResponse() const {
      return (m_pointers.size() ? GetResponse(m_pointers) : -9999.);
    }

    /** these return Sum( purity_i*weight_i )/Sum weight_i */
    float GetClassification(const std::vector<float*>& pointers) const;
    float GetClassification() const {
      return (m_pointers.size() ? GetClassification(m_pointers) : -9999.);
    }

    /** these return 2.0/(1.0+exp(-2.0*sum))-1, with no offset */
    float GetGradBoostMVA(const std::vector<float>& values) const;
    float GetGradBoostMVA(const std::vector<float*>& pointers) const;
    float GetGradBoostMVA() const {
      return (m_pointers.size() ? GetGradBoostMVA(m_pointers) : -9999.);
    }

    /** these return a vector of normalised responses */
    std::vector<float> GetMultiResponse(const std::vector<float>& values, unsigned int numClasses) const;
    std::vector<float> GetMultiResponse(const std::vector<float*>& pointers, unsigned int numClasses) const;
    std::vector<float> GetMultiResponse(unsigned int numClasses) const;
	
    //for debugging, print out tree or forest to stdout
    void PrintForest() const;
    void PrintTree(Node::index_t index) const;
    //dump out a TTree
    TTree* WriteTree(TString name = "BDT");
	
    /** Return the values corresponding to m_pointers (or an empty vector) **/
    std::vector<float> GetValues() const;
	
    /** Return stored pointers (which are used by GetResponse with no args)*/
    std::vector<float*> GetPointers() const { return m_pointers; }

    /** Set the stored pointers so that one can use GetResponse with no args non-const not MT safe*/
    void SetPointers(const std::vector<float*>& pointers) { m_pointers = pointers; }
	
  private:

    // create new tree 
    void newTree(const std::vector<int>& vars, const std::vector<float>& values);
    float GetTreeResponse(const std::vector<float>& values, Node::index_t index) const;
    float GetTreeResponse(const std::vector<float*>& pointers, Node::index_t index) const;

    float m_offset; //!< the offset to add in the GetResponse functions
    float m_sumWeights; //!< the sumOfBoostWeights--no need to recompute each call
    std::vector<Node::index_t> m_forest; //!< indices of the top-level nodes of each tree
    std::vector<float> m_weights; //!< boost weights
    std::vector<Node> m_nodes; //!< where the nodes of the forest are stored
    std::vector<float*> m_pointers; //!< where vars to cut on can be set (but can also be passed) 
  };
}

#endif
