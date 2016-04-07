// emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTBASETREETOOL_H
#define TRIGHLTBASETREETOOL_H


/**
 * @author Tomasz.Bold@cern.ch
 * @brief The tool adds to the tree a Lvl1Id and RoIId
 * There is currently one problem with the finalization.
 * The tree has to be registered first and then other tools
 * like TrigTimeTreeTool can attach the its own info to it.
 * Therefore the tree has to be there first. Filling is then 
 * another issue. The tree fill operation has to be done last.
 */


#include "TrigMonitorBase/TrigMonitorToolBase.h"

class TTree;
class TBranch;
namespace HLT {
  class Algo;
}
class TrigHLTBaseTreeTool : public TrigMonitorToolBase {
 public:
    TrigHLTBaseTreeTool(const std::string & type, 
			const std::string & name,
			const IInterface* parent);
    virtual ~TrigHLTBaseTreeTool();

    StatusCode bookHists();
    StatusCode fillHists();
    StatusCode finalHists();

private:

    struct PassData {
	unsigned int Lvl1Id;
	unsigned int RoIId;
    };
    PassData m_pass;
    std::string m_parentName;
    std::string m_path;
    HLT::Algo* m_algo;
    TTree *m_tree;      // tree where we want to fill
    int event;
};
#endif
