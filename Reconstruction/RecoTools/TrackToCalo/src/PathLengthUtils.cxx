/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PathLengthUtils.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include <iostream>

//=================================
PathLengthUtils::PathLengthUtils(){
//=================================

}

//==================================
PathLengthUtils::~PathLengthUtils(){
//==================================
 
}

//=========================================================================================================================
double PathLengthUtils::get3DPathLength(const CaloCell& cell, const Amg::Vector3D& entrance, const Amg::Vector3D& exit, double drFix, double dzFix) const{
//=========================================================================================================================

    int debugLevel = 0;

    if(debugLevel>=1)  std::cout << std::endl; 

    double dmin = 10.;
    double dphimin = dmin/cell.caloDDE()->r();

// Get cell parameters 

    double dphi = cell.caloDDE()->dphi();
    if(dphi<dphimin) dphi = dphimin;
    double CellPhimin = cell.caloDDE()->phi()-dphi*0.5;
    double CellPhimax = cell.caloDDE()->phi()+dphi*0.5;

// Order according to IP to follow track convention with entry and exit planes

    int isign = 1;
    if(cell.caloDDE()->z()<0) isign = -1;
    double dr = cell.caloDDE()->dr();
//
// always use fixed values that correspond to the Calorimeter Tracking Geometry
// these are different from the CaloCell values
//
    if(dr==0) dr = drFix;
    dr = drFix;
    if(dr<dmin) dr = dmin;
    double dz = cell.caloDDE()->dz();
    if(dz==0) dz = dzFix;
    dz = dzFix;
    if(dz<dmin) dz = dmin;

    double CellZmin = cell.caloDDE()->z()-isign*dz*0.5;
    double CellZmax = cell.caloDDE()->z()+isign*dz*0.5;
    double CellRmin = cell.caloDDE()->r()-dr*0.5;
    double CellRmax = cell.caloDDE()->r()+dr*0.5;

// fix cells with zero R or Z width 

//    if(cell.caloDDE()->dr() == 0){
//        // V = dr*dphi*r*dz
//        double CellVolume = cell.caloDDE()->volume();
//        double product = cell.caloDDE()->dphi() * cell.caloDDE()->r() * cell.caloDDE()->dz();
//        double drFormula = dmin;
//        if(product!=0){
//            drFormula = CellVolume / product;
//        } 
//        if(drFormula<dmin) drFormula = dmin;
//        CellRmin = cell.caloDDE()->r() - drFormula*0.5;
//        CellRmax = cell.caloDDE()->r() + drFormula*0.5;
//        dr = drFormula;
//        if(debugLevel>=2) std::cout << "drFormula " << drFormula << " new CellRmin " << CellRmin << " new CellRmax " << CellRmax << std::endl;
//    } 

//    if(cell.caloDDE()->dz() == 0){ 
//        // V = dr*dphi*r*dz
//        double CellVolume = cell.caloDDE()->volume();
//        double product = cell.caloDDE()->dphi() * cell.caloDDE()->r() * cell.caloDDE()->dr();
//        double dzFormula = dmin;
//        if(product!=0){
//            dzFormula = CellVolume / product;
//        } 
//        if(dzFormula<dmin) dzFormula = dmin;
//        CellZmin = cell.caloDDE()->z() - isign*dzFormula*0.5;
//        CellZmax = cell.caloDDE()->z() + isign*dzFormula*0.5;
//        dz = dzFormula;
//        if(debugLevel>=2) std::cout << "dzFormula " << dzFormula << " new CellZmin " << CellZmin << " new CellZmax " << CellZmax << std::endl;
//    } 
//
// For CPU reason do a fast check on crossing 
//
// track direction 
//
    Amg::Vector3D dir = exit - entrance; 
    if(dir.mag()==0) return 0.;
    dir = dir/dir.mag();
//
// point of closest approach in x,y 
//
    double r0 = (entrance.x()-cell.caloDDE()->x())*sin(dir.phi()) - (entrance.y()-cell.caloDDE()->y())*cos(dir.phi()); 
    bool crossing = true;
    if(fabs(r0)>sqrt(cell.caloDDE()->r()*cell.caloDDE()->r()*dphi*dphi + dr*dr)) crossing = false;
//
// point of closest approach in r,z 
//
    double rz0 = (entrance.perp()-cell.caloDDE()->r())*cos(dir.theta()) - (entrance.z()-cell.caloDDE()->z())*sin(dir.theta()); 
    if(fabs(rz0)>sqrt(dr*dr+dz*dz)) crossing = false;

    if(debugLevel>=2&&crossing) {
      std::cout << " get3DPathMatch distance xy " << r0 << " distance rz " << rz0 << std::endl;
      std::cout << std::endl;  
    } 

    if(!crossing) return 0.;

// construct 8 corners of the volume
  
    Amg::Vector3D Corner0(CellRmin*cos(CellPhimin),CellRmin*sin(CellPhimin),CellZmin);
    Amg::Vector3D Corner1(CellRmin*cos(CellPhimax),CellRmin*sin(CellPhimax),CellZmin);
    Amg::Vector3D Corner2(CellRmin*cos(CellPhimin),CellRmin*sin(CellPhimin),CellZmax);
    Amg::Vector3D Corner3(CellRmin*cos(CellPhimax),CellRmin*sin(CellPhimax),CellZmax);

    Amg::Vector3D Corner4(CellRmax*cos(CellPhimin),CellRmax*sin(CellPhimin),CellZmin);
    Amg::Vector3D Corner5(CellRmax*cos(CellPhimax),CellRmax*sin(CellPhimax),CellZmin);
    Amg::Vector3D Corner6(CellRmax*cos(CellPhimin),CellRmax*sin(CellPhimin),CellZmax);
    Amg::Vector3D Corner7(CellRmax*cos(CellPhimax),CellRmax*sin(CellPhimax),CellZmax);

    if(debugLevel>=1) {
      double volume_ratio = cell.caloDDE()->dr()*cell.caloDDE()->r()*cell.caloDDE()->dphi()*cell.caloDDE()->dz() /(cell.caloDDE()->volume() + 1.) ; 
      std::cout << " sampling " << cell.caloDDE()->getSampling() << " CellPhimin " << CellPhimin << " CellPhimax " << CellPhimax << " CellZmin " << CellZmin << " CellZmax " << CellZmax << " CellRmin " << CellRmin << " CellRmax " <<  CellRmax << " cell.caloDDE()->dz() " << cell.caloDDE()->dz() << " cell.caloDDE()->dr() " << cell.caloDDE()->dr() << " cell.caloDDE()->dx() " << cell.caloDDE()->dx() << " cell.caloDDE()->dy() " << cell.caloDDE()->dy() << " volume_ratio " << volume_ratio << std::endl;
    }
    if(debugLevel>=2) {
      std::cout << " Corner0 x " << Corner0.x() << " y " << Corner0.y() << " z " << Corner0.z() << std::endl;
      std::cout << " Corner1 x " << Corner1.x() << " y " << Corner1.y() << " z " << Corner1.z() << std::endl;
      std::cout << " Corner2 x " << Corner2.x() << " y " << Corner2.y() << " z " << Corner2.z() << std::endl;
      std::cout << " Corner3 x " << Corner3.x() << " y " << Corner3.y() << " z " << Corner3.z() << std::endl;
      std::cout << " Corner4 x " << Corner4.x() << " y " << Corner4.y() << " z " << Corner4.z() << std::endl;
      std::cout << " Corner5 x " << Corner5.x() << " y " << Corner5.y() << " z " << Corner5.z() << std::endl;
      std::cout << " Corner6 x " << Corner6.x() << " y " << Corner6.y() << " z " << Corner6.z() << std::endl;
      std::cout << " Corner7 x " << Corner7.x() << " y " << Corner7.y() << " z " << Corner7.z() << std::endl;
    } 

// Entry plane vectors through Corner0

    Amg::Vector3D dir01 = Corner1 - Corner0;   // direction along Phi
    Amg::Vector3D dir02 = Corner2 - Corner0;   // direction along Z
// Second (side) plane through Corner0 has dir20 and dir40
    Amg::Vector3D dir04 = Corner4 - Corner0;   // direction along R
// Third plane through Corner0 has dir10 and dir40
   
    
// Exit plane vectors through Corner6

    Amg::Vector3D dir67 = Corner7 - Corner6;  // direction along Phi
    Amg::Vector3D dir64 = Corner4 - Corner6;  // direction along Z
// Second (side) plane through Corner dir62 and dir64
    Amg::Vector3D dir62 = Corner2 - Corner6;  // direction along R
// Third plane through Corner dir67 and dir62

// minus direction of track
    Amg::Vector3D dirT = exit - entrance; 
    dirT = -dirT;

// check positions

   if(debugLevel>=2) {
     if(entrance.perp()>Corner0.perp()) {
       std::cout << " PROBLEM radius entrance larger than cell corner " << std::endl;
     } 
     if(entrance.mag()>Corner0.mag()||fabs(exit.z())<fabs(Corner0.z())) {
       std::cout << " PROBLEM 3D or z distance to IP entrance larger than cell corner " << std::endl;
     } 
 
     if(exit.perp()<Corner6.perp()) {
       std::cout << " PROBLEM radius exit small than cell corner " << std::endl;
     } 
     if(entrance.mag()>Corner6.mag()||fabs(exit.z())<fabs(Corner6.z())) {
       std::cout << " PROBLEM 3D or Z distance to IP exit smaller than cell corner " << std::endl;
     } 
   } 

// dot products of track with plane vectors 

    if(debugLevel>=2) { 
      std::cout << " dir01 " << dir01.x() << " y " << dir01.y() << " z " << dir01.z() << " mag " << dir01.mag() << std::endl;
      std::cout << " dir02 " << dir02.x() << " y " << dir02.y() << " z " << dir02.z() << " mag " << dir02.mag() << std::endl;
      std::cout << " dir04 " << dir04.x() << " y " << dir04.y() << " z " << dir04.z() << " mag " << dir04.mag() << std::endl;
    }

    double dotp1 = fabs(dirT.dot(dir01)/dir01.mag()/dirT.mag());
    double dotp2=  fabs(dirT.dot(dir02)/dir02.mag()/dirT.mag());
    double dotp4 = fabs(dirT.dot(dir04)/dir04.mag()/dirT.mag());
//
// order planes according to dotproduct and calculate Matrices 
//        mEntry matrix 
//               column(0) vector for first plane  e.g. dir01
//               column(1) second vector for first plane e.g. dir02
//               column(2) = minus track direction 
//               type = 124 here plane made by 1 and 2  = dir01 and dir02 
//                          later also plane made by 14 = dir01 and dir04 can be tried (second try)
//                          or                       24 = dir02 and dir04 (third try)     
//
    Amg::MatrixX mEntry(3,3);
    mEntry.col(2) = dirT;
    Amg::MatrixX mExit(3,3);
    mExit.col(2) = dirT;

    int type = 0;
    double dotmax = 0.;
    if(dotp1<=dotp2&&dotp2<=dotp4) {
      type = 124;
      dotmax = dotp4;
      mEntry.col(0) = dir01;          
      mEntry.col(1) = dir02;          
      mExit.col(0) = dir67;          
      mExit.col(1) = dir64;          
    } else if (dotp1<=dotp4&&dotp4<=dotp2) {
      type = 142;
      dotmax = dotp2;
      mEntry.col(0) = dir01;          
      mEntry.col(1) = dir04;          
      mExit.col(0) = dir67;          
      mExit.col(1) = dir62;          
    } else if (dotp2<=dotp1&&dotp1<=dotp4) {
      type = 214;
      dotmax = dotp4;
      mEntry.col(0) = dir02;          
      mEntry.col(1) = dir01;          
      mExit.col(0) = dir64;          
      mExit.col(1) = dir67;          
    } else if (dotp2<=dotp4&&dotp4<=dotp1) {
      type = 241;
      dotmax = dotp1;
      mEntry.col(0) = dir02;          
      mEntry.col(1) = dir04;          
      mExit.col(0) = dir64;          
      mExit.col(1) = dir62;          
    } else if (dotp4<=dotp2&&dotp2<=dotp1) {
      type = 421;
      dotmax = dotp1;
      mEntry.col(0) = dir04;          
      mEntry.col(1) = dir02;          
      mExit.col(0) = dir62;          
      mExit.col(1) = dir64;          
    } else if (dotp4<=dotp1&&dotp1<=dotp2) {
      type = 412;
      dotmax = dotp2;
      mEntry.col(0) = dir04;          
      mEntry.col(1) = dir01;          
      mExit.col(0) = dir62;          
      mExit.col(1) = dir67;          
    }

// position of track w.r.t. all planes 

    Amg::Vector3D posEn = entrance-(Corner1+Corner2+Corner4)/2.;
    Amg::Vector3D posEx = entrance-(Corner2+Corner4+Corner7)/2.;
//
// put posEn en posEx in the middle of the plane
//
//  this makes it possible to check that the Matrix solution lies within the bounds 
//              |x|<0.5 and |y|<0.5  
//          x corresponds to the first  column(0) e.g. dir01
//          y corresponds to the second column(1) e.g. dir02
//
    int type0 = type-10*int(type/10);
    if(type0==1) { 
// last plane 1 not used 
      posEn += Corner1/2.;
      posEx += Corner7/2.;
    }
    if(type0==2) { 
// last plane 2 not used 
      posEn += Corner2/2.;
      posEx += Corner4/2.;
    }
    if(type0==4) { 
// last plane 4 not used 
      posEn += Corner4/2.;
      posEx += Corner2/2.;
    }

    if(debugLevel>=2) {
      std::cout << " not normalized direction track " << dirT.x() << " y " << dirT.y() << " z "  << dirT.z() << " eta " << dirT.eta() << " type " << type << " dotmax " << dotmax << std::endl;
      std::cout << " dotp1 " << dotp1 <<  " dotp2 " << dotp2 <<  " dotp4 " << dotp4 << std::endl;
      std::cout << " entry position " << entrance.x() << " y " << entrance.y() << " z " << entrance.z() << std::endl;
      std::cout << " mEntry Matrix " << std::endl;
      std::cout << mEntry << std::endl;
      std::cout << " exit position " << exit.x() << " y " << exit.y() << " z " << exit.z() << std::endl;
      std::cout << " mExit Matrix " << std::endl;
      std::cout << mExit << std::endl;
    }

// cross with entry plane
    int typeCheck = type0; 

    Amg::Vector3D pathEn;
    if(debugLevel>=2) std::cout << " Try crossing Entry first try for type " << type << std::endl;
    bool crossingEn = crossingMatrix(mEntry,posEn,pathEn);
// if dotmax near to 1 Matrix is singular (track is parallel to the plane)
    if(debugLevel>=2&&crossingEn) std::cout << " Succesfull crossing Entry first try for type " << type << " type 0 " << type0 << std::endl;
    if(!crossingEn) {
      int type1 = int((type-100*int(type/100))/10);
      int type2 = int(type/100);
      if(type1==1) posEn += Corner1/2.;
      if(type1==2) posEn += Corner2/2.;
      if(type1==4) posEn += Corner4/2.;
      if(type0==1) {
        mEntry.col(1) = dir01;          
        posEn -= Corner1/2.;
      }  
      if(type0==2) {
        mEntry.col(1) = dir02;          
        posEn -= Corner2/2.;
      }  
      if(type0==4) {
        mEntry.col(1) = dir04;          
        posEn -= Corner4/2.;
      }  
      if(debugLevel>=2) std::cout << " Try crossing Entry second try for type " << type << std::endl;
      crossingEn = crossingMatrix(mEntry,posEn,pathEn);
      typeCheck = type1; 
      if(debugLevel>=2&&crossingEn) std::cout << " Succesfull crossing Entry second try for type " <<type  << std::endl;
      if(!crossingEn) {
        if(type2==1) posEn += Corner1/2.;
        if(type2==2) posEn += Corner2/2.;
        if(type2==4) posEn += Corner4/2.;
        if(type1==1) {
          mEntry.col(0) = dir01;          
          posEn -= Corner1/2.;
        }  
        if(type1==2) {
          mEntry.col(0) = dir02;          
          posEn -= Corner2/2.;
        }  
        if(type1==4) {
          mEntry.col(0) = dir04;          
          posEn -= Corner4/2.;
        }  
        if(debugLevel>=2) std::cout << " Try crossing Entry third try for type " << type << std::endl;
        crossingEn = crossingMatrix(mEntry,posEn,pathEn);
        typeCheck = type2; 
        if(debugLevel>=2&&crossingEn) std::cout << " Succesfull crossing Entry third try for type " << type << std::endl;
        if(debugLevel>=2&&!crossingEn) std::cout << " No crossing Entry third try for type " << type << std::endl;
      } 
    }
  
    Amg::Vector3D posXEn = entrance;
    if(crossingEn) {
      posXEn = entrance - dirT*pathEn.z(); 
      if(typeCheck==1) {
        double dphiCheck = acos(cos(posXEn.phi())*cos((CellPhimax+CellPhimin)/2.)+sin(posXEn.phi())*sin((CellPhimax+CellPhimin)/2.));
        if(dphiCheck>fabs(CellPhimax-CellPhimin)) {
           crossingEn = false;
        } 
        if(debugLevel>=1&&!crossingEn) std::cout << " check crossing in phi " << crossingEn << " dphiCheck " << dphiCheck << " CellPhimin " << CellPhimin << " CellPhimax " << CellPhimax  << " position crossing phi " << posXEn.phi() << std::endl; 
      }
      if(typeCheck==2) {
        if(posXEn.perp()<CellRmin) crossingEn = false;
        if(posXEn.perp()>CellRmax) crossingEn = false;
        if(debugLevel>=1&&!crossingEn) std::cout << " check crossing in R " << crossingEn << " CellRmin " << CellRmin << " CellRmax " << CellRmax  << " position crossing R " << posXEn.perp() << std::endl; 
      }
      if(typeCheck==4) {
        if(isign*posXEn.z()<isign*CellZmin) crossingEn = false;
        if(isign*posXEn.z()>isign*CellZmax) crossingEn = false;
        if(debugLevel>=1&&!crossingEn) std::cout << " check crossing in Z " << crossingEn << " CellZmin " << CellZmin << " CellZmax " << CellZmax  << " position crossing Z " << posXEn.z() << std::endl; 
      }
    }

     

    if(debugLevel>=2) {
      std::cout << " Entry pathE solution " << pathEn.x() << " y " << pathEn.y() << " z " << pathEn.z()  << " crossing " << crossingEn << std::endl;
      std::cout << " crossing entrance plane with bounds " << posXEn.x() << " y " << posXEn.y() << " z " << posXEn.z() << std::endl;
      Amg::Vector3D check = entrance - dirT*pathEn.z();
      std::cout << " crossing entrance plane check " << check.x() << " y " << check.y() << " z " << check.z() << std::endl;
    }

    if(!crossingEn) return 0.;


// cross with exit plane 

    Amg::Vector3D pathEx;
   if(debugLevel>=2) std::cout << " Try crossing Exit first try for type " << type << std::endl;
    bool crossingEx = crossingMatrix(mExit,posEx,pathEx);
    if(debugLevel>=2&&crossingEx) std::cout << " Succesfull crossing Exit first try for type " << type << " type0 " << std::endl;
    typeCheck = type0; 
    if(!crossingEx) {
      int type1 = int((type-100*int(type/100))/10);
      int type2 = int(type/100);
      if(type1==1) posEx += Corner7/2.;
      if(type1==2) posEx += Corner4/2.;
      if(type1==4) posEx += Corner2/2.;
      if(type0==1) {
        mExit.col(1) = dir67;          
        posEx -= Corner7/2.;
      }  
      if(type0==2) {
        mExit.col(1) = dir64;          
        posEx -= Corner4/2.;
      }  
      if(type0==4) {
        mExit.col(1) = dir62;          
        posEx -= Corner2/2.;
      }  
      if(debugLevel>=2) std::cout << " Try crossing Exit second try for type " << type << std::endl;
      crossingEx = crossingMatrix(mExit,posEx,pathEx);
      typeCheck = type1;
      if(debugLevel>=2&&crossingEx) std::cout << " Succesfull crossing Exit second try for type " << type  << std::endl;
      if(!crossingEx) {
        if(type2==1) posEx += Corner7/2.;
        if(type2==2) posEx += Corner4/2.;
        if(type2==4) posEx += Corner2/2.;
        if(type1==1) {
          mExit.col(0) = dir67;          
          posEx -= Corner7/2.;
        }  
        if(type1==2) {
          mExit.col(0) = dir64;          
          posEx -= Corner4/2.;
        }  
        if(type1==4) {
          mExit.col(0) = dir62;          
          posEx -= Corner2/2.;
        }  
        if(debugLevel>=2) std::cout << " Try crossing Exit third try for type " << type << std::endl;
        crossingEx = crossingMatrix(mExit,posEx,pathEx);
        typeCheck = type2;
        if(debugLevel>=2&&crossingEx) std::cout << " Succesfull crossing Exit third try for type " << type << std::endl;
      } 
    }
   
    Amg::Vector3D posXEx = posXEn;
    if(crossingEx) {
      posXEx = entrance - dirT*pathEx.z(); 
      if(typeCheck==1) {
        double dphiCheck = acos(cos(posXEx.phi())*cos((CellPhimax+CellPhimin)/2.)+sin(posXEx.phi())*sin((CellPhimax+CellPhimin)/2.));
        if(dphiCheck>fabs(CellPhimax-CellPhimin)) {
           crossingEx = false;
        } 
        if(debugLevel>=1&&!crossingEx) std::cout << " check crossing in phi " << crossingEx << " dphiCheck " << dphiCheck << " CellPhimin " << CellPhimin << " CellPhimax " << CellPhimax  << " position crossing phi " << posXEx.phi() << std::endl; 
      }
      if(typeCheck==2) {
        if(posXEx.perp()<CellRmin) crossingEx = false;
        if(posXEx.perp()>CellRmax) crossingEx = false;
        if(debugLevel>=1&&!crossingEx) std::cout << " check crossing in R " << crossingEx << " CellRmin " << CellRmin << " CellRmax " << CellRmax  << " position crossing R " << posXEx.perp() << std::endl; 
      }
      if(typeCheck==4) {
        if(isign*posXEx.z()<isign*CellZmin) crossingEx = false;
        if(isign*posXEx.z()>isign*CellZmax) crossingEx = false;
        if(debugLevel>=1&&!crossingEx) std::cout << " check crossing in Z " << crossingEx << " CellZmin " << CellZmin << " CellZmax " << CellZmax  << " position crossing Z " << posXEx.z() << std::endl; 
      }
    }

    if(debugLevel>=2) {
      std::cout << " Exit pathEx solution " << pathEx.x() << " y " <<  pathEx.y() << " z " <<  pathEx.z()  << " crossing " << crossingEx << std::endl;
      std::cout << " crossing exit plane with bounds " << posXEx.x() << " y " << posXEx.y() << " z " << posXEx.z() << std::endl;
      Amg::Vector3D check = entrance - dirT*pathEx.z();
      std::cout << " crossing exit plane check " << check.x() << " y " << check.y() << " z " << check.z() << std::endl;
    }

    double pathT = (entrance-exit).mag();
    double path = 0.;
    if(crossingEn&&crossingEx) {
      path = (posXEx-posXEn).mag();
      if(debugLevel>=1) {
        std::cout << std::endl;
        std::cout << " pathT " << pathT  << " path " << path << std::endl;
        std::cout << " entrance position                   " << entrance.x() << " y " << entrance.y() << " z " << entrance.z() << std::endl;
        std::cout << " crossing entrance plane with bounds " << posXEn.x() << " y " << posXEn.y() << " z " << posXEn.z() << std::endl;
        std::cout << " exit position                       " << exit.x() << " y " << exit.y() << " z " << exit.z() << std::endl;
        std::cout << " crossing exit plane with bounds     " << posXEx.x() << " y " << posXEx.y() << " z " << posXEx.z() << std::endl;
      } 
    } 
    if(debugLevel>=2) { 
      std::cout << " pathT " << pathT  << " path " << path << std::endl;
    }

    return path; 
 
}
bool PathLengthUtils::crossingMatrix(const Amg::MatrixX& Matrix ,const Amg::Vector3D& entry, Amg::Vector3D& path) const {

//    std::cout << " Matrix determinant " << Matrix.determinant() << std::endl;
//    std::cout << " Matrix " << std::endl; 
//    std::cout << Matrix << std::endl;

    if(Matrix.determinant()==0) {
 //      std::cout << " PathLengthUtils::crossingMatrix Matrix determinant ZERO " << std::endl; 
       return false;
    }
    Amg::MatrixX Minv = Matrix.inverse();

//    std::cout << " inverse Matrix " << std::endl;
//    std::cout << Minv << std::endl;

    path = Minv*entry;

    bool crossing = false;
//
// Inside middle of the plane ?   range -0.5 to 0.5
//
    if(fabs(path.x())<0.5&&fabs(path.y())<0.5) crossing = true;

    return crossing;
}

