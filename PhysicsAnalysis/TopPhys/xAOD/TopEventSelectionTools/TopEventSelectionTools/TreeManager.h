/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TREEMANAGER_H_
#define TREEMANAGER_H_

#include "TTree.h"

#include <string>
#include <list>
#include <sstream>
#include <functional>
#include <vector>

class TFile;

namespace top {

/**
 * @brief A class that hopefully makes making a flat ntuple tree a bit easier
 * since you don't need to worry about what type to make the branches and stuff.
 *
 * Almost all this code is stolen from SFrame:
 * http://sourceforge.net/p/sframe
 */
class TreeManager {
public:
    /**
     * @brief Default constructor - note need to initialize the class if you use this.
     */
    TreeManager();

    /**
     * @brief You need to construct this at the start (in the same way as for
     * the plot manager.
     *
     * @param file The file to write the tree to.
     * @param name The name of the tree (e.g. nominal, systematic1 etc).
     */
    TreeManager(const std::string& name, TFile* outputFile,const bool setAutoFlushZero = false);

    ///We do allow you to move it
    TreeManager(const TreeManager&& rhs);

    ///But not to copy it.
    TreeManager(const TreeManager&)= delete;

    ///Or assign it.
    TreeManager& operator=(TreeManager const&) = delete;

    /**
     * @brief Initialize the class with a new output file
     */
    void initialize(const std::string& name, TFile* outputFile,const bool setAutoFlushZero = false);

    /**
     * @brief Calls TTree::Fill.  Needed to fill the tree.
     */
    void fill();
    
    /**
     * @brief name of the TTree
     */
    const std::string& name() const;

    /** @brief function object type used for branch filters */
    typedef std::function<int(TreeManager const *, std::string const &)> BranchFilter;

    /** @copydoc TreeManager::m_branchFilters */
    std::vector<BranchFilter> & branchFilters() { return m_branchFilters; }

    /** @copydoc TreeManager::m_branchFilters */
    std::vector<BranchFilter> const & branchFilters() const { return m_branchFilters; }

    /**
     * @brief The idea is to simplify the creation of branches so that you don't
     * have to write tree->Branch("blah", &blah, "blah/I") because the computer
     * can figure out the "blah/I" bit for you.
     *
     * Stolen from SFrame.
     *
     * @param obj The object that you would like to save in a branch.  This can
     * be a primitive type (e.g. int, unsigned int, etc.) or an object (e.g.
     * std::vector<float>).
     * @param name The name you would like to give this branch.
     */
    template<class T>
    void makeOutputVariable(T& obj, const std::string name) {
        for (auto branchFilter : branchFilters()) {
            int status = branchFilter(this, name);
            if (status > 0)
                break;
            if (status == 0)
                return;
        }
        const char* type_name = typeid(obj).name();
        if (strlen( type_name ) == 1) {
            std::ostringstream leaflist;
            leaflist << name << "/" << RootType( type_name );
            m_tree->Branch(name.c_str(), &obj, leaflist.str().c_str());
        } else {
            m_outputVarPointers.push_back( &obj );
            T** pointer = reinterpret_cast< T** >( &m_outputVarPointers.back() );
            m_tree->Branch( name.c_str(), pointer );
        }
    }

private:
    /** 
     * name of the tree
     */
    std::string m_name;
  
  
    /**
     * @brief Stolen from SFrame code.
     *
     * This is a tricky one. In SCycleBaseNTuple::DeclareVariable(...) the function
     * automatically detects the type of the variable to be put into the output
     * tree. Since ROOT uses a different naming scheme for the primitives than C++'s
     * typeid call, the typeid names have to be translated for ROOT. This is the
     * function doing that.
     *
     * @warning The translation is probably only valid on various UNIX systems,
     *          probably doesn't work on Windows. (Did anyone ever try SFrame on
     *          Windows anyway???)
     *
     * @param typeid_type Primitive type name in the "typeid" format
     * @returns The primitive type name in ROOT's format
     */
    const char* RootType(const char* typeid_type);

    /**
     * @brief Stolen from SFrame code.
     *
     * This function is used internally to check whether the user tries to connect
     * the correct type of primitive to the branches. ROOT can have some trouble
     * identifying such code problems...
     *
     * @warning The implementation might be platform specific!
     *
     * @param root_type ROOT primitive type name
     * @returns The typeid type name for the ROOT primitive type
     */
    const char* TypeidType(const char* root_type);

    ///Pointer to the TTree, created by this class in the constructor.
    TTree* m_tree;

    /**
     * @brief Stolen from SFrame code.
     *
     * We have to keep the pointers to the output variables defined by the user.
     * ROOT keeps track of the objects by storing pointers to pointers to the
     * objects. Since the user probably wants to use the output objects directly
     * and not through pointers, the base class has to take care of this
     * pointer issue by itself...
     */
    std::list< void* > m_outputVarPointers;

    /**
     * @brief list of functions that control which variables are written out
     *
     * The function objects are called in order, with the TreeManager instance
     * and the variable name as parameters. A return value of 1 (0) tells the
     * TreeManager that the variable should (not) be written to the n-tuple. If
     * the function returns -1, the decision is left to the next function in the
     * list. If there are no functions in the list, or all functions return -1,
     * the variable is written to the n-tuple.
     */
    std::vector<BranchFilter> m_branchFilters;

};

}

#endif
