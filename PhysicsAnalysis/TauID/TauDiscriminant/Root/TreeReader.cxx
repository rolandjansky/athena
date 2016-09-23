/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TreeReader.h"
#include "TROOT.h"
#include "TNamed.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TF1.h"

using namespace std;


void findBadNodes(Node* node, vector<Node*>& badNodes)
{
    if (!node) return;
    LeafNode<float> dummyLeafNode;
    if (string(typeid(dummyLeafNode).name()).compare(typeid(*node).name())==0) return;
    UnivariateCut<float>* intNode = static_cast<UnivariateCut<float>*>(node);
    if (!intNode->isComplete()) badNodes.push_back(intNode);
    findBadNodes(node->getLeftChild(),badNodes);
    findBadNodes(node->getRightChild(),badNodes);
}

string signature(Node* node, vector<Node*>* badNodes, string hash = "")
{
    if(!node) return hash;
    if (badNodes)
    {
        if (find(badNodes->begin(),badNodes->end(),node) != badNodes->end()) hash += "[BAD]";
    }
    Node* child = node->getLeftChild();
    if (child) hash = signature(child,badNodes,hash+"1")+"2";
    child = node->getRightChild();
    if (child) hash = signature(child,badNodes,hash+"3")+"2";
    return hash;
}

void getLeafNodes(Node* node, vector<Node*>& leafNodes)
{
    if (!node) return;
    if (node->isLeaf())
    {
        leafNodes.push_back(node);
        return;
    }
    getLeafNodes(node->getLeftChild(),leafNodes);
    getLeafNodes(node->getRightChild(),leafNodes);
}

void getIncompleteNodes(Node* node, vector<Node*>& incompleteNodes)
{
    if (!node) return;
    if (!node->isComplete())
    {
        incompleteNodes.push_back(node);
    }
    getIncompleteNodes(node->getLeftChild(),incompleteNodes);
    getIncompleteNodes(node->getRightChild(),incompleteNodes);
}

