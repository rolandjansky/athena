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

bool isGoodTree(Node* node)
{
    if (!node) return false;
    LeafNode<float> dummyLeafNode;
    if (string(typeid(dummyLeafNode).name()).compare(typeid(*node).name())==0) return true;
    return isGoodTree(node->getLeftChild()) && isGoodTree(node->getRightChild());
}

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

void attachTree(Node* tree, Node* subtree)
{
    if (!tree || !subtree) return;
    vector<Node*> incompleteNodes;
    getIncompleteNodes(tree, incompleteNodes);
    vector<Node*>::iterator it(incompleteNodes.begin());
    for (;it != incompleteNodes.end(); ++it)
    {
        if(!(*it)) return;
        if(!(*it)->getLeftChild()) static_cast<DecisionNode*>(*it)->setLeftChild(subtree->clone());
        if(!(*it)->getRightChild()) static_cast<DecisionNode*>(*it)->setRightChild(subtree->clone());
    }
}

Node* createBalancedTree(const vector<DecisionNode*>& nodes)
{
    if (nodes.size() == 0) return 0;
    DecisionNode* root = nodes.at(nodes.size()/2);
    if (!root) return 0;
    vector<DecisionNode*>::const_iterator midpoint(nodes.begin()+nodes.size()/2);
    vector<DecisionNode*> leftTree(nodes.begin(),midpoint);
    vector<DecisionNode*> rightTree(midpoint+1,nodes.end());
    root->setLeftChild(createBalancedTree(leftTree));
    root->setRightChild(createBalancedTree(rightTree));
    return root;
}

Node* createBinningTree(const void* variable, char type, const vector<float>& binCuts)
{
    vector<float>::const_iterator it(binCuts.begin());
    vector<DecisionNode*> nodes;
    for (;it != binCuts.end(); ++it)
    {
        if (type == 'F')
        {
            nodes.push_back(new UnivariateCut<float>((float*)variable,*it));
        }
        else if (type == 'I')
        {
            nodes.push_back(new UnivariateCut<int>((int*)variable,(int)*it));
        }
        else
        {
            return 0;
        }
    }
    return createBalancedTree(nodes);
}

