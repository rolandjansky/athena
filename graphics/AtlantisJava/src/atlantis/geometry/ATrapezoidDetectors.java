package atlantis.geometry;

// not quite correct for CSC (alpha!=0.) in YX and FR

import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import atlantis.utils.AMath;
import java.util.*;

/**
 * see comments in ABoxDetectors
 */

public final class ATrapezoidDetectors extends ADetectors {

  private int numRZTypes;
  private int[] typeRZ;
  private int numYXTypes;
  private int[] typeYX;

  private ATrapezoidDetector[] trapezoid;

  public ATrapezoidDetectors(List detectors) {
   super(filterTrapezoidDetectors(detectors));
   trapezoid= new ATrapezoidDetector[det.length];
   for(int i=0;i<trapezoid.length;++i)
     trapezoid[i]=(ATrapezoidDetector)det[i];


    typeYX=new int[numData];
    for(int i=0; i<numData; ++i)
      typeYX[i]=-1;

    numYXTypes=0;
    for(int i=0; i<numData; ++i)
      if(typeYX[i]==-1) {
        typeYX[i]=numYXTypes;
        for(int j=i+1; j<numData; ++j)
          if(typeYX[j]==-1&&det[i].equalsYX(det[j]))
            typeYX[j]=numYXTypes;
        numYXTypes++;
      }

    typeRZ=new int[numData];
    for(int i=0; i<numData; ++i)
      typeRZ[i]=-1;

    numRZTypes=0;
    for(int i=0; i<numData; ++i)
      if(typeRZ[i]==-1) {
        typeRZ[i]=numRZTypes;
        for(int j=i+1; j<numData; ++j)
          if(typeRZ[j]==-1&&det[i].equalsRZ(det[j]))
            typeRZ[j]=numRZTypes;
        numRZTypes++;
      }
  }

  private static ADetector[] filterTrapezoidDetectors(List detectors) {

    List tempDetectors=new ArrayList(detectors.size());
    Iterator it=detectors.iterator();

    while(it.hasNext()) {
      Object o=it.next();
      if(o instanceof ATrapezoidDetector) tempDetectors.add(o);
    }
    return (ADetector[])tempDetectors.toArray(new ADetector[tempDetectors.size()]);
  }

  private void makeDrawListYX() {
    int mode=parameterStore.get("YX", "Mode").getI();

    if(mode==0)
      numDraw=0;
    else {
      int num=0;
      for(int i=0; i<numData; ++i) {
        double z=trapezoid[i].zMin;
        String name=det[i].getName().substring(4, 6);
        if(z>0.)
          if((mode==1&&name.equals("T4"))||(mode==2&&name.equals("T1"))
              ||(mode==3&&name.equals("T2"))||(mode==4&&name.equals("T3"))
              ||(mode==5&&(name.equals("EI")||name.equals("CS")))||(mode==6&&name.equals("EE"))
              ||(mode==7&&name.equals("EM"))||(mode==8&&name.equals("EO")))
            listdl[num++]=i;
        numDraw=num;
      }
    }
  }

  private void makeDrawListXZ() {
    AParameter p=parameterStore.get("XZ", "Phi");

    // display muon data when phi is in the middle of a sector
    if(p.getD() % 22.5 < 1e-2) {
      int sect=(int)Math.round((p.getD() % 360.) / 22.5);
      int num=0;
      for(int i=0; i<numData; i++) {
        int s=trapezoid[i].sector;

        if(s==sect||s==sect-8||s==sect+8) {
          listdl[num++]=i;
        }
      }
      numDraw=num;
    } else {
      numDraw=0;
    }
  }

  private void makeDrawListRZ() {
    // first make the draw list
    // for each type find out if it has both +ve and negative sign
    int[] pos=new int[typeRZ.length];
    int[] neg=new int[typeRZ.length];
    double phiMid=Math.toRadians(parameterStore.get("RZ", "Phi").getD());

    for(int i=0; i<numRZTypes; ++i)
      neg[i]=pos[i]=-1;

    for(int i=0; i<numData; ++i) {
      double phiDiff=Math.abs(trapezoid[i].phi-phiMid);

      if(phiDiff<Math.PI/2.||phiDiff>3*Math.PI/2.)
        pos[typeRZ[i]]=i;
      else
        neg[typeRZ[i]]=i;
    }

    int num=0;
    for(int i=0; i<numRZTypes; ++i) {
      if(pos[i]!=-1)
        listdl[num++]=pos[i];
      if(neg[i]!=-1)
        listdl[num++]=neg[i];
    }
    numDraw=num;
  }

  private void makeDrawListFZ() {
    if(parameterStore.get("FZ", "Mode").getI()==0) {
      constructDefaultDrawList();
    } else {
      numDraw=0;
    }
  }

   protected void makeDrawList(String projection) {
     if (projection.equals("YX") || projection.equals("FR") )
       makeDrawListYX();
     else if (projection.equals("FZ"))
       makeDrawListFZ();
     else if (projection.equals("RZ"))
       makeDrawListRZ();
     else if (projection.equals("XZ"))
       makeDrawListXZ();
     else
       numDraw=0;
  }

}