bool hasEnding(const string& fullString, const string& ending)
{
    if (fullString.length() > ending.length())
    {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}

template<typename T>
bool read(istream& file, T& data, Format format)
{
    if (format == ASCII || format == ROOTFILE)
    {
        if ((file >> data).fail())
        {
            return false;
        }
    }
    else
    {
        file.read((char*)&data,sizeof(T));
    }
    return true;
}

bool good_file(TFile* file)
{
    if (!file)
        return false;
    return file->IsOpen() && !file->IsZombie();
}

Node* TreeReader::readTree(
    istream& treeFile,
    TFile* rootFile,
    Format format,
    vector<string>& variableList,
    vector<char>& variableTypeList,
    unsigned int& numNodes)
{
    int variable = 0;
    int idata;
    float fdata;
    string line;
    Node* rootNode(0);
    Node* node(0);
    stack<DecisionNode*> parentNodeStack;
    DecisionNode* parent(0);
    if (!read<int>(treeFile,variable,format))
    {
        ATH_MSG_VERBOSE("Failed extracting variable index from the discriminant file");
        return 0;
    }
    while (variable != ENDOFTREE)
    {
        if (variable == LEAF)
        {  // parsing leaf node
            if (!read<float>(treeFile,fdata,format))
            {
                ATH_MSG_VERBOSE("Failed extracting leaf node purity");
                return 0;
            }
            node = new LeafNode<float>(fdata);
        }
        else if (variable == POINTERLEAF)
        {
            node = new PointerLeafNode<TreeVector>();
        }
        else if(variable == GRAPH || variable == FUNC || variable == TRANS || variable == GRAPH2D) // Now reading a SlidingCut/Tranformation node
        {
            int numPoints = 0;
            string expression;
            string graphName;
            float lowX, highX;
            float lowY, highY;
            if (variable == GRAPH)
            {
                if (!read<int>(treeFile,numPoints,format))
                {
                    ATH_MSG_VERBOSE("Failed extracting number of points for a UnivariateSlidingCut1D or MultivariateSlidingCut2D node");
                    return 0;
                }
                if (numPoints < 2)
                {
                    ATH_MSG_VERBOSE("Invalid number of points in UnivariateSlidingCut1D node");
                    return 0;
                }
		if(numPoints > 50000){
		  ATH_MSG_ERROR("You have over 50k points, are you sure you want to do this? If so, raise the limit.");
		  return nullptr;
		}
            }
            else if (variable == FUNC)
            {
                treeFile >> expression;
                getline(treeFile,line); // Discard \n
            }
            else
            {
                if (!rootFile)
                {
                    ATH_MSG_VERBOSE("Requested a transformation but input file is not a ROOT file");
                    return 0;
                }
                if (!good_file(rootFile))
                {
                    ATH_MSG_VERBOSE("Input ROOT file is not open while trying to get transformation!");
                    return 0;
                }
                treeFile >> graphName;
                getline(treeFile,line); // Discard \n
            }
            int varX, varY, varZ;
            if (!read<int>(treeFile,varX,format))
            {
                ATH_MSG_VERBOSE("Failed extracting X for a UnivariateSlidingCut1D node");
                return 0;
            }
            if (variable == TRANS)
            {
                if(!read<float>(treeFile,lowX,format) || !read<float>(treeFile,highX,format))
                {
                    ATH_MSG_VERBOSE("Failed extracting bounds on X for transformation");
                    return 0;
                }
            }
            if (!read<int>(treeFile,varY,format))
            {
                ATH_MSG_VERBOSE("Failed extracting Y for a UnivariateSlidingCut1D node");
                return 0;
            }
            if (variable == TRANS)
            {
                if(!read<float>(treeFile,lowY,format) || !read<float>(treeFile,highY,format))
                {
                    ATH_MSG_VERBOSE("Failed extracting bounds on Y for transformation");
                    return 0;
                }
            }
	    if(variable == GRAPH2D){
	      if(!read<int>(treeFile,varZ,format)){
		ATH_MSG_VERBOSE("Failed to extract Z for the MultivariateSlidingCut2D");
		return 0;
	      }
	      if(varZ < 0){
		ATH_MSG_VERBOSE("Invalid Z for MultivariateSlidingCut2D");
		return 0;
	      }
	    }
	    
            if (varX < 0 || varY < 0)
            {
                ATH_MSG_VERBOSE("Invalid X or Y for UnivariateSlidingCut1D node");
                return 0;
            }
            TObject* graph;
            if (variable == GRAPH)
            {
                graph = new TGraph(numPoints);
            }
            else if (variable == FUNC)
            {
                graph = new TF1(expression.c_str(),expression.c_str(),0.,1.);
            }
            else
            {
                TGraph2D* tempgraph = dynamic_cast<TGraph2D*>(rootFile->Get(graphName.c_str()));
                if (!tempgraph)
                {
                    ATH_MSG_VERBOSE("Could not get transformation graph from ROOT file");
                    return 0;
                }
                //tempgraph = dynamic_cast<TGraph2D*>(tempgraph->Clone());
                graph = tempgraph;
            }

	    if(variable == GRAPH2D){ //deal with MultivariateSlidingCut2D separately here
	      //this is going to be messy...
	      if(variableTypeList[varX] == 'F'){
		map<string,const float*>::const_iterator it1(m_floatVariables->find(variableList[varX]));
		if(it1 == m_floatVariables->end()){
		  ATH_MSG_VERBOSE("Did not find variable "+variableList[varX]+" in float variables");
		  return 0;
		}
		if(variableTypeList[varY] == 'F'){
		  map<string,const float*>::const_iterator it2(m_floatVariables->find(variableList[varY]));
		  if(it2 == m_floatVariables->end()){
		    ATH_MSG_VERBOSE("Did not find variable "+variableList[varY]+" in float variables");
		    return 0;
		  }
		  if(variableTypeList[varZ] == 'F'){
		    map<string,const float*>::const_iterator it3(m_floatVariables->find(variableList[varZ]));
		    if(it3 == m_floatVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in float variables");
		      return 0;
		    }
		    node = new MultivariateSlidingCut2D<float, TGraph2D, float, float>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		  }else if(variableTypeList[varZ] == 'I'){
		    map<string,const int*>::const_iterator it3(m_intVariables->find(variableList[varZ]));
		    if(it3 == m_intVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in int variables");
		      return 0;
		    }
		    node = new MultivariateSlidingCut2D<int, TGraph2D, float, float>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		  }else{
		    ATH_MSG_VERBOSE("Unknown type");
		    return 0;
		  }
		}else if(variableTypeList[varY] == 'I'){
		  map<string,const int*>::const_iterator it2(m_intVariables->find(variableList[varY]));
		  if(it2 == m_intVariables->end()){
		    ATH_MSG_VERBOSE("Did not find variable "+variableList[varY]+" in int variables");
		    return 0;
		  }
		  if(variableTypeList[varZ] == 'F'){
		    map<string,const float*>::const_iterator it3(m_floatVariables->find(variableList[varZ]));
		    if(it3 == m_floatVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in float variables");
		      return 0;
		    }
		    node = new MultivariateSlidingCut2D<float, TGraph2D, float, int>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		  }else if(variableTypeList[varZ] == 'I'){
		    map<string,const int*>::const_iterator it3(m_intVariables->find(variableList[varZ]));
		    if(it3 == m_intVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in int variables");
		      return 0;
		    }
		    node = new MultivariateSlidingCut2D<int, TGraph2D, float, int>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		  }else{
		    ATH_MSG_VERBOSE("Unknown type");
		    return 0;
		  }
		}else{
		  ATH_MSG_VERBOSE("Unknown type");
		  return 0;
		}
	      }else if(variableTypeList[varX] == 'I'){
		map<string,const int*>::const_iterator it1(m_intVariables->find(variableList[varX]));
		if(it1 == m_intVariables->end()){
		  ATH_MSG_VERBOSE("Did not find variable "+variableList[varX]+" in int variables");
		  return 0;
		}
		if(variableTypeList[varY] == 'F'){
		  map<string,const float*>::const_iterator it2(m_floatVariables->find(variableList[varY]));
		  if(it2 == m_floatVariables->end()){
		    ATH_MSG_VERBOSE("Did not find variable "+variableList[varY]+" in float variables");
		    return 0;
		  }
		  if(variableTypeList[varZ] == 'F'){
		    map<string,const float*>::const_iterator it3(m_floatVariables->find(variableList[varZ]));
		    if(it3 == m_floatVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in float variables");
		      return 0;
		    }
		    node = new MultivariateSlidingCut2D<float, TGraph2D, int, float>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		  }else if(variableTypeList[varZ] == 'I'){
		    map<string,const int*>::const_iterator it3(m_intVariables->find(variableList[varZ]));
		    if(it3 == m_intVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in int variables");
		      return 0;
		    }
		    node = new MultivariateSlidingCut2D<int, TGraph2D, int, float>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		  }else{
		      ATH_MSG_VERBOSE("Unknown type");
		      return 0;
		    }
		  }else if(variableTypeList[varY] == 'I'){
		    map<string,const int*>::const_iterator it2(m_intVariables->find(variableList[varY]));
		    if(it2 == m_intVariables->end()){
		      ATH_MSG_VERBOSE("Did not find variable "+variableList[varY]+" in int variables");
		      return 0;
		    }
		    if(variableTypeList[varZ] == 'F'){
		      map<string,const float*>::const_iterator it3(m_floatVariables->find(variableList[varZ]));
		      if(it3 == m_floatVariables->end()){
			ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in float variables");
			return 0;
		      }
		      node = new MultivariateSlidingCut2D<float, TGraph2D, int, int>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		    }else if(variableTypeList[varZ] == 'I'){
		      map<string,const int*>::const_iterator it3(m_intVariables->find(variableList[varZ]));
		      if(it3 == m_intVariables->end()){
			ATH_MSG_VERBOSE("Did not find variable "+variableList[varZ]+" in int variables");
			return 0;
		      }
		      node = new MultivariateSlidingCut2D<int, TGraph2D, int, int>(it3->second, static_cast<TGraph2D*>(graph), it1->second, it2->second);
		    }else{
		      ATH_MSG_VERBOSE("Unknown type");
		      return 0;
		    }
		}else{
		  ATH_MSG_VERBOSE("Unknown type");
		  return 0;
		}
	      }else{
		ATH_MSG_VERBOSE("Unknown type");
		return 0;
	      }
	    }
            else if (variableTypeList[varX] == 'F' && variableTypeList[varY] == 'I')
            {
                map<string,const float*>::const_iterator it1(m_floatVariables->find(variableList[varX]));
                if (it1 == m_floatVariables->end())
                {
                    ATH_MSG_VERBOSE("A Did not find variable "+variableList[varX]+" in booked float variables!");
                    return 0;
                }
                map<string,const int*>::const_iterator it2(m_intVariables->find(variableList[varY]));
                if (it2 == m_intVariables->end())
                {
                    ATH_MSG_VERBOSE("B Did not find variable "+variableList[varY]+" in booked float variables!");
                    return 0;
                }
                if (variable == GRAPH)
                {
                    node = new UnivariateSlidingCut1D<int,TGraph,float>(it2->second,static_cast<TGraph*>(graph),it1->second);
                }
                else if (variable == FUNC)
                {
                    node = new UnivariateSlidingCut1D<int,TF1,float>(it2->second,static_cast<TF1*>(graph),it1->second);
                }
                else
                {
                    node = new TransformationNode<float,int,TGraph2D>(it1->second,lowX,highX,it2->second,lowY,highY,static_cast<TGraph2D*>(graph));
                }
            }
            else if (variableTypeList[varX] == 'I' && variableTypeList[varY] == 'F')
            { 
                map<string,const int*>::const_iterator it1(m_intVariables->find(variableList[varX]));
                if (it1 == m_intVariables->end())
                {
                    ATH_MSG_VERBOSE("Did not find variable "+variableList[varX]+" in booked int variables!");
                    return 0;
                }
                map<string,const float*>::const_iterator it2(m_floatVariables->find(variableList[varY]));
                if (it2 == m_floatVariables->end())
                {
                    ATH_MSG_VERBOSE("Did not find variable "+variableList[varY]+" in booked int variables!");
                    return 0;
                }
                if (variable == GRAPH)
                {
                    node = new UnivariateSlidingCut1D<float,TGraph,int>(it2->second,static_cast<TGraph*>(graph),it1->second);
                }
                else if (variable == FUNC)
                {
                    node = new UnivariateSlidingCut1D<float,TF1,int>(it2->second,static_cast<TF1*>(graph),it1->second);
                }
                else
                {
                    node = new TransformationNode<int,float,TGraph2D>(it1->second,lowX,highX,it2->second,lowY,highY,static_cast<TGraph2D*>(graph));
                }
            }
            else if (variableTypeList[varX] == 'F' && variableTypeList[varY] == 'F')
            { 
                map<string,const float*>::const_iterator it1(m_floatVariables->find(variableList[varX]));
                if (it1 == m_floatVariables->end())
                {
                    ATH_MSG_VERBOSE("D Did not find variable "+variableList[varX]+" in booked float variables!");
                    return 0;
                }
                map<string,const float*>::const_iterator it2(m_floatVariables->find(variableList[varY]));
                if (it2 == m_floatVariables->end())
                {
                    ATH_MSG_VERBOSE("E Did not find variable "+variableList[varY]+" in booked float variables!");
                    return 0;
                }
                if (variable == GRAPH)
                {
                    node = new UnivariateSlidingCut1D<float,TGraph,float>(it2->second,static_cast<TGraph*>(graph),it1->second);
                }
                else if (variable == FUNC)
                {
                    node = new UnivariateSlidingCut1D<float,TF1,float>(it2->second,static_cast<TF1*>(graph),it1->second);
                }
                else
                {
                    node = new TransformationNode<float,float,TGraph2D>(it1->second,lowX,highX,it2->second,lowY,highY,static_cast<TGraph2D*>(graph));
                }
            }
            else if (variableTypeList[varX] == 'I' && variableTypeList[varY] == 'I')
            {
                map<string,const int*>::const_iterator it1(m_intVariables->find(variableList[varX]));
                if (it1 == m_intVariables->end())
                {
                    ATH_MSG_VERBOSE("Did not find variable "+variableList[varX]+" in booked int variables!");
                    return 0;
                }
                map<string,const int*>::const_iterator it2(m_intVariables->find(variableList[varY]));
                if (it2 == m_intVariables->end())
                {
                    ATH_MSG_VERBOSE("Did not find variable "+variableList[varY]+" in booked int variables!");
                    return 0;
                }
                if (variable == GRAPH)
                {
                    node = new UnivariateSlidingCut1D<int,TGraph,int>(it2->second,static_cast<TGraph*>(graph),it1->second);
                }
                else if (variable == FUNC)
                {
                    node = new UnivariateSlidingCut1D<int,TF1,int>(it2->second,static_cast<TF1*>(graph),it1->second);
                }
                else
                {
                    node = new TransformationNode<int,int,TGraph2D>(it1->second,lowX,highX,it2->second,lowY,highY,static_cast<TGraph2D*>(graph));
                }
            }
            else
            {
                ATH_MSG_VERBOSE("Unsupported variable type in list found in discriminant file!");
                return 0;
            }
            if (variable == GRAPH)
            {
                TGraph* tgraph = static_cast<TGraph*>(graph);
                float X,Y;
                for (int k(0); k < numPoints; ++k )
                {
                    if (!read<float>(treeFile,X,format) || !read<float>(treeFile,Y,format))
                    {
                        ATH_MSG_VERBOSE("Failed extracting X,Y for a UnivariateSlidingCut1D node");
                        delete node;
                        delete rootNode;
                        return 0;
                    }
                    tgraph->SetPoint(k,X,Y);
                }
            }
        }
        else
        { // parsing internal node
            if (variableTypeList[variable] == 'F')\
            { // internal node cuts on float
                map<string,const float*>::const_iterator it(m_floatVariables->find(variableList[variable]));
                if (it == m_floatVariables->end())
                {
                    ATH_MSG_VERBOSE("F Did not find variable "+variableList[variable]+" in booked float variables!");
                    return 0;
                }
                if (!read<float>(treeFile,fdata,format))
                {
                    ATH_MSG_VERBOSE("Failed extracting internal float node cut");
                    return 0;
                }
                node = new UnivariateCut<float>(it->second,fdata);
            }
            else if (variableTypeList[variable] == 'I')
            { // internal node cuts on int
                map<string,const int*>::const_iterator it(m_intVariables->find(variableList[variable]));
                if (it == m_intVariables->end())
                {
                    ATH_MSG_VERBOSE("Did not find variable "+variableList[variable]+" in booked int variables!");
                    return 0;
                }
                if (!read<int>(treeFile,idata,format))
                {
                    ATH_MSG_VERBOSE("Failed extracting internal int node cut");
                    return 0;
                }
                node = new UnivariateCut<int>(it->second,idata);
            }
            else
            { // unknown node type
                ATH_MSG_VERBOSE("Unsupported variable type in list found in discriminant file!");
                return 0;
            }
        }
        ++numNodes;

        if (parentNodeStack.empty())
        {
            if (variable == LEAF)
            {
                ATH_MSG_VERBOSE("Corrupt tree! Adding leaf node as root node.");
                delete node;
                return 0;
            }
            rootNode = node;
        }
        else
        {
            parent = parentNodeStack.top();
            while (parent->isComplete())
            {
                parentNodeStack.pop();
                if (parentNodeStack.empty())
                {
                    ATH_MSG_VERBOSE("Corrupt tree! Expected a parent node.");
                    delete node;
                    delete rootNode;
                    return 0;
                }
                parent = parentNodeStack.top();
            }
            if (!parent->getLeftChild())
            {
                parent->setLeftChild(node);
            }
            else if (!parent->getRightChild())
            {
                parent->setRightChild(node);
            }
            else
            {
                ATH_MSG_VERBOSE("Corrupt tree! Attempted to add a child to a complete node!");
                delete node;
                delete rootNode;
                return 0;
            }
        }

        if (variable != LEAF)
        {
            parentNodeStack.push(static_cast<DecisionNode*>(node));
        }

        if (!read<int>(treeFile,variable,format))
        {
            ATH_MSG_VERBOSE("Failed extracting variable index");
            delete rootNode;
            return 0;
        }
    }
    while (!parentNodeStack.empty()) parentNodeStack.pop();
    return rootNode;
}

TreeReader::TreeReader(const string& filename):
  AsgMessaging("TauDiscriminant:TreeReader"),
  m_fileName(filename),
  m_floatVariables(nullptr),
  m_intVariables(nullptr),
  m_treeInfo(nullptr),
  m_treeFile(),
  m_treeString()
{
    if (hasEnding(m_fileName, ".bin"))
    {
        m_format = BINARY;
        ATH_MSG_VERBOSE("Reading input as binary");
    }
    else if (hasEnding(m_fileName, ".txt"))
    {
        m_format = ASCII;
        ATH_MSG_VERBOSE("Reading input as ASCII text");
    }
    else if (hasEnding(m_fileName, ".root"))
    {
        m_format = ROOTFILE;
        ATH_MSG_VERBOSE("Reading input as ROOT file");
    }
    else
    {
        m_format = UNKNOWN;
        ATH_MSG_ERROR("Unknown discriminant format");
	return;
    }

    unsigned int numVariables = 0;
    unsigned int numBinningVariables = 0;
    char type;
    string line;
    
    if (m_format == ROOTFILE)
    {
        // read in tree data from ROOT file and place in stringstream
        m_file = new TFile(m_fileName.c_str(), "READ");
        if (!good_file(m_file))
        {
            ATH_MSG_ERROR("The discriminant ROOT file "+m_fileName+" will not open!");
            return;
        }
        TNamed* treeInfoText = (TNamed*)m_file->Get("TreeInfo");
        if (!treeInfoText)
        {
            ATH_MSG_ERROR("Could not find TreeInfo in discriminant ROOT file!");
            m_file->Close();
            return;
        }
        string treeInfoTextString(treeInfoText->GetTitle());
	m_treeString.str(treeInfoTextString);
        delete treeInfoText;
        m_treeInfo = &m_treeString;
    }
    else
    {
        m_treeFile.open(m_fileName.c_str(),ios::in);
        if (!m_treeFile.is_open())
        {
            ATH_MSG_ERROR("The discriminant file "+m_fileName+" will not open!");
            return;
        }
        m_treeInfo = &m_treeFile;
    }

    if ((*m_treeInfo >> numBinningVariables).fail())
    {
        ATH_MSG_ERROR("Failed extracting the number of binning variables in the discriminant file!");
        return;
    }

    //input validation to make Coverity happy
    if(numBinningVariables > 10000){
      ATH_MSG_ERROR("You have over 10k binning variables, are you sure you want to do this? If so, raise the limit.");
      return;
    }
    
    m_binningVariableList = vector<string>(numBinningVariables,"");
    m_binningVariableTypeList = vector<char>(numBinningVariables,'F');

    for (unsigned int j(0); j < numBinningVariables; ++j)
    {
        if ((*m_treeInfo >> m_binningVariableList[j]).fail())
        {
            ATH_MSG_ERROR("Failed extracting a variable name from the discriminant file");
            return;
        }
        if ((*m_treeInfo >> type).fail())
        {
            ATH_MSG_ERROR("Failed extracting a variable type from the discriminant file");
            return;
        }
        if (type != 'F' && type != 'I')
        {
            ATH_MSG_ERROR("Unsupported variable type found in the discriminant file: "+std::to_string(type));
            return;
        }
        m_binningVariableTypeList[j] = type;
    }
    getline(*m_treeInfo,line); // discard \n

    //save stream pos with category tree info and skip
    if (numBinningVariables > 0){
      m_catTreePos = m_treeInfo->tellg();
      int variable = 0;
      do{
	if (!read<int>(*m_treeInfo, variable, ASCII))
	  {
	    ATH_MSG_ERROR("Failed extracting variable index from the discriminant file");
	    return;
	  }
	getline(*m_treeInfo, line);
      }while (variable != ENDOFTREE);
    }

    if ((*m_treeInfo >> numVariables).fail())
    {
        ATH_MSG_ERROR("Failed extracting number of variables from the discriminant file!");
        return;
    }

    //input validation
    if(numVariables > 10000){
      ATH_MSG_ERROR("You have over 10k variables, are you sure you want to do this? If so, raise the limit.");
      return;
    }

    m_variableList = vector<string>(numVariables,"");
    m_variableTypeList = vector<char>(numVariables,'F');


    for (unsigned int j(0); j < numVariables; ++j)
    {
        if ((*m_treeInfo >> m_variableList[j]).fail())
        {
            ATH_MSG_ERROR("Failed extracting a variable name from the discriminant file");
            return;
        }
        if ((*m_treeInfo >> type).fail())
        {
            ATH_MSG_ERROR("Failed extracting a variable type from the discriminant file");
            return;
        }
        if (type != 'F' && type != 'I')
        {
            ATH_MSG_ERROR("Unsupported variable type found in the discriminant file: "+std::to_string(type));
            return;
        }
        m_variableTypeList[j] = type;
    }
    getline(*m_treeInfo,line); // discard \n
    
    m_constructionFinished = true;
}

Node* TreeReader::build(bool checkTree)
{
    Node* categoryTree;
    Node* rootNode;
    vector<string> tokens;
    string token;
    unsigned int numNodes(0);

    if (m_format == UNKNOWN){
      ATH_MSG_ERROR("Unknown discriminant format");
      return nullptr;
    }

    if(!m_constructionFinished){
      ATH_MSG_ERROR("TreeReader construction failed. Not constructing tree.");
      return nullptr;
    }  
    
    if (m_catTreePos > -1)
    {
        int save_pos = m_treeInfo->tellg();
        m_treeInfo->seekg(m_catTreePos);
        categoryTree = readTree(*m_treeInfo, m_file, ASCII, m_binningVariableList, m_binningVariableTypeList, numNodes);
	m_treeInfo->seekg(save_pos);
    }
    else
    {
        categoryTree = new PointerLeafNode<TreeVector>();
    }

    // Get vector of all leaf nodes of the category tree and initialize an iterator
    vector<Node*> categories;
    vector<Node*>::const_iterator category_it, category_end;
    getLeafNodes(categoryTree, categories);
    category_it = categories.begin();
    category_end = categories.end();
    unsigned int numTrees = 0;
    float weight;

    // Build the (categorized) TreeVector
    TreeVector* treeVector;
    for (;category_it != category_end; ++category_it)
    {
        numNodes = 0;
        treeVector = new TreeVector();
        static_cast<PointerLeafNode<TreeVector>* >(*category_it)->setValue(treeVector);
        if (!read<unsigned int>(*m_treeInfo, numTrees, m_format)) ATH_MSG_VERBOSE("Failed extracting number of trees from the discriminant file!");
	if(numTrees > 50000){
	  ATH_MSG_ERROR("You have over 50k trees, are you sure you want to do this? If so, raise the limit.");
	  return nullptr;
	}
        for (unsigned int j(0); j < numTrees; ++j)
        {
            rootNode = 0;

            if (!read<float>(*m_treeInfo, weight, m_format))
            {
                ATH_MSG_VERBOSE("Failed extracting tree weight from the discriminant file");
                delete categoryTree;
                return nullptr;
            }
            rootNode = readTree(*m_treeInfo, m_file, m_format, m_variableList, m_variableTypeList, numNodes);
            if (!rootNode)
            {
                ATH_MSG_VERBOSE("Null tree!");
                delete categoryTree;
                return nullptr;
            }
            if (checkTree)
            {
                vector<Node*> badNodes;
                findBadNodes(rootNode,badNodes);
                if (badNodes.size()>0)
                {
                    ATH_MSG_VERBOSE("Tree is not well-formed!");
                    ATH_MSG_VERBOSE("Bad tree has signature: "+signature(rootNode,&badNodes));
                    delete categoryTree;
                    return nullptr;
                }
            }
            treeVector->addTree(rootNode,weight);
        }
        ATH_MSG_VERBOSE("Created a tree vector with "+to_string(numNodes)+" nodes");
    }
    m_treeFile.close();
    if (m_file)
    {
        //m_file->Close();
    }
    //delete m_file;
    return categoryTree;
}

vector<string> TreeReader::getRequiredVariables(char type){
  vector<string> variableNames;
  for(unsigned int i = 0; i < m_binningVariableList.size(); ++i){
    if(m_binningVariableTypeList[i] == type){
      variableNames.push_back(m_binningVariableList[i]);
    }
  }
  for(unsigned int i = 0; i < m_variableList.size(); ++i){
    if(m_variableTypeList[i] == type){
      variableNames.push_back(m_variableList[i]);
    }
  }
  return variableNames;
}