double PathLengthUtils::getPathLengthInTile(const CaloCell& cell, const Amg::Vector3D& entrance, const Amg::Vector3D& exit) const {
//=========================================================================================================================
    // OBTAIN LAYER INDICES FOR LINEAR INTERPOLATION
    unsigned int SampleID = cell.caloDDE()->getSampling();

    double CellZB[9]  = {141.495, 424.49, 707.485, 999.605, 1300.855, 1615.8, 1949., 2300.46, 2651.52};
    double CellDZB[9] = {282.99, 283., 282.99, 301.25, 301.25, 328.64, 337.76, 365.16, 336.96};
    double CellZC[9]  = {159.755, 483.83, 812.465, 1150.23, 1497.125, 1857.71, 2241.12, 2628.695,0};
    double CellDZC[9] = {319.51, 328.64, 328.63, 346.9, 346.89, 374.28, 392.54, 382.61,0};

    // SPECIAL CASE: BC9
    bool isBC9 = false;
    if(SampleID==13 && ( fabs(cell.caloDDE()->eta()-0.85)<0.001 || fabs(cell.caloDDE()->eta()+0.85)<0.001 ) ) isBC9 = true;
    
    // OBTAIN TRACK AND CELL PARAMETERS
    double pathl = 0.;

    double Layer1X(exit.x()),Layer1Y(exit.y()),Layer1Z(exit.z());
    double Layer2X(entrance.x()),Layer2Y(entrance.y()),Layer2Z(entrance.z());

    double CellPhimin = cell.caloDDE()->phi()-cell.caloDDE()->dphi()*0.5;
    double CellPhimax = cell.caloDDE()->phi()+cell.caloDDE()->dphi()*0.5;
    double CellZmin = cell.caloDDE()->z()-cell.caloDDE()->dz()*0.5;
    double CellZmax = cell.caloDDE()->z()+cell.caloDDE()->dz()*0.5;
    double CellRmin = cell.caloDDE()->r()-cell.caloDDE()->dr()*0.5;
    double CellRmax = cell.caloDDE()->r()+cell.caloDDE()->dr()*0.5;

    // FIX FOR CELLS WITH ZERO WIDTH IN R
    if(cell.caloDDE()->dr() == 0){
        // V = dr*dphi*r*dz
        double CellVolume = cell.caloDDE()->volume();
        double product = cell.caloDDE()->dphi() * cell.caloDDE()->r() * cell.caloDDE()->dz();
        double drFormula = 0;
        if(product!=0){
            drFormula = CellVolume / product;
        } // IF 
        CellRmin = cell.caloDDE()->r() - drFormula*0.5;
        CellRmax = cell.caloDDE()->r() + drFormula*0.5;
    } // IF

    //TileCellDim *cell_dim = m_tileDDM->get_cell_dim(cell->caloDDE()->identify());

    double CellXimp[2], CellYimp[2], CellZimp[2];
    double X(0), Y(0), Z(0), Phi(0);
    double DeltaPhi;
    
    // COMPUTE PATH
    bool compute = true;
    int lBC(0);
    // LOOP IS USUALLY RUN ONCE, EXCEPT FOR LADDER SHAPED TILECAL CELLS
    while(compute){
        if(lBC==1 && isBC9) break;
        int Np = 0;
        if(sqrt((Layer1X-Layer2X)*(Layer1X-Layer2X)+(Layer1Y-Layer2Y)*(Layer1Y-Layer2Y)) < 3818.5){
            if(SampleID == 13 && lBC == 0){
                int cpos = fabs(cell.caloDDE()->eta())/0.1;

                CellRmin = 2600.; //cell_dim->getRMin(0);
                CellRmax = 2990.; //cell_dim->getRMax(2);
                CellZmin = CellZB[cpos] - 0.5*CellDZB[cpos]; //cell_dim->getZMin(0);
                CellZmax = CellZB[cpos] + 0.5*CellDZB[cpos]; //cell_dim->getZMax(0);
            } // ELSE IF
            else if(SampleID == 13 && lBC == 1){
                int cpos = fabs(cell.caloDDE()->eta())/0.1;

                CellRmin = 2990.; //cell_dim->getRMin(3);
                CellRmax = 3440.; //cell_dim->getRMax(5);
                CellZmin = CellZC[cpos] - 0.5*CellDZC[cpos]; //cell_dim->getZMin(3);
                CellZmax = CellZC[cpos] + 0.5*CellDZC[cpos]; //cell_dim->getZMax(3);
            } // ELSE IF
            
            // CALCULATE GRADIENTS
            double Sxy = (Layer2X-Layer1X)/(Layer2Y-Layer1Y);
            double Sxz = (Layer2X-Layer1X)/(Layer2Z-Layer1Z);
            double Syz = (Layer2Y-Layer1Y)/(Layer2Z-Layer1Z);

            // CALCULATE POINTS OF INTERSECTION
            // INTERSECTIONS R PLANES
            double Radius(CellRmin);
            
            double x0int(exit.x()), x1int(entrance.x()),
            y0int(exit.y()), y1int(entrance.y()),
            z0int(exit.z()), z1int(entrance.z());
            double S((y1int-y0int)/(x1int-x0int));
            double a(1+S*S), b(2*S*y0int-2*S*S*x0int), c(y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int);
            double x1((-b+sqrt(b*b-4*a*c))/(2*a)), x2((-b-sqrt(b*b-4*a*c))/(2*a));
            double y1(y0int+S*(x1-x0int)), y2(y0int+S*(x2-x0int));
            double S1 = ((z1int-z0int)/(x1int-x0int));
            double z1(z0int+S1*(x1-x0int)), z2(z0int+S1*(x2-x0int));
            
            X = x1;
            Y = y1;
            Z = z1;
            
            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X=x2;
                Y=y2;
                Z=z2;
            } // IF

            if(CellRmin != CellRmax){
            Phi = acos(X/sqrt(X*X+Y*Y));
            if(Y <= 0) Phi = -Phi;
            //R = CellRmin;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X;
                CellYimp[Np]=Y;
                CellZimp[Np]=Z;
                Np=Np+1;
               
            } // IF
            
            Radius = CellRmax;
            
            c  = y0int*y0int-Radius*Radius+S*S*x0int*x0int-2*y0int*S*x0int;
            x1 = ((-b+sqrt(b*b-4*a*c))/(2*a));
            x2 = ((-b-sqrt(b*b-4*a*c))/(2*a));
            y1 = (y0int+S*(x1-x0int));
            y2 = (y0int+S*(x2-x0int));
            z1 = (z0int+S1*(x1-x0int));
            z2 = (z0int+S1*(x2-x0int));
            S1 = ((z1int-z0int)/(x1int-x0int));

            X = x1;
            Y = y1;
            Z = z1;

            if( ((x1-x0int)*(x1-x0int)+(y1-y0int)*(y1-y0int)+(z1-z0int)*(z1-z0int)) >
               ((x2-x0int)*(x2-x0int)+(y2-y0int)*(y2-y0int)+(z2-z0int)*(z2-z0int)) ){
                X = x2;
                Y = y2;
                Z = z2;
            } // IF

            Phi=std::acos(X/sqrt(X*X+Y*Y));
            if (Y <= 0) Phi=-Phi;
            //R=CellRmax;
            
            if(Z>=CellZmin && Z<=CellZmax && Phi>=CellPhimin && Phi<=CellPhimax){
                CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                Np=Np+1;
            } // IF
            } // IF
            
            // INTERSECTIONS Z PLANES
            if(CellZmin != CellZmax){
            if(Np < 2){
                Z = CellZmin;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                const double R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF

            if(Np < 2){
                Z = CellZmax;
                X = Layer1X+Sxz*(Z-Layer1Z);
                Y = Layer1Y+Syz*(Z-Layer1Z);
                const double R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                if(R>=CellRmin && R<=CellRmax && Phi>=CellPhimin && Phi<=CellPhimax){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            } // IF

            // INTERSECTIONS PHI PLANES
            if(CellPhimin != CellPhimax){
            if(Np < 2){
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimin));
                Y = X*std::tan(CellPhimin);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                const double R = sqrt(X*X+Y*Y);
                Phi = std::acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimin);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimin);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;                  
                } // IF
            } // IF

            if(Np < 2){
                X = (Layer1X-Sxy*Layer1Y)/(1-Sxy*tan(CellPhimax));
                Y = X*std::tan(CellPhimax);
                Z = Layer1Z+(1/Sxz)*(X-Layer1X);
                const double R = sqrt(X*X+Y*Y);
                Phi=acos(X/R);
                if(Y <= 0) Phi=-Phi;
                DeltaPhi=fabs(Phi-CellPhimax);
                if(DeltaPhi > 3.141593) DeltaPhi=fabs(Phi+CellPhimax);
                if(R>=CellRmin && R<=CellRmax && Z>=CellZmin && Z<=CellZmax && DeltaPhi<0.0001){
                    CellXimp[Np]=X; CellYimp[Np]=Y; CellZimp[Np]=Z;
                    Np=Np+1;
                } // IF
            } // IF
            } // IF

            // CALCULATE PATH IF TWO INTERSECTIONS WERE FOUND
            if(Np == 2){                
                pathl += sqrt( (CellXimp[0]-CellXimp[1])*(CellXimp[0]-CellXimp[1]) +
                               (CellYimp[0]-CellYimp[1])*(CellYimp[0]-CellYimp[1]) +
                               (CellZimp[0]-CellZimp[1])*(CellZimp[0]-CellZimp[1]) );
            } // IF
        } // IF
        if(SampleID == 13 && lBC == 0) ++lBC;
        else compute = false;
    } // WHILE (FOR LBBC LAYER)

    return pathl;
} // PathLengthUtils::getPathLengthInTile



