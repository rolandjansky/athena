/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCellList_H
#define CaloCellList_H

/*  Selecting Cells from a Container of CaloCells
                                        H. Ma  Sept 2000
    The constructor takes a pointer to  ObjectVector<CaloCell>
    Two selection implemented:
       delta_eta, delta_phi around a fix eta,phi, and
       a cone of size dR around a fixed eta,phi
    An optional sampling layer requirement.
    Using the templated method, Tselect, user can write a
    function object with the operator()(CaloCell*) to select
    the wanted cells.
    Each select call clears previous selection.
    methods et() and energy() return et and energy.
    list of cells can also be accessed through iterators.

   Mod 17 Jun 2004 David Rousseau: work off big CaloCellContainer

   Mod 21 Sep 2004 Damir Lelas: -speeding up using "CaloDetDescr/CaloDetDescrManager" class.
                                -vector of cells added to an exsisting list of cells

   Mod 11 Oct 2004 Damir Lelas: -new method "select_nstrips" added; possibility to get number
                                 of strips around given (eta,phi) direction for
                                 (ncell_eta,ncell_phi) specified. The List of cells replaced
                                 by the Vector of cells.

*/

class CaloCellContainer;
class CaloCell;
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

class CaloCellList
{

public:
  typedef std::vector<const CaloCell*> vector_type;
  typedef vector_type::const_iterator list_iterator;

  //   CaloCellList();
  CaloCellList(const CaloCellContainer* cell_container);
  CaloCellList(const CaloCellContainer* cell_container, const CaloCell_ID::SUBCALO caloNum);
  CaloCellList(const CaloCellContainer* cell_container, const std::vector<CaloCell_ID::SUBCALO>& caloNums);

  ~CaloCellList();

  // deta/dphi are the half-widths of the window.
  // That is, we select cells within eta-deta .. eta+deta and similarly for
  // phi.

  void select(double eta, double phi, double deta, double dphi);
  void select(double eta, double phi, double deta, double dphi, int sam);
  void select(double eta, double phi, double dR);
  void select(double eta, double phi, double dR, int sam);

  // Add overload accepting const CaloDetDescrManager&
  void select(const CaloDetDescrManager& mgr, double eta, double phi, double deta, double dphi);

  void select(const CaloDetDescrManager& mgr, double eta, double phi, double deta, double dphi, int sam);

  void select(const CaloDetDescrManager& mgr, double eta, double phi, double dR);

  void select(const CaloDetDescrManager& mgr, double eta, double phi, double dR, int sam);

  list_iterator begin() const;
  list_iterator end() const;

  double energy() const; // returns the energy of the selected cells
  double et() const;     // returns the et of the selected cells
  int ncells() const;    // returns the number of cells

private:
  void doSelect(const CaloDetDescrManager& mgr,
                double eta,
                double phi,
                double deta,
                double dphi,
                double dR,
                CaloCell_ID::CaloSample sam = CaloCell_ID::Unknown);

  const CaloCellContainer* m_cellcont;
  std::vector<CaloCell_ID::SUBCALO> m_caloNums;
  vector_type m_theCellVector;
  double m_energy;
  double m_et;
};

inline CaloCellList::list_iterator
CaloCellList::begin() const
{
  return m_theCellVector.begin();
}

inline CaloCellList::list_iterator
CaloCellList::end() const
{
  return m_theCellVector.end();
}

inline double
CaloCellList::energy() const
{
  return m_energy;
}

inline double
CaloCellList::et() const
{
  return m_et;
}

inline int
CaloCellList::ncells() const
{
  return m_theCellVector.size();
}

#endif

