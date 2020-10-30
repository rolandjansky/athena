///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMOMENTTOOLS_MISSINGCALOCELLLISTTOOL_H
#define JETMOMENTTOOLS_MISSINGCALOCELLLISTTOOL_H 1
/////////////////////////////////////////////////////////////////// 
/// MissingCellListTool.h 
/// Header file for class MissingCellListTool
/// Author: P.A. Delsart
/// \class MissingCellListTool
/// Builds a missing/bad calo cell map to be used by JetBadChanCorrTool
///
/// MissingCellListTool is an IJetExecuteTool which builds and record 
/// a list of calo cell. This list is also a special geometrical map
/// which allow to efficiently retrieve all cells around a given (eta,phi) direction.
///
/// Some options govern wich cells enter the list : typically permanently disconnected
/// cells, "bad cells" in the sens of CaloCell::badcell(), or user given cells.
/// 
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <string>
#include <list>
#include <unordered_set>

// FrameWork includes
//#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TileConditions/TileCablingSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"

#include "JetUtils/TiledEtaPhiMap.h"

#include "xAODCore/CLASS_DEF.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"

// Forward declaration

class CaloCell;
class CaloDetDescrManager;
class ITileBadChanTool ;

namespace jet {
  ///////////////////////////////////////////////////  
  /// \class CellPosition representation of a cell position for the geometric map
  /// of missing/bad  cells
  ///////////////////////////////////////////////////  
  struct CellPosition {
    typedef CaloCell_ID::CaloSample CaloSample;
    
    CellPosition(){};
    CellPosition(double eta, double phi): m_eta(eta),m_phi(phi),m_sampling(),m_id(){};
    CellPosition(double eta, double phi, Identifier id, CaloSample samp): m_eta(eta),m_phi(phi), m_sampling(samp),m_id(id){};

    double x()const {return m_eta;}
    double y()const {return m_phi;}
    void setX(double x){m_eta=x;}
    void setY(double x){m_phi=x;}

    double eta()const {return m_eta;}
    double phi()const {return m_phi;}
    CaloSample sampling() const {return m_sampling;}
    // define a distance.
    struct DR2 {
      double operator()(const CellPosition &p1,const CellPosition &p2) const {
        return JetTiledMap::utils::DR2(p1.x(),p1.y(), p2.x(), p2.y() );
      }
    };

    double m_eta,m_phi;
    CaloSample m_sampling;
    Identifier m_id;
  };

  typedef std::unordered_set<Identifier> cellset_t;


  class CaloCellFastMap : public JetTiledMap::TiledEtaPhiMap<CellPosition> {
  public:
    virtual ~CaloCellFastMap() {}
    std::vector<CellPosition> cellsInDeltaR(double eta,double phi, double r) const {
      CellPosition p(eta,phi,Identifier(), (CellPosition::CaloSample) 0);
      return pointsInDr( p , r);
    }
    
    const cellset_t & cells() const {return m_allCells;}
    cellset_t & cells()  {return m_allCells;}
  protected:
    cellset_t m_allCells;
  };


}

CLASS_DEF(  jet::CaloCellFastMap , 35228686 , 1 )



class MissingCellListTool
  : public asg::AsgTool,
    virtual public IJetExecuteTool
    
//virtual public IIncidentListener // allows to detect begining of events
{ 
  ASG_TOOL_CLASS(MissingCellListTool, IJetExecuteTool)
 public: 



  // Copy constructor: 

  /// Default constructor: 
  MissingCellListTool(const std::string& name);


  /// Destructor: 
  virtual ~MissingCellListTool(); 
  
  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();


  // Interface implementation
  // virtual StatusCode prepareCellList();
  // virtual const_iterator begin() ;
  // virtual const_iterator end()   ;
  // virtual size_t size() ;


  // virtual const list_t & missingCellList() {return m_missingCells;};  
  // virtual const list_t & extendedList() {return m_badandmissingCells;};  

  virtual int execute() const;

  // /// returns all bad cells within r of (eta,phi).
  // std::vector<CellPosition> cellsInDeltaR(double eta,double phi, double r);
  
  // /// Implementation of IIncidentListener::handle
  // virtual void handle(const Incident&);


  /////////////////////////////////////////////////////////////////// 
  // PRIVATE data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  

  const CaloDetDescrManager * m_caloDDM;

  typedef std::vector<unsigned int> cellidvec_t;
  cellidvec_t m_userRemovedCells;
  cellidvec_t m_userAddedCells;


  double m_rmax;
  //bool m_needSetup;
  bool m_addBadCells;
  bool m_addCellFromTool;
  unsigned int m_larMaskBit;
  unsigned int m_tileMaskBit;

  ServiceHandle<TileCablingSvc> m_tileCabling;
  ToolHandle<ITileBadChanTool> m_tileTool;
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey{this, "BadChanKey", "LArBadChannel", "SG bad channels key"};

  std::string m_missingCellMapName;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !JETMOMENTTOOLS_BADCALOCELLLISTTOOL_H
// DoxygenDocumentation
/*! @class MissingCellListTool
 @brief Builds a missing/bad calo cell map to be used by JetBadChanCorrTool

MissingCellListTool is an IJetExecuteTool which builds and record 
a list of calo cell. This list is also a special geometrical map
which allow to efficiently retrieve all cells around a given (eta,phi) direction.

Some options govern wich cells enter the list : typically permanently disconnected
cells, "bad cells" in the sens of CaloCell::badcell(), or user given cells.


<b>Properties:</b>
<table style="text-align: left; width: 80%;" border="1">
<tr><td> <b>Name</b>             </td> <td><b>Type</b>    </td> <td><b>Default</b> </td> <td><b>Description</b></td></tr>

<tr><td>UpdateEvery              </td> <td>               </td> <td>True           </td> <td>frequency of updating the bad cell list, once or event by event</td></tr>
<tr><td>AddBadCell               </td> <td>               </td> <td>True           </td> <td>add cells of container in case badcell()=true</td></tr>


<tr><td>AddCellList      </td> <td>               </td> <td>True           </td> <td>add cells given by property</td></tr>
<tr><td>RemoveCellList   </td> <td>               </td> <td>True           </td> <td>remove cells given by property</td></tr>

<tr><td>AddCellFromTool          </td> <td>               </td> <td>False          </td> <td>add bad cells from bad channel tools</td></tr>
<tr><td>LArMaskBit               </td> <td>               </td> <td>( 1<< 0 | 1<< 2 | 1<<16 | 1<<8 | 1<<11 | 1<<14 | 1<<19 )    </td> <td>deadReadout | deadPhys | missingFEB | highNoiseHG | highNoiseMG | highNoiseLG | sporadicBurstNoise</td></tr>
<tr><td>TileMaskBit              </td> <td>               </td> <td>( 1<< 0 )    </td> <td>dead</td></tr>

<tr><td>DeltaRmax                   </td> <td>               </td> <td>1.0            </td> <td>Max cone size in which cells will be searched</td></tr>

</table>

*/