CaloSampling::CaloSample PathLengthUtils::tileEntrance(CaloSampling::CaloSample sample) const {
    if(sample==CaloSampling::TileBar0||sample==CaloSampling::TileBar1||sample==CaloSampling::TileBar2||sample==CaloSampling::TileGap2) return CaloSampling::TileBar0;
    if(sample==CaloSampling::TileGap1)                                                       return CaloSampling::TileBar1;
    if(sample==CaloSampling::TileGap3||sample==CaloSampling::TileExt0||sample==CaloSampling::TileExt1||sample==CaloSampling::TileExt2) return CaloSampling::TileExt0;
    return CaloSampling::Unknown;
    //return sample;
} // PathLengthUtils::entrance 

CaloSampling::CaloSample PathLengthUtils::tileExit(CaloSampling::CaloSample sample) const {
    if(sample==CaloSampling::TileBar0||sample==CaloSampling::TileBar1||sample==CaloSampling::TileBar2||sample==CaloSampling::TileGap1) return CaloSampling::TileBar2;
    if(sample==CaloSampling::TileGap2)                                                       return CaloSampling::TileBar1;
    if(sample==CaloSampling::TileGap3)                                                       return CaloSampling::TileExt1;
    if(sample==CaloSampling::TileExt0||sample==CaloSampling::TileExt1||sample==CaloSampling::TileExt2)                   return CaloSampling::TileExt2;
    return CaloSampling::Unknown;
    //return sample;
} // PathLengthUtils::exit 

