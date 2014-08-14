package atlantis.data;


import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.*;
import java.util.*;


/**
 * Simulated vertices
 */
public class ASVxData extends A3DPointData {

  // protected final String PARAMETER_GROUP = getParameterGroup();


  public String getParameterGroup()
  {
      return "SVx";
  }


  public String getName()
  {
      return "SVx";
  }


  public String getNameScreenName()
  {
    return "SimVertex";

  }


  public String getHitInfo(int index)
  {
	  int simpleOutput = AGlobals.instance().getSimpleOutput();
	  if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
      AMath.RHO+" = "+String.format("%.3f",rho[index])+"\n "+
      "z = "+String.format("%.3f",z[index])+" cm\n"+
      AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
	  
      return getNameScreenName() + " (id: " + id[index] + " index: " + index + ")" +
          "\n x = " + String.format("%.5f",x[index]) + " cm" +
          "\n y = " + String.format("%.5f",y[index]) + " cm" +
          "\n z = " + String.format("%.5f",z[index]) + " cm" +
          "\n " + AMath.RHO + " = " + String.format("%.5f",rho[index]) + " cm" +
          "\n " + AMath.PHI + " = " +
          String.format("%.5f",Math.toDegrees(phi[index])) + AMath.DEGREES +
          "  (" + String.format("%.5f",phi[index]) + " rad)";

  } // getHitInfo()  --------------------------------------------------------



  ASVxData(AHashMap p, AEvent e) {
    super(p,e);
  }

  // SVx do not come correctly in input xml file
  // must create them from STr data in xml file
  public static AHashMap createSVx(AHashMap p) {
    float[] rhoVertex=p.getFloatArray("rhoVertex");
    float[] phiVertex=p.getFloatArray("phiVertex");
    float[] zVertex=p.getFloatArray("zVertex");
    int[] sv=ASVxData.assignVertexNumbers(phiVertex, rhoVertex, zVertex);
    int numVertex=0;

    for(int i=0; i<sv.length; i++)
      if(sv[i]>=numVertex) numVertex=sv[i]+1;

    float[] rho=new float[numVertex];
    float[] phi=new float[numVertex];
    float[] z=new float[numVertex];

    for(int i=0; i<sv.length; i++) {
      rho[sv[i]]=(float)(Math.sqrt(rhoVertex[i]*rhoVertex[i]-zVertex[i]*zVertex[i]));
      phi[sv[i]]=phiVertex[i];
      z[sv[i]]=zVertex[i];
    }

    AHashMap newP=new AHashMap(4);

    newP.put("numData", new Integer(numVertex));
    newP.put("rho", rho);
    newP.put("phi", phi);
    newP.put("z", z);
    return newP;
  }

  public static int[] assignVertexNumbers(float[] phi, float[] rho, float[] z) {
    int[] sv=new int[phi.length];
    Vertex[] key=new Vertex[phi.length];

    HashMap vertices=new HashMap();

// this is ugly, related to the fact that id[i]=i+1 by default;
    int num=1;
    for(int i=0; i<sv.length; i++) {
      key[i]=new Vertex(rho[i], phi[i], z[i]);
      Object value=vertices.get(key[i]);
      if(value==null) {
        sv[i]=num;
        vertices.put(key[i], new Integer(num++));
      } else {
        sv[i]=((Integer)value).intValue();
      }
    }
    return sv;
  }

  protected int internalColor() {
    colorByConstant();
    return 1;
  }

  protected void applyCuts() {
    cutIndex();
    cutPhi(phi);
    cutEta(rho, z);
    cut("CutsInDet", "SVx", " Vertex", id);
  }


  public double[] getVertex(int index) {

    double[] vertex=new double[3];

    if(index>-1&&index<numData) {
      vertex[0]=x[index];
      vertex[1]=y[index];
      vertex[2]=z[index];
    }
    return vertex;
  }

}


class Vertex {
  float x;
  float y;
  float z;
  int hashcode;

  Vertex(float x, float y, float z) {
    this.x=x;
    this.y=y;
    this.z=z;
    hashcode=Float.floatToIntBits(z);
  }

  @Override
  public int hashCode() {
    return hashcode;
  }

  @Override
  public boolean equals(Object a) {
    if(!(a instanceof Vertex)) return false;
    Vertex b=(Vertex)a;
    return x==b.x&&y==b.y&&z==b.z;
  }
}