Node* buildCategoryTree(const vector<Node*>& binningTrees)
{
    vector<Node*>::const_iterator it(binningTrees.begin());
    Node* tree = *(it++);
    if (!tree) return 0;
    tree = tree->clone();
    for (;it != binningTrees.end(); ++it)
    {
        attachTree(tree,*(it));
    }
    vector<Node*> incompleteNodes;
    getIncompleteNodes(tree,incompleteNodes);
    it = incompleteNodes.begin();
    for (;it != incompleteNodes.end(); ++it)
    {
        if(!(*it)) return 0;
        if(!(*it)->getLeftChild()) static_cast<DecisionNode*>(*it)->setLeftChild(new PointerLeafNode<TreeVector>());
        if(!(*it)->getRightChild()) static_cast<DecisionNode*>(*it)->setRightChild(new PointerLeafNode<TreeVector>());
    }
    return tree;
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
    int variable;
    int idata;
    float fdata;
    string line;
    Node* rootNode(0);
    Node* node(0);
    stack<DecisionNode*> parentNodeStack;
    DecisionNode* parent(0);
    if (!read<int>(treeFile,variable,format))
    {
        print("Failed extracting variable index from the discriminant file");
        return 0;
    }
    while (variable != ENDOFTREE)
    {
        if (variable == LEAF)
        {  // parsing leaf node
            if (!read<float>(treeFile,fdata,format))
            {
                print("Failed extracting leaf node purity");
                return 0;
            }
            node = new LeafNode<float>(fdata);
        }
        else if (variable == POINTERLEAF)
        {
            node = new PointerLeafNode<TreeVector>();
        }
        else if(variable == GRAPH || variable == FUNC || variable == TRANS) // Now reading a UnivariateSlidingCut1D/Tranformation node
        {
            int numPoints;
            string expression;
            string graphName;
            float lowX, highX;
            float lowY, highY;
            if (variable == GRAPH)
            {
                if (!read<int>(treeFile,numPoints,format))
                {
                    print("Failed extracting number of points for a UnivariateSlidingCut1D node");
                    return 0;
                }
                if (numPoints < 2)
                {
                    print("Invalid number of points in UnivariateSlidingCut1D node");
                    return 0;
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
                    print("Requested a transformation but input file is not a ROOT file");
                    return 0;
                }
                if (!good_file(rootFile))
                {
                    print("Input ROOT file is not open while trying to get transformation!");
                    return 0;
                }
                treeFile >> graphName;
                getline(treeFile,line); // Discard \n
            }
            int varX, varY;
            if (!read<int>(treeFile,varX,format))
            {
                print("Failed extracting X for a UnivariateSlidingCut1D node");
                return 0;
            }
            if (variable == TRANS)
            {
                if(!read<float>(treeFile,lowX,format) || !read<float>(treeFile,highX,format))
                {
                    print("Failed extracting bounds on X for transformation");
                    return 0;
                }
            }
            if (!read<int>(treeFile,varY,format))
            {
                print("Failed extracting Y for a UnivariateSlidingCut1D node");
                return 0;
            }
            if (variable == TRANS)
            {
                if(!read<float>(treeFile,lowY,format) || !read<float>(treeFile,highY,format))
                {
                    print("Failed extracting bounds on Y for transformation");
                    return 0;
                }
            }
            if (varX < 0 || varY < 0)
            {
                print("Invalid X or Y for UnivariateSlidingCut1D node");
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
                    print("Could not get transformation graph from ROOT file");
                    return 0;
                }
                //tempgraph = dynamic_cast<TGraph2D*>(tempgraph->Clone());
                graph = tempgraph;
            }
            if (variableTypeList[varX] == 'F' && variableTypeList[varY] == 'I')
            {
                map<string,const float*>::const_iterator it1(floatVariables->find(variableList[varX]));
                if (it1 == floatVariables->end())
                {
                    print("A Did not find variable "+variableList[varX]+" in booked float variables!");
                    return 0;
                }
                map<string,const int*>::const_iterator it2(intVariables->find(variableList[varY]));
                if (it2 == intVariables->end())
                {
                    print("B Did not find variable "+variableList[varY]+" in booked float variables!");
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
                map<string,const int*>::const_iterator it1(intVariables->find(variableList[varX]));
                if (it1 == intVariables->end())
                {
                    print("Did not find variable "+variableList[varX]+" in booked int variables!");
                    return 0;
                }
                map<string,const float*>::const_iterator it2(floatVariables->find(variableList[varY]));
                if (it2 == floatVariables->end())
                {
                    print("Did not find variable "+variableList[varY]+" in booked int variables!");
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
                map<string,const float*>::const_iterator it1(floatVariables->find(variableList[varX]));
                if (it1 == floatVariables->end())
                {
                    print("D Did not find variable "+variableList[varX]+" in booked float variables!");
                    return 0;
                }
                map<string,const float*>::const_iterator it2(floatVariables->find(variableList[varY]));
                if (it2 == floatVariables->end())
                {
                    print("E Did not find variable "+variableList[varY]+" in booked float variables!");
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
                map<string,const int*>::const_iterator it1(intVariables->find(variableList[varX]));
                if (it1 == intVariables->end())
                {
                    print("Did not find variable "+variableList[varX]+" in booked int variables!");
                    return 0;
                }
                map<string,const int*>::const_iterator it2(intVariables->find(variableList[varY]));
                if (it2 == intVariables->end())
                {
                    print("Did not find variable "+variableList[varY]+" in booked int variables!");
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
                print("Unsupported variable type in list found in discriminant file!");
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
                        print("Failed extracting X,Y for a UnivariateSlidingCut1D node");
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
                map<string,const float*>::const_iterator it(floatVariables->find(variableList[variable]));
                if (it == floatVariables->end())
                {
                    print("F Did not find variable "+variableList[variable]+" in booked float variables!");
                    return 0;
                }
                if (!read<float>(treeFile,fdata,format))
                {
                    print("Failed extracting internal float node cut");
                    return 0;
                }
                node = new UnivariateCut<float>(it->second,fdata);
            }
            else if (variableTypeList[variable] == 'I')
            { // internal node cuts on int
                map<string,const int*>::const_iterator it(intVariables->find(variableList[variable]));
                if (it == intVariables->end())
                {
                    print("Did not find variable "+variableList[variable]+" in booked int variables!");
                    return 0;
                }
                if (!read<int>(treeFile,idata,format))
                {
                    print("Failed extracting internal int node cut");
                    return 0;
                }
                node = new UnivariateCut<int>(it->second,idata);
            }
            else
            { // unknown node type
                print("Unsupported variable type in list found in discriminant file!");
                return 0;
            }
        }
        ++numNodes;

        if (parentNodeStack.empty())
        {
            if (variable == LEAF)
            {
                print("Corrupt tree! Adding leaf node as root node.");
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
                    print("Corrupt tree! Expected a parent node.");
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
                print("Corrupt tree! Attempted to add a child to a complete node!");
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
            print("Failed extracting variable index");
            delete rootNode;
            return 0;
        }
    }
    while (!parentNodeStack.empty()) parentNodeStack.pop();
    return rootNode;
}

Node* TreeReader::build(
    const string& filename,
    bool checkTree)
{

    Format format;
    if (hasEnding(filename, ".bin"))
    {
        format = BINARY;
        if (verbose) print("Reading input as binary");
    }
    else if (hasEnding(filename, ".txt"))
    {
        format = ASCII;
        if (verbose) print("Reading input as ASCII text");
    }
    else if (hasEnding(filename, ".root"))
    {
        format = ROOTFILE;
        if (verbose) print("Reading input as ROOT file");
    }
    else
    {
        print("Unknown discriminant format");
        return 0;
    }

    unsigned int numVariables;
    Node* categoryTree;
    Node* rootNode;
    vector<string> tokens;
    string line;
    string token;
    vector<string> variableList;
    vector<char> variableTypeList;
    vector<string> binningVariableList;
    vector<char> binningVariableTypeList;
    char type;
    unsigned int numNodes(0);

    istream* treeInfoTemp;
    ifstream treeFile;
    istringstream treeString;
    TFile* file(0);

    if (format == ROOTFILE)
    {
        // read in tree data from ROOT file and place in stringstream
        file = new TFile(filename.c_str(), "READ");
        if (!good_file(file))
        {
            print("The discriminant ROOT file "+filename+" will not open!");
            return 0;
        }
        TNamed* treeInfoText = (TNamed*)file->Get("TreeInfo");
        if (!treeInfoText)
        {
            print("Could not find TreeInfo in discriminant ROOT file!");
            file->Close();
            return 0;
        }
        string treeInfoTextString(treeInfoText->GetTitle());
        treeString.str(treeInfoTextString);
        delete treeInfoText;
        treeInfoTemp = &treeString;
    }
    else
    {
        treeFile.open(filename.c_str(),ios::in);
        if (!treeFile.is_open())
        {
            print("The discriminant file "+filename+" will not open!");
            return 0;
        }
        treeInfoTemp = &treeFile;
    }
    istream& treeInfo(*treeInfoTemp);

    unsigned int numBinningVariables = 0;
    if ((treeInfo >> numBinningVariables).fail())
    {
        print("Failed extracting the number of binning variables in the discriminant file!");
        return 0;
    }
    
    binningVariableList = vector<string>(numBinningVariables,"");
    binningVariableTypeList = vector<char>(numBinningVariables,'F');

    for (unsigned int j(0); j < numBinningVariables; ++j)
    {
        if ((treeInfo >> binningVariableList[j]).fail())
        {
            print("Failed extracting a variable name from the discriminant file");
            return 0;
        }
        if ((treeInfo >> type).fail())
        {
            print("Failed extracting a variable type from the discriminant file");
            return 0;
        }
        if (type != 'F' && type != 'I')
        {
            print("Unsupported variable type found in the discriminant file: "+type);
            return 0;
        }
        binningVariableTypeList[j] = type;
    }
    getline(treeFile,line); // discard \n

    if (numBinningVariables > 0)
    {
        categoryTree = readTree(treeInfo,file,ASCII,binningVariableList,binningVariableTypeList,numNodes); 
    }
    else
    {
        categoryTree = new PointerLeafNode<TreeVector>();
    }

    if ((treeInfo >> numVariables).fail())
    {
        print("Failed extracting number of variables from the discriminant file!");
        delete categoryTree;
        return 0;
    }

    variableList = vector<string>(numVariables,"");
    variableTypeList = vector<char>(numVariables,'F');

    for (unsigned int j(0); j < numVariables; ++j)
    {
        if ((treeInfo >> variableList[j]).fail())
        {
            print("Failed extracting a variable name from the discriminant file");
            delete categoryTree;
            return 0;
        }
        if ((treeInfo >> type).fail())
        {
            print("Failed extracting a variable type from the discriminant file");
            delete categoryTree;
            return 0;
        }
        if (type != 'F' && type != 'I')
        {
            print("Unsupported variable type found in the discriminant file: "+type);
            delete categoryTree;
            return 0;
        }
        variableTypeList[j] = type;
    }
    getline(treeInfo,line); // discard \n

    // Get vector of all leaf nodes of the category tree and initialize an iterator
    vector<Node*> categories;
    vector<Node*>::const_iterator category_it, category_end;
    getLeafNodes(categoryTree, categories);
    category_it = categories.begin();
    category_end = categories.end();

    unsigned int numTrees;
    float weight;

    // Build the (categorized) TreeVector
    TreeVector* treeVector;
    for (;category_it != category_end; ++category_it)
    {
        numNodes = 0;
        treeVector = new TreeVector();
        static_cast<PointerLeafNode<TreeVector>* >(*category_it)->setValue(treeVector);

        if (!read<unsigned int>(treeInfo,numTrees,format)) print("Failed extracting number of trees from the discriminant file!");

        for (unsigned int j(0); j < numTrees; ++j)
        {
            rootNode = 0;

            if (!read<float>(treeInfo,weight,format))
            {
                print("Failed extracting tree weight from the discriminant file");
                delete categoryTree;
                return 0;
            }
            rootNode = readTree(treeInfo,file,format,variableList,variableTypeList,numNodes); 
            if (!rootNode)
            {
                print("Null tree!");
                delete categoryTree;
                return 0;
            }
            if (checkTree)
            {
                vector<Node*> badNodes;
                findBadNodes(rootNode,badNodes);
                if (badNodes.size()>0)
                {
                    print("Tree is not well-formed!");
                    print("Bad tree has signature: "+signature(rootNode,&badNodes));
                    delete categoryTree;
                    return 0;
                }
            }
            treeVector->addTree(rootNode,weight);
        }
        if (verbose) print("Created a tree vector with "+to_string(numNodes)+" nodes");
    }
    treeFile.close();
    if (file)
    {
        //file->Close();
    }
    //delete file;
    return categoryTree;
}
