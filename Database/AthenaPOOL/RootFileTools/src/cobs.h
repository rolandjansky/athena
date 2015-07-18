/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>

#include <TTree.h>

class cobs {

public:
        
    cobs()
      : m_useCINTlibrary(false),
        m_compressionLevel(0),
        m_splitLevel(0),
        m_dedicatedMemory(0),
        m_minCompressionFactor(0),
        m_debug(false),
        m_showBranches(false)
    {}
    cobs(std::string inputFile);
    cobs(std::string inputFile, std::string outputFile);

    void setDebug(bool debug);
    void setOutputFile(std::string outputFile);
    void setCompressionLevel(short CL);
    void setSplitLevel(short SL);
    void setDedicatedMemory(int DM); // in kilobytes
    void setDebugLevel(short DL);
    void setTreeToSkip(std::string treeName);
    void setTreeMemory(int mem, std::string treeName);
    void setMinCompressionFactor(float minComp);
    void setLibraryName(std::string);
    void useCINTLibrary(bool use=false);
    void mkProject();
                   
    // clone fast to reorder baskets (default: by entry)
    bool order(short orderBy=2);
    bool resizeBaskets();
    void changeSplitLevel();
                
    void  printTrees(std::string fileName);
    Int_t printBranches(TTree *T);
    void  setShowBranches(bool b) { m_showBranches = b; }
                               
private:
                
    void SimpleOptimizeBaskets(TTree*, unsigned long, float );
    bool RecreateTheTree(TTree* );
                
    std::string m_libraryName;
    std::string m_libraryDirName;
    bool        m_useCINTlibrary;
    std::string m_inputFilename;
    std::string m_outputFilename;
    short       m_compressionLevel;
    short       m_splitLevel;
    int         m_dedicatedMemory; // in kb's
    float       m_minCompressionFactor; // don't compress branches with CF smaller than this number
    bool        m_debug;
    std::vector<std::string> m_treesToSkip;
    std::vector<std::pair<int, std::string> > m_memoryForATree;
                
    bool        m_showBranches;
                
};