/** Return the length(mm) of the path crossed inside the cell, given the parameters for the extrapolation at entrance and exit of the layer **/
double PathLengthUtils::pathInsideCell(const CaloCell& cell, const CaloExtensionHelpers::EntryExitLayerMap&  entryExitLayerMap) const {
  
  CaloSampling::CaloSample sample = cell.caloDDE()->getSampling();

  //------------------------------------------------------------------------------------------
  //special treatment for tile calo cells
  CaloSampling::CaloSample tileEntranceID = tileEntrance(sample);
  CaloSampling::CaloSample tileExitID     = tileExit(sample);

  auto tileEntrancePair = entryExitLayerMap.find(tileEntranceID);
  auto tileExitPair     = entryExitLayerMap.find(tileExitID);
  if( tileEntrancePair != entryExitLayerMap.end() && tileExitPair != entryExitLayerMap.end() ){
    return getPathLengthInTile( cell, tileEntrancePair->second.first, tileExitPair->second.second);
  }
  //------------------------------------------------------------------------------------------

  auto entryExitPair = entryExitLayerMap.find(sample);
  if( entryExitPair == entryExitLayerMap.end() ) return -1.;
  
  const Amg::Vector3D& entry = entryExitPair->second.first;
  const Amg::Vector3D& exit  = entryExitPair->second.second;

  if (!crossedPhi(cell, entry.phi(), exit.phi())) return -1.;
  double pathCrossed = -1;
  if (cell.caloDDE()->getSubCalo() != CaloCell_ID::TILE){
    pathCrossed = getPathLengthInEta(cell, entry.eta(), exit.eta());
  }
  else{
    pathCrossed = getPathLengthInZ(cell, entry.z(), exit.z());
  }
  if (pathCrossed <= 0) return -1.;
  return pathCrossed * (exit-entry).mag();
}
