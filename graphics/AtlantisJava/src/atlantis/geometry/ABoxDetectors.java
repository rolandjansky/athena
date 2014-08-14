package atlantis.geometry;

import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import atlantis.utils.AMath;
import java.util.*;


/**
 * ABoxDetector's contains all instances of ABoxDetector.
 * In different projections not all Boxes need to be drawn
 * as they may exactly overlap. Filling of these areas is
 * slow, so only the Boxes needed are drawn in each projection.
 * (That is why boxes are group into ABoxDetectors)
 * This is achieved by making a drawlist before drawing.
 */
public final class ABoxDetectors extends ADetectors {

  private int numRZTypes;
  private int[] typeRZ;
  private int numYXTypes;
  private int[] typeYX;

  private ABoxDetector[] box;

  public ABoxDetectors(List detectors) {
   super(filterBoxDetectors(detectors));
   box= new ABoxDetector[det.length];
   for(int i=0;i<box.length;++i)
     box[i]=(ABoxDetector)det[i];

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

  private static ADetector[] filterBoxDetectors(List detectors) {

    List tempDetectors=new ArrayList(detectors.size());
    Iterator it=detectors.iterator();

    while(it.hasNext()) {
      Object o=it.next();
      if(o instanceof ABoxDetector || o instanceof ATBxDetector) tempDetectors.add(o);
    }
    return (ADetector[])tempDetectors.toArray(new ADetector[tempDetectors.size()]);
  }

  private void makeDrawListYX() {
    int mode=parameterStore.get("YX", "Mode").getI();

    if(mode==0) {
      numDraw=numYXTypes;
      for(int i=0; i<numData; ++i)
        listdl[typeYX[i]]=i;
    } else
      numDraw=0;
  }

  private void makeDrawListXZ() {
    AParameter p=parameterStore.get("XZ", "Phi");

    // display muon data when phi is in the middle of a sector
    if(p.getD() % 22.5 < 1e-2) {
      int sect=(int)Math.round((p.getD() % 360.) / 22.5);
      int num=0;
      for(int i=0; i<numData; i++) {
        int s=box[i].sector;
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
      double phiDiff=Math.abs(box[i].phi-phiMid);

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
    int mode=parameterStore.get("FZ", "Mode").getI();

    if(mode==0)
      numDraw=0;
    else {
      int num=0;

      for(int i=0; i<numData; ++i) {
        char type=det[i].getName().charAt(0);
        char station=det[i].getName().charAt(5);
        char io=det[i].getName().charAt(det[i].getName().length()-1);

        if((mode==1&&station=='M'&&io=='I'&&type=='R')||(mode==2&&station=='M'&&io=='O'&&type=='R')
            ||(mode==3&&station=='O'&&type=='R')||(mode==4&&(station=='I'||station=='E')&&type=='M')
            ||(mode==5&&station=='M'&&type=='M')||(mode==6&&station=='O'&&type=='M'))
          listdl[num++]=i;
        numDraw=num;
      }
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

